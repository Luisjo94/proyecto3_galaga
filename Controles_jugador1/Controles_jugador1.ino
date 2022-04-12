//************************ Variables de botones **********************************
struct Boton {
  const uint8_t PIN;
  char caracter;
  bool estado;
}

//se define lo que hace cada boton (pin, character que representa y su estado actual)
Boton up = {25, 'W', false};
Boton left = {33, 'A', false};
Boton down = {32, 'S', false};
Boton right = {32, 'D', false};

//************************ Codigo de interrupciones **********************************
void IRAM_ATTR isr() {
  if (digitalRead(up.PIN)){//se preciono up
    up.estado = true;
    //lo que queremos que haga
    
  }

  if (digitalRead(left.PIN)){//se preciono left
    left.estado = true;
    //lo que queremos que haga
    
  }

  if (digitalRead(down.PIN)){//se preciono down
    down.estado = true;
    //lo que queremos que haga
    
  }

  if (digitalRead(right.PIN)){//se preciono right
    right.estado = true;
    //lo que queremos que haga
    botones_setup ();
  }
}


void setup() {
  Serial.begin(115200);
  
  attachInterrupt(up.PIN, isr, FALLING);
}

void loop() {
  // put your main code here, to run repeatedly:

}

void botones_setup (){
  pinMode(up.PIN, INPUT_PULLUP);
  pinMode(left.PIN, INPUT_PULLUP);
  pinMode(down.PIN, INPUT_PULLUP);
  pinMode(right.PIN, INPUT_PULLUP);
}

void interrupt (){
  attachInterrupt(up.PIN, isr, FALLING);
  attachInterrupt(left.PIN, isr, FALLING);
  attachInterrupt(down.PIN, isr, FALLING);
  attachInterrupt(right.PIN, isr, FALLING);
}
