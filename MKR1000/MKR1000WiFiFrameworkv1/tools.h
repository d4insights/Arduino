#define SECRET_apSSID "mkr1000AP"
#define SECRET_apPASS "                    "
#define SECRET_wifiSSID "                         "
#define SECRET_wifiPASS "                         "

String SECRET_routerSSID = "CiscoE2500";         // Se usa solo para el debugMode TRUE
String SECRET_routerPWD = "4CE9C5C6F6";         // Se usa solo para el debugMode TRUE

#include <FlashStorage.h>
#include <Adafruit_SleepyDog.h>

typedef struct {
  boolean valid;
  char mySsid[100];
  char myPwd[100];
} FSecurity;


bool ledBuiltInStatus           = LOW;
unsigned long previousMillis    = 0;  
long intervalFlashingLed        = 0;


// Reservo un espacio de memoria flash y creo objeto credenciales 
FlashStorage(my_flash_store, FSecurity);
FSecurity credenciales;

char apssid[] = SECRET_apSSID;      // your network SSID (name)
char appass[] = SECRET_apPASS;      // your network password (use for WPA, or use as key for WEP)

String modoMKR1010 = "Disconnected";
bool firstTime = true;              // Variable de estado para saber si es el primer ciclo después de bootear
bool portalConsumido = false;       // Variable de control para chequear si no tengo conexión y estoy ciclando AP por ROUTER (necesito saber si el portal no tiene actividad es que se cortó Internet y estoy ciclando)
int ciclosSinConsumo = 0;           // Cantidad de ciclos que hago sin que un cliente consuma el portal (osea nadie esta queriendo usar el AP)

WiFiServer server(80);

String parametros  = "";
String wifiSSID    = "";
String wifiPASS    = "";
String buttonorder = "";


int status = WL_IDLE_STATUS;


void printWiFiStatus() {
  
  Serial.print("Router SSID: ");
  Serial.print(WiFi.SSID());
  Serial.print(" | my IP: ");
  IPAddress ip = WiFi.localIP();
  Serial.print(ip);
  Serial.print(" | status --> ");
  Serial.println(status);
  Serial.println();

}

// Rutina para guardar las credenciales en la FlashStorage
void rutinaSaveCredentials(FSecurity credenciales){

  if(buttonorder == "Save"){
     wifiSSID.toCharArray(credenciales.mySsid, 100);
     wifiPASS.toCharArray(credenciales.myPwd, 100);
     credenciales.valid = true; 
     buttonorder = "";                     // Lo apago para que guarde solo una vez
     modoMKR1010 = "Memory save !!";
     my_flash_store.write(credenciales);   // Guardo en la Flash los datos 
     Serial.println();
     Serial.print("SSID: ");
     Serial.println(credenciales.mySsid);
     Serial.print("PWD : ");
     Serial.println(credenciales.myPwd);
     Serial.println("have been saved. Thank you!");
  } 

  if(buttonorder == "Erase"){
    credenciales.valid = false; 
    buttonorder = "";                     // Lo apago para que guarde solo una vez
    modoMKR1010 = "Memory erase";
    my_flash_store.write(credenciales);   // Guardo en la Flash los datos
    Serial.println("SSID y PWD borradas !!!");   
  }

  if(buttonorder == "Reboot"){
    int countdownMS = Watchdog.enable(1000);
    buttonorder = "";                     // Lo apago para que guarde solo una vez
    modoMKR1010 = "Restarting..";
    Serial.print("The watchdog will reset de Mkr1010 in ");
    Serial.print(countdownMS, DEC);
    Serial.println(" milliseconds!");
  }

  if(buttonorder == "Quit"){
    buttonorder = "";                     // Lo apago para que guarde solo una vez
    modoMKR1010 = "Disconnected";
    Serial.println();
    Serial.println("EL usuario presiono QUIT - Apago el AP e intento conectar al router con las claves que tenía");
    WiFi.end();
    int countdownMS = Watchdog.enable(1000);
    Serial.print("The watchdog will reset de Mkr1010 in ");
    Serial.print(countdownMS, DEC);
    Serial.println(" milliseconds!");
  }
  
}

// Conexión en modalidad Cliente WEB
void connectClientMode(){
  modoMKR1010 = "Starting WebClient";
  int intentos = 0;
 
  while (status != WL_CONNECTED) {
    
    Serial.print("Intentando conectar al ROUTER SSID: ");
    Serial.print(credenciales.mySsid);
    Serial.print(" | ");
    Serial.print(credenciales.myPwd);
    Serial.print(" | valid --> ");
    Serial.print(credenciales.valid);
    Serial.print(" | intento --> ");
    Serial.println(intentos);
    Serial.println();
    status = WiFi.begin(credenciales.mySsid, credenciales.myPwd);     // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    delay(3000);     // Este delay espera que la conexión al router se establezca
    if(intentos++ >= 5){
      
        // Si después de 10 intentos no conecta intento bootear como AP
        // Con esto puedo cambia una clave de algún router viejo
        Serial.println();
        Serial.println("Error de conexión al ROUTER de WIFI luego de 5 intentos");    
        Serial.println();
        WiFi.end();
        status = WiFi.status();
        credenciales.valid = false;
        firstTime = true;
        break;       
    }
  }
  
  if(status == WL_CONNECTED){
    Serial.println("Conectado exitosamente al router WIFI!!!");
    firstTime = false;
    ciclosSinConsumo = 0;
    printWiFiStatus();
    modoMKR1010 = "WebClient ready!";
  }
 
  //delay(100);
 
}

// Conexión en modalidad Access Point
void connectAPMode(){
  modoMKR1010 = "Starting AP..";
  int intentos = 0;
 
  while (status != WL_AP_LISTENING){
      Serial.print("Intentando abrir Access Point SSID: ");
      Serial.print(apssid);
      Serial.print(" | Intento --> ");
      Serial.println(intentos);
      Serial.println();
      status = WiFi.beginAP(apssid);
      delay(3000);
      if (status != WL_AP_LISTENING) {
        
        // Acá preveo reenganchar luego de 30 segundo por si solo se había caído internet
        if(intentos++ >= 10){
          Serial.println();
          Serial.println("Falló la creación del Access Point luego de 10 intentos !!! ");
          Serial.println();
          WiFi.end();
          status = WiFi.status();
          credenciales.valid = true;
          break;
        }
      }
        
  }
  
  if(status == WL_AP_LISTENING){
      server.begin();  // Inicio el web server es port 80
      delay(2000);     // Este delay espera que el Server se encienda
      Serial.println("Connected to wifi as Access Point..!!!!");
      printWiFiStatus();
      modoMKR1010 = "AP listening";
  }
 
  //delay(100);
  
}



void parserParametros(){

    int inicioparametros = parametros.indexOf('?')+1;

    // Dejo solo el string de los parametros.. ose a lo que viene después del "?"
    parametros = parametros.substring(inicioparametros, parametros.length());
    Serial.print("Client WEB Parameters: <");
    Serial.print(parametros);
    Serial.println(">");

    // Recupero el boton que presionó el cliente web
    Serial.print("Boton: ");
    if(parametros.indexOf("Erase")>=0){
         buttonorder = "Erase";
    } 
    if(parametros.indexOf("Quit")>=0){
      buttonorder = "Quit";
    } 
    if(parametros.indexOf("Save")>=0){
      buttonorder = "Save";
    }     
    if(parametros.indexOf("Reboot")>=0){
      buttonorder = "Reboot";
    }     
    Serial.println(buttonorder);

    // Separo el nombre del SSID y descarto esa parte del string de Parametros para seguir parseando
    wifiSSID = parametros.substring(parametros.indexOf('=')+1, parametros.indexOf('&'));
    Serial.print("SSID: ");
    Serial.println(wifiSSID);
    
    parametros = parametros.substring(parametros.indexOf("pwd")+3, parametros.length());
    //Serial.println(parametros);

    // Separo la password y descarto esa parte del string de Parametros para seguir parseando
    wifiPASS = parametros.substring(parametros.indexOf('=')+1, parametros.indexOf('&'));
    Serial.print("PWD : ");
    Serial.println(wifiPASS);

    parametros = "";
}


void rutinaPortalWEB()
{
  WiFiClient client = server.available();   // listen for incoming clients
  if (client) {                             // if you get a client,
    Serial.println("new client");           // print a message out the serial port
    modoMKR1010 = "WebServer ready!";
    portalConsumido = true;                 // Esto sirve para saber si tengo alguien conectado o ejecuto en cambio de AP por conexión al ROUTER
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
      
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:

          
          if (currentLine.length() == 0) {
            
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();
            
            client.print("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">"); 
            client.print("<body style=\"background-color:black\">");
            client.print("<h1 style=\"font-family:verdana; color:GoldenRod\">MKR1010 Access Point </h1>");    
            
            client.print("<fieldset> <legend> <h3 style=\"font-family:verdana; color:White\">Security Form</h3> </legend>");
            client.print("<form action=\"checkpass.php\" method=GET>");
            client.print("<p style=\"font-family:verdana; color:GoldenRod\">");
            client.print("Enter router SSID: <br>");
            client.print("<input type=text name=ssid> <br> <br>");
            client.print("router password:<br>");
            client.print("<input type=text name=pwd> <br> <br> <br>");
            
            client.print("<input type=submit name=button value='Save'>   ");
            client.print("<input type=submit name=button value='Quit'>   ");
            client.print("<input type=submit name=button value='Erase'>  ");
            client.print("<input type=submit name=button value='Reboot'> </p> <br><br>");

            client.print("<p style=\"font-family:verdana; color:White\">Save   -> save the new SSID and Password </p>");
            client.print("<p style=\"font-family:verdana; color:White\">Quit   -> do nothing.. </p>");
            client.print("<p style=\"font-family:verdana; color:White\">Erase  -> blank the flash memory </p>");
            client.print("<p style=\"font-family:verdana; color:White\">Reboot -> Click twice -> restart MKR100 </p>");
            
            client.print("</form> </fieldset>");
            

            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          }
          else
          {      
            
            if(currentLine.startsWith("Referer"))
                parametros = currentLine;
            currentLine = "";
          }
        }
        else 
        if (c != '\r') {         // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    
    // close the connection:
    client.stop();
    
    Serial.println("client disconnected !!! ");
    parserParametros();

  }
}




// Rutina para en que modo esta el MKR1010 (AP/Cliente/ServerWEB)
// La variable modoMKR1010 te dice en que modalidad esta en cada momento.. 
// esta bueno para meter en una pantallita en el footer

void checkNetworkStatus(){
    
    status = WiFi.status();
    if(credenciales.valid == false){         // Valid == false => no tengo credenciales validas
        if(firstTime){
          firstTime = false;
          portalConsumido = false;
          connectAPMode();
        }
        rutinaPortalWEB();
        rutinaSaveCredentials(credenciales);
        if(portalConsumido == false){
          Serial.println();
          Serial.print("Esperando un Cliente WEB que acceda al AP y además entre al Portal WEB  | intento --> ");
          Serial.println(ciclosSinConsumo);
          Serial.println();
          if(ciclosSinConsumo++ >= 10){
            Serial.println();
            Serial.println("Nadie se conecta al Portal WEB luego de 10 intentos !!! ");
            Serial.println();
            WiFi.end();
            status = WiFi.status();
            credenciales.valid = true;
            ciclosSinConsumo = 0;   
          }
          delay(4000);
        } 
    } else {
        if(status != WL_CONNECTED){
           connectClientMode();
        }
    }

}


void validacionHardwareFirmaware(){
//
//  // Chequeo que tenga el módulo de WiFi
//  if (WiFi.status() == WL_NO_MODULE) {
//    while (true);   // don't continue
//  }
  
//  // Chequeo que tenga la versión de firmware actualizada
//  String fv = WiFi.firmwareVersion();
//  if (fv < "1.1.0") {
//    Serial.println("Please upgrade the firmware");
//  }
}


// En ModoDebug salteo la rutina del pedir la clave al inicio después de cada compilación
void validacionModoDebug(bool modo){
  if(modo){                              // Si estoy en debug salto el seteo de claves, hardcodeandola
    SECRET_routerSSID.toCharArray(credenciales.mySsid, 100);
    SECRET_routerPWD.toCharArray(credenciales.myPwd, 100);
    credenciales.valid  = true;
  } else {
    credenciales = my_flash_store.read();    // Recupero el contenido de la FlashStorage
  }  
}
