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

   La variable "modoMKR1010" va describiéndo en que punto del flujo esta como para ponerlo en una pantallita
       
*/

#include <SPI.h>
#include <WiFiNINA.h>
#include "sh1106I2C.h" 
#include "tools.h" 


void setup() {               
  startOled();                         // Inicia la pantalla OLED sh1106 y Splash Window de d4i 
  pinMode(LED_BUILTIN, OUTPUT);        // Uso el led builtin para señalar estado de actividad 
  intervalFlashingLed = 3000;          // Intervalo para prender y apagar el Led de la Placa
  Serial.begin(115200);
  delay(3000);
  Serial.println("Aplication Start....."); 
  wifiOled();                          // Loguito de WIFI
  validacionHardwareFirmaware();
  credenciales = my_flash_store.read();    // Recupero el contenido de la FlashStorage
}


void loop() {
  checkNetworkStatus();                   // Rutina de manejo de conexión y reenganche AP, Server Web o Cliente Web
  
   


  

  if (millis() - previousMillis >= intervalFlashingLed) {
      previousMillis = millis();
      digitalWrite(LED_BUILTIN, ledBuiltInStatus=!ledBuiltInStatus);
  }
  displayFooter(modoMKR1010);        // Imprime el estado en el Footer de la pantallita OLED
}
