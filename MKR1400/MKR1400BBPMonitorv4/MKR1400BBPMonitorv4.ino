/*
 *   Framework MKR GSM 1400
 * 
 *   Maneja Conexión 4G, Comunicación a una Base de Datos, Pantalla OLED 128x64 
 * 
 * 
 *   date: 01/03/2019
*/

// Definición de las variables de ICONOS de Estados del HEADER
String modoMKR1400    = "4g disconnected";       // Mensaje general
String myIMEI         = "";                      // IMEI de la SIM que tengo puesta
bool iconAlerta       = false;                   // Alerte true or false
String iconSMS        = "";                      // Mail OUT, IN o vacio para que no aparezca
bool iconSincro       = false;                   // Sincronización con Internet true or false
int iconSenal         = 0;                       // Señal de 4G true or false Varía entre 0 y 31 (mejor)
float iconBateria     = 0;                       // Voltaje de la Batería auxiliar del MKR1400
int SMSRetencion      = 0;

String myLocation = "";                          // Coordenadas satelitales del dispositivo


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
bool alertaSMS = true;                        // Activas o silenciar las alertas por SMS

#include "smsTools.h" 
#include "sh1106I2C.h" 
#include "gsmTools.h"
#include "mySQL.h" 
#include "adcTools.h" 
#include "flashS.h"


bool debugMode = true;                        // Poner en false cuando está en producción 


unsigned long ultimoCicloBody   = 5000;      // Auxiliar para manejo del período de tiempo entre ciclos de display cuerpo de pantalla
long intervalCicloBody          = 5000;      // Manejo del período de tiempo entre ciclos de display cuerpo de pantalla
unsigned long ultimoCicloHeader = 5000;       // Auxiliar para manejo del período de tiempo entre ciclos de display header
long intervalCicloHeader        = 5000;       // Manejo del período de tiempo entre ciclos de display header



void setup() {

  delay(100);
  Serial.begin(115200);
 
  analogReadResolution(12);                   // Aumenta la precisión de la pata abalogina de 0-1023 a 0-4096  
  
  if(debugMode){          
    if(memFlash.valid == false){           
      String tel = String("1167577019");
      tel.toCharArray(celuGuardia,20);
      memFlash.valid = true;  
    }
    else 
      leeFlashStorage(); 
  } else {
    leeFlashStorage();  
  }  
  
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

        // Handler ENTRADA Voltaje, Amperaje, Potencia 
        //
        displayInput();
        if(VIN < 190 && alertaSMS){
          iconSMS = "OUT";
          String msg = "Alerta!!.. BAJA TENSION DE ENTRADA ";
          msg = msg + '\n' + "VIN:" + (String) VIN + "v" + '\n' + "d4i:" + myIMEI;
          sendSMSTemporizedRed(celuGuardia, msg);
          registraAlertaDB("A","AC","BAJA_Tension_de_entrada");
        } 
        if(VIN > 240 && alertaSMS){
          iconSMS = "OUT";
          String msg = "Alerta!!.. ALTA TENSION DE ENTRADA ";
          msg = msg + '\n' + "VIN:" + (String) VIN + "v" + '\n' + "d4i:" + myIMEI;
          sendSMSTemporizedRed(celuGuardia, msg);
          registraAlertaDB("A","AC","ALTA_Tension_de_entrada");
        }
        if(VIN >= 190 && VIN <= 240 && iconSMS == "OUT")
          iconSMS = "";
        delay(1500);


        
        // Handler SALIDA Voltaje, Amperaje, Potencia
        //
        displayOutput();
        if(IOUT > 4 && alertaSMS){
          iconSMS = "OUT";
          String msg = "Alerta!!.. CONSUMO DE SALIDA EXCESIVO ";
          msg = msg + '\n' + "IOUT:" + (String) IOUT + "a" + '\n' + "d4i:" + myIMEI;
          sendSMSTemporizedRed(celuGuardia, msg);
          registraAlertaDB("A","AC","EXCESIVO_Consumo_de_salida");
        }
        if(IOUT <= 4 && iconSMS == "OUT")
          iconSMS = "";
        delay(1500);

        // Handler de lectura del GPS
        //
        myUbicacionGPS();

        // Handler Baterias de soporte del inversor
        //
        bateria01 = readADCVolaje(1);
        bateria02 = readADCVolaje(2);
        bateria03 = readADCVolaje(3);
        bateria04 = readADCVolaje(4);
        if((bateria01 < 3.5 || bateria02 < 3.5 || bateria03 < 3.5 || bateria03 < 3.5)  && alertaSMS){
          iconSMS = "OUT";
          iconAlerta = false;
          String msg = "Alerta!!.. BATERIAS DE SOPORTE DEL INVERSOR ";
          msg = msg + '\n' + "Bat01:"+ (String) bateria01 + "v, Bat02:" + (String) bateria02 + "v, Bat03:" + (String) bateria03 + "v, Bat04:" + (String) bateria04 + "v" + '\n' + "d4i:" + myIMEI;
          sendSMSTemporizedIBat(celuGuardia, msg);
          registraAlertaDB("M","DC","BAJA_CARGA_Bateria_de_Soporte_Inversor");
        }        
        displayBatteriesLevel();
         

      
        // Handler Bateria de Contingencia
        //
        iconBateria = readADCVolaje(0);
        if(iconBateria < 2.7 && alertaSMS){
          //iconBateria = 0.0;
          iconSMS = "OUT";
          String msg = "Alerta!!.. BATERIA DE CONTINGENCIA ";
          msg = msg + '\n' + "BatCont:" + (String) iconBateria + "v" + '\n' + "d4i:" + myIMEI;
          sendSMSTemporizedSBat(celuGuardia, msg);
          registraAlertaDB("B","DC","BAJA_CARGA_Bateria_de_Contingencia");
        }
        if(iconBateria >= 2.7 && iconSMS == "OUT")
          iconSMS = "";
        if(iconBateria < 3.8){
          delay(800);
          displaySafetyBateryLevel(iconBateria);
           
        }

        
        // Handler REGISTRO EN BASE DE DATOS
        //
        registraMedicionDB();

        
        // Handler SMS
        reciveSMS();
        if(iconSMS == "IN")
            displaySmsMsg();
        handleCommandSMS(lastSMSRecibed);


        // Handler Confirmación si tiene un celular de guardia válido
        //
        if(memFlash.valid == false){
          displayWarningCeluGuardia();
          iconAlerta = true;
          delay(1500);
        }
        else {
          iconAlerta = false;
        }


        // Handler de Almacenamiento en la FlashMemory un nuevo nro de Celular que haya cambiado por SMS
        //
        if(newCeluGuardia != ""){
          Serial.print("NewCeluGuardia: ");
          Serial.println(newCeluGuardia);
          escribeFlashStorage();                     // Almacena el nuevo celular en la Flash Memory
          newCeluGuardia = "";
        }



   }
   if (millis() - ultimoCicloHeader >= intervalCicloHeader) {
        ultimoCicloHeader = millis();   
        displayHeader(modoMKR1400, iconAlerta, iconSMS, iconSincro, iconSenal, iconBateria);         // Imprime el estado en el Footer de la pantallita OLED
   }
}
