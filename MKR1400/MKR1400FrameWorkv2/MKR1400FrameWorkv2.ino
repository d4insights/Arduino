/*
     Framework MKR GSM 1400

     Maneja Conexión 4G, Comunicación a una Base de Datos, Pantalla OLED 128x64


     date: 04/05/2019
*/

// Definición de las variables de ICONOS de Estados del HEADER
String modoMKR1400    = "4g no iniciado";      // Mensaje general
String myIMEI         = "";                      // IMEI de la SIM que tengo puesta
bool iconAlerta       = false;                   // Alerte true or false
String iconSMS        = "";                      // Mail OUT, IN o vacio para que no aparezca
bool iconSincro       = false;                   // Sincronización con Internet true or false
int iconSenal         = 0;                       // Señal de 4G true or false Varía entre 0 y 31 (mejor)
float iconBateria     = 0;                       // Voltaje de la Batería auxiliar del MKR1400
int SMSRetencion      = 0;

String myLocation = "";                          // Coordenadas satelitales del dispositivo
String myLatitud = "";
String myLongitud = "";

bool commandExecuted = true;
bool alertaSMS = true;                        // Activas o silenciar las alertas por SMS

#include "smsTools.h"
#include "sh1106I2C.h"
#include "gsmTools.h"
#include "mySQL.h"
#include "adcTools.h"
#include "flashS.h"


bool debugMode = true;                        // Poner en false cuando está en producción


unsigned long ultimoCicloBody   = 6000;      // Auxiliar para manejo del período de tiempo entre ciclos de display cuerpo de pantalla
long intervalCicloBody          = 6000;      // Manejo del período de tiempo entre ciclos de display cuerpo de pantalla
unsigned long ultimoCicloHeader = 3000;      // Auxiliar para manejo del período de tiempo entre ciclos de display header
long intervalCicloHeader        = 3000;      // Manejo del período de tiempo entre ciclos de display header



void setup() {

  delay(100);
  pinMode(LED_BUILTIN, OUTPUT);               // Uso el led builtin para señalar estado de actividad

  Serial.begin(115200);

  if (debugMode) {
    if (memFlash.valid == false) {
      String tel = String("1167577019");
      tel.toCharArray(celuGuardia, 20);
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
  startOled();                                        // Logo d4i
  delay(1500);

  rtc.begin();                                  // initialize RTC
  

  displayHeader(modoMKR1400, iconAlerta, iconSMS, iconSincro, iconSenal, iconBateria);         // Imprime el estado en el Footer de la pantallita OLED
  displayMessage("Iniciando los", "servicios ...");

  Serial.println("Starting Arduino GSM Connection..");
  modemConnect();

  iconBateria = readADCVoltaje(0);
  myUbicacionGPS();                       
  displayHeader(modoMKR1400, iconAlerta, iconSMS, iconSincro, iconSenal, iconBateria);         // Imprime el estado en el Footer de la pantallita OLED
}



void loop() {


  if (millis() - ultimoCicloBody >= intervalCicloBody) {
    ultimoCicloBody = millis();


    // Rutina de reconexión por si se pierde el 4G
    //
    if (!gsmAccess.ready()) {
      gsmConnected = false;
      displayError("4g Desconectado", "Reconectando..");
      delay(1000);
      modemConnect();
    }


    // Handler del RELOJ
    //
    if (rtc.getYear() < 18 || rtc.getYear() > 25 ) {
      setClock();                                     // Recupera la hora local y setea el reloj RTC
    }
    else {
      actualizaClock();
      displayReloj(rtcDia, rtcHora);
      delay(1000);
    }


    // Handler de lectura del GPS
    //
    myUbicacionGPS();
    displayUbicacion(myLatitud, myLongitud);


    // Handler Bateria de Contingencia
    //
    //iconBateria = readADCVolaje(0);
    //iconBateria = bateria01;
    //iconBateria = (bateria01 + bateria02 + bateria03 + bateria04) / 4;

    iconBateria = readADCVoltaje(0);
    if (iconBateria < 2.7 && alertaSMS) {
      //if(iconBateria < 10.5 && alertaSMS){
      iconSMS = "OUT";
      String msg = "Alerta!!.. BATERIA DE CONTINGENCIA ";
      msg = msg + '\n' + "BatCont:" + (String) iconBateria + "v" + '\n' + "d4i:" + myIMEI;
      //sendSMSTemporizedSBat(celuGuardia, msg);
      //registraAlertaDB("B","DC",String(iconBateria,2), "BAJA_CARGA_Bateria_de_Contingencia");
    }

    if (iconBateria >= 2.7 && iconSMS == "OUT")
      //if(iconBateria >= 10.5 && iconSMS == "OUT")
      iconSMS = "";

    //if(iconBateria < 3.8){
    //if(iconBateria < 12){
    delay(1000);
    displaySafetyBateryLevel(iconBateria);
    //}




    // Handler SMS recibidos y respuestas en base a KeyWords
    //
    reciveSMS();
    if (iconSMS == "IN")
      displaySmsMsg();
    handleCommandSMS(lastSMSRecibed);



    // Handler Confirmación si tiene un celular de guardia válido
    //
    if (memFlash.valid == false) {
      displayWarningCeluGuardia();
      iconAlerta = true;
      delay(1500);
    }
    else {
      iconAlerta = false;
    }


    // Handler de Almacenamiento en la FlashMemory un nuevo nro de Celular que haya cambiado por SMS
    //
    if (newCeluGuardia != "") {
      Serial.print("NewCeluGuardia: ");
      Serial.println(newCeluGuardia);
      escribeFlashStorage();                     // Almacena el nuevo celular en la Flash Memory
      newCeluGuardia = "";
    }

    //insertDB();

    ledFlashing();                  // Flashing del LedBuitIn de la placa cada n milisegundos
  }


  if (millis() - ultimoCicloHeader >= intervalCicloHeader) {
    ultimoCicloHeader = millis();
    displayHeader(modoMKR1400, iconAlerta, iconSMS, iconSincro, iconSenal, iconBateria);         // Imprime el estado en el Footer de la pantallita OLED
  }



}
