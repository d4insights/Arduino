
/*
     Programa: Controlador de Alarmas diarias, semanales y timers
        
               0.  Capas de operar con el RTC
               1.  Si existe una conexion serial activa se sincroniza con la PC
               2.  Permite setear múltiple alarmas semanales o diarias
               3.  Permite setear timers de ejecución automática o de ejecución de única vez
         
     Autor: Diego Fernández Sanz
     Fecha: 08/02/2015    
 */

#include <TimeAlarms.h> 
#include <Time.h>


void setup()
{
  Serial.begin(9600);
  setTime(8,29,0,1,1,11);                               // Setea el reloj interno -- > Saturday 8:29:00am Jan 1 2011
  
  // create the alarms 
  Alarm.alarmRepeat(8,30,0, MorningAlarm);              // 8:30am every day
  Alarm.alarmRepeat(17,45,0,EveningAlarm);              // 5:45pm every day 
  Alarm.alarmRepeat(dowSaturday,8,30,30,WeeklyAlarm);   // 8:30:30 every Saturday 

 
  Alarm.timerRepeat(15, Repeats);                      // timer for every 15 seconds    
  Alarm.timerOnce(10, OnceOnly);                       // called once after 10 seconds 
}

void  loop(){  
  digitalClockDisplay();                               // Muestra la hora del reloj
  Alarm.delay(1000);                                   // Espere de 1 segundo para mostrar
}




// functions to be called when an alarm triggers:
void MorningAlarm(){
  Serial.println("Alarm: - turn lights off");    
}

void EveningAlarm(){
  Serial.println("Alarm: - turn lights on");           
}

void WeeklyAlarm(){
  Serial.println("Alarm: - its Monday Morning");      
}

void ExplicitAlarm(){
  Serial.println("Alarm: - this triggers only at the given date and time");       
}

void Repeats(){
  Serial.println("15 second timer");         
}

void OnceOnly(){
  Serial.println("This timer only triggers once");  
}

void digitalClockDisplay()
{
  // digital clock display of the time
  printDigits(hour());
  Serial.print(":");
  printDigits(minute());
  Serial.print(":");
  printDigits(second());
  Serial.println(); 
}

void printDigits(int digits)
{
  if(digits < 10)
    Serial.print('0');
  Serial.print(digits);
}

