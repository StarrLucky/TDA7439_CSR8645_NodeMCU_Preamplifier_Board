#ifndef DEFINES_H
#define DEFINES_H


#define STANDBY_PIN     13    // D7     AMPLIFIER STANDBY control pin
#define BT_PIN          2     // D4     CSR8645 module Enable Pin
#define LED_PIN         15    // D8       Pin with 2 Light diodes in
#define CURSENS_PIN     17    // A0     CS60-010 Current Sensor AS324 OPAMP out
#define TEMPSENS_PIN    17    // A0     Temp Sensor pin
#define BUTTON_PIN      0     // D3     Button Pin
#define AMPPOWER_PIN    9     //SD3     Amplifier power relay control pin
#define AMPPOWER_ON     0     //        RELAY MODULE ACTIVE LOW
#define AMPPOWER_OFF    1     //

// Button behaviour. GPIO0 carries the boot strap pull-up, so the contact pulls
// it down: idle is HIGH and a press reads LOW. Flip this if the button is
// wired active high.
#define BUTTON_PRESSED_LEVEL  LOW
#define BUTTON_DEBOUNCE_MS    30     // contact bounce window
#define BUTTON_LONGPRESS_MS   1500   // hold this long for amplifier power toggle


extern bool diag;



#endif
