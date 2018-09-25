
/*
      Programa: Lector de Clock Externo
      Lee el RTC y es capáz de ajustarlo a la fecha hora que tiene la PC vía el puerto serial (ver línea RTC.adjust)
            
      Autor: Diego C. Fernández Sanz
      Fecha: 08/02/2015
*/

#include <Wire.h> 
#include <RTClib.h>



RTC_DS1307 RTC;

const char* days[] = {"Domingo", "Lunes", "Martes", "Miércoles", "Jueves", "Viernes", "Sábado"};

void setup () {
  Wire.begin();                                     // Inicia el puerto I2C
  RTC.begin();                                      // Inicia la comunicación con el RTC
  RTC.adjust(DateTime(__DATE__, __TIME__));         // Establece la fecha y hora (Comentar una vez establecida la hora)
  Serial.begin(9600);                               // Establece la velocidad de datos del puerto serie
}

void loop(){
  DateTime now = RTC.now();                         // Obtiene la fecha y hora del RTC
  
  PrintDigits(now.day());                           // Día
  Serial.print('/');
  PrintDigits(now.month());                         // Mes
  Serial.print('/');
  PrintDigits(now.year());                          // Año
  Serial.print(' ');
  PrintDigits(now.hour());                          // Hora
  Serial.print(':');
  PrintDigits(now.minute());                        // Minuto
  Serial.print(':');
  PrintDigits(now.second());                        // Segundo
  Serial.print("   -->  ");
  Serial.println(days[now.dayOfWeek()]);            // Día de la Semana en formato de Texto
  
  delay(1000);                                      // La información se actualiza cada 1 seg.
}


// Acomoda los digitos de 1 digito agregando el cero delante
void PrintDigits(int digits){
  if(digits < 10)
    Serial.print('0');
  Serial.print(digits);
}
