#include <SPI.h>
#include <Adafruit_SleepyDog.h>

void setup() {
  delay(3000);
  Serial.begin(9600);
  //int countdownMS = Watchdog.enable(1000);
  //Watchdog.reset();
  //Watchdog.disable();
  int sleepMS = Watchdog.sleep(3000);
  
}

void loop() {
  delay(1000);
  Serial.println("Waking up...");
}
