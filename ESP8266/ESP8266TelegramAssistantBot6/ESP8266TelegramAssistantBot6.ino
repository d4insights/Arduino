//
//  TELEGRAM BOT - WiFi Manager- 
//  Bot que responde a ordenes predeconfiguradas
//  se conecta a un router y guarda la configuración de WiFi y el Key de Bot de Telegram en EEPROM
//  Los registros sensados los guarda en la base de datos MySql utilizando la siguiente instrucción WEB.
//
//  Poner en la URL del explorador
//          http://fernandezsanz.dyndns.org:8080/index.php?Sensor=DHT02&Temperatura=33&Humedad=22
//
// 
//
//  Fecha 02/07/2018
//



#include <ESP8266WiFi.h>
#include <UniversalTelegramBot.h>

#include <EEPROM.h>

#include <DNSServer.h>            // Local DNS Server used for redirecting all clients to the configuration portal
#include <ESP8266WebServer.h>     // Local WebServer used to serve the configuration portal
#include <ESP8266HTTPClient.h>    // Cliente para guardar registros en la base de datos
#include <WiFiManager.h>          // https://github.com/tzapu/WiFiManager WiFi Configuration Magic
#include <Ticker.h>               // Manejador del Led de status para la conexión
#include "DHTesp.h"               // Sensor DHT22 digital de temperatura y humedad (sin encapsulado) 

WiFiManager wifiManager;
WiFiClientSecure client;
Ticker ticker;
DHTesp dht;


String DHTName = "DHT02";       // ID del DHT22 en la Base de Datos
String CHTName = "CB002";       // ID del ChatBot en la Base de Datos



#define BOT_TOKEN_LENGTH 46

const int PinAccionador   = D5;    // Pin de un accionador de prueba (Led o señal de prueba)
const int pinDHT11        = D2;    // Pin de entrada de la señal del sensor DHT22 (BLANCO)

// Token del BOT de Telegram. Si no lo pones de todas formas te permite levantarlo desde el portal 
char botToken[BOT_TOKEN_LENGTH] = "511137579:AAGJuEQLaFrqVOJJyjHCtr_FT71Ms4Ye0KM";


UniversalTelegramBot *bot;
int  ledStatus = 0;

int  Bot_mtbs = 1000;         // Retardo del BOT - scanea mensajes cada 1 segundo
long Bot_lasttime;            // Auxiliar de última lectua del BOT
int supeResponder = 0;        // Flag para saber si el Bot encontró un palabra clave para contestar.


int  DHT22_mtbs = 60000;        // Retardo de lecturas del DHT11
long DHT22_lasttime;            // Auxiliar de última lectua del DHT11

float  Temperature = 0;
float  Humidity    = 0;


bool shouldSaveConfig = false;          //flag for saving data



void setup() {

    
  // APAGAR EN MODO PRODUCTIVO
  Serial.begin(115200);

  pinMode(BUILTIN_LED, OUTPUT);
  ticker.attach(0.2, tick);

  dht.setup(D2, DHTesp::DHT22);    // Configuro el DHT22 en pata D2 del ESP

  delay(1000);        //NO SACAR PORQUE HAY QUE DARLE TIEMPO A QUE TODOS LOS OBJETOS INSTANCIEN



  String thisBoard= ARDUINO_BOARD;
  Serial.print("Iniciando Dispositivo: ");
  Serial.println(thisBoard);  
  Serial.println("");
  
  EEPROM.begin(BOT_TOKEN_LENGTH);

  
  pinMode(PinAccionador, OUTPUT);          // initialize digital PinAccionador as an output.
  digitalWrite(PinAccionador, LOW);        // initialize pin as off

  Serial.println("Iniciando Conexión y Telegram BOT");
  Serial.println("");

  Serial.print("Recuperando token del BOT : ");
  readBotTokenFromEeprom(0);
  Serial.println(botToken);
  Serial.println("");

  
  // Fija la modalidad verbose de WiFi Manager para que saque por el serial mas mensajes
  // APAGAR EN MODO PRODUCTIVO
  //wifiManager.setDebugOutput(true);
  
  // Fija el tiempo de time out para entrar en Modo de Configuración de Wifi si no se puede conectar
  wifiManager.setConnectTimeout(50);

  // Fija el tiempo que va a quedar prendido el AP y si nadie se conecta podríamos volver a intentar con el ciclo con el SSID  
  wifiManager.setTimeout(60);

  //exit after config instead of connecting
  //wifiManager.setBreakAfterConfig(true);
  
  // Con esta orden podría borrar los seteos incluso la password y usuario de Wifi en EPROM
  // wifiManager.resetSettings();

  wifiManager.setSaveConfigCallback(saveConfigCallback);
  
  //Adding an additional config on the WIFI manager webpage for the bot token
  WiFiManagerParameter custom_bot_id("botid", "Bot Token", botToken, 50);
  wifiManager.addParameter(&custom_bot_id);
  

  //If it fails to connect it will create a TELEGRAM-BOT access point
  if (!wifiManager.autoConnect("AssistantBot"))
  {
    Serial.println("Error de conexión WIFI.. reseteando la ESP");
    delay(3000);
    ESP.restart();
    delay(5000);
  }


  strcpy(botToken, custom_bot_id.getValue());
  if (shouldSaveConfig) {
    writeBotTokenToEeprom(0);
  }

  bot = new UniversalTelegramBot(botToken, client);
  
  Serial.println("");
  Serial.println("Conexión Internet establecida");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  IPAddress ip = WiFi.localIP();
  Serial.println(ip);
  
  digitalWrite(PinAccionador, LOW);
}



void loop() {

  ESP.wdtFeed();

 
  // Reviso si perdí la conexión
  //
  if (WiFi.status() != WL_CONNECTED) 
  {
    ticker.attach(0.2, tick);
    Serial.println("Conexión WiFi perdida.. Reconectando");
    if (!wifiManager.autoConnect("AssistantBot"))
    {
      Serial.println("Error de conexión WIFI.. reseteando la ESP");
      delay(3000);
      ESP.restart();
      delay(5000);
    }
  }
  else
  {
     ticker.attach(2, tick); 
  }


  
  // Leo el sensor de DHT22
  //
  if (millis() > DHT22_lasttime + DHT22_mtbs)  
    LeerDHT22();


  // Busca nuevos mensajes recibidos por el Bot de Telegram
  //
  if (millis() > Bot_lasttime + Bot_mtbs)  
    RecuperaNuevosMensajes();

  
}




void RecuperaNuevosMensajes()
{
    int numNewMessages = bot->getUpdates(bot->last_message_received + 1);
    while(numNewMessages) 
    {
      Serial.println("Mensaje recibido.. ");
      handleNewMessages(numNewMessages);
      numNewMessages = bot->getUpdates(bot->last_message_received + 1);
      
    }
    Bot_lasttime = millis();
}



void handleDataBase(String sensor, int estado, float Temperature, float Humidity, int nLuz, int entradas, int salidas, int movimiento)
{


  HTTPClient http;
    
  char CadenaInstrucc[500];           // OJO QUE SI NO TE ALCANZA LA DIMENSION PARA GUARDAR EL STRING EL PROGRAMA EXPLOTA POR EL WATCHDOG DE LA ESP

  estado = ledStatus;

  String Instruccion = "http://fernandezsanz.dyndns.org:8080/index.php";
  Instruccion.concat("?Sensor=");
  Instruccion.concat(sensor);
  Instruccion.concat("&Estado=");
  Instruccion.concat(ledStatus);                //Acá forcé el led de estado del accionador para jugar
  Instruccion.concat("&Ingresos=");
  Instruccion.concat(entradas);
  Instruccion.concat("&Egresos=");
  Instruccion.concat(salidas);
  Instruccion.concat("&Temperatura=");
  Instruccion.concat(String(Temperature,2));
  Instruccion.concat("&Humedad=");
  Instruccion.concat(String(Humidity,2));
  Instruccion.concat("&Movimiento=");
  Instruccion.concat(movimiento);
  Instruccion.concat("&NivelLuz=");
  Instruccion.concat(nLuz);
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
}


// Manejador de los mensajes del BOT de Telegram
//
void handleNewMessages(int numNewMessages) {
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));
  
  for(int i=0; i<numNewMessages; i++) {

    supeResponder = 0;
    handleDataBase(CHTName, 0, 0, 0, 0, 0, 0, 1);         // Cargo un registro en el histórico de la Base de Datos
    
    String chat_id = String(bot->messages[i].chat_id);
    String text = bot->messages[i].text;
  
    text.toLowerCase();

    keywordyRespuesta(chat_id,  text, "hola", "Hola, soy ["+CHTName+"] decime en que puedo ayudarte.\n\nQuerés conocer la temperatura o la humedad? \n o quizá encender un dispositivo? ");
     
    keywordyRespuesta(chat_id,  text, "temperatura", "En este momento la temperatura ambiente de ["+CHTName+"] es de : " + String(Temperature) + " grados.");
    keywordyRespuesta(chat_id,  text, "humedad", "La humedad relativa ambiente de ["+CHTName+"] es de: " + String(Humidity) + "%.");

    keywordyRespuesta(chat_id,  text, "diego", "A ese Diego lo conozco.. me parece!! \nQué estas necesitando ?"); 
      
    keywordyRespuesta(chat_id,  text, "estado", "En este momento el dispositivo ["+CHTName+"] reporta valor en: "+ String(ledStatus) + ".");

    keywordyRespuesta(chat_id,  text, "chau", "hasta pronto!!. Cualquier cosa me avisas. Saludos!!");
    keywordyRespuesta(chat_id,  text, "gracias", "de nada, avisame si necesitas algo mas");

    keywordyRespuesta(chat_id,  text, "qué haces?", "sé hacer varias cosas, consultá el help cualquier cosita");
    keywordyRespuesta(chat_id,  text, "qué sabes?", "sé unas cuantas cosas, consultá el help y preguntame nomás");
    keywordyRespuesta(chat_id,  text, "hacer", "hacer por hacer,  sé hacer varias cosas, consultá el help");    
    keywordyRespuesta(chat_id,  text, "sabes", "sé de todo un poco, pero vos sé mas específico con lo que necesitas");
    keywordyRespuesta(chat_id,  text, "sos", "yo... en realidad soy un robot, en qué te puedo ayudar?, querés que te llame? ");
    keywordyRespuesta(chat_id,  text, "ayer", "tengo poca memoria, de ayer no me acuerdo nada. \nSi queres mas data conectate al Tablero de Contro.");
    keywordyRespuesta(chat_id,  text, "mañana", "Soy un Bot no un adivino, de mañana todavía no sé nada.. jaja !!");    
    keywordyRespuesta(chat_id,  text, "bueno" ,"a qué te referís con bueno, explicame mejor que necesitar, por favor");    
    keywordyRespuesta(chat_id,  text, "contact" ,"Si queres charlar personalmente, llamame al +54 11 6757-7019 \no entrá en mi página web:\nhttps://www.d4insights.com \nte espero !!!"); 
    keywordyRespuesta(chat_id,  text, "llama" ,"Si queres hablar conmigo, llamame al +54 11 6757-7019 \no entrá en mi página web:\nhttps://www.d4insights.com ");     
    
    if (text.indexOf("encender")>=0) 
    {
      ledStatus = 1;
      digitalWrite(PinAccionador, HIGH);    // turn the LED off (LOW is the voltage level)
      bot->sendMessage(chat_id, "ya encendí el dispositivo ["+CHTName+"] ... \nSe te ofrece algo mas? \nEstado - Led is ON", "");
      supeResponder = 1;
    }
    
    if (text.indexOf("apagar")>=0) 
    {
      ledStatus = 0;
      digitalWrite(PinAccionador, LOW);    // turn the LED off (LOW is the voltage level)
      bot->sendMessage(chat_id, "listo, ["+CHTName+"] ya esta apagado. \nLed is OFF \nQué hacemos ahora?", "");
      supeResponder = 1;
    }
    
    if (text.indexOf("status")>=0) 
    {
      if(ledStatus)
      {
        bot->sendMessage(chat_id, "El dispositivo["+CHTName+"] esta ENCENDIDO", "");
      } 
      else 
      {
        bot->sendMessage(chat_id, "El dispositivo ["+CHTName+"] esta APAGADO \n Si lo quiere prender, solo indiqueme encender?", "");
      }
      supeResponder = 1;
    }


    if (text.indexOf("help")>=0 || text.indexOf("ayuda")>=0 ) 
    {
      String msg = "Ayuda del AssistantBOT \n";
      msg = msg + "encender  - Enciende el accionador TEST \n";
      msg = msg + "apagar    - Apaga el accioandor TEST\n";
      msg = msg + "status    - Parea conocer status Accionador\n";
      msg = msg + "restart   - Iniciar nuevamente Assistant BOT\n";
      msg = msg + "help      - Para obtener mas ayuda\n";
      msg = msg + "\n";
      bot->sendMessage(chat_id, msg, "Markdown");
      supeResponder = 1;
    }

    
    if (text.indexOf("start")>=0) 
    {
      String msg = "Bienvenido al AssistantBOT ["+CHTName+"] \n";
      msg = msg + "Asistente personal de ejemplo\n";
      msg = msg + "Sensa información con pequeños\n";
      msg = msg + "almacena información en una BD\n";
      msg = msg + "es capaz de responder preguntas\n";
      msg = msg + "\n";
      msg = msg + "help   - Para ver comandos\n";
      msg = msg + "\n";
      bot->sendMessage(chat_id, msg, "Markdown");
      supeResponder = 1;
    }

    // Tratamiento para las keywords que no conocemos aún
    //
    if (supeResponder == 0)
    {
         bot->sendMessage(chat_id, "No entiendo a qué te referís, me explicas un poco mas por favor?\n\nCon \help podrás ver mis comandos directos \n o charlemos en lenguaje natural si te parece mejor.", "Markdown");
    }
    
  }
}



// Se fija si el texto contiene la keyword que busco y contesta en consecuencia
//
void keywordyRespuesta(String chat_id,String text, String clave, String respuesta)
{    
    
    int pos=text.indexOf(clave);
    
    if (pos>=0)
    {
           supeResponder = 1;
           bot->sendMessage(chat_id, respuesta, "");
    }      
}



//callback notifying us of the need to save config
//
void saveConfigCallback () 
{
  Serial.println("Should save config");
  shouldSaveConfig = true;
}


// Lee el token del Bot que guarde en la EEProm
//
void readBotTokenFromEeprom(int offset){
  for(int i = offset; i<BOT_TOKEN_LENGTH; i++ ){
    botToken[i] = EEPROM.read(i);
  }
  EEPROM.commit();
}


// Guarda el token del Bot en la EEProm
//
void writeBotTokenToEeprom(int offset){
  for(int i = offset; i<BOT_TOKEN_LENGTH; i++ ){
    EEPROM.write(i, botToken[i]);
  }
  EEPROM.commit();
}



// Maneja el parpadeo del Led de estado de conexión a Internet (BuiltInLed)
//
void tick()
{
  //toggle state
  int state = digitalRead(BUILTIN_LED);   
  digitalWrite(BUILTIN_LED, !state);      
}



// Lectura del DHT22 
void LeerDHT22()
{
    delay(dht.getMinimumSamplingPeriod());
    Humidity = dht.getHumidity();
    Temperature = dht.getTemperature();

    handleDataBase(DHTName, 0, Temperature, Humidity, 0, 0, 0, 0);
    DHT22_lasttime = millis();
}


