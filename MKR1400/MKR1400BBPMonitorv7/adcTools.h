/*
 *   Librería de uso de herramientas para el ADC (ADS1115)
 * 
 * 
 * 
 * 
 * 
 *   date: 10/04/2019
*/

char datosSensores[50] = {0};     //Aca guardo los datos de la trama de medición de AC Byte a Byte 


const int largoMuestra = 20;                      // Largo de la muestra de lecturas DC
//float voltageArray00[largoMuestra];       // Array que guarda el muestreo de mediciones de tensión de 5 baterías
float voltageArray01[largoMuestra];         // Array que guarda el muestreo de mediciones de tensión de 5 baterías
float voltageArray02[largoMuestra];         // Array que guarda el muestreo de mediciones de tensión de 5 baterías
float voltageArray03[largoMuestra];         // Array que guarda el muestreo de mediciones de tensión de 5 baterías
float voltageArray04[largoMuestra];         // Array que guarda el muestreo de mediciones de tensión de 5 baterías

int   pVoltageArray = 0;          // Puntero al array de muestreo 


  

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




// Lectura de los voltajes de las baterias  || 0- Safety Battery   ||  1,2,3,4 - Batrías del Inversor
//
float readADCVoltaje(int pin){
  float voltage = 0.0;
  
  analogReadResolution(12);                   // Aumenta la precisión de la pata abalogina de 0-1023 a 0-4096  
  int aux = analogRead(pin);
  delay(50);
  if (pin == 0)
      voltage =  aux * (4.5714 / 4095.0);
  else
      voltage =  aux * (13.05 / 4095.0);
  
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


// Lee repetidamente las tensiones de las baterías para normalizar la medición sacando un promedio
//
void muestreoTensionDC(){

  //voltageArray00[pVoltageArray] = readADCVoltaje(0);
  voltageArray01[pVoltageArray] = readADCVoltaje(1);
  voltageArray02[pVoltageArray] = readADCVoltaje(2);
  voltageArray03[pVoltageArray] = readADCVoltaje(3);
  voltageArray04[pVoltageArray] = readADCVoltaje(4);

  Serial.print("Muestreo DC: ");
  Serial.print(pVoltageArray);
  Serial.print(" | Bat01: "); 
  Serial.print(voltageArray01[pVoltageArray]); 
  Serial.print(" | Bat02: ");
  Serial.print(voltageArray01[pVoltageArray]);
  Serial.print(" | Bat03: ");
  Serial.print(voltageArray01[pVoltageArray]);
  Serial.print(" | Bat04: ");
  Serial.println(voltageArray01[pVoltageArray]);
  
  if(pVoltageArray<largoMuestra-1)
    pVoltageArray++;
  else
    pVoltageArray=0;

  
}


void medicionDCNormalizada(){

  float arrayPromedios[5] = {0.0, 0.0, 0.0, 0.0, 0.0};

  for(int a=0;a<largoMuestra;a++){
    //arrayPromedios[0] = arrayPromedios[0] + voltageArray00[a];
    arrayPromedios[1] = arrayPromedios[1] + voltageArray01[a];
    arrayPromedios[2] = arrayPromedios[2] + voltageArray02[a];
    arrayPromedios[3] = arrayPromedios[3] + voltageArray03[a];
    arrayPromedios[4] = arrayPromedios[4] + voltageArray04[a];
  }

  //iconBateria = arrayPromedios[0] / largoMuestra; 
  bateria01 = arrayPromedios[1] / largoMuestra;
  bateria02 = arrayPromedios[2] / largoMuestra;
  bateria03 = arrayPromedios[3] / largoMuestra;
  bateria04 = arrayPromedios[4] / largoMuestra;

  Serial.print("Promedio DC");
  Serial.print(" | Bat01: "); 
  Serial.print(bateria01);
  Serial.print(" | Bat02: "); 
  Serial.print(bateria02);
  Serial.print(" | Bat03: "); 
  Serial.print(bateria03);
  Serial.print(" | Bat04: "); 
  Serial.println(bateria04);

}
