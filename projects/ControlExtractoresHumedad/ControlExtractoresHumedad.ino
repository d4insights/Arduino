// Proyecto: CONTROL DE EXTRACTORES DE HUMEDAD
// Alcance:  Activar un dispositivo de extracción de aire de 220v a partir de un umbral 
//           de humedad ambiente censado por un DHT11 con un ciclo de 1 minuto
//              - El motor puede permanecer prendido hasta maxupTime (minutos)
//              - Después de maxupTime debe descansar como mínimo el 30% del tiempo de trabajo (dinRest)
//              - Si el motor viene sin esfuerzos el tiempo mínimo para rearrancar el motor es de mindownTime
//              - time guarda los minutos que hace desde que prendí el microcontrolador
// Autor:    Diego C. Fernández Sanz 
// Fecha:    30/07/2017


#include <SimpleDHT.h>

int pinDHT11        = 2;                        // pin de entrada de la señal del sensor DHT11
int pinMotor        = 13;                       // pin de salida que controla el motor
int pinResist       = A0;                       // pin del preset que regula el % humedad

float Regulador     = 0.00;                     // Valor de lectura del preset que regula el % humedad
float Umbral        = 0.00;                     // % de Humedad a la que quiero que se prenda el motor
byte temperature    = 0;
byte humidity       = 0;
int err             = SimpleDHTErrSuccess;      // Atributo de error del DHT11
unsigned long time  = 0;                        // Minutos que hace desde que arranqué el dispositivo (sensor)
int stateMotor      = 0;                        // Estado del motor 0=Apagado, 1=Prendido
int upTime          = 0;                        // Minutos que hace que el motor esta prendido
int downTime        = 0;                        // Minutos que hace que el motor esta apagado
int maxupTime       = 120;                      // Minutos de máximo que quiero que el motor permanezca prendido 
int mindownTime     = 2;                        // Minutos de mínima de descanso para que el motor se vuelva a prender
int dinRest         = 0;                        // Minutos de descanso dinámico para cuando vengo de cortar por maxupTime (fijo en 30%) de este último


SimpleDHT11 dht11;                              // Instancio el objeto DHT11


void setup() {
  Serial.begin(9600);

  pinMode(pinMotor, OUTPUT);
  pinMode(pinResist, INPUT);
  pinMode(pinDHT11, INPUT);
 
  digitalWrite(pinMotor, LOW);
  digitalWrite(pinDHT11,HIGH);
  
}


void loop() {
  
  
  // Leo el sensor de DHT11 y si tira error muestro el código y vuelvo a iniciar el ciclo
  if ((err = dht11.read(pinDHT11, &temperature, &humidity, NULL)) != SimpleDHTErrSuccess) 
  {
    Serial.print("Error DHT11, err="); 
    Serial.println(err); 
    delay(1000);
    return;
  }


  // Leo el pin de una resistencia variable entre 0..1023 
  // y la traspolo a 0..100% para manejar el umbral al que quiero
  // que prenta el extractor
  Regulador = analogRead(pinResist);
  Umbral = (int)((Regulador - 0.00) * (100.00 - 0.00) / (1023.00 - 0.00) + 0.00);



  // Control del encendido y apagado del motor
  // 
  if(stateMotor == 0)
  {
      if((humidity>=Umbral && downTime>(mindownTime+dinRest)) || (humidity>=Umbral && time < 2))
      {
          digitalWrite(pinMotor, HIGH);
          stateMotor = 1;
          dinRest = dinRest/3;          // Bajo a un 30% el descanso dinámico considerando que estoy saliendo de un descanso
      } 
  }
  if(stateMotor == 1)
  {
      if(humidity<Umbral || upTime>maxupTime)
      {
          digitalWrite(pinMotor, LOW);
          stateMotor = 0;
          dinRest = upTime/3;          // Defino un descanso dinámico del 30% del último período que el motor estuve prendido
      } 
  }
   

  // Saco por el monitor serial los valores de estado
  //
  Serial.print("Temp: ");
  Serial.print((int)temperature); 
  Serial.print(" C - Humedad: "); 
  Serial.print((int)humidity); 
  Serial.print(" % - UmbralH: ");
  Serial.print(Umbral);
  Serial.print(" % - StateMotor: ");
  Serial.print(stateMotor);
  Serial.print(" - Time: ");
  Serial.print(time);
  Serial.print(" - UPTime: ");
  Serial.print(upTime);
  Serial.print(" - DOWNTime: ");
  Serial.print(downTime);
  Serial.print(" - dinRest: ");
  Serial.println(dinRest);
  

  // Acumula el UPTIME del motor para que no quede prendido eternamente un día de lluvia con 100% de Humedad
  // por otro lado el DOWNTIME de motor asegura un tiempo de descanso del motor y evita que se reprenda el
  // motor un día de lluvia luego de haber cumplido el tiempo máximo de UPTIME
  if (stateMotor == 0)
  {
    upTime=0;
    downTime++;
  }
  else
  {
    upTime++;
    downTime=0;    
  }  
  
  time++;               // Con un delay de 60.000 en el ciclo este timer cuenta minutos reloj

  delay(60000);         // NO CAMBIAR ESTE DELAY PORQUE REPRESENTA UN TIEMPO DE CICLO DE 1 MINUTO   
}
