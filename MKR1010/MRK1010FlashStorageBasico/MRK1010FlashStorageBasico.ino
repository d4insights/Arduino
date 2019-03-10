
/*
  Store and retrieve structured data in Flash memory.

  Almacena en la Flash un usuario y una clave persistentes

  OJO...Cada vez que recompilo lo que tenía guardado se pierde !!!!!!!!!!!!!!!!!
   
*/


#include <FlashStorage.h>

typedef struct {
  boolean valid;
  char mySsid[100];
  char myPwd[100];
} FSecurity;

// Reservo un espacio de memoria flash 
FlashStorage(my_flash_store, FSecurity);


void setup() {
  Serial.begin(9600);
    
    while (!Serial) { }

  // Creo el objeto credenciales del tipo de la estructura FSecurity
  FSecurity credenciales;

  // Leo la estructura de la memoria que estaba guardada
  credenciales = my_flash_store.read();

  // El elemento valid de la estructura la primera vez va a ser falso y luego lo reemplazo para saber que tengo datos
  if (credenciales.valid == false) {

    
    Serial.setTimeout(30000);
    Serial.println("Insert the SSID:");
    String mySsid = Serial.readStringUntil('\n');
    Serial.println("Insert the Password:");
    String myPwd = Serial.readStringUntil('\n');

    // Lleno la estructura credenciales 
    mySsid.toCharArray(credenciales.mySsid, 100);
    myPwd.toCharArray(credenciales.myPwd, 100);
    credenciales.valid = true;                        // la pongo en verdadero para confimar que tengo datos en la Flash

    // Guardo en la Flash los datos de la estructura
    my_flash_store.write(credenciales);

    
    Serial.println();
    Serial.print("SSID: ");
    Serial.println(credenciales.mySsid);
    Serial.print("PWD : ");
    Serial.println(credenciales.myPwd);
    Serial.println("have been saved. Thank you!");

  } else {

    // acá pasa la segunda vez que entre porque sabe que los datos son válidos
    Serial.println();
    Serial.print("Hi, your credentials are: ");
    Serial.print(credenciales.mySsid);
    Serial.print(" | ");
    Serial.print(credenciales.myPwd);
    Serial.println(",jejeje :-)");

  }
}

void loop() {
  // Do nothing...
}
