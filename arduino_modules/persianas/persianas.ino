#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Wire.h>
 
//Declaremos los pines CE y el CSN
#define CE_PIN 9
#define CSN_PIN 10

#define P1_BTN_UP 11
#define P1_BTN_DOWN 11
#define P2_BTN_UP 11
#define P2_BTN_DOWN 11

#define P1_LINE_UP 11
#define P1_LINE_DOWN 11
#define P2_LINE_UP 11
#define P2_LINE_DOWN 11

#define TIMER_OFF 0
#define TIMER_READY 1
#define TIMER_DELAY 30

//Variable con la dirección del canal que se va a leer
const uint64_t direccion = 0x72646F4E31LL; 

//creamos el objeto radio (NRF24L01)
RF24 radio(CE_PIN, CSN_PIN);

//vector para los datos recibidos
char datos[32];   // deviceid[4]+type[4]+operation[2]+data[22]

int stateP1, newStateP1 = 0;
int stateP2, newStateP2 = 0;

int timer = TIMER_OFF; // En 0 parado, en 1 salta

void setup()
{
  // Botones
  pinMode(P1_BTN_UP,INPUT_PULLUP);
  pinMode(P1_BTN_DOWN,INPUT_PULLUP);
  pinMode(P2_BTN_UP,INPUT_PULLUP);
  pinMode(P2_BTN_DOWN,INPUT_PULLUP);

  // Salidas a relés
  pinMode(P1_LINE_UP,OUTPUT);
  pinMode(P1_LINE_DOWN,OUTPUT);
  pinMode(P2_LINE_UP,OUTPUT);
  pinMode(P2_LINE_DOWN,OUTPUT);
  
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
void stopMotor(int lineUP, int lineDOWN)
{
  stateP1, newStateP1 = 0;
  stateP2, newStateP2 = 0;

  digitalWrite(lineUP, 0);
  digitalWrite(lineDOWN, 0);
  timer = TIMER_OFF;
}

// Enciende el motor y arranca el timer
void startMotor(int line)
{
  digitalWrite(line, 1);
  timer = TIMER_DELAY;
}

void checkButtons(int *state, int *newState, int btnUP, int bntDOWN, int lineUP, int lineDOWN)
{
  bitWrite(*newState, digitalRead(btnUP), 0);
  bitWrite(*newState, digitalRead(bntDOWN), 1);

  if(newState != state)
  {
    Serial.println("Cambio de estado");
    Serial.println(*newState);

    switch((int) newState) {
      case 0: stopMotor(lineUP, lineDOWN); break;
      case 1: stopMotor(lineUP, lineDOWN); startMotor(lineUP); break;
      case 2: stopMotor(lineUP, lineDOWN); startMotor(bntDOWN); break;
    }
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
      stopMotor(P1_LINE_UP, P1_LINE_DOWN);
      stopMotor(P2_LINE_UP, P2_LINE_DOWN);
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

 checkButtons(&stateP1, &newStateP1, P1_BTN_UP, P1_BTN_DOWN, P1_LINE_UP, P1_LINE_DOWN);
 checkButtons(&stateP2, &newStateP2, P2_BTN_UP, P2_BTN_DOWN, P2_LINE_UP, P2_LINE_DOWN);
 
 //if ( radio.available(&numero_canal) )
 if ( radio.available() )
 {    
     //Leemos los datos y los guardamos en la variable datos[]
     radio.read(datos,sizeof(datos));
     
     //reportamos por el puerto serial los datos recibidos
     Serial.print(datos[0]);
     Serial.print(datos[1]);
     Serial.println(datos[2]);

 }
 else
 {
     Serial.println("No hay datos de radio disponibles");
 }

 checkTimer();
 delay(1000);
}
