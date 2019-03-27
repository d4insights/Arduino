/*
 *  Librería de Actualización del reloj del MKR1010 
 *  se sincroniza solo desde Internet
 * 
 *      https://www.arduino.cc/en/Tutorial/MKRGSMExamplesGPRSUdpNtp
*/


#include <RTCZero.h>



const long timeZoneOffset = -10800L;                  // Este es el Offset para GMT-3 (en segundos)
unsigned int localPort = 2390;                        // local port to listen for UDP packets
IPAddress timeServer(129, 6, 15, 28);                 // time.nist.gov NTP server
const int NTP_PACKET_SIZE = 48;                       // NTP time stamp is in the first 48 bytes of the message
byte packetBuffer[ NTP_PACKET_SIZE];                  // buffer to hold incoming and outgoing packe


String rtcDia  = "";                     // Este String guarda el día luego de traerlo de Internet
String rtcHora = "";                     // Este String guarda la hora luego de traerla de Internet

GSMUDP Udp;                              // A UDP instance to let us send and receive packets over UDP

RTCZero rtc;

void startUDP(){
  Serial.println("\nStarting connection to Time Server...");
  Udp.begin(localPort);
}


// Envía un NTP request al servidor de administración del tiempo en Internet
unsigned long sendNTPpacket(IPAddress& address)
{
  //Serial.println("1");
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  //Serial.println("2");
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;      // Stratum, or type of clock
  packetBuffer[2] = 6;      // Polling Interval
  packetBuffer[3] = 0xEC;   // Peer Clock Precision
                            // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

  //Serial.println("3");

  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  Udp.beginPacket(address, 123);                    //NTP requests are to port 123
  
  //Serial.println("4");
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  
  //Serial.println("5");
  Udp.endPacket();
  
  //Serial.println("6");

  Serial.println("Paquete enviado");
}



// Setear el reloj interno del MKR1400 con la hora de internet
// Devuelve 0-->"error" ó 1-->"RTC Seteado"
int setUDPRelojRTC(){
  
    sendNTPpacket(timeServer);  // send an NTP packet to a time server
    delay(1000);                // wait to see if a reply is available
    
    //if ( Udp.parsePacket() ) {
     if(true){
      Serial.println("packet received");
    
      Udp.read(packetBuffer, NTP_PACKET_SIZE);                              // read the packet into the buffer
  
      //the timestamp starts at byte 40 of the received packet and is four bytes,
      // or two words, long. First, esxtract the two words:
      unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
      unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
  
      // combine the four bytes (two words) into a long integer
      unsigned long secsSince1900 = highWord << 16 | lowWord;               // this is NTP time (seconds since Jan 1 1900):
      Serial.print("Seconds since Jan 1 1900 = " );
      Serial.println(secsSince1900);
      
      // now convert NTP time into everyday time:
      Serial.print("Unix time = ");                                         
      const unsigned long seventyYears = 2208988800UL;                      // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
      unsigned long epoch = secsSince1900 - seventyYears;                   // subtract seventy years:
  
      //epoch = epoch + timeZoneOffset;                                       // OffSet para GMT-3 en segundos
      
      Serial.print("Recuperé el siguiente epoch: ");
      Serial.println(epoch);                                                // print Unix time:
      rtc.setEpoch(1553458975L);
      //return 1;

                
                    // print the hour, minute and second:
                    Serial.print("The UTC time is ");       // UTC is the time at Greenwich Meridian (GMT)
                    Serial.print((epoch  % 86400L) / 3600); // print the hour (86400 equals secs per day)
                    Serial.print(':');
                    if ( ((epoch % 3600) / 60) < 10 ) {
                      // In the first 10 minutes of each hour, we'll want a leading '0'
                      Serial.print('0');
                    }
                    Serial.print((epoch  % 3600) / 60); // print the minute (3600 equals secs per minute)
                    Serial.print(':');
                    if ( (epoch % 60) < 10 ) {
                      // In the first 10 seconds of each minute, we'll want a leading '0'
                      Serial.print('0');
                    }
                    Serial.println(epoch % 60); // print the second
                    delay(10000);


      
    
    } else {
      Serial.println("No llegó naranja!!!"); 
      return 0;
    }

    

}

// Voy a leer la hora de Internet para setear el RTC del micro
// Devuelve 0-->"error" ó 1-->"RTC Seteado"
int setRelojRTC(){
  unsigned long epoch;
  int numberOfTries = 0;
  int maxTries = 6;

 
  do {
    epoch = gsmAccess.getTime();
    numberOfTries++;
  } while ((epoch == 0) && (numberOfTries < maxTries));
  epoch = epoch + timeZoneOffset;                   // OffSet para GMT-3 en segundos
  if (numberOfTries > maxTries) {
    Serial.print("NTP unreachable!!");
    return 0;
  }
  else {
    Serial.print("Epoch received: ");
    Serial.println(epoch);
    rtc.setEpoch(epoch);
    iconSincro = true;
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


String rtcGetHoraLarga()
{
  rtcHora  = print2digits(rtc.getHours());
  rtcHora += ":";
  rtcHora += print2digits(rtc.getMinutes());
  rtcHora += ":";
  rtcHora += print2digits(rtc.getSeconds());
  return rtcHora;
}

String rtcGetHoraCorta()
{
  rtcHora  = print2digits(rtc.getHours());
  rtcHora += ":";
  rtcHora += print2digits(rtc.getMinutes());
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
