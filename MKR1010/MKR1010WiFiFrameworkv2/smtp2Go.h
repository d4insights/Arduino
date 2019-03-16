/*
 *  Librería para mandar eMails desde el MKR1010
 *  Usa un servicio de SMTP en la nube llamado SMTP2GO
 *  Usuario: user@d4insights.com / diego@fernandezsanz.com
 *  Pwd: t..0 para ambas
 *  Conversor a Base64 = https://www.opinionatedgeek.com/codecs/base64encoder
 *  
 *  
*/


String smtpTO         = "dfernandezsanz@itpatagonia.com";
String smtpSubject    = "defina el subject";
String smtpText       = "defina el cuerpo del mensaje";

char serversmtp[] = "mail.smtp2go.com"; 
int port = 2525; // You can also try using Port Number 25, 8025 or 587.

WiFiClient clientsmtp;



// Rutina de manejo de errores recibidos desde el SMTP Server de SMTP2GO
//
void efail(){
  byte thisByte = 0;
  int loopCount = 0;
  clientsmtp.println(F("QUIT"));
  while(!clientsmtp.available()) {
    delay(1);
    loopCount++;

    // if nothing received for 10 seconds, timeout
    if(loopCount > 10000) {
      clientsmtp.stop();
      Serial.println(F("\r\nTimeout"));
      return;
    }
  }
  while(clientsmtp.available()){ 
    thisByte = clientsmtp.read(); 
    Serial.write(thisByte);
  }
  clientsmtp.stop();
  Serial.println(F("disconnected"));
}


// Rutina de manejo de respuestas de parte del Server de SMTP
//
byte eRcv(){
  byte respCode;
  byte thisByte;
  int loopCount = 0;
  while(!clientsmtp.available()) {
    delay(1);
    loopCount++;
    if(loopCount > 10000) {                 // if nothing received for 10 seconds, timeout
      clientsmtp.stop();
      Serial.println(F("\r\nTimeout"));
      return 0;
    }
  }
  respCode = clientsmtp.peek();
  while(clientsmtp.available()) { 
    thisByte = clientsmtp.read(); 
    Serial.write(thisByte);
  }
  if(respCode >= '4') {
    efail();
    return 0; 
  }
  return 1;
}


// Rutina principal de envío del MAIL
//
byte sendEmail(String myTO, String mySubject, String myText){
  byte thisByte = 0;
  byte respCode;
  
  // Conexión con el SMTP Server SMTO2GO
  //
  if(clientsmtp.connect(serversmtp,port) == 1) {
    Serial.println(F("connected"));
  } else {
    Serial.println(F("connection failed"));
    return 0;
  }
  if(!eRcv()) return 0;

  // Protocolo de inicio de comunicación
  //  
  Serial.println(F("Sending hello"));
  clientsmtp.print("EHLO ");                         
  clientsmtp.println(WiFi.localIP());
  if(!eRcv()) return 0;

  // Protocolo de Autenticación
  //
  Serial.println(F("Sending auth login"));
  clientsmtp.println("auth login");
  if(!eRcv()) return 0;
  Serial.println(F("Sending User"));
  //clientsmtp.println(F("ZGllZ29AZmVybmFuZGV6c2Fuei5jb20=")); // diego@fernandezsanz.com     Change to your base64 encoded user
  clientsmtp.println(F("dXNlckBkNGluc2lnaHRzLmNvbQ=="));       // user@d4insights.com  Change to your base64 encoded user
  if(!eRcv()) return 0;
  Serial.println(F("Sending Password")); 
  //clientsmtp.println(F("VmphTVhTMHREQWdV"));  // diego@fernandezsanz.com     Change to your base64 encoded user
  clientsmtp.println(F("OWdDY0FjYzRSMGhm"));    // user@d4insights.com  Change to your base64 encoded user
  if(!eRcv()) return 0;

  // change to your email address (sender)
  Serial.println(F("Sending From"));
  clientsmtp.println("MAIL From: <user@d4insights.com>");
  if(!eRcv()) return 0;

  // change to recipient address
  Serial.println(F("Sending To"));
  clientsmtp.print("RCPT To: <");
  clientsmtp.print (myTO);
  clientsmtp.println(">");
  if(!eRcv()) return 0;

  Serial.println(F("Sending DATA"));
  clientsmtp.println("DATA");
  if(!eRcv()) return 0;

  Serial.println(F("Sending email"));

  // change to recipient address
  clientsmtp.print("To:");
  clientsmtp.print(myTO.substring(0,myTO.indexOf("@")));
  clientsmtp.print("<");
  clientsmtp.print(myTO);
  clientsmtp.println(">");

  // change to your address
  clientsmtp.println("From: d4iRobot <user@d4insights.com>");

  clientsmtp.print("Subject: ");
  clientsmtp.println(mySubject);

  clientsmtp.println(myText);
  clientsmtp.println();
  clientsmtp.println();
  clientsmtp.println(".");
  if(!eRcv()) return 0;

  Serial.println(F("Sending QUIT"));
  clientsmtp.println("QUIT");
  if(!eRcv()) return 0;

  clientsmtp.stop();

  Serial.println(F("disconnected"));

  return 1;
}
