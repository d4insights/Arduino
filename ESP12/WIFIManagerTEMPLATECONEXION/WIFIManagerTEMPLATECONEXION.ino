#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino

//needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager

//for LED status
#include <Ticker.h>
Ticker ticker;

void tick()
{
  //toggle state
  int state = digitalRead(BUILTIN_LED);  // get the current state of GPIO1 pin
  digitalWrite(BUILTIN_LED, !state);     // set pin to the opposite state
}

//gets called when WiFiManager enters configuration mode
void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  //if you used auto generated SSID, print it
  Serial.println(myWiFiManager->getConfigPortalSSID());
  //entered config mode, make led toggle faster
  ticker.attach(0.2, tick);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  
  //set led pin as output
  pinMode(BUILTIN_LED, OUTPUT);
  pinMode(D8, OUTPUT);
  // start ticker with 0.5 because we start in AP mode and try to connect
  ticker.attach(0.6, tick);

  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;
  //reset settings - for testing
  //wifiManager.resetSettings();

   wifiManager.setTimeout(60);                  // Parece identico al que sigue     
   wifiManager.setConfigPortalTimeout(60);      // Tiempo que esperará el portal antes de salir aunque no configures nada

  //set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
  wifiManager.setAPCallback(configModeCallback);

  //fetches ssid and pass and tries to connect
  //if it does not connect it starts an access point with the specified name
  //here  "AutoConnectAP"
  //and goes into a blocking loop awaiting configuration
  if (!wifiManager.autoConnect()) {
    Serial.println("failed to connect and hit timeout");
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(1000);
  }

  //if you get here you have connected to the WiFi
  Serial.println("Me conecte desde la rutina de setup ...  yeey :)");
  
  ticker.attach(2, tick);
  //ticker.detach();
  //keep LED on
  //digitalWrite(BUILTIN_LED, LOW);
}

void loop() {


  if (WiFi.status() != WL_CONNECTED){
    Serial.println("Perdí la conexión la puta madre !!!!");
    ticker.attach(0.2, tick);
    delay(1000);
    digitalWrite(D8, HIGH);
    
  }
  else{
    digitalWrite(D8, LOW);
    Serial.println("Sigo conectadooooo...");
    ticker.attach(2, tick);
    delay(2000);


    // ACA VAN TODAS LAS ACCIONES QUE REQUIERAN CONEXION EXTERNA ASINCRONICA
    // SI EL PIEDO LA CONEXION Y ME CONECTO AL BOT O A LA BASE DE DATOS EL PROCESO SE CAE POR 
    // TIMEOUT Y EL ESP VA A SALIR POR UN ERROR DE TIMEOUT DEL WATCH DOG QUE DEJA COLGADA 
    // LA APLICACION
    

    
  }

}
