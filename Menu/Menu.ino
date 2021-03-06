
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


#include <SPI.h>
#include <SD.h>

File vidas;


#define SW1 PF_4
#define SW2 PF_0

#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3


//extern uint8_t ee[];
//extern uint8_t gameover[];



//***************************************************************************************************************************************
// Variables
//***************************************************************************************************************************************
//-------------------- definicion para todas las entidades --------------------------
struct commands {
  unsigned char izq;
  unsigned char der;
  unsigned char disp;
};


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
  struct commands   actions;
};
//naves
struct entity shipP1;
struct entity shipP2;

//enemigos
struct entity shipNPC1;
struct entity shipNPC2;
struct entity shipNPC3;
struct entity shipNPC4;
struct entity shipNPC5;
struct entity shipNPC6;
struct entity shipNPC7;
struct entity shipNPC8;
struct entity shipNPC9;

//balas
struct entity bulletP1;
struct entity bulletP2;

//balas enemigas
struct entity NPCbullet1;
struct entity NPCbullet2;
struct entity NPCbullet3;
struct entity NPCbullet4;
struct entity NPCbullet5;
struct entity NPCbullet6;
struct entity NPCbullet7;
struct entity NPCbullet8;
struct entity NPCbullet9;

unsigned char lectura;

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

String v;
char ojala_funcione;


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


short score;


//-------------------- tiempo -------------------------------------
unsigned long currentMillis;


//***************************************************************************************************************************************
// Inicialización
//***************************************************************************************************************************************
void setup() {
  SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
  Serial.begin(115200);

//coso nuevo
  SPI.setModule (0);

  if (!SD.begin(4)) {
    Serial.println("Ha ocurrido un error en la inicializacion");
    return;
  }
  Serial.println("Inicializacion completa.");

  vidas = SD.open("vidas.txt");
  if (vidas) {
    Serial.println("vidas.txt:");

    // read from the file until there's nothing else in it:
    while (vidas.available()) {
      Serial.write(vidas.read());
      v = vidas.read();
    }
    // close the file:
    vidas.close();
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening vidas.txt");
  }

  ojala_funcione = (v.toInt()) -7;

  shipP1.player.vidas = ojala_funcione;



  
  //--------------------------
  GPIOPadConfigSet(GPIO_PORTB_BASE, 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD_WPU);
  Serial.println("Inicio");
  LCD_Init();
  LCD_Clear(0x00);

  start = 1;

  pinMode(SW1, INPUT_PULLUP);
  pinMode(SW2, INPUT_PULLUP);

  Serial2.begin(115200);


}

unsigned long descanzo;
unsigned long prev;
//***************************************************************************************************************************************
// Loop Infinito
//***************************************************************************************************************************************
void loop() {
  
  currentMillis = millis();
//que sean banderas no los digitos
//
  while(Serial2.available()>0){
    lectura = Serial2.read();
      //derecha de jugador 1
      if (lectura == 'D'){
        shipP1.actions = {0,0,0};
        //shipP2.actions = shipP1.actions;
        shipP1.actions.der = 1;
      }

      if (lectura == 'A'){
        shipP1.actions = {0,0,0};
        //shipP2.actions = shipP1.actions;
        shipP1.actions.izq = 1;
      }

      if (lectura == 'S'){
        shipP1.actions = {0,0,0};
        //shipP2.actions = shipP1.actions;
        shipP1.actions.disp = 1;
      }

      if (lectura == 'Z'){
        shipP2.actions = {0,0,0};
        //shipP2.actions = shipP1.actions;
        shipP2.actions.der = 1;
      }

      if (lectura == 'C'){
        shipP2.actions = {0,0,0};
        //shipP2.actions = shipP1.actions;
        shipP2.actions.izq = 1;
      }

      if (lectura == 'X'){
        shipP2.actions = {0,0,0};
        //shipP2.actions = shipP1.actions;
        shipP2.actions.disp = 1;
      }
  }


  switch (estado_juego) {

    //****************************** TITLE SCREEN ******************************
    case 0: //pantalla de inicio
      if (start){
        SetupMenu();
        start = 0;
        //delay(6800);
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
      int a;
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

        // ---------- niveles ----------
        if (shipP1.player.score >= 0 && shipP1.player.score < 10)
        {
          nivel1Solo();
          a = 1;
        }
        else if (shipP1.player.score >= 10 && shipP1.player.score < 25)
        {
          LCD_Print("2", 200, 200, 2, 0xFFFF, 0x0);
          nivel2Solo();
        }
        else if (shipP1.player.score >= 25 && shipP1.player.score < 45)
        {
          LCD_Print("3", 200, 200, 2, 0xFFFF, 0x0);
          nivel3Solo();
        }
        else 
        {
          estado_juego = 4;
        }



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

        

        // ---------- generacion de enemigos ----------
        // ---------- niveles ----------
        score = shipP1.player.score + shipP2.player.score;


       // ---------- niveles ----------
        if ((score>=0) && (score<10))
        {
          nivel1Solo();
          
          hitboxPlayer (&shipP2, &NPCbullet1);
          hitboxPlayer (&shipP2, &NPCbullet2);

          hitboxNPC(&shipNPC1, &bulletP2, &shipP2);
          hitboxNPC(&shipNPC2, &bulletP2, &shipP2);
        }
        else if ((score>=10) && (score<25))
        {
          nivel2Solo();
          hitboxPlayer (&shipP2, &NPCbullet3);
          hitboxPlayer (&shipP2, &NPCbullet4);
          hitboxPlayer (&shipP2, &NPCbullet5);

          hitboxNPC(&shipNPC3, &bulletP2, &shipP2);
          hitboxNPC(&shipNPC4, &bulletP2, &shipP2);
          hitboxNPC(&shipNPC5, &bulletP2, &shipP2);
        }
        else if ((score>=25) && (score<45))
        {
          nivel3Solo();
          hitboxPlayer (&shipP2, &NPCbullet6);
          hitboxPlayer (&shipP2, &NPCbullet7);
          hitboxPlayer (&shipP2, &NPCbullet8);
          hitboxPlayer (&shipP2, &NPCbullet9);

          hitboxNPC(&shipNPC6, &bulletP2, &shipP2);
          hitboxNPC(&shipNPC7, &bulletP2, &shipP2);
          hitboxNPC(&shipNPC8, &bulletP2, &shipP2);
          hitboxNPC(&shipNPC9, &bulletP2, &shipP2);
        }
        else 
        {
          estado_juego = 4;
        }

        break;


    case 3:
        if (!start){
            GameOver();
            start = 2;
            LCD_Print ("Restart to play again", 20, 210, 1, 0xFFFF, 0x0);
        }
//        if (shipP1.actions.disp == 1 || shipP2.actions.disp == 1){
//          shipP1.actions.disp = 0;
//          shipP2.actions.disp = 0;
//          
//          estado_juego = 0;
//          start = 1;
//          
//        }
        break;

    case 4:
        if (!start){
          Winner();
          start = 2;
          LCD_Print ("Restart to play again", 20, 210, 1, 0xFFFF, 0x0);
        }

//        if (shipP1.actions.disp == 1 || shipP2.actions.disp == 1){
//          shipP1.actions.disp = 0;
//          shipP2.actions.disp = 0;
//          
//          estado_juego = 0;
//          start = 1;
//        }
        break;


    case 6://ee
        if (start){
        LCD_Clear(0x00);
//solo
        start = 0;
        Serial2.print('r');
        }
        for (int i=0; i<6; i++){
        LCD_Sprite (100,70,120,110, ee, 6, i,0,0);
        delay(150);
        }

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


  LCD_Print ("Highscore: 45", 30, 120, 1, 0xFFFF, 0x0);
  LCD_Bitmap (70, 100, 15, 15, nave1);

  //ventana dos jugadores
  Rect (170, 70, 140, 65, 0xFFFF);
  LCD_Print ("Duos", 210, 62, 2, 0xFFFF, 0x0);
  LCD_Bitmap (215, 100, 15, 15, nave1);
  LCD_Bitmap (245, 100, 15, 15, nave2);
//
//  LCD_Print ("WP1:", 180, 120, 1, 0xFFFF, 0x0);
//  LCD_Print ("WP2:", 240, 120, 1, 0xFFFF, 0x0);


  //condiciones iniciales de las naves


  shipP1.dimension = {15, 15};
  shipP1.pos = {155, 170};
  shipP1.mils = {0, 5};
  shipP1.player.score = 0;
  //shipP1.player = {0, 3};
  shipP1.limites = {0,304, 0, 250};
  shipP1.actions = {0,0,0};

  shipP2 = shipP1;
  shipP2.actions = {0,0,0};

  shipNPC1.dimension = {15, 15};
  shipNPC1.pos = {0, 100};
  shipNPC1.mils = {0,20};
  shipNPC1.info = {0,0,0};

  bulletP1.dimension = {8, 3};
  bulletP1.mils = {0, 2};
  bulletP1.info = {0,0,0};

  bulletP2 = bulletP1;

  NPCbullet1 = bulletP1;
  
  NPCbullet1.mils = {0,15};
  NPCbullet1.player = {0,8}; //el 8 sirve para identificar que es bala enemiga

  NPCbullet2 = NPCbullet1;
  NPCbullet3 = NPCbullet1;
  NPCbullet4 = NPCbullet1;
  NPCbullet5 = NPCbullet1;
  NPCbullet6 = NPCbullet1;
  NPCbullet7 = NPCbullet1;
  NPCbullet8 = NPCbullet1;
  NPCbullet9 = NPCbullet1;

  //aparecer la nave 1
  spawn_ship (nave1, &shipP1);

  Serial2.print('t');

}

void SetupSolo () {
//lineas
  LCD_Clear(0x0);
  H_line(0, 191, 319, 0xFFFF);
// ********** text **********
  LCD_Print("Level 1", 105, 200, 2, 0xFFFF, 0x0000);
  LCD_Print("Solo Mode", 5, 195, 1, 0xFFFF, 0x0000);
  LCD_Print("Score:", 260, 195, 1, 0xFFFF, 0x0000);
// ********** vidas J1 **********
  LCD_Bitmap(5,210,15,15,nave1);
  LCD_Bitmap(22,210,15,15,nave1);
  //LCD_Bitmap(39,210,15,15,nave1);
// ********** nave J1 **********
  shipP1.pos = {155, 170};
  shipP1.player.vidas = 3;
  spawn_ship (nave1, &shipP1);
// ********** enemigos **********
  shipNPC1.dimension = {15,15};
  shipNPC1.limites.maxiX = 303;
  shipNPC1.mils.interval = 5;

  shipNPC2.pos = {303, 50};
  shipNPC2.dimension = {15,15};
  shipNPC2.limites.maxiX = 303;
  shipNPC2.mils.interval = 10;

  shipNPC3.pos = {303, 30};
  shipNPC3.dimension = {15,15};
  shipNPC3.limites.maxiX = 303;
  shipNPC3.mils.interval = 15;

  shipNPC4.pos = {0, 70};
  shipNPC4.dimension = {15,15};
  shipNPC4.limites.maxiX = 303;
  shipNPC4.mils.interval = 15;

  shipNPC5.pos = {303, 110};
  shipNPC5.dimension = {15,15};
  shipNPC5.limites.maxiX = 303;
  shipNPC5.mils.interval = 15;

  shipNPC6.pos = {303, 30};
  shipNPC6.dimension = {15,15};
  shipNPC6.limites.maxiX = 303;
  shipNPC6.mils.interval = 15;

  shipNPC7.pos = {0, 60};
  shipNPC7.dimension = {15,15};
  shipNPC7.limites.maxiX = 303;
  shipNPC7.mils.interval = 5;

  shipNPC8.pos = {303, 90};
  shipNPC8.dimension = {15,15};
  shipNPC8.limites.maxiX = 303;
  shipNPC8.mils.interval = 5;

  shipNPC9.pos = {0, 120};
  shipNPC9.dimension = {15,15};
  shipNPC9.limites.maxiX = 303;
  shipNPC9.mils.interval = 5;
}

void SetupDuos (){
    //lineas
  LCD_Clear(0x0);
  H_line(0, 191, 319, 0xFFFF);

  shipP1.player.vidas = 3;
  shipP2.player.vidas = shipP2.player.vidas;
// ********** text ********** 
 LCD_Print(" DUOS! ", 105, 200, 2, 0xFFFF, 0x0000);
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

 
// ********** enemigos **********
  shipNPC1.dimension = {15,15};
  shipNPC1.limites.maxiX = 303;
  shipNPC1.mils.interval = 5;

  shipNPC2.pos = {303, 50};
  shipNPC2.dimension = {15,15};
  shipNPC2.limites.maxiX = 303;
  shipNPC2.mils.interval = 10;

  shipNPC3.pos = {303, 30};
  shipNPC3.dimension = {15,15};
  shipNPC3.limites.maxiX = 303;
  shipNPC3.mils.interval = 15;

  shipNPC4.pos = {0, 70};
  shipNPC4.dimension = {15,15};
  shipNPC4.limites.maxiX = 303;
  shipNPC4.mils.interval = 15;

  shipNPC5.pos = {303, 110};
  shipNPC5.dimension = {15,15};
  shipNPC5.limites.maxiX = 303;
  shipNPC5.mils.interval = 15;

  shipNPC6.pos = {303, 30};
  shipNPC6.dimension = {15,15};
  shipNPC6.limites.maxiX = 303;
  shipNPC6.mils.interval = 15;

  shipNPC7.pos = {0, 60};
  shipNPC7.dimension = {15,15};
  shipNPC7.limites.maxiX = 303;
  shipNPC7.mils.interval = 5;

  shipNPC8.pos = {303, 90};
  shipNPC8.dimension = {15,15};
  shipNPC8.limites.maxiX = 303;
  shipNPC8.mils.interval = 5;

  shipNPC9.pos = {0, 120};
  shipNPC9.dimension = {15,15};
  shipNPC9.limites.maxiX = 303;
  shipNPC9.mils.interval = 5;
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
      Serial2.print('y');
  }
  else{
      LCD_Print("Score Player 1:", 100, 130, 1, 0xFFFF, 0x0);
      LCD_Print(String(shipP1.player.score), 160, 150, 1, 0xFFFF, 0x0);
      Serial2.print('y');
  }
}

void Winner(void)
{
  LCD_Clear(0x0);
  LCD_Print("Congratulations!", 30, 100, 2, 0xFFFF, 0x0);
  LCD_Print("Score Player 1:", 100, 130, 1, 0xFFFF, 0x0);
  LCD_Print(String(shipP1.player.score), 160, 150, 1, 0xFFFF, 0x0);
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
        FillRect(5,210,15,15,0x0);
        FillRect(22,210,15,15,0x0);
        FillRect(39,210,15,15,0x0);
        break;
    case 0:
        boom(explosion_ship, shipP1);
        H_line(0, 191, 319, 0xFFFF);
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
      boom(explosion_ship, shipP2);
      H_line(0, 191, 319, 0xFFFF);
      estado_juego = 3;
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

//las ocndiciones
void move_player (const unsigned char tipo [], struct entity *sel){
  if (currentMillis - sel->mils.previo >= sel->mils.interval){
    //if (digitalRead(SW1)==1 && digitalRead(SW2)==0 && sel->pos.ejeX < sel->limites.maxiX){
    if ((sel->actions.der == 1) && sel->pos.ejeX < sel->limites.maxiX){
      
      (sel->pos.ejeX) ++;

      LCD_Bitmap(sel->pos.ejeX, sel->pos.ejeY, sel->dimension.ancho, sel->dimension.alto, tipo);

      V_line ((sel->pos.ejeX) - 1, sel->pos.ejeY, sel->dimension.ancho, 0x0);
      V_line ((sel->pos.ejeX) +1 + (sel->dimension.ancho), sel->pos.ejeY, sel->dimension.ancho, 0x0);

      //sel->actions.der = 0;
    }

    //if (digitalRead(SW1)==0 && digitalRead(SW2)==1 && sel->pos.ejeX > sel->limites.miniX){
    if ((sel->actions.izq == 1) && sel->pos.ejeX > sel->limites.miniX){

      (sel->pos.ejeX) --;

      LCD_Bitmap(sel->pos.ejeX, sel->pos.ejeY, sel->dimension.ancho, sel->dimension.alto, tipo);

      V_line ((sel->pos.ejeX) - 1, sel->pos.ejeY, sel->dimension.ancho, 0x0);
      V_line ((sel->pos.ejeX) +1 + (sel->dimension.ancho), sel->pos.ejeY, sel->dimension.ancho, 0x0);

      //sel->actions.izq = 0;

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
      if (((sel->pos.ejeY) > (sel->limites.miniY)) && (sel->info.flag)!=1){
        (sel->pos.ejeY) --;
        LCD_Bitmap (sel->pos.ejeX, sel->pos.ejeY, sel->dimension.ancho, sel->dimension.alto, tipo);
        H_line (sel->pos.ejeX, (sel->pos.ejeY) -1, sel->dimension.ancho, 0x0);
        H_line (sel->pos.ejeX, (sel->pos.ejeY) + (sel->dimension.alto) + 1, sel->dimension.ancho, 0x0);

        sel->info.active = 1;
      }
      // en el caso de llegar al limite se enciende la bandera
      else {
        sel->info.flag = 1; //terminó de moverse hacia arriba
      }
      break;


      case DOWN:
      if (((sel->pos.ejeY) < sel->limites.maxiY) && (sel->info.flag)!=2){
        (sel->pos.ejeY) ++;
        LCD_Bitmap (sel->pos.ejeX, sel->pos.ejeY, sel->dimension.ancho, sel->dimension.alto, tipo);
        H_line (sel->pos.ejeX, (sel->pos.ejeY) -1, sel->dimension.ancho, 0x0);
        H_line (sel->pos.ejeX, (sel->pos.ejeY) + (sel->dimension.alto) + 1, sel->dimension.ancho, 0x0);

        sel->info.active = 1;
      }
      else {
        sel->info.flag = 2; //terminó de moverse hacia abajo
      }
      break;

      case LEFT:
      if (((sel->pos.ejeX) > sel->limites.miniX) && (sel->info.flag)!=3){
        (sel->pos.ejeX) --;
        LCD_Bitmap (sel->pos.ejeX, sel->pos.ejeY, sel->dimension.ancho, sel->dimension.alto, tipo);
        V_line ((sel->pos.ejeX) -1, sel->pos.ejeY, sel->dimension.alto, 0x0);
        V_line ((sel->pos.ejeX) + (sel->dimension.ancho) +1, sel->pos.ejeY, sel->dimension.alto, 0x0);

        sel->info.active = 1;
      }
      else {
        sel->info.flag = 3; //terminó de moverse hacia la izquierda
      }
      break;

      case RIGHT:
      if (((sel->pos.ejeX) < (sel->limites.maxiX)) && (sel->info.flag)!=4){
        (sel->pos.ejeX) ++;
        LCD_Bitmap (sel->pos.ejeX, sel->pos.ejeY, sel->dimension.ancho, sel->dimension.alto, tipo);
        V_line ((sel->pos.ejeX) -1, sel->pos.ejeY, sel->dimension.alto, 0x0);
        V_line ((sel->pos.ejeX) + (sel->dimension.ancho) +1, sel->pos.ejeY, sel->dimension.alto, 0x0);

        sel->info.active = 1;
      }
      else {
        sel->info.flag = 4; //terminó de moverse hacia la derecha
      }
      break;

      default:
      sel->info.flag = 5;   //algo más pasó
      break;
    }
    sel->mils.previo = currentMillis;
  }
}

void shoot_player (const unsigned char tipo[], struct entity *sel_ref, struct entity *sel){
  //generar el disparo
  if ((sel_ref->actions.disp == 1) && !(sel->info.active)){
    Serial2.print('u');
    sel->pos.ejeX = (sel_ref->pos.ejeX) + 6;
    sel->pos.ejeY = (sel_ref->pos.ejeY) - (sel->dimension.alto);
    sel->info.active = 1;
    sel->info.hit = 0;

    sel_ref->actions.disp = 0;
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
  if ((currentMillis - (sel->mils.frecuencia) >= sel->mils.interval) && !(sel->info.active) && sel_ref.info.active){
    sel-> mils.frecuencia = currentMillis;
    sel-> pos.ejeX = sel_ref.pos.ejeX + 6;
    sel-> pos.ejeY = sel_ref.pos.ejeY + (sel ->dimension.alto);
    sel-> info.active = 1;
    sel-> info.hit = 0;
  }
}


 void hitboxNPC(struct entity *NPC, struct entity *bala, struct entity *ship){
  if (NPC->info.active && bala->info.active){
    if (bala->pos.ejeY == ((NPC->pos.ejeY) - (NPC->dimension.alto))){
      if (((bala->pos.ejeX) + (bala->dimension.ancho) <= ((NPC->pos.ejeX) + (NPC->dimension.ancho))) && (bala->pos.ejeX >= NPC->pos.ejeX)){
        NPC->info.flag = 1;
        NPC->info.active = 0;
        bala->info.hit=1;
        bala->info.active = 0;
        ship->player.score = (ship->player.score)+5;

        FillRect (bala->pos.ejeX, bala->pos.ejeY, bala->dimension.ancho, bala->dimension.alto, 0x0);
        boom (explosion_bad, *NPC);
      }
    }
  }
}


void hitboxPlayer (struct entity *tetano, struct entity *bala){
  if (bala->pos.ejeY >= ((tetano->pos.ejeY) - (tetano->dimension.alto)) && bala->pos.ejeY+8 <= tetano->pos.ejeY){
    if (((bala->pos.ejeX) + (bala->dimension.ancho) <= ((tetano->pos.ejeX) + (tetano->dimension.ancho))) && (bala->pos.ejeX >= tetano->pos.ejeX)){
      (tetano->player.vidas)--;
      bala->info.hit=1;
      bala->info.active = 0;
      FillRect(tetano->pos.ejeX, (tetano->pos.ejeY)-15, 15, 15, 0x0);
    }
  }
}

void boom (const unsigned char tipo[], struct entity sel){
  //Rect ((sel.pos.ejeX)-12, sel.pos.ejeY, 32, 32, 0x0);
  Serial2.print('i');
  for (char i = 0; i < 5; i++){
    LCD_Sprite((sel.pos.ejeX) - 12, sel.pos.ejeY, 32, 32, tipo, 5, i, 0, 0);
    delay(60);
  }
}

void setup_P1 () {
  move_player (nave1, &shipP1);
  shoot_player (bullet, &shipP1, &bulletP1);
  disparo_volando (bullet, &bulletP1);
  //hitboxPlayer(&shipP1, &NPCbullet1);
}

void setup_P2 () {
  move_player (nave2, &shipP2);
  shoot_player (bullet, &shipP2, &bulletP2);
  disparo_volando (bullet, &bulletP2);
  
}



void nivel1Solo () {
  // ---------- nave 1 ----------
  hitboxPlayer(&shipP1, &NPCbullet1);
  hitboxPlayer(&shipP1, &NPCbullet2);

  // ---------- enemigo 1 ----------
  // up, down, left, right
  if ((shipNPC1.info.flag == 0) || (shipNPC1.info.flag == 3))
  {
    move_NPC(enemy1, &shipNPC1, RIGHT);
  }
  if (shipNPC1.info.flag == 4)
  {
    move_NPC(enemy1, &shipNPC1, LEFT);
  }
  shoot_NPC(bulletE, shipNPC1, &NPCbullet1, 15);
  disparo_volando(bulletE, &NPCbullet1);
  hitboxNPC(&shipNPC1, &bulletP1, &shipP1);

  // ---------- enemigo 2 ----------
  // up, down, left, right
  if ((shipNPC2.info.flag == 0) || (shipNPC2.info.flag == 4))
  {
    move_NPC(enemy1, &shipNPC2, LEFT);
  }
  if (shipNPC2.info.flag == 3)
  {
    move_NPC(enemy1, &shipNPC2, RIGHT);
  }
  shoot_NPC(bulletE, shipNPC2, &NPCbullet2, 15);
  disparo_volando(bulletE, &NPCbullet2);
  hitboxNPC(&shipNPC2, &bulletP1, &shipP1);
}



void nivel2Solo () {
  // ---------- nave 1 ----------
  hitboxPlayer(&shipP1, &NPCbullet3);
  hitboxPlayer(&shipP1, &NPCbullet4);
  hitboxPlayer(&shipP1, &NPCbullet5);

  // ---------- enemigo 3 ----------
  // up, down, left, right
  if ((shipNPC3.info.flag == 0) || (shipNPC3.info.flag == 4))
  {
    move_NPC(enemy3, &shipNPC3, LEFT);
  }
  if (shipNPC3.info.flag == 3)
  {
    move_NPC(enemy3, &shipNPC3, RIGHT);
  }
  shoot_NPC(bulletE, shipNPC3, &NPCbullet3, 15);
  disparo_volando(bulletE, &NPCbullet3);
  hitboxNPC(&shipNPC3, &bulletP1, &shipP1);

  // ---------- enemigo 4 ----------
  // up, down, left, right
  if ((shipNPC4.info.flag == 0) || (shipNPC4.info.flag == 3))
  {
    move_NPC(enemy1, &shipNPC4, RIGHT);
  }
  if (shipNPC4.info.flag == 4)
  {
    move_NPC(enemy1, &shipNPC4, LEFT);
  }
  shoot_NPC(bulletE, shipNPC4, &NPCbullet4, 15);
  disparo_volando(bulletE, &NPCbullet4);
  hitboxNPC(&shipNPC4, &bulletP1, &shipP1);

  // ---------- enemigo 5 ----------
  // up, down, left, right
  if ((shipNPC5.info.flag == 0) || (shipNPC5.info.flag == 4))
  {
    move_NPC(enemy2, &shipNPC5, LEFT);
  }
  if (shipNPC5.info.flag == 3)
  {
    move_NPC(enemy2, &shipNPC5, RIGHT);
  }
  shoot_NPC(bulletE, shipNPC5, &NPCbullet5, 15);
  disparo_volando(bulletE, &NPCbullet5);
  hitboxNPC(&shipNPC5, &bulletP1, &shipP1);
}






void nivel3Solo () {
  // ---------- nave 1 ----------
  hitboxPlayer(&shipP1, &NPCbullet6);
  hitboxPlayer(&shipP1, &NPCbullet7);
  hitboxPlayer(&shipP1, &NPCbullet8);
  hitboxPlayer(&shipP1, &NPCbullet9);

  // ---------- enemigo 6 ----------
  // up, down, left, right
  if ((shipNPC6.info.flag == 0) || (shipNPC6.info.flag == 4))
  {
    move_NPC(enemy3, &shipNPC6, LEFT);
  }
  if (shipNPC6.info.flag == 3)
  {
    move_NPC(enemy3, &shipNPC6, RIGHT);
  }
  shoot_NPC(bulletE, shipNPC6, &NPCbullet6, 15);
  disparo_volando(bulletE, &NPCbullet6);
  hitboxNPC(&shipNPC6, &bulletP1, &shipP1);

  // ---------- enemigo 7 ----------
  // up, down, left, right
  if ((shipNPC7.info.flag == 0) || (shipNPC7.info.flag == 3))
  {
    move_NPC(enemy1, &shipNPC7, RIGHT);
  }
  if (shipNPC7.info.flag == 4)
  {
    move_NPC(enemy1, &shipNPC7, LEFT);
  }
  shoot_NPC(bulletE, shipNPC7, &NPCbullet7, 15);
  disparo_volando(bulletE, &NPCbullet7);
  hitboxNPC(&shipNPC7, &bulletP1, &shipP1);

  // ---------- enemigo 8 ----------
  // up, down, left, right
  if ((shipNPC8.info.flag == 0) || (shipNPC8.info.flag == 4))
  {
    move_NPC(enemy2, &shipNPC8, LEFT);
  }
  if (shipNPC8.info.flag == 3)
  {
    move_NPC(enemy2, &shipNPC8, RIGHT);
  }
  shoot_NPC(bulletE, shipNPC8, &NPCbullet5, 15);
  disparo_volando(bulletE, &NPCbullet8);
  hitboxNPC(&shipNPC8, &bulletP1, &shipP1);

  // ---------- enemigo 9 ----------
  // up, down, left, right
  if ((shipNPC9.info.flag == 0) || (shipNPC9.info.flag == 3))
  {
    move_NPC(enemy4, &shipNPC9, RIGHT);
  }
  if (shipNPC9.info.flag == 4)
  {
    move_NPC(enemy4, &shipNPC9, LEFT);
  }
  shoot_NPC(bulletE, shipNPC9, &NPCbullet5, 15);
  disparo_volando(bulletE, &NPCbullet9);
  hitboxNPC(&shipNPC9, &bulletP1, &shipP1);
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
