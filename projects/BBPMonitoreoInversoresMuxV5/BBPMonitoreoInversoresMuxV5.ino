// Programa: Monitoreo de Inversores PH1800
//           Mediante sensores medimos la Entrada, Salida y la granja de baterias de contingencia
//
// Fecha: 18/08/2018
// 
// A1  - Voltaje entrada de la red
// A2  - Voltaje salida del inversor

// MUX CHANNELS
// 1  - Amperaje entrada de la red
// 2  - Amperaje salida del inversor
// 3  - Voltaje Batería 1
// 4  - Voltaje Batería 2
// 5  - Voltaje Batería 3
// 6  - Voltaje Batería 4
// 7  - Voltaje Batería 5
// 8  - Voltaje Batería 6
// 9 - Voltaje Batería 7
// 10 - Voltaje Batería 8
// 11 - Voltaje Batería 9
// 12 - Voltaje Batería 10
// 13 - Voltaje Batería 11
// 14 - Voltaje Batería 12
// 15 - Voltaje Batería 13
// 16 - Voltaje Batería 14


#include "EmonLib.h"              // Librería para medir energía electrica
EnergyMonitor energyMonitor;


#include "Wire.h"                 // Para manejar las conexiones i2C


int largoMuestra = 20;                           //Cantidad de muestras que toma de TensiónAC para promediar 
int muestraTensionAC = 0;                        //Acumulador de vueltas de muestreo que hizo

int maxi = 0, mini = 2000, maxi1 = 0, mini1 = 0; //Valores Maximos y minimos del muestreo 
int v1,v2,v1ant = 220,v2ant = 220, en = 0;       //Memorias de muestreo
char enm = 0, enm1 = 0;                          //Filtros de calidad del muestreo

byte ordenI2c = 0;                               // Código de la órden en i2C que recibe el esclavo

const int muxSIG = A0;           // Pata analógica del MUX
const int muxS0 = 2;             // Pata de control del MUX
const int muxS1 = 3;             // Pata de control del MUX
const int muxS2 = 4;             // Pata de control del MUX
const int muxS3 = 5;             // Pata de control del MUX


// Variables Auxiliare de Monitoreo de los Inversores
int   VIN = 220;              // Voltaje de entrada       (Volts)
float IIN = 0;                // Corriente de entrada     (Amperes)
int   PIN = 0;                // Potencia de entrada      (Watts)
int   VOUT = 220;             // Voltaje de salida        (Volts)
float IOUT = 0;               // Corriente de salida      (Amperes)
int   POUT = 0;               // Potencia de salida       (Watts)
float VB[13] = {0};           // Voltaje de cada Batería  (Volts)   (empieza en posición 1)       
int   CantBaterias = 1;       // Cantidad de Baterías 


void setup() {
  Serial.println();
  Serial.println("Inicializando MICRO.... ");
  
  pinMode(muxS0, OUTPUT);       // Seteo los pines de control del MUX como salida
  pinMode(muxS1, OUTPUT);       // Seteo los pines de control del MUX como salida
  pinMode(muxS2, OUTPUT);       // Seteo los pines de control del MUX como salida
  pinMode(muxS3, OUTPUT);       // Seteo los pines de control del MUX como salida

  Serial.begin(115200);         // Activo el monitor serial

  Wire.begin(1);                // Inicia la transmisión del I2C (Esclavo1)
  //Wire.onReceive(receiveEvent); // Por esta escucha ordenes
  Wire.onRequest(requestEvent); // Por esta recibe peticiones que sirven para que el esclavo contesta al master
  
  Serial.println("Wire activo...");
  
}


void loop() {
  
  // comunicacion(recepcion);
  
  tensionAC();                    // Muestrea la tensión de Entrada y Salida (VIN y VOUT)
  
  SetMuxChannel(0);               // MUX CH2 - Sensor Amperaje de la entrada de red
  IIN = corrienteAC(2.80);
  PIN = potenciaAC(1, VIN, IIN);
  
  //tensionAC();
  SetMuxChannel(1);               // MUX CH3 - Sensor Amperaje de la salidsa del Inversor
  IOUT = corrienteAC(2.80);
  POUT = potenciaAC(1, VIN, IOUT);
  
  //tensionAC();
  //tensionBAT(VB, CantBaterias);   // Leo el estado de las baterías conectadas
  
  //imprimesalidamonitorserial();   // Mando el resúmen al Monitor Serial

}


// Orden que envía el Master (no sirve para responder, solo para acciones)
//
void receiveEvent(int howMany){

  if(Wire.available() == 1){
     ordenI2c = Wire.read();  
     Serial.print("Order- ");
     Serial.println(ordenI2c);
  }
  
}


// Pedido de respuesta desde el Master (sirve para devolver valores al Master)
//
void requestEvent(){
     Wire.write((byte)VIN);  
     Wire.write((byte)VOUT);
     
     Wire.write((byte)IIN);
     int centenas = IIN / 100;
     int decenas  = (IIN - centenas*100) / 10;
     int unidades = (IIN - centenas*100 - decenas*10) / 1;
     int decimales = (IIN - centenas*100 - decenas*10 - unidades) * 100;
     Wire.write((byte)decimales);  
     
     //Wire.write(IOUT);

     Serial.print(VIN);
     Serial.print("\t");

     Serial.print(VOUT);
     Serial.print("\t");

     Serial.print((int)IIN);
     Serial.print("\t");
     
     Serial.print(decimales);
     Serial.print("\n");
          
}



// Seteo del Canal para identificar que puerta linkea
//
int SetMuxChannel(byte channel)
{
   digitalWrite(muxS0, bitRead(channel, 0));
   digitalWrite(muxS1, bitRead(channel, 1));
   digitalWrite(muxS2, bitRead(channel, 2));
   digitalWrite(muxS3, bitRead(channel, 3));
}



// Mide la tensión de alterna (Volts)
//
int tensionAC(){
  
  int lecEntrada = analogRead(A1);
  int lecSalida = analogRead(A2);
  
  if(lecSalida > maxi1)
    maxi1 = lecSalida;
  
  if(lecSalida < mini1)
    mini1 = lecSalida;
  
  if(lecEntrada > maxi)
    maxi = lecEntrada;
  
  if(lecEntrada < mini)
    mini = lecEntrada;

  

  if(muestraTensionAC >= largoMuestra){
    VIN = maxi-mini;
    if(VIN > 250)
      VIN = v1ant;
    
    VOUT = maxi1-mini1;
    if(VOUT > 250)
      VOUT = v2ant;
    
    mini = 2000;
    maxi = 0;
    
    mini1 = 2000;
    maxi1 = 0;
    
    v1ant = v1;
    v2ant = v2;
    
    muestraTensionAC = 0;
  }
  else{
        
    muestraTensionAC ++;  
  }


  
}


// Mide la corriente de alterna (Amperes)
//
float corrienteAC(float cal){
  float Irms = energyMonitor.calcIrms(1484);
  energyMonitor.current(0, cal);
  if(Irms < 0 || Irms < 1)
    Irms = 0;  
  return Irms; 
}


// Cálculo de Potencia en alterna (Watts)
//
double potenciaAC(int trigger, int I, int V){
  double pot = I * V;
  if(pot < trigger)
    pot = 0;
  return pot;
}



//Calculo la tension de las baterias
//
void tensionBAT(float baterias[], int cantidad){
  
  int ch = 3;
  for(int i = 0; i < 12; i ++)
    baterias[i] = 0;
  for(int i = 0; i < cantidad; i ++){
    delay(10);
    ch ++;
    SetMuxChannel(ch);
    baterias[i+1] = analogRead(A0) * 17.75 / 1024;
  }
}



// Imprime las salidas de lecturas por el monitor serial
//
void imprimesalidamonitorserial(){

  Serial.println(" ");
  Serial.print(VIN);
  Serial.print("V");
  Serial.print("\t");
  Serial.print(IIN);
  Serial.print("A");
  Serial.print("\t");
  Serial.print(PIN);
  Serial.print("W");
  Serial.print("\t");
  Serial.print("\t");
  Serial.print(VOUT);
  Serial.print("V");
  Serial.print("\t");
  Serial.print(IOUT);
  Serial.print("A");
  Serial.print("\t");
  Serial.print(POUT);
  Serial.print("W");
  Serial.print("\t");

  Serial.print(VB[1]);
  
  Serial.println("V");

//  Serial.print(act);
}  

/*
//Envio
//
void envio(int codigo){
  //Serial.println(codigo);
  Sensor.print(codigo);
  Sensor.println("\n");
  delay(30);
}

//Recepcion
//
int recibo(){
  while(Sensor.available()>0){
    int val = Sensor.parseInt();
    if(Sensor.read()== '\n'){
       return val;
       break;
    }
  }
}

*/
/*//Manejo la comunicacion
void comunicacion(int dato){
  switch(dato){
    case 0:
      Sensor.print(VIN);
      Sensor.println("\n");
      break;
    case 1:
      Sensor.print(IIN);
      Sensor.println("\n");
      break;
    case 2:
      Sensor.print(PIN);
      Sensor.println("\n");
      break;
    case 3:
      Sensor.print(VOUT);
      Sensor.println("\n");
      break;
    case 4:
      Sensor.print(IOUT);
      Sensor.println("\n");
      break;
    case 5:
      Sensor.print(POUT);
      Sensor.println("\n");
      break;
    case 6:
      Sensor.print(VB[1]);
      Sensor.println("\n");
      break;
    case 7:
      Sensor.print(VB[2]);
      Sensor.println("\n");
      break;
    case 8:
      Sensor.print(VB[3]);
      Sensor.println("\n");
      break;
    case 9:
      Sensor.print(VB[4]);
      Sensor.println("\n");
      break;
    case 10:
      Sensor.print(VB[5]);
      Sensor.println("\n");
      break;
    case 11:
      Sensor.print(VB[6]);
      Sensor.println("\n");
      break;
    case 12:
      Sensor.print(VB[7]);
      Sensor.println("\n");
      break;
    case 13:
      Sensor.print(VB[8]);
      Sensor.println("\n");
      break;
    case 14:
      Sensor.print(VB[9]);
      Sensor.println("\n");
      break;
    case 15:
      Sensor.print(VB[10]);
      Sensor.println("\n");
      break;

  }
    
  
}
*/
