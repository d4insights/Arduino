#include "EmonLib.h"
EnergyMonitor energyMonitor;

const int muxSIG = A0;
const int muxS0 = D0;
const int muxS1 = D1;
const int muxS2 = D2;
const int muxS3 = D3;
int lec[17] = {0};


// Seteo del Canal para identificar que puerta linkea
//
int SetMuxChannel(byte channel)
{
   digitalWrite(muxS0, bitRead(channel, 0));
   digitalWrite(muxS1, bitRead(channel, 1));
   digitalWrite(muxS2, bitRead(channel, 2));
   digitalWrite(muxS3, bitRead(channel, 3));
   delay(100);
      
  //    Serial.print(bitRead(channel, 3));
  //    Serial.print("\t");
  //    Serial.print(bitRead(channel, 2));
  //    Serial.print("\t");
  //    Serial.print(bitRead(channel, 1));
  //    Serial.print("\t");
  //    Serial.print(bitRead(channel, 0));
  //    Serial.print("\n"); 
}
 


void setup()
{
   Serial.begin(9600);
   pinMode(muxS0, OUTPUT);
   pinMode(muxS1, OUTPUT);
   pinMode(muxS2, OUTPUT);
   pinMode(muxS3, OUTPUT);
   
}
 
void loop()
{

  SetMuxChannel(0);
  int AcIN  = tensionAC();
  SetMuxChannel(1);
  int AcOUT = tensionAC();
  SetMuxChannel(2);
  float Iin   = corrienteAC(2.80);
  int   Pin   = potenciaAC(1, AcIN, Iin);
   
   Serial.println();
   delay(1000);
}


// Mide la tensión de alterna (Volts)
//
int tensionAC(){
  int VmaxD = 0, sensor = 0;
  for(int c = 0;c < 800; c ++ ){
    sensor=analogRead(A0);
    if(sensor>VmaxD && sensor > 511)
      VmaxD = sensor;
  }
  int Veffd = VmaxD / sqrt(2);
  int Veff = (((Veffd-420.76)/-90.24)*-210.2)+210.2;
  Serial.print("Tension =");
  Serial.println(Veff);
  return Veff;
}


// Mide la corriente de alterna (Amperes)
//
float corrienteAC(float cal){
  float Irms = energyMonitor.calcIrms(1484);
  energyMonitor.current(0, cal);
  if(Irms < 0 || Irms < 1)
    Irms = 0;  
  Serial.print("Corriente AC: ");
  Serial.println(Irms);
  return Irms; 
}


// Cálculo de Potencia en alterna (Watts)
//
double potenciaAC(int trigger, int I, int V){
  double pot = I * V;
  if(pot < trigger)
    pot = 0;
  Serial.print("Potencia AC: ");
  Serial.println(pot);
  return pot;
}
  

