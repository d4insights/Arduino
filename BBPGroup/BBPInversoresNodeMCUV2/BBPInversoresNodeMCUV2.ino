// BBP Monitoreo de Inversores
// Módulo NodeMCU ESP-12 E - Control de Conectividad, pantalla y registro en MySql
//
//

#include <Wire.h>
#include "SH1106.h" 
#include "SH1106Brzo.h"
#include "images.h"

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <DNSServer.h>            //Local DNS Server used for redirecting all rs to the configuration portal
#include <ESP8266WebServer.h>     //Local WebServer used to serve the configuration portal
#include <ESP8266HTTPClient.h>    // Cliente para guardar registros en la base de datos
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager WiFi Configuration Magic


//Configuracion Multiplexor
const int muxS0 = D3;
const int muxS1 = D4;
const int muxS2 = D5;
const int muxS3 = D6;


WiFiClientSecure client;
IPAddress ip;
WiFiManager wifiManager;
//flag for saving data de WiFiManager
bool shouldSaveConfig = false;

String dataFileName = "";        //Inicializo la trama de datos a enviar 
char   datosSensores[50] = {0};  //Aca levanto los datos por cada peticion 
char   FlagNuevosDatos    =  0 ; //Me pongo en uno cuando tengo datos nuevos provenientes de los sensores

double lastSend = 0;
// Initialize the OLED display using brzo_i2c (Paralelo a conexion con sensores)
//  SDA -- D1
//  SCL -- D2
SH1106 display(0x3c, D1, D2);

#define DEMO_DURATION 3000
typedef void (*Demo)(void);
int demoMode = 0;
int counter = 1;

// Variables Auxiliare de Monitoreo de los Inversores
int   VIN = 0;                // Voltaje de entrada       (Volts)
float IIN = 0;                // Corriente de entrada     (Amperes)
int   PIN = 0;                // Potencia de entrada      (Watts)
int   VOUT = 0;               // Voltaje de salida        (Volts)
float IOUT = 0;               // Corriente de salida      (Amperes)
int   POUT = 0;               // Potencia de salida       (Watts)
float VB[13] = {0};           // Voltaje de cada Batería  (Volts)   (empieza en posición 1)       
int   CantBaterias = 3;      // Cantidad de Baterías 

String router = "Desconectado..";

double  intervaloGuardarBD = 60000;
long    ultimaHandlerBD = 0;

double  intervaloAlertaVIN = 300000;
long    ultimaAlertaVIN = 0;

// Pantalla de Información de Entrada desde la línea de red 220v
//
void drawEntrada() {
    
    // create more fonts at http://oleddisplay.squix.ch/
    
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(Open_Sans_Bold_14);
    display.drawString(1, 1, "Entrada Red >>");

    display.drawVerticalLine(40, 21, 29);
    display.drawVerticalLine(83, 21, 29);
    
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(Open_Sans_Regular_12);
    display.drawString(5, 27, String(VIN)+"V" );

    display.setTextAlignment(TEXT_ALIGN_CENTER);
    display.setFont(Open_Sans_Regular_12);
    display.drawString(64, 27, String(IIN,2)+"A");

    display.setTextAlignment(TEXT_ALIGN_RIGHT);
    display.setFont(Open_Sans_Regular_12);
    display.drawString(128, 27, String(PIN)+"W");

    drawZocalo();
    
}


// Pantalla de Información de Salida del Inversor
// 
void drawSalida() {

     // create more fonts at http://oleddisplay.squix.ch/
    
    display.setTextAlignment(TEXT_ALIGN_RIGHT);
    display.setFont(Open_Sans_Bold_14);
    display.drawString(128, 1, "<< Salida Inversor");

    display.drawVerticalLine(40, 21, 29);
    display.drawVerticalLine(83, 21, 29);
    
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(Open_Sans_Regular_12);
    display.drawString(5, 27, String(VOUT)+"V");

    display.setTextAlignment(TEXT_ALIGN_CENTER);
    display.setFont(Open_Sans_Regular_12);
    display.drawString(64, 27, String(IOUT,2)+"A");

    display.setTextAlignment(TEXT_ALIGN_RIGHT);
    display.setFont(Open_Sans_Regular_12);
    display.drawString(128, 27, String(POUT)+"W");

    drawZocalo();
}



// Pantalla de Información de Salida del Inversor
// 
void drawBaterias1_4() {

     // create more fonts at http://oleddisplay.squix.ch/
    
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    display.setFont(Open_Sans_Bold_14);
    display.drawString(64, 1, ">> Baterias 1-4 <<");

    display.drawVerticalLine(64, 21, 29);
    display.drawHorizontalLine(0, 35, 128);
    
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(Open_Sans_Regular_12);
    display.drawString(5, 18, "01 : "+String(VB[1],1)+"V");

    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(Open_Sans_Regular_12);
    display.drawString(70, 18, "02 :" +String(VB[2],1)+"V");

    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(Open_Sans_Regular_12);
    display.drawString(5,35, "03 :" +String(VB[3],1)+"V");

    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(Open_Sans_Regular_12);
    display.drawString(70, 35, "04 : 11,9V");

    drawZocalo();
}


// Pantalla de Información de Salida del Inversor
// 
void drawBaterias5_8() {

     // create more fonts at http://oleddisplay.squix.ch/
    
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    display.setFont(Open_Sans_Bold_14);
    display.drawString(64, 1, "<< Baterias 5-8 >>");

    display.drawVerticalLine(64, 21, 29);
    display.drawHorizontalLine(0, 35, 128);
    
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(Open_Sans_Regular_12);
    display.drawString(5, 18, "05 : --,-V");

    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(Open_Sans_Regular_12);
    display.drawString(70, 18, "06 : --,-V");

    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(Open_Sans_Regular_12);
    display.drawString(5,35, "07 : --,-V");

    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(Open_Sans_Regular_12);
    display.drawString(70, 35, "08 : --,-V");

    drawZocalo();
    
}


// Pantalla de Información de Salida del Inversor
// 
void drawBaterias9_12() {

     // create more fonts at http://oleddisplay.squix.ch/
    
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    display.setFont(Open_Sans_Bold_14);
    display.drawString(64, 1, ">> Baterias 9-12 <<");

    display.drawVerticalLine(64, 21, 29);
    display.drawHorizontalLine(0, 35, 128);
    
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(Open_Sans_Regular_12);
    display.drawString(5, 18, "09 : --,-V");

    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(Open_Sans_Regular_12);
    display.drawString(70, 18, "10 : --,-V");

    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(Open_Sans_Regular_12);
    display.drawString(5,35, "11 : --,-V");

    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(Open_Sans_Regular_12);
    display.drawString(70, 35, "12 : --,-V");

    drawZocalo();
    
}

// Imprime el pié de página de las hojas del slide show
//
void drawZocalo(){
    display.drawHorizontalLine(0, 52, 200);
    
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(ArialMT_Plain_10);
    display.drawString(0, 52, router);
    
    display.setTextAlignment(TEXT_ALIGN_RIGHT);
    display.setFont(ArialMT_Plain_10);
    display.drawString(124, 52, "d4");

    display.setTextAlignment(TEXT_ALIGN_RIGHT);
    display.setFont(Yellowtail_Regular_12);
    display.drawString(127, 52, "i");
}


// Definición de pantallas que harán el slide show
//
Demo demos[] = {drawEntrada, drawSalida, drawBaterias1_4, drawBaterias5_8, drawBaterias9_12};
int demoLength = (sizeof(demos) / sizeof(Demo));
long timeSinceLastModeSwitch = 3;


void setup() {
 Serial.begin(9600); /* begin serial for debug */
 display.init();
 display.flipScreenVertically();
 display.setFont(ArialMT_Plain_10);
 // Muetra los Splash Window de inicio de aplicación
 //
 drawImagebbp();
 delay(3000);
 drawImaged4i();
 delay(3000);

 drawProgressBarDemo(counter);
 // Fija el tiempo de time out para entrar en Modo de Configuración de Wifi si no se puede conectar
 wifiManager.setConnectTimeout(50);
 
 // Fija el tiempo que va a quedar prendido el AP y si nadie se conecta podríamos volver a intentar con el ciclo con el SSID  
 wifiManager.setTimeout(60);
 
 // Fija el tiempo que va a estar levantado el AP esperando por el seteo
 // luego baja el AP y deja que siga la aplicación
 // si no pones esto ojo que salta el Watch Dog que te cuelga la aplicación
 wifiManager.setConfigPortalTimeout(60);      // Tiempo que esperará el portal antes de salir aunque no configures nada
 
 wifiManager.setSaveConfigCallback(saveConfigCallback);
 
 counter = 25;
 drawProgressBarDemo(counter);
 
 //If it fails to connect it will create a TELEGRAM-BOT access point
 wifiManager.autoConnect("InversorAP");
 
 counter = 50;
 drawProgressBarDemo(counter);
 while (WiFi.status() != WL_CONNECTED){
     Serial.print(".");
     if (counter < 100) {
        drawProgressBarDemo(counter);
     }
     else
     {
        display.flipScreenVertically();               // Giro de la pantalla
        display.setFont(ArialMT_Plain_10);
        display.setTextAlignment(TEXT_ALIGN_CENTER);
        display.setContrast(255);
        display.clear();
        display.drawString(64, 10, " Configurar WiFi ");
        display.drawString(64, 20, "  InversorAP  ");
        display.drawString(64, 40, " IP: 192.168.4.1 ");
        router = "Desconectado..";
        display.display(); 
        delay(3000);
        Serial.println("Error de conexión WIFI.. reseteando la ESP");
        delay(3000);
        ESP.restart();
        delay(5000);
     }   
     counter++;
  }

  ip = WiFi.localIP();

  if (WiFi.status() == WL_CONNECTED){
    counter = 100;
    drawProgressBarDemo(counter);
    delay(500);
    router = String(WiFi.SSID()); 
  }
  else {
      router = "Conexión perdida.. !!";
      delay(2000);
      ESP.restart();
      delay(5000);
  }
 pinMode(muxS0, OUTPUT);
 pinMode(muxS1, OUTPUT);
 pinMode(muxS2, OUTPUT);
 pinMode(muxS3, OUTPUT);
 Wire.begin(D1, D2); /* join i2c bus with SDA=D1 and SCL=D2 of NodeMCU */
}

void loop() {
 // Recupera la conexión si por alguna razón se hubiera caído
 if (WiFi.status() != WL_CONNECTED){
      router = "Conexión perdida.. !!";
      delay(500);
      wifiManager.autoConnect("InversorAP");
      
      if (WiFi.status() == WL_CONNECTED)
      { 
            delay(500);
            router = String(WiFi.SSID());
      }
      else{
         router = "Conexión perdida.. !!";
         delay(2000);
         ESP.restart();
         delay(3000);        
     }
    
 }
 // Comienza el Slide Show de Pantallas que van ciclando
 //
 display.clear();
 demos[demoMode]();
 display.display();
 if (millis() - timeSinceLastModeSwitch > DEMO_DURATION) {
    Envio();
    Recibo(30);
    DecodificacionDatos();
    TensionBaterias(CantBaterias);
    demoMode = (demoMode + 1)  % demoLength;
    timeSinceLastModeSwitch = millis();
 }

 RegistrosBaseDeDatos();  // Logueo de registros en la base de datos (funcionamiento normal)

 // Rutina de revisión alerta de tensión de entrada de red
 if(VIN==0){
       AlertaBaseDeDatos(1, "A", "AC", "Corriente%20de%20Red%20Interrumpida ");      
 }
 if(VIN<190){
      AlertaBaseDeDatos(1, "A", "AC", "Baja%20tensión%20de%20Red ");  
 }
 if(VIN>245){
      AlertaBaseDeDatos(1, "A", "AC", "Alta%20tensión%20de%20Red ");     
 }



}

void TensionBaterias(int cantidad){
  for(int i = 0; i < cantidad; i ++){
    SetMuxChannel(i);
    float lec = analogRead(A0);
    VB[i+1] = ((lec * 31.50) / 1024.00);
  }  
}

//Genero la String a enviar con los parametros introducidos
//
void CreoDatos(int cantBat, int C1, int C2){
       dataFileName = String(String(cantBat)+"|"+       //Cantidad de Baterias
                      String(C1)+"|"+                   //Configurable uno
                      String(C2)+"|");                   //Configurable dos               
}

//Envio datos en funcion de lo colocado en CreoDatos
//
void Envio(){
 Wire.beginTransmission(8); /* begin with device address 8 */
 CreoDatos(100,200,120);
 char __dataFileName[200];
 dataFileName.toCharArray(__dataFileName, sizeof(__dataFileName));
 Wire.write(__dataFileName);  /* sends hello string */
 Wire.endTransmission();    /* stop transmitting */
 
}

//Recepciono datos, la longitud maxima de los mismos la doy por el parametro
//
void Recibo(int bits){
int pos = 0;
 Wire.requestFrom(8, bits); /* request & read data of size 13 from slave */
 while(Wire.available()){
    char c = Wire.read();
    datosSensores[pos] = c - 48;
    pos++;
    Serial.print(c);
    FlagNuevosDatos = 1;
 }
 Serial.println();
}

// Logo de d4i
//
void drawImaged4i() {
    // see http://blog.squix.org/2015/05/esp8266-nodemcu-how-to-create-xbm.html
    // on how to create xbm files
    display.clear();
    display.drawXbm(34, 7, d4i_Logo_width, d4i_Logo_height, d4i_Logo_bits);
    
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    display.setFont(Open_Sans_Regular_12);
    display.drawString(60, 44, "d4insights");
    
    display.display();
}


// Logo de bbp
//
void drawImagebbp() {
    // see http://blog.squix.org/2015/05/esp8266-nodemcu-how-to-create-xbm.html
    // on how to create xbm files
    display.clear();
    display.drawXbm(24, 7, bbp_Logo_width, bbp_Logo_height, bbp_Logo_bits);

    
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    display.setFont(Open_Sans_Regular_12);
    display.drawString(64, 40, "BBP group");
    
    display.setTextAlignment(TEXT_ALIGN_RIGHT);
    display.setFont(ArialMT_Plain_10);
    display.drawString(124, 52, "d4");
    display.setTextAlignment(TEXT_ALIGN_RIGHT);
    display.setFont(Yellowtail_Regular_12);
    display.drawString(127, 52, "i");
    
    display.display();
}

//Recibo los datos, los proceso y guardo
//
void ControlDatos(){
char dato = 0;//Valor actual leido
int  p    = 0;//Posicion en el array de recepcion
  if(FlagNuevosDatos == 1){
    while(dato != '¿'){
        dato = datosSensores[p];
    }  
  }   
}

// Esta función se ejecuta cuando entra en modo AP
//
void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("Entré en modo Confguración del AP");
  delay(2000);
  Serial.println(WiFi.softAPIP());
  Serial.println(myWiFiManager->getConfigPortalSSID());
}


// Esta función se ejecuta para identificar cuando hay que grabar la configuración en EEPROM
//
void saveConfigCallback () {
  Serial.println("Seteo en TRUE la variable shouldSaveConfig para guardar parámetros");
  shouldSaveConfig = true;
}

// Gráfico de avance
//
void drawProgressBarDemo(int progress) {
  
  display.clear();
  
  // draw the progress bar
  display.drawProgressBar(0, 25, 120, 10, progress);

  // draw the percentage as String
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.setFont(Open_Sans_Regular_12);
  display.drawString(64, 45, String(progress) + "%");
  
  display.display();
}

// Seteo del Canal para identificar que puerta linkea
//
int SetMuxChannel(byte channel)
{
   digitalWrite(muxS0, bitRead(channel, 0));
   digitalWrite(muxS1, bitRead(channel, 1));
   digitalWrite(muxS2, bitRead(channel, 2));
   digitalWrite(muxS3, bitRead(channel, 3));
   delay(10);
}

//Decodifico los datos provenientes del micro de sensores, guardando los datos adonde corresponde
//
void DecodificacionDatos(){
  VIN  = datosSensores[0] * 10000 + datosSensores[1] * 1000 + datosSensores[2]* 100 + datosSensores[3]* 10 + datosSensores[4]* 1;
  Serial.print("VIN: ");
  Serial.println(VIN);
  VOUT = datosSensores[5] * 10000 + datosSensores[6] * 1000 + datosSensores[7]* 100 + datosSensores[8]* 10 + datosSensores[9]* 1;
  Serial.print("VOUT: ");
  Serial.println(VOUT);
  IIN = datosSensores[10] * 100+ datosSensores[11] * 10 + datosSensores[12] + (float)datosSensores[13]/10.00  + (float)datosSensores[14]/100.00;
  Serial.print("IIN: ");
  Serial.println(IIN,2);
  IOUT = datosSensores[15] * 100+ datosSensores[16] * 10 + datosSensores[17] + (float)datosSensores[18]/10.00 + (float)datosSensores[19]/100.00;
  Serial.print("IOUT: ");
  Serial.println(IOUT,2);
  PIN  = VIN * IIN;
  POUT = VOUT* IOUT;

}

//Guardo en la base de datos
//
void RegistrosBaseDeDatos(){
  if((millis()-ultimaHandlerBD) > intervaloGuardarBD){

    HTTPClient http;
    char CadenaInstrucc[500];           // OJO QUE SI NO TE ALCANZA LA DIMENSION PARA GUARDAR EL STRING EL PROGRAMA EXPLOTA POR EL WATCHDOG DE LA ESP
 
    String Instruccion = "http://fernandezsanz.dyndns.org/edenor/insert.php";
    Instruccion.concat("?IDInversor=");
    Instruccion.concat(001);
    
    Instruccion.concat("&VIN=");
    Instruccion.concat(VIN);                //Acá forcé el led de estado del accionador para jugar
    Instruccion.concat("&IIN=");
    Instruccion.concat(String(IIN,2));
    Instruccion.concat("&PIN=");
    Instruccion.concat(PIN);
    
    Instruccion.concat("&VOUT=");
    Instruccion.concat(VOUT);
    Instruccion.concat("&IOUT=");
    Instruccion.concat(String(IOUT,2));
    Instruccion.concat("&POUT=");
    Instruccion.concat(POUT);

    Instruccion.concat("&VB01=");
    Instruccion.concat(String(VB[1],2));
    Instruccion.concat("&VB02=");
    Instruccion.concat(String(VB[2],2));
    Instruccion.concat("&VB03=");
    Instruccion.concat(String(VB[3],2));
    Instruccion.concat("&VB04=");
    Instruccion.concat(String(VB[4],2));
    Instruccion.concat("&VB05=");
    Instruccion.concat(String(VB[5],2));
    Instruccion.concat("&VB06=");
    Instruccion.concat(String(VB[6],2));
    Instruccion.concat("&VB07=");
    Instruccion.concat(String(VB[7],2));
    Instruccion.concat("&VB08=");
    Instruccion.concat(String(VB[8],2));
    Instruccion.concat("&VB09=");
    Instruccion.concat(String(VB[9],2));
    Instruccion.concat("&VB10=");
    Instruccion.concat(String(VB[10],2));
    Instruccion.concat("&VB11=");
    Instruccion.concat(String(VB[11],2));
    Instruccion.concat("&VB12=");
    Instruccion.concat(String(VB[12],2));
    Instruccion.concat("&VB13=");
    Instruccion.concat(String(VB[13],2));
    Instruccion.concat("&VB14=");
    Instruccion.concat(String(VB[14],2));
    Instruccion.concat("&VB15=");
    Instruccion.concat(String(VB[15],2));
    Instruccion.concat("&VB16=");
    Instruccion.concat(String(VB[16],2));
    
    Instruccion.concat(" ");
  
    Instruccion.toCharArray(CadenaInstrucc,Instruccion.length());
  
    http.begin(CadenaInstrucc);
  
    // Lee el HTTP que le devuelve el server de Apache
    int httpCode = http.GET();
  
    // httpCode dá negativo si hubo un error
    if(httpCode > 0) {
        
        Serial.printf("[HTTP] GET... code: %d\n", httpCode);
        
        if(httpCode == HTTP_CODE_OK) {
            String httpDevuelto = http.getString();
            Serial.println(httpDevuelto);
        }
    } 
    else 
    {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();
   
    ultimaHandlerBD = millis(); 
  }  
 
}




// Registro Alertas en la base de datos
//
void AlertaBaseDeDatos(int IDInversor, String Criticidad, String TipoAlerta, String Descripcion){
  
  if((millis()-ultimaAlertaVIN) > intervaloAlertaVIN){

    HTTPClient http;
    char CadenaInstrucc[500];           // OJO QUE SI NO TE ALCANZA LA DIMENSION PARA GUARDAR EL STRING EL PROGRAMA EXPLOTA POR EL WATCHDOG DE LA ESP

    char auxiliar[50];
    Descripcion.toCharArray(auxiliar,Descripcion.length());
    
    String Instruccion = "http://fernandezsanz.dyndns.org/edenor/alerta.php";
    Instruccion.concat("?IDInversor=");
    Instruccion.concat(IDInversor);
    
    Instruccion.concat("&Criticidad=");
    Instruccion.concat(Criticidad);                //Acá forcé el led de estado del accionador para jugar
    Instruccion.concat("&TipoAlerta=");
    Instruccion.concat(TipoAlerta);
    Instruccion.concat("&Descripcion=");
    Instruccion.concat(auxiliar);
    
    Instruccion.concat(" ");
  
    Instruccion.toCharArray(CadenaInstrucc,Instruccion.length());
    Serial.println(CadenaInstrucc);

    
    http.begin(CadenaInstrucc);
  
    // Lee el HTTP que le devuelve el server de Apache
    int httpCode = http.GET();
  
    // httpCode dá negativo si hubo un error
    if(httpCode > 0) {
        
        Serial.printf("[HTTP] GET... code: %d\n", httpCode);
        
        if(httpCode == HTTP_CODE_OK) {
            String httpDevuelto = http.getString();
            Serial.println(httpDevuelto);
        }
    } 
    else 
    {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();
    ultimaAlertaVIN = millis();  
 }
 
}  
 

