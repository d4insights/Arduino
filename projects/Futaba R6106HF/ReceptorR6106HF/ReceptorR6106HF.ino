/*

    Programa: Lector del estado de un stick del receptor FUTABA R6106FH
              Leo el PIN 9 del Arduino donde pongo el cable de señal del Receptor
              Uso el Canal 1 del receptor (Alerones)
              
         Experimentalmente:
         el valor alto del switch del radio tira lecturas cercanas a 1900 microsegundos
         el valor medio tira 1500 microsegundos
         el valor bajo tira 1100 microsegundos
         
    Autor: Diego Fernandez Sanz
    Fecha: 04/01/2015
    
 */

#include <Servo.h> 

Servo myservo;
int pin = 2;
int val = 0;
unsigned int duration;



void setup()  
{ 
  Serial.begin(9600);
  pinMode(pin, INPUT);
  pinMode(9,OUTPUT);
  
  myservo.attach(9);                      		// Conecta el servo al pin 9 

  
}


void loop()  
{ 
   duration = pulseIn(pin, HIGH);               // Lee el ancho del pulso alto del ciclo 
   
   val = map(duration, 1086, 1913, 0, 170);    // Remapea la posición del stick que leyo para transladarla al angulo del servo correspondiente



   Serial.print("Ancho del Pulso --> ");
   Serial.print(duration);
   Serial.print(" --> ");
   Serial.println(val);      
   
    
   myservo.write(val);                    // setea el servo 
   delay(40);                           // Retardo visual para esperar que el servo se posicione (valor empírico)
   
   
 }

