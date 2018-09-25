/*
  This a simple example of the aREST Library for Arduino (Uno/Mega/Due/Teensy)
  using the Ethernet library (for example to be used with the Ethernet shield).
  See the README file for more details.

  Written in 2014 by Marco Schwartz under a GPL license.
*/

// Libraries
#include <SPI.h>
#include <Ethernet.h>
#include <aREST.h>


// Enter a MAC address for your controller below.
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

// IP address in case DHCP fails
IPAddress ip(192,168,1,152);

// Ethernet server
EthernetServer server(80);

// Create aREST instance
aREST rest = aREST();

// Variables to be exposed to the API
int temperature;
int humidity;

void setup(void)
{
  // Start Serial
  Serial.begin(115200);

  pinMode(6, OUTPUT);
  pinMode(2, OUTPUT);
  
  pinMode(A0, INPUT);
  pinMode(A1, OUTPUT);
  analogWrite(A1,1);
  
  // Init variables and expose them to REST API
  temperature = 24;
  humidity = 40;
  rest.variable("temperature",&temperature);
  rest.variable("humidity",&humidity);

  // Function to be exposed
  rest.function("led",ledControl);
  rest.function("buzzer",BuzzerControl);

  // Give name & ID to the device (ID should be 6 characters long)
  rest.set_id("Ether");
  rest.set_name("Arduino");

  // Start the Ethernet connection and the server
  //if (Ethernet.begin(mac) == 0) {
  //  Serial.println("Failed to configure Ethernet using DHCP");
    // no point in carrying on, so do nothing forevermore:
    // try to congifure using IP address instead of DHCP:
    Ethernet.begin(mac, ip);
  //}
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());


}

void loop() {

  // listen for incoming clients
  EthernetClient client = server.available();
  rest.handle(client);

  

}

// Custom function accessible by the API
int ledControl(String command) {

  // Get state from command
  int state = command.toInt();

  digitalWrite(6,state);
  return 1;
}

// Custom function Buzzer
int BuzzerControl(){
  // Hace sonar el buzzer 
  digitalWrite(2, HIGH);
  delay(300);
  digitalWrite(2, LOW);
  delay(100);
  digitalWrite(2, HIGH);
  delay(300);
  digitalWrite(2, LOW);
  delay(100);
  digitalWrite(2, HIGH);
  delay(300);
  digitalWrite(2, LOW);

  return 1;
}

