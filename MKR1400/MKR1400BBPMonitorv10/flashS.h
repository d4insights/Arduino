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
  char deltaVIN[5];
  char deltaVOUT[5];
  char deltaIIN[5];
  char deltaIOUT[5];
  char deltaBAT0[5];
  char deltaBAT1[5];
  char deltaBAT2[5];
  char deltaBAT3[5];
  char deltaBAT4[5];
} FMemory;


FlashStorage(my_flash_store, FMemory);              // Reservo un espacio de memoria flash
FMemory memFlash;                                   // Creo el objeto credenciales del tipo de la estructura FMemory

String myPassword  = "";


void leeFlashStorage(){ 
  memFlash = my_flash_store.read();                 // Leo la estructura de la memoria que estaba guardada
  
  if(memFlash.valid == true) {                      // El elemento valid de la estructura la primera vez va a ser falso y luego lo reemplazo para saber que tengo datos
    for(int a=0; a<20; a++)
        celuGuardia[a] = memFlash.telGuardia[a];
  }
  
  myPassword = memFlash.password;

  mydeltaVIN  = memFlash.deltaVIN;
  mydeltaVOUT = memFlash.deltaVOUT;
  mydeltaIIN  = memFlash.deltaIIN;
  mydeltaIOUT = memFlash.deltaIOUT;
  mydeltaBAT0 = memFlash.deltaBAT0;
  mydeltaBAT1 = memFlash.deltaBAT1;
  mydeltaBAT2 = memFlash.deltaBAT2;
  mydeltaBAT3 = memFlash.deltaBAT3;
  mydeltaBAT4 = memFlash.deltaBAT4;

  
  Serial.print("Recuperé de FlashStorage <-- ");
  Serial.println(celuGuardia);
}


void escribeFlashStorage(){
  
  for(int a=0; a<20; a++)
    memFlash.telGuardia[a] = celuGuardia[a];
  memFlash.valid = true;
  
  myPassword.toCharArray(memFlash.password,20);
  
  mydeltaVIN.toCharArray(memFlash.deltaVIN,5);
  mydeltaVOUT.toCharArray(memFlash.deltaVOUT,5);
  
  mydeltaBAT0.toCharArray(memFlash.deltaBAT0,5);
  mydeltaBAT1.toCharArray(memFlash.deltaBAT1,5);
  mydeltaBAT2.toCharArray(memFlash.deltaBAT2,5);
  mydeltaBAT3.toCharArray(memFlash.deltaBAT3,5);
  mydeltaBAT4.toCharArray(memFlash.deltaBAT4,5);
  
  my_flash_store.write(memFlash);                   // Guardo en la Flash los datos de la estructura
  Serial.print("Guardé en FlashStorage --> ");
  Serial.println(memFlash.telGuardia);
}
