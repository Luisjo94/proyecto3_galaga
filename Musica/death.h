// Credit:
// Midi to Arduino Converter
//     - Andy Tran (extramaster), 2015
// https://www.extramaster.net/tools/midiToArduino/
//
// Process:
// Midi -> Midi tracks -> Note mappings -> Frequency
//
// CC0

// Set this to be the pin that your buzzer resides in. (Note that you can only have one buzzer actively using the PWM signal at a time).

#ifndef death.h
#define death.h

void game_over(int tonePin) {

    tone(tonePin, 493, 71.875);
    delay(79.8611111111);
    delay(3.47222222222);
    tone(tonePin, 554, 71.875);
    delay(79.8611111111);
    delay(3.47222222222);
    tone(tonePin, 659, 71.875);
    delay(79.8611111111);
    delay(3.47222222222);
    tone(tonePin, 554, 71.875);
    delay(79.8611111111);
    delay(3.47222222222);
    tone(tonePin, 659, 71.875);
    delay(79.8611111111);
    delay(3.47222222222);
    tone(tonePin, 739, 71.875);
    delay(79.8611111111);
    delay(3.47222222222);
    tone(tonePin, 830, 71.875);
    delay(79.8611111111);
    delay(3.47222222222);
    tone(tonePin, 659, 71.875);
    delay(79.8611111111);
    delay(3.47222222222);
    tone(tonePin, 739, 71.875);
    delay(79.8611111111);
    delay(3.47222222222);
    tone(tonePin, 830, 71.875);
    delay(79.8611111111);
    delay(3.47222222222);
    tone(tonePin, 987, 71.875);
    delay(79.8611111111);
    delay(3.47222222222);
    tone(tonePin, 739, 71.875);
    delay(79.8611111111);
    delay(3.47222222222);
    tone(tonePin, 830, 71.875);
    delay(79.8611111111);
    delay(3.47222222222);
    tone(tonePin, 987, 71.875);
    delay(79.8611111111);
    delay(3.47222222222);
    tone(tonePin, 1244, 71.875);
    delay(79.8611111111);
    delay(170.138888889);
    tone(tonePin, 1318, 146.875);
    delay(163.194444444);
    delay(86.8055555556);
    tone(tonePin, 1244, 221.875);
    delay(246.527777778);
    delay(3.47222222222);
    tone(tonePin, 1108, 250.0);
    delay(277.777777778);

}

#endif
