/*
 *   Libreria de uso de SMS por medio de la red de 4g
 * 
 *   Manda mensajes de texto por 4G (lo considera siempre conectado)
 * 
 * 
 * 
 *   Ojo con los mensajes que te manda la compañía de celular !!!
 *   SMS Message received from: 773
 *   [Claro]Credito insuficiente para enviar el SMS.Responde 35 y te prestamos $35. Ahora podes pedir tu Presta y comprar un Pack a solo un clic en claro.co
 *   END OF MESSAGE
 * 
*/




unsigned long ultimoSMSSafetyBatery  = 600000;                 // Auxiliar para no mandar SMS todo el tiempo ante un fallo (así manda cada 10 min)
long intervalSMSSafetyBatery         = 600000;                 // Manejo del período de tiempo entre SMS (así manda cada 10 min)

unsigned long ultimoSMSInversorBatery  = 600000;               // Auxiliar para no mandar SMS todo el tiempo ante un fallo (así manda cada 10 min)
long intervalSMSInversorBatery         = 600000;               // Manejo del período de tiempo entre SMS (así manda cada 10 min)

unsigned long ultimoSMSRed            = 600000;               // Auxiliar para no mandar SMS todo el tiempo ante un fallo (así manda cada 10 min)
long intervalSMSRed                   = 600000;               // Manejo del período de tiempo entre SMS (así manda cada 10 min)


#include <MKRGSM.h>
#include <Adafruit_SleepyDog.h>   

char celuGuardia[20]       = "*11111111111111111";    // Celular de Guardia pata mandar SMS
String newCeluGuardia = "";                           // Almacena cualquier nuevo celular de guardia cambiado por SMS

String lastSMSSenderN = "";                           // Ultimo SMS Sender Number recibido
String lastSMSRecibed = "";                           // Ultimo SMS Message

GSM_SMS smsOutcome;                                   // Objeto SMS de salida
GSM_SMS smsIncome;                                    // Objeto SMS de entrada




// Mensajes temporizados de Alertas en la Red Electrica
//
void sendSMSTemporizedRed(char* cel,String msg){
 if(cel[0] != '*'){
      if (millis() - ultimoSMSRed >= intervalSMSRed) {
        
        ultimoSMSRed = millis();
         
        char txtMsg[200];
        msg.toCharArray(txtMsg,200);
        Serial.print("SMS message RED --> to: ");
        Serial.print(cel);
        Serial.print(" | msg: ");
        Serial.println(txtMsg);
          
        smsOutcome.beginSMS(cel);                    // send the message
        smsOutcome.print(txtMsg);
        smsOutcome.endSMS();
        Serial.println("SMS Sent !!!");
      }
  
  }
}



// Mensajes temporizados de Alertas en la Safety Battery
//
void sendSMSTemporizedSBat(char* cel,String msg){

 if(cel[0] != '*'){
     
     if (millis() - ultimoSMSSafetyBatery >= intervalSMSSafetyBatery) {
        
        ultimoSMSSafetyBatery = millis();
         
        char txtMsg[200];
        msg.toCharArray(txtMsg,200);
        Serial.print("SMS message SAFETY BATTERY--> to: ");
        Serial.print(cel);
        Serial.print(" | msg: ");
        Serial.println(txtMsg);
          
        smsOutcome.beginSMS(cel);                    // send the message
        smsOutcome.print(txtMsg);
        smsOutcome.endSMS();
        Serial.println("SMS Sent !!!");
      }
  }
}



// Mensajes temporizados de Alertas en las baterías del Inversor
//
void sendSMSTemporizedIBat(char* cel,String msg){
 if(cel[0] != '*'){ 
     if (millis() - ultimoSMSInversorBatery >= intervalSMSInversorBatery) {
        
        ultimoSMSInversorBatery = millis();
         
        char txtMsg[200];
        msg.toCharArray(txtMsg,200);
        Serial.print("SMS message INVERSOR BATTERIES --> to: ");
        Serial.print(cel);
        Serial.print(" | msg: ");
        Serial.println(txtMsg);
          
        smsOutcome.beginSMS(cel);                    // send the message
        smsOutcome.print(txtMsg);
        smsOutcome.endSMS();
        Serial.println("SMS Sent !!!");
      }
  }
}



// Envía notificaciones SMS en forma directa y sin repetición
//
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
//
void reciveSMS(){
  
  // Mensaje de Claro cuando te quedas sin crédito.. serviría para trapear???
  //[Claro] Credito insuficiente para navegar.
  // Para saber si la tarjeta tiene saldo:
  //    Mandar un SMS al 611 (sin el asterisco)
  //    Poner en el cuerpo del mensaje SALDO
  //    Te devuelve otro SMS con el saldo 
  //
   
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
  
//      // An example of message disposal
//      if (smsIncome.peek() == '#') {                              // Any messages starting with # should be discarded
//        Serial.println("SMS Discarded >> #");
//        smsIncome.flush();
//      }
  
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



// Manejo de los mensajes SMS recibido. Los clasifca por KeyWords para responder
// si no interpreta ninguna keyword responde con un mensaje genérico de ofrecimiento de ayuda
//
void handleCommandSMS(String Command){

  bool conteste = false;

  Command.toLowerCase();

  if ((Command.indexOf("entrada")>=0 && commandExecuted == false) || (Command.indexOf("red")>=0 && commandExecuted == false) ){
    Serial.println("COMMAND.. estado de la entrada red");
    commandExecuted = true;
    String rta = "Estado de la red electrica de entrada ";
    rta = rta + '\n' + "VIN:"+ (String) VIN + "v, IIN:" + (String) IIN + "a, PIN:" + (String) PIN + "w" + '\n' + "d4i:" + myIMEI;
    char nCelu[20];
    lastSMSSenderN.toCharArray(nCelu,20);
    sendSMSDirect(nCelu, rta);
    conteste = true;
  }

  if (Command.indexOf("salida")>=0 && commandExecuted == false){
    Serial.println("COMMAND.. estado de la salida del inversor");
    commandExecuted = true;
    String rta = "Estado de la salida del inversor ";
    rta = rta + '\n' + "VOUT:"+ (String) VOUT + "v, IOUT:" + (String) IOUT + "a, POUT:" + (String) POUT + "w" + '\n' + "d4i:" + myIMEI;
    char nCelu[20];
    lastSMSSenderN.toCharArray(nCelu,20);
    sendSMSDirect(nCelu, rta);
    conteste = true;
  }

  if (Command.indexOf("bater")>=0 && commandExecuted == false){
    Serial.println("COMMAND.. estado de las baterias de Soporte");
    commandExecuted = true;
    String rta = "Baterias de Soporte del Inversor ";
    rta = rta + '\n' + "Bat01:"+ (String) bateria01 + "v, Bat02:" + (String) bateria02 + "v, Bat03:" + (String) bateria03 + "v, Bat04:" + (String) bateria04 + "v" + '\n' + "d4i:" + myIMEI;
    char nCelu[20];
    lastSMSSenderN.toCharArray(nCelu,20);
    sendSMSDirect(nCelu, rta);
    conteste = true;
  }
  
  if ((Command.indexOf("conting")>=0 && commandExecuted == false) || (Command.indexOf("safety")>=0 && commandExecuted == false)){
    Serial.println("COMMAND.. estado de la Safety Battery");
    commandExecuted = true;
    String rta = "La bateria de respaldo tiene una carga de ";
    rta = rta + (String) iconBateria + "v" + '\n' + "d4i:" + myIMEI;
    char nCelu[20];
    lastSMSSenderN.toCharArray(nCelu,20);
    sendSMSDirect(nCelu, rta);
    conteste = true;
  }  


  if ((Command.indexOf("reinic")>=0 && commandExecuted == false) || (Command.indexOf("reboot")>=0 && commandExecuted == false)){
    Serial.println("COMMAND.. reboot de MKR1400");
    commandExecuted = true;

    char nCelu[20];
    lastSMSSenderN.toCharArray(nCelu,20);
    sendSMSDirect(nCelu, "Reiniciando el dispositivo en 5 segundos!!");
   
    int countdownMS = Watchdog.enable(5000);
    Serial.print("The watchdog will reset de MKR1400 in ");
    Serial.print(countdownMS, DEC);
    Serial.println(" milliseconds!");
    conteste = true;
  } 

  if (Command.indexOf("alertassms=apagar")>=0 && commandExecuted == false){
    Serial.println("COMMAND.. Apagar las alertas de SMS");
    commandExecuted = true;
    alertaSMS = false;
    
    char nCelu[20];
    lastSMSSenderN.toCharArray(nCelu,20);
    
    sendSMSDirect(nCelu, "Usted ha apagado las alertas SMS. Continuare repondiendo pero no enviare alertas");
    conteste = true;
  } 

  if (Command.indexOf("alertassms=prender")>=0 && commandExecuted == false){
    Serial.println("COMMAND.. Prender las alertas de SMS");
    commandExecuted = true;
    alertaSMS = true;
    
    char nCelu[20];
    lastSMSSenderN.toCharArray(nCelu,20);
    
    sendSMSDirect(nCelu, "Usted ha prendido las alertas SMS");
    conteste = true;
  }
  
  if ((Command.indexOf("ayuda")>=0 && commandExecuted == false) || (Command.indexOf("help")>=0 && commandExecuted == false)){
    Serial.println("COMMAND.. help");
    commandExecuted = true;
    String rta = "Lista de Keywords";
    rta = rta + '\n' + "Entrada" + '\n' + "Salida" + '\n' + "Baterias" + '\n' + "Reboot" + '\n' + "Ayuda" + '\n' + "d4i:" + myIMEI;
    char nCelu[20];
    lastSMSSenderN.toCharArray(nCelu,20);
    sendSMSDirect(nCelu, rta);
    conteste = true;
  }

  if ((Command.indexOf("coord")>=0 && commandExecuted == false) || (Command.indexOf("ubic")>=0 && commandExecuted == false)){
    Serial.println("COMMAND.. Coordenadas del GPS");
    commandExecuted = true;
    String rta = "Mi ubicacion actual es:";
    rta = rta + '\n' + myLocation + '\n' + "d4i:" + myIMEI;
    char nCelu[20];
    lastSMSSenderN.toCharArray(nCelu,20);
    sendSMSDirect(nCelu, rta);
    conteste = true;
  }


  int x = Command.indexOf("guardia=");
  Command = Command.substring(x+8, Command.length());
  if ( x >=0 && commandExecuted == false){
    Serial.print("COMMAND.. Celular Guardia: ");
    Serial.println(Command);
    commandExecuted = true;
    String rta = "Usted a cambiado el celular de guardia por el: ";
    rta = rta + '\n' + Command + '\n' + "d4i:" + myIMEI;
    
    Command.toCharArray(celuGuardia,20);       // Cambio en celular de guardia
    newCeluGuardia = celuGuardia;
     
    char nCelu[20];
    lastSMSSenderN.toCharArray(nCelu,20);
    sendSMSDirect(nCelu, rta);
    conteste = true;
  }


  if (conteste == false && commandExecuted == false && (lastSMSSenderN.length()>= 8) && (lastSMSSenderN.indexOf("Filo.")<0) ){
    Serial.println("COMMAND.. No supe contestar nada !!!");
    commandExecuted = true;
    String rta = "Puedes enviarme un SMS con la palabra <AYUDA> y con gusto te voy a guiar.";
    rta = rta + '\n' + "d4i:" + myIMEI;
    char nCelu[20];
    lastSMSSenderN.toCharArray(nCelu,20);
    sendSMSDirect(nCelu, rta);
  }
    
}
