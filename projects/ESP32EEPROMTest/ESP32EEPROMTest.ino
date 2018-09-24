#include <EEPROM.h>

int a = 0;
char value;
//char botToken[46] = "511137579:AAGJuEQLaFrqVOJJyjHCtr_FT71Ms4Ye0KM";
char botToken[46];



void setup()
{
  Serial.begin(115200);

  EEPROM.begin(46);

  //writeBotTokenToEeprom(0);
  
}

void loop()
{
  value = EEPROM.read(a);

/*  Serial.print(a);
  Serial.print("\t");
  Serial.print(value);
  Serial.println();

  a = a + 1;

  if (a == 50)
    a = 0;
*/

  readBotTokenFromEeprom(0);
  Serial.println(botToken);

  delay(1000);

  

   
}



// Guarda el token del Bot en la EEProm
//
void writeBotTokenToEeprom(int offset){
  for(int i = offset; i<100; i++ ){
    EEPROM.write(i, botToken[i]);
  }
  EEPROM.commit();
}


// Lee el token del Bot que guarde en la EEProm
//
void readBotTokenFromEeprom(int offset){
  for(int i = offset; i<100; i++ ){
    botToken[i] = EEPROM.read(i);
  }
  EEPROM.commit();
}
