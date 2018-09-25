/* 
    Programa: Control BT para el control de luces de Cessna  

    Una vez conectado el HC-06 (PIN 1234) el control responde a las siguientes ordenes:
    Si acciono una combinación de luces, puedo desconectarme y el control continúa solo

    parking on        Luz fija de parking
    parking off
    navegation on     Luces de las alas secuencial
    navegation off
    timon on          Luz de cola secuencial
    timon off
    interior on       Luz interna de la cabina
    interior off
    panza on          Luz de panza del avión
    panza off
    stop all          Apaga todo
    start all         Prende todo
        
    Autor:    Diego Fernández Sanz
    Fecha:    06/11/2016  
*/
 
#include <SoftwareSerial.h>     //Librería que permite establecer comunicación serie en otros pins
#define RxD 10 
#define TxD 11 
 
 
char dato;            //Caracter que va guardando lo que entra vía BT
char cadena[255];     //Creamos un array de caracteres de 256 posiciones
int  i=0;             //Tamaño actual del array
int  alternaluces=0;  //Ciclo de 4 tiempos para alternar las luces de navegación 

int  navDerPin=3;     //PIN Luz de Navegacion DERECHA
int  navIzqPin=4;     //PIN Luz de Navegacion IZQUIERDA
int  parkingPin=5;    //PIN Luz de Parking
int  timonPin=6;      //PIN Luz de Timón
int  panzaPin=7;      //PIN Luz de panza
int  accesorioPin=8;  //PIN Accesorio
int  interiorPin=9;   //PIN Luz de interior

int  navegaSta=0;     //Estado de luces de navegación 
int  parkingSta=0;    //Estado de luces de Parking
int  timonSta=0;      //Estado de la luz del timon
int  interiorSta=0;   //Estado de la luz de interior de cabina
int  panzaSta=0;      //Estado de la luz de panza del avión
int  accesorioSta=0;  //Estado del accesorio

// Nueva instancia del objeto BlueTooth
SoftwareSerial BT(TxD, RxD); 
 
 
void setup() 
{   
  pinMode(parkingPin,OUTPUT);
  pinMode(navDerPin,OUTPUT);
  pinMode(navIzqPin,OUTPUT);
  pinMode(timonPin,OUTPUT);
  pinMode(interiorPin,OUTPUT);
  pinMode(panzaPin,OUTPUT);
  pinMode(accesorioPin,OUTPUT);
  

      
  BT.begin(9600); 
  Serial.begin(9600); 

  Serial.println("Ready..."); 
  BT.print("Ready..\n\n"); 

} 
 
 
void loop() 
{ 
  // Mientras tenga caracteres entrando por la comunicación BT 
  if (BT.available())
  {    
    dato=BT.read();       //Guarda los datos carácter a carácter en la variable "dato"
    cadena[i++]=dato;     //Vamos colocando cada carácter recibido en el array "cadena"
    
    // Manejo de últimos estados cada vez que cierra un mensaje
    //
    if(dato == '\n')
    {
      Serial.print(cadena); 
      
      if(strstr(cadena,"parking on")!=0)
      {
        parkingSta = 1;
      }
      if(strstr(cadena,"parking off")!=0)
      {
        parkingSta = 0;
      }


      
      if(strstr(cadena,"interior on")!=0)
      {
        interiorSta = 1;
      }
      if(strstr(cadena,"interior off")!=0)
      {
        interiorSta = 0;
      }


      if(strstr(cadena,"accesorio on")!=0)
      {
        accesorioSta = 1;
      }
      if(strstr(cadena,"accesorio off")!=0)
      {
        accesorioSta = 0;
      }


      
      if(strstr(cadena,"navegation on")!=0)
      {
        navegaSta = 1;
      }
      if(strstr(cadena,"navegation off")!=0)
      {
        navegaSta = 0;
      }      

      if(strstr(cadena,"timon on")!=0)
      {
        timonSta = 1;
      }
      if(strstr(cadena,"timon off")!=0)
      {
        timonSta = 0;
      }
      

      if(strstr(cadena,"panza on")!=0)
      {
        panzaSta = 1;
      }
      if(strstr(cadena,"panza off")!=0)
      {
        panzaSta = 0;
      }


      if(strstr(cadena,"stop all")!=0)
      {
        timonSta   = 0;
        navegaSta  = 0;
        parkingSta = 0;
        interiorSta = 0;
        panzaSta = 0;
        accesorioSta = 0;
      }
      
      if(strstr(cadena,"start all")!=0)
      {
        timonSta   = 1;
        navegaSta  = 1;
        parkingSta = 1;
        interiorSta = 1;
        panzaSta = 1;
        accesorioSta = 1;
      }
      
      clean();
    }
  }

  
  // Secuanciador de Luces
  //
  
  // Luces de Parking
  if(parkingSta==1)
  {
    digitalWrite(parkingPin,HIGH);
  }
  else
  {
    digitalWrite(parkingPin,LOW);
  }

  // Luces de Interior
  if(interiorSta==1)
  {
    digitalWrite(interiorPin,HIGH);
  }
  else
  {
    digitalWrite(interiorPin,LOW);
  }


  // Accesorio
  if(accesorioSta==1)
  {
    digitalWrite(accesorioPin,HIGH);
  }
  else
  {
    digitalWrite(accesorioPin,LOW);
  }
  
  // Luces de Navegación
  if(navegaSta==1)
  {
    if (alternaluces==0 || alternaluces==2 || alternaluces==4)
    {
      digitalWrite(navDerPin,HIGH);
      digitalWrite(navIzqPin,LOW);
    }
    if (alternaluces==1 || alternaluces==3 || alternaluces==5)
    {
      digitalWrite(navDerPin,LOW);
      digitalWrite(navIzqPin,HIGH);
    }
    delay(300);
  }
  else
  {
    digitalWrite(navDerPin,LOW);
    digitalWrite(navIzqPin,LOW);
  }
  
  // Luces de Timon
  if(timonSta==1)
  {
    if (alternaluces==0 || alternaluces==2 || alternaluces==4)
    {
      digitalWrite(timonPin,HIGH);
    }
    if (alternaluces==1 || alternaluces==3 || alternaluces==5)
    {
      digitalWrite(timonPin,LOW);
    }
    delay(300);
  }
  else
  {
    digitalWrite(timonPin,LOW);
  }


  // Luces de Panza
  if(panzaSta==1)
  {
    if (alternaluces==1 || alternaluces==3 || alternaluces==5)
    {
      digitalWrite(panzaPin,HIGH);
    }
    if (alternaluces==0 || alternaluces==2 || alternaluces==4)
    {
      digitalWrite(panzaPin,LOW);
    }
    delay(300);
  }
  else
  {
    digitalWrite(panzaPin,LOW);
  }



    
  
  // Incrementa el secuenciador de luces para dar los efectos de combinación 
  if(alternaluces++ > 5)  
  {
    alternaluces=0;
  }
    
} 

// Limpia el array
void clean()
{
  for (int cl=0; cl<=i; cl++)
  {
    cadena[cl]=0;
  }
  i=0;
}


