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


#include <MKRGSM.h>

char celuGuardia[20]       = "1167577019";            // Celular de Guardia pata mandar SMS


String lastSMSSenderN = "";                           // Ultimo SMS Sender Number recibido
String lastSMSRecibed = "";                           // Ultimo SMS Message

GSM_SMS smsOutcome;                                   // Objeto SMS de salida
GSM_SMS smsIncome;                                    // Objeto SMS de entrada



void sendSMSTemporized(char* cel,String msg){
  
 if (millis() - ultimoSMSSafetyBatery >= intervalSMSSafetyBatery) {
    
    ultimoSMSSafetyBatery = millis();
     
    char txtMsg[200];
    msg.toCharArray(txtMsg,200);
    Serial.print("Message SMS --> to: ");
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
  Serial.print("Message SMS --> to: ");
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
  
     
  if (smsIncome.available()) {                                  // Chequea si tengo un mensaje ingresando

    lastSMSSenderN = "";                                        // GLOBAL - Ultimo SMS Sender Number recibido
    lastSMSRecibed = "";                                        // GLOBAL - Ultimo SMS Message
    
    char senderNumber[40];                                      // LOCAL - Celular que me esta mandando un mensaje income
    char inComeMsg[500];                                        // LOCAL - Mensaje recibio desde el SMS

      
    Serial.print("Message received from: ");
    smsIncome.remoteNumber(senderNumber, 40);                   // Nro de telefono que me lo envía
    lastSMSSenderN = (String) senderNumber;
    Serial.println(lastSMSSenderN);

    // An example of message disposal
    if (smsIncome.peek() == '#') {                              // Any messages starting with # should be discarded
      Serial.println("Discarded SMS #");
      smsIncome.flush();
    }

    // Read message bytes and print them
    while ((cs = smsIncome.read()) != -1) {
      inComeMsg[auxs++] = cs;
    }
  
    lastSMSRecibed = ((String) inComeMsg)+ '\n';
    lastSMSRecibed.substring(0,auxs);
    
    Serial.println(lastSMSRecibed);
    Serial.println("END OF MESSAGE");
    
    smsIncome.flush();                                          // Delete message from modem memory
    Serial.println("MESSAGE DELETED");
    iconSMS = "IN";
    SMSRetencion = 0;
  }
}
