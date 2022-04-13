//************************ Variables de botones **********************************
struct Boton {
  const uint8_t PIN;
  char caracter;
  bool estado;
  //bool auxiliar;
};

//se define lo que hace cada boton (pin, character que representa y su estado actual)
//Boton up = {25, 'W', false};
//Boton left = {33, 'A', false};
//Boton down = {32, 'S', false};
//Boton right = {35, 'D', false};
Boton shoot = {34, 'E', false};

struct Joystick {
  const uint8_t PIN;
  uint16_t volt;
  const uint16_t centro;
};
//dead zone de 100 en x
//dead zone de 75 en y
Joystick ejeX {33, 0, 1095};
Joystick ejeY {32, 0, 1894};

void setup() {
  Serial.begin(115200);
  
  input_setup();
  //interrupt_setup();
}

void loop() {
  // put your main code here, to run repeatedly:
  if (!digitalRead(shoot.PIN)){
    shoot.estado = true;
    if (digitalRead(shoot.PIN) && shoot.estado){
      Serial.println(shoot.caracter);
      shoot.estado = false;
    }
  }

  //Que letra debe enviar
  ejeX.volt = analogRead (ejeX.PIN);
  ejeY.volt = analogRead (ejeY.PIN); //valores en X y Y
  
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

void input_setup (){
  pinMode(shoot.PIN, INPUT_PULLUP);
  pinMode(ejeX.PIN, INPUT);
  pinMode(ejeY.PIN, INPUT);
}

//void interrupt_setup (){
//  attachInterrupt(up.PIN, isr, FALLING);
//  attachInterrupt(left.PIN, isr, FALLING);
//  attachInterrupt(down.PIN, isr, FALLING);
//  attachInterrupt(right.PIN, isr, FALLING);
//}
