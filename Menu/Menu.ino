
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

    Se trabajó sobre la librería para el uso de pantalla ILI9341 previamente creada por Pablo Mazariegos y José Morales
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

#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3




//***************************************************************************************************************************************
// Variables
//***************************************************************************************************************************************
//-------------------- definicion para todas las entidades --------------------------
struct dimensions {
  char alto;
  char ancho;
};

struct posicion {
  short ejeX;
  short ejeY;
};

struct tiempo {
  unsigned long previo;
  unsigned long interval;
  unsigned long frecuencia;
};

struct jugador {
  unsigned short score;
  char vidas;
};

struct misc {
  char active;
  char hit;
  char flag;
};

struct bound {
  short miniX;
  short maxiX;
  short miniY;
  short maxiY;
};

struct entity {
  struct dimensions dimension;
  struct posicion   pos;
  struct tiempo     mils;
  struct jugador    player;
  struct misc       info;
  struct bound      limites;
};
//naves
struct entity shipP1;
struct entity shipP2;

struct entity shipNPC1;
struct entity shipNPC2;
struct entity shipNPC3;
struct entity shipNPC4;
struct entity shipNPC5;
struct entity shipNPC6;


//balas
struct entity bulletP1;
struct entity bulletP2;

struct entity NPCbullet1;



//------------------- limites
//------------------- Estado del juego ----------------------
char estado_juego = 0; //al reainiciar es el estado default, pantalla de inicio
char start = 1; //bandera para cargar el menu
char duos_flag = 0;
// 1, solo
// 2, duos
// 3, endgame
//  3.1 tiene highscore
//  3.2 no tiene highscore


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
void LCD_Sprite(int x, int y, int width, int height, unsigned char bitmap[], int columns, int index, char flip, char offset);


//-------------------- prototipo de funciones --------------------------------------------
//pantalla inicial y condiciones iniciales
void SetupMenu (void);
void SetupSolo (void);
void SetupDuos (void);

//spawnear naves
void spawn_ship (unsigned char tipo [], struct entity *sel);

//mover naves
void move_player (unsigned char tipo [], struct entity *sel);
void move_NPC2 (unsigned char tipo [], struct entity *sel, short mini, short maxi, char direccion);

//disparo desde nave
void shoot_player (unsigned char tipo[], struct entity sel_ref, struct entity *sel);
void shoot_NPC (const unsigned char tipo[], struct entity sel_ref, struct entity *sel, unsigned short tiempo);
void disparo_volando (unsigned char tipo [], struct entity *sel);

//vidas y hitboxes
void vidasJ1 (struct entity *sel);
void hitboxNPC(struct entity *NPC, struct entity *bala, struct entity *ship);
void boom (unsigned char tipo[], struct entity sel);

//setup de las naves
void setup_P1 (void);
void setup_P2 (void);
void setup_nivel1 (void);
//viejas funciones




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

  start = 1;

  pinMode(SW1, INPUT_PULLUP);
  pinMode(SW2, INPUT_PULLUP);


}


//***************************************************************************************************************************************
// Loop Infinito
//***************************************************************************************************************************************
void loop() {
  currentMillis = millis();

  switch (estado_juego) {

    //****************************** TITLE SCREEN ******************************
    case 0: //pantalla de inicio
      if (start){
        SetupMenu();
        start = 0;
      }
      setup_P1 ();

      if (bulletP1.pos.ejeY <= 135){
        if (bulletP1.pos.ejeX <= 150 - bulletP1.dimension.ancho && bulletP1.pos.ejeX >= 7){
          estado_juego = 1;
          bulletP1.info.hit = 1;
          bulletP1.info.active = 0;
          start = 1;
        }

        if (bulletP1.pos.ejeX <= 315 - bulletP1.dimension.ancho && bulletP1.pos.ejeX >= 169){
          estado_juego = 2;
          bulletP1.info.hit = 1;
          bulletP1.info.active = 0;
          start = 1;
        }
        if (bulletP1.pos.ejeY <= 40) {
          if (bulletP1.info.active){
            estado_juego = 6;
            bulletP1.info.hit = 1;
            bulletP1.info.active = 0;
            start = 1;
          }
        }
      }
      
      break;

      // ****************************** SOLO MODE ******************************
      case 1:
      // ---------- map setup ----------
        if (start){
          SetupSolo ();
          start = 0;

          //condiciones iniciales de los enemigos
          shipNPC1.mils.interval = 20;
        }
        // ---------- movimiento y disparos----------
        setup_P1 ();

        // ---------- vidas ----------
        vidasJ1(&shipP1);

        // ---------- puntos ----------
        ScoreSoloMode(shipP1.player.score);

        // ---------- enemigos ----------
        
        //spawn_ship(enemy1, &shipNPC1); No es necesari spaqwnear la nave enemiga ya que con el movimiento aparece
        move_NPC(enemy1, &shipNPC1, RIGHT);
        shoot_NPC(bullet, shipNPC1, &NPCbullet1, 15);
        disparo_volando(bullet, &NPCbullet1);
        hitboxNPC(&shipNPC1, &bulletP1, &shipP1);
        


//
//        // ---------- movimiento y disparos nave 1----------
//        P1_setup ();
//
//        // ---------- movimiento y disparos enemy1----------
//        if (bad1.flag == 0){
//          move_NPC(enemy1, bad1.ancho, bad1.alto, bad1.ejeY, &bad1.ejeX, enemy.xMin, enemy.xMax, &bad1.previo, bad1.intervalo, RIGHT, &bad1.flag);
//        }
//
//          //move_NPC(enemy1, bad1.ancho, bad1.alto, bad1.ejeY, &bad1.ejeX, enemy.xMin, enemy.xMax, &bad1.previo, bad1.intervalo, LEFT, &bad1.flag);
//
//
//        //hitbox menu
//      if (bulletP1.ejeY == bad1.ejeY+15){
//        if (bulletP1.ejeX <= bad1.ejeX + bad1.ancho && bulletP1.ejeX >= bad1.ejeX && bulletP1.active){
//          P1.score = P1.score+5;
//          bulletP1.hit = 1;
//          bulletP1.active = 0;
//          bad1.flag = 1;
//
//          boom (explosion_bad, bad1.ejeX, bad1.ejeY);
//
//          for (char i = 0; i < 5; i++)
//          {
//            LCD_Sprite(bad1.ejeX-12, bad1.ejeY-10, 32, 32, explosion_bad, 5, i, 0, 0);
//            delay(20);
//          }
//          bad1.ejeX = -15;
//
//        }
//      }
//
      break;


      // ****************************** DUOS MODE ******************************
    case 2:
        if (start){
            SetupDuos (); //cambiar a duos despues
            start = 0;
            duos_flag = 1;
            //posiciones iniciales en duos
            shipP1.pos = {0,170};
            shipP2.pos = {303,170};
        }

        // ---------- movimiento y disparos ----------
        setup_P1();
        setup_P2();

        // ---------- vidas ----------
        vidasJ1(&shipP1);
        vidasJ2(&shipP2);

        // ---------- puntos ----------
        ScoreDuosMode(shipP1.player.score, shipP2.player.score);

        // ---------- enemigos ----------
        shipNPC1.pos = {0, 100};
        move_NPC(enemy1, &shipNPC1, RIGHT);


        break;


    case 3:
        if (!start){
            GameOver();
            start = 2;
        }
        break;


    case 6://ee
        if (start){
        LCD_Clear(0x00);
        
        start = 0;
        }
//        for (int i=0; i<6; i++){
//        LCD_Sprite (100,70,120,110, ee, 6, i,0,0);
//        delay(150);
//        }

        break;


    default: 
        estado_juego = 0;
      break;


  }
}
//***************************************************************************************************************************************
// Función para menus
//***************************************************************************************************************************************
void SetupMenu (){
  //limpiar pantalla
  LCD_Clear(0x00);

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


  //condiciones iniciales de las naves


  shipP1.dimension = {15, 15};
  shipP1.pos = {155, 170};
  shipP1.mils = {0, 5};
  shipP1.player = {0, 3};
  shipP1.limites = {0,304, 0, 250};

  shipP2 = shipP1;

  shipNPC1.dimension = {15, 15};
  shipNPC1.pos = {0, 100};
  shipNPC1.mils = {0,20};
  shipNPC1.info = {0,0,0};

  bulletP1.dimension = {8, 3};
  bulletP1.mils = {0, 5};
  bulletP1.info = {0,0,0};
  

  bulletP2 = bulletP1;
  NPCbullet1 = bulletP1;
  NPCbullet1.mils = {0,15};
  NPCbullet1.player = {0,8}; //el 8 sirve para identificar que es bala enemiga

  //aparecer la nave 1
  spawn_ship (nave1, &shipP1);

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
  //LCD_Bitmap(39,210,15,15,nave1);
// ********** nave J1 **********
  shipP1.pos = {155, 170};
  spawn_ship (nave1, &shipP1);
// ********** enemigos **********
shipNPC1.dimension = {15,15};
shipNPC1.limites.maxiX = 303;
shipNPC1.mils.interval = 5;
}

void SetupDuos (){
    //lineas
  LCD_Clear(0x0);
  H_line(0, 191, 319, 0xFFFF);
// ********** text **********
  LCD_Print("Nivel 1", 105, 200, 2, 0xFFFF, 0x0000);
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
 shipP1.pos = {0, 170};
 shipP2.pos = {303, 170};
 spawn_ship (nave1, &shipP1);
 spawn_ship (nave2, &shipP2);
 
 // ********** balas **********
}

void GameOver(void)
{
  LCD_Clear(0x0);
  LCD_Bitmap(10, 70, 293, 33, gameover);
  if (duos_flag){
      LCD_Print("Score Player 1:", 20, 130, 1, 0xFFFF, 0x0);
      LCD_Print("Score Player 2:", 180, 130, 1, 0xFFFF, 0x0);
      LCD_Print(String(shipP1.player.score), 80, 150, 1, 0xFFFF, 0x0);
      LCD_Print(String(shipP2.player.score), 230, 150, 1, 0xFFFF, 0x0);
  }
  else{
      LCD_Print("Score Player 1:", 100, 130, 1, 0xFFFF, 0x0);
      LCD_Print(String(shipP1.player.score), 160, 150, 1, 0xFFFF, 0x0);
  }
}

void vidasJ1 (struct entity *sel){
  switch(sel->player.vidas){
    case 3:
        FillRect(39,210,15,15,0x0);
        break;
    case 2:
        FillRect(22,210,15,15,0x0);
        FillRect(39,210,15,15,0x0);
        break;
    case 1:
        FillRect(2,210,15,15,0x0);
        FillRect(22,210,15,15,0x0);
        FillRect(39,210,15,15,0x0);
        break;
    case 0:
        estado_juego = 3;
        break;
  }
}

void vidasJ2(struct entity *sel)
{
  switch(sel->player.vidas)
  {
    case 3:
      FillRect(265,210,15,15,0x0);
      break;
    case 2:
      FillRect(282,210,15,15,0x0);
      FillRect(265,210,15,15,0x0);
      break;
    case 1:
      FillRect(265,210,15,15,0x0);
      FillRect(282,210,15,15,0x0);
      FillRect(299,210,15,15,0x0);
      break;
    case 0:
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

void spawn_ship (const unsigned char tipo [], struct entity *sel){
  LCD_Bitmap(sel->pos.ejeX, sel->pos.ejeY, sel->dimension.ancho, sel->dimension.alto, tipo);
}
//---------------------funciones para entidades -----------------------------------------
void move_player (const unsigned char tipo [], struct entity *sel){
  if (currentMillis - sel->mils.previo >= sel->mils.interval){
    if (digitalRead(SW1)==1 && digitalRead(SW2)==0 && sel->pos.ejeX < sel->limites.maxiX){
      (sel->pos.ejeX) ++;

      LCD_Bitmap(sel->pos.ejeX, sel->pos.ejeY, sel->dimension.ancho, sel->dimension.alto, tipo);

      V_line ((sel->pos.ejeX) - 1, sel->pos.ejeY, sel->dimension.ancho, 0x0);
      V_line ((sel->pos.ejeX) +1 + (sel->dimension.ancho), sel->pos.ejeY, sel->dimension.ancho, 0x0);
    }

    if (digitalRead(SW1)==0 && digitalRead(SW2)==1 && sel->pos.ejeX > sel->limites.miniX){
      (sel->pos.ejeX) --;

      LCD_Bitmap(sel->pos.ejeX, sel->pos.ejeY, sel->dimension.ancho, sel->dimension.alto, tipo);

      V_line ((sel->pos.ejeX) - 1, sel->pos.ejeY, sel->dimension.ancho, 0x0);
      V_line ((sel->pos.ejeX) +1 + (sel->dimension.ancho), sel->pos.ejeY, sel->dimension.ancho, 0x0);
    }
    sel->mils.previo = currentMillis;
  }
}

//pude mover un NPC a cualquier direccion
void move_NPC (const unsigned char tipo [], struct entity *sel, char direccion){
  //chequea los millis para ell moviemiento, la velociad
  if (currentMillis - sel->mils.previo >= sel->mils.interval){
    //permite elegir la dirccion a la que se movera el enmigo
    switch (direccion) {
      case UP:
      //revisa que no haya llegado al limite y la bandera este apagada
      if (((sel->pos.ejeY) > (sel->limites.miniY)) && !(sel->info.flag)){
        (sel->pos.ejeY) --;
        LCD_Bitmap (sel->pos.ejeX, sel->pos.ejeY, sel->dimension.ancho, sel->dimension.alto, tipo);
        H_line (sel->pos.ejeX, (sel->pos.ejeY) -1, sel->dimension.ancho, 0x0);
        H_line (sel->pos.ejeX, (sel->pos.ejeY) + (sel->dimension.alto) + 1, sel->dimension.ancho, 0x0);

        sel->info.active = 1;
      }
      // en el caso de llegar al limite se enciende la bandera
      else {
        sel->info.flag = 1;
      }
      break;


      case DOWN:
      if (((sel->pos.ejeY) < sel->limites.maxiY) && !(sel->info.flag)){
        (sel->pos.ejeY) ++;
        LCD_Bitmap (sel->pos.ejeX, sel->pos.ejeY, sel->dimension.ancho, sel->dimension.alto, tipo);
        H_line (sel->pos.ejeX, (sel->pos.ejeY) -1, sel->dimension.ancho, 0x0);
        H_line (sel->pos.ejeX, (sel->pos.ejeY) + (sel->dimension.alto) + 1, sel->dimension.ancho, 0x0);
        
        sel->info.active = 1;
      }
      else {
        sel->info.flag = 1;
      }
      break;

      case LEFT:
      if (((sel->pos.ejeX) > sel->limites.miniX) && !(sel->info.flag)){
        (sel->pos.ejeX) --;
        LCD_Bitmap (sel->pos.ejeX, sel->pos.ejeY, sel->dimension.ancho, sel->dimension.alto, tipo);
        V_line ((sel->pos.ejeX) -1, sel->pos.ejeY, sel->dimension.alto, 0x0);
        V_line ((sel->pos.ejeX) + (sel->dimension.ancho) +1, sel->pos.ejeY, sel->dimension.alto, 0x0);
      
        sel->info.active = 1;
      }
      else {
        sel->info.flag = 1;
      }
      break;

      case RIGHT:
      if (((sel->pos.ejeX) < (sel->limites.maxiX)) && !(sel->info.flag)){
        (sel->pos.ejeX) ++;
        LCD_Bitmap (sel->pos.ejeX, sel->pos.ejeY, sel->dimension.ancho, sel->dimension.alto, tipo);
        V_line ((sel->pos.ejeX) -1, sel->pos.ejeY, sel->dimension.alto, 0x0);
        V_line ((sel->pos.ejeX) + (sel->dimension.ancho) +1, sel->pos.ejeY, sel->dimension.alto, 0x0);
      
        sel->info.active = 1;
      }
      else {
        sel->info.flag = 1;
      }
      break;

      default:
      sel->info.flag = 2;
      break;
    }
    sel->mils.previo = currentMillis;
  }
}

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
    //si se trata de disparo de jugador
    if (sel->player.vidas <= 3){
            //disparo activo y fuera de la pantalla
      if (sel->pos.ejeY < -(sel->dimension.alto)) {
        sel-> info.active = 0;
        sel-> info.hit = 1;
      }

      //disparo activo y dentro de la pantalla
      else {
        if (currentMillis - (sel->mils.previo) >= sel->mils.interval){
          sel ->mils.previo = currentMillis;
          (sel ->pos.ejeY) --;
          LCD_Bitmap(sel->pos.ejeX, sel->pos.ejeY, sel->dimension.ancho, sel->dimension.alto, tipo);
        }
      }
    }
    
    //disparo de enemigo
    else{
      //se salio de la pantalla
        if (sel->pos.ejeY > 180) {
        sel-> info.active = 0;
        sel-> info.hit = 1;
        FillRect (sel->pos.ejeX, sel->pos.ejeY, sel->dimension.ancho, sel->dimension.alto, 0x0);
      }

      //disparo activo y dentro de la pantalla
      else {
        if (currentMillis - (sel->mils.previo) >= sel->mils.interval){
          sel ->mils.previo = currentMillis;
          (sel ->pos.ejeY) ++;
          LCD_Bitmap(sel->pos.ejeX, sel->pos.ejeY, sel->dimension.ancho, sel->dimension.alto, tipo);
          H_line (sel->pos.ejeX, (sel->pos.ejeY) -1, sel->dimension.ancho, 0x0);
        }
      }
    }
  }  
}






void shoot_NPC (const unsigned char tipo[], struct entity sel_ref, struct entity *sel, unsigned short tiempo){
  //generar el disparo con frecuencia
  if ((currentMillis - (sel->mils.frecuencia) >= tiempo) && !(sel->info.active) && sel_ref.info.active){
    sel-> mils.frecuencia = currentMillis;
    sel-> pos.ejeX = sel_ref.pos.ejeX + 6;
    sel-> pos.ejeY = (sel_ref.pos.ejeY) + (sel_ref.dimension.alto);
    sel-> info.active = 1;
    sel-> info.hit = 0;
  }
}


void hitboxNPC(struct entity *NPC, struct entity *bala, struct entity *ship){
  if (NPC->info.active && bala->info.active){
    if (bala->pos.ejeY == ((NPC->pos.ejeY) - (NPC->dimension.alto))){
      if (((bala->pos.ejeX) + (bala->dimension.ancho) <= ((NPC->pos.ejeX) + (NPC->dimension.ancho))) && (bala->pos.ejeX >= NPC->pos.ejeX)){
        FillRect (bala->pos.ejeX, bala->pos.ejeY, bala->dimension.ancho, bala->dimension.alto, 0x0);
        NPC->info.flag = 1;
        NPC->info.active = 0;
        bala->info.hit=1;
        bala->info.active = 0;
        ship->player.score = (ship->player.score)+5;
        boom (explosion_bad, *NPC);
      }
    }
  }
}
//***************************************************************************************************************************************
// Función para dibujar un rectángulo - parámetros ( coordenada x, cordenada y, ancho, alto, color)
//***************************************************************************************************************************************
void boom (const unsigned char tipo[], struct entity sel){
  Rect ((sel.pos.ejeX)-12, sel.pos.ejeY, 32, 32, 0x0);
  for (char i = 0; i < 5; i++){
    LCD_Sprite((sel.pos.ejeX) - 12, (sel.pos.ejeY), 32, 32, tipo, 5, i, 0, 0);
    delay(40);
  }
}

void setup_P1 () {
  move_player (nave1, &shipP1);
  shoot_player (bullet, shipP1, &bulletP1);
  disparo_volando (bullet, &bulletP1);
}

void setup_P2 () {
  move_player (nave2, &shipP2);
  shoot_player (bullet, shipP2, &bulletP2);
  disparo_volando (bullet, &bulletP2);
}

void setup_nivel1 (){
  shipNPC2 = shipNPC1;
  shipNPC3 = shipNPC1;
  shipNPC4 = shipNPC1;
  shipNPC5 = shipNPC1;
  shipNPC6 = shipNPC1;
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
void LCD_Bitmap(unsigned int x, unsigned int y, unsigned int width, unsigned int height, const unsigned char bitmap[]) {
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
void LCD_Sprite(int x, int y, int width, int height, const unsigned char bitmap[], int columns, int index, char flip, char offset) {
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

//
