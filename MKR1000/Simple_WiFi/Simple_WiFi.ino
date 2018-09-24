#include <SPI.h>
#include <WiFi101.h>

char ssid[] = "WRT1900AC";
char pass[] = "4CE9C5C6F6";
int status = WL_IDLE_STATUS;

void setup() {
    Serial.begin(9600);
    while (status !=WL_CONNECTED) {
        Serial.println("Attempting to connect to network…");
        status = WiFi.begin(ssid, pass);
        delay(10000);
    }
    Serial.print("SSID:");
    Serial.println(WiFi.SSID());
}

void loop() {
  Serial.println("Congrats! You're connected.");
  delay(5000);
}
