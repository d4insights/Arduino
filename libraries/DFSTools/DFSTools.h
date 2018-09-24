/*
DFSTools
	Librería de herramientas frecuentes de Diego Fernandez Sanz

	Clases Implementadas
		Morse -> Ejemplo para implementar Morse con un led (es el primer ejemplo)
		
*/

#ifndef DFSTools_h
#define DFSTools_h

#include "Arduino.h"	// Tipos estándar y constantes de Arduino (no se incluyen solas en las librerias por ende hay que ponerlo


// CLASE MORSE
// Sabe representar en el PIN que le pase en código morse
//
class Morse
{
  public: 

	Morse(int pin); 
	void dot();
   	void dash();
   
   private:
      int _pin;
};

// CLASE FORMATO
// Maneja el formateo de caracteres
// 
class Formato
{
   public:
	Formato();
	void rellena();

   private:

};

#endif