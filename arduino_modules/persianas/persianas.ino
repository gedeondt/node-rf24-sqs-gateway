#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Wire.h>
 
//Declaremos los pines CE y el CSN
#define CE_PIN 9
#define CSN_PIN 10

#define P1_BTN_UP 7
#define P1_BTN_DOWN 8
#define P2_BTN_UP 5
#define P2_BTN_DOWN 4

#define P1_LINE_ON A0
#define P1_LINE_SELECT A1
#define P2_LINE_ON A2
#define P2_LINE_SELECT 6

#define TIMER_OFF 0
#define TIMER_READY 1
#define TIMER_DELAY 40

#define GO_UP 1
#define GO_DOWN 0

#define RELAY_ON 0
#define RELAY_OFF 1

//Variable con la dirección del canal que se va a leer
const uint64_t direccion = 0x72646F4E31LL; 

//creamos el objeto radio (NRF24L01)
RF24 radio(CE_PIN, CSN_PIN);

//vector para los datos recibidos
char datos[32];   // deviceid[4]+type[4]+operation[4]+data[20]
String device = "0000"; // Yo
String type = "1111"; // Persiana
String operationDown = "2222";
String operationUp = "3333";

byte stateP1 = 0b00000000;
byte stateP2 = 0b00000000;

int timer = TIMER_OFF; // En 0 parado, en 1 salta

void setup()
{
  // Botones
  pinMode(P1_BTN_UP,INPUT_PULLUP);
  pinMode(P1_BTN_DOWN,INPUT_PULLUP);
  pinMode(P2_BTN_UP,INPUT_PULLUP);
  pinMode(P2_BTN_DOWN,INPUT_PULLUP);

  // Salidas a relés
  pinMode(P1_LINE_ON,OUTPUT);
  pinMode(P1_LINE_SELECT,OUTPUT);
  pinMode(P2_LINE_ON,OUTPUT);
  pinMode(P2_LINE_SELECT,OUTPUT);
  digitalWrite(P1_LINE_ON, RELAY_OFF);
  digitalWrite(P1_LINE_SELECT, RELAY_OFF);
  digitalWrite(P2_LINE_ON, RELAY_OFF);
  digitalWrite(P2_LINE_SELECT, RELAY_OFF);
  
  //inicializamos el puerto serie
  Serial.begin(115200);
  
  if(radio.begin()) Serial.println("Arrancando radio");
  else Serial.println("Error arrancando radio");
  
  //Abrimos el canal de Lectura
  radio.openReadingPipe(1, direccion);
  
  //empezamos a escuchar por el canal
  radio.startListening();
 
}

// Paramos motores y timer
void stopMotor(int lineOn, int lineSelect)
{
  digitalWrite(lineOn, RELAY_OFF);
  digitalWrite(lineSelect, RELAY_OFF);
  timer = TIMER_OFF;
}

// Enciende el motor y arranca el timer
void startMotor(int lineOn, int lineSelect, int dir)
{
  stopMotor(lineOn, lineSelect);
  digitalWrite(lineOn, RELAY_ON);
  digitalWrite(lineSelect, dir);
  timer = TIMER_DELAY;
}

void printByte(byte what)
{
  for(int i = 7; i>=0; i--) {
    Serial.print((char)('0' + ((what>>i)&1)));
  }
  Serial.println();
}

void checkButtons(byte *state, int btnUP, int bntDOWN, int lineOn, int lineSelect)
{
  byte newState = 0b00000000;
  
  bitWrite(newState, 0, !digitalRead(btnUP));
  bitWrite(newState, 1, !digitalRead(bntDOWN));
  
  if(newState != *state)
  {
    Serial.println("\nCambio de estado\n");

    switch((int) newState) {
      case 0b00000000: stopMotor(lineOn, lineSelect); break;
      case 0b00000001: startMotor(lineOn,lineSelect, GO_DOWN); break;
      case 0b00000010: startMotor(lineOn,lineSelect, GO_UP); break;
    }

    *state = newState;
  }
}

void checkTimer()
{
  if(timer != TIMER_OFF)
  {
    if(timer == TIMER_READY)
    {
      Serial.println("Timer salta");
      timer = TIMER_OFF;
      stopMotor(P1_LINE_ON, P1_LINE_SELECT);
      stopMotor(P2_LINE_ON, P2_LINE_SELECT);
    }
    else
    {
      Serial.print("Timer ");
      Serial.println(timer);
      timer = timer - 1;
    }
  }
}
 
void loop() {
 uint8_t numero_canal;

 checkButtons(&stateP1, P1_BTN_UP, P1_BTN_DOWN, P1_LINE_ON, P1_LINE_SELECT);
 checkButtons(&stateP2, P2_BTN_UP, P2_BTN_DOWN, P2_LINE_ON, P2_LINE_SELECT);
 
 if ( radio.available() )
 {    

     Serial.println("[Radio] Mensaje");

     //Leemos los datos y los guardamos en la variable datos[]
     radio.read(datos,sizeof(datos));
     
     String strDatos = String(datos);
     String strDevice = strDatos.substring(0,4);
     String strType = strDatos.substring(4,8);
     String strOperation = strDatos.substring(8,12);
     String strMotor = strDatos.substring(12,13);
     
     Serial.print("Device: ");
     Serial.println(strDevice);
     Serial.print("Type: ");
     Serial.println(strType);
     Serial.print("Operation: ");
     Serial.println(strOperation);

     if(strDevice == device)
     {
        Serial.println("[Radio] Mensaje para mi");
        Serial.println(datos);
        if(strType == type)
        {
          Serial.println("[Radio] Para el módulo persiana");
          if(strOperation == operationDown)
          {
              Serial.println("[Radio] Bajar persiana");
              int lineOn = strMotor == "0" ? P1_LINE_ON : P2_LINE_ON;
              int lineSelect = strMotor == "0" ? P1_LINE_SELECT : P2_LINE_SELECT;
              startMotor(lineOn, lineSelect, GO_DOWN);
          }

          if(strOperation == operationUp)
          {
              Serial.println("[Radio] Subir persiana");
              int lineOn = strMotor == "0" ? P1_LINE_ON : P2_LINE_ON;
              int lineSelect = strMotor == "0" ? P1_LINE_SELECT : P2_LINE_SELECT;
              startMotor(lineOn, lineSelect, GO_UP);
          }
        }
     }
     else
     {
        Serial.println("[Radio] No parecep para mi");
        Serial.println(datos);
     }
 }

 checkTimer();
 delay(1000);
}