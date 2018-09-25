/*
    Programa:
    Lector de RC Receptor para todos los canales del radio (hoy esta dimensionado solo para 3 por razones de performance pero se extiende sin problemas)

    Auto: Diego Fernández Sanz
    Fecha: 09/01/2015
      
*/

#include <PinChangeInt.h>                          // http://playground.arduino.cc/Main/PinChangeInt
#include <PinChangeIntConfig.h>
#include <TimerOne.h>                              // http://playground.arduino.cc/Code/Timer1

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

void setup() {
    Serial.begin(115200);
//    Serial.print("PinChangeInt ReciverReading test");
//    Serial.println();                             //warm up the serial port

    Timer1.initialize(2200);                      //longest pulse in PPM is usally 2.1 milliseconds,
                                                  //pick a period that gives you a little headroom.
    Timer1.stop();                                //stop the counter
    Timer1.restart();                             //set the clock to zero

    for (byte i=0; i<PIN_COUNT; i++)
    {
        pinMode(pin[i], INPUT);                           //set the pin to input
        digitalWrite(pin[i], HIGH);                       //use the internal pullup resistor
    }
    PCintPort::attachInterrupt(pin[i], rise, RISING);       // attach a PinChange Interrupt to our first pin
}

void loop() 
{
//    cmd=Serial.read();                                     //while you got some time gimme a systems report
//    if (cmd=='p')
//    {
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
/*      Serial.print("\t");
        Serial.print(clockCyclesToMicroseconds(Timer1.pwmPeriod), DEC);
        Serial.print("\t");
        Serial.print(Timer1.clockSelectBits, BIN);
        Serial.print("\t");
        Serial.println(ICR1, DEC);*/
 //   }
    
 //   cmd=0;
    
    switch (state)
    {
        case RISING:                                               //we have just seen a rising edge
            PCintPort::detachInterrupt(pin[i]);
            PCintPort::attachInterrupt(pin[i], fall, FALLING);     //attach the falling end
            state=255;
            break;
        case FALLING:                                             //we just saw a falling edge
            PCintPort::detachInterrupt(pin[i]);
            i++;                                                  //move to the next pin
            i = i % PIN_COUNT;                                    //i ranges from 0 to PIN_COUNT
            PCintPort::attachInterrupt(pin[i], rise, RISING);
            state=255;
            break;
        default:
            //do nothing
            break;
    }
}

void rise()                                                      //on the rising edge of the currently intresting pin
{
    Timer1.restart();                                            //set our stopwatch to 0
    Timer1.start();                                              //and start it up
    state=RISING;
    //Serial.print('r');
    burp++;
}

void fall()                                                      //on the falling edge of the signal
{
   state=FALLING;
                                   
   time[i]=Timer1.read();                                       
                                                                
    Timer1.stop();
    //Serial.print('f');
}


