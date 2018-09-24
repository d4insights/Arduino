// Prueba de funcionamienot de la librería MORSE
// Instancia un objeto morse que tiene un método dot() y otro dash()
//

#include <Morse.h>

Morse morse(13);

void setup() {}

void loop()
{
   morse.dot(); morse.dot(); morse.dot();
   morse.dash(); morse.dash(); morse.dash();
   morse.dot(); morse.dot(); morse.dot();
   delay(3000);
}
