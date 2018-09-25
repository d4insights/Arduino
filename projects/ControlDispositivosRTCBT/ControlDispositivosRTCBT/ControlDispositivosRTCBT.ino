
/*
     Programa: Controlador de Dispositivos RTC BlueTooth 
                             
               0.  Opera con el Clock de tiempo real RTC
               1.  Si existe una conexion serial activa se puede sincronizar la fecha y la hora con la PC
               2.  Permite setear múltiple alarmas semanales o diarias
               3.  Permite setear timers de ejecución automática o de ejecución de única vez
               4.  Muestra la hora en un display LCD de 16 X 2
               5.  Recibe ordenes vía BlueTooth desde un terminal de ANDROIND 
                         show clock
                         show alarm
                         show timer
                         set clock on
                         set clock usb
                         set alarm on
                         set timer on
                         
                         backlight on
                         backlight off
                         buzzer
                         reset
                         
               6.  Setea la FECHA, HORA y ALARMAS vía BlueTooth 
               
        Ejemplos de seteos de alarmas y timers 
          
          Alarm.alarmRepeat(8,30,0, MorningAlarm);              // 8:30am every day
          Alarm.alarmRepeat(17,45,0,EveningAlarm);              // 5:45pm every day 
          Alarm.alarmRepeat(dowSaturday,8,30,30,WeeklyAlarm);   // 8:30:30 every Saturday 
          Alarm.timerOnce(10, OnceOnly);                        // called once after 10 seconds 
               
     Autor: Diego Fernández Sanz
     Fecha: 28/02/2015    
     
     LCD PINUP
         Para utilizar con el LCD 16 X 2 de WINSTAR directo 
         - LCD RS pin to digital         PIN12
         - LCD Enable pin to digital     PIN11
         - LCD D4 pin to digital         PIN5
         - LCD D5 pin to digital         PIN4
         - LCD D6 pin to digital         PIN3
         - LCD D7 pin to digital         PIN2 
         - LCD R/W pin to ground         GND
         - 10K resistor:
         - ends to +5V and ground
         - wiper to LCD VO pin (pin 3) 
   
 */

#define RESTART asm("jmp 0x0000")

#include <Wire.h>               // Comunicación I2C
#include <RTClib.h>             // Reloj de Tiempo Real
#include <TimeAlarms.h>         
#include <Time.h>
#include <LiquidCrystal.h>      // Manejo del LCD
#include <SoftwareSerial.h>     // Comunicación serial por pines alternativos para el BlueThoot

// RTC Clock
RTC_DS1307 RTC;                                        // Crea el objeto Clock de Tiempo Real
//const char* diasSem[] = {"Dom", "Lun", "Mar", "Mie", "Jue", "Vie", "Sab"};
//const char* meses[] = {"Enero","Febrero","Marzo","Abril","Mayo","Junio","Julio","Agosto","Septiembre","Octubre","Noviembre","Diciembre"};


// Alarm
int pinAccionadorA = 8;                      // Define el pin donde voy a poner el dispositivo que voy a accionar cuando suene la alarma
int pinBackLight   = 13;                     // Define el pin donde voy a poner el backlight del LCD


// Define e inicializa los pines de la interface de LCD
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);


// BlueTooth pins RXD,TDX 
SoftwareSerial BT(9,10);              //09 RX, 10 TX.
char cadena[255];                     //Creamos un array de caracteres de 256 posiciones para recibir ordenes desde el BT
int i=0;                              //Tamaño actual del array de ordenes recibidas por BT

DateTime tm;
int ALARM[] = {0,0,0,0};              // Declaración de los elementos del ALARM (Hora, minuto, Segundo, DOW) ... Se disparan cada día a una hora específca
int TIMER[] = {0,0,0};                // Declaración de los elementos del TIMER (Hora, minuto, Segundo) ... Se disparan eventos a intervalos de n tiempo
int CLOCK[] = {0,0,0,1,1,2000};       // Declaración de los elementos del TIMER (Hora, minuto, Segundo, Día, Mes, Año)

void setup()
{
  // Seteo la cantidad de filas y columnas que va a tener el LCD 
  lcd.begin(16, 2);
  
  // Inicializo la comunicación con el RTC Clock
  Wire.begin();                                           // Inicia el puerto I2C
  RTC.begin();                                            // Inicia la comunicación con el RTC

  // Si está el USB conectado sincroniza el RTC con la PC 
  //RTC.adjust(DateTime(__DATE__, __TIME__) + 25);               // Establece la fecha y hora (Comentar una vez establecida la hora)
 
  // Leo el RTC para cargar el objeto reloj interno desde el que después voy a comparar las alarmas
  DateTime now = RTC.now();                                                           // Obtiene la fecha y hora del RTC
  setTime(now.hour(),now.minute(),now.second(),now.day(),now.month(),now.year());     // Setea el clock interno a partir del RTC            
  
  tm = now;

  
  // Inicializo el puerto de comunicación con el Monitor Serial  
  Serial.begin(9600);

  // Inicializo el puerto de comunicación con el BlueTooth
  BT.begin(9600);
  
  
  // Inicializo como salida el pin donde voy a conectar los accionadores
  pinMode(pinAccionadorA, OUTPUT);
  pinMode(pinBackLight, OUTPUT);
   
  
  
}

void  loop(){  
   

  
  Alarm.delay(100);                                       // Muestreo de las alarmas y timers
  
  //digitalClockDisplay();                                  // Muestra la hora en el Monitor Serial
  digitalLCDClockDisplay();                               // Muestra el Fecha y la Hora en el LCD
  
  
  // Rutina de interpretación de los datos recibidos por BlueTooth
  ReadBlueTooth(tm);
  
 
  

}



void ReadBlueTooth(DateTime tm)
{
   
  // Rutina de lectura del puerto de BT por donde se van a recibir las ordenes a ejecutar
  if(BT.available())
  {
    char dato=BT.read();                           //Guarda los datos carácter a carácter en la variable "dato"
    cadena[i++]=dato;                              //Vamos colocando cada carácter recibido en el array "cadena"
    if(dato=='\n')                                 //Cuando reciba un caracter de nueva línea (al pulsar enter en la app) se considera completa la cadena de la orden
    {
      Serial.print(cadena);                        //Visualizamos el comando recibido en el Monitor Serial
 
      
      // Analiza la orden recibida por el BT y dispara los procedimientos que correspondan
      //
      //
      // BackLigth ON
      if(strstr(cadena,"backlight on")!=0)
      {
           backLightON();
           Serial.println("Procedimiento [backlight on] disparado");
           BT.write("OK");
      }

      // BackLigth OFF
      if(strstr(cadena,"backlight off")!=0)
      {
           backLightOFF();
           Serial.println("Procedimiento [backlight off] disparado");
           BT.write("OK");
      }      
            
      // Ver Fecha y Hora
      if(strstr(cadena,"show clock")!=0)
      {
           digitalLCDClockDisplay();
           Serial.println("Procedimiento [show clock] disparado");
           BT.write("OK");
      }

      // Setear manual Reloj
      //  
      if(strstr(cadena,"set clock on")!=0)
      {
          
          // Camptura de los datos desde BlueTooth
             LCDClear();        // Limpio el LCD para mostrar los mensajes específicos          
                    
            // Muestro en el LCD la indicación que estoy en modo SetUp de Alarma    
            backLightON();
            lcd.setCursor(0, 0);
            lcd.print("Setup CLOCK mode");
            
  
           // Lectura de los parametros (ver combinaciones posibles de alarma en la cabecera de esta documento)
           //

           // Horas
           //BT.print("Hora: ");
           while(BT.available()==0){}
            CLOCK[0] = BT.parseInt();
           lcd.setCursor(0, 1);
           lcd.print("Hora: ");
           lcd.print(CLOCK[0]);
           lcd.print("         ");

           // Mínutos
           //BT.print(" Minutos: ");
           while(BT.available()==0){}
             CLOCK[1] = BT.parseInt();
           lcd.setCursor(0, 1);
           lcd.print("Min: ");
           lcd.print(CLOCK[1]);
           lcd.print("         ");
       
           // Segundos
           //BT.print(" Segundos: ");
           while(BT.available()==0){}
             CLOCK[2] = BT.parseInt();
           lcd.setCursor(0, 1);
           lcd.print("Seg: ");
           lcd.print(CLOCK[2]);
           lcd.print("         ");
        
           // Día
           //BT.print("Dia: ");
           while(BT.available()==0){}
            CLOCK[3] = BT.parseInt();
           lcd.setCursor(0, 1);
           lcd.print("Dia: ");
           lcd.print(CLOCK[3]);
           lcd.print("         ");

           // Mes
           //BT.print("Mes: ");
           while(BT.available()==0){}
            CLOCK[4] = BT.parseInt();
           lcd.setCursor(0, 1);
           lcd.print("Mes: ");
           lcd.print(CLOCK[4]);
           lcd.print("         ");

           // Año
           //BT.print("Ano: ");
           while(BT.available()==0){}
            CLOCK[5] = BT.parseInt();
           lcd.setCursor(0, 1);
           lcd.print("Ano: ");
           lcd.print(CLOCK[5]);
           lcd.print("         ");
        
        
           Serial.print("Procedimiento [set clock on] disparado --> ");
           Serial.print(CLOCK[0]);
           Serial.print(":");
           Serial.print(CLOCK[1]);
           Serial.print(":");
           Serial.print(CLOCK[2]);
           Serial.print(" - ");
           Serial.print(CLOCK[3]);
           Serial.print("/");
           Serial.print(CLOCK[4]);
           Serial.print("/");
           Serial.println(CLOCK[5]);
        
          // Seteo primero el RTC
          // January 21, 2014 at 3am you would call:
          RTC.adjust(DateTime(CLOCK[5], CLOCK[4], CLOCK[3], CLOCK[0], CLOCK[1], CLOCK[2]));
          
          // Cargo el reloj del micro con la fecha y hora del RTC
          DateTime now = RTC.now();                                                           // Obtiene la fecha y hora del RTC
          setTime(now.hour(),now.minute(),now.second(),now.day(),now.month(),now.year());     // Setea el clock interno a partir del RTC            

          backLightOFF(); 
          
          BT.write("OK");
      }


      // Setear USB Reloj
      if(strstr(cadena,"set clock usb")!=0)
      {
           
           Wire.begin();                                           // Inicia el puerto I2C
           RTC.begin();                                            // Inicia la comunicación con el RTC
          
           // Seteo primero el RTC con los datos de fecha y hora de la PC vía USB
           RTC.adjust(DateTime(__DATE__, __TIME__) + millis()/1000);                          // Establece la fecha y hora (Comentar una vez establecida la hora)
          
          // Cargo el reloj del micro con la fecha y hora del RTC
          DateTime now = RTC.now();                                                           // Obtiene la fecha y hora del RTC
          setTime(now.hour(),now.minute(),now.second(),now.day(),now.month(),now.year());     // Setea el clock interno a partir del RTC            
          
          Serial.println("Procedimiento [set clock usb] disparado");
          BT.write("OK");
      }



      // Ver TIMER
      if(strstr(cadena,"show timer")!=0)
      {
           
           Serial.println("Procedimiento [show timer] disparado");         
                    
                   
           LCDClear();        // Limpio el LCD para mostrar los mensajes específicos          
                    
            // Muestro en el LCD la indicación que estoy en modo SetUp de Alarma    
            backLightON();
            lcd.setCursor(0, 0);
            lcd.print("TIMER is set on");

            lcd.setCursor(0, 1);
            lcd.print("    ");
            printLCDigits(TIMER[0]);
            lcd.print(":");
            printLCDigits(TIMER[1]);
            lcd.print(":");
            printLCDigits(TIMER[2]);
            lcd.print("     ");

            backLightOFF();
            BT.write("OK");
            
            delay(5000);
            
      }


      // Setea el TIMER  que se repite cada intervalos de tiempo iguales de tiempo
      if(strstr(cadena,"set timer on")!=0)
      {

            LCDClear();        // Limpio el LCD para mostrar los mensajes específicos          
                    
            // Muestro en el LCD la indicación que estoy en modo SetUp de Alarma    
            backLightON();
            lcd.setCursor(0, 0);
            lcd.print("Setup TIMER mode");
            
  
           // Lectura de los parametros (ver combinaciones posibles de alarma en la cabecera de esta documento)
           //
           // Horas
           //BT.print("Hora: ");
           while(BT.available()==0){}
            TIMER[0] = BT.parseInt();
           lcd.setCursor(0, 1);
           lcd.print("Hora: ");
           lcd.print(TIMER[0]);
           lcd.print("         ");

           // Mínutos
           //BT.print(" Minutos: ");
           while(BT.available()==0){}
             TIMER[1] = BT.parseInt();
           lcd.setCursor(0, 1);
           lcd.print("Min: ");
           lcd.print(TIMER[1]);
           lcd.print("         ");
       
           // Segundos
           //BT.print(" Segundos: ");
           while(BT.available()==0){}
             TIMER[2] = BT.parseInt();
           lcd.setCursor(0, 1);
           lcd.print("Seg: ");
           lcd.print(TIMER[2]);
           lcd.print("         ");
       
           // Seteo del TIMER 
           Alarm.timerRepeat(TIMER[0], TIMER[1], TIMER[2], ProcesoTIMER);                    

           Serial.print("Procedimiento [set timer on] disparado --> ");
           Serial.print(TIMER[0]);
           Serial.print(":");
           Serial.print(TIMER[1]);
           Serial.print(":");
           Serial.println(TIMER[2]);
           
           
           backLightOFF();
           BT.write("OK");
      }
      


      // Ver el seteo que tengo de ALARM
      //
      if(strstr(cadena,"show alarm")!=0)
      {
           
           Serial.println("Procedimiento [show alarm] disparado");         
                    
                   
           LCDClear();        // Limpio el LCD para mostrar los mensajes específicos          
                    
            // Muestro en el LCD la indicación que estoy en modo SetUp de Alarma    
            backLightON();
            lcd.setCursor(0, 0);
            lcd.print("ALARM is set on");

            lcd.setCursor(0, 1);
            lcd.print("    ");
            printLCDigits(ALARM[0]);
            lcd.print(":");
            printLCDigits(ALARM[1]);
            lcd.print(":");
            printLCDigits(ALARM[2]);
            lcd.print("     ");

            backLightOFF();
            BT.write("OK");
            
            delay(5000);
      }


      // Setea la ALARM  que se repite cada intervalos de tiempo iguales de tiempo
      if(strstr(cadena,"set alarm on")!=0)
      {

            LCDClear();        // Limpio el LCD para mostrar los mensajes específicos          
                    
            // Muestro en el LCD la indicación que estoy en modo SetUp de Alarma    
            backLightON();
            lcd.setCursor(0, 0);
            lcd.print("Setup ALARM mode");
            
  
           // Lectura de los parametros (ver combinaciones posibles de alarma en la cabecera de esta documento)
           //
           // Horas
           //BT.print("Hora: ");
           while(BT.available()==0){}
            ALARM[0] = BT.parseInt();
           lcd.setCursor(0, 1);
           lcd.print("Hora: ");
           lcd.print(ALARM[0]);
           lcd.print("         ");

           // Mínutos
           //BT.print(" Minutos: ");
           while(BT.available()==0){}
             ALARM[1] = BT.parseInt();
           lcd.setCursor(0, 1);
           lcd.print("Min: ");
           lcd.print(ALARM[1]);
           lcd.print("         ");
       
           // Segundos
           //BT.print(" Segundos: ");
           while(BT.available()==0){}
             ALARM[2] = BT.parseInt();
           lcd.setCursor(0, 1);
           lcd.print("Seg: ");
           lcd.print(ALARM[2]);
           lcd.print("         ");
       
           // Seteo del ALARM 
           Alarm.alarmRepeat(ALARM[0], ALARM[1], ALARM[2], ProcesoALARM);                    

           Serial.print("Procedimiento [set alarm on] disparado --> ");
           Serial.print(ALARM[0]);
           Serial.print(":");
           Serial.print(ALARM[1]);
           Serial.print(":");
           Serial.println(ALARM[2]);
           
           backLightOFF();
           BT.write("OK");
      }



      
      // Reset del Micro
      if(strstr(cadena,"reset")!=0)
      {
           
            LCDClear();        // Limpio el LCD para mostrar los mensajes específicos          
                    
            // Muestro en el LCD la indicación que estoy en modo reset    
            backLightON();
            lcd.setCursor(0, 0);
            lcd.print("ALARM & TIMER   ");
            lcd.setCursor(0, 1);
            lcd.print("  R e s e t..   ");
            delay(5000);
            backLightOFF();
           
           // Reseteo del Micro                     
           Serial.println("Procedimiento [reset] disparado");
           BT.write("OK");
           reset();  
      }



      // Prueba del Buzzer
      if(strstr(cadena,"buzzer")!=0)
      {
           buzzer();
           Serial.println("Procedimiento buzzer disparado");
           BT.write("OK");
      }  
  
      
      BT.write("\r");                           //Enviamos un retorno de carro de la app. La app ya crea una línea nueva
      cleancadena();                            //Ejecutamos la función clean() para limpiar el array
   
    }
    
  }
  
}




// Muestra la Fecha y la Hora en el LCD
void digitalLCDClockDisplay()
{
  // Muestra la Fecha y Hora en el LCD
  lcd.setCursor(0, 0);
  lcd.print("    ");
  printLCDigits(hour());
  lcd.print(":");
  printLCDigits(minute());
  lcd.print(":");
  printLCDigits(second());
  lcd.print("     ");

  lcd.setCursor(0, 1);
  lcd.print("   ");
  printLCDigits(day());
  lcd.print("/");
  printLCDigits(month());
  lcd.print("/");
  printLCDigits(year());
  lcd.print("    ");

}

// Imprime la Hora en el monitor Serial si es que hay una conección USB con una PC
void digitalClockDisplay()
{
  // digital clock display of the time
  printDigits(hour());
  Serial.print(":");
  printDigits(minute());
  Serial.print(":");
  printDigits(second());
  Serial.println(); 
}


// Procedimiento que se dispara con el Timer
void ProcesoTIMER(){
  Serial.print("TIMER disparado!!! ");    
  digitalClockDisplay();     // Muestra la hora del reloj
  
 
  // Muestra el mensaje en el LCD 
  backLightON();
  lcd.setCursor(0, 0);
  lcd.print("Evento de TIMER");
  lcd.setCursor(0, 1);
  lcd.print("Disp. Proceso X ");
  buzzer();
  delay(5000);
  backLightOFF();
}


// Procedimiento que se dispara con la Alarma 
void ProcesoALARM(){
  Serial.print("ALARM disparado!!! ");    
  digitalClockDisplay();     // Muestra la hora del reloj
  
 
  // Muestra el mensaje en el LCD 
  backLightON();
  lcd.setCursor(0, 0);
  lcd.print("Evento de ALARM");
  lcd.setCursor(0, 1);
  lcd.print("Disp. Proceso X ");
  buzzer();
  delay(5000);
  backLightOFF();
}


// Hace sonar el Buzzer de Alarmas
void buzzer()
{
  // Hace sonar el buzzer 
  digitalWrite(pinAccionadorA, HIGH);
  delay(300);
  digitalWrite(pinAccionadorA, LOW);
  delay(100);
  digitalWrite(pinAccionadorA, HIGH);
  delay(300);
  digitalWrite(pinAccionadorA, LOW);
  delay(100);
  digitalWrite(pinAccionadorA, HIGH);
  delay(300);
  digitalWrite(pinAccionadorA, LOW);
}



// Monitor serial - Rellena con 0 las cifras de Día, Mes, Hora, Minuto, Segundo cuando son menores a 2 dígitos solo para mantener un formato uniforme)
void printDigits(int digits)
{
  if(digits < 10)
    Serial.print('0');
  Serial.print(digits);
}


// LCD - Rellena con 0 las cifras de Día, Mes, Hora, Minuto, Segundo cuando son menores a 2 dígitos solo para mantener un formato uniforme)
void printLCDigits(int digits)
{
  if(digits < 10)
    lcd.print("0");
    lcd.print(digits);
}


//Limpia el array que se utilizó para recibir las orden desde el BT
void cleancadena()
{
  for (int cl=0; cl<=i; cl++)
  {
    cadena[cl]=0;
  }
  i=0;
}

void backLightON()
{
   digitalWrite(pinBackLight, HIGH);
}


void backLightOFF()
{
   digitalWrite(pinBackLight, LOW);
}


void reset()
{
    delay(500);
    RESTART;
}

void LCDClear()
{
    lcd.setCursor(0, 0);
    lcd.print("                ");
    lcd.setCursor(0, 1);
    lcd.print("                ");
}
