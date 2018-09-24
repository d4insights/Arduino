// Programita de Ejemplo básico para el uso de Librerías 
// Este solo reproduce SOS en código Morse sobre un LED conectado en el pin LED_BUILTIN
//


#include <DFSTools.h>

Morse morse(LED_BUILTIN);

void setup() {}

void loop()
{
   morse.dot(); morse.dot(); morse.dot();
   morse.dash(); morse.dash(); morse.dash();
   morse.dot(); morse.dot(); morse.dot();
   delay(3000);
}