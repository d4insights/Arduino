/*
  MySQL Connector/Arduino Example : connect by wifi using WiFi 101 shield

  This example demonstrates how to connect to a MySQL server from an
  Arduino using using the new WiFi Shield 101 from arduino.cc.

  NOTICE NOTICE NOTICE

  The new WiFi 101 library is quite large. You should use this sketch and the
  shield with the new Arduino Due or Zero.

  You should also use the latest Arduino IDE from arduino.cc. This sketch was
  tested with release 1.6.7 from https://www.arduino.cc/en/Main/Software
  running on a Due board.

  Also, make sure your hardware libraries are uptodate by visiting the
  boards manager and installing updates for the boards you are tying to use
  (e.g. Due, Zero).

  INSTRUCTIONS FOR USE

  1) Change the address of the server to the IP address of the MySQL server
  2) Change the user and password to a valid MySQL user and password
  3) Change the SSID and pass to match your WiFi network
  4) Connect a USB cable to your Arduino
  5) Select the correct board and port
  6) Compile and upload the sketch to your Arduino
  7) Once uploaded, open Serial Monitor (use 115200 speed) and observe

  If you do not see messages indicating you have a connection, refer to the
  manual for troubleshooting tips. The most common issues are the server is
  not accessible from the network or the user name and password is incorrect.

  Note: The MAC address can be anything so long as it is unique on your network.

  Created by: Dr. Charles A. Bell
*/
#include <SPI.h>
#include <WiFi101.h>
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>

byte mac_addr[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

IPAddress server_addr(192,168,1,178);  // IP of the MySQL *server* here
char user[] = "testDB";              // MySQL user login username
char password[] = "Passw0rd";        // MySQL user login password

// WiFi card example
char ssid[] = "WRT1900AC";    // your SSID
char pass[] = "4CE9C5C6F6";       // your SSID Password

// Sample query
char INSERT_DATA[] = "INSERT INTO test_arduino.hello_sensor (message, sensor_num, value) VALUES ('%s',%d,%s)";
char query[128];

WiFiClient client;
MySQL_Connection conn((Client *)&client);

void setup() {
  Serial.begin(115200);
  while (!Serial); // wait for serial port to connect

  // Begin WiFi section
  int status = WiFi.begin(ssid, pass);
  if ( status != WL_CONNECTED) {
    Serial.println("Couldn't get a wifi connection");
    while(true);
  }
  // print out info about the connection:
  else {
    Serial.println("Connected to network");
    IPAddress ip = WiFi.localIP();
    Serial.print("My IP address is: ");
    Serial.println(ip);
  }
  // End WiFi section

  Serial.println("Connecting...");
  if (conn.connect(server_addr, 3306, user, password)) {
    delay(1000);
    
    // Initiate the query class instance
    MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
    // Save
    //dtostrf(50.125, 1, 1, temperature);
    sprintf(query, INSERT_DATA, "test sensor", 24, "50");
    // Execute the query
    cur_mem->execute(query);
    // Note: since there are no results, we do not need to read any data
    // Deleting the cursor also frees up memory used
    delete cur_mem;
    Serial.println("Data recorded.");    
  }
  else
    Serial.println("Connection failed.");
  conn.close();
}

void loop() {
}
