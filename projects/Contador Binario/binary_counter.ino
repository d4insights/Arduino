int button = 2;	// pin to connect the button
int presses = 0;	// variable to store number of presses
long time = 0;	// used for debounce
long debounce = 100;	// how many ms to "debounce"
const byte numPins = 8; // how many leds
int state;	// used for HIGH or LOW
// pins to connect leds
byte pins[] = {5, 6, 7, 8, 9, 10, 11, 12};
     
void setup ()
{
  /* we setup all led pins as OUTPUT */
  for(int i = 0; i < numPins; i++) {
    pinMode(pins[i], OUTPUT);
  }
  pinMode(button, INPUT);
  /* use pin 2 which has interrupt 0 on Arduino UNO */
  attachInterrupt(0, count, LOW);
}

void loop ()
{
  /* convert presses to binary and store it as a string */
  String binNumber = String(presses, BIN);
  /* get the length of the string */
  int binLength = binNumber.length();	
  if(presses <= 255) {	// if we have less or equal to 255 presses
  // here is the scary code
  for(int i = 0, x = 1; i < binLength; i++, x+=2) {
    if(binNumber[i] == '0') state = LOW;
    if(binNumber[i] == '1') state = HIGH;
    digitalWrite(pins[i] + binLength - x, state);
  }	
}
else {
  // do something when we reach 255
}
}     

/* function to count the presses */
void count() {
  // we debounce the button and increase the presses
  if(millis() - time > debounce)	presses++;
  time = millis();
}
