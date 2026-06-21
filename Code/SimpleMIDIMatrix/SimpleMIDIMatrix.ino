// =============================================================================
//  SimpleMIDIMatrix.ino
//  Minimal MIDI matrix firmware for OpenMIDIStomper hardware.
//
//  Default behavior:
//    - 8 buttons send Control Change messages
//    - MIDI Channel 16
//    - CC numbers 102–109
//
//  Note support is still kept in the code.
// =============================================================================

#include <MIDIUSB.h>

// ---------------------------------------------------------------------------
//  USER-CONFIGURABLE SECTION
// ---------------------------------------------------------------------------

const byte MIDI_CHANNEL = 16;

const byte rowPins[2] = { 15, 14 };
const byte colPins[4] = { 21, 20, 19, 16 };

const byte ledPins[8] = { 2, 3, 4, 5, 6, 7, 8, 9 };

enum MidiMessageType {
  MIDI_NOTE,
  MIDI_CC
};

// Default: all buttons send CC.
// To make a button send a note, change MIDI_CC to MIDI_NOTE for that index.
const MidiMessageType buttonTypes[8] = {
  MIDI_CC,
  MIDI_CC,
  MIDI_CC,
  MIDI_CC,
  MIDI_CC,
  MIDI_CC,
  MIDI_CC,
  MIDI_CC
};

// If buttonTypes[i] is MIDI_CC, this is the CC number.
// If buttonTypes[i] is MIDI_NOTE, this is the note number.
const byte midiNumbers[8] = {
  102, 103, 104, 105,
  106, 107, 108, 109
};

const byte NOTE_ON_VELOCITY = 127;

const byte CC_ON_VALUE = 127;
const byte CC_OFF_VALUE = 0;

const unsigned long DEBOUNCE_MS = 10;

// ---------------------------------------------------------------------------
//  INTERNAL STATE
// ---------------------------------------------------------------------------

byte buttonCurrentState[8] = { 0 };
unsigned long lastDebounceTime[8] = { 0 };
byte rawState[8] = { 0 };
byte ledState[8] = { 0 };

// ---------------------------------------------------------------------------
//  SETUP
// ---------------------------------------------------------------------------

void setup() {
  for (int r = 0; r < 2; r++) {
    pinMode(rowPins[r], OUTPUT);
    digitalWrite(rowPins[r], HIGH);
  }

  for (int c = 0; c < 4; c++) {
    pinMode(colPins[c], INPUT_PULLUP);
  }

  for (int i = 0; i < 8; i++) {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW);
  }
}

// ---------------------------------------------------------------------------
//  MAIN LOOP
// ---------------------------------------------------------------------------

void loop() {
  scanMatrix();
  readMidiInput();
}

// ---------------------------------------------------------------------------
//  MATRIX SCAN WITH DEBOUNCE AND EDGE DETECTION
// ---------------------------------------------------------------------------

void scanMatrix() {
  for (int row = 0; row < 2; row++) {
    digitalWrite(rowPins[row], LOW);

    for (int col = 0; col < 4; col++) {
      int idx = row * 4 + col;
      byte reading = (digitalRead(colPins[col]) == LOW) ? 1 : 0;

      if (reading != rawState[idx]) {
        lastDebounceTime[idx] = millis();
        rawState[idx] = reading;
      }

      if ((millis() - lastDebounceTime[idx]) >= DEBOUNCE_MS) {
        if (reading != buttonCurrentState[idx]) {
          buttonCurrentState[idx] = reading;

          if (reading == 1) {
            sendButtonOn(idx);
            setLED(idx, 1);
          } else {
            sendButtonOff(idx);
            setLED(idx, 0);
          }
        }
      }
    }

    digitalWrite(rowPins[row], HIGH);
  }
}

// ---------------------------------------------------------------------------
//  INCOMING MIDI HANDLER
// ---------------------------------------------------------------------------

void readMidiInput() {
  midiEventPacket_t rx;

  do {
    rx = MidiUSB.read();

    if (rx.header != 0) {
      byte statusByte = rx.byte1;
      byte msgType = statusByte & 0xF0;
      byte channel = statusByte & 0x0F;
      byte number = rx.byte2;
      byte value = rx.byte3;

      if (channel != (MIDI_CHANNEL - 1)) continue;

      if (msgType == 0x90) {
        setLEDForMidiMessage(MIDI_NOTE, number, value > 0 ? 1 : 0);
      } else if (msgType == 0x80) {
        setLEDForMidiMessage(MIDI_NOTE, number, 0);
      } else if (msgType == 0xB0) {
        setLEDForMidiMessage(MIDI_CC, number, value > 0 ? 1 : 0);
      }
    }
  } while (rx.header != 0);
}

// ---------------------------------------------------------------------------
//  BUTTON MIDI DISPATCH
// ---------------------------------------------------------------------------

void sendButtonOn(int idx) {
  if (buttonTypes[idx] == MIDI_NOTE) {
    sendNoteOn(midiNumbers[idx]);
  } else {
    sendControlChange(midiNumbers[idx], CC_ON_VALUE);
  }
}

void sendButtonOff(int idx) {
  if (buttonTypes[idx] == MIDI_NOTE) {
    sendNoteOff(midiNumbers[idx]);
  } else {
    sendControlChange(midiNumbers[idx], CC_OFF_VALUE);
  }
}

// ---------------------------------------------------------------------------
//  MIDI OUTPUT HELPERS
// ---------------------------------------------------------------------------

void sendNoteOn(byte note) {
  midiEventPacket_t pkt = {
    0x09,
    (uint8_t)(0x90 | (MIDI_CHANNEL - 1)),
    note,
    NOTE_ON_VELOCITY
  };

  MidiUSB.sendMIDI(pkt);
  MidiUSB.flush();
}

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

void sendControlChange(byte ccNumber, byte value) {
  midiEventPacket_t pkt = {
    0x0B,
    (uint8_t)(0xB0 | (MIDI_CHANNEL - 1)),
    ccNumber,
    value
  };

  MidiUSB.sendMIDI(pkt);
  MidiUSB.flush();
}

// ---------------------------------------------------------------------------
//  LED HELPERS
// ---------------------------------------------------------------------------

void setLED(int idx, byte state) {
  ledState[idx] = state;
  digitalWrite(ledPins[idx], state ? HIGH : LOW);
}

void setLEDForMidiMessage(MidiMessageType type, byte number, byte state) {
  for (int i = 0; i < 8; i++) {
    if (buttonTypes[i] == type && midiNumbers[i] == number) {
      setLED(i, state);
    }
  }
}