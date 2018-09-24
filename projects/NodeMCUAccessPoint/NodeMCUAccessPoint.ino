

/* Create a WiFi access point and provide a web server on it. */

#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>

/* Set these to your desired credentials. */
const char *ssid = "NodeMCU";
const char *password = "Passw0rd";

ESP8266WebServer server(80);

/* Just a little test message.  Go to http://192.168.4.1 in a web browser
 * connected to this access point to see it.
 */
void handleRoot() {
  
	server.send(200, "text/html", "<br> <h1>Instrucciones</h1> <br> Con /PARAM --> vas a parametros <br> con /SALIR --> No hace nada <br> Sino espere y será atendido <br>");
  //server.send(200, "text/plain", "hello from esp8266!");
}


void handleParam() {
  
  server.send(200, "text/html", "<br> <h1>Parametros de Conexión</h1> <br> SSID=WRT1900AC <br> PWD=22222222 <br>");
  
}



void handleConten() {
  
    char temp[400];
    int sec = millis() / 1000;
    int min = sec / 60;
    int hr = min / 60;

    snprintf ( temp, 400,

      "<!DOCTYPE html>\
       <html>\
        <head>\
          <meta http-equiv='refresh' content='5'/>\
          <title><h1>ESP8266 Demo</h1></title>\
          <style>\
            body { background-color: #ffffff; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
          </style>\
        </head>\
        <body>\
          <h1>Hello from ESP8266!</h1>\
          <p>Uptime: %02d:%02d:%02d</p>\
          <img src=\"/test.svg\" />\
        </body>\
      </html>",
      
          hr, min % 60, sec % 60
        );
        
    server.send ( 200, "text/html", temp );
  
}


void drawGraph() {
  String out = "";
  char temp[100];
  out += "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" width=\"400\" height=\"150\">\n";
  out += "<rect width=\"400\" height=\"150\" fill=\"rgb(250, 230, 210)\" stroke-width=\"1\" stroke=\"rgb(0, 0, 0)\" />\n";
  out += "<g stroke=\"black\">\n";
  int y = rand() % 130;
  for (int x = 10; x < 390; x+= 10) {
    int y2 = rand() % 130;
    sprintf(temp, "<line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" stroke-width=\"1\" />\n", x, 140 - y, x + 10, 140 - y2);
    out += temp;
    y = y2;
  }
  out += "</g>\n</svg>\n";

  server.send ( 200, "image/svg+xml", out);
}


void handleNotFound(){
  
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  
}


void setup() {
	delay(1000);
	Serial.begin(115200);
	Serial.println();
	Serial.println("Configuring access point...");
	
	/* You can remove the password parameter if you want the AP to be open. */
	WiFi.softAP(ssid, password);

	IPAddress myIP = WiFi.softAPIP();
	Serial.print("AP IP address: ");
	Serial.println(myIP);

  /* Inicio el servidor */
	server.on("/", handleRoot);                 // este es el directorio de inicio

  server.on("/param", handleParam);           // directorio de Parametros

  server.on("/conten", handleConten);         // directorio de ejemplo de contenido

  server.on ( "/test.svg", drawGraph );

  server.on("/inline", [](){
    server.send(200, "text/plain", "this works as well");
  });



  server.onNotFound(handleNotFound);
  
	server.begin();
	Serial.println("HTTP server started");
}

void loop() {
	server.handleClient();
}
