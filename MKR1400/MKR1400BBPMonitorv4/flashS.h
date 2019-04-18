/*
 *  Libreria de manejo de la Flash Storage
 *  Guarda parametros en la Memoria persistente del MKR1400
 * 
 * 
 *  date: 15/04/2019
*/


#include <FlashStorage.h>

typedef struct {
  boolean valid;
  char telGuardia[20];
  char password[20];
} FMemory;


FlashStorage(my_flash_store, FMemory);              // Reservo un espacio de memoria flash
FMemory memFlash;                                   // Creo el objeto credenciales del tipo de la estructura FMemory

String myPassword = "";

void leeFlashStorage(){ 
  memFlash = my_flash_store.read();                 // Leo la estructura de la memoria que estaba guardada
  if(memFlash.valid == true) {                      // El elemento valid de la estructura la primera vez va a ser falso y luego lo reemplazo para saber que tengo datos
    for(int a=0; a<20; a++)
        celuGuardia[a] = memFlash.telGuardia[a];
  }
  myPassword = memFlash.password;
  
  Serial.print("Recuperé de FlashStorage <-- ");
  Serial.println(celuGuardia);
}


void escribeFlashStorage(){
  for(int a=0; a<20; a++)
    memFlash.telGuardia[a] = celuGuardia[a];
  memFlash.valid = true;
  myPassword.toCharArray(memFlash.password,20);

  my_flash_store.write(memFlash);                   // Guardo en la Flash los datos de la estructura
  Serial.print("Guardé en FlashStorage --> ");
  Serial.println(memFlash.telGuardia);
}
