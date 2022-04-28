// Credit:
// Midi to Arduino Converter
//     - Andy Tran (extramaster), 2015
// https://www.extramaster.net/tools/midiToArduino/
//
// Process:
// Midi -> Midi tracks -> Note mappings -> Frequency
//
// CC0
#ifndef disparo.h
#define disparo.h



// Set this to be the pin that your buzzer resides in. (Note that you can only have one buzzer actively using the PWM signal at a time).
void shoot (int tonePin);

void shoot(int tonePin) {

    tone(tonePin, 369, 8.72093023256);
    delay(9.68992248062);
    delay(9.68992248062);
    tone(tonePin, 1174, 8.72093023256);
    delay(9.68992248062);
    tone(tonePin, 69, 8.72093023256);
    delay(9.68992248062);
    tone(tonePin, 277, 8.72093023256);
    delay(9.68992248062);
    delay(9.68992248062);
    delay(9.68992248062);
    tone(tonePin, 58, 8.72093023256);
    delay(9.68992248062);
    tone(tonePin, 1046, 8.72093023256);
    delay(9.68992248062);
    tone(tonePin, 246, 8.72093023256);
    delay(9.68992248062);
    tone(tonePin, 311, 8.72093023256);
    delay(9.68992248062);
    delay(9.68992248062);
    delay(9.68992248062);
    tone(tonePin, 329, 8.72093023256);
    delay(9.68992248062);
    tone(tonePin, 659, 8.72093023256);
    delay(9.68992248062);
    delay(9.68992248062);
    tone(tonePin, 155, 8.72093023256);
    delay(9.68992248062);
    delay(9.68992248062);
    delay(9.68992248062);
    delay(9.68992248062);
    tone(tonePin, 293, 8.72093023256);
    delay(9.68992248062);
    tone(tonePin, 61, 8.72093023256);
    delay(9.68992248062);
    tone(tonePin, 391, 8.72093023256);
    delay(9.68992248062);
    delay(9.68992248062);
    tone(tonePin, 1108, 8.72093023256);
    delay(9.68992248062);
    tone(tonePin, 25, 8.72093023256);
    delay(9.68992248062);
    delay(9.68992248062);
    delay(9.68992248062);
    tone(tonePin, 493, 8.72093023256);
    delay(9.68992248062);
    tone(tonePin, 130, 8.72093023256);
    delay(9.68992248062);
    tone(tonePin, 830, 8.72093023256);
    delay(9.68992248062);
    delay(9.68992248062);
    tone(tonePin, 440, 8.72093023256);
    delay(9.68992248062);
    tone(tonePin, 1046, 8.72093023256);
    delay(9.68992248062);
    tone(tonePin, 261, 8.72093023256);
    delay(9.68992248062);
    delay(9.68992248062);
    tone(tonePin, 659, 8.72093023256);
    delay(9.68992248062);
    delay(9.68992248062);
    delay(9.68992248062);
    tone(tonePin, 415, 8.72093023256);
    delay(9.68992248062);
    tone(tonePin, 87, 8.72093023256);
    delay(9.68992248062);
    delay(9.68992248062);
    tone(tonePin, 1174, 8.72093023256);
    delay(9.68992248062);
    tone(tonePin, 92, 8.72093023256);
    delay(9.68992248062);
    delay(9.68992248062);
    tone(tonePin, 82, 17.4418604651);
    delay(19.3798449612);
    delay(9.68992248062);
    delay(9.68992248062);
    tone(tonePin, 311, 17.4418604651);
    delay(19.3798449612);
    tone(tonePin, 1046, 8.72093023256);
    delay(9.68992248062);
    delay(19.3798449612);
    delay(9.68992248062);
    tone(tonePin, 293, 8.72093023256);
    delay(9.68992248062);
    delay(9.68992248062);
    delay(9.68992248062);
    tone(tonePin, 46, 17.4418604651);
    delay(19.3798449612);
    delay(9.68992248062);
    delay(29.0697674419);
    tone(tonePin, 207, 26.1627906977);
    delay(29.0697674419);
    delay(19.3798449612);
    tone(tonePin, 116, 26.1627906977);
    delay(29.0697674419);

}

#endif
