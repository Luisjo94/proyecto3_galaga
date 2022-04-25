//***************************************************************************************************************************************
/* Librería para el uso de la pantalla ILI9341 en modo 8 bits
   Basado en el código de martinayotte - https://www.stm32duino.com/viewtopic.php?t=637
   Adaptación, migración y creación de nuevas funciones: Pablo Mazariegos y José Morales
   Con ayuda de: José Guerra
   IE3027: Electrónica Digital 2 - 2019
*/
//***************************************************************************************************************************************
/* 
    Proyecto 3: Galaga
    Integrantes: Nicolas Urioste (19907) y Luis José Archila (20403)
    IE3027: Electrónica Digital 2 - 2022

    Se trabajó sobre la librería para el uso de pantalla ILI9341 previamente creada
*/
//***************************************************************************************************************************************
#include <stdint.h>
#include <stdbool.h>
#include <TM4C123GH6PM.h>


#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/rom_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"

#include "bitmaps.h"
#include "font.h"
#include "lcd_registers.h"


#define SW1 PF_4
#define SW2 PF_0


//***************************************************************************************************************************************
// Variables
//***************************************************************************************************************************************
//------------------- Estado del juego ----------------------
char estado_juego = 0; //al reainiciar es el estado default, pantalla de inicio
char start = 1; //bandera para cargar el menu
// 1, solo
// 2, duos
// 3, endgame
//  3.1 tiene highscore
//  3.2 no tiene highscore



//------------------- Posicion de naves ---------------------
struct ship {
  short ejeX;
  short ejeY;
  const char  alto;
  const char  ancho;
  unsigned long previo;
  unsigned long intervalo;
  char vidas;
  unsigned short score;
};
//Posicion inicial de naves jugadores
struct ship P1 = {155, 170, 15, 15, 0, 5};
struct ship P2 = {303, 170, 15, 15, 0, 2};

//Posicion enemigo
struct ship bad1 = {100, 100};

//------------------- disparos ---------------------------
struct object {
  short ejeX;
  short ejeY;
  char  alto;
  char  ancho;
  char  active;
  char  hit;
  unsigned long previo;
  unsigned long intervalo;
};
                                               
//balas de ambos jugadores
struct object bulletP1 = {0,0, 8, 3, 0, 0, 0, 2};
struct object bulletP2 = bulletP1;


//------------------- limites ---------------------------
struct Bound {
  short xMin;
  short xMax;
  short yMin;
  short yMax;
};
struct Bound player = {0, 304, 0, 224};

struct Menu {
  short startX;
  short startY;
  short width;
  short height;
};


//------------------- tiempo --------------------------


#define LCD_RST PD_0
#define LCD_CS PD_1
#define LCD_RS PD_2
#define LCD_WR PD_3
#define LCD_RD PE_1
int DPINS[] = {PB_0, PB_1, PB_2, PB_3, PB_4, PB_5, PB_6, PB_7};
//***************************************************************************************************************************************
// Functions Prototypes
//***************************************************************************************************************************************
void LCD_Init(void);
void LCD_CMD(uint8_t cmd);
void LCD_DATA(uint8_t data);
void SetWindows(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2);
void LCD_Clear(unsigned int c);
void H_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c);
void V_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c);
void Rect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c);
void FillRect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c);
void LCD_Print(String text, int x, int y, int fontSize, int color, int background);

void LCD_Bitmap(unsigned int x, unsigned int y, unsigned int width, unsigned int height, unsigned char bitmap[]);
//void LCD_Sprite(int x, int y, int width, int height, unsigned char bitmap[], int columns, int index, char flip, char offset);



extern uint8_t fondo[];

//-------------------- prototipo de funciones --------------------------------------------
void SetupMenu (void);
void SetupSolo (void);
void SetupDuos (void);

void P1_setup (void);

void mover_nave_ejeX (unsigned char tipo [], unsigned char ancho, unsigned char alto,  short *posicionX, short posicionY, short miniX, short maxiX, unsigned long *PrevMillis, unsigned long interval);
void mover_nave_ejeY (unsigned char tipo [], unsigned char ancho, unsigned char alto,  short posicionX, short *posicionY, short miniY, short maxiY);

void generar_disparo (short *posicionX, short *posicionY, short refX, short refY, char *active, char *hit);
void disparo_volando (unsigned char tipo [], char *active, char *hit, char ancho, char largo, short posicionX, short *posicionY, unsigned long *previo, unsigned long intervalo);

//-------------------- tiempo -------------------------------------
unsigned long currentMillis;


//***************************************************************************************************************************************
// Inicialización
//***************************************************************************************************************************************
void setup() {
  SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
  Serial.begin(115200);
  GPIOPadConfigSet(GPIO_PORTB_BASE, 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD_WPU);
  Serial.println("Inicio");
  LCD_Init();
  LCD_Clear(0x00);

  pinMode(SW1, INPUT_PULLUP);
  pinMode(SW2, INPUT_PULLUP);


}


//***************************************************************************************************************************************
// Loop Infinito
//***************************************************************************************************************************************
void loop() {
  currentMillis = millis();
  
  switch (estado_juego) { 
    case 0: //pantalla de inicio
      if (start){
        SetupMenu();
        start = 0;
      }
      P1_setup ();
      
      //hitbox menu
      if (bulletP1.ejeY <= 135){
        if (bulletP1.ejeX <= 150 - bulletP1.ancho && bulletP1.ejeX >= 7){
          estado_juego = 1;
          bulletP1.hit = 1;
          bulletP1.active = 0;
          start = 1;
        }

        if (bulletP1.ejeX <= 315 - bulletP1.ancho && bulletP1.ejeX >= 169){
          estado_juego = 2;
          bulletP1.hit = 1;
          start = 1;
        }                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      
      }
      break;

      case 1:
        if (start){
          SetupSolo ();
          start = 0;
        }
        P1_setup ();

      break;

      case 2:
        if (start){
          SetupDuos (); //cambiar a duos despues
          start = 0;
        }

    default: //pantalla de incio
      //nave para seleccionar jugadores
      //escribir que el cartucho esta corrupto
      break;
      
    
  }
}
//***************************************************************************************************************************************
// Función para menus
//***************************************************************************************************************************************
void SetupMenu (){
  //limpiar pantalla
  LCD_Clear(0x00);

  //limites de las naves
  player = {0, 304, 0, 224};

  //spawnear la nave 1
  LCD_Bitmap(P1.ejeX, P1.ejeY, P1.ancho, P1.alto, nave1);
  
  //reiniciar los scores
  P1.score = 0;
  P2.score = 0; 

  LCD_Bitmap (62, 15, 195, 30, titulo);
  LCD_Print ("Shoot to select a game mode", 40, 210, 1, 0xFFFF, 0x0);


  //Ventana un jugador
  Rect (7, 70, 143, 65, 0xFFFF);
  LCD_Print ("Solo", 50, 62, 2, 0xFFFF, 0x0); 
  LCD_Bitmap (70, 100, 15, 15, nave1);
  
  //ventana dos jugadores
  Rect (170, 70, 140, 65, 0xFFFF);
  LCD_Print ("Duos", 210, 62, 2, 0xFFFF, 0x0);
  LCD_Bitmap (215, 100, 15, 15, nave1);
  LCD_Bitmap (245, 100, 15, 15, nave2);
}

void SetupSolo () {
  //lineas
  LCD_Clear(0x0);
  H_line(0, 191, 319, 0xFFFF);

  LCD_Bitmap(5,210,15,15,nave1);
  LCD_Bitmap(22,210,15,15,nave1);


  LCD_Print("Nivel 1", 105, 200, 2, 0xFFFF, 0x0000);


  P1.ejeX = 155;
  P1.ejeY = 170;
  LCD_Bitmap (P1.ejeX, P1.ejeY, P1.ancho, P1.alto, nave1);
}

void SetupDuos (){
  LCD_Clear(0x0);
  H_line(0, 191, 319, 0xFFFF);

  LCD_Bitmap(5,210,15,15,nave1);
  LCD_Bitmap(22,210,15,15,nave1);
  LCD_Bitmap(39,210,15,15,nave1);

  LCD_Bitmap(265,210,15,15,nave2);
  LCD_Bitmap(282,210,15,15,nave2);
  LCD_Bitmap(299,210,15,15,nave2);
  
  LCD_Print("Nivel 1", 105, 200, 2, 0xFFFF, 0x0000);
}


//atento a habilitar la condicional de movimiento
//esta funcion permite mover algun objeto pre_existente a lo largo del eje X, se pued variar la velocidad a la que se mueve en vase a la funcion Millis
void mover_nave_ejeX (unsigned char tipo [], unsigned char ancho, unsigned char alto,  short *posicionX, short posicionY, short miniX, short maxiX, unsigned long *PrevMillis, unsigned long interval){
  //revisa que hayan pasado los suficiente millis para moverse, es decir la velocidad a la que se puede mover
  if (currentMillis - *PrevMillis >= interval){

    //mover a la derecha
    if (digitalRead(SW1)==1 && digitalRead(SW2)==0 && *posicionX < maxiX){
      //se incrementa la posicion del objeto en el eje X  
      (*posicionX)++;

      //cargar el bitmap nueva mente pero con el desplasamiento
      LCD_Bitmap(*posicionX, posicionY, ancho, alto, tipo);

      //se utilizan para borrar las orillas del model, es decir que no se ve su trazo
      V_line((*posicionX) - 1, posicionY, ancho ,0x0);
      V_line((*posicionX) + 1 +ancho, posicionY, ancho ,0x0);
    }
  
    //mover izquierda, mismo proceso que a la derecha
    if (digitalRead(SW1)==0 && digitalRead(SW2)==1 && *posicionX > miniX){
      (*posicionX)--;
      LCD_Bitmap(*posicionX, posicionY, ancho, alto, tipo);
      V_line((*posicionX) - 1, posicionY, ancho ,0x0);
      V_line((*posicionX) + 1 +ancho, posicionY, ancho ,0x0);
    }
    *PrevMillis = currentMillis;
  }
}

//lo mismo que en el ejeX
void mover_nave_ejeY (unsigned char tipo [], unsigned char ancho, unsigned char alto,  short posicionX, short *posicionY, short miniY, short maxiY){
  //mover a la derecha
  if (digitalRead(SW1)==1 && digitalRead(SW2)==0 && *posicionY < maxiY){
    (*posicionY)++;
    LCD_Bitmap(posicionX, *posicionY, ancho, alto, tipo);
    H_line(posicionX, (*posicionY) - 1, alto, 0x0);
    H_line(posicionX, (*posicionY) + 1 + alto, alto, 0x0);
  }

  //mover izquierda
  if (digitalRead(SW1)==0 && digitalRead(SW2)==1 && *posicionY > miniY){
    (*posicionY)--;
    LCD_Bitmap(posicionX, *posicionY, ancho, alto, tipo);
    H_line(posicionX, (*posicionY) - 1, alto, 0x0);
    H_line(posicionX, (*posicionY) + 1 + alto, alto, 0x0);
  }
}


//permite controlar un objeto que este volando a traves de la pantalla, un disparo de las naves
void disparo_volando (unsigned char tipo [], char *active, char *hit, char ancho, char alto, short posicionX, short *posicionY, unsigned long *previo, unsigned long intervalo){
  //de primero ve si el disparo esta activo y no haya golpeado algo
  if ((*active) && !(*hit)){
    //si se sale de la pantalla reiniciar el estado del disparo
    if (*posicionY < - (alto)){
      *active = 0;
      *hit = 0;
    }

    //el disparo esta dentro de la pantalla
    else {
      //controlar la velocidad del disparo
      if (currentMillis - (*previo) >= intervalo){
        //actualizar su valor para que siga funcionando piola
        *previo = currentMillis;
        //hace que l disparo suba una casilla
        (*posicionY) --;
        //actualizar su bitmap, en esta caso no es necesario ir borrando ya que tiene lineas negras arriva y abajo
        LCD_Bitmap(posicionX, *posicionY, ancho, alto, tipo);
      }
    }
  }
}

//estar atento a la condicion de disparo
void generar_disparo (short *posicionX, short *posicionY, short refX, short refY, char *active, char *hit){
  if (!digitalRead(SW1) && !digitalRead(SW2) && !(*active)){
    *posicionX = refX + 6;
    *posicionY = refY - 8;
    *active = 1;
    *hit = 0;
  }
}


//setup para la nave del jugador 1
void P1_setup () {
  mover_nave_ejeX (nave1, P1.ancho, P1.alto, &P1.ejeX, P1.ejeY, player.xMin, player.xMax, &P1.previo, P1.intervalo);
  disparo_volando (bullet, &bulletP1.active, &bulletP1.hit, bulletP1.ancho, bulletP1.alto, bulletP1.ejeX, &bulletP1.ejeY, &bulletP1.previo, bulletP1.intervalo); 
  generar_disparo (&bulletP1.ejeX, &bulletP1.ejeY, P1.ejeX, P1.ejeY, &bulletP1.active, &bulletP1.hit);
}


















//***************************************************************************************************************************************
// Función para inicializar LCD
//***************************************************************************************************************************************
void LCD_Init(void) {
  pinMode(LCD_RST, OUTPUT);
  pinMode(LCD_CS, OUTPUT);
  pinMode(LCD_RS, OUTPUT);
  pinMode(LCD_WR, OUTPUT);
  pinMode(LCD_RD, OUTPUT);
  for (uint8_t i = 0; i < 8; i++) {
    pinMode(DPINS[i], OUTPUT);
  }
  //****************************************
  // Secuencia de Inicialización
  //****************************************
  digitalWrite(LCD_CS, HIGH);
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_WR, HIGH);
  digitalWrite(LCD_RD, HIGH);
  digitalWrite(LCD_RST, HIGH);
  delay(5);
  digitalWrite(LCD_RST, LOW);
  delay(20);
  digitalWrite(LCD_RST, HIGH);
  delay(150);
  digitalWrite(LCD_CS, LOW);
  //****************************************
  LCD_CMD(0xE9);  // SETPANELRELATED
  LCD_DATA(0x20);
  //****************************************
  LCD_CMD(0x11); // Exit Sleep SLEEP OUT (SLPOUT)
  delay(100);
  //****************************************
  LCD_CMD(0xD1);    // (SETVCOM)
  LCD_DATA(0x00);
  LCD_DATA(0x71);
  LCD_DATA(0x19);
  //****************************************
  LCD_CMD(0xD0);   // (SETPOWER)
  LCD_DATA(0x07);
  LCD_DATA(0x01);
  LCD_DATA(0x08);
  //****************************************
  LCD_CMD(0x36);  // (MEMORYACCESS)
  LCD_DATA(0x40 | 0x80 | 0x20 | 0x08); // LCD_DATA(0x19);
  //****************************************
  LCD_CMD(0x3A); // Set_pixel_format (PIXELFORMAT)
  LCD_DATA(0x05); // color setings, 05h - 16bit pixel, 11h - 3bit pixel
  //****************************************
  LCD_CMD(0xC1);    // (POWERCONTROL2)
  LCD_DATA(0x10);
  LCD_DATA(0x10);
  LCD_DATA(0x02);
  LCD_DATA(0x02);
  //****************************************
  LCD_CMD(0xC0); // Set Default Gamma (POWERCONTROL1)
  LCD_DATA(0x00);
  LCD_DATA(0x35);
  LCD_DATA(0x00);
  LCD_DATA(0x00);
  LCD_DATA(0x01);
  LCD_DATA(0x02);
  //****************************************
  LCD_CMD(0xC5); // Set Frame Rate (VCOMCONTROL1)
  LCD_DATA(0x04); // 72Hz
  //****************************************
  LCD_CMD(0xD2); // Power Settings  (SETPWRNORMAL)
  LCD_DATA(0x01);
  LCD_DATA(0x44);
  //****************************************
  LCD_CMD(0xC8); //Set Gamma  (GAMMASET)
  LCD_DATA(0x04);
  LCD_DATA(0x67);
  LCD_DATA(0x35);
  LCD_DATA(0x04);
  LCD_DATA(0x08);
  LCD_DATA(0x06);
  LCD_DATA(0x24);
  LCD_DATA(0x01);
  LCD_DATA(0x37);
  LCD_DATA(0x40);
  LCD_DATA(0x03);
  LCD_DATA(0x10);
  LCD_DATA(0x08);
  LCD_DATA(0x80);
  LCD_DATA(0x00);
  //****************************************
  LCD_CMD(0x2A); // Set_column_address 320px (CASET)
  LCD_DATA(0x00);
  LCD_DATA(0x00);
  LCD_DATA(0x01);
  LCD_DATA(0x3F);
  //****************************************
  LCD_CMD(0x2B); // Set_page_address 480px (PASET)
  LCD_DATA(0x00);
  LCD_DATA(0x00);
  LCD_DATA(0x01);
  LCD_DATA(0xE0);
  //  LCD_DATA(0x8F);
  LCD_CMD(0x29); //display on
  LCD_CMD(0x2C); //display on

  LCD_CMD(ILI9341_INVOFF); //Invert Off
  delay(120);
  LCD_CMD(ILI9341_SLPOUT);    //Exit Sleep
  delay(120);
  LCD_CMD(ILI9341_DISPON);    //Display on
  digitalWrite(LCD_CS, HIGH);
}
//***************************************************************************************************************************************
// Función para enviar comandos a la LCD - parámetro (comando)
//***************************************************************************************************************************************
void LCD_CMD(uint8_t cmd) {
  digitalWrite(LCD_RS, LOW);
  digitalWrite(LCD_WR, LOW);
  GPIO_PORTB_DATA_R = cmd;
  digitalWrite(LCD_WR, HIGH);
}
//***************************************************************************************************************************************
// Función para enviar datos a la LCD - parámetro (dato)
//***************************************************************************************************************************************
void LCD_DATA(uint8_t data) {
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_WR, LOW);
  GPIO_PORTB_DATA_R = data;
  digitalWrite(LCD_WR, HIGH);
}
//***************************************************************************************************************************************
// Función para definir rango de direcciones de memoria con las cuales se trabajara (se define una ventana)
//***************************************************************************************************************************************
void SetWindows(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2) {
  LCD_CMD(0x2a); // Set_column_address 4 parameters
  LCD_DATA(x1 >> 8);
  LCD_DATA(x1);
  LCD_DATA(x2 >> 8);
  LCD_DATA(x2);
  LCD_CMD(0x2b); // Set_page_address 4 parameters
  LCD_DATA(y1 >> 8);
  LCD_DATA(y1);
  LCD_DATA(y2 >> 8);
  LCD_DATA(y2);
  LCD_CMD(0x2c); // Write_memory_start
}
//***************************************************************************************************************************************
// Función para borrar la pantalla - parámetros (color)
//***************************************************************************************************************************************
void LCD_Clear(unsigned int c) {
  unsigned int x, y;
  LCD_CMD(0x02c); // write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);
  SetWindows(0, 0, 319, 239); // 479, 319);
  for (x = 0; x < 320; x++)
    for (y = 0; y < 240; y++) {
      LCD_DATA(c >> 8);
      LCD_DATA(c);
    }
  digitalWrite(LCD_CS, HIGH);
}
//***************************************************************************************************************************************
// Función para dibujar una línea horizontal - parámetros ( coordenada x, cordenada y, longitud, color)
//***************************************************************************************************************************************
void H_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c) {
  unsigned int i, j;
  LCD_CMD(0x02c); //write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);
  l = l + x;
  SetWindows(x, y, l, y);
  j = l;// * 2;
  for (i = 0; i < l; i++) {
    LCD_DATA(c >> 8);
    LCD_DATA(c);
  }
  digitalWrite(LCD_CS, HIGH);
}
//***************************************************************************************************************************************
// Función para dibujar una línea vertical - parámetros ( coordenada x, cordenada y, longitud, color)
//***************************************************************************************************************************************
void V_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c) {
  unsigned int i, j;
  LCD_CMD(0x02c); //write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);
  l = l + y;
  SetWindows(x, y, x, l);
  j = l; //* 2;
  for (i = 1; i <= j; i++) {
    LCD_DATA(c >> 8);
    LCD_DATA(c);
  }
  digitalWrite(LCD_CS, HIGH);
}


//***************************************************************************************************************************************
// Función para dibujar un rectángulo - parámetros ( coordenada x, cordenada y, ancho, alto, color)
//***************************************************************************************************************************************
void Rect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c) {
  H_line(x  , y  , w, c);
  H_line(x  , y + h, w, c);
  V_line(x  , y  , h, c);
  V_line(x + w, y  , h, c);
}
//***************************************************************************************************************************************
// Función para dibujar un rectángulo relleno - parámetros ( coordenada x, cordenada y, ancho, alto, color)
//***************************************************************************************************************************************

void FillRect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c) {
  LCD_CMD(0x02c); // write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);

  unsigned int x2, y2;
  x2 = x + w;
  y2 = y + h;
  SetWindows(x, y, x2 - 1, y2 - 1);
  unsigned int k = w * h * 2 - 1;
  unsigned int i, j;
  for (int i = 0; i < w; i++) {
    for (int j = 0; j < h; j++) {
      LCD_DATA(c >> 8);
      LCD_DATA(c);

      //LCD_DATA(bitmap[k]);
      k = k - 2;
    }
  }
  digitalWrite(LCD_CS, HIGH);
}
//***************************************************************************************************************************************
// Función para dibujar texto - parámetros ( texto, coordenada x, cordenada y, color, background)
//***************************************************************************************************************************************
void LCD_Print(String text, int x, int y, int fontSize, int color, int background) {
  int fontXSize ;
  int fontYSize ;

  if (fontSize == 1) {
    fontXSize = fontXSizeSmal ;
    fontYSize = fontYSizeSmal ;
  }
  if (fontSize == 2) {
    fontXSize = fontXSizeBig ;
    fontYSize = fontYSizeBig ;
  }

  char charInput ;
  int cLength = text.length();
  Serial.println(cLength, DEC);
  int charDec ;
  int c ;
  int charHex ;
  char char_array[cLength + 1];
  text.toCharArray(char_array, cLength + 1) ;
  for (int i = 0; i < cLength ; i++) {
    charInput = char_array[i];
    Serial.println(char_array[i]);
    charDec = int(charInput);
    digitalWrite(LCD_CS, LOW);
    SetWindows(x + (i * fontXSize), y, x + (i * fontXSize) + fontXSize - 1, y + fontYSize );
    long charHex1 ;
    for ( int n = 0 ; n < fontYSize ; n++ ) {
      if (fontSize == 1) {
        charHex1 = pgm_read_word_near(smallFont + ((charDec - 32) * fontYSize) + n);
      }
      if (fontSize == 2) {
        charHex1 = pgm_read_word_near(bigFont + ((charDec - 32) * fontYSize) + n);
      }
      for (int t = 1; t < fontXSize + 1 ; t++) {
        if (( charHex1 & (1 << (fontXSize - t))) > 0 ) {
          c = color ;
        } else {
          c = background ;
        }
        LCD_DATA(c >> 8);
        LCD_DATA(c);
      }
    }
    digitalWrite(LCD_CS, HIGH);
  }
}
//***************************************************************************************************************************************
// Función para dibujar una imagen a partir de un arreglo de colores (Bitmap) Formato (Color 16bit R 5bits G 6bits B 5bits)
//***************************************************************************************************************************************
void LCD_Bitmap(unsigned int x, unsigned int y, unsigned int width, unsigned int height, unsigned char bitmap[]) {
  LCD_CMD(0x02c); // write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);

  unsigned int x2, y2;
  x2 = x + width;
  y2 = y + height;
  SetWindows(x, y, x2 - 1, y2 - 1);
  unsigned int k = 0;
  unsigned int i, j;

  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      LCD_DATA(bitmap[k]);
      LCD_DATA(bitmap[k + 1]);
      //LCD_DATA(bitmap[k]);
      k = k + 2;
    }
  }
  digitalWrite(LCD_CS, HIGH);
}
//***************************************************************************************************************************************
// Función para dibujar una imagen sprite - los parámetros columns = número de imagenes en el sprite, index = cual desplegar, flip = darle vuelta
//***************************************************************************************************************************************
