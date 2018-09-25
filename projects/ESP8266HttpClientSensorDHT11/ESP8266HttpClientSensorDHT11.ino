// Programa que escribe en la paginita PHP que armé en la Raspberry PI para subir info de los sensores a un MySQL
// versión 4
// Ejemplo de uso: 
//    
//    Poner en la URL del explorador
//          http://fernandezsanz.dyndns.org:8080/index.php?Sensor=DHT02&Temperatura=33&Humedad=22
//
//    Frecuencia de barrido (1 minuto)
//


#include <Arduino.h>
#include <SimpleDHT.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>

#define USE_SERIAL Serial

ESP8266WiFiMulti WiFiMulti;

int pinDHT11        = 2;                        // pin de entrada de la señal del sensor DHT11

String Sensor     = "DHT02";

int   Estado      = 1;
byte  Temperature = 10;
byte  Humidity    = 10;
int   NivelLuz    = 0;
int   Ingresos    = 0;
int   Egresos     = 0;

int MaxLengthInstruccio = 300;              // Largo máximo que tiene el string de la instrucción a enviar


int   errDHT         = SimpleDHTErrSuccess;      // Atributo de error del DHT11

SimpleDHT11 dht11;                              // Instancio el objeto DHT11


void setup() {

    USE_SERIAL.begin(115200);

    for(uint8_t t = 4; t > 0; t--) {
        USE_SERIAL.printf("[SETUP] WAIT %d...\n", t);
        USE_SERIAL.flush();
        delay(1000);
    }

    WiFiMulti.addAP("WRT1900AC", "4CE9C5C6F6");

    pinMode(pinDHT11, INPUT);

}

void loop() {

    // Leo el sensor de DHT11 y si tira error muestro el código y vuelvo a iniciar el ciclo
    if ((errDHT = dht11.read(pinDHT11, &Temperature, &Humidity, NULL)) != SimpleDHTErrSuccess) 
    {
      USE_SERIAL.print("Error DHT11, err="); 
      USE_SERIAL.print(errDHT); 
    }
  
    // wait for WiFi connection
    if((WiFiMulti.run() == WL_CONNECTED)) {

        HTTPClient http;

        char CadenaInstrucc[MaxLengthInstruccio];
        char SEN[6];
        Sensor.toCharArray(SEN,6);
        
        String Instruccion = "http://fernandezsanz.dyndns.org:8080/index.php?Sensor=";
        Instruccion.concat(SEN);
        Instruccion.concat("&Temperatura=");
        Instruccion.concat(Temperature);
        Instruccion.concat("&Humedad=");
        Instruccion.concat(Humidity);
        Instruccion.concat(" ");
                
        Instruccion.toCharArray(CadenaInstrucc,Instruccion.length());

        USE_SERIAL.print(CadenaInstrucc);

        http.begin(CadenaInstrucc);

        // start connection and send HTTP header
        int httpCode = http.GET();

        // httpCode will be negative on error
        if(httpCode > 0) {
            // HTTP header has been send and Server response header has been handled
            USE_SERIAL.printf("[HTTP] GET... code: %d\n", httpCode);

            // file found at server
            if(httpCode == HTTP_CODE_OK) {
                String payload = http.getString();
                USE_SERIAL.println(payload);
            }
        } else 
        {
            USE_SERIAL.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        }

        http.end();
    }

    delay(60000);
}

