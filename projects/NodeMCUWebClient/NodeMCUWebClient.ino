#include <ESP8266WiFi.h>                        //Incluye la librería ESP8266WiFi
 
const char* ssid      = "WRT1900AC";            //Indicamos el nombre de la red WiFi (SSID) a la que queremos conectarnos.
const char* password  = "4CE9C5C6F6";           //Indicamos la contraseña de de red WiFi
 
const char* host = "www.vermiip.es";            //Declaramos el servidor de conexión
 


void setup()
{
  Serial.begin(115200);                         //Inicializamos el Puerto Serie
  Serial.println();
 
  Serial.printf("Connecting to %s ", ssid);     //Inicializamos la conexión Wi-Fi en modo Station
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" connected");
}


 
void loop()
{
  WiFiClient ClienteHTTP;                                   //Inicializamos el cliente (ClienteHTTP)
 
  Serial.printf("\n[Conectando a %s ... ", host);           //Establecemos la conexión con el servidor
  if (ClienteHTTP.connect(host, 80))
  {
    Serial.println("conectado]");
 
    Serial.println("[Enviando peticion]");                  //Enviamos la petición de datos
    ClienteHTTP.print(String("GET /") + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "Connection: close\r\n" +
                 "\r\n"
                );
 
    Serial.println("[Respuesta:]");                         //Leemos la respuesta del servidor
    while (ClienteHTTP.connected())
    {
      if (ClienteHTTP.available())
      {
        String line = ClienteHTTP.readStringUntil('\n');
        Serial.println(line);
      }
    }
    ClienteHTTP.stop();                                     //Finalizamos la conexión con el servidor
    Serial.println("\n[Desconectado]");
  }
  else
  {
    Serial.println("conexion con el servidor no lograda!]");
    ClienteHTTP.stop();
  }
  delay(10000);
}
