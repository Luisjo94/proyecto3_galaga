#include <SPI.h>
#include <SD.h>

#define R   PF_1
#define G   PF_3
#define B   PF_2

int opcion = 0;
int menu = 0;


File pokeball;
File sans;
File nft;



void setup()
{

  // ---- pines RGB ----
  pinMode(R, OUTPUT);
  pinMode(G, OUTPUT);
  pinMode(B, OUTPUT);

  //apagarlos al inicio
  digitalWrite(R, LOW);
  digitalWrite(G, LOW);
  digitalWrite(B, LOW);
  
  
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  SPI.setModule(0);


  Serial.print("Inicializando la SD...");
  // On the Ethernet Shield, CS is pin 4. It's set as an output by default.
  // Note that even if it's not used as the CS pin, the hardware SS pin
  // (10 on most Arduino boards, 53 on the Mega) must be left as an output
  // or the SD library functions will not work.
  pinMode(10, OUTPUT);

  if (!SD.begin(4)) {
    Serial.println("Ha ocurrido un error en la inicializacion");
    return;
  }
  Serial.println("Inicializacion completa.");


  
    // ------------------------- MENU ------------------------
    Serial.println("");
    Serial.println("Ingrese la opcion que desee realizar ");
    Serial.println("1. Abrir pokeball.txt");
    Serial.println("2. Abrir sans.txt");
    Serial.println("3. Abrir nft.txt");
    Serial.println("");
  
    digitalWrite(R, LOW);
    digitalWrite(G, LOW);
    digitalWrite(B, LOW);

}

void loop()
{

    
    opcion = Serial.read();

    

  // ------------------------- ABRIR POKEBALL ------------------------
  if (opcion == 49)
  {
    digitalWrite(R, HIGH);
    digitalWrite(G, LOW);
    digitalWrite(B, LOW);
    Serial.println("Abriendo pokeball.txt");
    
    pokeball = SD.open("pokeball.txt");
    if (pokeball) 
    {
      Serial.println("pokeball.txt:");
  
      // read from the file until there's nothing else in it:
      while (pokeball.available()) {
        Serial.write(pokeball.read());
      }
      // close the file:
      pokeball.close();
    } 
    else 
    {
      // if the file didn't open, print an error:
      Serial.println("error abriendo pokeball.txt");
    }
    
    menu = 1;
  }

  // ------------------------- ABRIR SANS ------------------------
  if (opcion == 50)
  {
    digitalWrite(R, LOW);
    digitalWrite(G, HIGH);
    digitalWrite(B, LOW);
    Serial.println("Abriendo sans.txt");
    
    sans = SD.open("sans.txt");
    if (sans) 
    {
      Serial.println("sans.txt:");
  
      // read from the file until there's nothing else in it:
      while (sans.available()) {
        Serial.write(sans.read());
      }
      // close the file:
      sans.close();
    } 
    else 
    {
      // if the file didn't open, print an error:
      Serial.println("error abriendo sans.txt");
    }
    
    menu = 1;
  }


  // ------------------------- ABRIR CRYPTOPUNK ------------------------
  if (opcion == 51)
  {
    digitalWrite(R, LOW);
    digitalWrite(G, LOW);
    digitalWrite(B, HIGH);
    Serial.println("Abriendo nft.txt");
    
    nft = SD.open("nft.txt");
    if (nft) {
      Serial.println("nft.txt:");
  
      // read from the file until there's nothing else in it:
      while (nft.available()) {
        Serial.write(nft.read());
      }
      // close the file:
      nft.close();
    } else {
      // if the file didn't open, print an error:
      Serial.println("Ha ocurrido un error abriendo nft.txt");
    }
    
    
    menu = 1;
  }

  if (menu == 1)
  {
    Serial.println("");
    Serial.println("Ingrese la opcion que desee realizar ");
    Serial.println("1. Abrir pokeball.txt");
    Serial.println("2. Abrir sans.txt");
    Serial.println("3. Abrir nft.txt");
    Serial.println("");
    menu = 0;
  }
  
}































//menu 
//
//if 1 abrir
//else menu
//
//menu
