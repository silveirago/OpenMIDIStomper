#ifndef MESSAGE_CONFIGURATIONS_H
#define MESSAGE_CONFIGURATIONS_H

#include "ButtonDefinitions.h"
#include "MIDIMessages.h"


// Function to initialize message arrays
void configureMessages() {
  for (int bank = 0; bank < N_BANKS; bank++) {
    for (int button = 0; button < N_BUTTONS; button++) {
      for (int message = 0; message < N_MESSAGES; message++) {
        MESSAGE_TYPE[bank][button][message] = NN;
        MESSAGE_VAL[bank][button][message] = (message == 0) ? button : -1;
        MESSAGE_CH[bank][button][message] = 0;
      }
    }
  }
  MidiMessages();
}



#endif
