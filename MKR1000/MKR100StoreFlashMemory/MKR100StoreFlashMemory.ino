/*
  Programa para almacenar el Usuario y Password de un SSID en la Flash
  Se carga el SSID y la PWD por el Serial Monitor (read)
  Hay que esperar 20 segundos para cargar cada variable
  
  OJO!!!!Se limpia cada vez que subo el programa
  
*/

#include <FlashStorage.h>

typedef struct {
  boolean valid;
  char ssid[100];
  char pwd[100];
} Autenticacion;


String mySSID = "";
String myPWD  = "";


FlashStorage(my_flash_store, Autenticacion);
  
Autenticacion memflash;  // Instanca del storage de memoria flash


void setup() {
  Serial.begin(9600);
  while (!Serial) { }



  // Read the content of "my_flash_store" into the "owner" variable
  memflash = my_flash_store.read();

  // La primera vez va a dar falso y esto sirve para disparar 
  // el proceso de carga desde el Serial Monitor del IDE de Arduino 
  if (memflash.valid == false) {

    // ...in this case we ask for user data.
    Serial.setTimeout(15000);
    Serial.println("Proceso de registro del clave ");
    Serial.println("   1. Ingrese el SSID y luego ENTER");
    Serial.println("   2. Esperar unos 10 segundos");
    Serial.println("   3. Ingrese el PWD y luego ENTER");
    Serial.println("  ");
    Serial.println("Ingrese el SSID: ");
    String SSID = Serial.readStringUntil('\n');
    
    Serial.println("Ingrese la Clave: ");
    String PWD = Serial.readStringUntil('\n');

    // Fill the "owner" structure with the data entered by the user...
    SSID.toCharArray(memflash.ssid, 100);
    PWD.toCharArray(memflash.pwd, 100);
    
    // set "valid" to true, so the next time we know that we
    // have valid data inside
    memflash.valid = true;

    // ...and finally save everything into "my_flash_store"
    my_flash_store.write(memflash);

    // Print a confirmation of the data inserted.
    Serial.println();
    Serial.print("SSID: ");
    Serial.println(memflash.ssid);
    Serial.print("PWD : ");
    Serial.println(memflash.pwd);
    Serial.println("los datos han sido almacenados en la Flash Memory!!");
  } 
  else
  {
    //mySSID = String(mySSID + memflash.ssid);
    //myPWD  = String(memflash.pwd);

    //for(int i=0; i<10; i++)
    //mySSID += memflash.ssid;
    
  }
      
}

void loop() {
  
    Serial.println();
    Serial.print("Autenticación: ");
    Serial.print(memflash.ssid);
    Serial.print(" - ");
    Serial.print(memflash.pwd);
    Serial.print(" | ");
    Serial.print(mySSID);
    Serial.print(" - ");
    Serial.print(myPWD);
    Serial.println("");

    delay(10000);
}

