/*
 *   Librería de manejo de las conexiones con el MYSQL server de la nube de Azure
 *   
 *    http://srvpoc.eastus.cloudapp.azure.com/phpmyadmin/
 * 
 *   Usa una trama del tipo GET de HTML  
 *          http://srvpoc.eastus.cloudapp.azure.com/VACMonitor/insert.php?IDSensor=1&VIN=100.01&IIN=2.02&PIN=1.01 
 *          http://srvpoc.eastus.cloudapp.azure.com/edenor/insertLiPo.php?IDInversor=33&VLIPO=2.70
 * 
*/


char serverBD[] = "srvpoc.eastus.cloudapp.azure.com";   // Servidor de BD

unsigned long ultimoRegistroDB = 60000;                 // Auxiliar para manejo del períod de tiempo entre registros
long intervalRegistroBD        = 60000;                 // Manejo del período de tiempo entre grabaciones en la BDatos




// Guarda el voltaje de la última medición de la batería interna en la tabla BOARDBATTERY
// Devuelve:  false-> No Conectado  y true--> Conectado y el server respondió
//
bool registraDBvLIPO(){

 if (millis() - ultimoRegistroDB >= intervalRegistroBD) {
    
    ultimoRegistroDB = millis();
    //char path[] = "/";
    int port = 80;                                            // port 80 is the default for HTTP
    bool rta = false;
    int tries = 0;

    char CadenaInstrucc[100];           // OJO QUE SI NO TE ALCANZA LA DIMENSION PARA GUARDAR EL STRING EL PROGRAMA EXPLOTA POR EL WATCHDOG DE LA ESP
    
    String Instruccion = "/edenor/insertLiPo.php";
    Instruccion.concat("?IDInversor=");
    Instruccion.concat(033);
    Instruccion.concat("&VLIPO=");
    Instruccion.concat(String(iconBateria,2)); 
            
    Instruccion.concat(" ");
    
    Instruccion.toCharArray(CadenaInstrucc,Instruccion.length());
      
    if (client.connect(serverBD, port)) {
      iconSincro = true;
      displayHeader(modoMKR1400, iconAlerta, iconSMS, iconSincro, iconSenal, iconBateria); 
      
      Serial.print("Sending a request to a Server..");
      Serial.println(CadenaInstrucc);
      
      client.print("GET ");
      client.print(CadenaInstrucc);
      client.println(" HTTP/1.1");
      client.print("Host: ");
      client.println(serverBD);
      client.println("Connection: close");
      client.println();
      iconSincro = true;
    } else {
      Serial.println("Connection to a Server failed !!!");              // Si fallo la conexión
      iconSincro = false;
      displayHeader(modoMKR1400, iconAlerta, iconSMS, iconSincro, iconSenal, iconBateria);
    } 
                                                                  
    while(!client.available() && tries < 4000)                         // delay necesario para esperar la latencia de la conexión
        tries++;
    
    tries = 0;
    Serial.print("Recibed for server.. ");
    while(client.available()) {
      char c = client.read();                                          // Leo la respuesta del server
      Serial.print(c);
      rta = true;
      if(tries++>15)
       break;
    }   
   
    if (!client.available() && !client.connected()) {                 // Si el server me desconectó, paro el cliente WEB  
      Serial.println();
      Serial.println("disconnecting...");
      client.stop();
    }

    iconSincro = false;
    displayHeader(modoMKR1400, iconAlerta, iconSMS, iconSincro, iconSenal, iconBateria);
  
    if(rta == false){
       return false;     
    } else {
       return true;
    }
  }
}
