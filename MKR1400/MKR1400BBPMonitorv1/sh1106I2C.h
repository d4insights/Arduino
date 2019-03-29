/*
    Librería tiny de U8g2 para manejar la pantalla SH1106 con I2C
    Resolución 128X64
    Monocromo 
  
*/

#include <U8g2lib.h>


#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

// Definición de las variables de ICONOS de Estados del HEADER
String modoMKR1400    = "4g disconnected";       // Mensaje general
bool iconAlerta       = false;                   // Alerte true or false
String iconSMS        = "";                      // Mail OUT, IN o vacio para que no aparezca
bool iconSincro       = false;                   // Sincronización con Internet true or false
int iconSenal         = 0;                       // Señal de 4G true or false Varía entre 0 y 31 (mejor)
float iconBateria     = 0;                       // Voltaje de la Batería auxiliar del MKR1400
float lastIconBateria = 0;                       // Memoriza ultima lectura de bateria para no hacer un display si no cambio el valor



/*
  U8g2lib Example Overview:
    Frame Buffer Examples: clearBuffer/sendBuffer. Fast, but may not work with all Arduino boards because of RAM consumption
    Page Buffer Examples: firstPage/nextPage. Less RAM usage, should work with all Arduino boards.
    U8x8 Text Only Example: No RAM usage, direct communication with display controller. No graphics, 8x8 Text only.
    
*/

// Please UNCOMMENT one of the contructor lines below
// U8g2 Contructor List (Frame Buffer)
// The complete list is available here: https://github.com/olikraus/u8g2/wiki/u8g2setupcpp
// Please update the pin numbers according to your setup. Use U8X8_PIN_NONE if the reset pin is not connected
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);



// ICONOS UTILES
//               https://github.com/olikraus/u8g2/wiki/fntgrpiconic
//
//   u8g2.setFont(u8g2_font_open_iconic_email_1x_t);
//   u8g2.drawStr(5,20,"@");                              // mail cerrado
//   u8g2.drawStr(15,20,"A");                             // mail abierto
//   u8g2.setFont(u8g2_font_open_iconic_embedded_1x_t);
//   u8g2.drawStr(5,30,"@");                              // batería baja
//   u8g2.drawStr(15,30,"I");                             // batería llena
//   u8g2.drawStr(25,30,"G");                             // warning
//   u8g2.drawStr(35,30,"A");                             // campanita
//   u8g2.setFont(u8g2_font_open_iconic_other_1x_t);
//   u8g2.drawStr(5,40,"F");                              // señal celular
//   u8g2.setFont(u8g2_font_open_iconic_www_1x_t);
//   u8g2.drawStr(5,50,"Q");                              // señal wifi
//   u8g2.setFont(u8g2_font_open_iconic_arrow_1x_t);
//   u8g2.drawStr(15,50,"W");                             // Sincronizando


// LOGO d4i
#define d4i_Logo_width 52
#define d4i_Logo_height 36
static const unsigned char d4i_Logo_bits[] U8X8_PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, 0x01, 
  0x00, 0x07, 0x00, 0x00, 0x00, 0xE0, 0x01, 0x80, 0x07, 0x00, 0x00, 0x00, 
  0xE0, 0x01, 0xC0, 0x07, 0x00, 0x00, 0x00, 0xE0, 0x01, 0xC0, 0x07, 0x00, 
  0x00, 0x00, 0xE0, 0x01, 0xE0, 0x07, 0x00, 0x00, 0x00, 0xE0, 0x01, 0xF0, 
  0x07, 0x00, 0x00, 0xE0, 0xEF, 0x01, 0xF0, 0x07, 0x00, 0x00, 0xF0, 0xFF, 
  0x01, 0xB8, 0x07, 0x00, 0x00, 0xF8, 0xFF, 0x01, 0x9C, 0x07, 0x00, 0x00, 
  0x3C, 0xF8, 0x01, 0x9E, 0x07, 0x00, 0x00, 0x0E, 0xE0, 0x01, 0x8F, 0x07, 
  0x00, 0x03, 0x0E, 0xE0, 0x81, 0x87, 0x07, 0x00, 0x06, 0x0F, 0xE0, 0x81, 
  0x83, 0x07, 0x00, 0x03, 0x0F, 0xE0, 0xC1, 0x81, 0x07, 0x80, 0x01, 0x0F, 
  0xE0, 0xE1, 0x80, 0x07, 0x00, 0x00, 0x0F, 0xE0, 0xE1, 0x80, 0x07, 0x00, 
  0x00, 0x0F, 0xE0, 0x71, 0x80, 0x07, 0x00, 0x00, 0x0F, 0xE0, 0xF1, 0xFF, 
  0x7F, 0x30, 0x00, 0x0F, 0xE0, 0xF1, 0xFF, 0x7F, 0x28, 0x00, 0x0E, 0xE0, 
  0x01, 0x80, 0x07, 0x34, 0x00, 0x0E, 0xE0, 0x01, 0x80, 0x07, 0x1C, 0x00, 
  0x1C, 0xE0, 0x01, 0x80, 0x07, 0x0A, 0x00, 0x3C, 0xF8, 0x01, 0x80, 0x07, 
  0x0E, 0x00, 0xF8, 0xFF, 0x01, 0x80, 0x07, 0x07, 0x00, 0xF0, 0xDF, 0x01, 
  0x80, 0x07, 0x06, 0x03, 0xC0, 0x03, 0x00, 0x00, 0x80, 0x83, 0x01, 0x00, 
  0x00, 0x00, 0x00, 0x80, 0xEF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x75, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0E, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

};

#define bbp_Logo_width 78
#define bbp_Logo_height 35
static const unsigned char bbp_Logo_bits[] U8X8_PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0xF0, 0xFF, 0x5F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xFF, 
  0xFF, 0xFF, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, 0xFF, 0xFF, 0xFF, 
  0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0x07, 
  0x00, 0x00, 0x00, 0x80, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x5F, 0x00, 0x00, 
  0x00, 0xE0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0xF0, 
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x07, 0x00, 0x00, 0xFC, 0xFF, 0xFF, 
  0xFF, 0xFF, 0xFF, 0xFF, 0x17, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
  0xFF, 0xFF, 0x3F, 0x00, 0x80, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
  0x56, 0x00, 0xC0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0x00, 
  0xC0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0x01, 0xC0, 0xFF, 
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0xC0, 0xFF, 0xFF, 0xFF, 
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0xC0, 0xFF, 0xFF, 0xFF, 0xFF, 0xBF, 
  0xFF, 0xDD, 0x5F, 0x02, 0x80, 0xFF, 0xFF, 0xFF, 0xF7, 0xF7, 0xFF, 0xBD, 
  0xDD, 0x01, 0x80, 0xFF, 0xFF, 0x7F, 0x00, 0x00, 0x88, 0xDA, 0x6F, 0x01, 
  0x80, 0xFF, 0xFF, 0x07, 0x00, 0x00, 0x00, 0xBA, 0xE1, 0x01, 0x00, 0xFF, 
  0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, 0xAD, 0x00, 0x00, 0xFF, 0x0F, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x5A, 0x00, 0x00, 0xFE, 0x03, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x7C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00,
  };




void displayBorraPantallaCompleta(){
  u8g2.clear();   
  u8g2.clearBuffer();   
  u8g2.sendBuffer(); 
}

void displayBorraBodyPantalla(){
  u8g2.setDrawColor(0);
  u8g2.drawBox(0,11,128,53);
  u8g2.setDrawColor(1);          
  u8g2.sendBuffer(); 
}

void displayBorraHeaderPantalla(){
  u8g2.setDrawColor(0);
  u8g2.drawBox(0,0,128,11);
  u8g2.setDrawColor(1);          
  u8g2.sendBuffer(); 
}

void displayWaitingStartUP(){
  displayBorraBodyPantalla();
  u8g2.setFont( u8g2_font_helvB08_tf);          
  u8g2.drawStr(9,40,"Starting up services...");      
  u8g2.setFont(u8g2_font_timR08_tr);            
  u8g2.drawStr(114,62,"d4");
  u8g2.setFont(u8g2_font_5x7_tf);
  u8g2.drawStr(124,63,"i"); 
  u8g2.sendBuffer();   
  
  
}

void displayHeader(String texto, bool alerta, String sms, bool sincro, int senal, float bateria){  
   
  char buff[128];
  texto.toCharArray(buff,128);
  
  u8g2.setDrawColor(0);
  u8g2.drawBox(0,0,128,11);
  u8g2.setDrawColor(1);  
                     
  u8g2.setFontMode(1);                       
  u8g2.setFont(u8g2_font_6x10_tf);            
  u8g2.drawStr(0,7,buff);                   
  

  // ICONO de Alertas
  if(alerta){
  u8g2.setFont(u8g2_font_open_iconic_embedded_1x_t);
  u8g2.drawStr(80,8,"A");                             // campanita
  }


  // ICONO de Mail
  if(sms=="OUT"){
  u8g2.setFont(u8g2_font_open_iconic_email_1x_t);
  u8g2.drawStr(90,7,"@");                             // Mail saliente
  }
  if(sms=="IN"){
  u8g2.setFont(u8g2_font_open_iconic_email_1x_t);
  u8g2.drawStr(90,8,"A");                             // Mail Recibido
  }

  // ICONO de Sincronización con Internet 
  if(sincro){
    u8g2.setFont(u8g2_font_open_iconic_arrow_1x_t);
    u8g2.drawStr(100,8,"W");                            // Sincronizando Internet OK       
  } else {
    u8g2.setFont(u8g2_font_open_iconic_arrow_1x_t);
    u8g2.drawStr(100,8,"W");                            // NO Sincranizando
    u8g2.drawLine(100, 0, 108, 8); 
  }

  // ICONO de Señal del Celular
  if (senal <= 0){
    u8g2.setFont(u8g2_font_open_iconic_other_1x_t);
    u8g2.drawStr(111,8,"F");                            // señal celular tachada
    u8g2.drawLine(111, 0, 118, 8); 
  }
  if(senal > 0 && senal <=10){
    u8g2.setFont(u8g2_font_open_iconic_other_1x_t);
    u8g2.drawStr(111,8,"F");                            // señal celular UNA RAYITA    
    u8g2.setDrawColor(0);
    u8g2.drawBox(113,0,6,8);
    u8g2.setDrawColor(1);             
  } 
  
  if (senal > 10 && senal <=20){
    u8g2.setFont(u8g2_font_open_iconic_other_1x_t);
    u8g2.drawStr(111,8,"F");                            // señal celular DOS RAYITAS    
    u8g2.setDrawColor(0);
    u8g2.drawBox(115,0,4,8);
    u8g2.setDrawColor(1);
  }

  if (senal > 20 && senal <=25){
    u8g2.setFont(u8g2_font_open_iconic_other_1x_t);
    u8g2.drawStr(111,8,"F");                            // señal celular TRES RAYITAS   
    u8g2.setDrawColor(0);
    u8g2.drawBox(117,0,2,8);
    u8g2.setDrawColor(1); 
  }

  if (senal > 25){
    u8g2.setFont(u8g2_font_open_iconic_other_1x_t);
    u8g2.drawStr(111,8,"F");                            // señal celular CUATRO RAYITAS    
  }

  // ICONO de Nivel de Bateria
  if(bateria <= 2.7){
    u8g2.setFont(u8g2_font_open_iconic_embedded_1x_t);
    u8g2.drawStr(120,8,"@");                            // batería vacía
    u8g2.drawLine(120, 0, 128, 8); 
  } 

  if(bateria > 2.7 && bateria <= 3.4){
    u8g2.setFont(u8g2_font_open_iconic_embedded_1x_t);
    u8g2.drawStr(120,8,"@");                            // batería 25%
    u8g2.drawBox(120,2,2,4);
  }

  if(bateria > 3.4 && bateria <= 3.8){
    u8g2.setFont(u8g2_font_open_iconic_embedded_1x_t);
    u8g2.drawStr(120,8,"@");                            // batería 50%
    u8g2.drawBox(120,2,3,4);
  }

  if(bateria > 3.8 && bateria <= 4.10){
    u8g2.setFont(u8g2_font_open_iconic_embedded_1x_t);
    u8g2.drawStr(120,8,"@");                            // batería 75%
    u8g2.drawBox(120,2,5,4);
  }
    
  if (bateria > 4.10){
    u8g2.setFont(u8g2_font_open_iconic_embedded_1x_t);
    u8g2.drawStr(120,8,"I");                            // batería llena
  }


  u8g2.drawHLine(0, 10, 128);                 // Dibuja una línea horizontal
  u8g2.sendBuffer();                          // transfer internal memory to the display
}


// Inicia el OLED y pone el icono de d4i
void startOled(){

   // see http://blog.squix.org/2015/05/esp8266-nodemcu-how-to-create-xbm.html
   // on how to create xbm files
   
   u8g2.begin();                                // Inicia el display  
   u8g2.enableUTF8Print();
   u8g2.clear();                           
   u8g2.drawXBMP(36, 10, d4i_Logo_width, d4i_Logo_height, d4i_Logo_bits); 
   u8g2.setFont( u8g2_font_helvB08_tf);          
   u8g2.drawStr(14,62,"www.d4insghts.com");      
   u8g2.sendBuffer();   
   delay(1000);                        
}


// Logo de bbp
//
void drawImagebbp() {

   u8g2.begin();                                // Inicia el display  
   u8g2.enableUTF8Print();
   u8g2.clear();                           
   u8g2.drawXBMP(24, 7, bbp_Logo_width, bbp_Logo_height, bbp_Logo_bits); 
   u8g2.setFont( u8g2_font_helvB08_tf);          
   u8g2.drawStr(37,55,"BBP Group");      
   u8g2.setFont(u8g2_font_timR08_tr);            
   u8g2.drawStr(114,62,"d4");
   u8g2.setFont(u8g2_font_5x7_tf);
   u8g2.drawStr(124,63,"i"); 
   u8g2.sendBuffer();   
   delay(2000); 
}


void displayReloj(String dia, String hora){   

  char cHora[10];
  hora.toCharArray(cHora,10);

  char cDia[10];
  dia.toCharArray(cDia,10);
  
  u8g2.setDrawColor(0);
  u8g2.drawBox(0,11,128,53);
  u8g2.setDrawColor(1);                        
  u8g2.setFontMode(1);                       
  u8g2.setFont(u8g2_font_fub17_tn);   
  u8g2.drawStr(17,40,cDia);
  u8g2.drawStr(36,64,cHora);                   

  u8g2.sendBuffer();                           // transfer internal memory to the display
}

void displayBateryLevel(float iconBateria){
  
  if(iconBateria < 1.0)
    iconBateria = 0.0;
  
  if(lastIconBateria != iconBateria){
    char volts[10];
    String aux= String(iconBateria);
    aux.toCharArray(volts,10);
    
    displayBorraBodyPantalla();

    u8g2.setFontMode(1);   
    u8g2.setFont(u8g2_font_6x10_tf);   
    u8g2.drawStr(5,40,"Internal");
    u8g2.drawStr(5,50,"Battery");
    u8g2.drawStr(120,50,"v");
    u8g2.drawLine(60, 25, 60, 55); 
    u8g2.setFont(u8g2_font_fub17_tn);   
    u8g2.drawStr(70,50,volts);
    u8g2.sendBuffer();         
    lastIconBateria = iconBateria;
    
  }
}
