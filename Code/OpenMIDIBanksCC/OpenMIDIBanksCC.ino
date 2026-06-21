#include <MIDIUSB.h>

//#define ENABLE_EXPRESSION_A0  1
#define ENABLE_SUSTAIN_A10    1

const byte MIDI_CHANNEL = 16;

const byte NUM_ROWS = 2;
const byte NUM_COLS = 4;
const byte N_BUTTONS = 8;
const byte N_BANKS = 8;
const byte N_LEDS = 8;

const byte rowPins[NUM_ROWS] = { 15, 14 };
const byte colPins[NUM_COLS] = { 21, 20, 19, 16 };
const byte ledPins[N_LEDS] = { 2, 3, 4, 5, 6, 7, 8, 9 };

const unsigned long DEBOUNCE_DELAY = 10;
const unsigned long LONG_PRESS_DURATION = 1000;
const int ANALOG_JITTER_THRESHOLD = 4;
const unsigned long ANALOG_ACTIVITY_TIMEOUT = 300;
const unsigned long SUSTAIN_DEBOUNCE_MS = 20;
const byte SUSTAIN_CC = 47;

const byte buttonCCs[N_BANKS][N_BUTTONS] = {
  { 56, 57, 58, 59, 60, 61, 62, 63 },
  { 65, 66, 67, 68, 69, 70, 71, 72 },
  { 74, 75, 76, 77, 78, 79, 80, 81 },
  { 83, 84, 85, 86, 87, 88, 89, 90 },
  { 92, 93, 94, 95, 96, 97, 98, 99 },
  { 101, 102, 103, 104, 105, 106, 107, 108 },
  { 110, 111, 112, 113, 114, 115, 116, 117 },
  { 119, 120, 121, 122, 123, 124, 125, 126 }
};

const byte expressionCCs[N_BANKS] = { 64, 73, 82, 91, 100, 109, 118, 127 };

int currentBank = 0;
byte buttonCState[N_BUTTONS] = { 0 };
byte buttonPState[N_BUTTONS] = { 0 };
int buttonBankAtPress[N_BUTTONS] = { 0 };
unsigned long buttonPressStartTime[N_BUTTONS] = { 0 };
bool longPressDetected[N_BUTTONS] = { false };
bool isAnyButtonPressed = false;
unsigned long lastDebounceTime[N_BUTTONS] = { 0 };

#if ENABLE_EXPRESSION_A0
const byte expressionA0Pin = A0;
int lastAnalogRaw = 0;
byte lastAnalogMidi = 255;
unsigned long analogMoveTimestamp = 0;
#endif

#if ENABLE_SUSTAIN_A10
const byte sustainPin = A10;
bool sustainState = HIGH;
bool sustainLastReading = HIGH;
unsigned long sustainLastDebounceTime = 0;
#endif

void updateLEDs();
void scanButtonMatrix();
void handleButtonState(byte index);
void sendControlChange(byte control, byte value);

#if ENABLE_EXPRESSION_A0
void handleExpressionA0();
#endif

#if ENABLE_SUSTAIN_A10
void handleSustainPedal();
#endif

void setup() {
  for (byte row = 0; row < NUM_ROWS; ++row) {
    pinMode(rowPins[row], OUTPUT);
    digitalWrite(rowPins[row], HIGH);
  }

  for (byte col = 0; col < NUM_COLS; ++col) {
    pinMode(colPins[col], INPUT_PULLUP);
  }

  for (byte led = 0; led < N_LEDS; ++led) {
    pinMode(ledPins[led], OUTPUT);
    digitalWrite(ledPins[led], LOW);
  }

#if ENABLE_EXPRESSION_A0
  lastAnalogRaw = analogRead(expressionA0Pin);
  lastAnalogMidi = map(lastAnalogRaw, 0, 1023, 0, 127);
  analogMoveTimestamp = millis();
#endif

#if ENABLE_SUSTAIN_A10
  pinMode(sustainPin, INPUT_PULLUP);
#endif

  updateLEDs();
}

void loop() {
  scanButtonMatrix();

  if (!isAnyButtonPressed) {
    updateLEDs();
  }

#if ENABLE_EXPRESSION_A0
  handleExpressionA0();
#endif

#if ENABLE_SUSTAIN_A10
  handleSustainPedal();
#endif
}

void scanButtonMatrix() {
  for (byte row = 0; row < NUM_ROWS; ++row) {
    digitalWrite(rowPins[row], LOW);

    for (byte col = 0; col < NUM_COLS; ++col) {
      byte index = row * NUM_COLS + col;

      if (digitalRead(colPins[col]) == LOW) {
        buttonCState[index] = 1;

        if (buttonPState[index] == 0) {
          buttonPressStartTime[index] = millis();
          longPressDetected[index] = false;
          buttonBankAtPress[index] = currentBank;
        }
      } else {
        buttonCState[index] = 0;
      }

      handleButtonState(index);
    }

    digitalWrite(rowPins[row], HIGH);
  }

  bool anyPressed = false;
  for (byte index = 0; index < N_BUTTONS; ++index) {
    if (buttonPState[index] > 0) {
      anyPressed = true;
      break;
    }
  }
  isAnyButtonPressed = anyPressed;
}

void handleButtonState(byte index) {
  unsigned long now = millis();
  if ((now - lastDebounceTime[index]) <= DEBOUNCE_DELAY) {
    return;
  }

  lastDebounceTime[index] = now;

  if (buttonPState[index] != buttonCState[index]) {
    if (buttonCState[index] > 0) {
      digitalWrite(ledPins[index], HIGH);
    } else {
      digitalWrite(ledPins[index], LOW);

      if (!longPressDetected[index]) {
        sendControlChange(buttonCCs[buttonBankAtPress[index]][index], 127);
      }
    }

    buttonPState[index] = buttonCState[index];
  } else if (buttonCState[index] > 0 && !longPressDetected[index]) {
    if ((now - buttonPressStartTime[index]) >= LONG_PRESS_DURATION) {
      longPressDetected[index] = true;
      currentBank = index;
      updateLEDs();
    }
  }
}

void updateLEDs() {
  for (byte led = 0; led < N_LEDS; ++led) {
    digitalWrite(ledPins[led], LOW);
  }

  if (currentBank >= 0 && currentBank < N_LEDS) {
    digitalWrite(ledPins[currentBank], HIGH);
  }
}

#if ENABLE_EXPRESSION_A0
void handleExpressionA0() {
  unsigned long now = millis();
  int raw = analogRead(expressionA0Pin);
  int rawDelta = abs(raw - lastAnalogRaw);

  if (rawDelta > ANALOG_JITTER_THRESHOLD) {
    analogMoveTimestamp = now;
    lastAnalogRaw = raw;
  }

  byte midiValue = map(raw, 0, 1023, 0, 127);

  if ((now - analogMoveTimestamp) < ANALOG_ACTIVITY_TIMEOUT) {
    if (midiValue != lastAnalogMidi) {
      sendControlChange(expressionCCs[currentBank], midiValue);
      lastAnalogMidi = midiValue;
    }
  }
}
#endif

#if ENABLE_SUSTAIN_A10
void handleSustainPedal() {
  unsigned long now = millis();
  bool reading = digitalRead(sustainPin);

  if (reading != sustainLastReading) {
    sustainLastDebounceTime = now;
    sustainLastReading = reading;
  }

  if ((now - sustainLastDebounceTime) >= SUSTAIN_DEBOUNCE_MS) {
    if (reading != sustainState) {
      sustainState = reading;
      if (sustainState == LOW) {
        sendControlChange(SUSTAIN_CC, 127);
      } else {
        sendControlChange(SUSTAIN_CC, 0);
      }
    }
  }
}
#endif

void sendControlChange(byte control, byte value) {
  midiEventPacket_t event = {
    0x0B,
    (uint8_t)(0xB0 | ((MIDI_CHANNEL - 1) & 0x0F)),
    control,
    value
  };

  MidiUSB.sendMIDI(event);
  MidiUSB.flush();
}
