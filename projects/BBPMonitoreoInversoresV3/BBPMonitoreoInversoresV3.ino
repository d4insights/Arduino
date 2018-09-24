// Programa: Monitoreo de Inversores PH1800
//           Mediante sensores medimos la Entrada, Salida y la granja de baterias de contingencia
//
// Fecha: 18/08/2018
// 
// MUX CHANNELS
// 0  - Voltaje entrada de la red
// 1  - Voltaje salida del inversor
// 2  - Amperaje entrada de la red
// 3  - Amperaje salida del inversor
// 4  - Voltaje Batería 1
// 5  - Voltaje Batería 2
// 6  - Voltaje Batería 3
// 7  - Voltaje Batería 4
// 8  - Voltaje Batería 5
// 9  - Voltaje Batería 6
// 10 - Voltaje Batería 7
// 11 - Voltaje Batería 8
// 12 - Voltaje Batería 9
// 13 - Voltaje Batería 10
// 14 - Voltaje Batería 11
// 15 - Voltaje Batería 12
//
// El LED de la pata D7 muestra el estado de la conexión de WIFI  (Titila rápido = Desconectado | Titila lento = andando y conectado)
//




#include <Wire.h>  // Only needed for Arduino 1.6.5 and earlier
#include "SH1106.h" alis for `#include "SH1106Wire.h"`
#include "SH1106Brzo.h"

#include "images.h"
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

#include <EEPROM.h>
#include <DNSServer.h>            //Local DNS Server used for redirecting all rs to the configuration portal
#include <ESP8266WebServer.h>     //Local WebServer used to serve the configuration portal
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager WiFi Configuration Magic
#include <Ticker.h>               // Manejador del Led de status para la conexión

#include "EmonLib.h"              // Librería para medir energía electrica
EnergyMonitor energyMonitor;

const int muxSIG = A0;            // Pata analógica del MUX
const int muxS0 = D0;             // Pata de control del MUX
const int muxS1 = D1;             // Pata de control del MUX
const int muxS2 = D2;             // Pata de control del MUX
const int muxS3 = D3;             // Pata de control del MUX

const int lTicker = D7;           // LED de testigo del Ticker para saber si esta conectado WiFi


#define BOT_TOKEN_LENGTH 46

char botToken[BOT_TOKEN_LENGTH] = "683684685:AAHB5PXCYebXJlixcET9ux0Kw6tbf68z440";



WiFiClientSecure client;
Ticker ticker;


UniversalTelegramBot *bot;

IPAddress ip;
String router = "Desconectado..";

WiFiManager wifiManager;


int Bot_mtbs = 1000; //mean time between scan messages
long Bot_lasttime;   //last time messages' scan has been done
bool Start = false;


// Variables Auxiliare de Monitoreo de los Inversores
int   VIN = 0;                // Voltaje de entrada       (Volts)
float IIN = 0;                // Corriente de entrada     (Amperes)
int   PIN = 0;                // Potencia de entrada      (Watts)
int   VOUT = 0;               // Voltaje de salida        (Volts)
float IOUT = 0;               // Corriente de salida      (Amperes)
int   POUT = 0;               // Potencia de salida       (Watts)
float VB[13] = {0};           // Voltaje de cada Batería  (Volts)   (empieza en posición 1)       
int   CantBaterias = 12;      // Cantidad de Baterías 



// Initialize the OLED display using brzo_i2c
//  SDA -- D6
//  SCL -- D5
SH1106 display(0x3c, D6, D5);


#define DEMO_DURATION 3000
typedef void (*Demo)(void);
int demoMode = 0;
int counter = 1;


//flag for saving data de WiFiManager
bool shouldSaveConfig = false;



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
    display.drawString(64, 27, String(IIN,1)+"A");

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
    display.drawString(64, 27, String(IOUT,1)+"A");

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
    display.drawString(70, 18, "02 : 13,1V");

    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(Open_Sans_Regular_12);
    display.drawString(5,35, "03 : 12,5V");

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
  Serial.begin(115200);
  
  delay(500);                   // No sacar porque es necesario para esperar que el micro arranque.
  Serial.println();
  Serial.println("Inicializando MICRO.... ");

  pinMode(BUILTIN_LED, OUTPUT);
  ticker.attach(0.2, tick);
  
  pinMode(muxS0, OUTPUT);       // Seteo los pines de control del MUX como salida
  pinMode(muxS1, OUTPUT);       // Seteo los pines de control del MUX como salida
  pinMode(muxS2, OUTPUT);       // Seteo los pines de control del MUX como salida
  pinMode(muxS3, OUTPUT);       // Seteo los pines de control del MUX como salida
  
  pinMode(lTicker, OUTPUT);     // LED de salida del testigo de conexión WiFi

   
  // initialize display
  // 
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
  
  
  EEPROM.begin(BOT_TOKEN_LENGTH);
  
  Serial.print("EEPROM - Recuperando token del BOT : ");
  readBotTokenFromEeprom(0);
  Serial.println(botToken);


  counter = 25;
  drawProgressBarDemo(counter);


  // Fija el tiempo de time out para entrar en Modo de Configuración de Wifi si no se puede conectar
  wifiManager.setConnectTimeout(50);

  // Fija el tiempo que va a quedar prendido el AP y si nadie se conecta podríamos volver a intentar con el ciclo con el SSID  
  wifiManager.setTimeout(60);

  // Fija el tiempo que va a estar levantado el AP esperando por el seteo
  // luego baja el AP y deja que siga la aplicación
  // si no pones esto ojo que salta el Watch Dog que te cuelga la aplicación
  wifiManager.setTimeout(60);                  // Parece identico al que sigue     
  wifiManager.setConfigPortalTimeout(60);      // Tiempo que esperará el portal antes de salir aunque no configures nada


  //exit after config instead of connecting
  //wifiManager.setBreakAfterConfig(true);
  
  // Con esta orden podría borrar los seteos incluso la password y usuario de Wifi en EPROM
  //wifiManager.resetSettings();

  //wifiManager.setAPCallback(configModeCallback);
  wifiManager.setSaveConfigCallback(saveConfigCallback);
    
  //Adding an additional config on the WIFI manager webpage for the bot token
  WiFiManagerParameter custom_bot_id("botid", "Bot Token", botToken, 50);
  wifiManager.addParameter(&custom_bot_id);
  

  //If it fails to connect it will create a TELEGRAM-BOT access point
  wifiManager.autoConnect("AutoConnectAP");
  
  counter = 50;
  drawProgressBarDemo(counter);

 
  
  while (WiFi.status() != WL_CONNECTED){
     
     Serial.print(".");
     if (counter < 100) {
        drawProgressBarDemo(counter);
     }
     else
     {
        ticker.attach(0.2, tick); 
        display.flipScreenVertically();               // Giro de la pantalla
        display.setFont(ArialMT_Plain_10);
        display.setTextAlignment(TEXT_ALIGN_CENTER);
        display.setContrast(255);
        display.clear();
        display.drawString(64, 10, " Configurar WiFi ");
        display.drawString(64, 20, "  AutoConnectAP  ");
        display.drawString(64, 40, " IP: 192.168.4.1 ");
        router = "Desconectado..";
        display.display(); 
        delay(3000);
        Serial.println("Error de conexión WIFI.. reseteando la ESP");
        drawImagewifi("Reiniciando Micro");
        delay(3000);
        ESP.restart();
        delay(5000);
        
     }   
     counter++;
  }


  ip = WiFi.localIP();

  if (WiFi.status() == WL_CONNECTED){
    counter = 100;
    ticker.attach(2, tick); 
    drawProgressBarDemo(counter);
    delay(300);
    drawImagewifi("Internet Conectado!!");
    delay(500);
    router = String(WiFi.SSID()); 
  }
  else {
      router = "Conexión perdida.. !!";
      ticker.attach(0.2, tick); 
      drawImagewifi("Reiniciando Micro");
      delay(3000);
      ESP.restart();
      delay(5000);
  }


  // Guardo el token del Bot si cambio 
  //
  strcpy(botToken, custom_bot_id.getValue());
  if (shouldSaveConfig) {
      writeBotTokenToEeprom(0);
  }

  
  bot = new UniversalTelegramBot(botToken, client);   //Instancio el objeto BOT

}





void loop() {


  // Recupera la conexión si por alguna razón se hubiera caído
  if (WiFi.status() != WL_CONNECTED){

      router = "Conexión perdida.. !!";
      ticker.attach(0.2, tick); 
      drawImagewifi("Reconectando Internet");
      delay(500);
       
      wifiManager.autoConnect("AutoConnectAP");

      if (WiFi.status() == WL_CONNECTED)
      {
            ticker.attach(2, tick); 
            drawImagewifi("Conexión Recuperada!!");
            delay(500);
            router = String(WiFi.SSID());
      }
      else{
          router = "Conexión perdida.. !!";
          ticker.attach(0.2, tick); 
          drawImagewifi("Reiniciando Micro");
          delay(3000);
          ESP.restart();
          delay(3000);        
      }
    
  }


  // Barrida leyendo los sensores
  SetMuxChannel(0);               // MUX CH0 - Sensor Voltaje Entrada de la red 
  VIN  = tensionAC();
  
  SetMuxChannel(1);               // MUX CH1 - Sensor Voltaje Salida del Inversor
  VOUT = tensionAC();
  
  SetMuxChannel(2);               // MUX CH2 - Sensor Amperaje de la entrada de red
  IIN = corrienteAC(2.80);
  PIN = potenciaAC(1, VIN, IIN);

  SetMuxChannel(3);               // MUX CH3 - Sensor Amperaje de la salidsa del Inversor
  IOUT = corrienteAC(2.80);
  POUT = potenciaAC(1, VIN, IOUT);

  tensionBAT(VB, CantBaterias);   // Leo el estado de las baterías conectadas

  imprimesalidamonitorserial();   // Mando el resúmen al Monitor Serial


  // Revisa el tiempo transcurrido para ver si revisa los mensajes del BOT
  //
  if (millis() > Bot_lasttime + Bot_mtbs)  {
    //Serial.println(bot->getUpdates(bot->last_message_received + 1));
    int numNewMessages = bot->getUpdates(bot->last_message_received + 1);
    
    while(numNewMessages) {
      handleNewMessages(numNewMessages);
      numNewMessages = bot->getUpdates(bot->last_message_received + 1);
    }
    Bot_lasttime = millis();

  }


  
  // Comienza el Slide Show de Pantallas que van ciclando
  //
  display.clear();
  demos[demoMode]();
  display.display();

  if (millis() - timeSinceLastModeSwitch > DEMO_DURATION) {
    demoMode = (demoMode + 1)  % demoLength;
    timeSinceLastModeSwitch = millis();
  }


}



void handleNewMessages(int numNewMessages) {
  
  Serial.println(" ");
  Serial.print("Mensajes del BOT recibidos: ");
  Serial.println(String(numNewMessages));

  int supeResponder = 0;
  
  for(int i=0; i<numNewMessages; i++) {
  
    String chat_id = String(bot->messages[i].chat_id);
    String text = bot->messages[i].text;

    text.toLowerCase();             // Paso todo a lower case para comparar y no preocuparme de las mayúsculas

    
    if (text.indexOf("entrad")>=0) {
      String msg = "Datos de Entrada de Red\n";
      msg = msg + "VIN  : "+String(VIN)+ " Volts\n";
      msg = msg + "IIN   : "+String(IIN,2)+ " Amperes\n";
      msg = msg + "PIN  : "+String(PIN)+ " Watts\n";
      bot->sendMessage(chat_id,msg, "");
      supeResponder = 1;
    }
    
    if (text.indexOf("salid")>=0) {
      String msg = "Salida del Inversor\n";
      msg = msg + "VOUT  : "+String(VOUT)+ " Volts\n";
      msg = msg + "IOUT   : "+String(IOUT,2)+ " Amperes\n";
      msg = msg + "POUT  : "+String(POUT)+ " Watts\n";
      bot->sendMessage(chat_id,msg, "");
      supeResponder = 1;
    }


    if (text.indexOf("bater")>=0) {
      String msg = "Estado batería de respaldo\n";
      for(i=1; i<=CantBaterias; i++){
        if(i<10)
        {
          msg = msg +"0"+ String(i) + " : " + String(VB[i],2) + " Volts\n";
        }
        else
        {
          msg = msg + String(i) + " : " + String(VB[i],2) + " Volts\n";
        }
      }
      bot->sendMessage(chat_id,msg, "");
      supeResponder = 1;
    }
    
    
    if (text.indexOf("reinic")>=0) {
        bot->sendMessage(chat_id, "Reiniciando dispositivo..\nEspere un minuto para reconectarse", "Markdown");
        ticker.attach(0.2, tick); 
        router = "Conexión perdida.. !!";
        drawImagewifi("BOT pidió Reiniciar");
        WiFi.disconnect(); 
        delay(3000);
        ESP.restart();
        delay(5000);
    }

    
    if (text.indexOf("star")>=0 || text.indexOf("hol")>=0 || text.indexOf("instr")>=0 || text.indexOf("orden")>=0) {
      String welcome = "Monitor de Inversores PH1800\n\n";
      welcome = welcome + "Entrada  : Parametros de la red\n";
      welcome = welcome + "Salida   : Parametros Inversor\n";
      welcome = welcome + "Baterías : Estado de baterías\n";
      welcome = welcome + "Reinicio : resetear dispositivo\n";
      
      bot->sendMessage(chat_id, welcome, "Markdown");
      supeResponder = 1;
    }

    if (text == "mychatID") {
      bot->sendMessage(chat_id, chat_id, "Markdown");
      supeResponder = 1;
    }
    
  }
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


// Logo que se muestra al inicio
//
void drawImagewifi(String mensaje) {
    // see http://blog.squix.org/2015/05/esp8266-nodemcu-how-to-create-xbm.html
    // on how to create xbm files
    
    display.clear();
    display.drawXbm(34, 7, WiFi_Logo_width, WiFi_Logo_height, WiFi_Logo_bits);

    display.setTextAlignment(TEXT_ALIGN_CENTER);
    display.setFont(Open_Sans_Regular_12);
    display.drawString(64, 45, mensaje);
    
    display.display();
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



// Esta función se ejecuta cunado entra en modo AP
//
void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("Entré en modo Confguración del AP");
  Serial.println(WiFi.softAPIP());
  Serial.println(myWiFiManager->getConfigPortalSSID());
}


// Esta función se ejecuta para identificar cuando hay que grabar la configuración en EEPROM
//
void saveConfigCallback () {
  Serial.println("Seteo en TRUE la variable shouldSaveConfig para guardar parámetros");
  shouldSaveConfig = true;
}



// Lee el Token del BOT
//
void readBotTokenFromEeprom(int offset){
  for(int i = offset; i<BOT_TOKEN_LENGTH; i++ ){
    botToken[i] = EEPROM.read(i);
  }
  EEPROM.commit();
}


// Guarda el Token del BOT
//
void writeBotTokenToEeprom(int offset){
  for(int i = offset; i<BOT_TOKEN_LENGTH; i++ ){
    EEPROM.write(i, botToken[i]);
  }
  EEPROM.commit();
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



// Mide la tensión de alterna (Volts)
//
int tensionAC(){
  int VmaxD = 0, sensor = 0;
  for(int c = 0;c < 800; c ++ ){
    sensor=analogRead(A0);
    if(sensor>VmaxD && sensor > 511)
      VmaxD = sensor;
  }
  int Veffd = VmaxD / sqrt(2);
  int Veff = (((Veffd-420.76)/-90.24)*-210.2)+210.2;
  return Veff;
}


// Mide la corriente de alterna (Amperes)
//
float corrienteAC(float cal){
  float Irms = energyMonitor.calcIrms(1484);
  energyMonitor.current(0, cal);
  if(Irms < 0 || Irms < 1)
    Irms = 0;  
  return Irms; 
}


// Cálculo de Potencia en alterna (Watts)
//
double potenciaAC(int trigger, int I, int V){
  double pot = I * V;
  if(pot < trigger)
    pot = 0;
  return pot;
}



//Calculo la tension de las baterias
//
void tensionBAT(float baterias[], int cantidad){
  
  int ch = 3;
  for(int i = 0; i < 12; i ++)
    baterias[i] = 0;
  for(int i = 0; i < cantidad; i ++){
    delay(10);
    ch ++;
    SetMuxChannel(ch);
    baterias[i+1] = analogRead(A0) * 17.75 / 1024;
  }
}


// Maneja el parpadeo del Led de estado de conexión a Internet (BuiltInLed)
//
void tick()
{
  //toggle state
  int state = digitalRead(lTicker);   
  digitalWrite(lTicker, !state);      
}


// Imprime las salidas de lecturas por el monitor serial
//
void imprimesalidamonitorserial(){

  Serial.println(" ");
  Serial.print(VIN);
  Serial.print("V");
  Serial.print("\t");
  Serial.print(IIN);
  Serial.print("A");
  Serial.print("\t");
  Serial.print(PIN);
  Serial.print("W");
  Serial.print("\t");
  Serial.print("\t");
  Serial.print(VOUT);
  Serial.print("V");
  Serial.print("\t");
  Serial.print(IOUT);
  Serial.print("A");
  Serial.print("\t");
  Serial.print(POUT);
  Serial.print("W");
  Serial.print("\t");

  Serial.print(VB[1]);
  
  Serial.println("V");
}  

