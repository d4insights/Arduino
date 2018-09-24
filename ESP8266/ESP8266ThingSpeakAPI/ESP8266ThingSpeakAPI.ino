// Programa que graba registros en ThingSpeak.com mediante API pra luego explotar la info con la API que la página provee
// Permite hacer gráfico como indicadores en la nube
//
// A intervalos regulares voy a subir a ThingSpeak la temperatura y humedad ambiente de casa para recuperarlo con 
// los Cubitos de Estación Meteorológica
//

#include <ThingSpeak.h>
#include <ESP8266WiFi.h>
#include <SimpleDHT.h>

// Network Parameters
const char* ssid     = "WRT1900AC";
const char* password = "4CE9C5C6F6";

// ThingSpeak information
char thingSpeakAddress[] = "api.thingspeak.com";
unsigned long channelID = 398137;
char* readAPIKey = "MH9VRFLNVE5RREJ5";
char* writeAPIKey = "7LZ08VGNUNUDQ6MJ";
const unsigned long postingInterval = 120L * 1000L;

unsigned int dataFieldOne   = 1;                     // Field to write temperature data
unsigned int dataFieldTwo   = 2;                     // Field to write humidity data
unsigned int dataFieldThree = 3;                     // Field to write elapsed time data

unsigned int aField = 6;                             // Field to hold first constant of the thermistor calibration                
unsigned int bField = 7;                             // Field to hold second constant of the thermistor calibration
unsigned int cField = 8;                             // Field to hold third constant of the thermistor calibration

// Global variables
// These constants are device specific.  You will need to get them from the manufacturer or determine them yourself.
float aConst = 2.25E-02;   
float bConst = -0.003422894649;
float cConst = 0.00001518485044;


// Simple DHT declaraciones y conexionado
//      VCC: 5V or 3V
//      GND: GND
//      DATA: 2
int pinDHT11 = D2;                      // Hay que acordarse de poned la D sino en el ESP8266-12E no te funciona
int errDHT = SimpleDHTErrSuccess; 
SimpleDHT11 dht11;
byte temperature = 0;
byte humidity    = 0;


unsigned long lastConnectionTime = 0;
long lastUpdateTime = 0; 
WiFiClient client;                                  

void setup() {

  pinMode(pinDHT11, INPUT);
  
  Serial.begin(9600);
  Serial.println("Start");
  connectWiFi();
  
  // Read the constants at startup.
  aConst = readTSData( channelID, aField );
  bConst = readTSData( channelID, bField );
  cConst = readTSData( channelID, cField );
}


void loop() {   
    
    // Only update if posting time is exceeded
    if (millis() - lastUpdateTime >=  postingInterval) {
        
        // Ciclo de lectura del DHT11
        if ((errDHT = dht11.read(pinDHT11, &temperature, &humidity, NULL)) != SimpleDHTErrSuccess) 
        {
          Serial.print("Read DHT11 failed, err="); Serial.println(errDHT);delay(2000);
          return;
        }
        
        Serial.print("Lectura OK: ");
        Serial.print((float)temperature); Serial.print(" ºc, ");
        Serial.print((float)humidity); Serial.println(" hr%");
        
        lastUpdateTime = millis();
        
 
        write2TSData( channelID , dataFieldOne , temperature , dataFieldTwo , humidity , dataFieldThree , millis() );      

    delay(1000);


    }
}

int connectWiFi(){
    
    while (WiFi.status() != WL_CONNECTED) {
      
        WiFi.begin( ssid, password );
        delay(2500);
        Serial.println("Connecting to WiFi");
    }
    
    Serial.println( "Connected" );
    ThingSpeak.begin( client );
}

float readTSData( long TSChannel,unsigned int TSField ){
    
  float data =  ThingSpeak.readFloatField( TSChannel, TSField, readAPIKey );
  Serial.println( " Data read from ThingSpeak: " + String( data, 9 ) );
  return data;
  
}

// Use this function if you want to write a single field
int writeTSData( long TSChannel, unsigned int TSField, float data ){
  int  writeSuccess = ThingSpeak.writeField( TSChannel, TSField, data, writeAPIKey ); // Write the data to the channel
  if ( writeSuccess ){
    
    Serial.println( String(data) + " written to Thingspeak." );
    }
    
    return writeSuccess;
}

//use this function if you want multiple fields simultaneously
int write2TSData( long TSChannel, unsigned int TSField1, float field1Data, unsigned int TSField2, long field2Data, unsigned int TSField3, long field3Data ){

  ThingSpeak.setField( TSField1, field1Data );
  ThingSpeak.setField( TSField2, field2Data );
  ThingSpeak.setField( TSField3, field3Data );
   
  int writeSuccess = ThingSpeak.writeFields( TSChannel, writeAPIKey );
  return writeSuccess;
}
