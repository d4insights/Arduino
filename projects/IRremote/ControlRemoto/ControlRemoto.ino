#include <IRremote.h>

int RECV_PIN = 11;                          // define input pin on Arduino



IRrecv irrecv (RECV_PIN);
decode_results results;

void setup ()
{

  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  
  Serial.begin (9600);
  irrecv.enableIRIn ();                     // Start the receiver



}


void loop () {
 
  digitalWrite(2, HIGH);
  digitalWrite(3, LOW);
  digitalWrite(4, LOW);
  digitalWrite(5, HIGH);
  delay(15);
  digitalWrite(2, HIGH);
  digitalWrite(3, HIGH);
  digitalWrite(4, LOW);
  digitalWrite(5, LOW);
  delay(15);
  digitalWrite(2, LOW);
  digitalWrite(3, HIGH);
  digitalWrite(4, HIGH);
  digitalWrite(5, LOW);
  delay(15);
  digitalWrite(2, LOW);
  digitalWrite(3, LOW);
  digitalWrite(4, HIGH);
  digitalWrite(5, HIGH);
  delay(15);
  
  
  if (irrecv.decode (& results))
  {
    if (results.value != 4294967295 )
    {
      Serial.print (".");                        
    }
    irrecv.resume ();                       // Receive the next value
  }
}
