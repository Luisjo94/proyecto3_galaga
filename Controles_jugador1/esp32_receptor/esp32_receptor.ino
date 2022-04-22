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


//ESP now
typedef struct struct_message {
    int id; // must be unique for each sender board
    int movimiento; //A donde se desplaza
} struct_message;
struct_message myData;

// Crear una estructura con las ESP a utilizar
struct_message jugador1;
//struct_message jugador2;

// Crear un array de los ESP
//struct_message boardsStruct[2] = {jugador1, jugador2};
struct_message boardsStruct[1] = {jugador1};



void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  ESP_setup();
}

void loop() {
  // put your main code here, to run repeatedly:
  
}



void ESP_setup(){  
  //Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  //Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_register_recv_cb(OnDataRecv);
}

void OnDataRecv(const uint8_t * mac_addr, const uint8_t *incomingData, int len) {
  char macStr[18];
  Serial.print("Packet received from: ");
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.println(macStr);
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.printf("Board ID %u: %u bytes\n", myData.id, len);
  // Update the structures with the new incoming data
  boardsStruct[myData.id-1].caracter = myData.caracter;
  Serial.printf("caracter: %d \n", boardsStruct[myData.id-1].caracter);
  Serial.println();
}
