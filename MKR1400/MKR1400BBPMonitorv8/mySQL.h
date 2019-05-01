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


unsigned long ultimoRegistroAlertaDB = 0;                 // Auxiliar para manejo del períod de tiempo entre registros
long intervalRegistroAlertaBD        = 60000;                 // Manejo del período de tiempo entre grabaciones en la BDatos



// Guarda la mediciones de los sensores de Entrada, Salida y Baterías internas
// Devuelve:  false-> No Conectado  y true--> Conectado y el server respondió
//
bool registraMedicionDB(){

 if (millis() - ultimoRegistroDB >= intervalRegistroBD) {
    
    ultimoRegistroDB = millis();
    
    int port = 80;                                            // port 80 is the default for HTTP
    bool rta = false;
    int tries = 0;

    char CadenaInstrucc[500];           // OJO QUE SI NO TE ALCANZA LA DIMENSION PARA GUARDAR EL STRING EL PROGRAMA EXPLOTA POR EL WATCHDOG DE LA ESP
    
    String Instruccion = "/edenor/insert.php";
    Instruccion.concat("?IDInversor=");
    Instruccion.concat(myIMEI.substring(myIMEI.length()-10,myIMEI.length()));
    
    Instruccion.concat("&VIN=");
    Instruccion.concat(VIN);    
    Instruccion.concat("&IIN=");
    Instruccion.concat(IIN);  
    Instruccion.concat("&PIN=");
    Instruccion.concat(PIN);  
    
    Instruccion.concat("&VOUT=");
    Instruccion.concat(VOUT);      
    Instruccion.concat("&IOUT=");
    Instruccion.concat(IOUT);  
    Instruccion.concat("&POUT=");
    Instruccion.concat(POUT);
              
    Instruccion.concat("&VB00=");
    Instruccion.concat(String(iconBateria,2)); 
    Instruccion.concat("&VB01=");
    Instruccion.concat(bateria01); 
    Instruccion.concat("&VB02=");
    Instruccion.concat(bateria02); 
    Instruccion.concat("&VB03=");
    Instruccion.concat(bateria03); 
    Instruccion.concat("&VB04=");
    Instruccion.concat(bateria04); 
    Instruccion.concat("&VB05=");
    Instruccion.concat(0); 
    Instruccion.concat("&VB06=");
    Instruccion.concat(0); 
    Instruccion.concat("&VB07=");
    Instruccion.concat(0);
    Instruccion.concat("&VB08=");
    Instruccion.concat(0);
    Instruccion.concat("&VB09=");
    Instruccion.concat(0); 
    Instruccion.concat("&VB10=");
    Instruccion.concat(0);
    Instruccion.concat("&VB11=");
    Instruccion.concat(0);
    Instruccion.concat("&VB12=");
    Instruccion.concat(0);
    Instruccion.concat("&VB13=");
    Instruccion.concat(0);
    Instruccion.concat("&VB14=");
    Instruccion.concat(0);
    Instruccion.concat("&VB15=");
    Instruccion.concat(0);
    Instruccion.concat("&VB16=");
    Instruccion.concat(0);
                
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
    Serial.print("Recibed from server.. ");
    String respuesta = "";
    while(client.available()) {
      char c = client.read();                                          // Leo la respuesta del server
      //Serial.print(c);
      rta = true;
      respuesta = respuesta + c;
      if(tries++>15)
          break;
    }
   
    Serial.println(respuesta);
    if(respuesta.indexOf("HTTP/1.1 200 OK")<0){                       // Si no devuelva esta cadena no pudo llegar a la Base de Datos para grabar
          displayError("Error Acceso", "DBServer(Med)");  
          iconSincro = false; 
          delay(1500);
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






// Guarda las Alertas
// Devuelve:  false-> No Conectado  y true--> Conectado y el server respondió
//
bool registraAlertaDB(String criticidad, String tipoAl, String valorAl, String desc){

 if (millis() - ultimoRegistroAlertaDB >= intervalRegistroAlertaBD) {
    
    ultimoRegistroAlertaDB = millis();
    
    int port = 80;                                            // port 80 is the default for HTTP
    bool rta = false;
    int tries = 0;

    char CadenaInstrucc[500];           // OJO QUE SI NO TE ALCANZA LA DIMENSION PARA GUARDAR EL STRING EL PROGRAMA EXPLOTA POR EL WATCHDOG DE LA ESP
    
    String Instruccion = "/edenor/alerta.php";
    Instruccion.concat("?IDInversor=");
    Instruccion.concat(myIMEI.substring(myIMEI.length()-10,myIMEI.length()));
    
    Instruccion.concat("&Criticidad=");
    Instruccion.concat(criticidad);    
    Instruccion.concat("&TipoAlerta=");
    Instruccion.concat(tipoAl); 
    Instruccion.concat("&ValorAlerta=");
    Instruccion.concat(valorAl);      
    Instruccion.concat("&Descripcion=");
    Instruccion.concat(desc);  
                    
    Instruccion.concat(" ");
    
    Instruccion.toCharArray(CadenaInstrucc,Instruccion.length());
      
    if (client.connect(serverBD, port)) {
      iconSincro = true;
      displayHeader(modoMKR1400, iconAlerta, iconSMS, iconSincro, iconSenal, iconBateria); 
      
      Serial.print("Sending a ALERT request to a Server..");
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
    Serial.print("Recibed from server.. ");
    String respuesta = "";
    while(client.available()) {
      char c = client.read();                                          // Leo la respuesta del server
      //Serial.print(c);
      rta = true;
      respuesta = respuesta + c;
      if(tries++>15)
          break;
    }
   
    Serial.println(respuesta);
    if(respuesta.indexOf("HTTP/1.1 200 OK")<0){                       // Si no devuelva esta cadena no pudo llegar a la Base de Datos para grabar
          displayError("Error Acceso", "DBServer(Al)");
          iconSincro = false; 
          delay(1500);
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
