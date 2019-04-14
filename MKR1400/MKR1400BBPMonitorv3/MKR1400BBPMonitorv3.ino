/*
 *   Framework MKR GSM 1400
 * 
 *   Maneja Conexión 4G, Comunicación a una Base de Datos, Pantalla OLED 128x64 
 * 
 * 
 *   date: Mar/2019
*/

// Definición de las variables de ICONOS de Estados del HEADER
String modoMKR1400    = "4g disconnected";       // Mensaje general
String myIMEI         = "";                      // IMEI de la SIM que tengo puesta
bool iconAlerta       = false;                   // Alerte true or false
String iconSMS        = "";                      // Mail OUT, IN o vacio para que no aparezca
bool iconSincro       = false;                   // Sincronización con Internet true or false
int iconSenal         = 0;                       // Señal de 4G true or false Varía entre 0 y 31 (mejor)
float iconBateria     = 0;                       // Voltaje de la Batería auxiliar del MKR1400
int SMSRetencion = 0;

// Variables Auxiliare de Monitoreo de los Inversores
float   VIN = 0;                // Voltaje de entrada       (Volts)
float   IIN = 0;                // Corriente de entrada     (Amperes)
int     PIN = 0;                // Potencia de entrada      (Watts)
float   VOUT = 0;               // Voltaje de salida        (Volts)
float   IOUT = 0;               // Corriente de salida      (Amperes)
int     POUT = 0;               // Potencia de salida       (Watts)


// Granja de baterias del inversor
float bateria01 = 0.0;
float bateria02 = 0.0;
float bateria03 = 0.0;
float bateria04 = 0.0;


bool commandExecuted = true;


#include "smsTools.h" 
#include "sh1106I2C.h" 
#include "gsmTools.h"
#include "mySQL.h" 
#include "adcTools.h" 


bool debugMode = true;                        // Poner en false cuando está en producción 


unsigned long ultimoCicloBody   = 10000;      // Auxiliar para manejo del período de tiempo entre ciclos de display cuerpo de pantalla
long intervalCicloBody          = 10000;      // Manejo del período de tiempo entre ciclos de display cuerpo de pantalla
unsigned long ultimoCicloHeader = 5000;       // Auxiliar para manejo del período de tiempo entre ciclos de display header
long intervalCicloHeader        = 5000;       // Manejo del período de tiempo entre ciclos de display header



void setup() {
  delay(100);
  
 
  analogReadResolution(12);                   // Aumenta la precisión de la pata abalogina de 0-1023 a 0-4096  
  
  if(debugMode)
    Serial.begin(115200);
  Serial.println("Starting Arduino Aplication...");
  
  //startOled();              // Logo d4i
  drawImagebbp();             // Logo BBPGroup Splash window
  
  displayHeader(modoMKR1400, iconAlerta, iconSMS, iconSincro, iconSenal, iconBateria);         // Imprime el estado en el Footer de la pantallita OLED
  displayWaitingStartUP();

  Serial.println("Starting Arduino GSM Connection..");
  modemConnect();
  iconBateria = readADCVolaje(0);
  displayHeader(modoMKR1400, iconAlerta, iconSMS, iconSincro, iconSenal, iconBateria);         // Imprime el estado en el Footer de la pantallita OLED
}


void loop() {
   if (millis() - ultimoCicloBody >= intervalCicloBody) {
        ultimoCicloBody = millis();      

        sendRequestACModule();

        // Handler Voltaje, Amperaje, Potencia de ENTRADA       
        displayInput();
        delay(1500);
        
        // Handler Voltaje, Amperaje, Potencia de SALIDA
        displayOutput();
        delay(1500);

        // Handler Bateria de soporte del inversor
        bateria01 = readADCVolaje(1);
        bateria02 = readADCVolaje(2);
        bateria03 = readADCVolaje(3);
        bateria04 = readADCVolaje(4);
        if(bateria01 < 3.5 || bateria02 < 3.5 || bateria03 < 3.5 || bateria03 < 3.5){
          iconSMS = "OUT";
          iconAlerta = false;
          String msg = "Alerta!!.. Baterias de Soporte de Inversor ";
          msg = msg + '\n' + "Bat01:"+ (String) bateria01 + "v, Bat02:" + (String) bateria02 + "v, Bat03:" + (String) bateria03 + "v, Bat04:" + (String) bateria04 + "v" + '\n' + "d4i:" + myIMEI;
          sendSMSTemporizedIBat(celuGuardia, msg);
        }        
        displayBatteriesLevel();
        delay(500);

      
        // Handler SafetyLIPO
        iconBateria = readADCVolaje(0);
        if(iconBateria < 1.0){
          //iconBateria = 0.0;
          iconSMS = "OUT";
          String msg = "Alerta!!.. SafetyBatery: ";
          msg = msg + '\n' + (String) iconBateria + "v" + '\n' + "d4i:" + myIMEI;
          sendSMSTemporizedSBat(celuGuardia, msg);
        }
        if(iconBateria >= 1.0 && iconSMS == "OUT")
          iconSMS = "";
        if(iconBateria < 3.8){
          displaySafetyBateryLevel(iconBateria);
          delay(1000);
        }
        registraDBvLIPO();

        
        // Handler SMS
        reciveSMS();
        if(iconSMS == "IN")
            displaySmsMsg();
        handleCommandSMS(lastSMSRecibed);


   }
   if (millis() - ultimoCicloHeader >= intervalCicloHeader) {
        ultimoCicloHeader = millis();   
        displayHeader(modoMKR1400, iconAlerta, iconSMS, iconSincro, iconSenal, iconBateria);         // Imprime el estado en el Footer de la pantallita OLED
   }
}
