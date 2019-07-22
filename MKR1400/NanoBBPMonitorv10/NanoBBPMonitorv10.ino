/*
              Modulo Alterna
  Proyecto: Monitoreo De Inversores
  Fecha   :       20/07/2019
  Version :           1

*/
//--------------------------------------------------------------------------------------------------------------
//Bibliotecas
//
#include <Wire.h>
//--------------------------------------------------------------------------------------------------------------
//Pines de lecturas
//
#define inPinV   0
#define inPinI1  2
#define inPinV2  1
#define inPinI2  3
//--------------------------------------------------------------------------------------------------------------
//Pin del led de Status
//
#define LEDpin   13
//--------------------------------------------------------------------------------------------------------------
//Tiempo entre lecturas
//
#define BANDA 5000
//--------------------------------------------------------------------------------------------------------------
//String de la trama- Comunicacion con el MKR1400 GSM
//
String dataFileName = ""; //Inicio la String de la trama
//--------------------------------------------------------------------------------------------------------------
//Variables de lectura
//
float VI, VO, II, IO, POUT, PIN;
int analog_inputs_pins[] = {0, 1, 2, 3}; // Analog pins to sample and pin sampling order
int numberof_analog_inputs = sizeof(analog_inputs_pins) / 2;
int conversion_in_progress = 0;
int sample_in_register;
int next_conversion = 0;
unsigned long timer;
double frequency;
signed int lastSampleV, sampleV;
signed long shifted_filterV;
float sumV, total_sumV;
signed int lastSampleI1, sampleI1;
signed long shifted_filterI1;
float sumI1, sumP1, total_sumI1, total_sumP1;
signed int lastSampleV2, sampleV2;
signed long shifted_filterV2;
float sumV2, total_sumV2;
signed int lastSampleI2, sampleI2;
signed long shifted_filterI2;
float sumI2, sumP2, total_sumI2, total_sumP2;
unsigned long numberOfSamples = 0, total_numberOfSamples;
unsigned int numberOfCycles = 0;
boolean last_cyclestate, cyclestate = false;
unsigned long last_cycle_time, cycle_period;
unsigned long total_cycle_period = 0;
static signed long filteredV;
static signed long filteredV2;
static signed long filteredI1;
static signed long filteredI2;
bool validacion = 0;

//--------------------------------------------------------------------------------------------------------------
//Valores de CALIBRACION
//
double VICAL = 252.30;
double IICAL = 1.2;          //1.817516;
double VOCAL = 260.60;
double IOCAL = 1.25;         //1.9337643;
double POCAL = 0.9317;
double PICAL = 1;
int supply_voltage = 5000;
double VI_RATIO = VICAL * ((supply_voltage / 1000.0) / 1023.0);
double II_RATIO = IICAL * ((supply_voltage / 1000.0) / 1023.0);
double VO_RATIO = VOCAL * ((supply_voltage / 1000.0) / 1023.0);
double IO_RATIO = IOCAL * ((supply_voltage / 1000.0) / 1023.0);


//=============================================================================================================
//Setup del programa
//
void setup()
{
  Serial.begin(9600);
  Serial.println("Modulo de alterna V4");
  Serial.println("d4Insights || 20/07/2019");
  pinMode(LEDpin, OUTPUT);
  digitalWrite(LEDpin, HIGH); delay(100); digitalWrite(LEDpin, LOW); //Parpadeo Rapido
  // REFS0 sets AVcc with external capacitor on AREF pin
  // CT1PIN sets the analog input pin to start reading from
  ADMUX = _BV(REFS0) | next_conversion;

  ADCSRA = _BV(ADATE) | _BV(ADIE);

  ADCSRA |= _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);

  ADCSRA |= _BV(ADEN) | _BV(ADSC);

  //Inicio la comunicacion con el modulo de comunicaciones
  Wire.begin(8);                        // Inicio en modo slave (dispositivo Nro 8)
  Wire.onRequest(requestEvent);

  digitalWrite(LEDpin, HIGH); delay(5000); digitalWrite(LEDpin, LOW); //long flash LED to indicate power up success

  last_cycle_time = micros();
}
//=============================================================================================================
//Interrupcion de lectura
//
ISR(ADC_vect)
{
  signed long shiftedFCL;
  signed long shiftedFCL2;
  sample_in_register = conversion_in_progress;
  conversion_in_progress = analog_inputs_pins[next_conversion];

  if (sample_in_register == inPinV)
  {
    // VOLTAGE
    lastSampleV = sampleV;                               // Used for digital high pass filter
    sampleV = ADC;                                       // Read in raw voltage signal

    // See documentation here for tutorial on digital filters:
    // http://openenergymonitor.org/emon/buildingblocks/digital-filters-for-offset-removal
    shiftedFCL = shifted_filterV + (long)((sampleV - lastSampleV) << 8);
    shifted_filterV = shiftedFCL - (shiftedFCL >> 8);
    filteredV = (shifted_filterV + 128) >> 8;

    sumV += filteredV * filteredV;
  }

  if (sample_in_register == inPinI1)
  {
    // CT1
    lastSampleI1 = sampleI1;
    sampleI1 = ADC;

    shiftedFCL = shifted_filterI1 + (long)((sampleI1 - lastSampleI1) << 8);
    shifted_filterI1 = shiftedFCL - (shiftedFCL >> 8);
    filteredI1 = (shifted_filterI1 + 128) >> 8;

    sumI1 += filteredI1 * filteredI1;
    sumP1 += abs(filteredV * filteredI1);
  }

  if (sample_in_register == inPinV2)
  {
    // VOLTAGE 2
    lastSampleV2 = sampleV2;                               // Used for digital high pass filter
    sampleV2 = ADC;                                       // Read in raw voltage signal

    // See documentation here for tutorial on digital filters:
    // http://openenergymonitor.org/emon/buildingblocks/digital-filters-for-offset-removal
    shiftedFCL2 = shifted_filterV2 + (long)((sampleV2 - lastSampleV2) << 8);
    shifted_filterV2 = shiftedFCL2 - (shiftedFCL2 >> 8);
    filteredV2 = (shifted_filterV2 + 128) >> 8;

    sumV2 += filteredV2 * filteredV2;
  }

  if (sample_in_register == inPinI2)
  {
    // CT2
    lastSampleI2 = sampleI2;
    sampleI2 = ADC;

    shiftedFCL2 = shifted_filterI2 + (long)((sampleI2 - lastSampleI2) << 8);
    shifted_filterI2 = shiftedFCL2 - (shiftedFCL2 >> 8);
    filteredI2 = (shifted_filterI2 + 128) >> 8;

    sumI2 += filteredI2 * filteredI2;
    sumP2 += abs(filteredV2 * filteredI2);
  }

  // Set the adc channel to read from the sample after the current one already underway
  // the value of which will not be in the next ISR call but the one after.

  // cycle through analog inputs to the next input which is enabled

  boolean next_conversion_set = false;
  while (next_conversion_set == false)
  {
    next_conversion++;
    // If we've looped through all analog inputs then go back to the start
    if (next_conversion > (numberof_analog_inputs - 1)) next_conversion = 0;
    ADMUX = _BV(REFS0) | analog_inputs_pins[next_conversion]; next_conversion_set = true;

    // If we're starting at input zero again then a whole set of inputs have been sampled, time to do calcs
    if (next_conversion == 0)
    {
      numberOfSamples++;                                   // Count number of times looped.

      last_cyclestate = cyclestate;
      if (filteredV > 0) cyclestate = true; else cyclestate = false;

      if (last_cyclestate == 0 && cyclestate == 1)
      {
        numberOfCycles ++;

        unsigned long cycle_time = micros();
        cycle_period = cycle_time - last_cycle_time;
        last_cycle_time = cycle_time;

        total_cycle_period += cycle_period;

        total_numberOfSamples += numberOfSamples; numberOfSamples = 0;
        total_sumV += sumV; sumV = 0;

        total_sumI1 += sumI1; sumI1 = 0;
        total_sumP1 += sumP1; sumP1 = 0;

        total_sumI2 += sumI2; sumI2 = 0;
        total_sumP2 += sumP2; sumP2 = 0;

        total_sumV2 += sumV2; sumV2 = 0;
      }
    }
  }
}
//=============================================================================================================
//Setup del programa
//
void loop()
{
  if ((millis() - timer) > BANDA)
  {
    timer = millis();

    //-------------------------------------------------------------------------------------------------------------------------
    // Complete measurement calculations
    //-------------------------------------------------------------------------------------------------------------------------
    // Calculation of the root of the mean of the voltage and current squared (rms)
    // Calibration coeficients applied.

    VI = VI_RATIO * sqrt(total_sumV  / total_numberOfSamples);
    II = II_RATIO * sqrt(total_sumI1 / total_numberOfSamples);
    if (II < 0.02)
      II = 0;
    VO = VO_RATIO * sqrt(total_sumV2 / total_numberOfSamples);
    IO = IO_RATIO * sqrt(total_sumI2 / total_numberOfSamples);
    if (IO < 0.02)
      IO = 0;
    //Calculation power values
    PIN  = VI_RATIO * II_RATIO * total_sumP1 * PICAL / total_numberOfSamples;
    if (PIN < 3 || II == 0)
      PIN = 0;
    POUT = VO_RATIO * IO_RATIO * total_sumP2 * POCAL / total_numberOfSamples;
    if (POUT < 3 || IO == 0)
      POUT = 0;

    frequency = 1.0 / ((total_cycle_period / numberOfCycles) / 1000000.0);

    //Genero la trama a enviar
    generoTramaI2C();


    // Reset integer mains cycle accumulators
    numberOfCycles = 0;
    total_cycle_period = 0;

    total_numberOfSamples = 0;
    total_sumV = 0;

    total_sumI1 = 0;
    total_sumP1 = 0;

    total_sumI2 = 0;
    total_sumP2 = 0;

    total_sumV2 = 0;



    //Short Flash Led de estado
    digitalWrite(LEDpin, HIGH); delay(500); digitalWrite(LEDpin, LOW);

  }
}
//=============================================================================================================
//Respuesta a la request del maestro... Envio los datos
//
void requestEvent() {
  char __dataFileName[100] = {0};
  dataFileName.toCharArray(__dataFileName, sizeof(__dataFileName));
  Wire.write(__dataFileName);  /*send string on request */
  validacion = 1;

}
//=============================================================================================================
// Genero la trama para enviarle al maestro cuando lo solicite
//
void generoTramaI2C() {

  // Print output
  Serial.println("=======================================================================================================================");
  Serial.println("ENTRADA ==> VIN: " + String(VI, 1) + "V| IIN : " + String(II, 3) + "A| PIN : " + String(PIN, 2) + "| F: "  + String(frequency, 2) + "Hz");
  Serial.println("SALIDA  ==> VO : " + String(VO, 1) + "V| IOUT: " + String(IO, 3) + "A| POUT: " + String(POUT, 2) + "| F: " + String(frequency, 2) + "Hz");
  Serial.println("=======================================================================================================================");
  Serial.println();



  dataFileName = String(VI, 2) + "|" + String(II, 2) + "|" + String(round(PIN)) + "|" + String(VO, 2) + "|" + String(IO, 2) + "|" + String(round(POUT)) + "|" + "FIN!";
  Serial.print("'");
  Serial.print(dataFileName);
  Serial.println("'");
  Serial.println("| VI | II | PI | VO | IO | PO | VALIDACION");
  Serial.println();
}
