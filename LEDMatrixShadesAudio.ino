// AS1130 LED Shades
// Designed for 8MHz 3.3V ATMega328P
// Garrett Mace
// 2015 macetech LLC

#include <Wire.h>            // I2C library
#include <avr/pgmspace.h>    // Flash storage of long strings and fonts

// Initial settings
#define CYCLETIME 5000
#define startbrightness     153 

// Global variables
unsigned long currentMillis = 0;
unsigned long cycleMillis = 0;
unsigned long effectMillis = 0;
unsigned long audioMillis = 0;
boolean autoCycle = true;
byte currentEffect = 0;

// Data tables stored in flash memory
#include "messages.h"          // Message strings
#include "mathtables.h"        // Useful tables of values
#include "pcbiosfont.h"        // 8x8 bitmap font
#include "graphicframes.h"     // bitmap graphics

// Helper functions for AS1130 interfacing and graphics generation
#include "AS1130functions.h"   // Generic AS1130 functions
#include "glassesfunctions.h"  // Glasses-specifc functions
#include "audio.h"             // Interface with MSGEQ7 chip
#include "effects.h"   // Graphical patterns
#include "buttons.h"           // button handler

// Begin program
void setup() {

  Wire.begin();                // Start I2C
  TWBR = 2;                    // Set to 400KHz bus speed (on 8MHz device)

  glassesInit();               // Preconfigure AS1130 chips

  pinMode(MODEBUTTON, INPUT_PULLUP);          // Prepare button input
  pinMode(BRIGHTNESSBUTTON, INPUT_PULLUP);    // Prepare button input
  analogReference(DEFAULT);                   // Select analog reference source
  pinMode(ANALOGPIN, INPUT);                  // Set MSGEQ7 input pin to input
  pinMode(STROBEPIN, OUTPUT);                 // Set MSGEQ7 strobe pin to output
  pinMode(RESETPIN, OUTPUT);                  // Set MSGEQ7 reset pin to output
  digitalWrite(RESETPIN, LOW);                // Set MSGEQ7 reset pin to initial value
  digitalWrite(STROBEPIN, HIGH);              // Set MSGEQ7 strobe pin to initial value

}




// List of effects that will be displayed
functionList effectList[] = { audioHearts,
  realEQ,
  bigVU,
  fillAudioPWM,
  audioRain
};

const byte numEffects = (sizeof(effectList)/sizeof(effectList[0]));

// Main loop
void loop() 
{
  currentMillis = millis();
  updateButtons();
  doButtons();

  if (currentMillis - audioMillis > AUDIODELAY) {
    audioMillis = currentMillis;  
    doAnalogs();
  }

  // switch to a new effect every cycleTime milliseconds
  if (currentMillis - cycleMillis > CYCLETIME && autoCycle == true) {
    cycleMillis = currentMillis; 
    if (++currentEffect >= numEffects) currentEffect = 0; // loop to start of effect list
    effectInit = false; // trigger effect initialization when new effect is selected
  }
  
  // run the currently selected effect every effectDelay milliseconds
  if (currentMillis - effectMillis > effectDelay) {
    effectMillis = currentMillis;
    effectList[currentEffect](); // run the selected effect function
  }


}
