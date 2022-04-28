#include "BluetoothSerial.h"

BluetoothSerial BT_ESP;

//************************ Variables de palanca **********************************
struct tiempo {
  unsigned long previo;
  unsigned long intervalo;
};

struct dir {
  unsigned char izq;
  unsigned char der;
};


struct Boton {
  const uint8_t PIN;
  char caracter;
  bool estado;
  struct tiempo mils;
};
Boton shoot1 = {18, 'S', 0};
Boton shoot2 = {19, 'X', 0};

struct Joystick {
  const uint8_t PIN;
  uint16_t volt;
  struct tiempo mils;
  struct dir movimiento;
};
Joystick P1 {33, 0};
Joystick P2 {32, 0};

void IRAM_ATTR ISR1(){
  shoot1.estado = 1;
}

void IRAM_ATTR ISR2(){
  shoot2.estado = 1;
}

unsigned long currentMillis;

void setup() {
  Serial.begin(115200);
  BT_ESP.begin("ESP32");
  input_setup();

  control_setup();
  attachInterrupt(shoot1.PIN, ISR1, HIGH);
  attachInterrupt(shoot2.PIN, ISR2, HIGH);
}



void loop() {
  currentMillis = millis();

  
  Joy_function (&P1);
  Joy_function (&P2);

  disparo (&shoot1);
  disparo (&shoot2);
}

void input_setup (){  
  pinMode(shoot1.PIN, INPUT_PULLUP);
  pinMode(shoot2.PIN, INPUT_PULLUP);
  pinMode(P1.PIN, INPUT);
  pinMode(P2.PIN, INPUT);
}

void control_setup (){
  P1.movimiento = {'A', 'D'};
  P2.movimiento = { 'C', 'Z'};

  P1.mils = {0, 100};
  P2.mils = P1.mils;
}

void Joy_function (struct Joystick *sel){
  if (currentMillis - (sel->mils.previo) > (sel->mils.intervalo)){
    sel->volt = analogRead ((sel->PIN));

    if ((sel->volt) > 2500){
      Serial.write(sel->movimiento.der);
      BT_ESP.print(sel->movimiento.der);
    }

    if ((sel->volt) < 800){
      Serial.write(sel->movimiento.izq);
      BT_ESP.print(sel->movimiento.izq);
    }
    sel->mils.previo = currentMillis;
  
  }
}

void disparo (struct Boton *sel){
  if (!digitalRead(sel->PIN) && (sel->estado)){
    Serial.print(sel->caracter);
    BT_ESP.print(sel->caracter);
    sel->estado = 0;
  }
}
