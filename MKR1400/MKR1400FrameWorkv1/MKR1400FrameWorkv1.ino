/*
 *   Framework MKR GSM 1400
 * 
 *    
 * 
 * 
 * 
*/

#include "gsmTools.h" 
#include "rtcWiFiTime.h" 
#include "sh1106I2C.h" 




void setup() {
  
  delay(100);
  Serial.begin(115200);
  startOled();
  wifiOled();
  displayReloj("15/10/69", "10:00:25");
  
  Serial.println("Starting Arduino GSM Connection..");
  modemConnect();

  Serial.println("Test a Google.com connection");
  if(testConnection())
    Serial.println("Test a Google.com server conection.. results OK");
  else
    Serial.println("Test a Google.com server conection.. results FAIL");  

  modemDisconect();

}

void loop() {

      

}
