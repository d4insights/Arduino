/*
 *   Librería de uso de herramientas para el ADC (ADS1115)
 * 
 * 
 * 
 * 
 * 
 *   date: 10/04/2019
*/



// Lectura de los voltajes de las baterias  || 0- Safety Battery   ||  1,2,3,4 - Batrías del Inversor
//
float readADCVoltaje(int pin){
  float voltage = 0.0;
  
  analogReadResolution(12);                   // Aumenta la precisión de la pata abalogina de 0-1023 a 0-4096  
  
  if (pin == 0){
      int aux = analogRead(ADC_BATTERY);
      voltage =  aux * (4.2134604027 / 4095.0);
  } else {
      int aux = analogRead(pin);
      voltage =  aux * (13.05 / 4095.0);
  }
  
//  Serial.print("BAT");
//  Serial.print(pin);
//  Serial.print(": ");
//  Serial.print(aux);
//  Serial.print(" | ");
//  Serial.print(voltage);
//  Serial.println("v");
  
  if (voltage < 1)                    // Serviría para planchar el voltaje cercano a 0 si fluctúa
     voltage = 0;
     
  return voltage;   
}
