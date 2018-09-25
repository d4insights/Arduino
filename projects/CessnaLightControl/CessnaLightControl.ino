#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiServer.h>
#include <WiFiUdp.h>

// Cessna Light Control (PULSEIN) 
// Autor: Diego Fernández Sanz
// Fecha: 02/07/2017
// Función:
//
//  1. Lee en el PIN 13 la señal del receptor FUTABA correspondiente al canal compartido con la alimentación
//
//  2. Analiza la posición de la llave a la que esta conectada al canal del receptor que esta leyendo en 1.
//      Posición A -->  Bajo  --> APAGADO       pulso devuelto -->  +/- 1100
//      Posición B -->  Medio --> NAVEGACION    pulso devuelto -->  +/- 1500
//      Posición C -->  Alto  --> NAV+PARKING   pulso devuelto -->  +/- 1900
//
//  3. A partir de la posición de la llave del radio que leyó acciona los comportamientos  
//
//  4. Comportamiento NAVEGACION
//      Alerna luces de las punteras de alas, panza y timón de cola
//
//  5. Comportamiento NAV+PARKING
//      A las luces de navegación agrega la luz de parking blanca en las ruedas del avión


int pinINPUTrc = 13;        // PIN donde voy a conectar la señal del canal del receptor a leer
unsigned long duration;     // Duración del pulso que me va a devolver PULSEIN
int alternaLuces = 0;       // Manejador de la secuencia de encentido de las luces para hacer la conbinación 0..5 
int fadeLuces = 0;          // Manejador del atenuado de la luz de Timón y de Panza del avión
int incrFade = 40;          // Incrementos en el ciclo de FADE que deberían ser múltimplos de 5 [5..255]
int retardoCiclo = 500;     // Delay de todo el ciclo para atenuar la velocidad de Alternancia y de Fading

int pinNavIZQ=2;
int pinNavDER=3;
int pinTimon=4;
int pinPanza=5;
int pinParking=6;
int pinAuxA=7;               // Auxiliar que se prende conjuntamente con la luz de Parking
int pinAuxB=8;               // Auxiliar que se prende siempre que la llave esté en MEDIUM o HIGH

void setup() {

  
  pinMode(pinINPUTrc, INPUT);      // Definición del pin de lectura como INPUT
  
  pinMode(pinNavIZQ,   OUTPUT);    // PIN de salida de luces
  pinMode(pinNavDER,   OUTPUT);
  pinMode(pinTimon,    OUTPUT);
  pinMode(pinPanza,    OUTPUT);
  pinMode(pinParking,  OUTPUT);
  pinMode(pinAuxA,  OUTPUT);
  pinMode(pinAuxB,  OUTPUT);

  

  Serial.begin(9600);              // Inicio el monitor serial para testear el ancho del pulso del receptor

}



void loop() {
  
  duration = pulseIn(pinINPUTrc, HIGH);             // Lectura del pin del receptor
  
  // Saco por el monitor serial la data de las principales variables
  Serial.print("Pulse: ");
  Serial.print(duration, DEC);                     
  Serial.print('\t');
  Serial.print(fadeLuces); 
  Serial.print('\t');
  Serial.println(incrFade);

  // En base a la duración del flanco alto del pulso determino en que posición esta la llave del radio
  //
  // Llave en la posición LOW
  if(duration<1300)                         
  {
      digitalWrite(pinNavIZQ,LOW);
      digitalWrite(pinNavDER,LOW);
      digitalWrite(pinTimon,LOW);
      digitalWrite(pinPanza,LOW);
      digitalWrite(pinParking,LOW);  
      digitalWrite(pinAuxA,LOW);  
      digitalWrite(pinAuxB,LOW);  
      
  } 

  // Llave en la posición MEDIUM o HIGH
  if (duration>1500)                   
  {
      // Combinaciones de luces de navegación que van alternando
      if (alternaLuces==0 || alternaLuces==1 || alternaLuces==4 || alternaLuces==5)
      {
        digitalWrite(pinNavDER,HIGH);
        digitalWrite(pinNavIZQ,LOW);
        
      }
      if (alternaLuces==2 || alternaLuces==3 || alternaLuces==6 || alternaLuces==7)
      {
        digitalWrite(pinNavDER,LOW);
        digitalWrite(pinNavIZQ,HIGH);
      }

      if (alternaLuces==0|| alternaLuces==2 || alternaLuces==4 || alternaLuces==6)
      {
        digitalWrite(pinTimon,HIGH); 
      }

      if (alternaLuces==1|| alternaLuces==3 || alternaLuces==5 || alternaLuces==7)
      {
        digitalWrite(pinTimon,LOW); 
      }
       
      // Fade de las luces que encienden y apagan lento
      analogWrite(pinPanza,fadeLuces); 

      // Auxiliar B debe quedar prendido siempre en MEDIUM y HIGH
      digitalWrite(pinAuxB,HIGH);

      // El la posición mas alta de la llave además prendo las luces de PARKING
      //
      if (duration>1900)
      {
        digitalWrite(pinParking,HIGH);
        digitalWrite(pinAuxA,HIGH);
      }
      else
      {
        digitalWrite(pinParking,LOW);
        digitalWrite(pinAuxA,LOW);
      }

      // Retardo para alternar las luces
      delay(retardoCiclo);
      
  }



  // Incrementa el secuenciador de luces para dar los efectos de combinación 
  //
  if(alternaLuces++ > 7)  
  {
    alternaLuces=0;
  }


  // Incrementa y decrementa el ciclo de FADE
  //
 
  fadeLuces = fadeLuces + incrFade;

  if (fadeLuces < 0)               
  {
    fadeLuces = 0;
    incrFade = -incrFade;
  }

  if (fadeLuces > 255)
  {
    fadeLuces = 255;
    incrFade = -incrFade;
  }
  
  
}
