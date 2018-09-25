
/*
     Programa: Controlador de Alarmas diarias, semanales y timers
               0.  Capas de operar con el Clock de tiempo real RTC
               1.  Si existe una conexion serial activa se puede sincronizar la fecha y la hora con la PC
               2.  Permite setear múltiple alarmas semanales o diarias
               3.  Permite setear timers de ejecución automática o de ejecución de única vez
               4.  Muestra la hora en un display LCD de 16 X 2
               
               
     Autor: Diego Fernández Sanz
     Fecha: 08/02/2015    
     
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

#include <Wire.h>
#include <RTClib.h>
#include <TimeAlarms.h> 
#include <Time.h>
#include <LiquidCrystal.h>

// RTC Clock
RTC_DS1307 RTC;                                        // Crea el objeto Clock de Tiempo Real
const char* days[] = {"Domingo", "Lunes", "Martes", "Miércoles", "Jueves", "Viernes", "Sábado"};

// Alarm
int led=13;                                            // Define el pin donde voy a poner la alarma

// Define e inicializa los pines de la interface de LCD
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);





void setup()
{
  // Seteo la cantidad de filas y columnas que va a tener el LCD 
  lcd.begin(16, 2);
  
  // Inicializo la comunicación con el RTC Clock
  Wire.begin();                                     // Inicia el puerto I2C
  RTC.begin();                                      // Inicia la comunicación con el RTC
  //RTC.adjust(DateTime(__DATE__, __TIME__));       // Establece la fecha y hora (Comentar una vez establecida la hora)
  //setTime(19,25,0,16,2,2015);                     // Setea manualmente el reloj interno del micro (sin RTC) -- > 19:25:00 16/02/2015
 
  // Leo el RTC para cargar el objeto reloj interno desde el que después voy a comparar las alarmas
  DateTime now = RTC.now();                                                           // Obtiene la fecha y hora del RTC
  setTime(now.hour(),now.minute(),now.second(),now.day(),now.month(),now.year());     // Setea el clock interno a partir del RTC            
    
    
  Serial.begin(9600);
  
  pinMode(led, OUTPUT);
   
  //Ejemplos de seteos de alarmas y timers 
  //Alarm.alarmRepeat(8,30,0, MorningAlarm);              // 8:30am every day
  //Alarm.alarmRepeat(17,45,0,EveningAlarm);              // 5:45pm every day 
  //Alarm.alarmRepeat(dowSaturday,8,30,30,WeeklyAlarm);   // 8:30:30 every Saturday 
  //Alarm.timerOnce(10, OnceOnly);                        // called once after 10 seconds 
 
  Alarm.timerRepeat(30, AlarmA);                          // timer for every 15 seconds para encender el dispositivo   

}

void  loop(){  
  
  //digitalClockDisplay();                                                             // Muestra la hora del reloj
  
  Alarm.delay(100);                                                                  // Muestreo de las alarmas y timers
  
  // Muestra la Fecha y Hora en el LCD
  lcd.setCursor(0, 0);
  lcd.print("Time: ");
  printLCDigits(hour());
  lcd.print(":");
  printLCDigits(minute());
  lcd.print(":");
  printLCDigits(second());
  lcd.setCursor(0, 1);
  lcd.print("Date: ");
  printLCDigits(day());
  lcd.print("/");
  printLCDigits(month());
  lcd.print("/");
  printLCDigits(year());
  
  
}





void AlarmA(){
  Serial.print("Alarma Conectada!!! ");    
  digitalClockDisplay();     // Muestra la hora del reloj
  
  // Muestra el mensaje en el LCD 
  lcd.setCursor(0, 0);
  lcd.print("ALARMAAAA!!!     ");
  lcd.setCursor(0, 1);
  lcd.print("Disparo Proceso X");

  buzzer();
  
  delay(5000);
}


void buzzer()
{
  // Hace sonar el buzzer 
  digitalWrite(led, HIGH);
  delay(300);
  digitalWrite(led, LOW);
  delay(100);
  digitalWrite(led, HIGH);
  delay(300);
  digitalWrite(led, LOW);
  delay(100);
  digitalWrite(led, HIGH);
  delay(300);
  digitalWrite(led, LOW);
}


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

void printDigits(int digits)
{
  if(digits < 10)
    Serial.print('0');
  Serial.print(digits);
}

void printLCDigits(int digits)
{
  if(digits < 10)
    lcd.print("0");
    lcd.print(digits);
}

