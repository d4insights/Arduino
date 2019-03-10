/*  Template básico de conexión a una WIFI mediante la utina de datos desde un AP 
   que luego guarda los datos en una FlashStorage para recuperarla cada vez que se prenda.
   OJO que cada vez que quemo el programa la FlashStorage se va a borrar

   1. Si se cae WiFi, intenta 10 veces reconectarse y si no puede se prende como AP
   2. Si se conecta como AP, nadie entra al Portal Web luego de 40 intentos. switcheo y trato de conectar al ROUTER (por si recupero Internet)
   Ojo, que en el caso 2., con el AP prendido alguien entra en el Portal WEB, deja de ciclar 
   y tendría que apretar el botón de REBOOT para ciclar
   
   Si se conecta como AP y entro en el Portal hhtp://192.168.4.1 :
          - puedo borrar una SSID y PWD desde el portal con el botón ERASE del portal
          - con el botón de QUIT salgo sin hacer nada (conservo el SSID y PWD)
          - Si quiero rebootear el micreo presiono REBOOT 2 veces

   La variable "modoMKR1010" va describiéndo en que punto del flujo de conexión en que esta el MKR1010 como para ponerlo en una pantallita
   La variable "debugMode" permite setear si esta en Producción o en Desarrollo para saltear lo seteo de Claves    

*/

#include <SPI.h>
#include <WiFiNINA.h>
#include "sh1106I2C.h" 
#include "tools.h" 
#include "smtp2Go.h" 


void setup() {               
  delay(100);
  
  startOled();                         // Inicia la pantalla OLED sh1106 y Splash Window de d4i 
  pinMode(LED_BUILTIN, OUTPUT);        // Uso el led builtin para señalar estado de actividad 
  intervalFlashingLed = 3000;          // Intervalo para prender y apagar el Led de la Placa
  Serial.begin(115200);
  
  Serial.println("Aplication Start....."); 
  wifiOled();                                // Loguito de WIFI
  validacionHardwareFirmaware();
  bool debugMode = true;                     // True: Hardcodea el Seteo SSID y PWD para que no lo pida cada vez que compilo  
  validacionModoDebug(debugMode);            // Confirmo si estoy en Producción o Desarrollo

}


void loop() {
  checkNetworkStatus();                   // Rutina de manejo de conexión y reenganche AP, Server Web o Cliente Web
  



  //Rutina de email 
  //
  smtpTO      = "dfernandezsanz@itpatagonia.com";
  smtpSubject = "MKR1010 SMTP auto sender...";
  smtpText    = "Estimado Usuario, \n\nEste es un mensaje automatico para notificar un evento del MKR1010.\nCualquier cosa no dude en resolverlo.\n\nSaludos!!";
  
  byte inChar;
  inChar = Serial.read();  
  if(inChar == 'e')
  {
  if(sendEmail(smtpTO, smtpSubject, smtpText)) 
      Serial.println(F("Email sent"));
  else 
      Serial.println(F("Email failed"));
  }


  
  // Rutina de manejo del LED de estado de la placa MKR1010
  if (millis() - previousMillis >= intervalFlashingLed) {
      previousMillis = millis();
      digitalWrite(LED_BUILTIN, ledBuiltInStatus=!ledBuiltInStatus);
  }

  
  displayFooter(modoMKR1010);        // Imprime el estado en el Footer de la pantallita OLED
}
