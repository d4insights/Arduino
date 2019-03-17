/*
 *   Framework MKR GSM 1400
 * 
 *    
 * 
 * 
 * 
*/

#include "gsmTools.h" 





void setup() {
  
  delay(100);
  Serial.begin(115200);


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
