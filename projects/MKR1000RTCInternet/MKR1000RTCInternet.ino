//  MKR1000 WiFi RTC (Real Time Clock)
//
//  Levanta por WIFI UDP la fecha y hora de Internet, la setea en el RTC del MKR1000 
//  
//
//  http://arduino.cc/en/Tutorial/WiFiRTC
//   


#include <SPI.h>
#include <WiFi101.h>
#include <WiFiUdp.h>
#include <RTCZero.h>

RTCZero rtc;

char ssid[] = "WRT1900AC";  //  your network SSID (name)
char pass[] = "4CE9C5C6F6"; // your network password
int keyIndex = 0;           // your network key Index number (needed only for WEP)

int status = WL_IDLE_STATUS;

const int GMT = -3;         //change this to adapt it to your time zone

void setup() {
  Serial.begin(115200);

  // Chequeo del módulo WIFI
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi module not present");
    while (true);
  }

  // Intento conectar al WIFI
  Serial.print("Conectando a SSID: ");
  Serial.print(ssid);
  Serial.print(" ");
  while ( status != WL_CONNECTED) {
    Serial.print(".");
    status = WiFi.begin(ssid, pass);   // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    delay(1000);
  }
  Serial.println(" Connected!!");
    
  printWiFiStatus();                      // Imprime el status de conexión

  rtc.begin();

  unsigned long epoch;
  int numberOfTries = 0, maxTries = 6;
  do {
    epoch = WiFi.getTime();
    numberOfTries++;
  }
  while ((epoch == 0) || (numberOfTries > maxTries));

  if (numberOfTries > maxTries) {
    Serial.print("Servidor NTP (Network Time Protocol) no accesible!!");
    while (1);
  }
  else {
    Serial.print("Epoch received: ");
    Serial.println(epoch);
    rtc.setEpoch(epoch);

    Serial.println();
  }
}

void loop() {
  printDate();
  printTime();
  Serial.println();
  delay(1000);
}

void printTime()
{
  print2digits(rtc.getHours()+GMT);
  Serial.print(":");
  print2digits(rtc.getMinutes());
  Serial.print(":");
  print2digits(rtc.getSeconds());
  Serial.println();
}

void printDate()
{
  Serial.print(rtc.getDay());
  Serial.print("/");
  Serial.print(rtc.getMonth());
  Serial.print("/");
  Serial.print(rtc.getYear());

  Serial.print(" ");
}


void printWiFiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

void print2digits(int number) {
  if (number < 10) {
    Serial.print("0");
  }
  Serial.print(number);
}
