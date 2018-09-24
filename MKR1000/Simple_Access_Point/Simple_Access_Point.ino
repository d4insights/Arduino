#include <WiFi101.h>


WiFiServer server(80);
char ssid[] = "MKR1000";
int status = WL_IDLE_STATUS;
String HTTP_req;
boolean readingData = false;
String data = "";

void setup() {
  delay(1000);
  Serial.begin(9600);
  Serial.print("Creating access point named: ");
  Serial.println(ssid);
  if (WiFi.beginAP(ssid) != WL_CONNECTED) {
    Serial.println("Creating access point failed");
    while (true);
  }
  server.begin();
  printStatus();
}

void loop() {
  WiFiClient client = server.available();
  if (client) {
    Serial.println("New Client");
    String line = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.print(c);
        if (c=='?') readingData = true;
        if (readingData) {
          if (c==',') {
            Serial.print("Data: ");
            Serial.println(data);
            readingData = false;
            client.stop();
          }
          else if (c!='?') {
            data += c;
          }
        }
        else if (c == '\n') {
          if (line.length() == 0) {
              client.println("HTTP/1.1 200 OK");
              client.println("Content-type:text/html");
              client.println();
              client.println("<html>");
              client.println("<body>");
              client.print("Data:");
              client.print("<input id=\"data\"/><br>");
              client.print("<button type=\"button\" onclick=\"SendText()\">Enter</button>");
              client.println("</body>");
              client.println("<script>");
              client.println("var network = document.querySelector('#data');");
              client.println("function SendText() {");
              client.println("nocache=\"&nocache=\" + Math.random() * 1000000;");
              client.println("var request =new XMLHttpRequest();");
              client.println("netText = \"&txt=\" + \"?\" + data.value + \",&end=end\";");
              client.println("request.open(\"GET\", \"ajax_inputs\" + netText + nocache, true);");
              client.println("request.send(null)");
              client.println("name.value=''}");
              client.println("</script>");
              client.println("</html>");
              client.println();
              break;
          }
          else {
            line = "";
          }
        }
        else if (c != '\r') {
          line += c;
        }
      }
    }
    client.stop();
    Serial.println("Client Disconnected");
  }
}

void printStatus() {
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
  Serial.print("To connect to the access point, join the network and open a browser to http://");
  Serial.println(ip);
}

