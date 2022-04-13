//************************ Variables de palanca **********************************
struct Boton {
  const uint8_t PIN;
  char caracter;
  bool estado;
};
Boton shoot = {19, 'E', false};

struct Joystick {
  const uint8_t PIN;
  uint16_t volt;
};
Joystick ejeX {33, 0};
Joystick ejeY {32, 0};

void IRAM_ATTR ISR(){

  shoot.estado = true;

}

void setup() {
  Serial.begin(115200);
  input_setup();
  attachInterrupt(shoot.PIN, ISR, FALLING);
}



void loop() {
  Joy_function(); 


  if (digitalRead(shoot.PIN) && shoot.estado){
    Serial.println(shoot.caracter);
    shoot.estado = false;
  }
  //3.3v > push > pin boton, resistencias > gnd
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
  }

  if (ejeX.volt < 800) {
    Serial.print ("A");
  }

  if (ejeY.volt < 800){
    Serial.print ("W");
  }

  if (ejeY.volt >2500){
    Serial.print ("S");
  }
}
