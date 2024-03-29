/*
    Librería tiny de U8g2 para manejar la pantalla SH1106 con I2C
    Resolución 128X64
    Monocromo 
  
*/

#include <U8g2lib.h>


#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

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


#define WiFi_Logo_width 60
#define WiFi_Logo_height 36
static const unsigned char WiFi_Logo_bits[] U8X8_PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xFF, 0x07, 0x00, 0x00, 0x00,
  0x00, 0x00, 0xE0, 0xFF, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0xFF,
  0x7F, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFC, 0xFF, 0xFF, 0x00, 0x00, 0x00,
  0x00, 0x00, 0xFE, 0xFF, 0xFF, 0x01, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF,
  0xFF, 0x03, 0x00, 0x00, 0x00, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00,
  0x00, 0xFF, 0xFF, 0xFF, 0x07, 0xC0, 0x83, 0x01, 0x80, 0xFF, 0xFF, 0xFF,
  0x01, 0x00, 0x07, 0x00, 0xC0, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x0C, 0x00,
  0xC0, 0xFF, 0xFF, 0x7C, 0x00, 0x60, 0x0C, 0x00, 0xC0, 0x31, 0x46, 0x7C,
  0xFC, 0x77, 0x08, 0x00, 0xE0, 0x23, 0xC6, 0x3C, 0xFC, 0x67, 0x18, 0x00,
  0xE0, 0x23, 0xE4, 0x3F, 0x1C, 0x00, 0x18, 0x00, 0xE0, 0x23, 0x60, 0x3C,
  0x1C, 0x70, 0x18, 0x00, 0xE0, 0x03, 0x60, 0x3C, 0x1C, 0x70, 0x18, 0x00,
  0xE0, 0x07, 0x60, 0x3C, 0xFC, 0x73, 0x18, 0x00, 0xE0, 0x87, 0x70, 0x3C,
  0xFC, 0x73, 0x18, 0x00, 0xE0, 0x87, 0x70, 0x3C, 0x1C, 0x70, 0x18, 0x00,
  0xE0, 0x87, 0x70, 0x3C, 0x1C, 0x70, 0x18, 0x00, 0xE0, 0x8F, 0x71, 0x3C,
  0x1C, 0x70, 0x18, 0x00, 0xC0, 0xFF, 0xFF, 0x3F, 0x00, 0x00, 0x08, 0x00,
  0xC0, 0xFF, 0xFF, 0x1F, 0x00, 0x00, 0x0C, 0x00, 0x80, 0xFF, 0xFF, 0x1F,
  0x00, 0x00, 0x06, 0x00, 0x80, 0xFF, 0xFF, 0x0F, 0x00, 0x00, 0x07, 0x00,
  0x00, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0x01, 0x00, 0x00, 0xF8, 0xFF, 0xFF,
  0xFF, 0x7F, 0x00, 0x00, 0x00, 0x00, 0xFE, 0xFF, 0xFF, 0x01, 0x00, 0x00,
  0x00, 0x00, 0xFC, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0xFF,
  0x7F, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, 0xFF, 0x1F, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x80, 0xFF, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFC,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  };


void displayFooter(String texto){  


    
  char buff[128];
  texto.toCharArray(buff,128);
  
  //u8g2.clear();   
  u8g2.clearBuffer();                         
  u8g2.setFontMode(1);                       
  u8g2.setFont(u8g2_font_6x10_tf);            
  u8g2.drawStr(0,62,buff);                   
  u8g2.setFont(u8g2_font_timR08_tr);            
  u8g2.drawStr(115,62,"d4i");                
  u8g2.drawHLine(0, 52, 128);                // Dibuja una línea horizontal
  u8g2.sendBuffer();                         // transfer internal memory to the display
}



void startOled(){
   u8g2.begin();                                // Inicia el display  
   u8g2.enableUTF8Print();
   u8g2.clear();                           
   u8g2.drawXBMP(36, 10, d4i_Logo_width, d4i_Logo_height, d4i_Logo_bits);
   u8g2.setFont( u8g2_font_helvB08_tf);          
   u8g2.drawStr(14,62,"www.d4insghts.com");      
   u8g2.sendBuffer();                           
}

void wifiOled(){
   u8g2.clear();                           
   u8g2.drawXBMP(32, 3, WiFi_Logo_width, WiFi_Logo_height, WiFi_Logo_bits);  
   
   u8g2.setDrawColor(0);
   u8g2.drawBox(40,45,45,6);
   u8g2.setDrawColor(1);
   u8g2.drawDisc(42, 47, 3, U8G2_DRAW_ALL);
   u8g2.drawCircle(52, 47, 3, U8G2_DRAW_ALL);
   u8g2.drawCircle(62, 47, 3, U8G2_DRAW_ALL);
   u8g2.drawCircle(72, 47, 3, U8G2_DRAW_ALL);
   u8g2.drawCircle(82, 47, 3, U8G2_DRAW_ALL);
   u8g2.sendBuffer();
   delay(50);
   u8g2.setDrawColor(0);
   u8g2.drawBox(40,45,45,6);
   u8g2.setDrawColor(1);   
   u8g2.drawCircle(42, 47, 3, U8G2_DRAW_ALL);
   u8g2.drawDisc(52, 47, 3, U8G2_DRAW_ALL);
   u8g2.drawCircle(62, 47, 3, U8G2_DRAW_ALL);
   u8g2.drawCircle(72, 47, 3, U8G2_DRAW_ALL);
   u8g2.drawCircle(82, 47, 3, U8G2_DRAW_ALL);
   u8g2.sendBuffer();
   delay(200);
   u8g2.setDrawColor(0);
   u8g2.drawBox(40,45,45,6);
   u8g2.setDrawColor(1);
   u8g2.drawCircle(42, 47, 3, U8G2_DRAW_ALL);
   u8g2.drawCircle(52, 47, 3, U8G2_DRAW_ALL);
   u8g2.drawDisc(62, 47, 3, U8G2_DRAW_ALL);
   u8g2.drawCircle(72, 47, 3, U8G2_DRAW_ALL);
   u8g2.drawCircle(82, 47, 3, U8G2_DRAW_ALL);
   u8g2.setFont(u8g2_font_timR08_tr);            
   u8g2.drawStr(20,63,"waiting connection.."); 
   u8g2.sendBuffer();
   delay(100);
   u8g2.setDrawColor(0);
   u8g2.drawBox(40,45,45,6);
   u8g2.setDrawColor(1);
   u8g2.drawCircle(42, 47, 3, U8G2_DRAW_ALL);
   u8g2.drawCircle(52, 47, 3, U8G2_DRAW_ALL);
   u8g2.drawCircle(62, 47, 3, U8G2_DRAW_ALL);
   u8g2.drawDisc(72, 47, 3, U8G2_DRAW_ALL);
   u8g2.drawCircle(82, 47, 3, U8G2_DRAW_ALL);
   u8g2.sendBuffer();
//   delay(200);
//   u8g2.setDrawColor(0);
//   u8g2.drawBox(40,45,45,6);
//   u8g2.setDrawColor(1);
//   u8g2.drawCircle(42, 47, 3, U8G2_DRAW_ALL);
//   u8g2.drawCircle(52, 47, 3, U8G2_DRAW_ALL);
//   u8g2.drawCircle(62, 47, 3, U8G2_DRAW_ALL);
//   u8g2.drawCircle(72, 47, 3, U8G2_DRAW_ALL);
//   u8g2.drawDisc(82, 47, 3, U8G2_DRAW_ALL);
   u8g2.sendBuffer();   
}
