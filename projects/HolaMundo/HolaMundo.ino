/*
  Programacion de LCD de 16 x 2 
  Escribe Hola Mundo en el LCD
  
  Autor: Diego Fernández Sanz
  Fecha: 28/12/2014 
 
 PINUP del Circuito con el Arduino UNO
 
 con el shield de Arduino
 * LCD RS pin to digital         PIN8
 * LCD Enable pin to digital     PIN9
 * LCD D4 pin to digital         PIN4
 * LCD D5 pin to digital         PIN5
 * LCD D6 pin to digital         PIN6
 * LCD D7 pin to digital         PIN7 
 * LCD R/W pin to ground         GND
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)

 con el LCD 16 X 2 de WINSTAR directo 
 * LCD RS pin to digital         PIN12
 * LCD Enable pin to digital     PIN11
 * LCD D4 pin to digital         PIN5
 * LCD D5 pin to digital         PIN4
 * LCD D6 pin to digital         PIN3
 * LCD D7 pin to digital         PIN2 
 * LCD R/W pin to ground         GND
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)


 */

// include the library code:
#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins para el SHIELD de ARDUINO
//LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

// initialize the library with the numbers of the interface pins para el LCD WINSTAR 16 X 2
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);


void setup() {
  // set up the LCD's number of columns and rows: 
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("Hola Mundo!!!!");
}

void loop() {
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  lcd.setCursor(0, 1);
  // print the number of seconds since reset:
  lcd.print(millis()/1000);
}

