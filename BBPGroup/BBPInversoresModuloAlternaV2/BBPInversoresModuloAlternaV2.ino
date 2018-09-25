// BBP Monitoreo de Inversores
// Módulo de lectura de baterías
// 

#include "EmonLib.h"
#include <Wire.h>
EnergyMonitor energyMonitor;

#define espera 3000

double lst = 0; //Guardo los tiempos para sincronizar lecturas de sensores de alterna

//Variables adonde guardo los datos de los sensores
int VIN   = 0; //Tension de entrada de Alterna
int VOUT  = 0; //Tension de salida Alterna
float IIN = 0; //Corriente de entrada Alterna
float IOUT= 0; //Corriente de salida Alterna


String dataFileName = ""; //Inicio la String de la trama 

void setup() {
  Wire.begin(8);                /* join i2c bus with address 8 */
  Wire.onReceive(receiveEvent); /* register receive event */
  Wire.onRequest(requestEvent); /* register request event */
  //Declaro las entradas de flancos
  pinMode(11, INPUT);           
  pinMode(12, INPUT);
  pinMode(13, OUTPUT);
  Serial.begin(115200);
  //Establesco last time
  lst = millis();
}

void loop() {
  //Si paso el tiempo de espera..
  if((millis()-lst) > espera){
    //Levanto la informacion de los sensores
    VIN =  TensionAC(A0,11,0);
    VOUT = TensionAC(A1,12,0);
    IIN   = corrienteAC(2.80, A3);
    IOUT  = corrienteAC(2.80, A2);
    //Muestro en el serial
    Serial.print("VIN= ");
    Serial.println(VIN);
    Serial.print("VOUT= ");
    Serial.println(VOUT);
    Serial.print("IIN= ");
    Serial.println(IIN);
    Serial.print("IOUT= ");
    Serial.println(IOUT);
    //Si tengo tension en entrada y salida apago la Alarma
    if(VIN !=0 && VOUT != 0)
       digitalWrite(13, LOW);
    //Genero los datos a enviar en la proxima transmision actualizados con los valores de los sensores
    CreoDatos();
   }
   
}

//Mido la tension de alterna y la devuelvo
//Parametros: Pata de señal, Pin digital Detector de ceros, correcion del valor 
//
int TensionAC(int senal, int detectordemaximos, int offset){
int contador = 0;
int maximo = 0;
int minimo = 0;
int prev = 0;
    double times = millis();
    while(digitalRead(detectordemaximos) == 1){
       if((millis()-times) > 200){
          lst = millis();
          delay(1);
          digitalWrite(13, HIGH);
          return 0; 
       }
    }
    times = millis();
    while(contador < 3){
      if((millis()-times) > 200){
        lst = millis();
        delay(1);
        digitalWrite(13, HIGH);
        return 0; 
      }
      int flancos = digitalRead(detectordemaximos);
      float lec = analogRead(senal);
      if(lec < minimo)
        minimo = lec;
      if(lec > maximo)
        maximo = lec;
      if(flancos == 0 && prev == 1)
        contador++;
      prev = flancos;
    }
  lst = millis();
  return int((maximo - minimo)*0.378) + offset;
}

// Mide la corriente de alterna (Amperes)
//
float corrienteAC(float cal, int pin){
  float Irms = energyMonitor.calcIrms(1484);
  energyMonitor.current(pin, cal);
  if(Irms < 0 || Irms < 0.1)
    Irms = 0;  
  return Irms; 
}

// Creo los datos para enviar
// Aca leeria los sensores.. la idea es que se actualice cada cierto tiempo
void CreoDatos(){
  String VI = String(VIN);
  String VII = "";
  String VO = String(VOUT);
  String VOO = "";
  String II = String(int(IIN*100));
  String III = "";
  String IO = String(int(IOUT*100));
  String IOO = "";
  if((VI.length()) < 5 ){
    for(int i = 0; i < (5 - (VI.length())); i ++){
       VII = VII + "0";   
    }
    VII = VII + VI;
  }
  else
    VII = VI;
    
  if((VO.length()) < 5 ){
      for(int i = 0; i < (5 - (VO.length())); i ++){
         VOO = VOO + "0";   
      }
      VOO = VOO + VO;
    }
    else
      VOO = VO;
      
  if((IO.length()) < 5 ){
      for(int i = 0; i < (5 - (IO.length())); i ++){
         IOO = IOO + "0";   
      }
      IOO = IOO + IO;
    }
    else
      IOO = IO;
      
  if((II.length()) < 5 ){
      for(int i = 0; i < (5 - (II.length())); i ++){
         III = III + "0";   
      }
      III = III + II;
    }
    else
      III = II;   

    dataFileName = VII + VOO + III + IOO;
}

// function that executes whenever data is received from master
void receiveEvent(int howMany) {
 Serial.print("Recibi: ");
 while (0 <Wire.available()) {
    char c = Wire.read();      /* receive byte as a character */
    Serial.print(c);           /* print the character */
  }
 Serial.println();             /* to newline */
}

// function that executes whenever data is requested from master
void requestEvent() {
 char __dataFileName[200];
 dataFileName.toCharArray(__dataFileName, sizeof(__dataFileName));
 Wire.write(__dataFileName);  /*send string on request */
}

void adaptacionString(String elemento, char tamano){
String aux = "";
  if(sizeof(elemento) < tamano){
    for(int i = 0; i < (tamano - sizeof(elemento)); i ++){
      aux = aux + "0";
    }
   elemento = aux + elemento; 
   Serial.print("String modificada = ");
   Serial.println(elemento); 
  }  
}
