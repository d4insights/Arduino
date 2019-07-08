/*
 *   Librería de manejo de las conexiones con el MYSQL server de la nube de Azure
 *   
 *    http://srvpoc.eastus.cloudapp.azure.com/phpmyadmin/
 *    
 *    IP: 40.114.124.9
 * 
 *   Usa una trama del tipo GET de HTML  
 *          http://srvpoc.eastus.cloudapp.azure.com/VACMonitor/insert.php?IDSensor=1&VIN=100.01&IIN=2.02&PIN=1.01 
 *          http://srvpoc.eastus.cloudapp.azure.com/edenor/insertLiPo.php?IDInversor=33&VLIPO=2.70
 * 
*/


#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>

//char serverBD[] = "srvpoc.eastus.cloudapp.azure.com";   // Servidor de BD

//unsigned long ultimoRegistroDB = 60000;                 // Auxiliar para manejo del períod de tiempo entre registros
//long intervalRegistroBD        = 60000;                 // Manejo del período de tiempo entre grabaciones en la BDatos


IPAddress server_addr(192,168,1,178);            // IP of the MySQL *server* here
char user[] = "testDB";                         // MySQL user login username
char password[] = "Passw0rd";                   // MySQL user login password


// Sample query
char INSERT_DATA[] = "INSERT INTO testDB.VACMonitor (IDSensor, VIN, IIN, PIN) VALUES (%d, %s, %s, %s)";
char query[128];


// INSERT en la Base de Datos
//
void insertDB(){
  GSMClient client;  
  MySQL_Connection conn((Client *)&client);
    
      Serial.print("Connecting Data Base...");
      if (conn.connect(server_addr, 3306, user, password)) {
        delay(1000);
        Serial.println("Connection sucssess!!");
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
        Serial.println("Data recorded.");       }
      else
        Serial.println("Connection failed");
      conn.close(); 
 
}
