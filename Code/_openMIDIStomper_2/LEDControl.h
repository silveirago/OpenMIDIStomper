#ifndef LED_CONTROL_H
#define LED_CONTROL_H

#include "ButtonDefinitions.h"

// Initialize LED pins
void setupLEDs() {
  for (int i = 0; i < N_LEDS; i++) {
    pinMode(ledPin[i], OUTPUT);
    digitalWrite(ledPin[i], LOW);  // Turn off all LEDs initially
  }
}

// Update LEDs based on the current bank
void updateLEDs() {
  for (int i = 0; i < N_LEDS; i++) {
    digitalWrite(ledPin[i], LOW);
  }
  if (currentBank >= 0 && currentBank < N_LEDS) {
    digitalWrite(ledPin[currentBank], HIGH);
  }
}

// Handle LED states
void handleLEDs() {
  updateLEDs();
}

#endif
