/*
 *    Librería de conexión y uso de la base de datos MYSQL
 *    basado en el UBUNTU Server de AZURE
 *    
 *    Guarda mediante el puente con una rutina PHP que te abstrae del usuario y la clave de la base de datos por seguridad
 *
 *    Usa una trama del tipo GET de HTML  srvpoc.eastus.cloudapp.azure.com/VACMonitor/insert.php?IDSensor=1&VIN=100.01&IIN=2.02&PIN=1.01 
 */

 
WiFiClient client;


char serverBD[] = "srvpoc.eastus.cloudapp.azure.com";   // Servidor de BD

unsigned long ultimoRegistroDB = 60000;                 // Auxiliar para manejo del períod de tiempo entre registros
long intervalRegistroBD        = 60000;                 // Manejo del período de tiempo entre grabaciones en la BDatos




//Guardo en la base de datos
void guardarBaseDatos(){

 if (millis() - ultimoRegistroDB >= intervalRegistroBD) {
    
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
      client.println("Host: srvpoc.eastus.cloudapp.azure.com");
      client.println("Connection: close");
      client.println();
    }
    
    // Recupero lo que me desponda el servidor
    while (client.available()) {
      char c = client.read();
      Serial.write(c);
    }
    
    // Desconecto el cliente
    if (!client.connected()) {
      //Serial.println();
      //Serial.println("disconnecting from server.");
      client.stop();
    }
  }
  
  Serial.println();
}
