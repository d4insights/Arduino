/*
 *   Framework MKR GSM 1400
 * 
 *    
 * 
 * 
 * 
*/

#include "4GTools.h" 



char serverTest[] = "google.com.ar";
char path[] = "/";
int port = 80;                                            // port 80 is the default for HTTP

void setup() {
  
  delay(100);
  Serial.begin(115200);


  Serial.println("Starting Arduino web client.");
  bool connected = false;                               // connection state


  // Inicaliza el modem GSM.begin() y loguea a la red de GPRS mediante el APN (usuario y password)
  while (!connected) {
    if ((gsmAccess.begin(PINNUMBER) == GSM_READY) && (gprs.attachGPRS(GPRS_APN, GPRS_LOGIN, GPRS_PASSWORD) == GPRS_READY)) {
      connected = true;
      Serial.println("APN Connected.. OK");
    } else {
      Serial.println("APN Connection error!!");
      delay(1000);
    }
  }

  Serial.println("Conecting to a Server ..");

  
  if (client.connect(serverTest, port)) {
    Serial.println("Sending a request to a Server..");
    client.print("GET ");
    client.print(path);
    client.println(" HTTP/1.1");
    client.print("Host: ");
    client.println(serverTest);
    client.println("Connection: close");
    client.println();
  } else {
    Serial.println("Connection to a Server failed !!!");              // Si fallo la conexión
  }
}

void loop() {

  if (client.available()) {
    char c = client.read();                                           // Leo la respuesta del server
    Serial.print(c);
  }

  if (!client.available() && !client.connected()) {                   // Si el server me desconectó, paro el cliente WEB  
    Serial.println();
    Serial.println("disconnecting...");
    client.stop();

      
    for (;;)
      ;
  }
}
