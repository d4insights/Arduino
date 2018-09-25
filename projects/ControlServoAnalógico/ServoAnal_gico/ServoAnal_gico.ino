/*
   Programa: ServoAnalógico
    
    Prueba de funcionamiento de los servos con la sentencia analógica para escribir la posición en microsegundos
  
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
   
   Serial.println("700");		   
   myservo.writeMicroseconds(700);  // set servo to low
   delay(2000);   // Retardo visual.. No es necesario porque el servo reacciona rápido lo mismo
   
   Serial.println("1500");		   
   myservo.writeMicroseconds(1500);  // set servo to mid-point
   delay(2000);   // Retardo visual.. No es necesario porque el servo reacciona rápido lo mismo

   Serial.println("2300");		   
   myservo.writeMicroseconds(2300);  // set servo to hight
   delay(2000);   // Retardo visual.. No es necesario porque el servo reacciona rápido lo mismo
   
   
   
}
