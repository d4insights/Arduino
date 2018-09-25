// Programa 
//      LC Studio SD Card Read From File Tutorial
//      Ejemplo de como conecto un MKR100 con una SD para leer un archivo
//      de configuración del sistema que estará en la raíz del SD
//      en un archivo llamado config.txt
//
//      Connections:  
//          MOSI - pin D08 
//          MISO - pin D10 
//           CLK - pin D09 
//            CS - pin D12  (pata llamada SCL)



#include <SPI.h>
#include <SD.h>


int cs = 12;    
File myFile;                                    // Objeto Archivo
 
void setup()
{
  char myFileName[] = "config.txt";             // Nombre del archivo (si no existe lo crea)
  String LineString = "";  
  
  pinMode(cs, OUTPUT);
  pinMode(SS, OUTPUT);                          // No tocar esto lo definen las especificaciones de fabricante

  
  // Abre el puerto serie para el monitor
  Serial.begin(115200);
        while (!Serial) {
              
        }
  Serial.println("Initializing SD card...");
  Serial.println();
     
  // Controlo si la SD esta disponible
  if (!SD.begin(cs)) {
    Serial.println("SD did not initiliaze");   
    while (1) ;
  }
  
  Serial.println("SD initialized.");
  Serial.println();
  Serial.println("Reading file...");
  Serial.println();
 
  // Abro el archivo para lectura
  myFile = SD.open(myFileName, FILE_READ);
  
  // Leo línea a línea (hasta nueva línea \n)
  while (myFile.available() != 0) {  
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
