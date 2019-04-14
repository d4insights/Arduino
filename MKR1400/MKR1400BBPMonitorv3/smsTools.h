/*
 *   Libreria de uso de SMS por medio de la red de 4g
 * 
 *   Manda mensajes de texto por 4G (lo considera siempre conectado)
 * 
 * 
 * 
 * 
 * 
*/

unsigned long ultimoSMSSafetyBatery  = 600000;                 // Auxiliar para no mandar SMS todo el tiempo ante un fallo (así manda cada 10 min)
long intervalSMSSafetyBatery         = 600000;                 // Manejo del período de tiempo entre SMS (así manda cada 10 min)


unsigned long ultimoSMSInversorBatery  = 600000;               // Auxiliar para no mandar SMS todo el tiempo ante un fallo (así manda cada 10 min)
long intervalSMSInversorBatery         = 600000;               // Manejo del período de tiempo entre SMS (así manda cada 10 min)



#include <MKRGSM.h>
#include <Adafruit_SleepyDog.h>

char celuGuardia[20]       = "1167577019";            // Celular de Guardia pata mandar SMS

String lastSMSSenderN = "";                           // Ultimo SMS Sender Number recibido
String lastSMSRecibed = "";                           // Ultimo SMS Message

GSM_SMS smsOutcome;                                   // Objeto SMS de salida
GSM_SMS smsIncome;                                    // Objeto SMS de entrada



void sendSMSTemporizedSBat(char* cel,String msg){
  
 if (millis() - ultimoSMSSafetyBatery >= intervalSMSSafetyBatery) {
    
    ultimoSMSSafetyBatery = millis();
     
    char txtMsg[200];
    msg.toCharArray(txtMsg,200);
    Serial.print("SMS message --> to: ");
    Serial.print(cel);
    Serial.print(" | msg: ");
    Serial.println(txtMsg);
      
    smsOutcome.beginSMS(cel);                    // send the message
    smsOutcome.print(txtMsg);
    smsOutcome.endSMS();
    Serial.println("SMS Sent !!!");
  }
}

void sendSMSTemporizedIBat(char* cel,String msg){
  
 if (millis() - ultimoSMSInversorBatery >= intervalSMSInversorBatery) {
    
    ultimoSMSInversorBatery = millis();
     
    char txtMsg[200];
    msg.toCharArray(txtMsg,200);
    Serial.print("SMS message --> to: ");
    Serial.print(cel);
    Serial.print(" | msg: ");
    Serial.println(txtMsg);
      
    smsOutcome.beginSMS(cel);                    // send the message
    smsOutcome.print(txtMsg);
    smsOutcome.endSMS();
    Serial.println("SMS Sent !!!");
  }
}


void sendSMSDirect(char* cel, String msg){
  
  char txtMsg[200];
  msg.toCharArray(txtMsg,200);
  Serial.print("SMS message --> to: ");
  Serial.print(cel);
  Serial.print(" | msg: ");
  Serial.println(txtMsg);
    
  smsOutcome.beginSMS(cel);                    // send the message
  smsOutcome.print(txtMsg);
  smsOutcome.endSMS();
  Serial.println("SMS Sent !!!");
}




// Recibo los mensajes de texto que me llegan, los analizo y los voy borrando de la cola de mensajes 
void reciveSMS(){
  
  // Mensaje de Claro cuando te quedas sin crédito.. serviría para trapear???
  //[Claro] Credito insuficiente para navegar.
 
  int cs;
  int auxs=0;
  
  if(smsIncome.ready()){    
    if (int largo = smsIncome.available()) {                      // Chequea si tengo un mensaje ingresando
  
      lastSMSSenderN = "";                                        // GLOBAL - Ultimo SMS Sender Number recibido
      lastSMSRecibed = "";                                        // GLOBAL - Ultimo SMS Message
      
      char senderNumber[40];                                      // LOCAL - Celular que me esta mandando un mensaje income
      char inComeMsg[500];                                        // LOCAL - Mensaje recibio desde el SMS
  
        
      Serial.print("SMS Message received from: ");
      smsIncome.remoteNumber(senderNumber, 40);                   // Nro de telefono que me lo envía
      lastSMSSenderN = (String) senderNumber;
      Serial.println(lastSMSSenderN);
  
      // An example of message disposal
      if (smsIncome.peek() == '#') {                              // Any messages starting with # should be discarded
        Serial.println("SMS Discarded >> #");
        smsIncome.flush();
      }
  
      // Read message bytes and print them
      while ((cs = smsIncome.read()) != -1) {
        inComeMsg[auxs++] = cs;
      }
      inComeMsg[auxs++] = '\n';
    
      lastSMSRecibed = ((String) inComeMsg);
      lastSMSRecibed = lastSMSRecibed.substring(0,lastSMSRecibed.indexOf('\n'));
      
      Serial.println(lastSMSRecibed);
      Serial.println("END OF MESSAGE");
      
      smsIncome.flush();                                          // Delete message from modem memory
      Serial.println("MESSAGE DELETED");
      iconSMS = "IN";
      SMSRetencion = 0;

      commandExecuted = false;
    }
  }
}


void handleCommandSMS(String Command){

  Command.toLowerCase();


  if (Command.indexOf("red")>=0 && commandExecuted == false){
    Serial.println("COMMAND.. estado de la red");
    commandExecuted = true;
    String rta = "Estado de la red electrica de entrada ";
    rta = rta + '\n' + "VIN:"+ (String) VIN + "v, IIN:" + (String) IIN + "a, PIN:" + (String) PIN + "w" + '\n' + "d4i:" + myIMEI;
    char nCelu[20];
    lastSMSSenderN.toCharArray(nCelu,20);
    sendSMSDirect(nCelu, rta);
  }

  if (Command.indexOf("salida")>=0 && commandExecuted == false){
    Serial.println("COMMAND.. estado de la salida del inversor");
    commandExecuted = true;
    String rta = "Estado de la salida del inversor ";
    rta = rta + '\n' + "VOUT:"+ (String) VOUT + "v, IOUT:" + (String) IOUT + "a, POUT:" + (String) POUT + "w" + '\n' + "d4i:" + myIMEI;
    char nCelu[20];
    lastSMSSenderN.toCharArray(nCelu,20);
    sendSMSDirect(nCelu, rta);
  }

  if (Command.indexOf("bater")>=0 && commandExecuted == false){
    Serial.println("COMMAND.. estado de las baterias de Soporte");
    commandExecuted = true;
    String rta = "Baterias de Soporte del Inversor ";
    rta = rta + '\n' + "Bat01:"+ (String) bateria01 + "v, Bat02:" + (String) bateria02 + "v, Bat03:" + (String) bateria03 + "v, Bat04:" + (String) bateria04 + "v" + '\n' + "d4i:" + myIMEI;
    char nCelu[20];
    lastSMSSenderN.toCharArray(nCelu,20);
    sendSMSDirect(nCelu, rta);
  }
  
  if (Command.indexOf("safety")>=0 && commandExecuted == false){
    Serial.println("COMMAND.. estado de la Safety Battery");
    commandExecuted = true;
    String rta = "La bateria de respaldo tiene una carga de ";
    rta = rta + (String) iconBateria + "v" + '\n' + "d4i:" + myIMEI;
    char nCelu[20];
    lastSMSSenderN.toCharArray(nCelu,20);
    sendSMSDirect(nCelu, rta);
  }  


  if (Command.indexOf("reboot")>=0 && commandExecuted == false){
    Serial.println("COMMAND.. reboot de MKR1400");
    commandExecuted = true;
    int countdownMS = Watchdog.enable(1000);
    Serial.print("The watchdog will reset de MKR1400 in ");
    Serial.print(countdownMS, DEC);
    Serial.println(" milliseconds!");
  } 
    
}
