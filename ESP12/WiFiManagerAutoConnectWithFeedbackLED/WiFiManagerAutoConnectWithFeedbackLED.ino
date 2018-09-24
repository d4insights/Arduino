#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino

#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager

//for LED status
#include <Ticker.h>
Ticker ticker;




void setup() {

  // put your setup code here, to run once:
  Serial.begin(115200);
  
  //set led pin as output
  pinMode(BUILTIN_LED, OUTPUT);

  // start ticker with 0.9 because we start in AP mode and try to connect
  ticker.attach(0.9, tick);

  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;
  
 
  wifiManager.setDebugOutput(true);
  //wifiManager.setConnectTimeout(60);
  
  //reset settings - for testing
  //wifiManager.resetSettings();

  
  //set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
  //wifiManager.setAPCallback(configModeCallback);

  //fetches ssid and pass and tries to connect
  //if it does not connect it starts an access point with the specified name
  //here  "AutoConnectAP"
  //and goes into a blocking loop awaiting configuration
  wifiManager.setConnectTimeout(180);
  ticker.attach(0.2, tick);
  if (!wifiManager.autoConnect()) {
    Serial.println("failed to connect and hit timeout");
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(1000);
  }

  //if you get here you have connected to the WiFi
  Serial.println("connected...yeey :) ");
  Serial.print(F("SSID:"));Serial.println(WiFi.SSID());
  Serial.print(F("PWD:"));Serial.println(WiFi.psk());

  ticker.detach();
  //keep LED on
  digitalWrite(BUILTIN_LED, LOW);

  
  
}

void loop() {
  // put your main code here, to run repeatedly:

}



void tick()
{
  //toggle state
  int state = digitalRead(BUILTIN_LED);  // get the current state of GPIO1 pin
  digitalWrite(BUILTIN_LED, !state);     // set pin to the opposite state
}

