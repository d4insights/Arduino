/*
 *   Librería de uso de herramientas para el ADC (ADS1115)
 * 
 * 
 * 
 * 
 * 
 *   date: 10/04/2019
*/

char datosSensores[50] = {0};     //Aca guardo los datos de medición de AC Byte a Byte 



//Decodifico los datos provenientes del micro de sensores, guardando los datos adonde corresponde
//
void decodificacionTramaAC(){
  float VI,VO,II,IO;
  
  VI  = datosSensores[0] * 100 + datosSensores[1] * 10 + datosSensores[2] + (float)datosSensores[3] / 10.0 + (float)datosSensores[4] / 100.0;
  VO = datosSensores[5] * 100 + datosSensores[6] * 10 + datosSensores[7] + (float)datosSensores[8] / 10.0 + (float)datosSensores[9] / 100.0;
  
  II = datosSensores[10] + (float)datosSensores[11] / 10.0 + (float)datosSensores[12] / 100.00 +  (float)datosSensores[13] / 1000.0  + (float)datosSensores[14] / 10000.0;
  IO = datosSensores[15] + (float)datosSensores[16] / 10.0 + (float)datosSensores[17] / 100.00 +  (float)datosSensores[18] / 1000.0  + (float)datosSensores[14] / 10000.0;

  if(VI < 300 && VO < 300 && II < 10 && IO < 10){
    VIN  = VI;
    VOUT = VO;
    IOUT = IO;
    IIN  = II;
    Serial.print("Mediciones AC --> VIN: ");
    Serial.print(VIN);
    Serial.print(" | VOUT: ");
    Serial.print(VOUT);
    Serial.print(" | IIN: ");
    Serial.print(IIN,2);
    Serial.print(" | IOUT: ");
    Serial.println(IOUT,2);
    PIN  = VIN * IIN;
    POUT = VOUT * IOUT;   
  }
}



void sendRequestACModule(){
  int pos = 0;
  Wire.requestFrom(8, 21);              // Manda la petición al dispositivo 8 y espera recibir 21 bytes
  Serial.print("Trama Recibida:<");
  while(Wire.available())               // slave may send less than requested
  { 
    char c = Wire.read();               // receive a byte as character
    datosSensores[pos] = c - 48;
    pos++;
    Serial.print(c);           
  }
  Serial.println(">");  
  decodificacionTramaAC();
}



float readADCVolaje(int pin){
  float voltage = 0.0;
  
  analogReadResolution(12);                   // Aumenta la precisión de la pata abalogina de 0-1023 a 0-4096  
  int aux = analogRead(pin);
  
  if (pin == 0)
      voltage =  aux * (4.5714 / 4095.0);
  else
      voltage =  aux * (13.6454 / 4095.0);
  
  Serial.print("BAT");
  Serial.print(pin);
  Serial.print(": ");
  Serial.print(aux);
  Serial.print(" | ");
  Serial.print(voltage);
  Serial.println("v");
  
  if (voltage < 1)                    // Serviría para planchar el voltaje cercano a 0 si fluctúa
     voltage = 0;
     
  return voltage;   
}
