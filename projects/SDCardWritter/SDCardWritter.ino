// LC Studio SD Card Create and Write to File Tutorial
//Connections:  MOSI - pin 11, MISO - pin 12, CLK - pin 13, CS - pin 10

 
#include <SD.h>
#include <SPI.h>
 
int cs = 10; // Set Chip Select to pin ten

File myFile;  // a File Object
 
void setup()
{
  //
  char myFileName[] = "MyFile.txt";  // The name of the file we will create
  
  
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
 
 
 // Lets check to make sure that the SD card doesn't already have our file
  if (! SD.exists(myFileName)){
     // This next statement will open a file for writing if it exists
     // If it does not exist, it will create that file. That's what we're doing here.
     myFile = SD.open(myFileName, FILE_WRITE); 
     // This next statement checks to see if the file
     myFile.println("My 1st Line of Data");  // Send Your First Line to that file
     myFile.flush();  // Save it.
  }
  else{
    // We got here because the file already exists.
    // Therefore we're simple opening the file and writing to it. We will add another line at the end.   
    myFile = SD.open(myFileName, FILE_WRITE); 
    myFile.println("Another Line of Data");  // Send Your First Line to that file
    myFile.flush();
    
  }
  
  Serial.println("Done Writing");
  
}

void loop()
{

}

