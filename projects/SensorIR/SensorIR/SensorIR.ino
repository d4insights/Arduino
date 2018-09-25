/*
    Programa:  Lectura de un sensor de IR

    Lee una señal analógica en el pin A0 que viene del sensor IR

    Autor:  Diego Fernández Sanz
    Fecha: 11/01/2015
    
    
*/

int IRPin = A0;  
int Estado = 0;

void setup() {
  Serial.begin(9600);
  pinMode(IRPin, INPUT);     // Setea como entrada el pin del sensor IR

}


void loop(){
  
  Estado = analogRead(IRPin);

  if (Estado > 500) {     
     
    Serial.print("ACTIVADO");
    Serial.print(" : ");
    Serial.println(Estado);
    
  } 
  else {
       
    Serial.print("--------");
    Serial.print(" : "); 
    Serial.println(Estado);
  }
}
