#include <ESP8266WiFi.h>                        //Incluye la librería ESP8266WiFi
 
const char* ssid = "-----------";               //Indicamos el nombre de la red WiFi (SSID) a la que queremos conectarnos.
const char* password = "-----------";           //Indicamos la contraseña de de red WiFi
 
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
  WiFiClient client_1;                                 //Inicializamos el cliente (client_1)
 
  Serial.printf("\n[Conectando a %s ... ", host);      //Establecemos la conexión con el servidor
  if (client_1.connect(host, 80))
  {
    Serial.println("conectado]");
 
    Serial.println("[Enviando peticion]");             //Enviamos la petición de datos
    client_1.print(String("GET /") + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "Connection: close\r\n" +
                 "\r\n"
                );
 
    Serial.println("[Respuesta:]");                    //Leemos la respuesta del servidor
    while (client_1.connected())
    {
      if (client_1.available())
      {
        String line = client_1.readStringUntil('\n');
        println line;
      }
    }
    client_1.stop();                                   //Finalizamos la conexión con el servidor
    Serial.println("\n[Desconectado]");
  }
  else
  {
    Serial.println("conexion con el servidor no lograda!]");
    client_1.stop();
  }
  delay(10000);
}
