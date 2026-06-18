// =============================================================================
//  SimpleMIDIMatrix.ino
//  A minimal, self-contained MIDI firmware for the OpenMIDIStomper hardware.
//
//  Hardware: Arduino Pro Micro (ATmega32U4)
//  MIDI lib : MIDIUSB (built-in for 32U4 boards)
//
//  Matrix  : 2 rows × 4 columns = 8 buttons
//  LEDs    : 8 individual LEDs, one per button (active HIGH)
//  Buttons : active LOW (INPUT_PULLUP on columns; rows driven LOW to scan)
//
//  Behaviour:
//    - Button press  → Note On  (velocity 127)
//    - Button release → Note Off (velocity 0)
//    - Incoming Note On  (vel > 0) → turns matching LED on
//    - Incoming Note Off (vel = 0) → turns matching LED off
//    - Local LED mirrors button state immediately on press/release
//
//  Isolation:
//    This file is completely self-contained. It shares no headers,
//    no globals, and no state with the original firmware in
//    Code/_openMIDIStomper/ or Code/_openMIDIStomper_2/.
//    Opening this folder in the Arduino IDE loads only this sketch.
// =============================================================================

#include <MIDIUSB.h>

// ---------------------------------------------------------------------------
//  USER-CONFIGURABLE SECTION
//  Change pins, notes, channel, and debounce delay here.
// ---------------------------------------------------------------------------

// MIDI channel (1–16). The library uses 0-based internally, so we subtract 1
// when sending. Leave MIDI_CHANNEL as a human-readable 1-based value.
const byte MIDI_CHANNEL = 1;

// Button matrix pins
//   rowPins  : set OUTPUT, driven LOW one at a time to activate a row.
//   colPins  : set INPUT_PULLUP; reads LOW when button in active row is pressed.
const byte rowPins[2] = { 15, 14 };           // Row 0 = pin 15, Row 1 = pin 14
const byte colPins[4] = { 21, 20, 19, 16 };   // Col 0..3

// LED pins (one per button, active HIGH).
// Button index = row * 4 + col  →  same order as the note table below.
//   Button 1 (row0,col0) → ledPins[0] = pin 2
//   Button 2 (row0,col1) → ledPins[1] = pin 3
//   ...
//   Button 8 (row1,col3) → ledPins[7] = pin 9
const byte ledPins[8] = { 2, 3, 4, 5, 6, 7, 8, 9 };

// MIDI note numbers, one per button (index 0..7 matches button 1..8).
const byte noteNumbers[8] = { 36, 37, 38, 39, 40, 41, 42, 43 };

// Debounce delay in milliseconds.
const unsigned long DEBOUNCE_MS = 10;

// ---------------------------------------------------------------------------
//  INTERNAL STATE  (do not edit unless you know what you're doing)
// ---------------------------------------------------------------------------

// Current and previous debounced button states (1 = pressed, 0 = released).
byte buttonCurrentState[8]  = { 0 };
byte buttonPreviousState[8] = { 0 };

// Timestamp of the last state change per button, for debounce.
unsigned long lastDebounceTime[8] = { 0 };

// Raw reading before debounce (updated every scan, compared after DEBOUNCE_MS).
byte rawState[8] = { 0 };

// LED state: 1 = on, 0 = off.  Tracks both local presses and MIDI feedback.
byte ledState[8] = { 0 };

// ---------------------------------------------------------------------------
//  SETUP
// ---------------------------------------------------------------------------

void setup() {
  // Row pins: OUTPUT, idle HIGH (a row is activated by pulling it LOW).
  for (int r = 0; r < 2; r++) {
    pinMode(rowPins[r], OUTPUT);
    digitalWrite(rowPins[r], HIGH);
  }

  // Column pins: INPUT_PULLUP (reads LOW when button connects col to active row).
  for (int c = 0; c < 4; c++) {
    pinMode(colPins[c], INPUT_PULLUP);
  }

  // LED pins: OUTPUT, all off initially.
  for (int i = 0; i < 8; i++) {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW);
  }
}

// ---------------------------------------------------------------------------
//  MAIN LOOP
// ---------------------------------------------------------------------------

void loop() {
  scanMatrix();       // Detect presses/releases → send MIDI, update LEDs.
  readMidiInput();    // Handle incoming MIDI → update LEDs.
}

// ---------------------------------------------------------------------------
//  MATRIX SCAN WITH DEBOUNCE AND EDGE DETECTION
// ---------------------------------------------------------------------------

void scanMatrix() {
  for (int row = 0; row < 2; row++) {
    // Activate this row.
    digitalWrite(rowPins[row], LOW);

    for (int col = 0; col < 4; col++) {
      int idx = row * 4 + col;          // Button index 0..7
      byte reading = (digitalRead(colPins[col]) == LOW) ? 1 : 0;

      // Debounce: only accept a change if the raw reading has been stable
      // for at least DEBOUNCE_MS milliseconds.
      if (reading != rawState[idx]) {
        // The pin changed; restart the debounce timer.
        lastDebounceTime[idx] = millis();
        rawState[idx] = reading;
      }

      if ((millis() - lastDebounceTime[idx]) >= DEBOUNCE_MS) {
        // The reading is stable. Check for edge (state change).
        if (reading != buttonCurrentState[idx]) {
          buttonCurrentState[idx] = reading;

          if (reading == 1) {
            // --- PRESS EDGE ---
            sendNoteOn(noteNumbers[idx]);
            setLED(idx, 1);
          } else {
            // --- RELEASE EDGE ---
            sendNoteOff(noteNumbers[idx]);
            setLED(idx, 0);
          }
        }
      }
    }

    // Deactivate this row before moving to the next.
    digitalWrite(rowPins[row], HIGH);
  }
}

// ---------------------------------------------------------------------------
//  INCOMING MIDI HANDLER (MIDI feedback → LED control)
// ---------------------------------------------------------------------------

void readMidiInput() {
  midiEventPacket_t rx;

  // Drain all available MIDI packets in this loop iteration.
  do {
    rx = MidiUSB.read();

    if (rx.header != 0) {
      byte statusByte = rx.byte1;
      byte msgType    = statusByte & 0xF0;  // Upper nibble: message type
      byte channel    = statusByte & 0x0F;  // Lower nibble: channel (0-based)
      byte note       = rx.byte2;
      byte velocity   = rx.byte3;

      // We only care about our MIDI channel.
      // MIDI_CHANNEL is 1-based; channel from packet is 0-based.
      if (channel != (MIDI_CHANNEL - 1)) continue;

      if (msgType == 0x90) {
        // Note On message.
        if (velocity > 0) {
          // Genuine Note On: find matching button and turn LED on.
          setLEDForNote(note, 1);
        } else {
          // Note On with velocity 0 is equivalent to Note Off.
          setLEDForNote(note, 0);
        }
      } else if (msgType == 0x80) {
        // Note Off message.
        setLEDForNote(note, 0);
      }
    }
  } while (rx.header != 0);
}

// ---------------------------------------------------------------------------
//  MIDI OUTPUT HELPERS
// ---------------------------------------------------------------------------

// Send Note On: channel is 0-based inside the packet.
void sendNoteOn(byte note) {
  midiEventPacket_t pkt = {
    0x09,
    (uint8_t)(0x90 | (MIDI_CHANNEL - 1)),
    note,
    127
  };
  MidiUSB.sendMIDI(pkt);
  MidiUSB.flush();
}

// Send Note Off: velocity 0, channel is 0-based inside the packet.
void sendNoteOff(byte note) {
  midiEventPacket_t pkt = {
    0x08,
    (uint8_t)(0x80 | (MIDI_CHANNEL - 1)),
    note,
    0
  };
  MidiUSB.sendMIDI(pkt);
  MidiUSB.flush();
}

// ---------------------------------------------------------------------------
//  LED HELPERS
// ---------------------------------------------------------------------------

// Set the LED for button index idx to state (1 = on, 0 = off).
void setLED(int idx, byte state) {
  ledState[idx] = state;
  digitalWrite(ledPins[idx], state ? HIGH : LOW);
}

// Search through noteNumbers[] for a matching note and update its LED.
void setLEDForNote(byte note, byte state) {
  for (int i = 0; i < 8; i++) {
    if (noteNumbers[i] == note) {
      setLED(i, state);
    }
  }
}
