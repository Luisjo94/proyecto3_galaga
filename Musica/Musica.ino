/*  RickRollCode

    AUTHOR: Rowan Packard
    rowanpackard@gmail.com

    DISCLAIMER: The song "Never Gonna Give You Up" by Rick Astley
    is not the creative property of the author. This code simply
    plays a Piezo buzzer rendition of the song.
*/

#include "musica_ee.h"
#define piezo PD7

void play (void);

volatile int beatlength = 100; // determines tempo
float beatseparationconstant = 0.3;


char flag = 1;

char a = 4;
char b = 0;


char RickRoll = 'r';

void setup()
{
  pinMode(piezo, OUTPUT);
  Serial.begin(9600);

}

void loop()
{
  // play next step in song
  Serial.print(Serial.read());
  if (Serial.read() == RickRoll){
    play ();
  }
}

void play() {
    unsigned short notelength;
  //primera parte de la cancion, se repite dos veces
  if (a == 1 || a == 2) {
    // intro
    notelength = beatlength * song1_intro_rhythmn[b];
    if (song1_intro_melody[b] > 0) {
      //digitalWrite(led, HIGH);
      tone(piezo, song1_intro_melody[b], notelength);
    }
    b++;
    if (b >= sizeof(song1_intro_melody) / sizeof(int)) {
      a++;
      b = 0;

    }
  }

  //se repite tres veces cuando a=3 y a=5
  else if (a == 3 || a == 5) {
    // verse
    notelength = beatlength * 2 * song1_verse1_rhythmn[b];
    if (song1_verse1_melody[b] > 0) {
      //digitalWrite(led, HIGH);
      //Serial.print(lyrics_verse1[c]); son las lyrics, no importan
      tone(piezo, song1_verse1_melody[b], notelength);

    }
    b++;
    if (b >= sizeof(song1_verse1_melody) / sizeof(int)) {
      a++;
      b = 0;

    }
  }

  //parte 4 y 6
  else if (a == 4 || a == 6) {
    // chorus
    notelength = beatlength * song1_chorus_rhythmn[b];
    if (song1_chorus_melody[b] > 0) {
      //digitalWrite(led, HIGH);
      //Serial.print(lyrics_chorus[c]);
      tone(piezo, song1_chorus_melody[b], notelength);

    }
    b++;
    if (b >= sizeof(song1_chorus_melody) / sizeof(int)) {
      a++;
      b = 0;

    }
  }
  
  delay(notelength);
  noTone(piezo);
//  digitalWrite(led, LOW);
  delay(notelength * beatseparationconstant);

  //finalizacion dela cancion
  if (a == 7) { // loop back around to beginning of song
    a = 1;
  }
}
  
