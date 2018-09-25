/* 
    Programa:
   
    Autor: Diego Fernández Sanz
    Fecha:    1/02/2015  

 */

 
#include <SoftwareSerial.h> 
 
 
#define RxD 10 
#define TxD 11 
#define PWR 5 
#define KEY 4 
 
 
SoftwareSerial BTSerial(RxD, TxD); 
 
 
void setup() 
{ 
   
  pinMode(PWR, OUTPUT); 
  pinMode(KEY, OUTPUT); 
  digitalWrite(PWR, LOW); 
  digitalWrite(KEY, HIGH); 
  digitalWrite(PWR, HIGH); 
    
  delay(500); 
    
  BTSerial.flush(); 
  delay(1000); 
  BTSerial.begin(9600); 
  Serial.begin(9600); 
  Serial.println("Enter AT commands:"); 
 
  BTSerial.print("Ready..\r\n"); 

 
 
} 
 
 
void loop() 
{ 
 
 
  if (BTSerial.available()) 
    Serial.write(BTSerial.read()); 

 
  if (Serial.available()) 
    BTSerial.write(Serial.read()); 
    
 
} 

