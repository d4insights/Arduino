/*
    Programa: Read Reciber Write Servo Directo
    Lee un canal de 1 receptor y lo pasa directo a un servo
    Sirve para probar la velocidad de lectura mediante interrupciones y escritura.
    Es la base para después alterar los movimientos del servo de acuerdo a lo que se lee
    
    Auto: Diego Fernández Sanz
    Fecha: 09/01/2015
      
*/

#include <PinChangeInt.h>                          // http://playground.arduino.cc/Main/PinChangeInt
#include <PinChangeIntConfig.h>
#include <TimerOne.h>                              // http://playground.arduino.cc/Code/Timer1
#include <Servo.h>                                 // Librería para control del servo analógico

#define NO_PORTB_PINCHANGES                        // PinChangeInt setup
#define NO_PORTC_PINCHANGES                        
#define PIN_COUNT 1                                // CANTIDAD DE CANALES QUE VOY A LEER ... IMPORTANTE PONER SOLO LOS CONECTADOS!!!!                                                 
#define MAX_PIN_CHANGE_PINS PIN_COUNT              // los canales del receptor van conectados al PIN 2, 3, 4, 5....  Empezando siempre del PIN2 

#define RC_TURN 3                                  // Define constantes para nombrar los canales del receptor que estoy leyendo
#define RC_FWD 2
#define RC_FIRE 4


byte pin[] = {RC_FWD, RC_TURN, RC_FIRE};          // Array con los canales a utilizar
unsigned int time[] = {0,0,0};                    // Array para guardar los valores de tiempo de cada canal

byte state=0;                                     // Guarda el estado RISE o FALL del pulso
byte burp=0;                                      // Contador de la cantidad de ciclos que leyó
byte cmd=0;                                       // Guarda el valor que leyó del serial para pedirle leer el canal
byte i=0;                                         // contador básico para ciclos


Servo myservo;        			          // Instancia el objeto servo declarándolo en forma global  


void setup() {
    Serial.begin(115200);

    Timer1.initialize(2200);                      // largo del duty cycle                                               
    Timer1.stop();                                // frena el contador de interrupción
    Timer1.restart();                             // pone el timer en cero

    for (byte i=0; i<PIN_COUNT; i++)
    {
        pinMode(pin[i], INPUT);                           // Setear el pin como input
        digitalWrite(pin[i], HIGH);                       // activa el internal pullup resistor
    }
    
    myservo.attach(11);                      		// Conecta el servo al pin 9 
    
    PCintPort::attachInterrupt(pin[i], rise, RISING);     // Attacha la rutina de interrupción en el primer pin
     


}

void loop() 
{
        Serial.print("time:\t");
        for (byte i=0; i<PIN_COUNT;i++)
        {
            Serial.print(i,DEC);
            Serial.print(":");
            Serial.print(time[i],DEC);
            Serial.print("\t");
            
                                       
        }
        Serial.print(burp, DEC);
        Serial.println();

        //int val = map(time[i], 1095, 1933, 700, 2300);     // mapea los valores del receptor en el rango de valores del servo
        myservo.writeMicroseconds(2000);                    // setea el servo 
        delay(1000);                                          // Retardo visual para esperar que el servo se posicione (valor empírico)
            
           
           
    switch (state)
    {
        case RISING:                                                
            PCintPort::detachInterrupt(pin[i]);
            PCintPort::attachInterrupt(pin[i], fall, FALLING);     
            state=255;
            break;
        case FALLING:                                             
            PCintPort::detachInterrupt(pin[i]);
            i++;                                                   
            i = i % PIN_COUNT;                                    // i rango de 0 a PIN_COUNT
            PCintPort::attachInterrupt(pin[i], rise, RISING);
            state=255;
            break;
        default:
            // no hace nada
            break;
    }
}

void rise()                                                       
{
    Timer1.restart();                                             
    Timer1.start();                                               
    state=RISING;
    burp++;
}

void fall()                                                      
{
   state=FALLING;
   time[i]=Timer1.read();                                                                                           
   Timer1.stop();
}



