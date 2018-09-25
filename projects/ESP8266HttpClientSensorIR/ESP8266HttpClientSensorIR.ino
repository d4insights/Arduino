// Programa que escribe en el Raspberry Pi 3 para subir info de los sensores a un SQLServer
// versión 1
// Ejemplo de uso: 
//    
//    Poner en la URL del explorador
//    
//


#include <Arduino.h>


#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>

#define USE_SERIAL Serial

ESP8266WiFiMulti WiFiMulti;



String Sensor     = "IR001";

int   Estado      = 1;
byte  Temperature = 0;
byte  Humidity    = 0;
int   Accesos     = 0;

int GPIO0 = 1;
int GPIO1 = 2;


int MaxLengthInstruccio = 300;              // Largo máximo que tiene el string de la instrucción a enviar



void setup() {
   pinMode(2, INPUT);
   pinMode(1, INPUT);

    USE_SERIAL.begin(115200);

    for(uint8_t t = 4; t > 0; t--) {
        USE_SERIAL.printf("[SETUP] WAIT %d...\n", t);
        USE_SERIAL.flush();
        delay(1000);
    }

    WiFiMulti.addAP("WRT1900AC", "4CE9C5C6F6");

}

void loop() {

    if (analogRead(GPIO0) > 700)
    {
        Accesos = 1;  
    }

    if (analogRead(GPIO1) > 700)
    {
        Accesos = -1;  
    }

    
  
    // wait for WiFi connection
    if((WiFiMulti.run() == WL_CONNECTED) && Accesos!=0) {

        HTTPClient http;

        char CadenaInstrucc[MaxLengthInstruccio];
        char SEN[6];
        Sensor.toCharArray(SEN,6);
        
        String Instruccion = "http://fernandezsanz.dyndns.org:8080/index.php?Sensor=";
        Instruccion.concat(SEN);
        Instruccion.concat("&TipoSensor=TempHum");
        Instruccion.concat("&Estado=");
        Instruccion.concat(Estado);
        Instruccion.concat("&Temperatura=");
        Instruccion.concat(Temperature);
        Instruccion.concat("&Humedad=");
        Instruccion.concat(Humidity);
        Instruccion.concat("&Accesos=");
        Instruccion.concat(Accesos);
        Instruccion.concat(" ");
                
        Instruccion.toCharArray(CadenaInstrucc,Instruccion.length());

        USE_SERIAL.println(CadenaInstrucc);


        Accesos = 0;    // lo vuelvo a cero después de haber informado para que no repita el estado

        
        //USE_SERIAL.print("[HTTP] begin...\n");
         
        http.begin(CadenaInstrucc);

        
        //USE_SERIAL.print("[HTTP] GET...\n");

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

    //delay(5000);
}

