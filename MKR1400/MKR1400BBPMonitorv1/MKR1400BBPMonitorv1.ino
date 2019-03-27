/*
 *   Framework MKR GSM 1400
 * 
 *    
 * 
 * 
 * 
*/


#include "sh1106I2C.h" 
#include "gsmTools.h" 

bool debugMode = true;                // Poner en false cuando está en producción 
int analogPin = A0;                   // Pin para leer el voltaje de la bateria LIPO del MKR1400


void setup() {
  pinMode(analogPin, INPUT);
  
  delay(100);
  if(debugMode){
    Serial.begin(115200);
//    while (!Serial) {
//      ; // wait for serial port to connect. Needed for native USB port only
//    }
  }
  
  //startOled();              // Logo d4i
  drawImagebbp();             // Logo BBPGroup
  delay(1000);
  
  displayHeader(modoMKR1400, iconAlerta, iconSMS, iconSincro, iconSenal, iconBateria);         // Imprime el estado en el Footer de la pantallita OLED
  displayWaitingStartUP();

  Serial.println("Starting Arduino GSM Connection..");
  modemConnect();

  

//  Serial.println("Test a Google.com connection");
//  if(testConnection())
//    Serial.println("Test a Google.com server conection.. results OK");
//  else
//    Serial.println("Test a Google.com server conection.. results FAIL");  

 
}


void loop() {

  // Mido el voltaje de la bateria de LIPO MKR1400 para mostrar en el display
  int aux = analogRead(analogPin);
  iconBateria =  aux * (5.97 / 1023);
  Serial.print(aux);
  Serial.print(" | ");
  Serial.println(iconBateria);


  displayHeader(modoMKR1400, iconAlerta, iconSMS, iconSincro, iconSenal, iconBateria);         // Imprime el estado en el Footer de la pantallita OLED
  
  displayBateryLevel(iconBateria);
  
  delay(5000);
}
