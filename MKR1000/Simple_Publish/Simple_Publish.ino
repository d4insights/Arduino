#include <SPI.h>
#include <WiFi101.h>
#include <PubNub.h>

char ssid[] = "pubnub-2015-fall-2";
char pass[] = "4odCf2ov5oXpBnRb";
int status = WL_IDLE_STATUS;
char pubkey[] = "demo";
char subkey[] = "demo";
char channel[] = "temperature";

char *dtostrf (double val, signed char width, unsigned char dec, char *s) {
   char m[20];
   sprintf(m, "%%%d.%df", width, dec);
   sprintf(s, m, val);
   return s;
 }

char longString[100];
char *joinStrings(char* string1, char* string2, char* string3) {
    longString[0] = 0;
    strcat(longString, string1);
    strcat (longString, string2);
    strcat (longString, string3);
    return longString;
}
 
void setup() {
    Serial.begin(9600);
    while (status !=WL_CONNECTED) {
        Serial.print("Attempting to connect to network…");
        status = WiFi.begin(ssid, pass);
        delay(1000);
    }
    Serial.print("SSID:");
    Serial.println(WiFi.SSID());
    PubNub.begin(pubkey, subkey);
}

void loop() {
  char msg[200];
  float data = 3.0;
  dtostrf(data, 7, 2, msg);
  char *json = joinStrings("{\"Data\":\"", msg, "\"}}");
   WiFiClient *client = PubNub.publish(channel, json);
  while (client->connected()) {
    char c = client->read();
    Serial.print(c);
  }
  client->stop();
  delay(5000);
}
