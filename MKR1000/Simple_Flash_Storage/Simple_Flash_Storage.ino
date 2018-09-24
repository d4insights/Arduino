#include <SPI.h>
#include <WiFi101.h>
#include <FlashStorage.h>

FlashStorage(MySSID, String);
FlashStorage(MyPWD, String);

String Usuario;
String Password;


void setup() {
  delay(1000);
  Serial.begin(9600);
  Serial.print("Inicializando Aplicación Flash");
  Serial.print("");
  Usuario  = MySSID.read();
  Password = MyPWD.read();
}

void loop() {
  if (Usuario == "") {
    MySSID.write("WRT1900AC");
    Serial.println("Usuario Guardado");
  }
  else{
    Serial.print(Usuario);
    Serial.print("");  
  }

  if (Password == "") {
    MyPWD.write("4CE9C5C6F6");
    Serial.println("Passw0rd Guardada");
  }
  else{
    Serial.print(Password);
    Serial.print("");  
  }

}
