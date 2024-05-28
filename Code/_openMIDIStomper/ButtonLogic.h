#ifndef BUTTON_LOGIC_H
#define BUTTON_LOGIC_H

#include "ButtonDefinitions.h"
#include "MIDIOut.h"

// Button states
int btnStates[NUM_ROWS][NUM_COLS];

// Function declarations
void printbuttonCState(int index);
void scanButtonMatrix();

// Initialize button pins
void setupButtons() {
  for (int row = 0; row < NUM_ROWS; row++) {
    pinMode(rowPins[row], OUTPUT);
    digitalWrite(rowPins[row], HIGH);  // Set to HIGH initially
  }

  for (int col = 0; col < NUM_COLS; col++) {
    pinMode(colPins[col], INPUT_PULLUP);
  }
}

// Scan the button matrix
void scanButtonMatrix() {
  for (int row = 0; row < NUM_ROWS; row++) {
    digitalWrite(rowPins[row], LOW);  // Set the current row LOW
    for (int col = 0; col < NUM_COLS; col++) {
      byte buttonIndex = row * NUM_COLS + col;

      if (digitalRead(colPins[col]) == LOW) {
        // Button pressed
        btnStates[row][col] = 1;
        buttonCState[buttonIndex] = 1;

        // Record the start time if this is a new press
        if (buttonPState[buttonIndex] == 0) {
          buttonPressStartTime[buttonIndex] = millis();
          longPressDetected[buttonIndex] = false;  // Reset long press detection
        }
      } else {
        // Button released
        btnStates[row][col] = 0;
        buttonCState[buttonIndex] = 0;
      }

      printbuttonCState(buttonIndex);  // Print button state
    }
    digitalWrite(rowPins[row], HIGH);  // Reset the row pin to HIGH after checking
  }
}

// Print button current state and handle debouncing
void printbuttonCState(int index) {
  static unsigned long lastDebounceTime[N_BUTTONS] = { 0 };

  if ((millis() - lastDebounceTime[index]) > DEBOUNCE_DELAY) {
    lastDebounceTime[index] = millis();

    if (buttonPState[index] != buttonCState[index]) {
      if (buttonCState[index] > 0) {
        digitalWrite(ledPin[index], HIGH);

        isAnyButtonPressed = true;
        //Serial.println(isAnyButtonPressed);
      } else {
        digitalWrite(ledPin[index], LOW);
        // Only call sendMidiMsg if not a long press
        if (!longPressDetected[index]) {
          sendMidiMsg(index);
        }
        isAnyButtonPressed = false;
        //Serial.println(isAnyButtonPressed);
      }

      buttonPState[index] = buttonCState[index];
    } else if (buttonCState[index] > 0 && !longPressDetected[index]) {
      // Check for long press
      if ((millis() - buttonPressStartTime[index]) >= LONG_PRESS_DURATION) {
        longPressDetected[index] = true;
        currentBank = index;
        updateLEDs();
      }
    }
  }
}

#endif
