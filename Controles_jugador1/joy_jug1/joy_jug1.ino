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
  uint8_t mapeo;
};
//dead zone de 100 en x
//dead zone de 75 en y
Joystick ejeX {33, 0, 119};
Joystick ejeY {32, 0, 138};


int pendiente;
int y_temp;
int x_temp;

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

  //0,0 ajustado en posicion de descanzo del Joystick

  ejeX.mapeo = map(ejeX.volt, 0, 4096, 0, 255);
  ejeY.mapeo = map(ejeY.volt, 0, 4096, 255, 0);

  y_temp = ejeY.centro - ejeY.mapeo;
  x_temp = ejeX.centro - ejeX.mapeo;
  
  if (y_temp == 0){
    pendiente = 0;
  }
  else {
    pendiente = x_temp/y_temp;
    
  }
  
  
  //Serial.printf("X: %u   Y: %u\n", ejeX.mapeo, ejeY.mapeo);
  Serial.println(pendiente);
  
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
