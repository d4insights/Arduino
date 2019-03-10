#include "EmonLib.h"

EnergyMonitor energyMonitor;

const byte gClk = 3; //used to define which generic clock we will use for ADC
const byte intPri = 0; //used to set interrupt priority for ADC
const int cDiv = 1; //divide factor for generic clock
const float period = 3.3334; //period of 300k sample rate

unsigned int lst = 0;

//Pin digital de entrada del detector de ceros
#define DetectorDeCeros 5
//Pin de entrada analogico lectura de tension
#define SenalAC         A1

//Relacion de vueltas del transformador
#define RelacionDeTransformacion 21.536
//Ganancia del divisor resistivo (2K2 y 1K)
#define GananciaDivisor   6.733
//Tension de alterna con la que se calibra el sensor

//Tiempo de espera entre mediciones (segundos)
#define TiempoEntreMediciones 10

//Tension con la que alimenta el micro
#define Tension 3.3

//Guardo la ultima vez que realize una medicion
unsigned long lastTime = 0;

unsigned int   V = 220;//Tension   Alterna


void setup() {
  delay(100);
  Serial.begin(115200);
  pinMode(DetectorDeCeros,  INPUT);
  portSetup(); //setup the ports or pin to make ADC measurement
  genericClockSetup(gClk,cDiv); //setup generic clock and routed it to ADC
  aDCSetup(); //this function set up registers for ADC, input argument sets ADC reference
}

void loop() {
  Medicion();
}


//function for configuring ports or pins, note that this will not use the same pin numbering scheme as Arduino
void portSetup() {
  // Input pin for ADC Arduino A0/PA02
  REG_PORT_DIRCLR1 = PORT_PA02;

  // Enable multiplexing on PA02_AIN0 PA03/ADC_VREFA
  PORT->Group[0].PINCFG[2].bit.PMUXEN = 1;
  PORT->Group[0].PINCFG[3].bit.PMUXEN = 1;
  PORT->Group[0].PMUX[1].reg = PORT_PMUX_PMUXE_B | PORT_PMUX_PMUXO_B;
}

//this function sets up the generic clock that will be used for the ADC unit
//by default it uses the 48M system clock, input arguments set divide factor for generic clock and choose which generic clock
//Note unless you understand how the clock system works use clock 3. clocks 5 and up can brick the microcontroller based on how Arduino configures things
void genericClockSetup(int clk, int dFactor) {
  // Enable the APBC clock for the ADC
  REG_PM_APBCMASK |= PM_APBCMASK_ADC;
  
  //This allows you to setup a div factor for the selected clock certain clocks allow certain division factors: Generic clock generators 3 - 8 8 division factor bits - DIV[7:0]
  GCLK->GENDIV.reg |= GCLK_GENDIV_ID(clk)| GCLK_GENDIV_DIV(dFactor);
  while (GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY);  

  //configure the generator of the generic clock with 48MHz clock
  GCLK->GENCTRL.reg |= GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_DFLL48M | GCLK_GENCTRL_ID(clk); // GCLK_GENCTRL_DIVSEL don't need this, it makes divide based on power of two
  while (GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY);
  
  //enable clock, set gen clock number, and ID to where the clock goes (30 is ADC)
  GCLK->CLKCTRL.reg |= GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN(clk) | GCLK_CLKCTRL_ID(30);
  while (GCLK->STATUS.bit.SYNCBUSY);
}

/*
ADC_CTRLB_PRESCALER_DIV4_Val    0x0u  
ADC_CTRLB_PRESCALER_DIV8_Val    0x1u   
ADC_CTRLB_PRESCALER_DIV16_Val   0x2u   
ADC_CTRLB_PRESCALER_DIV32_Val   0x3u   
ADC_CTRLB_PRESCALER_DIV64_Val   0x4u   
ADC_CTRLB_PRESCALER_DIV128_Val  0x5u   
ADC_CTRLB_PRESCALER_DIV256_Val  0x6u   
ADC_CTRLB_PRESCALER_DIV512_Val  0x7u   
--> 8 bit ADC measurement takes 5 clock cycles, 10 bit ADC measurement takes 6 clock cycles
--> Using 48MHz system clock with division factor of 1
--> Using ADC division factor of 32
--> Sample rate = 48M / (5 x 32) = 300 KSPS
This function sets up the ADC, including setting resolution and ADC sample rate
*/
void aDCSetup() {
  // Select reference
  REG_ADC_REFCTRL = ADC_REFCTRL_REFSEL_INTVCC1; //set vref for ADC to VCC

  // Average control 1 sample, no right-shift
  REG_ADC_AVGCTRL |= ADC_AVGCTRL_SAMPLENUM_1;

  // Sampling time, no extra sampling half clock-cycles
  REG_ADC_SAMPCTRL = ADC_SAMPCTRL_SAMPLEN(0);
  
  // Input control and input scan
  REG_ADC_INPUTCTRL |= ADC_INPUTCTRL_GAIN_1X | ADC_INPUTCTRL_MUXNEG_GND | ADC_INPUTCTRL_MUXPOS_PIN0;
  // Wait for synchronization
  while (REG_ADC_STATUS & ADC_STATUS_SYNCBUSY);

  ADC->CTRLB.reg |= ADC_CTRLB_RESSEL_10BIT | ADC_CTRLB_PRESCALER_DIV4 | ADC_CTRLB_FREERUN; //This is where you set the divide factor, note that the divide call has no effect until you change Arduino wire.c
  //Wait for synchronization
  while (REG_ADC_STATUS & ADC_STATUS_SYNCBUSY);

  ADC->WINCTRL.reg = ADC_WINCTRL_WINMODE_DISABLE; // Disable window monitor mode
  while(ADC->STATUS.bit.SYNCBUSY);

  ADC->EVCTRL.reg |= ADC_EVCTRL_STARTEI; //start ADC when event occurs
  while (ADC->STATUS.bit.SYNCBUSY);

  ADC->CTRLA.reg |= ADC_CTRLA_ENABLE; //set ADC to run in standby
  while (ADC->STATUS.bit.SYNCBUSY);
}

//Mido cada cierto tiempo la corriente y la tension de alterna en pines especificos..
//
int Medicion(){
  //Reinicio las variables de acumulacion
  float ac = 0;
  int    N = 0;
  
  //Si se cumplio el tiempo entre mediciones..
  if((millis() - lastTime) >= TiempoEntreMediciones * 1000){
    //Espero el primer detector de ceros
    unsigned long InicioDeMedicion = millis(); //Guardo el momento en el que comienzo a buscar el cero
    
    while(digitalRead(DetectorDeCeros) == 0){
      //Si tardo mas de 10mS, tengo un corte en el servicio de tension o la red funciona mal
      if((millis() - InicioDeMedicion ) > 10){
        Serial.println("Corte de Luz Inminente(1)!!!");
        lastTime = millis();
        V = 0;
        return 0;
      }
    }
    //Espero que el pulso del detector se apague, para comenzar con la lectura..
    InicioDeMedicion = millis(); //Guardo el momento en el que comienzo a buscar el cero
    while(digitalRead(DetectorDeCeros) == 1){
      if((millis() - InicioDeMedicion ) > 4){
        Serial.println("Corte de Luz Inminente(4)!!!");
        lastTime = millis();
        V = 0;
        return 0;
      }
    }
    //Comienzo a muestrear en busca del valor maximo... hasta el proximo cero
    InicioDeMedicion = micros(); //Guardo el momento en el que comienzo a buscar el cero
    while(digitalRead(DetectorDeCeros ) == 0){
      if((micros() - InicioDeMedicion ) > 10000){
        Serial.println("Corte de Luz Inminente(2)!!!");
        lastTime = millis();
        V = 0;
        return 0;
      }
      int valor = analogRead(SenalAC);                //Leo la tension en la pata 0
      float tension = (valor * Tension / 1023) * RelacionDeTransformacion * GananciaDivisor;//Traduzco dicha lectura a tension   -- Mapeo los valores analogicos a una tension 0V-5V, Mutiplico por la relacion de transformacion y luego invierto la perdida en el divisor resistivo
      ac += tension;                                   //Acumulo la tension
      N++;                                             //Guardo el numero de lecturas para poder promediar
    }
    if(N == 0){
      Serial.println("Corte de Luz Inminente(3)!!!");
      lastTime = millis();
      return 0;
    }
    else{
      V = round((ac / N));
      Serial.print("Tension:");
      Serial.print(V);
      Serial.print("VAC");
      Serial.print(" | NS: ");
      Serial.print(N);
      Serial.print("| Muestrie por:");
      Serial.print(micros()-InicioDeMedicion);
      Serial.println("uS");
    }
   
    Serial.println();

    lastTime = millis();
  }
}
