/*
 *   Librería de uso de herramientas para el ADC (ADS1115)
 * 
 * 
 * 
 * 
 * 
 * 
*/



float readADCVolaje(int pin){
  float voltage = 0.0;
  int aux = analogRead(pin);
  delay(50);
  voltage =  aux * (4.7472 / 4095.0);
  Serial.print("BAT");
  Serial.print(pin);
  Serial.print(": ");
  Serial.print(aux);
  Serial.print(" | ");
  Serial.print(voltage);
  Serial.println("v");
  
//  if (voltage < 0.5)                    // Serviría para planchar el voltaje cercano a 0 si fluctúa
//     voltage = 0;
     
  return voltage;   
}
