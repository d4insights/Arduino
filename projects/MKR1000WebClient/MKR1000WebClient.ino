//  WiFi Web Client temporizado para MKR1000
//  Intenta conectar 1 vez por minuto al server 192.168.1.178 Raspberry 
//  para llamar al /index.php que tengo corriendo en el Apache2 Server
//  es una base para la automatización de sensores 
//
//  Como emeplo en este caso solo uso un sensor LDR de luz
//
//  Funciona tanto con:
//      páginas estándar  (puerto=80) 
//      páginas SSL       (puerto=443) 
//    
//  Por WIFI UDP seteo el reloj RTC del MKR1000 con el time zone de Argentina
//  con el RTC se puede temporizar las llamadas del Cliente al Server 
//
//  Utilizando un LC microSD lee los seteos iniciales de configuración de un archivo
//  de texto en la raíz de la SD llamado config.txt 
//  Conexión MicroSD
//          MOSI - pin D08 
//          MISO - pin D10 
//           CLK - pin D09 
//            CS - pin D12  (pata llamada SCL)
//
//  Material adicional de lectura adicional: http://arduino.cc/en/Tutorial/
//


#include <SPI.h>
#include <SD.h>
#include <WiFi101.h>
#include <WiFiUdp.h>
#include <RTCZero.h>

String ssid = "";               // SSID (name) la declaro vacía porque la pongo en el archivo de configuración en la SD             
String pass = "";               // password la declaro vacía porque la pongo en el archivo de configuración en la SD
int keyIndex = 0;               // network key Index number (needed only for WEP)


int status = WL_IDLE_STATUS;
char server[] = "000.000.0.000";      // Servidor al que me quiero conectar (dejar el patern con los ceros para que reserve el espacio de memorial
String IPserver = "";                 // IP del server en el archivo por String para leer de la SD
//int puerto = 443;                   // Activo este puerto si la conexion es SSL    
int puerto = 80;

bool sendRequest = true;          // Variable de estado para saber si tengo que mandar el request HTTP

  
// Inicialización del Cliente WEB 
// Estándar puerto 80 
// SSL puerto 443
WiFiClient client;
//WiFiSSLClient client;           // Cambiaría línea de arriba para activar la conexión SSL

// Creo el objeto reloj
RTCZero rtc;
const int GMT = -3;               // Zona GMT-3 para fijar la hora en Internet
unsigned long epoch;
int numberOfTries = 0, maxTries = 6;


// Seteos de la MicroSD
int cs = 12;                                  // Pata SCL de control en el MKR1000
File myFile;                                  // Objeto Archivo
char myFileName[] = "config.txt";             // Nombre del archivo (si no existe lo crea)
String LineString = "";                       // Variable auxiliar para guardar cada línea del archivo de configuración que leo
//String parametro;                           // Acá pondré las instrucciones que busco en el archivo de configuración


int sensorLuz = A6;                           // Pin en el que conecto el sensor LDR de Luz
int NivelLuz = 0;                             // Valor del Nivel de luz que voy a tomar desde el LDR

  
void setup() {
 
  pinMode(cs, OUTPUT);        // Declaración de la pata de control de MicroSD
  pinMode(SS, OUTPUT);        // No tocar esto lo definen las especificaciones de fabricante MicroSD
  pinMode(sensorLuz, INPUT);  // Sensor de Luz

  
  Serial.begin(115200);   // Inicializo el monitor serial
  Serial.println("Ready to go..");
  
  motarMicroSD();         // Solo compruebo que la SD esta instalada y accesible
  cargarConfigTXT();      // leo la configuración desde el archivo ./config.txt

  connectToAP();          // Conexión al Router o AP
  printWifiStatus();      // Muestro en Monitor Serial el resultado de la conexión

  setDateTime();          // Seteo la Fecha Hora con el reloj de Internet
  setAlarm();             // Fijo una alarma cada 1 minuto
 
}



void loop() {
  if (sendRequest) 
  {
  
    if(status != WL_CONNECTED)
    {
        connectToAP();
    }
    sendRequest = false;
    httpRequest();        // Envío el request al Server vía GET de HTML
    listenToClient();     // Me quedo escuchando la respuesta HTML
  }
}



void printWifiStatus() {
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}


void alarmMatch() {
  sendRequest = true;
}


void connectToAP() {
  // Checkeo si la placa WIFI esta disponible
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi board not present");
    while (true);      
  }
  delay(1000);

 
  // Intento conectar al Router o AP vía WIFI
  Serial.print("Conectando a SSID: ");
  Serial.print(ssid);
  Serial.print(" ");
  while ( status != WL_CONNECTED) {
    Serial.print(".");
    status = WiFi.begin(ssid, pass);   // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    delay(1000);
  }
  Serial.println(" Connected!!");
}

// this method makes a HTTP connection to the server:
void httpRequest() {
  sendRequest = false;

  // Muestro por Monitor Serial request time
  Serial.println();
  Serial.print("Request sent @ ");
  printDate();
  printTime();
  Serial.println();
  
  // Lectura del sensor de lus
  NivelLuz = analogRead(sensorLuz);
  NivelLuz = map(NivelLuz,0,1023,100,0);    // Remapeo el valor e invierto el sentido por el LDR da 1023 (nada luz y 0 mucha luz)
  
  if (client.connect(server, puerto)) {
    
    // Make a HTTP request:
    client.print("GET /index.php?Sensor=LDR01&NivelLuz=");
    client.println(NivelLuz);

    client.print("Host: ");
    client.println(server);
    client.println("Connection: close");
    client.println();
  }
  else {
    Serial.println("connection failed");
  }
}



void listenToClient()
{
  unsigned long startTime = millis();
  bool received = false;

  while ((millis() - startTime < 10000) && !received) { //try to listen for 10 seconds
    while (client.available()) {
      received = true;
      char c = client.read();
      Serial.write(c);
    }
  }
  client.stop();
  Serial.println();
}


void print2digits(int number) {
  if (number < 10) {
    Serial.print("0");
  }
  Serial.print(number);
}



void setDateTime(){

  rtc.begin();
  //rtc.setTime(hours, minutes, seconds);     //Seteo manual de la hora
  //rtc.setDate(day, month, year);            //Seteo manual de la fecha

  do {
    epoch = WiFi.getTime();
    numberOfTries++;
  }
  while ((epoch == 0) || (numberOfTries > maxTries));

  if (numberOfTries > maxTries) {
    Serial.print("Servidor NTP (Network Time Protocol) no accesible!!");
    while (1);
  }
  else {
    Serial.print("Epoch (Fecha/Hora): ");
    Serial.println(epoch);
    rtc.setEpoch(epoch);     //Seteo de Fecha / Hora Internet sin ajuste de GMT
    Serial.println();

    // Ajuste de hora según la configuración GMT-3 de Argentina
    // Si la hora es las 00 me tira la hora 29 del día posterior
    // Osea si la hora es menor que las 3AM tengo que restar 1 al día y recalcular la hora
    // El fragmento de código de años bisiestos y manejo de fin de mes y fin de año no están probados
    
    if (rtc.getHours()>3)
    {
        rtc.setTime((rtc.getHours()+GMT), rtc.getMinutes(), rtc.getSeconds());
    } 
    else
    {
        rtc.setTime((24+rtc.getHours()+GMT), rtc.getMinutes(), rtc.getSeconds());
        
        // Cualquier día no fin de mes, ni fin de año, ni año bisiesto
        rtc.setDate(rtc.getDay()-1, rtc.getMonth(), rtc.getYear()); 

        // Fin de año tradicional
        if((rtc.getMonth()==1) && (rtc.getDay()==1))
        {
            rtc.setDate(31, 12, rtc.getYear()-1); 
        }

        // Fin de mes tradicional cuando el mes anterior fue de 31 días
        if(((rtc.getMonth()==2) || (rtc.getMonth()==4) || (rtc.getMonth()==6) || (rtc.getMonth()==8) || (rtc.getMonth()==9) || (rtc.getMonth()==11)) && (rtc.getDay()==1))
        {
              rtc.setDate(31, rtc.getMonth()-1, rtc.getYear());
        }

        // Fin de mes tradicional cuando el mes anterior fue de 30 días
        if(((rtc.getMonth()==5) || (rtc.getMonth()==7) || (rtc.getMonth()==10) || (rtc.getMonth()==12)) && (rtc.getDay()==1))
        {
              rtc.setDate(30, rtc.getMonth()-1, rtc.getYear());
        }

        // Caso particular de MARZO pero en años que no son bisiestos
        if ((rtc.getMonth()==3) && (rtc.getDay()==1))
        {
            rtc.setDate(28, 2, rtc.getYear());
        }
        
        // Años bisiestos
        if ((rtc.getYear()==2020 || rtc.getYear()==2024 || rtc.getYear()==2028) && (rtc.getMonth()==3) && (rtc.getDay()==1))
        {
            rtc.setDate(29, 2, rtc.getYear());
        }
     
    }
  }
  
}


void setAlarm(){
  rtc.setAlarmTime(0, 0, 0);    //Setea alarma cada 1 minuto 
  rtc.enableAlarm(rtc.MATCH_SS);
  rtc.attachInterrupt(alarmMatch);
} 

 
void printTime(){
  print2digits(rtc.getHours());
  Serial.print(":");
  print2digits(rtc.getMinutes());
  Serial.print(":");
  print2digits(rtc.getSeconds());
  Serial.println();
}


void printDate() {
  Serial.print(rtc.getDay());
  Serial.print("/");
  Serial.print(rtc.getMonth());
  Serial.print("/");
  Serial.print(rtc.getYear());

  Serial.print(" ");
}


void motarMicroSD() {
  // Controlo si la SD esta disponible
  if (!SD.begin(cs)) {
    Serial.println("SD did not initiliaze");   
    while (1) ;
  }
  else {
    Serial.println("SD initialized.");
  }
}


void cargarConfigTXT(){
  String aux = "";
  
  Serial.println("Leyendo configuración...");
  Serial.println();
 
  // Abro el archivo para lectura
  myFile = SD.open(myFileName, FILE_READ);
  
  // Leo línea a línea (hasta nueva línea \n)
  while (myFile.available() != 0) {  
    LineString = myFile.readStringUntil('\n');
    Serial.print(LineString);  
    Serial.print(" --> ");

    // Busco si está el parámetro "ssid=" en la línea para cargarlo 
    aux = "ssid=";
    if (LineString.startsWith(aux)) {
      ssid = LineString.substring(aux.length(),LineString.length()-1);
      Serial.print("<");
      Serial.print(ssid);
      Serial.print(">");
    }

    // Busco si está el parámetro "password=" en la línea para cargarlo 
    aux = "password=";
    if (LineString.startsWith(aux)) {
      pass = LineString.substring(aux.length(),LineString.length()-1);
      Serial.print("<");
      Serial.print(pass);
      Serial.print(">");
    }

    // Busco si está el parámetro "server=" en la línea para cargarlo 
    aux = "server=";
    if (LineString.startsWith(aux)) {
      IPserver = LineString.substring(aux.length(),LineString.length()-1);
      Serial.print("<");
      Serial.print(IPserver);
      Serial.print(">");
      int a;
      for(a=0; a<IPserver.length(); a++){
        server[a] = IPserver.charAt(a);
      }
      //server[a]='\0';
    }
    
    Serial.println();     
  } 
  myFile.close(); 
  Serial.println("Configuración cargada desde ./config.txt");
  Serial.println();
}

