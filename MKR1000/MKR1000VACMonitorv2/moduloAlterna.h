#include <Wire.h>


char   datosSensores[10];         //Aca levanto los datos por cada peticion 


float Vrms = 220;                // Voltaje de entrada       (Volts)
float Irms = 0;                  // Corriente de entrada     (Amperes)

// Decodifico la trama que me llega del NANO que mide la tensión
void decodificacionTrama(){
  Vrms   = float ((datosSensores[0] * 10000 + datosSensores[1] * 1000 + datosSensores[2]* 100 + datosSensores[3]* 10 + datosSensores[4]* 1) / 100.00);
  Irms   = float ((datosSensores[10] * 10000 + datosSensores[11] * 1000 + datosSensores[12]* 100 + datosSensores[13] *10 + datosSensores[14]* 1) / 100.00);
  Serial.print("Vrms:");
  Serial.print((float)Vrms);
  Serial.print(" | ");
  Serial.print("Irms:");
  Serial.println(Irms);
}


// Recibo las lecturas del NANO 5 bytes de Voltaje y 5 bytes de Amperaje
void actualizacionLectura(int bytes){
int pos = 0;
 Wire.requestFrom(8, bytes);           // Hago el pedido al dispositivo 8
 while(Wire.available()){
    char c = Wire.read();
    datosSensores[pos] = c - 48;
    pos++;
 }
 decodificacionTrama();
}
