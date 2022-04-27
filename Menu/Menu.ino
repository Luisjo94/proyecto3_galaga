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

<<<<<<< Updated upstream
=======
#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3

//***************************************************************************************************************************************
// Variables
//***************************************************************************************************************************************
extern const unsigned char bulletE[], bullet[];
extern const unsigned char explosion_bad[];
extern const unsigned char enemy1[], nave1[], nave2[];
extern const unsigned char titulo[], gameover[];
>>>>>>> Stashed changes

//***************************************************************************************************************************************
// Variables
//***************************************************************************************************************************************
//------------------- Estado del juego ----------------------
char estado_juego = 0; //al reainiciar es el estado default, pantalla de inicio
char start = 0;
// 1, solo
// 2, duos
// 3, endgame
//  3.1 tiene highscore
//  3.2 no tiene highscore

//------------------- Banderas ----------------------
//char GameOver;  //Bandera: alguno de ambos jugadores perdió todas las vidas
//char act_vidas;
char a = 1;         //regresar al menu principal


//------------------- Posicion de naves ---------------------
struct ship {
  short ejeX;
  short ejeY;
  const char  alto;
  const char  ancho;
  short vidas;
  int score;
};
//Posicion inicial de naves jugadores
ship P1 = {0, 170, 15, 15, 3, 0};
ship P2 = {303, 170, 15, 15, 3, 0};

//Posicion enemigo
ship bad1 = {100, 100};

//------------------- disparos ---------------------------
struct object {
  short posX;
  short posY;
  char  largo;
  char  alto;
  char  active;
  char  hit;
  unsigned short previo;
  unsigned short intervalo;
};
                                               
//balas de ambos jugadores
<<<<<<< Updated upstream
object bulletP1;
object bulletP2;
=======
struct object bulletP1 = {0,0, 8, 3, 0, 0, 0, 2};
struct object bulletP2 = bulletP1;
struct object bulletE1 = {0,0, 8, 3, 0, 0, 0, 2};
>>>>>>> Stashed changes


//------------------- limites ---------------------------
struct Bound {
  short widthMin;
  short widthMax;
  short heightMin;
  short heightMax;
};
Bound player = {0, 304, 0, 224};

struct Menu {
  short startX;
  short startY;
  short width;
  short height;
};

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

void Menu(void);
void SetupSolo (void);
void SetupDuos (void);
void ScoreSoloMode(int scorej1);

extern uint8_t fondo[];



//***************************************************************************************************************************************
// Inicialización
//***************************************************************************************************************************************
void setup() {
  SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
  Serial.begin(115200);
  GPIOPadConfigSet(GPIO_PORTB_BASE, 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD_WPU);
  Serial.println("Inicio");
  LCD_Init();
  

  pinMode(SW1, INPUT_PULLUP);
  pinMode(SW2, INPUT_PULLUP);
  
  bulletP1.intervalo = 1;
  bulletP1.largo = 3;
  bulletP1.alto = 8;
  bulletP2.intervalo = 1;
  bulletP2.largo = 3;
  bulletP2.alto = 8;


}


//***************************************************************************************************************************************
// Loop Infinito
//***************************************************************************************************************************************
void loop() {
  unsigned long currentMillis = millis();
  
  switch (estado_juego) { 

    //****************************** TITLE SCREEN ******************************
    case 0: //pantalla de inicio
      if (a)
      {
        LCD_Clear(0x00);
        Menu();
        //Menu inicial
        player = {0, 303, 145, 170};
        LCD_Bitmap(P1.ejeX, P1.ejeY, P1.ancho, P1.alto, nave1);
        P1.score = 0;
        P2.score = 0; 
        a = 0;
      }
    
      //nave para seleccionar jugadores
      if (digitalRead(SW1)==1 && digitalRead(SW2)==0 && P1.ejeX < player.widthMax){
        P1.ejeX++;  //ir a la derecha
        LCD_Bitmap(P1.ejeX, P1.ejeY, P1.ancho, P1.alto, nave1);
        V_line( P1.ejeX-1, P1.ejeY, P1.ancho, 0x0);
        V_line( P1.ejeX+16, P1.ejeY, P1.ancho, 0x0);        
      }
      //moverse izquierda
      if (digitalRead(SW1)==0 && digitalRead(SW2)==1 && P1.ejeX > player.widthMin){
        P1.ejeX--;  //ir a la izquierda
        LCD_Bitmap(P1.ejeX, P1.ejeY, P1.ancho, P1.alto, nave1);
        V_line( P1.ejeX-1, P1.ejeY, P1.ancho, 0x0);
        V_line( P1.ejeX+16, P1.ejeY, P1.ancho, 0x0);
      }

      //la bala esta activa y no a golpeado nada
      if (bulletP1.active && !bulletP1.hit){
        //se salio de la pantalla
        if (bulletP1.posY < -8){
          FillRect (bulletP1.posX, bulletP1.posY, bulletP1.largo, bulletP1.alto, 0x0000);
          bulletP1.active = 0;
          bulletP1.hit = 0;
        }
        //esta dentro de la pantalla pero no ha golpeado nada
        else {
          if (currentMillis - bulletP1.previo > bulletP1.intervalo){
            bulletP1.previo = currentMillis;
            bulletP1.posY--;
            LCD_Bitmap(bulletP1.posX, bulletP1.posY, bulletP1.largo, bulletP1.alto, bullet);  
          }
        }
      }
      
      //disparar 
      if (digitalRead(SW1)==0 && digitalRead(SW2)==0 && bulletP1.active == 0){// condicion de disparo 
        bulletP1.posX = P1.ejeX+6;
        bulletP1.posY = P1.ejeY-8;
        bulletP1.active = 1;
        bulletP1.hit = 0;
      }
      //hitbox menu

      if (bulletP1.posY == 135){
        if (bulletP1.posX <= 150 - bulletP1.largo && bulletP1.posX >= 10){
          estado_juego = 1;
          bulletP1.hit = 1;
          bulletP1.active = 0;
          start = 1;
        }

        if (bulletP1.posX <= 310 - bulletP1.largo && bulletP1.posX >= 170){
          estado_juego = 2;
          bulletP1.hit = 1;
          bulletP1.active = 0;
          start = 1;
        }
      }
      break;

    // ****************************** SOLO MODE ******************************
    case 1:
      // ---------- map setup ----------
      if (start){
        P1.ejeX = 145;
        SetupSolo ();
        start = 0;
      }
      // ---------- vidas ----------
      vidasJ1(P1.vidas);
      P1.vidas = 2;

<<<<<<< Updated upstream
      // ---------- puntos ----------
      ScoreSoloMode(P1.score);
      
      // ---------- movimiento y disparos ----------
      //nave para seleccionar jugadores
      if (digitalRead(SW1)==1 && digitalRead(SW2)==0 && P1.ejeX < player.widthMax){
        P1.ejeX++;  //ir a la derecha
        LCD_Bitmap(P1.ejeX, P1.ejeY, P1.ancho, P1.alto, nave1);
        V_line( P1.ejeX-1, P1.ejeY, P1.ancho, 0x0);
        V_line( P1.ejeX+16, P1.ejeY, P1.ancho, 0x0);        
      }
      //moverse izquierda
      if (digitalRead(SW1)==0 && digitalRead(SW2)==1 && P1.ejeX > player.widthMin){
        P1.ejeX--;  //ir a la izquierda
        LCD_Bitmap(P1.ejeX, P1.ejeY, P1.ancho, P1.alto, nave1);
        V_line( P1.ejeX-1, P1.ejeY, P1.ancho, 0x0);
        V_line( P1.ejeX+16, P1.ejeY, P1.ancho, 0x0);
      }

      //la bala esta activa y no a golpeado nada
      if (bulletP1.active && !bulletP1.hit){
        //se salio de la pantalla
        if (bulletP1.posY < -8){
          FillRect (bulletP1.posX, bulletP1.posY, bulletP1.largo, bulletP1.alto, 0x0000);
          bulletP1.active = 0;
          bulletP1.hit = 0;
        }
        //esta dentro de la pantalla pero no ha golpeado nada
        else {
          if (currentMillis - bulletP1.previo > bulletP1.intervalo){
            bulletP1.previo = currentMillis;
            bulletP1.posY--;
            LCD_Bitmap(bulletP1.posX, bulletP1.posY, bulletP1.largo, bulletP1.alto, bullet);  
          }
        }
      }
      
      //disparar 
      if (digitalRead(SW1)==0 && digitalRead(SW2)==0 && bulletP1.active == 0){// condicion de disparo 
        bulletP1.posX = P1.ejeX+6;
        bulletP1.posY = P1.ejeY-8;
        bulletP1.active = 1;
        bulletP1.hit = 0;
        P1.score++;
      }
      
      
=======
        // ---------- puntos ----------
        ScoreSoloMode(P1.score);
        nivelSolo(P1.score);
        
        // ---------- movimiento y disparos nave 1----------
        P1_setup ();

<<<<<<< Updated upstream
        // ---------- movimiento y disparos enemy1----------
        if (bad1.flag == 0){
          move_NPC(enemy1, bad1.ancho, bad1.alto, bad1.ejeY, &bad1.ejeX, enemy.xMin, enemy.xMax, &bad1.previo, bad1.intervalo, RIGHT, &bad1.flag); 
        }
//        if (bad2.flag == 0){
//          move_NPC(enemy1, bad2.ancho, bad2.alto, bad2.ejeY, &bad2.ejeX, enemy.xMin, enemy.xMax-30, &bad2.previo, bad2.intervalo, RIGHT, &bad2.flag);   
=======
        setup_P1 ();
//        
//        // ---------- movimiento y disparos nave 1----------
//        P1_setup ();
//
//        // ---------- movimiento y disparos enemy1----------
//        if (bad1.flag == 0){
//          move_NPC(enemy1, bad1.ancho, bad1.alto, bad1.ejeY, &bad1.ejeX, enemy.xMin, enemy.xMax, &bad1.previo, bad1.intervalo, RIGHT, &bad1.flag); 
>>>>>>> Stashed changes
//        }
//        if (bad3.flag == 0){
//          move_NPC(enemy1, bad3.ancho, bad3.alto, bad3.ejeY, &bad3.ejeX, enemy.xMin, enemy.xMax-60, &bad3.previo, bad3.intervalo, RIGHT, &bad3.flag);   
//        }
<<<<<<< Updated upstream
//        if (bad4.flag == 0){
//          move_NPC(enemy1, bad4.ancho, bad4.alto, bad4.ejeY, &bad4.ejeX, enemy.xMin, enemy.xMax-90, &bad4.previo, bad4.intervalo, RIGHT, &bad4.flag);   
//        }
//        if (bad5.flag == 0){
//          move_NPC(enemy1, bad5.ancho, bad5.alto, bad5.ejeY, &bad5.ejeX, enemy.xMin, enemy.xMax-120, &bad5.previo, bad5.intervalo, RIGHT, &bad5.flag);   
//        }
//        if (bad6.flag == 0){
//          move_NPC(enemy1, bad6.ancho, bad6.alto, bad6.ejeY, &bad6.ejeX, enemy.xMin, enemy.xMax-150, &bad6.previo, bad6.intervalo, RIGHT, &bad6.flag);   
//        }
=======
//      }
//
      break;

      // ****************************** DUOS MODE ******************************
      case 2:
      // ---------- map setup ----------
        if (start){
          SetupDuos();
          start = 0;
        }
      setup_P1();
      setup_P2();
//        if (start){
//          P1.ejeX = 0;
//          P2.ejeX = 303;
//          SetupDuos (); //cambiar a duos despues
//          start = 0; 
//        }
//
//        // ---------- vidas ----------
//        vidasJ1(P1.vidas);
//        vidasJ2(P2.vidas);
//
//        // ---------- puntos ----------
//        ScoreDuosMode(P1.score, P2.score);
//
//        // ---------- movimiento y disparos ----------
//        duos_setup();
        break;
>>>>>>> Stashed changes

//void generar_disparo_NPC (short *posicionX, short *posicionY, short refX, short refY, char *active, char *hit,  unsigned long *previo, unsigned long intervalo)
//        if (bad1.ejeX == 100 || bad1.ejeX == 200){
//          generar_disparo_NPC(&bad1.ejeX+6, &bad1.ejeY, bad1.ejeX, bad1.ejeY, );
//        }
>>>>>>> Stashed changes

      
      break;
    // ****************************** DUOS MODE ******************************
    case 2:
        // ---------- map setup ----------
        if (start){
          P1.ejeX = 0;
          P2.ejeX = 303;
          SetupDuos (); //cambiar a duos despues
          start = 0; 
        }
        // ---------- vidas ----------
        vidasJ1(P1.vidas);
        vidasJ2(P2.vidas);

        // ---------- puntos ----------
        ScoreDuosMode(P1.score, P2.score);

      // -------------------- movimiento y disparos NAVE 1 --------------------
      //nave para seleccionar jugadores
      if (digitalRead(SW1)==1 && digitalRead(SW2)==0 && P1.ejeX+16 < P2.ejeX){
        P1.ejeX++;  //ir a la derecha
        LCD_Bitmap(P1.ejeX, P1.ejeY, P1.ancho, P1.alto, nave1);
        V_line( P1.ejeX-1, P1.ejeY, P1.ancho, 0x0);
        V_line( P1.ejeX+16, P1.ejeY, P1.ancho, 0x0);        
      }
      //moverse izquierda
      if (digitalRead(SW1)==0 && digitalRead(SW2)==1 && P1.ejeX > player.widthMin){
        P1.ejeX--;  //ir a la izquierda
        LCD_Bitmap(P1.ejeX, P1.ejeY, P1.ancho, P1.alto, nave1);
        V_line( P1.ejeX-1, P1.ejeY, P1.ancho, 0x0);
        V_line( P1.ejeX+16, P1.ejeY, P1.ancho, 0x0);
      }

      //la bala esta activa y no a golpeado nada
      if (bulletP1.active && !bulletP1.hit){
        //se salio de la pantalla
        if (bulletP1.posY < -8){
          FillRect (bulletP1.posX, bulletP1.posY, bulletP1.largo, bulletP1.alto, 0x0000);
          bulletP1.active = 0;
          bulletP1.hit = 0;
        }
        //esta dentro de la pantalla pero no ha golpeado nada
        else {
          if (currentMillis - bulletP1.previo > bulletP1.intervalo){
            bulletP1.previo = currentMillis;
            bulletP1.posY--;
            LCD_Bitmap(bulletP1.posX, bulletP1.posY, bulletP1.largo, bulletP1.alto, bullet);  
          }
        }
      }
      
      //disparar 
      if (digitalRead(SW1)==0 && digitalRead(SW2)==0 && bulletP1.active == 0){// condicion de disparo 
        bulletP1.posX = P1.ejeX+6;
        bulletP1.posY = P1.ejeY-8;
        bulletP1.active = 1;
        bulletP1.hit = 0;
        P1.score++;
      }


      // -------------------- movimiento y disparos NAVE 2 --------------------
      //nave para seleccionar jugadores
      if (digitalRead(SW1)==1 && digitalRead(SW2)==0 && P2.ejeX < player.widthMax){
        P2.ejeX++;  //ir a la derecha
        LCD_Bitmap(P2.ejeX, P2.ejeY, P2.ancho, P2.alto, nave2);
        V_line( P2.ejeX-1, P2.ejeY, P2.ancho, 0x0);
        V_line( P2.ejeX+16, P2.ejeY, P2.ancho, 0x0);        
      }
      //moverse izquierda
      if (digitalRead(SW1)==0 && digitalRead(SW2)==1 && P2.ejeX > P1.ejeX+16){
        P2.ejeX--;  //ir a la izquierda
        LCD_Bitmap(P2.ejeX, P2.ejeY, P2.ancho, P2.alto, nave2);
        V_line( P2.ejeX-1, P2.ejeY, P2.ancho, 0x0);
        V_line( P2.ejeX+16, P2.ejeY, P2.ancho, 0x0);
      }

      //la bala esta activa y no a golpeado nada
      if (bulletP2.active && !bulletP2.hit){
        //se salio de la pantalla
        if (bulletP2.posY < -8){
          FillRect (bulletP2.posX, bulletP2.posY, bulletP2.largo, bulletP2.alto, 0x0000);
          bulletP2.active = 0;
          bulletP2.hit = 0;
        }
        //esta dentro de la pantalla pero no ha golpeado nada
        else {
          if (currentMillis - bulletP2.previo > bulletP2.intervalo){
            bulletP2.previo = currentMillis;
            bulletP2.posY--;
            LCD_Bitmap(bulletP2.posX, bulletP2.posY, bulletP2.largo, bulletP2.alto, bullet);  
          }
        }
      }
      
      //disparar 
      if (digitalRead(SW1)==0 && digitalRead(SW2)==0 && bulletP2.active == 0){// condicion de disparo 
        bulletP2.posX = P2.ejeX+6;
        bulletP2.posY = P2.ejeY-8;
        bulletP2.active = 1;
        bulletP2.hit = 0;
        P2.score++;
      }



        
      break;

    // ****************************** GAME OVER ******************************
    case 3:
      if (a == 0)
      {
        GameOver();
        a = 1;
      }
      
      break;
      
    default: //pantalla de incio
      //nave para seleccionar jugadores
      //escribir que el cartucho esta corrupto
      break;
      
    
  }
}
//***************************************************************************************************************************************
// Función para menus
//***************************************************************************************************************************************
void Menu (){
  //320x240
  //titulo del juego
  LCD_Bitmap (62, 15, 195, 30, titulo);
  LCD_Print ("Shoot to select a game mode", 40, 210, 1, 0xFFFF, 0x0);

  //Ventana un jugador
  Rect (10, 70, 140, 65, 0xFFFF);
  LCD_Print ("Solo", 50, 62, 2, 0xFFFF, 0x0); 
  LCD_Bitmap (70, 100, 15, 15, nave1);
  
  //ventana dos jugadores
  Rect (168, 70, 140, 65, 0xFFFF);
  LCD_Print ("Duos", 208, 62, 2, 0xFFFF, 0x0);
  LCD_Bitmap (213, 100, 15, 15, nave1);
  LCD_Bitmap (243, 100, 15, 15, nave2);
}

void SetupSolo () {
  //lineas
  LCD_Clear(0x0);
  H_line(0, 191, 319, 0xFFFF);

  
// ********** text **********
 LCD_Print("Nivel 1", 105, 200, 2, 0xFFFF, 0x0000);
 LCD_Print("Solo Mode", 5, 195, 1, 0xFFFF, 0x0000);
 LCD_Print("Score:", 260, 195, 1, 0xFFFF, 0x0000);

// ********** vidas J1 **********
 LCD_Bitmap(5,210,15,15,nave1);
 LCD_Bitmap(22,210,15,15,nave1);
 LCD_Bitmap(39,210,15,15,nave1);
// ********** nave J1 **********
 LCD_Bitmap(P1.ejeX, P1.ejeY, P1.ancho, P1.alto, nave1);
}



void SetupDuos () {
  //lineas
  LCD_Clear(0x0);
  H_line(0, 191, 319, 0xFFFF);

  
// ********** text **********
// LCD_Print("Nivel 1", 105, 200, 2, 0xFFFF, 0x0000);
 LCD_Print("Score:", 5, 195, 1, 0xFFFF, 0x0000);
 LCD_Print("Score:", 240, 195, 1, 0xFFFF, 0x0000);

// ********** vidas J1 **********
 LCD_Bitmap(5,210,15,15,nave1);
 LCD_Bitmap(22,210,15,15,nave1);
 LCD_Bitmap(39,210,15,15,nave1);
// ********** vidas J2 **********
 LCD_Bitmap(265,210,15,15,nave2);
 LCD_Bitmap(282,210,15,15,nave2);
 LCD_Bitmap(299,210,15,15,nave2);
 // ********** naves J1 y J2 **********
 LCD_Bitmap(P1.ejeX, P1.ejeY, P1.ancho, P1.alto, nave1);
 LCD_Bitmap(P2.ejeX, P2.ejeY, P2.ancho, P2.alto, nave2);
 // ********** balas **********
  bulletP1.hit = 0;
  bulletP1.active = 0;
  bulletP2.hit = 0;
  bulletP2.active = 0;
}


void GameOver(void)
{
  LCD_Clear(0x0);
<<<<<<< Updated upstream
  //LCD_Bitmap (10, 10, 293, 33, gameover);
  LCD_Print("Game Over", 10, 10, 2, 0xFFFF, 0x0);
=======
  //LCD_Print("Game Over", 10, 10, 2, 0xFFFF, 0x0);
  LCD_Bitmap(0, 0, 293, 33, gameover);
>>>>>>> Stashed changes
}


// --------------- Funciones para verificar las vidas de los jugadores ---------------
void vidasJ1(short j1)
{
  switch(j1)
  {
//  case 3:
//    LCD_Bitmap(5,210,15,15,nave1);
//    LCD_Bitmap(22,210,15,15,nave1);
//    LCD_Bitmap(39,210,15,15,nave1);
//    break;
  case 2:
    //LCD_Bitmap(5,210,15,15,nave1);
    //LCD_Bitmap(22,210,15,15,nave1);
    FillRect(39,210,15,15,0x0);
    break;
  case 1:
    //LCD_Bitmap(5,210,15,15,nave1);
    FillRect(22,210,15,15,0x0);
    FillRect(39,210,15,15,0x0);
    break;
  case 0:
    FillRect(2,210,15,15,0x0);
    FillRect(22,210,15,15,0x0);
    FillRect(39,210,15,15,0x0);
    estado_juego = 3;
    break;
  }
}

<<<<<<< Updated upstream
=======
void vidasJ1(short j1)
{
  switch(j1)
  {
  case 2:
    //LCD_Bitmap(5,210,15,15,nave1);
    //LCD_Bitmap(22,210,15,15,nave1);
    FillRect(39,210,15,15,0x0);
    break;
  case 1:
    //LCD_Bitmap(5,210,15,15,nave1);
    FillRect(22,210,15,15,0x0);
    FillRect(39,210,15,15,0x0);
    break;
  case 0:
    FillRect(2,210,15,15,0x0);
    FillRect(22,210,15,15,0x0);
    FillRect(39,210,15,15,0x0);
    estado_juego = 3;
    break;
  }
}

>>>>>>> Stashed changes
void vidasJ2(short j2)
{
  switch(j2)
  {
//    case 3:
//      LCD_Bitmap(265,210,15,15,nave2);
//      LCD_Bitmap(282,210,15,15,nave2);
//      LCD_Bitmap(299,210,15,15,nave2);
//      break;
    case 2:
      //LCD_Bitmap(265,210,15,15,nave2);
      //LCD_Bitmap(282,210,15,15,nave2);
      FillRect(299,210,15,15,0x0);
      break;
    case 1:
      //LCD_Bitmap(265,210,15,15,nave2);
      FillRect(282,210,15,15,0x0);
      FillRect(299,210,15,15,0x0);
      break;
    case 0:
      FillRect(265,210,15,15,0x0);
      FillRect(282,210,15,15,0x0);
      FillRect(299,210,15,15,0x0);
      estado_juego = 3;
      break;
  }
}



void ScoreSoloMode(int scorej1)
{
  LCD_Print(String(scorej1), 280, 210, 1, 0xFFFF, 0x0000);
}

void ScoreDuosMode(int scorej1, int scorej2)
{
  //j1
  LCD_Print(String(scorej1), 55, 195, 1, 0xFFFF, 0x0);
  //j2
  LCD_Print(String(scorej2), 290, 195, 1, 0xFFFF, 0x0);
}

<<<<<<< Updated upstream
<<<<<<< Updated upstream
=======
=======
>>>>>>> Stashed changes
void spawn_ship (const unsigned char tipo [], struct entity *sel){
  LCD_Bitmap(sel->pos.ejeX, sel->pos.ejeY, sel->dimension.ancho, sel->dimension.alto, tipo);
}
//---------------------funciones para entidades -----------------------------------------
void move_player (const unsigned char tipo [], struct entity *sel){
  if (currentMillis - sel->mils.previo >= sel->mils.interval){
    if (digitalRead(SW1)==1 && digitalRead(SW2)==0 && sel->pos.ejeX < sel->limites.maxiX){
      (sel->pos.ejeX) ++;

      LCD_Bitmap(sel->pos.ejeX, sel->pos.ejeY, sel->dimension.ancho, sel->dimension.alto, tipo);
>>>>>>> Stashed changes

<<<<<<< Updated upstream
=======
//atento a habilitar la condicional de movimiento
//esta funcion permite mover algun objeto pre_existente a lo largo del eje X, se pued variar la velocidad a la que se mueve en base a la funcion Millis
void mover_nave_ejeX (const char tipo [], unsigned char ancho, unsigned char alto,  short *posicionX, short posicionY, short miniX, short maxiX, unsigned long *PrevMillis, unsigned long interval){
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

<<<<<<< Updated upstream

//#define UP 0
//#define DOWN 1
//#define LEFT 2
//#define RIGHT 3

//pude mover un PNC a cualquier direccion
void move_NPC (const char tipo [], unsigned char ancho, unsigned char alto,  short pos_cons, short *pos_change, short mini, short maxi, unsigned long *PrevMillis, unsigned long interval, char direccion, char *flag){
=======
//pude mover un NPC a cualquier direccion
void move_NPC2 (const unsigned char tipo [], struct entity *sel, short mini, short maxi, char direccion){
<<<<<<< Updated upstream
>>>>>>> Stashed changes
=======
>>>>>>> Stashed changes
  //chequea los millis para ell moviemiento, la velociad
  if (currentMillis - *PrevMillis >= interval){
    //permite elegir la dirccion a la que se movera el enmigo
    switch (direccion){
      case UP:
        //revisa que no haya llegado al limite y la bandera este apagada
        if ((*pos_change) > mini && !(*flag)){
          (*pos_change)--;
          LCD_Bitmap(pos_cons, *pos_change, ancho, alto, tipo);
          H_line(pos_cons, (*pos_change) - 1, alto, 0x0);
          H_line(pos_cons, (*pos_change) + 1 + alto, alto, 0x0);
        }
        // en el caso de llegar al limite se enciende la bandera
        else {
          *flag = 1;
        }
        break;
  
      case DOWN:
        if ((*pos_change) < maxi && !(*flag)) {
          (*pos_change)--;
          LCD_Bitmap(pos_cons, *pos_change, ancho, alto, tipo);
          H_line(pos_cons, (*pos_change) - 1, alto, 0x0);
          H_line(pos_cons, (*pos_change) + 1 + alto, alto, 0x0);
        }
        else {
          *flag = 1;
        }
        break;

      case LEFT:
        if ((*pos_change) > mini && !(*flag)) {
          (*pos_change)--;
          LCD_Bitmap(*pos_change, pos_cons, ancho, alto, tipo);
          V_line((*pos_change) - 1, pos_cons, alto, 0x0);
          V_line((*pos_change) +1 + ancho, pos_cons, alto, 0x0);
        }
        else {
          *flag = 1;
        }
        break;
        
      case RIGHT:
        if ((*pos_change) < maxi && !(*flag)) {
          (*pos_change)++;
          LCD_Bitmap(*pos_change, pos_cons, ancho, alto, tipo);
          V_line((*pos_change) - 1, pos_cons, alto, 0x0);
          V_line((*pos_change) + 1 + ancho, pos_cons, alto, 0x0);
        }
        else {
          *flag = 1;
        }
        break;

      //en el peor de los casos la bandera se pone en 2
      default:
        *flag = 2;
        break;
    }
    *PrevMillis = currentMillis;
  }
}

<<<<<<< Updated upstream
<<<<<<< Updated upstream
//permite controlar un objeto que este volando a traves de la pantalla, un disparo de las naves
void disparo_volando (const char tipo [], char *active, char *hit, char ancho, char alto, short posicionX, short *posicionY, unsigned long *previo, unsigned long intervalo){
  //de primero ve si el disparo esta activo y no haya golpeado algo
  if ((*active) && !(*hit)){
    //si se sale de la pantalla reiniciar el estado del disparo
    if (*posicionY < - (alto)){
      *active = 0;
      *hit = 0;
=======

=======
>>>>>>> Stashed changes
void shoot_player (const unsigned char tipo[], struct entity sel_ref, struct entity *sel){
  //generar el disparo
  if (!digitalRead(SW1) && !digitalRead(SW2) && !(sel->info.active)){
    sel->pos.ejeX = (sel_ref.pos.ejeX) + 6;
    sel->pos.ejeY = (sel_ref.pos.ejeY) - (sel->dimension.alto);
    sel->info.active = 1;
    sel->info.hit = 0;
  }
}

void disparo_volando (const unsigned char tipo [], struct entity *sel){
  //disparo activo y no ha golpeado nada
  if ((sel->info.active) && !(sel -> info.hit)){

    //disparo activo y fuera de la pantalla
    if (sel->pos.ejeY < -(sel->dimension.alto)) {
      sel-> info.active = 0;
      sel-> info.hit = 1;
>>>>>>> Stashed changes
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

<<<<<<< Updated upstream
<<<<<<< Updated upstream
//estar atento a la condicion de disparo
void generar_disparo (short *posicionX, short *posicionY, short refX, short refY, char *active, char *hit){
  if (!digitalRead(SW1) && !digitalRead(SW2) && !(*active)){
    *posicionX = refX + 6;
    *posicionY = refY - 8;
    *active = 1;
    *hit = 0;
=======
=======
>>>>>>> Stashed changes
void shoot_NPC (const unsigned char tipo[], struct entity sel_ref, struct entity *sel){
  //generar el disparo con frecuencia
  if ((currentMillis - (sel->mils.frecuencia) >= sel->mils.interval) && !(sel->info.active) && sel_ref.info.active){
    sel-> mils.frecuencia = currentMillis;
    sel-> pos.ejeX = sel_ref.pos.ejeX + 6;
    sel-> pos.ejeY = sel_ref.pos.ejeY - (sel ->dimension.alto);
    sel-> info.active = 1;
    sel-> info.hit = 0;
>>>>>>> Stashed changes
  }
}

void generar_disparo_NPC (short *posicionX, short *posicionY, short refX, short refY, char *active, char *hit,  unsigned long *previo, unsigned long intervalo) {
  if (currentMillis - (*previo) >= intervalo){
    *previo = currentMillis;
    *posicionX = refX + 6;
    *posicionY = refY - 8;
    *active = 1;
    *hit = 0;
  }
}

<<<<<<< Updated upstream
<<<<<<< Updated upstream
=======
void hitboxNPC(struct entity *NPC, struct entity *bala, struct entity *ship){
  if (NPC->info.active && bala->info.active){
    if (bala->pos.ejeY == ((NPC->pos.ejeY) - (NPC->dimension.alto))){
      if ((bala->pos.ejeX) + (bala->dimension.ancho) <= ((NPC->pos.ejeX) + (NPC->dimension.ancho)) && (bala->pos.ejeX >= NPC->pos.ejeX)){
        NPC->info.flag = 1;
        NPC->info.active = 0;
        bala->info.hit=1;
        ship->player.score = (ship->player.score)+5;
>>>>>>> Stashed changes

//setup para la nave del jugador 1
void P1_setup () {
  mover_nave_ejeX (nave1, P1.ancho, P1.alto, &P1.ejeX, P1.ejeY, player.xMin, player.xMax, &P1.previo, P1.intervalo);
  disparo_volando (bullet, &bulletP1.active, &bulletP1.hit, bulletP1.ancho, bulletP1.alto, bulletP1.ejeX, &bulletP1.ejeY, &bulletP1.previo, bulletP1.intervalo); 
  generar_disparo (&bulletP1.ejeX, &bulletP1.ejeY, P1.ejeX, P1.ejeY, &bulletP1.active, &bulletP1.hit);
}

//setup para la nave del jugador 2
void duos_setup () {
  //nave1
  mover_nave_ejeX (nave1, P1.ancho, P1.alto, &P1.ejeX, P1.ejeY, player.xMin, P2.ejeX-16, &P1.previo, P1.intervalo);
  disparo_volando (bullet, &bulletP1.active, &bulletP1.hit, bulletP1.ancho, bulletP1.alto, bulletP1.ejeX, &bulletP1.ejeY, &bulletP1.previo, bulletP1.intervalo); 
  generar_disparo (&bulletP1.ejeX, &bulletP1.ejeY, P1.ejeX, P1.ejeY, &bulletP1.active, &bulletP1.hit);
  //nave2
  mover_nave_ejeX (nave2, P2.ancho, P2.alto, &P2.ejeX, P2.ejeY, P1.ejeX+16, player.xMax, &P2.previo, P2.intervalo);
  disparo_volando (bullet, &bulletP2.active, &bulletP2.hit, bulletP2.ancho, bulletP2.alto, bulletP2.ejeX, &bulletP2.ejeY, &bulletP2.previo, bulletP2.intervalo); 
  generar_disparo (&bulletP2.ejeX, &bulletP2.ejeY, P2.ejeX, P2.ejeY, &bulletP2.active, &bulletP2.hit);
}

<<<<<<< Updated upstream
void boom (const char tipo [], short ejeX, short ejeY){
=======
void boom (const unsigned char tipo[], struct entity sel){
>>>>>>> Stashed changes
=======
void boom (const unsigned char tipo[], struct entity sel){
>>>>>>> Stashed changes
  for (char i = 0; i < 5; i++){
    LCD_Sprite(ejeX-12, ejeY-10, 32, 32, tipo, 5, i, 0, 0);
    delay(20);
  }
}

<<<<<<< Updated upstream


void hitbox1(void){
  //hitbox menu
      if (bulletP1.ejeY == bad1.ejeY+15){
        if (bulletP1.ejeX <= bad1.ejeX + bad1.ancho && bulletP1.ejeX >= bad1.ejeX && bulletP1.active){
          P1.score = P1.score+5;
          bulletP1.hit = 1;
          bulletP1.active = 0;
          bad1.flag = 1;
          boom (explosion_bad, bad1.ejeX, bad1.ejeY);
          bad1.ejeX = -15;
        }
      }

      //hitbox menu 2
      if (bulletP1.ejeY == bad2.ejeY+15){
        if (bulletP1.ejeX <= bad2.ejeX + bad2.ancho && bulletP1.ejeX >= bad2.ejeX && bulletP1.active){
          P1.score = P1.score+5;
          bulletP1.hit = 1;
          bulletP1.active = 0;
          bad2.flag = 1;
          boom (explosion_bad, bad2.ejeX, bad2.ejeY);
          bad2.ejeX = -15;
        }
      }

      //hitbox menu 3
      if (bulletP1.ejeY == bad3.ejeY+15){
        if (bulletP1.ejeX <= bad3.ejeX + bad3.ancho && bulletP1.ejeX >= bad3.ejeX && bulletP1.active){
          P1.score = P1.score+5;
          bulletP1.hit = 1;
          bulletP1.active = 0;
          bad3.flag = 1;
          boom (explosion_bad, bad3.ejeX, bad3.ejeY);
          bad3.ejeX = -15;
        }
      }

      //hitbox menu 4
      if (bulletP1.ejeY == bad4.ejeY+15){
        if (bulletP1.ejeX <= bad4.ejeX + bad4.ancho && bulletP1.ejeX >= bad4.ejeX && bulletP1.active){
          P1.score = P1.score+5;
          bulletP1.hit = 1;
          bulletP1.active = 0;
          bad4.flag = 1;
          boom (explosion_bad, bad4.ejeX, bad4.ejeY);
          bad4.ejeX = -15;
        }
      }

      //hitbox menu 5
      if (bulletP1.ejeY == bad5.ejeY+15){
        if (bulletP1.ejeX <= bad5.ejeX + bad5.ancho && bulletP1.ejeX >= bad5.ejeX && bulletP1.active){
          P1.score = P1.score+5;
          bulletP1.hit = 1;
          bulletP1.active = 0;
          bad5.flag = 1;
          boom (explosion_bad, bad5.ejeX, bad5.ejeY);
          bad5.ejeX = -15;
        }
      }

      //hitbox menu 6
      if (bulletP1.ejeY == bad6.ejeY+15){
        if (bulletP1.ejeX <= bad6.ejeX + bad6.ancho && bulletP1.ejeX >= bad6.ejeX && bulletP1.active){
          P1.score = P1.score+5;
          bulletP1.hit = 1;
          bulletP1.active = 0;
          bad6.flag = 1;
          boom (explosion_bad, bad6.ejeX, bad6.ejeY);
          bad6.ejeX = -15;
        }
      }
}


void nivelSolo (int s){
  if (s >= 30)
  {
    LCD_Print("2", 200, 200, 2, 0xFFFF, 0x0);
  }
}






=======
void setup_P1 () {
  move_player (nave1, &shipP1);
  shoot_player (bullet, shipP1, &bulletP1);
  disparo_volando (bullet, &bulletP1);
//  // ---------- vidas ----------
//  vidasJ1(.vidas);
//  // ---------- puntos ----------
//  ScoreSoloMode(player.score);
}


void setup_P2() {
  move_player (nave2, &shipP2);
  shoot_player (bullet, shipP2, &bulletP2);
  disparo_volando (bullet, &bulletP2);
}
>>>>>>> Stashed changes


>>>>>>> Stashed changes


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
<<<<<<< Updated upstream
<<<<<<< Updated upstream
void LCD_Bitmap(unsigned int x, unsigned int y, unsigned int width, unsigned int height, const char bitmap[]) {
=======
void LCD_Bitmap(unsigned int x, unsigned int y, unsigned int width, unsigned int height, const unsigned char bitmap[]) {
>>>>>>> Stashed changes
=======
void LCD_Bitmap(unsigned int x, unsigned int y, unsigned int width, unsigned int height, const unsigned char bitmap[]) {
>>>>>>> Stashed changes
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
<<<<<<< Updated upstream
<<<<<<< Updated upstream
<<<<<<< Updated upstream
=======
void LCD_Sprite(int x, int y, int width, int height, const char bitmap[], int columns, int index, char flip, char offset) {
=======
void LCD_Sprite(int x, int y, int width, int height, const unsigned char bitmap[], int columns, int index, char flip, char offset) {
>>>>>>> Stashed changes
=======
void LCD_Sprite(int x, int y, int width, int height, const unsigned char bitmap[], int columns, int index, char flip, char offset) {
>>>>>>> Stashed changes
  LCD_CMD(0x02c); // write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);

  unsigned int x2, y2;
  x2 =   x + width;
  y2 =    y + height;
  SetWindows(x, y, x2 - 1, y2 - 1);
  int k = 0;
  int ancho = ((width * columns));
  if (flip) {
    for (int j = 0; j < height; j++) {
      k = (j * (ancho) + index * width - 1 - offset) * 2;
      k = k + width * 2;
      for (int i = 0; i < width; i++) {
        LCD_DATA(bitmap[k]);
        LCD_DATA(bitmap[k + 1]);
        k = k - 2;
      }
    }
  } else {
    for (int j = 0; j < height; j++) {
      k = (j * (ancho) + index * width + 1 + offset) * 2;
      for (int i = 0; i < width; i++) {
        LCD_DATA(bitmap[k]);
        LCD_DATA(bitmap[k + 1]);
        k = k + 2;
      }
    }


  }
  digitalWrite(LCD_CS, HIGH);
}
>>>>>>> Stashed changes
