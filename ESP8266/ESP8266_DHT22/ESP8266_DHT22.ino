//
// Programa de ejemplo de uso de DHT22 (digital) sin la resistencia de PullUp
// ESTA ES LA VERSION QUE VIENE SIN ENCAPSULAR EN EL MODULITO CON PLACA.. ES EL BASICO DE LA CAJITA BLANCA
// esta optimizado para funcionar con la ESP8266-12e
// para que ande hay que ponerle las resistencias de PullUp a la para SDA
//
// PinOut (visto de frente de izquierda a derecha)
// 1 - VCC
// 2 - D2     SDA (con pullup)
// 3 - Nada
// 4 - GND
//


#include "DHTesp.h"

#ifdef ESP32
#pragma message(THIS EXAMPLE IS FOR ESP8266 ONLY!)
#error Select ESP8266 board.
#endif

DHTesp dht;

void setup()
{
  Serial.begin(115200);
  Serial.println();
  Serial.println("Status\tHumidity (%)\tTemperature (C)\t(F)\tHeatIndex (C)\t(F)");
  String thisBoard= ARDUINO_BOARD;
  Serial.println(thisBoard);

  // Autodetect is not working reliable, don't use the following line
  // dht.setup(17);
  // use this instead: 
  dht.setup(D2, DHTesp::DHT22); // Connect DHT sensor to GPIO 17
}

void loop()
{
  delay(dht.getMinimumSamplingPeriod());

  float humidity = dht.getHumidity();
  float temperature = dht.getTemperature();

  Serial.print(dht.getStatusString());
  Serial.print("\t");
  Serial.print(humidity, 1);
  Serial.print("\t\t");
  Serial.print(temperature, 1);
  Serial.print("\t\t");
  Serial.print(dht.toFahrenheit(temperature), 1);
  Serial.print("\t\t");
  Serial.print(dht.computeHeatIndex(temperature, humidity, false), 1);
  Serial.print("\t\t");
  Serial.println(dht.computeHeatIndex(dht.toFahrenheit(temperature), humidity, true), 1);
}

