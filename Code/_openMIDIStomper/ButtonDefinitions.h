#ifndef BUTTON_DEFINITIONS_H
#define BUTTON_DEFINITIONS_H

#include <MIDIUSB.h>

// Define the number of rows, columns, buttons, banks, and messages
#define NUM_ROWS 2
#define NUM_COLS 4
#define N_BUTTONS (NUM_COLS * NUM_ROWS)
#define N_BANKS 8
#define N_MESSAGES 10

byte rowPins[NUM_ROWS] = { 15, 14 };          // Pins for rows
byte colPins[NUM_COLS] = { 21, 20, 19, 16 };  // Pins for columns

// LEDs
#define N_LEDS 8
byte ledPin[N_LEDS] = { 2, 3, 4, 5, 6, 7, 8, 9 };  // Pins for LEDs

// Debounce delay in milliseconds
#define DEBOUNCE_DELAY 10

// Long press configuration
#define LONG_PRESS_DURATION 1000  // 1 second

// Current bank
int currentBank = 0;  // -1 indicates no bank is currently selected

// MIDI message types
enum MidiMessageType {
  NN,
  CC,
  PC,
  T
};

// MIDI message settings for buttons
byte MESSAGE_TYPE[N_BANKS][N_BUTTONS][N_MESSAGES];
byte MESSAGE_VAL[N_BANKS][N_BUTTONS][N_MESSAGES];
byte MESSAGE_CH[N_BANKS][N_BUTTONS][N_MESSAGES];

// Button states
byte buttonCState[N_BUTTONS] = { 0 };
byte buttonPState[N_BUTTONS] = { 0 };
unsigned long buttonPressStartTime[N_BUTTONS] = { 0 };
boolean longPressDetected[N_BUTTONS] = { false };
byte toggleVelocity[N_BUTTONS] = { 0 };

boolean isAnyButtonPressed = false;

// Initialize the MESSAGE_TYPE, MESSAGE_VAL, and MESSAGE_CH arrays with specific values
void initMessageArrays() {
  for (int bank = 0; bank < N_BANKS; bank++) {
    for (int btn = 0; btn < N_BUTTONS; btn++) {
      for (int msg = 0; msg < N_MESSAGES; msg++) {
        MESSAGE_TYPE[bank][btn][msg] = NN;
        MESSAGE_VAL[bank][btn][msg] = -1;
        MESSAGE_CH[bank][btn][msg] = 0;
      }
    }
  }
}


#endif
