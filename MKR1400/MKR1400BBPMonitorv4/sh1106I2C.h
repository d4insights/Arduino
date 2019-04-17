/*
 * Librería tiny de U8g2 para manejar la pantalla SH1106 con I2C
 * OLED Resolución 128X64 - Monocromo
 * 
 *  Manejo del display emulando a una pantalla de celular
 * 
 * 
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
  //u8g2.sendBuffer();              // Si lo pones flippea la pantalla, es mejor así porque si borro es para dibujar y ahí mando el Send 
}

void displayBorraBodyPantalla(){
  u8g2.setDrawColor(0);
  u8g2.drawBox(0,11,128,53);
  u8g2.setDrawColor(1);          
  //u8g2.sendBuffer();              // Si lo pones flippea la pantalla, es mejor así porque si borro es para dibujar y ahí mando el Send
}

void displayBorraHeaderPantalla(){
  u8g2.setDrawColor(0);
  u8g2.drawBox(0,0,128,11);
  u8g2.setDrawColor(1);          
  u8g2.sendBuffer(); 
}


void displayError(String texto){
  
  displayBorraBodyPantalla();
  
  char buff[128];
  texto.toCharArray(buff,128);
  
  u8g2.setFont( u8g2_font_helvB08_tf);          
  u8g2.drawStr(9,40,buff);      
  u8g2.setFont(u8g2_font_timR08_tr);            
  u8g2.drawStr(114,62,"d4");
  u8g2.setFont(u8g2_font_5x7_tf);
  u8g2.drawStr(124,63,"i"); 
  u8g2.sendBuffer();    
}

void displayWaitingStartUP(){
  
  displayBorraBodyPantalla();

  u8g2.setFont( u8g2_font_open_iconic_embedded_4x_t);
  u8g2.drawStr(9,53,"N");
  
  u8g2.setFont( u8g2_font_helvB08_tf);          
  u8g2.drawStr(50,30,"Iniciando los");
  u8g2.drawStr(50,50,"servicios ...");
     
  u8g2.setFont(u8g2_font_timR08_tr);            
  u8g2.drawStr(114,62,"d4");
  u8g2.setFont(u8g2_font_5x7_tf);
  u8g2.drawStr(124,63,"i"); 
  u8g2.sendBuffer();   
  
}

void displayWarningCeluGuardia(){
  
  displayBorraBodyPantalla();
  
  u8g2.setFont( u8g2_font_open_iconic_embedded_4x_t);
  u8g2.drawStr(4,53,"G"); 
  
  u8g2.setFont( u8g2_font_helvB08_tf);          
  u8g2.drawStr(43,30,"Registre el");
  u8g2.drawStr(43,50,"Celular Guardia");
        
  u8g2.setFont(u8g2_font_timR08_tr);            
  u8g2.drawStr(114,62,"d4");
  u8g2.setFont(u8g2_font_5x7_tf);
  u8g2.drawStr(124,63,"i"); 
  u8g2.sendBuffer();    
}

void displayHeader(String texto, bool alerta, String sms, bool sincro, int senal, float bateria){  

  if(iconAlerta)
     texto = texto.substring(0, 11); 
  else if(iconSMS !="")
     texto = texto.substring(0, 14);
  else
     texto = texto.substring(0, 15);
     
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
  u8g2.drawStr(72,8,"A");                             // campanita
  }


  // ICONO de Mail
  if(sms=="OUT"){
  u8g2.setFont(u8g2_font_open_iconic_email_1x_t);
  u8g2.drawStr(90,8,"@");                             // Mail saliente
  u8g2.setFont(u8g2_font_open_iconic_arrow_1x_t);
  u8g2.drawStr(83,8,"C");
  }
  if(sms=="IN"){
  u8g2.setFont(u8g2_font_open_iconic_email_1x_t);
  u8g2.drawStr(90,8,"A");                             // Mail Recibido
  u8g2.setFont(u8g2_font_open_iconic_arrow_1x_t);
  u8g2.drawStr(83,8,"@");
  }

  // ICONO de Sincronización con Internet 
  if(sincro){
    u8g2.setFont(u8g2_font_open_iconic_arrow_1x_t);
    u8g2.drawStr(100,8,"W");                            // Sincronizando Internet OK       
  } else {
//    u8g2.setFont(u8g2_font_open_iconic_arrow_1x_t);
//    u8g2.drawStr(100,8,"W");                            // NO Sincranizando
//    u8g2.drawLine(100, 0, 108, 8); 
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

  if(bateria > 2.7 && bateria <= 3.5){
    u8g2.setFont(u8g2_font_open_iconic_embedded_1x_t);
    u8g2.drawStr(120,8,"@");                            // batería 25%
    u8g2.drawBox(120,2,2,4);
  }

  if(bateria > 3.5 && bateria <= 3.8){
    u8g2.setFont(u8g2_font_open_iconic_embedded_1x_t);
    u8g2.drawStr(120,8,"@");                            // batería 50%
    u8g2.drawBox(120,2,3,4);
  }

  if(bateria > 3.8 && bateria <= 4.08){
    u8g2.setFont(u8g2_font_open_iconic_embedded_1x_t);
    u8g2.drawStr(120,8,"@");                            // batería 75%
    u8g2.drawBox(120,2,5,4);
  }
    
  if (bateria > 4.08){
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
   u8g2.setContrast(100);                        // Setea el contraste de la pantalla 0 a 255                                 
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
   u8g2.setContrast(100);                        // Setea el contraste de la pantalla 0 a 255                 
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



void displaySafetyBateryLevel(float iconBateria){
  
  char volts[10];
  String aux= String(iconBateria);
  aux.toCharArray(volts,10);
  
  displayBorraBodyPantalla();

  u8g2.setFontMode(1);   
  u8g2.setFont(u8g2_font_6x10_tf);   
  u8g2.drawStr(5,40,"BATERIA");
  u8g2.drawStr(5,50,"RESPALDO");
  u8g2.drawStr(120,50,"v");
  u8g2.drawLine(55, 25, 55, 55); 
  u8g2.setFont(u8g2_font_fub17_tn);   
  u8g2.drawStr(70,50,volts);
  u8g2.sendBuffer();         

}


// Muestra en el display los resultados de la medición del INPUT del Inversor
void displayInput(){

  char volts[10];
  String aux= String(VIN,0);
  aux.toCharArray(volts,10);

  char ampers[10];
  String aux2= String(IIN,2);
  aux2.toCharArray(ampers,10);

  char watts[10];
  String aux3= String(PIN);
  aux3.toCharArray(watts,10);
  
  
  displayBorraBodyPantalla();

  u8g2.setFontMode(1);   
  u8g2.setFontDirection(3);
  u8g2.setFont(u8g2_font_6x10_tf);   
  u8g2.drawStr(14,60,"ENTRADA");
  u8g2.setFontDirection(0);
  u8g2.drawStr(90,29,"Volts");
  u8g2.drawStr(90,46,"Ampers");
  u8g2.drawStr(90,63,"Watts");
  
  u8g2.drawLine(20, 25, 20, 55); 
  u8g2.setFont(u8g2_font_fub14_tn);   
  if(VIN>0)
    u8g2.drawStr(32,29,volts);
  else
    u8g2.drawStr(32,29,"0");
  if(IIN>0)
    u8g2.drawStr(32,46,ampers);
  else
    u8g2.drawStr(32,46,"0");
  u8g2.drawStr(32,63,watts);
    
  u8g2.sendBuffer();         
}

// Muestra en el display los resultados de la medición del OUTPUT del Inversor
void displayOutput(){

  char volts[10];
  String aux= String(VOUT,0);
  aux.toCharArray(volts,10);

  char ampers[10];
  String aux2= String(IOUT,2);
  aux2.toCharArray(ampers,10);

  char watts[10];
  String aux3= String(POUT);
  aux3.toCharArray(watts,10);
  
  displayBorraBodyPantalla();

  u8g2.setFontMode(1);   
  u8g2.setFontDirection(3);
  u8g2.setFont(u8g2_font_6x10_tf);   
  u8g2.drawStr(14,56,"SALIDA");
  u8g2.setFontDirection(0);
  u8g2.drawStr(90,29,"Volts");
  u8g2.drawStr(90,46,"Ampers");
  u8g2.drawStr(90,63,"Watts");
  
  u8g2.drawLine(20, 25, 20, 55); 
  u8g2.setFont(u8g2_font_fub14_tn);   
  if(VOUT>0)
    u8g2.drawStr(32,29,volts);
  else
    u8g2.drawStr(32,29,"0");
  if(IOUT>0)
    u8g2.drawStr(32,46,ampers);
  else
    u8g2.drawStr(32,46,"0");
  u8g2.drawStr(32,63,watts);
    
  u8g2.sendBuffer();         
}


// Muestra en el display los resultados de la medición de BATERIAS del Inversor
void displayBatteriesLevel(){

  char bat01[10];                              // Bateria 01 de soporte del INVERSOR
  String aux01= String(bateria01,2);
  aux01.toCharArray(bat01,10);

  char bat02[10];                              // Bateria 02 de soporte del INVERSOR
  String aux02= String(bateria02,2);
  aux02.toCharArray(bat02,10);

  char bat03[10];                              // Bateria 03 de soporte del INVERSOR
  String aux03= String(bateria03,2);
  aux03.toCharArray(bat03,10);

  char bat04[10];                              // Bateria 04 de soporte del INVERSOR 
  String aux04= String(bateria04,2);
  aux04.toCharArray(bat04,10);

  
  displayBorraBodyPantalla();

  u8g2.setFontMode(1);   
  u8g2.setFontDirection(3);
  u8g2.setFont(u8g2_font_6x10_tf);   
  u8g2.drawStr(14,63,"BATERIAS");
  u8g2.setFontDirection(0);

  u8g2.drawDisc(68,18,5,U8G2_DRAW_ALL);        // Circulitos para el nro de batería
  u8g2.drawDisc(122,18,5,U8G2_DRAW_ALL);       // Circulitos para el nro de batería
  u8g2.drawDisc(68,48,5,U8G2_DRAW_ALL);        // Circulitos para el nro de batería 
  u8g2.drawDisc(122,48,5,U8G2_DRAW_ALL);       // Circulitos para el nro de batería
  u8g2.setFontMode(0);
  u8g2.setDrawColor(0);
  u8g2.drawStr(66,22,"1");                     // Nro de batería dentro del circulito
  u8g2.drawStr(120,22,"2");
  u8g2.drawStr(66,52,"3");
  u8g2.drawStr(120,52,"4");
  u8g2.setFontMode(1);
  u8g2.setDrawColor(1);
  
  //u8g2.setFont(u8g2_font_5x7_mf);
  u8g2.drawStr(66,37,"v");
  u8g2.drawStr(120,37,"v");
  u8g2.drawStr(66,64,"v");
  u8g2.drawStr(120,64,"v");

  
  u8g2.drawLine(20, 25, 20, 55);              // tabique de adelante
  u8g2.drawLine(76, 25, 76, 55);              // tabique del medio
  u8g2.drawLine(23, 40, 125, 40);             // línea de división a mitad de la pantalla
  
  u8g2.setFont(u8g2_font_fub11_tn);   
  u8g2.drawStr(25,37,bat01);                  // Reemplazar por valores reales de medición de voltaje
  u8g2.drawStr(82,37,bat02);
  u8g2.drawStr(25,64,bat03);
  u8g2.drawStr(82,64,bat04);
    
  u8g2.sendBuffer();         
}



// Muestra el cuerpo de los mensajes SMS entrantes 5 veces
void displaySmsMsg(){

  char nTelefono[20];
  lastSMSSenderN.toCharArray(nTelefono,20);
  
  displayBorraBodyPantalla();
  
  u8g2.setFontMode(1);   
  u8g2.setFont(u8g2_font_5x7_mf);           // Mejor uso esta porque tienen espaciado proporcional
  u8g2.drawStr(0,20,"FROM:");
  u8g2.drawStr(35,20,nTelefono);
  u8g2.drawStr(0,30,"MSG:");

  char line1[30];
  lastSMSRecibed.substring(0,26).toCharArray(line1,30);
  u8g2.drawStr(0,40,line1);

  char line2[25];
  lastSMSRecibed.substring(26,52).toCharArray(line2,30);
  u8g2.drawStr(0,50,line2);

  char line3[25];
  lastSMSRecibed.substring(52,78).toCharArray(line3,30);
  u8g2.drawStr(0,60,line3);
  
  u8g2.sendBuffer();         
  delay(4000);

  if(iconSMS == "IN" && SMSRetencion++ >= 5){
      iconSMS = "";
  }
}
