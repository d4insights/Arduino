#include <Wire.h>


char   datosSensores[15];         //Aca levanto los datos por cada peticion 


float Vrms = 220;                // Voltaje de entrada       (Volts)
float Irms = 0;                  // Corriente de entrada     (Amperes)

// Decodifico la trama que me llega del NANO que mide la tensión
void decodificacionTrama(){
  Vrms   = float ((datosSensores[0] * 10000 + datosSensores[1] * 1000 + datosSensores[2]* 100 + datosSensores[3]* 10 + datosSensores[4]* 1) / 100.00);
  Irms   = float ((datosSensores[5] * 10000 + datosSensores[6] * 1000 + datosSensores[7]* 100 + datosSensores[8] *10 + datosSensores[9]* 1) / 100.00);
  //Serial.print("Vrms:");
  //Serial.print((float)Vrms);
  //Serial.print(" | ");
  //Serial.print("Irms:");
  //Serial.println((float)Irms);
}


// Recibo las lecturas del NANO 5 bytes de Voltaje y 5 bytes de Amperaje
void actualizacionLectura(int bytes){
 int pos = 0;
 Wire.requestFrom(8, bytes);           // Hago el pedido al dispositivo 8
 while(Wire.available()){
    char c = Wire.read();
    //Serial.write(c);
    datosSensores[pos] = c - 48;
    pos++;
 }
 decodificacionTrama();
}



  
  
