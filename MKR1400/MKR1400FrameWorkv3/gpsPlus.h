/*
 *   GPS - Devuelve las coordenadas de satélite y el reloj
 *   
 *   Conexión     MKR1400         GY-NE06MV2 
 *              RX - PIN 13           TX
 *              TX - PIN 14           RX
 *         
 *   El VCC del GPS va a 5v, pero los datos van a 3v3      
 * 
*/

#include <TinyGPS++.h>

static const uint32_t GPSBaud = 9600;       // El GPS se conecta Hardware Serial a 9600 baudios


TinyGPSPlus gps;




// Muestra por pantalla los resultados de la locación, Fecha y hora del GPS FISICO
//
void gpsDisplayInfo()
{
  Serial.print(F("Location: ")); 
  if (gps.location.isValid())
  {
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(","));
    Serial.print(gps.location.lng(), 6);
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.print(F("  Date/Time: "));
  if (gps.date.isValid())
  {
    Serial.print(gps.date.month());
    Serial.print(F("/"));
    Serial.print(gps.date.day());
    Serial.print(F("/"));
    Serial.print(gps.date.year());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.print(F(" "));
  if (gps.time.isValid())
  {
    if (gps.time.hour() < 10) Serial.print(F("0"));
    Serial.print(gps.time.hour());
    Serial.print(F(":"));
    if (gps.time.minute() < 10) Serial.print(F("0"));
    Serial.print(gps.time.minute());
    Serial.print(F(":"));
    if (gps.time.second() < 10) Serial.print(F("0"));
    Serial.print(gps.time.second());
    Serial.print(F("."));
    if (gps.time.centisecond() < 10) Serial.print(F("0"));
    Serial.print(gps.time.centisecond());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.println();
}



void gpsRead(){
  while(Serial1.available() > 0){
    if (gps.encode(Serial1.read())){
      if(gps.location.isValid()){
         //gpsDisplayInfo();
         myLatitud  = String(gps.location.lat(), 6);
         myLongitud = String(gps.location.lng(), 6);
         myLocation = myLatitud + ", " + myLongitud;
      }  
    }
  }
  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));
    
  }  
  
}
