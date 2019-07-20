/*
 *  This sketch sends data via HTTP GET requests to data.sparkfun.com service.
 *
 *  You need to get streamId and privateKey at data.sparkfun.com and paste them
 *  below. Or just customize this script to talk to other HTTP servers.
 *
 */

#include <WiFi.h>
#define ver 1

const char* ssid     = "WRT1900AC";
const char* password = "4CE9C5C6F6";

const char* host = "srvpoc.eastus.cloudapp.azure.com";
String line = "";
String mac = WiFi.macAddress();

bool checkUpdate(String url)
void setup()
{
    Serial.begin(115200);
    delay(10);

    // We start by connecting to a WiFi network

    Serial.println();
    Serial.println();
    Serial.print("myMAC: ");
    Serial.println(mac);
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

int value = 0;

void loop()
{
    delay(5000);
    ++value;
    int pos = 0;

    Serial.print("connecting to ");
    Serial.println(host);

    // Use WiFiClient class to create TCP connections
    WiFiClient client;
    const int httpPort = 80;
    if (!client.connect(host, httpPort)) {
        Serial.println("connection failed");
        return;
    }

    // We now create a URI for the request
    String url = "/OTAUpdate/config.sys";


    Serial.print("Requesting URL: ");
    Serial.println(url);

    // This will send the request to the server
    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "Connection: close\r\n\r\n");
    unsigned long timeout = millis();
    while (client.available() == 0) {
        if (millis() - timeout > 5000) {
            Serial.println(">>> Client Timeout !");
            client.stop();
            return;
        }
    }

    // Read all the lines of the reply from server and print them to Serial
    while(client.available()) {
        line = client.readStringUntil('\r');
        //Serial.print(line);

    }
    if((pos=line.indexOf(mac))>=0){
      Serial.println();
      String Fname = line.substring(pos+18, line.indexOf('\n',pos+18)); 
      Serial.println("El nombre del archivo es: " + Fname);
      String Sver  = line.substring(pos+19, line.indexOf('.',pos+19));
      Serial.print("Version: ");
      int ver_w = Sver.toInt();
      Serial.println(ver_w);   

      if(ver_w > ver){
        Serial.println("Debo actualizar!!");
      }
      else if(ver_w == ver){
        Serial.println("Estoy en la ultima version!!");  
      }
      else if(ver_w < ver){
        Serial.println("Estoy en una version futura...");  
      }
    }
    else
      line = "";
        

    Serial.println();
    Serial.println("closing connection");
}
