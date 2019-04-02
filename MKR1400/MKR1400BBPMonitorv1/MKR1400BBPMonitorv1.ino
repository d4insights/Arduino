/*
 *   Framework MKR GSM 1400
 * 
 *    
 * 
 * 
 * 
*/

// Definición de las variables de ICONOS de Estados del HEADER
String modoMKR1400    = "4g disconnected";       // Mensaje general
bool iconAlerta       = false;                   // Alerte true or false
String iconSMS        = "";                      // Mail OUT, IN o vacio para que no aparezca
bool iconSincro       = false;                   // Sincronización con Internet true or false
int iconSenal         = 0;                       // Señal de 4G true or false Varía entre 0 y 31 (mejor)
float iconBateria     = 0;                       // Voltaje de la Batería auxiliar del MKR1400
int SMSRetencion = 0;

#include "smsTools.h" 
#include "sh1106I2C.h" 
#include "gsmTools.h"
#include "mySQL.h" 


bool debugMode = true;                        // Poner en false cuando está en producción 
int analogPin = A0;                           // Pin para leer el voltaje de la bateria LIPO del MKR1400


unsigned long ultimoCicloBody   = 10000;      // Auxiliar para manejo del período de tiempo entre ciclos de display cuerpo de pantalla
long intervalCicloBody          = 10000;      // Manejo del período de tiempo entre ciclos de display cuerpo de pantalla
unsigned long ultimoCicloHeader = 5000;       // Auxiliar para manejo del período de tiempo entre ciclos de display header
long intervalCicloHeader        = 5000;       // Manejo del período de tiempo entre ciclos de display header



void setup() {
  delay(100);
  if(debugMode)
    Serial.begin(115200);
  Serial.println("Starting Arduino Aplication...");
  
  //startOled();              // Logo d4i
  drawImagebbp();             // Logo BBPGroup Splash window
  
  displayHeader(modoMKR1400, iconAlerta, iconSMS, iconSincro, iconSenal, iconBateria);         // Imprime el estado en el Footer de la pantallita OLED
  displayWaitingStartUP();

  Serial.println("Starting Arduino GSM Connection..");
  modemConnect();
  readVolajeLiPo();
  displayHeader(modoMKR1400, iconAlerta, iconSMS, iconSincro, iconSenal, iconBateria);         // Imprime el estado en el Footer de la pantallita OLED
}


void loop() {
   if (millis() - ultimoCicloBody >= intervalCicloBody) {
        ultimoCicloBody = millis();      

        // Handler FUNCIONALIDAD DE BBP Group para inversores       
        displayInput();
        delay(2000);
        displayOutput();
        delay(2000);
        displayBatteriesLevel();
        delay(2000);
        
        // Handler SafetyLIPO
        readVolajeLiPo();
        if(iconBateria < 3.7){
          displaySafetyBateryLevel(iconBateria);
          delay(2000);
        }
        registraDBvLIPO();

        // Handler SMS
        reciveSMS();
        if(iconSMS == "IN")
            displaySmsMsg();
   }
   if (millis() - ultimoCicloHeader >= intervalCicloHeader) {
        ultimoCicloHeader = millis();   
        displayHeader(modoMKR1400, iconAlerta, iconSMS, iconSincro, iconSenal, iconBateria);         // Imprime el estado en el Footer de la pantallita OLED
   }
}


// Mido el voltaje de la bateria de LIPO MKR1400 para mostrar en el display
void readVolajeLiPo(){
  analogReadResolution(12);                 // Aumenta la precisión de la pata abalogina de 0-1023 a 0-4096  
  analogReference(AR_DEFAULT);
  int aux = analogRead(analogPin);
  iconBateria =  aux * (5.97 / 4095);
  Serial.print(aux);
  Serial.print(" | ");
  Serial.println(iconBateria);

  if(iconBateria >= 1.0 && iconSMS == "OUT")
      iconSMS = "";
  
}

//void readVolajeLiPo(){
//  analogReadResolution(12);                 // Aumenta la precisión de la pata abalogina de 0-1023 a 0-4096  
//  int aux = analogRead(ADC_BATTERY);
//  iconBateria =  aux * (4.3 / 4095.0);
//  Serial.print(aux);
//  Serial.print(" | ");
//  Serial.println(iconBateria);
//
//  if(iconBateria >= 1.0 && iconSMS == "OUT")
//      iconSMS = "";
//  
//}


//void readVolajeLiPo(){
//
//  analogReadResolution(10); 
//  analogReference(AR_INTERNAL1V0);            //AR_DEFAULT: the default analog reference of 3.3V // AR_INTERNAL1V0: a built-in 1.0V reference
//  
//  int aux = analogRead(ADC_BATTERY);
//  iconBateria =  aux * (3.25 / 1023.0);
//  Serial.print(aux);
//  Serial.print(" | ");
//  Serial.println(iconBateria);
//
//  if(iconBateria >= 1.0 && iconSMS == "OUT")
//      iconSMS = "";
//  
//}
