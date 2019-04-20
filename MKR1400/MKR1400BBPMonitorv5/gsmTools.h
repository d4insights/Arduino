/*
 *  Biblioteca de conexión 4G 
 * 
 *  Conexión, desconexión, test de comunicación con el server de google
 * 
*/

#include <MKRGSM.h>

const char PINNUMBER[]     = "1111";                  // Este es el PIN de la SIM, anda igua si esta vacío 
const char GPRS_APN[]      = "igprs.claro.com.ar";    // GPRS APN (provisto por CLARO ARGENTINA)
const char GPRS_LOGIN[]    = "clarogprs";             // GPRS login (puede ir vacío en CLARO)
const char GPRS_PASSWORD[] = "clarogprs";             // GPRS password (puede ir vacío en CLARO)


bool gsmConnected = false;                            // connection GSM state

GSMClient client;                                     // Objeto Cliente GSM
GSMLocation location;                                 // Objeto GPS
GPRS gprs;                                            // Objeto Conexión (APN)
GSM gsmAccess;                                        // Objeto Autenticación de GSM
GSMScanner scannerNetworks;                           // Objeto de manejo de las redes para saber a cual me conecto
GSMModem modem;                                       // Objeto Modem sirve para conocer el IMEI


unsigned long ultimoLecturaGPS  = 600000;                 // Auxiliar para no mandar SMS todo el tiempo ante un fallo (así manda cada 10 min)
long intervalLecturaGPS         = 600000;                 // Manejo del período de tiempo entre SMS (así manda cada 10 min)


// Apaga el modem que es equivalente a apagar el celular 
//
void modemDisconect(){
  gsmConnected = false;
  Serial.println();
  Serial.println("Shutdown the modem ..");
  iconSenal = 0;
  gsmAccess.shutdown();
}


// La coordenada satelital del dispositivo medida desde la red GSM
//
void myUbicacionGPS(){
   if (millis() - ultimoLecturaGPS >= intervalLecturaGPS) {
    
    ultimoLecturaGPS = millis();
          
        if (location.available()) {
      //    Serial.print("Location: ");
      //    Serial.print(location.latitude(), 7);
      //    Serial.print(", ");
      //    Serial.print(location.longitude(), 7);
      //
      //    Serial.print(" | Altitude: ");
      //    Serial.print(location.altitude());
      //    Serial.print("m");
      //
      //    Serial.print(" | Accuracy: +/- ");
      //    Serial.print(location.accuracy());
      //    Serial.println("m");
      
          myLocation = String(location.latitude(), 7) + ", " + String(location.longitude(), 7);
          Serial.print("GPS Location: ");
          Serial.println(myLocation);
        }
   }      
}


// Conexión al APN
//
void modemConnect(){
  //Serial.println("GSM networks scanner starting up..");
  scannerNetworks.begin();

  //Serial.println("GSM Modem starting up..");
  //modem.begin();
  
  // Inicaliza el modem GSM.begin() y loguea a la red de GPRS mediante el APN (usuario y password)
  Serial.println("Conecting to a Server ..");
  while (!gsmConnected) {
    if ((gsmAccess.begin(PINNUMBER) == GSM_READY) && (gprs.attachGPRS(GPRS_APN, GPRS_LOGIN, GPRS_PASSWORD) == GPRS_READY)) {
      gsmConnected = true;
      Serial.println("APN Connected.. OK");
      
      myIMEI = modem.getIMEI();          // Recupera el nro de IMEI de la SIM
       
      // currently connected carrier
      modoMKR1400 = scannerNetworks.getCurrentCarrier();
      Serial.print("Current carrier: ");
      Serial.println(modoMKR1400);
      
      // returns strength and ber
      // signal strength in 0-31 scale. 31 means power > 51dBm
      // BER is the Bit Error Rate. 0-7 scale. 99=not detectable
      iconSenal = (scannerNetworks.getSignalStrength()).toInt();
      Serial.print("Signal Strength: ");
      Serial.print(iconSenal);
      Serial.println(" [0-31]");
  

    } else {
      gsmConnected = false;
      Serial.println("APN Connection error!!");
      iconSenal = 0;
      modoMKR1400 = "4g disconnected";
       
    }
  }
}




// Test de conexión a una página web (Supone que estoy conectado GPRS)
// Devuelve:  false-> No Conectado  y true--> Conectado y el server respondió
//
bool testConnection(){

  GSMClient clientT;

  char serverTest[] = "www.google.com";
  char path[] = "/";
  int port = 80;                                            // port 80 is the default for HTTP
  bool rta = false;
  int tries = 0;
    
  if (clientT.connect(serverTest, port)) {
    Serial.println("Sending a request to a Server..");
    clientT.print("GET ");
    clientT.print(path);
    clientT.println(" HTTP/1.1");
    clientT.print("Host: ");
    clientT.println(serverTest);
    clientT.println("Connection: close");
    clientT.println();
    iconSincro = true;
  } else {
    Serial.println("Connection to a Server failed !!!");              // Si fallo la conexión
    iconSincro = false;
  } 
  
                                                                
  while(!clientT.available() || tries > 4000)                         // delay necesario para esperar la latencia de la conexión
      tries++;
  
  tries = 0;
  while(clientT.available()) {
    char c = clientT.read();                                          // Leo la respuesta del server
    Serial.print(c);
    rta = true;
    if(tries++>100)
      break;
  } 

  if (!clientT.available() && !clientT.connected()) {                 // Si el server me desconectó, paro el cliente WEB  
    Serial.println();
    Serial.println("disconnecting...");
    clientT.stop();
    iconSincro = false;
  }

  if(rta == false){
     return false; 
     iconSincro = false; 
  } else {
     return true;
     iconSincro = true;
  }
  
}
