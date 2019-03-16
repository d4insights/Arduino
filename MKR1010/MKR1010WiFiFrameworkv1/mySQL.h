/*
 *    Librería de conexión y uso de la base de datos MYSQL
 *    basado en el UBUNTU Server de AZURE
 *    
 *    Guarda mediante el puente con una rutina PHP que te abstrae del usuario y la clave de la base de datos por seguridad
 *
 *    guardarBaseDatos();
 *    
 *    Usa una trama del tipo GET de HTML  srvpoc.eastus.cloudapp.azure.com/VACMonitor/insert.php?IDSensor=1&VIN=100.01&IIN=2.02&PIN=1.01 
 *    
 *     
 */

 
char serverBD[] = "srvpoc.eastus.cloudapp.azure.com";   // Servidor de BD

unsigned long ultimoRegistroDB = 60000;                 // Auxiliar para manejo del períod de tiempo entre registros
long intervalRegistroBD        = 60000;                 // Manejo del período de tiempo entre grabaciones en la BDatos

WiFiClient client;
WiFiClient clientTest;



// Test para asegurar que además de tener WiFi tengo servicio de Internet 
// PREVIO A LLAMAR ESTA FUNCION TENGO QUE ESTAR CONECTADO A UN AP
// true = Hay Internet || false = no hay Internet aunque puede haber WiFi
//
bool testInternetConnection(const char* serverTest){

  int rta=0;                            // Cantidad de caracteres que recibo de respuesta de la pagina web
  
  
  if (clientTest.connect(serverTest, 80)) {
    Serial.println("connected to server we have got Internet!!!");
    clientTest.println("GET  HTTP/1.1");
    client.print("Host: ");
    client.println(serverBD);
    clientTest.println("Connection: close");
    clientTest.println();
  }
  
  delay(300);  // Delay necesario para que la pagina web responda       
  
  // Muestro la respuesta por el Monitor Serial
  while (clientTest.available()) {
    char c = clientTest.read();
    rta++;
    Serial.print(c);
  }
  
  // Si el servidor me desconectó, desconecto el Cliente
  if (clientTest.connected()) {
    //Serial.println("disconnecting from server.");
    clientTest.stop();
  }

  if(rta==0){
     modoMKR1010="Internet down!!";
     return false; 
  }else{
     modoMKR1010 = WiFi.SSID();
     return true;
  }
}



/*
// Guardo en la base de datos
// Devuelve un int que representa la cantidad de caracteres de respuesta de la página si es 0 no había Internet
//
int guardarBaseDatos(){

 if (millis() - ultimoRegistroDB >= intervalRegistroBD) {
    int rta = 0;
    ultimoRegistroDB = millis();
  
    char CadenaInstrucc[500];           // OJO QUE SI NO TE ALCANZA LA DIMENSION PARA GUARDAR EL STRING EL PROGRAMA EXPLOTA POR EL WATCHDOG DE LA ESP
    
    String Instruccion = "/VACMonitor/insert.php";
    Instruccion.concat("?IDSensor=");
    Instruccion.concat(001);
    Instruccion.concat("&VIN=");
    Instruccion.concat(String(Vrms,2)); 
    Instruccion.concat("&IIN=");
    Instruccion.concat(String(Irms,2));
    Instruccion.concat("&PIN=");
    Instruccion.concat(String((Vrms*Irms),2));
            
    Instruccion.concat(" ");
    
    Instruccion.toCharArray(CadenaInstrucc,Instruccion.length());
    
    // Conecto al Server de BASE DE DATOS      
    //Serial.println("\nStarting connection to server...");
    if (client.connect(serverBD, 80)) {
      //Serial.println("connected to server");
      
      // Genero el HTTP request:
      client.print("GET ");
      client.print(CadenaInstrucc);
      Serial.print(CadenaInstrucc);
      //client.print("/VACMonitor/insert.php?IDSensor=1&VIN=100.01&IIN=2.02&PIN=1.01 ");
      client.println(" HTTP/1.1");
      client.print("Host: ");
      client.println(serverBD);
      client.println("Connection: close");
      client.println();
    }

    delay(300);  // Delay necesario para que la pagina web responda    
    
    // Recupero lo que me desponda el servidor
    while (client.available()) {
      char c = client.read();
      Serial.write(c);
      rta++;
    }
    
    // Desconecto el cliente
    if (!client.connected()) {
      //Serial.println();
      //Serial.println("disconnecting from server.");
      client.stop();
    }
  }
  
  Serial.println();
  return rta;
}
*/
