/*  
 *   Template básico de conexión a una WIFI mediante la utina de datos desde un AP 
 *   que luego guarda los datos en una FlashStorage para recuperarla cada vez que se prenda.
 *   OJO que cada vez que quemo el programa la FlashStorage se va a borrar
 *   
 *   1. Si se cae WiFi, intenta 10 veces reconectarse y si no puede se prende como AP
 *   2. Si se conecta como AP, nadie entra al Portal Web luego de 40 intentos. switcheo y trato de conectar al ROUTER (por si recupero Internet)
 *   Ojo, que en el caso 2., con el AP prendido alguien entra en el Portal WEB, deja de ciclar
 *   y tendría que apretar el botón de REBOOT para ciclar
 *   
 *   Si se conecta como AP y entro en el Portal hhtp://192.168.4.1 :
 *      - puedo borrar una SSID y PWD desde el portal con el botón ERASE del portal     
 *      - con el botón de QUIT salgo sin hacer nada (conservo el SSID y PWD)     
 *      - Si quiero rebootear el micreo presiono REBOOT 2 veces
 *   
 *   La variable "modoMKR1010" va describiéndo en que punto del flujo de conexión en que esta el MKR1010 como para ponerlo en una pantallita
 *   La variable "debugMode" permite setear si esta en Producción o en Desarrollo para saltear lo seteo de Claves    
 *   
*/

#include <SPI.h>
//#include <WiFiNINA.h>               // Esta línea hay que levantarla para el MKR1010
#include <WiFi101.h>                  // Esta línea hay que levantarla para el MKR1000
#include "sh1106I2C.h" 
#include "tools.h" 
#include "smtp2Go.h" 
#include "moduloAlterna.h" 
#include "mySQL.h" 

bool debugMode = false;               // True: Hardcodea el Seteo SSID y PWD para que no lo pida cada vez que compilo  

float lastVrms = 220;                 // Ultima lectura de la tensión para no reimprimir la OLED si no hace falta
bool flagCorteTension = false;        // false es que no estaba cortada la luz (estado para no mandar mil mails)
bool flagAltaTension  = false;        // false es que mandé mensaje de alta tensión (estado para no mandar mil mails)
bool flagBajaTension  = false;        // false es que mandé mensaje de baja tensión (estado para no mandar mil mails)
bool flagTensionNormalizada = false;

  
void setup() {               
  delay(100);
    
  startOled();                         // Inicia la pantalla OLED sh1106 y Splash Window de d4i 

  pinMode(LED_BUILTIN, OUTPUT);        // Uso el led builtin para señalar estado de actividad 
  intervalFlashingLed = 3000;          // Intervalo para prender y apagar el Led de la Placa
  Serial.begin(115200);
 
  Serial.println("Aplication Start....."); 
  wifiOled();                                // Loguito de WIFI
  validacionHardwareFirmaware();
 

  validacionModoDebug(debugMode);            // Confirmo si estoy en Producción o Desarrollo

}


void loop() {
  
  checkNetworkStatus();                   // Rutina de manejo de conexión y reenganche AP, Server Web o Cliente Web

  actualizacionLectura(15);               // Pido al NANO que me mande la última lectura que tenga en una trama

  if(Vrms > 1000){                         // Hasta que estabilice la lectura pongo el "SCANNING..." en la pantalla
    displayWaiting();
  }else{
  
      if(lastVrms != Vrms && Vrms < 500){
        if(Irms>=0)
              displayTensionMA(modoMKR1010, round(Vrms), round(Irms*1000));
        else
              displayTensionA(modoMKR1010, round(Vrms), round(Irms));
        Serial.print("Volts -> ");  
        Serial.print(Vrms);
        Serial.print(" | ");
        Serial.print("Ampers -> ");  
        Serial.print(Irms);
        Serial.print(" | ");
        lastVrms = Vrms;
        
        //guardarBaseDatos();                     // Mando el registro a guardar a la base de datos
        
        if(Vrms>215 && flagCorteTension == true){
            flagCorteTension = false;
            flagTensionNormalizada = true;
        }
        if(Vrms<260 && flagAltaTension == true){
            flagAltaTension = false;
            flagTensionNormalizada = true;
        }
        if(Vrms>215 && flagBajaTension == true){
            flagBajaTension = false;
            flagTensionNormalizada = true;
        } 
      }  
      
      guardarBaseDatos();                     // Mando el registro a guardar a la base de datos
      
      
      // Mensaje de SUMINISTROS RESTABLECIDO
      //  
      if(Vrms>=215 && Vrms <= 260 && flagTensionNormalizada == true){
        smtpTO      = "dfernandezsanz@itpatagonia.com";
        smtpSubject = "SUMINISTRO RESTABLECIDO";
        smtpText    = "Estimado Usuario, \n\nSe normalizó la tensión!!!. Zafó subir por la escalera otra vez\n\nQue suerte que tuvo!!!!.\n\nSaludos!!";
        if(sendEmail(smtpTO, smtpSubject, smtpText)){ 
            Serial.println(F("Email sent"));
            flagTensionNormalizada = false;
        } else { 
            Serial.println(F("Email failed"));
        }
      }


      // Mensaje de BAJA TENSION
      //
      if(Vrms<=215 && flagBajaTension == false){
        smtpTO      = "dfernandezsanz@itpatagonia.com";
        smtpSubject = "BAJA TENSION";
        smtpText    = "Estimado Usuario, \n\nLe queremos que hay una tensión por debajo de 200VAC.\n\nOjo con los aparatos que tengan motores!!.\n\nSaludos y suerte con eso";
        if(sendEmail(smtpTO, smtpSubject, smtpText)){ 
            Serial.println(F("Email sent"));
            flagBajaTension = true;
        } else { 
            Serial.println(F("Email failed"));
        }
      }
    
    
      // Mensaje de CORTE DE TENSION
      //  
      if(Vrms<=100 && flagCorteTension == false){
        smtpTO      = "dfernandezsanz@itpatagonia.com";
        smtpSubject = "CORTE DE LUZ";
        smtpText    = "Estimado Usuario, \n\nEste es un mensaje automatico para notificar que se cortó la luz y cuando vuelva va a tener que subir por las escaleras.\n\nQue cagadaaa.. chee !!!!.\n\nSaludos!!";
        if(sendEmail(smtpTO, smtpSubject, smtpText)){ 
            Serial.println(F("Email sent"));
            flagCorteTension = true;
        } else { 
            Serial.println(F("Email failed"));
        }
      }
    
     
    
      // Mensaje de ALTA TENSION
      //
      if(Vrms>=260 && flagAltaTension == false){
        smtpTO      = "dfernandezsanz@itpatagonia.com";
        smtpSubject = "ALTA TENSION";
        smtpText    = "Estimado Usuario, \n\nLe queremos notificar que esta habiéndo una tensión superior a 250VAC.\n\nOjo con la heladera!!!.\n\nSaludos!!";
        if(sendEmail(smtpTO, smtpSubject, smtpText)){ 
            Serial.println(F("Email sent"));
            flagAltaTension = true;
        } else { 
            Serial.println(F("Email failed"));
        }
       }
  }

  
  // Rutina de manejo del LED de estado de la placa MKR1010
  if (millis() - previousMillis >= intervalFlashingLed) {
      previousMillis = millis();
      digitalWrite(LED_BUILTIN, ledBuiltInStatus=!ledBuiltInStatus);
  }

  displayZocalo(modoMKR1010);
  //delay(1000);
  
}
