#include "EmonLib.h" // Include Emon Library
#include <Wire.h>
EnergyMonitor ctIN, ctOUT; // Create two instances

const int LEDpin=13;
const int ENPin =A7;

#define Banda 5000

unsigned long lstTime = 0;

float VIN, IIN, VOUT, IOUT;

String dataFileName = ""; //Inicio la String de la trama

void setup() {
  delay(100);
  pinMode(LEDpin, OUTPUT);
  digitalWrite(LEDpin, HIGH); delay(100); digitalWrite(LEDpin, LOW); //Parpadeo Rapido
  
  Serial.begin(115200);
  Serial.println("Medidor de Alterna Version 3");
  
  //Inicio los sensores de alterna
  ctIN.voltage(A0,  218.2, 1.7);        // Calibration, phase_shift
  ctOUT.voltage(A2, 215.194, 1.7);      // Calibration, phase_shift
  ctIN.current(A1, 1.92377);            // CT channel 1, calibration. calibration (2000 turns / 22 Ohm burden resistor = 90.909)
  ctOUT.current(A3, 1.92377);           // CT channel 2, calibration.
  
  //Inicio la comunicacion I2C
  Wire.begin(8);                        // Inicio en modo slave (dispositivo Nro 8)  
  Wire.onRequest(requestEvent);  
  
  digitalWrite(LEDpin, HIGH); delay(5000); digitalWrite(LEDpin, LOW); //long flash LED to indicate power up success
  lstTime = millis();
}

void loop() {
  if((millis() - lstTime ) > Banda){

    //Deshabilito envio de datos
    analogWrite(ENPin, 0);
    
    //Mediciones
    
    ctIN.calcVI(100,2000);                // Calculate all. No.of half wavelengths (crossings), time-out
    VIN = ctIN.Vrms ;         delay(50);
    IIN = ctIN.calcIrms(1484);delay(50);
    
    ctOUT.calcVI(100,2000);               // Calculate all. No.of half wavelengths (crossings), time-out
    VOUT = ctOUT.Vrms ;           delay(50);
    //IOUT = ctOUT.calcIrms(1484);delay(50);

    //Filtros

    if(VIN > 300  || VIN < 10)
      VIN = 0;
    if(VOUT > 300 || VOUT < 10)
      VOUT= 0;
    if(IOUT > 10 || IOUT < 0.1)
      IOUT = 0;
    if(IIN > 10 || IIN < 0.1)
      IIN = 0;

    //Habilito envio de datos
    analogWrite(ENPin, 670);
    
    //Impresion de Mediciones
    Serial.print("Lectura en: ");
    Serial.print(millis());
    Serial.print(" mS  || ");
    Serial.print("VIN: ");
    Serial.print(VIN);
    Serial.print(" | IIN: ");
    Serial.print(IIN);
    Serial.print(" \\/ VOUT: ");
    Serial.print(VOUT);
    Serial.print(" | IOUT: ");
    Serial.println(IOUT);

    //Genero la trama a enviar 
    generoTramaI2C();
    
    //Short Flash Led de estado
    digitalWrite(LEDpin, HIGH); delay(500); digitalWrite(LEDpin, LOW);
    
    //Reinicio el LastTime
    lstTime = millis();
    
  }

}

void generoTramaI2C(){
  String VI = String(round( float(VIN)*100 ));
  String II = String(round( float(IIN)*100 ));
  String VO = String(round( float(VOUT)*100 ));
  String IO = String(round( float(IOUT)*100 ));
  
  int l = VI.length();
  
  if( l < 5 ){
    for(int i = 0; i < (5 - l); i ++){
       VI += "0";   
    }
  }

  l = II.length();
  if( l < 5 ){
     for(int i = 0; i < (5 - l); i ++){
       II += "0";   
     }
   }

  l = VO.length();
  if( l < 5 ){
     for(int i = 0; i < (5 - l); i ++){
       VO += "0";   
     }
   }

  l = IO.length();
  if( l < 5 ){
     for(int i = 0; i < (5 - l); i ++){
       IO += "0";   
     }
   }

    dataFileName = VI + VO + II + IO ;
    Serial.print("'");
    Serial.print(dataFileName);
    Serial.println("'");
    Serial.println("| VI | VO | II | IO |");
    Serial.println();
}


// function that executes whenever data is requested from master
void requestEvent() {
 char __dataFileName[21];
 dataFileName.toCharArray(__dataFileName, sizeof(__dataFileName));
 Wire.write(__dataFileName);  /*send string on request */
}
