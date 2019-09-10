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


const int largoMuestra = 50;                // Largo de la muestra de lecturas DC
//float voltageArray00[largoMuestra];       // Array que guarda el muestreo de mediciones de tensión de 5 baterías
float voltageArray01[largoMuestra];         // Array que guarda el muestreo de mediciones de tensión de 5 baterías
float voltageArray02[largoMuestra];         // Array que guarda el muestreo de mediciones de tensión de 5 baterías
float voltageArray03[largoMuestra];         // Array que guarda el muestreo de mediciones de tensión de 5 baterías
float voltageArray04[largoMuestra];         // Array que guarda el muestreo de mediciones de tensión de 5 baterías

int   pVoltageArray = 0;                    // Puntero al array de muestreo 



// Request al NANO para que me devuelva las medicines de AC
// 
void sendRequestACModule(){

  Wire.requestFrom(8, 50);              // Manda la petición al dispositivo 8 y espera recibir 100 bytes

  int espera = 0;
  while(!Wire.available() && espera<100){
    espera++;
    delay(1);
  }

  // Serial.println(Wire.readStringUntil('F'));


 
  float VINaux  = Wire.readStringUntil('|').toFloat();
  float IINaux  = Wire.readStringUntil('|').toFloat();
  int   PINaux  = Wire.readStringUntil('|').toInt(); 
  float VOUTaux = Wire.readStringUntil('|').toFloat();
  float IOUTaux = Wire.readStringUntil('|').toFloat();
  int   POUTaux = Wire.readStringUntil('|').toInt();
  
//  if(Wire.readStringUntil('!') == "FIN"){
     VIN  = VINaux;
     IIN  = IINaux;
     PIN  = PINaux;
     VOUT = VOUTaux;
     IOUT = IOUTaux;
     POUT = POUTaux;
//  }
//  else
//  {
//    Serial.println("OJO.. La lectura de la trama no tiene el FIN de transmisión !!!");
//    displayWarningErrorMedicion();
//    delay(1000);
//  }


  Serial.print("VIN: ");
  Serial.print(VIN);
  Serial.print(" | IIN: ");
  Serial.print(IIN);
  Serial.print(" | PIN: ");
  Serial.print(PIN);
  Serial.print(" | VOUT: ");
  Serial.print(VOUT);
  Serial.print(" | IOUT: ");
  Serial.print(IOUT);
  Serial.print(" | POUT: ");
  Serial.println(POUT);
  
}




// Lectura de los voltajes de las baterias  || 0- Safety Battery   ||  1,2,3,4 - Baterías del Inversor
//
float readADCVoltaje(int pin){
  float voltage = 0.0;
  
//  float coef05V = 4.5714;                   // Coeficiente Multiplicador del divisor resistivo 5V
  float coef12V = 14.10;                      // Coeficiente Multiplicador del divisor resistivo 12V (Aprox.:14.3021393651 )
  
  analogReadResolution(12);                   // Aumenta la precisión de la pata abalogina de 0-1023 a 0-4096  
  delay(100);                                 // Espero a que se nivele antes de medir  
  int aux = analogRead(pin);
  delay(50);                                  // Separo mediciones por las dudas para que sea mas estable


  
//  if (pin == 0)
//      voltage =  (aux * (coef05V / 4095.0)) + mydeltaBAT0.toFloat();
  if (pin == 1)
      voltage =  (aux * (coef12V / 4095.0)) + mydeltaBAT1.toFloat();
  if (pin == 2)
      voltage =  (aux * (coef12V / 4095.0)) + mydeltaBAT2.toFloat();
  if (pin == 3)
      voltage =  (aux * (coef12V / 4095.0)) + mydeltaBAT3.toFloat();
  if (pin == 4)
      voltage =  (aux * (coef12V / 4095.0)) + mydeltaBAT4.toFloat();
  
//  Serial.print("BAT");
//  Serial.print(pin);
//  Serial.print(": ");
//  Serial.print(aux);
//  Serial.print(" | ");
//  Serial.print(voltage);
//  Serial.println("v");
  
  if (voltage < 4)                    // Serviría para planchar el voltaje cercano a 0 si fluctúa
     voltage = 0;
     
  return voltage;   
}


// Lee repetidamente las tensiones de las baterías para normalizar la medición sacando un promedio
//
void muestreoTensionDC(){

  //voltageArray00[pVoltageArray] = readADCVoltaje(0);
    voltageArray01[pVoltageArray] = readADCVoltaje(2);
  //voltageArray02[pVoltageArray] = readADCVoltaje(2);
  //voltageArray03[pVoltageArray] = readADCVoltaje(3);
  //voltageArray04[pVoltageArray] = readADCVoltaje(4);

  //Serial.print("Muestreo DC: ");
  //Serial.print(pVoltageArray);
  //Serial.print(" | Bat01: "); 
  //Serial.println(voltageArray01[pVoltageArray]); 
  //Serial.print(" | Bat02: ");
  //Serial.print(voltageArray02[pVoltageArray]);
  //Serial.print(" | Bat03: ");
  //Serial.print(voltageArray03[pVoltageArray]);
  //Serial.print(" | Bat04: ");
  //Serial.println(voltageArray04[pVoltageArray]);
  
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
    //arrayPromedios[2] = arrayPromedios[2] + voltageArray02[a];
    //arrayPromedios[3] = arrayPromedios[3] + voltageArray03[a];
    //arrayPromedios[4] = arrayPromedios[4] + voltageArray04[a];
  }

  //iconBateria = arrayPromedios[0] / largoMuestra; 
  bateria01 = arrayPromedios[1] / largoMuestra;
  //bateria02 = arrayPromedios[2] / largoMuestra;
  //bateria03 = arrayPromedios[3] / largoMuestra;
  //bateria04 = arrayPromedios[4] / largoMuestra;

  // if(bateria01 < 1.5) bateria01=0;             // Plancho un error posible cuando las baterías estan desconectadas
  if(bateria01 < 3.0) bateria01=0;                // Plancho un error posible cuando las baterías estan desconectadas
  //if(bateria02 < 3.0) bateria02=0;              // Plancho un error posible cuando las baterías estan desconectadas
  //if(bateria03 < 3.0) bateria03=0;              // Plancho un error posible cuando las baterías estan desconectadas
  //if(bateria04 < 3.0) bateria04=0;              // Plancho un error posible cuando las baterías estan desconectadas

  Serial.print("Promedio DC");
  Serial.print(" | Bat01: "); 
  Serial.println(bateria01);
  //Serial.print(" | Bat02: "); 
  //Serial.print(bateria02);
  //Serial.print(" | Bat03: "); 
  //Serial.print(bateria03);
  //Serial.print(" | Bat04: "); 
  //Serial.println(bateria04);
  Serial.println();

  
  // Cálculo del Porcentaje de baterías restante
  
  
  //porcBat = map(bateria01, 10.5, 12.6, 0.0, 100.0);

  porcBat = ((( (float)bateria01 - 10.5f)) / 2.1f) * 100.0f ;
  
  if (porcBat > 100.0f)
     porcBat = 100.0f;
  if (porcBat < 0.0f)
     porcBat = 0.0f;
     
  
  


  // Cálculo del tiempo de baterías (APROXIMACION)
  // Hay que definir el cálculo 
  // 

}
