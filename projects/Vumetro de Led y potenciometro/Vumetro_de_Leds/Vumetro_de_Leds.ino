/*
  Ejemplos de Manejo de Leds incrementales con un potenciometro
  Autor: Diego C. Fernandez Sanz
  Fecha: 27/12/2014

*/


int leds[]={2,3,4,5,6};            // Definición de todas las variables asociadas a los pines de control
int pot;                           // Definición de la variable que guardará el estado del potenciómetro
int n=0;                           // Variable auxiliar que utilizaremos como contador en el ciclos.

void setup(){
   for(n=0;n<5;n++)
  {
    pinMode(leds[n],OUTPUT);      // Define todos los puertos como salida
  }
  
  Serial.begin(9600);              //Iniciamos la comunicación con el puerto monitor serial
}


void loop(){
  pot = analogRead(A0);            // Lee el potenciometro con una lectura analógica desde el puerto A0

  //Monitorizamos el valor del potenciómetro con el serial.print seguido entre paréntesis de lo que queremos que nos muestre.
  Serial.print("Valor del potenciometro: ");
  Serial.print(pot);
  Serial.print(" \n");
  //delay(2000);


  
    // Ciclos para definir cuales de los leds se tienen que ir prendiendo
    // Todos apagados
    if(pot < 5){
     for(n=0;n<5;n++) 
       digitalWrite(leds[n],LOW);
    }

    // 1 Led endendido
    if(pot >= 5 && pot <= 200){         //Definimos el rango para el cual se encenderá el LED
       for(n=0;n<1;n++)                 //solo se enciende un LED, de ahí el número 1
         digitalWrite(leds[n],HIGH);    //Se encienden n LEDs, aquí n = 1
       for(n=1;n<5;n++)                 //Los demás LEDs están apagados
         digitalWrite(leds[n],LOW);
   } 


    // 2 Led endendidos
   if(pot > 200 && pot <= 400){
     for(n=0;n<2;n++) 
       digitalWrite(leds[n],HIGH);
     for(n=2;n<5;n++)
       digitalWrite(leds[n],LOW);
   } 
 
   // 3 Led endendidos
   if(pot > 400 && pot <= 600){
     for(n=0;n<3;n++) 
       digitalWrite(leds[n],HIGH);
     for(n=3;n<5;n++)
       digitalWrite(leds[n],LOW);
   }
   
   
   // 4 Led encendidos
   if(pot > 600 && pot <= 800){
     for(n=0;n<4;n++) 
       digitalWrite(leds[n],HIGH);
   for(n=4;n<5;n++)
       digitalWrite(leds[n],LOW);
   }
   
   // 5 Leds prendidos
   if(pot > 800){
     for(n=0;n<5;n++) 
       digitalWrite(leds[n],HIGH);
   }


 
 }

