#ifndef MIDI_OUT_H
#define MIDI_OUT_H

#include "ButtonDefinitions.h"

// Forward declarations
void noteOn(byte channel, byte pitch, byte velocity);
void noteOff(byte channel, byte pitch, byte velocity);
void controlChange(byte channel, byte control, byte value);
void programChange(byte channel, byte program);

void sendMidiMsg(int buttonIndex) {
  for (int i = 0; i < N_MESSAGES; i++) {
    if (MESSAGE_VAL[currentBank][buttonIndex][i] >= 0 && MESSAGE_VAL[currentBank][buttonIndex][i] < 128) {
      byte channel = MESSAGE_CH[currentBank][buttonIndex][i];
      switch (MESSAGE_TYPE[currentBank][buttonIndex][i]) {
        case NN:
          noteOn(channel, MESSAGE_VAL[currentBank][buttonIndex][i], 127);
          noteOff(channel, MESSAGE_VAL[currentBank][buttonIndex][i], 0);
          MidiUSB.flush();
          break;
        case PC:
          programChange(channel, MESSAGE_VAL[currentBank][buttonIndex][i]);
          MidiUSB.flush();
          break;
        case CC:
          controlChange(channel, MESSAGE_VAL[currentBank][buttonIndex][i], 127);
          controlChange(channel, MESSAGE_VAL[currentBank][buttonIndex][i], 0);
          MidiUSB.flush();
          break;
        case T:
          if (buttonCState[buttonIndex] == LOW) {
            toggleVelocity[buttonIndex] = !toggleVelocity[buttonIndex] * 127;
            noteOn(channel, MESSAGE_VAL[currentBank][buttonIndex][i], toggleVelocity[buttonIndex]);
            MidiUSB.flush();
          }
          break;
      }
    }
  }
}

void noteOn(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOn = {0x09, (uint8_t)(0x90 | channel), pitch, velocity};
  MidiUSB.sendMIDI(noteOn);
}

void noteOff(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOff = {0x08, (uint8_t)(0x80 | channel), pitch, velocity};
  MidiUSB.sendMIDI(noteOff);
}

void controlChange(byte channel, byte control, byte value) {
  midiEventPacket_t event = {0x0B, (uint8_t)(0xB0 | channel), control, value};
  MidiUSB.sendMIDI(event);
}

void programChange(byte channel, byte program) {
  midiEventPacket_t message = {0x0C, (uint8_t)(0xC0 | channel), program, 0};
  MidiUSB.sendMIDI(message);
}

#endif
