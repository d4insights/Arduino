/*
 *  Librería de Actualización del reloj del MKR1010 
 *  se sincroniza solo desde Internet
 * 
 * 
*/

#include <WiFiUdp.h>
#include <RTCZero.h>

RTCZero rtc;

const long timeZoneOffset = -10800L;     // Este es el Offset para GMT-3 (en segundos)

String rtcDia  = "";                     // Este String guarda el día luego de traerlo de Internet
String rtcHora = "";                     // Este String guarda la hora luego de traerla de Internet



// Voy a leer la hora de Internet para setear el RTC del micro
// Devuelve 0-->"error" ó 1-->"RTC Seteado"
int setRelojRTC(){
  unsigned long epoch;
  int numberOfTries = 0;
  int maxTries = 6;
  do {
    epoch = WiFi.getTime();
    numberOfTries++;
  } while ((epoch == 0) && (numberOfTries < maxTries));
  epoch = epoch + timeZoneOffset;                   // OffSet para GMT-3 en segundos
  if (numberOfTries > maxTries) {
    //Serial.print("NTP unreachable!!");
    return 0;
  }
  else {
    //Serial.print("Epoch received: ");
    //Serial.println(epoch);
    rtc.setEpoch(epoch);
    return 1;
  }
}


String print2digits(int number) {
  String aux = "";
  if (number < 10) {
    aux = "0";
  }
  aux += String(number);
  return aux;
}


String rtcGetHora()
{
  rtcHora  = print2digits(rtc.getHours());
  rtcHora += ":";
  rtcHora += print2digits(rtc.getMinutes());
  rtcHora += ":";
  rtcHora += print2digits(rtc.getSeconds());
  return rtcHora;
}

String rtcGetDia()
{
  rtcDia  = print2digits(rtc.getDay());
  rtcDia += "/";
  rtcDia += print2digits(rtc.getMonth());
  rtcDia += "/";
  rtcDia += print2digits(rtc.getYear());

  return rtcDia;
}
