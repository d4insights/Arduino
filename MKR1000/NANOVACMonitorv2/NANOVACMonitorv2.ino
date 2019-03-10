#include <Wire.h>
#include "EmonLib.h"             // Include Emon Library
EnergyMonitor emon1;             // Create an instance

#define TiempoEntreMediciones 5         //Tiempo de espera entre mediciones (segundos)
unsigned long lastTime = 0;             //Guardo la ultima vez que realize una medicion

float Vrms = 220;                       //Tension de Alterna
float Irms = 1;                         //Corriente de Alterna

String dataFileName = "";               //Inicio la String de la trama 

void setup()
{  
  
  Wire.begin(8);                        // Inicio en modo slave (dispositivo Nro 8)
  //Wire.onReceive(receiveEvent);  
  Wire.onRequest(requestEvent);  
 
  Serial.begin(9600);

  //Mensaje de Inicio del programa
  Serial.println("Iniciando modulo de medicion de alterna.....");
  Serial.println();

  
  emon1.voltage(A0, 218.136, 1.7);  // Voltage: input pin, calibration, phase_shift
  emon1.current(A1, 1.92377);       // Current: input pin, calibration.
  
}

void loop()
{

  if((millis() - lastTime) >= TiempoEntreMediciones * 1000){
      
      emon1.calcVI(20,2000);                 // Calculate all. No.of half wavelengths (crossings), time-out
      
      Vrms = emon1.Vrms + 1.1;
      Irms = emon1.calcIrms(1484);
    
      //Filtro ruido
      if(Irms > 100 || Irms < 0.02)
        Irms = 0;

      if(Vrms > 500 || Vrms < 10 )
        Vrms = 0;  

      lastTime = millis();
      generoTramaI2C();
  }
}


void generoTramaI2C(){
  String V = String(round( float(Vrms)*100 ));
  String I = String(round( float(Irms)*100 ));

  int l = V.length();
  if( l < 5 ){
    for(int i = 0; i < (5 - l); i ++){
       V = "0"+V;   
    }
  }

  l = I.length();
  if( l < 5 ){
     for(int i = 0; i < (5 - l); i ++){
       I = "0"+I;   
     }
   }

    dataFileName = V + I;
    Serial.print("Lectura en: ");
    Serial.print(millis());
    Serial.print(" mS  || ");
    Serial.print("Vrms: ");
    Serial.print(Vrms);
    Serial.print(" | Irms: ");
    Serial.print(Irms);
    Serial.print(" || Trama:'");
    Serial.print(dataFileName);
    Serial.println("'");
}


// function that executes whenever data is requested from master
void requestEvent() {
 char __dataFileName[10];
 dataFileName.toCharArray(__dataFileName, sizeof(__dataFileName));
 Wire.write(__dataFileName);  /*send string on request */
}
