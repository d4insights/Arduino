/*
 *   Framework MKR GSM 1400
 * 
 *   Maneja Conexión 4G, Comunicación a una Base de Datos, Pantalla OLED 128x64 
 * 
 * 
 *   date: 01/03/2019
*/

// Definición de las variables de ICONOS de Estados del HEADER
String modoMKR1400    = "4g no iniciado";        // Mensaje general
String myIMEI         = "";                      // IMEI de la SIM que tengo puesta
bool iconAlerta       = false;                   // Alerte true or false
String iconSMS        = "";                      // Mail OUT, IN o vacio para que no aparezca
bool iconSincro       = false;                   // Sincronización con Internet true or false
int iconSenal         = 0;                       // Señal de 4G true or false Varía entre 0 y 31 (mejor)
float iconBateria     = 0;                       // Voltaje de la Batería auxiliar del MKR1400
int SMSRetencion      = 0;
int contrastePantalla = 200;                     // Contraste de la pantalla entre 0 y 255 

//String myLocation = "";                          // Coordenadas satelitales del dispositivo


unsigned long ultimaConexion4G  = 5000;     // Timmer hasta próxima lectura permitida de AC posterior a usar 4G
long lagPosterior4G             = 5000;     // LAG posterior a usar el 4g antes de hacer próxima lectura


// Variables Auxiliare de Monitoreo de los Inversores
float   VIN = 0;                // Voltaje de entrada       (Volts)
float   IIN = 0;                // Corriente de entrada     (Amperes)
int     PIN = 0;                // Potencia de entrada      (Watts)
float   VOUT = 0;               // Voltaje de salida        (Volts)
float   IOUT = 0;               // Corriente de salida      (Amperes)
int     POUT = 0;               // Potencia de salida       (Watts)

float  porcBat = 0.0;           // Porcentaje de llenado de la batería de soporte del Inversor (donde esta conectado el MCI)


// Deltas parta ajustar los valores de la medición desde el SMS
String mydeltaVIN  = "";
String mydeltaVOUT = "";
String mydeltaIIN  = "";
String mydeltaIOUT = "";

String mydeltaBAT0 = "";
String mydeltaBAT1 = "";
String mydeltaBAT2 = "";
String mydeltaBAT3 = "";
String mydeltaBAT4 = "";

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
//#include "mySQL.h" 
#include "adcTools.h" 
#include "flashS.h"


bool debugMode = false;                        // Poner en false cuando está en producción 
  


unsigned long ultimoCicloBody   = 9000;      // Auxiliar para manejo del período de tiempo entre ciclos de display cuerpo de pantalla
long intervalCicloBody          = 9000;      // Manejo del período de tiempo entre ciclos de display cuerpo de pantalla
unsigned long ultimoCicloHeader = 3000;      // Auxiliar para manejo del período de tiempo entre ciclos de display header
long intervalCicloHeader        = 3000;      // Manejo del período de tiempo entre ciclos de display header




void setup() {

  delay(100);
  pinMode(LED_BUILTIN, OUTPUT);               // Uso el led builtin para señalar estado de actividad 
  
  Serial.begin(115200);

  Wire.begin();

  if(debugMode){          
    if(memFlash.valid == false){           
      String tel = String("1167577019");
      tel.toCharArray(celuGuardia,20);
      memFlash.valid = true;    
    }
    else 
      leeFlashStorage();
    
    alertaSMS = false;
       
  } else {
    leeFlashStorage();
    alertaSMS = true;  
  }  

  Serial.println("Starting Arduino Aplication...");
  
  //startOled();              // Logo d4i
  drawImagebbp();             // Logo BBPGroup Splash window

  
  // Lleno la matriz de muestreo de Tensión DC antes de empezar
  //
  //displayMessage("Estabilizando", "lecturas DC");
  for(int a=0; a<largoMuestra; a++)
    muestreoTensionDC();
  
  
  displayHeader(modoMKR1400, iconAlerta, iconSMS, iconSincro, iconSenal, iconBateria);         // Imprime el estado en el Footer de la pantallita OLED
  displayMessage("Iniciando los", "servicios ...");

  Serial.println("Starting Arduino GSM Connection..");
  modemConnect();
   
  // iconBateria = readADCVoltaje(0);
  displayHeader(modoMKR1400, iconAlerta, iconSMS, iconSincro, iconSenal, iconBateria);         // Imprime el estado en el Footer de la pantallita OLED 
}



void loop() {

   // Muetreo de lecturas de las baterias (Safety y de Soporte)
   //
   muestreoTensionDC();
  
   if (millis() - ultimoCicloBody >= intervalCicloBody) {
        ultimoCicloBody = millis();      
   

        // Rutina de reconexión por si se pierde el 4G
        //
        if(!gsmAccess.ready()){
            gsmConnected = false;
            displayError("4g Desconectado", "Reconectando..");
            delay(1000);
            modemConnect();
        }


        // Handler de lectura de AC
        // OJO !!!... Tienen que pasa 10segundos despues de usar el 4g para leer estable
        //
        if (millis() - ultimaConexion4G >= lagPosterior4G) {        
            ultimaConexion4G = millis();              
            sendRequestACModule();          
        }

        
        // Handler ENTRADA Voltaje, Amperaje, Potencia 
        //
        displayInput();
        if(VIN < 190 && alertaSMS){
          iconSMS = "OUT";
          String msg = "Alerta!!.. BAJA TENSION DE ENTRADA ";
          msg = msg + '\n' + "VIN:" + (String) VIN + "v" + '\n' + "d4i:" + myIMEI;
          sendSMSTemporizedRed(celuGuardia, msg);
          //registraAlertaDB("A","AC", String(VIN,2), "BAJA_Tension_de_entrada");
        } 
        if(VIN > 245 && alertaSMS){
          iconSMS = "OUT";
          String msg = "Alerta!!.. ALTA TENSION DE ENTRADA ";
          msg = msg + '\n' + "VIN:" + (String) VIN + "v" + '\n' + "d4i:" + myIMEI;
          sendSMSTemporizedRed(celuGuardia, msg);
          //registraAlertaDB("A","AC", String(VIN,2), "ALTA_Tension_de_entrada");
        }
        if(VIN >= 190 && VIN <= 245 && iconSMS == "OUT")
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
          //registraAlertaDB("A","AC",String(IOUT,2), "EXCESIVO_Consumo_de_salida");
        }
        if(IOUT <= 4 && iconSMS == "OUT")
          iconSMS = "";
        delay(1500);



        // Handler de lectura del GPS
        //
        //myUbicacionGPS();



        // Handler Baterias de soporte del inversor
        //

        medicionDCNormalizada();
//
//        if((bateria01 < 11.0 || bateria02 < 11.0 || bateria03 < 11.0 || bateria03 < 11.0)  && alertaSMS){
//          iconSMS = "OUT";
//          iconAlerta = false;
//          String msg = "Alerta!!.. BATERIAS DE SOPORTE DEL INVERSOR ";
//          msg = msg + '\n' + "Voltaje:"+ (String) bateria01 + "v" + '\n' + "d4i:" + myIMEI;
//          sendSMSTemporizedIBat(celuGuardia, msg);
//          float MIN = 50.0;
//          if (MIN > bateria01)
//              MIN = bateria01;
//          if (MIN > bateria02)
//              MIN = bateria02;
//          if (MIN > bateria03)
//              MIN = bateria03;
//          if (MIN > bateria04)
//              MIN = bateria04;              
//          registraAlertaDB("M","DC", String(MIN,2), "BAJA_CARGA_Bateria_de_Soporte_Inversor");
//        }        
        displayBatteriesLevel();
         

      
        // Handler Bateria de Contingencia
        //
        //iconBateria = readADCVoltaje(0);
        iconBateria = bateria01;
        //iconBateria = (bateria01 + bateria02 + bateria03 + bateria04) / 4;

        //if(iconBateria < 2.7 && alertaSMS){
        if(iconBateria < 12 && alertaSMS){
          iconSMS = "OUT";
          String msg = "Alerta!!.. BATERIAS DE SOPORTE BAJO NIVEL ";
          msg = msg + '\n' + "Voltaje:" + (String) iconBateria + "v" + '\n' + "d4i:" + myIMEI;
          sendSMSTemporizedSBat(celuGuardia, msg);
          //registraAlertaDB("B","DC",String(iconBateria,2), "BAJA_CARGA_Bateria_de_Contingencia");
        }
        
        //if(iconBateria >= 2.7 && iconSMS == "OUT")
        if(iconBateria >= 12 && iconSMS == "OUT")
          iconSMS = "";
        
        //if(iconBateria < 3.8){
        if(iconBateria < 12){
          delay(800);
          displaySafetyBateryLevel(iconBateria);
        }
      
        
       
        // Handler REGISTRO EN BASE DE DATOS
        //
//        registraMedicionDB();


        
        // Handler SMS recibidos y respuestas en base a KeyWords
        // 
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
        if(saveParam){
          Serial.println("Nuevos Parametros a guardar !!!");
          escribeFlashStorage();                            // Almacena los nuevos parametros en la Flash Memory
          saveParam = false;
        }
   
        ledFlashing();                  // Flashing del LedBuitIn de la placa cada n milisegundos
   }
   
   if (millis() - ultimoCicloHeader >= intervalCicloHeader) {
        ultimoCicloHeader = millis();   
        displayHeader(modoMKR1400, iconAlerta, iconSMS, iconSincro, iconSenal, iconBateria);         // Imprime el estado en el Footer de la pantallita OLED
   }
 
}
