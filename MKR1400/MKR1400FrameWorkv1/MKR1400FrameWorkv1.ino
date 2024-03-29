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
#include "udpNTPTime.h" 

void setup() {
  
  delay(100);
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  
  //startOled();              // Logo d4i
  drawImagebbp();             // Logo BBPGroup

  displayHeader(modoMKR1400, iconAlerta, iconSMS, iconSincro, iconSenal, iconBateria);         // Imprime el estado en el Footer de la pantallita OLED

  displayWaitingStartUP();

  Serial.println("Starting Arduino GSM Connection..");
  modemConnect();
  startUDP();

  

//  Serial.println("Test a Google.com connection");
//  if(testConnection())
//    Serial.println("Test a Google.com server conection.. results OK");
//  else
//    Serial.println("Test a Google.com server conection.. results FAIL");  


  // Rutina de Actualización y display del Reloj
  //if(rtcDia=="" || rtcHora==""){
      //setRelojRTC();
      setUDPRelojRTC();
  //}
  displayReloj(rtcGetDia(), rtcGetHoraCorta());
  Serial.println(rtcGetDia());
  Serial.println(rtcGetHoraCorta());

  
  //modemDisconect();

}


void loop() {
  displayHeader(modoMKR1400, iconAlerta, iconSMS, iconSincro, iconSenal, iconBateria);         // Imprime el estado en el Footer de la pantallita OLED

}
