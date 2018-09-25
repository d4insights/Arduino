/*
  aRestHandlerWIFI:
  Aplicación para el manejo de un MKR1000 utilizando WebServices vía WIFI
  Utiliza el DHT11 para leer la temperatura y humedad 
  
  SensorLuz puerto A0
  Buzzer    puerto 5
  External  puerto 6
  DHT11     puerto 2
            VCC: 5V or 3V
            GND: GND
            DATA: 2
  
  Expone las variables:
      Temperature       fernandezsanz.dyndns.org:15x/temperature
      humidity          fernandezsanz.dyndns.org:15x/humidity
      ID                fernandezsanz.dyndns.org:15x/id
      name              fernandezsanz.dyndns.org:15x/name
      external          fernandezsanz.dyndns.org:15x/extstate


  Expone la función:
      EXT ON            fernandezsanz.dyndns.org:15x/externalCtrl?params=1
      EXT OFF           fernandezsanz.dyndns.org:15x/externalCtrl?params=0
      BUZZER ON         fernandezsanz.dyndns.org:15x/buzzerCtrl

  
  Diego Fernandez Sanz
  13/10/2016
*/


#include <SPI.h>
#include <WiFi101.h>
#include <aREST.h> 
#include <DHT.h>

#define DHTPIN 2                // Pin donde voy a poner el data del DHT11
#define DHTTYPE DHT11           // DHT 11
DHT dht(DHTPIN, DHTTYPE);       // Instancio el DHT11

// Status de la conexión
int status = WL_IDLE_STATUS;

// Creo la instancia de aREST 
aREST rest = aREST();

// Parámetros de conexión WiFi 
char ssid[]     = "WRT1900AC";
char password[] = "4CE9C5C6F6";


// Definición del puerto que atederemos
#define LISTEN_PORT           80

// Creo la instancia del server que va a estar escuchando
WiFiServer server(LISTEN_PORT);

// Variables que vamos a exponer en la API
float   temperature;
float   humidity;
int     external;
int     luz;
int     cautonomo;
int     timerexternalON;
int     timerexternalOFF;
int     tiempo;
int     umbralHumedad;

// Funciones que vamos a exponer en la API
int externalCtrl(String command);
int buzzerControl(String command);

void setup(void)
{

  Serial.begin(115200);

  pinMode(6, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(A0, INPUT);

  temperature       = 0;
  humidity          = 0;
  external          = 0;
  luz               = 0;
  cautonomo         = 1;
  timerexternalON   = 0;
  timerexternalOFF  = 0;
  tiempo            = 10000;
  umbralHumedad     = 50;
  
  // Seteo el ID y NOMBRE del dispositivo (El ID puede tener 6 caracteres como máximo)
  rest.set_id("Two");
  rest.set_name("mkr1000");

  // Definición de variables expuestas a la API
  rest.variable("temperature",&temperature);
  rest.variable("humidity",&humidity);
  rest.variable("external",&external);
  rest.variable("luz",&luz);
  rest.variable("cautonomo",&cautonomo);

  // Definición de funciones expuestas a la API
  rest.function("externalCtrl",externalCtrl);
  rest.function("buzzerCtrl",buzzerCtrl);

  // Conexión a la red WiFi
  while (status != WL_CONNECTED) {
    Serial.print("Conectando a SSID: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, password);

    // Espero 10 segundos que el router responda
    delay(10000);
  }
  Serial.println("WiFi conectado");
  
  // Nivel de señal de WiFi
  long rssi = WiFi.RSSI();
  Serial.print("Senal de WiFi(RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");

  // Inicio el Server que empezará a escuchar
  server.begin();
  Serial.println("Server disponible para recibir peticiones..");

  // Dirección IP interna que le asignó el router
  IPAddress ip = WiFi.localIP();
  Serial.print("IP: ");
  Serial.println(ip);

  // Inicio el DHT11
  dht.begin();
}

void loop() {
  // Instancia el Objeto Cliente REST Handler
  WiFiClient client = server.available();
  
  // Actualizo las variables de ambiente
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();
  luz = ReadNivelLuz();

/*

  // Si está prendida la variable de Control Autónomo y no venía prendido el external.. lo prendo
  if(cautonomo==1 && humidity>umbralHumedad)
  {
    if(timerexternalON==0) 
    {
      externalCtrl("1");
      timerexternalON=1;
    }
    
    timerexternalON++;  
    timerexternalOFF++;
   
    if(timerexternalON>tiempo) 
    {
      externalCtrl("0");
      timerexternalOFF=0;
      
    }

    if(timerexternalOFF>tiempo) 
    {
      externalCtrl("1");
      timerexternalON=0;
      
    }
    
    
  }
  */
  
  
  rest.handle(client);
}

// Función DISP EXTERNO - expuesta en la API
int externalCtrl(String command) 
{
    int state = command.toInt();
    digitalWrite(6,state);
    
    external = state; 
    
    return state;
}


// Función BUZZER - expuesta en la API
int buzzerCtrl(String command)
{
    digitalWrite(5, HIGH);
    delay(300);
    digitalWrite(5, LOW);
    delay(100);
    digitalWrite(5, HIGH);
    delay(300);
    digitalWrite(5, LOW);
    delay(100);
    digitalWrite(5, HIGH);
    delay(300);
    digitalWrite(5, LOW);
    
    return 1;
}


// Función LUZ - expuesta en la API
int ReadNivelLuz()
{
   int lux = analogRead(A0);
   return lux;
}
