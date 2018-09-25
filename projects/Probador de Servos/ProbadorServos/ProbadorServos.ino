/*
   Programa: ServoMotores
   Maneja la posición de un servo analógico a partir de la lextura de un potenciometro
   El poteciometro se conecta a al pin 0
   La señal del servo (cable blanco) va conectado al pin 9
  
   Artor: Diego Fernandez Sanz
   Fecha: 03/01/2015

*/

#include <Servo.h> 


 
Servo myservo;  			// Instancia el objeto servo 
 
int val;    				// Variable auxiliar para almacenar el valor de la lectura del potenciometro 

void setup() 
{ 
 Serial.begin(9600); 
 myservo.attach(9);  			// Conecta el servo al pin 9 
} 
 

void loop() 
{ 
   val = analogRead(A0);            // lee el potenciometro (valores entre 0 .. 1023) 
   Serial.print(val);		    // saco por el monitor serial el valor del poteciometro que leí	
   Serial.print(" --> ");
    
   
   val = map(val, 0, 1023, 700, 2300);  // calculo el ángulo en el que debe estar el servo matcheandolo con la posición del potenciometro 
   Serial.println(val);			// saco el valor del angulo del servo que acabo de calcular
   
   
   myservo.writeMicroseconds(val);     // mando al servo la nueva posición en la que debe estar
   
   delay(40);                         // Delay empirico para que no tiemble el servo y reaccione rápido                  
   
   
}
