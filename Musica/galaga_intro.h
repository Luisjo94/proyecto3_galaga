#ifndef galaga_intro.h
#define galaga_intro.h


// Credit:
// Midi to Arduino Converter
//     - Andy Tran (extramaster), 2015
// https://www.extramaster.net/tools/midiToArduino/
//
// Process:
// Midi -> Midi tracks -> Note mappings -> Frequency
//
// CC0




void intro (int tonePin);
// Set this to be the pin that your buzzer resides in. (Note that you can only have one buzzer actively using the PWM signal at a time).


void intro(int tonePin) {
    tone(tonePin, 659, 234.375);
    delay(260.416666667);
    tone(tonePin, 391, 117.1875);
    delay(130.208333333);
    tone(tonePin, 587, 234.375);
    delay(260.416666667);
    tone(tonePin, 440, 117.1875);
    delay(130.208333333);
    tone(tonePin, 523, 234.375);
    delay(260.416666667);
    tone(tonePin, 391, 117.1875);
    delay(130.208333333);
    tone(tonePin, 440, 234.375);
    delay(260.416666667);
    tone(tonePin, 523, 117.1875);
    delay(130.208333333);
    tone(tonePin, 659, 234.375);
    delay(260.416666667);
    tone(tonePin, 391, 117.1875);
    delay(130.208333333);
    tone(tonePin, 587, 234.375);
    delay(260.416666667);
    tone(tonePin, 440, 117.1875);
    delay(130.208333333);
    tone(tonePin, 523, 234.375);
    delay(260.416666667);
    tone(tonePin, 391, 117.1875);
    delay(130.208333333);
    tone(tonePin, 523, 234.375);
    delay(260.416666667);
    tone(tonePin, 783, 117.1875);
    delay(130.208333333);
    tone(tonePin, 830, 234.375);
    delay(260.416666667);
    tone(tonePin, 783, 117.1875);
    delay(130.208333333);
    tone(tonePin, 698, 234.375);
    delay(260.416666667);
    tone(tonePin, 622, 117.1875);
    delay(130.208333333);
    tone(tonePin, 587, 234.375);
    delay(260.416666667);
    tone(tonePin, 523, 117.1875);
    delay(130.208333333);
    tone(tonePin, 466, 234.375);
    delay(260.416666667);
    tone(tonePin, 587, 117.1875);
    delay(130.208333333);
    tone(tonePin, 622, 234.375);
    delay(260.416666667);
    tone(tonePin, 698, 117.1875);
    delay(130.208333333);
    tone(tonePin, 622, 234.375);
    delay(260.416666667);
    tone(tonePin, 523, 117.1875);
    delay(130.208333333);
    tone(tonePin, 698, 117.1875);
    delay(130.208333333);
    tone(tonePin, 587, 117.1875);
    delay(130.208333333);
    tone(tonePin, 440, 117.1875);
    delay(130.208333333);
    tone(tonePin, 587, 117.1875);
    delay(130.208333333);
    tone(tonePin, 493, 117.1875);
    delay(130.208333333);
    tone(tonePin, 440, 468.75);
    delay(520.833333333);
}

#endif
