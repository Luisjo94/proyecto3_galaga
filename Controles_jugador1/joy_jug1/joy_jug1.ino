#include "BluetoothSerial.h"

BluetoothSerial BT_ESP;

//************************ Variables de palanca **********************************
struct Boton {
  const uint8_t PIN;
  char caracter;
  bool estado;
};
Boton shoot = {18, 'E', 0};

struct Joystick {
  const uint8_t PIN;
  uint16_t volt;
};
Joystick ejeX {33, 0};
Joystick ejeY {32, 0};

void IRAM_ATTR ISR(){
  shoot.estado = 1;
  
}

void setup() {
  Serial.begin(115200);
  BT_ESP.begin("ESP32");


  
  input_setup();
  attachInterrupt(shoot.PIN, ISR, HIGH);
}



void loop() {
//  BT_ESP.println("Hola mundo");
//  delay(10000);


  
  Joy_function(); 
  but ();
}

void input_setup (){
  pinMode(shoot.PIN, INPUT_PULLUP);
  pinMode(ejeX.PIN, INPUT);
  pinMode(ejeY.PIN, INPUT);

}

void Joy_function () {
  //lee los valores en eje X y X
  ejeX.volt = analogRead (ejeX.PIN);
  ejeY.volt = analogRead (ejeY.PIN);

  //Letra correspondiente de acuerdo a donde se encuentre la palanca
  if (ejeX.volt > 2500){
    Serial.print ("D");
    BT_ESP.print("D");
    delay(50);
  }

  if (ejeX.volt < 800) {
    Serial.print ("A");
    BT_ESP.print("A");
    delay(50);
  }

  if (ejeY.volt < 800){
    Serial.print ("W");
    BT_ESP.print("W");
    delay(50);
  }

  if (ejeY.volt >2500){
    Serial.print ("S");
    BT_ESP.print("S");
    delay(50);
  }
}

void but () {
  if (!digitalRead(shoot.PIN) && shoot.estado){
    Serial.println(shoot.caracter);
    BT_ESP.println(shoot.caracter);
    shoot.estado = 0;
    //delay(10);
  }
  //3.3v > push > pin boton, resistencias > gnd
}
