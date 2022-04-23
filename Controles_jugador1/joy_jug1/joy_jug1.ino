 /*********
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp-now-many-to-one-esp32/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*********/
#include <esp_now.h>
#include <WiFi.h>

// Address del ESP reseptor
uint8_t broadcastAddress[] = {0x58, 0xBF, 0x25, 0x17, 0x3E, 0xA8};

//************************ Variables **********************************
//Para el boton
struct Boton {
  const uint8_t PIN;
  char caracter;
  uint8_t estado;
};
Boton shoot = {18, 'E', 0};

//Para la palanca
struct Joystick {
  const uint8_t PIN;
  uint16_t volt;
};
Joystick ejeX {33, 0};
Joystick ejeY {32, 0};


const char up = 'W';
const char down = 'S';
const char left = 'A';
const char right = 'D';


//ESP now
typedef struct struct_message {
    int id; // must be unique for each sender board
    int movimiento; //A donde se desplaza
} struct_message;
struct_message myData;


// Create peer interface
esp_now_peer_info_t peerInfo;



//************************ Interrupcion **********************************
void IRAM_ATTR ISR(){
  shoot.estado = 1;
}

//************************ Set-up **********************************
void setup() {
  Serial.begin(115200);
//  BT_ESP.begin("ESP32");

  ESP_now();
  
  input_setup();
  attachInterrupt(shoot.PIN, ISR, HIGH);
  myData.id = 1;
}


//************************ Loop **********************************
void loop() {
  Joy_function(); 
  but ();
}

//************************ Funciones **********************************
//-----------Setup--------------------


//Se activa el dispositivo como una estacion WiFi
void ESP_now () {  
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
}


void input_setup (){
  pinMode(shoot.PIN, INPUT_PULLUP);
  pinMode(ejeX.PIN, INPUT);
  pinMode(ejeY.PIN, INPUT);
}

//-------------Generales ----------------------
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void ESP_function () {
  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
   
  if (result == ESP_OK) {
    Serial.println("Sent with success");
  }
  else {
    Serial.println("Error sending the data");
  }
}

//lee e interpreta el joystick
void Joy_function () {
  //lee los valores en eje X y X
  ejeX.volt = analogRead (ejeX.PIN);
  ejeY.volt = analogRead (ejeY.PIN);

  //Letra correspondiente de acuerdo a donde se encuentre la palanca
  if (ejeX.volt > 2500){ //derecha
    Serial.print (right);
    //BT_ESP.print(right);
    myData.movimiento = right;
    ESP_function ();
    delay(50);
  }

  if (ejeX.volt < 800) { //izquierda
    Serial.print (left);
    //BT_ESP.print(left);
    myData.movimiento = left;
    ESP_function ();
    delay(50);
  }

  if (ejeY.volt < 800){ //arriba
    Serial.print (up);
    //BT_ESP.print(up);
    myData.movimiento = up;
    ESP_function ();
    delay(50);
  }

  if (ejeY.volt >2500){ //abajo
    Serial.print (down);
    //BT_ESP.print(down);
    myData.movimiento = down;
    ESP_function ();
    delay(50);
  }
}

//codigo de antirrebote
void but () {
  if (!digitalRead(shoot.PIN) && shoot.estado){
    Serial.println(shoot.caracter);
    //BT_ESP.println(shoot.caracter);
    myData.movimiento = shoot.caracter;
    ESP_function ();
    shoot.estado = 0;
  }
}
