/*  

WiFi Interactor
Aplicación que interpreta los mensajes que se reciben desde una página web
Son interpretados por un WEBServer que direcciona ordenes
 
*/
 
#include <SPI.h>
#include <WiFi101.h>
#include <DHT.h>

#define DHTPIN 2                // Define el pin que va a usar para el Sensor de Temperatura
#define DHTTYPE DHT11           // Es el modelo de DHT que vamos a usar (en mi caso es un DHT11) 
DHT dht(DHTPIN, DHTTYPE, 30);   // Instancio un objeto sensor de temperatura
float Temperatura;              // Variable global para medir la temperatura
float Humedad;                  // Variable global para medir la humedad

char ssid[] = "WRT1900AC";
char pass[] = "4CE9C5C6F6";

int status = WL_IDLE_STATUS;
WiFiServer server(80);          // Asignación del servidor web en el puerto 80 de HTTP


int led=6;                      // Pin del led
String cadena="";               // Cadena para almacenar la instrucción


void setup() 
{
  Serial.begin(9600);           // Inicializo la comunicación serial
  pinMode(6, OUTPUT);           // Seteo como OUTPUT el LED

  // Rutina para controlar que esté la placa de WIFI conectada
  // en el MKR 1000 no se necesita porque la placa es on board
  if (WiFi.status() == WL_NO_SHIELD) {
      Serial.println("La placa de WiFi no esta conectada !!!!");
      while (true);          // Esta ciclo es eterno
  }
  

  // Conexión a la red WIFI
  //
  while ( status != WL_CONNECTED) 
  {
    status = WiFi.begin(ssid, pass);        // Conexión vía WPA/WPA2
    delay(1000);                            // espera unos segundos para que se conecte
      if(status == WL_CONNECTED)
      {
        MuestraStatusWIFI();
        LeeTemperaturaHumedad();
      }
      else
      {
        Serial.print("No me pude conectar con la red: ");
        Serial.println(ssid);
      }
  }

  server.begin();                           // Levanta el web server en el puerto 80

  dht.begin();                              // Empieza la comunicación con el DHT11 
}


void loop()
{
  WiFiClient client = server.available();       //Creamos un objeto cliente Web
  
    
  if (client)                                             //Cuando detecte un cliente a través de una petición HTTP
  {
    Serial.println(" ");
    Serial.println("cliente conectado.. esperando instruccion..");  
        
    //Ciclo de captura de caracteres de la petición HTTP    
    //este ciclo se interrumpe con un \n
    cadena="";
    client.flush();                                       // Limpia posibles caracteres no leídos del Buffer
    while (client.connected())                            // indica si el cliente esta conectado
    {
      //client.print("Arduino MKR100 - Ready..");
      if (client.available())                             // indica si aún tengo datos por leer
      {
        char c = client.read();                           // Leemos la petición HTTP carácter por carácter        
        if (c == '\n' || c == '\r' || c == -1 )            // con el enter interpreto se que se termino el comando
        {
             client.print("Arduino MKR100 - Temp:");  // Mando un HTTP request
             client.print(Temperatura);
             client.print("c");
             client.print(" - Request:");
             client.print(cadena);
             client.flush();                              // Limpia posibles caracteres no leídos del Buffer
             client.stop();                               // Cierra la conexión con cliente

             Serial.print(cadena);                        // Mando la cadena definitiva al Monitor serial para chaquear
             break;
        }
        else
        {
              cadena.concat(c);                           //Concateno los caracteres de enttrada para formar la instruccion
              Serial.print(c);
        }
  
        AnalizarInstruccionRecibida();                    // Acá analizo la instrucción y derivo las acciones a ejecutar
      }
      else
      {
         // Serial.println("Se acabaron los datos..");
      }
     
    }
    Serial.println("Me desconecte..");
    client.flush();                                 // Limpia posibles caracteres no leídos del Buffer
    client.stop();                                  // Cierra la conexión con cliente                                
  }
  else
  {
    //Serial.println('.');  
  }
}



// Analizar la cadena para interpretar la instrucción que me llego via HTTP
//
void AnalizarInstruccionRecibida()
{
   int posicion=cadena.indexOf("INSTRUCCION=");          //Guardamos la posición de la instancia "INSTRUCCION=" a la variable 'posicion'
   if(cadena.substring(posicion)=="INSTRUCCION=LED-ON")   
   {
      digitalWrite(led,HIGH);
   }
       
   if(cadena.substring(posicion)=="INSTRUCCION=LED-OFF")  
   {
      digitalWrite(led,LOW);
   }

   if(cadena.substring(posicion)=="INSTRUCCION=TOMAR-TEMPERATURA")  
   {
      LeeTemperaturaHumedad();
   }

   if(cadena.substring(posicion)=="INSTRUCCION=TOMAR-HUMEDAD")  
   {
      LeeTemperaturaHumedad();
   }
}


// Muestra el estado de conexion a la red WIFI
//
void MuestraStatusWIFI() 
{
  // Estado de Conexión
  Serial.print("Conectado a la red: ");
  Serial.println(WiFi.SSID());
  
  // WiFi IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // Received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("Signal Strength (RSSI): ");
  Serial.print(rssi);
  Serial.println("dBm");
}


// Lectura de la Temperratura y Humedad
//
void LeeTemperaturaHumedad()
{
  
  Temperatura = dht.readTemperature();
  Humedad     = dht.readHumidity();

  if(isnan(Temperatura) || isnan(Humedad))
  {
    Serial.println();
    Serial.println("--------------------------");
    Serial.println("Error de Lectura del DHT11");
    Serial.println("--------------------------");     
  }
  else
  {
    Serial.println();
    Serial.println("--------------------------");
    Serial.print("Temperatura: ");
    Serial.print(Temperatura);
    Serial.println(" C");
    Serial.print("Humedad    : ");
    Serial.print(Humedad);
    Serial.println(" %");
    Serial.println("--------------------------");  
  }
}


