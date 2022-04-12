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
Joystick ejeX {33, 0, 1905};
Joystick ejeY {32, 0, 1892};


//************************ Codigo de interrupciones **********************************

//void IRAM_ATTR isr() {
//  if (digitalRead(up.PIN)){//se preciono up
//    
//    up.estado = true;
//    //Serial.println(up.caracter);
//    //lo que queremos que haga
//    
//  }
//
//  if (digitalRead(left.PIN)){//se preciono left
//    left.estado = true;
//    //lo que queremos que haga
//    
//  }
//
//  if (digitalRead(down.PIN)){//se preciono down
//    down.estado = true;
//    //lo que queremos que haga
//    
//  }
//
//  if (digitalRead(right.PIN)){//se preciono right
//    right.estado = true;
//    //lo que queremos que haga
//    botones_setup ();
//  }
//}
//

int pendiente;

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
//  ejeX.mapeo = ejeX.volt - ejeX.centro;
//  ejeY.mapeo = ejeY.volt - ejeY.centro;

  if (((ejeX.volt < ejeX.centro - 100) && (ejeX.volt > ejeX.centro + 100)) || ((ejeY.volt < ejeY.centro - 100) && (ejeY.volt > ejeY.centro > 100))){
    
  
  if (ejeY.volt != 0) {
    pendiente = ejeX.volt/ejeY.volt;
    
    if (pendiente > 0.5 && pendiente <2){ //SD
      
          Serial.println("SD");
    }
  }
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
