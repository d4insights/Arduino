
// LC Studio SD Card Read From File Tutorial
//Connections:  MOSI - pin 11, MISO - pin 12, CLK - pin 13, CS - pin 10

#include <SPI.h>

#include <SD.h>


int cs = 10; // Set Chip Select to pin ten
File myFile;  // a File Object
 
void setup()
{
  //
  char myFileName[] = "config.txt";  // The name of the file we will create
  String LineString = "";  
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    
  }
  Serial.println("Initializing SD card...");
  Serial.println();
  
  pinMode(cs, OUTPUT);
  
  // Documentation says you're supposed to do this
  // even if you don't use it:
  pinMode(SS, OUTPUT);
   
  // see if the card is present and can be initialized:
  if (!SD.begin(cs)) {
    Serial.println("SD did not initiliaze");   
    while (1) ;
  }
  Serial.println("SD initialized.");
  Serial.println();
  Serial.println("Reading file...");
  Serial.println();
 
  // Open our File for Reading
  myFile = SD.open(myFileName, FILE_READ);
  
  // Keep Reading String until there are no more
  while (myFile.available() != 0) {  
    // read the string until we have a newline
    // Careful on using this where you don't have newlines.
    LineString = myFile.readStringUntil('\n');
    Serial.println(LineString);       
  } 
 myFile.close();
 
 Serial.println();
 Serial.println("Done");  
} 
  
void loop()
{
  
}
