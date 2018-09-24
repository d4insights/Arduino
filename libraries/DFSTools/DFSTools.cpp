/*
DFSTools
	Librería de herramientas frecuentes de Diego Fernandez Sanz

	Clases Implementadas
		Morse 	-> Ejemplo para implementar Morse con un led (es el primer ejemplo)
		Formato -> Maneja visualizaciones para el serial print 		
*/

#include "Arduino.h"
#include "DFSTools.h"



// CLASE MORSE
// Implementa la clase Morse 
//
Morse::Morse(int pin)
{
   pinMode(pin, OUTPUT);
   _pin = pin;
   Serial.println("Instancié Morse");
}

void Morse::dot()
{
   digitalWrite(_pin, HIGH); 
   delay(250);
   digitalWrite(_pin, LOW); 
   delay(250);
   Serial.println("dot");
}

void Morse::dash()
{
   digitalWrite(_pin, HIGH); 
   delay(1000);
   digitalWrite(_pin, LOW); 
   delay(250);
   Serial.println("dash");
}

// CLASE FORMATO
// Maneja visualizaciones por serial print
//
Formato::Formato()
{
	Serial.println("Constructor de Formato");
}

void Formato::rellena()
{
	//Serial.print("Intentos..");
	//Serial.println("..");
}