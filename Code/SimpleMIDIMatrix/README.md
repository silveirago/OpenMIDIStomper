# SimpleMIDIMatrix

A minimal, self-contained MIDI firmware for the **OpenMIDIStomper** hardware
(Arduino Pro Micro, ATmega32U4).  
It lives alongside the original firmware without touching it.

---

## What it does

| Event | Action |
|---|---|
| Button pressed | Sends **Note On** (velocity 127) |
| Button released | Sends **Note Off** (velocity 0) |
| Incoming Note On (vel > 0) | Turns the matching **LED on** |
| Incoming Note Off / Note On vel 0 | Turns the matching **LED off** |

Each button sends exactly **one** Note On on press and **one** Note Off on release.  
LEDs reflect both local presses and incoming MIDI feedback from a DAW/host.

---

## Default MIDI mapping

| Button | Row | Col | Arduino pin (LED) | MIDI Note |
|--------|-----|-----|-------------------|-----------|
| 1 | 0 | 0 | 2 | 36 (C2) |
| 2 | 0 | 1 | 3 | 37 (C#2) |
| 3 | 0 | 2 | 4 | 38 (D2) |
| 4 | 0 | 3 | 5 | 39 (D#2) |
| 5 | 1 | 0 | 6 | 40 (E2) |
| 6 | 1 | 1 | 7 | 41 (F2) |
| 7 | 1 | 2 | 8 | 42 (F#2) |
| 8 | 1 | 3 | 9 | 43 (G2) |

MIDI channel: **1**

---

## Hardware pin reference

```
Row pins  (OUTPUT, driven LOW to scan) : 15, 14
Col pins  (INPUT_PULLUP, reads LOW)    : 21, 20, 19, 16
LED pins  (OUTPUT, active HIGH)        : 2, 3, 4, 5, 6, 7, 8, 9
```

Buttons are wired between a row pin and a column pin — no external pull-up
resistors needed. LEDs have their anode connected to the LED pin through a
220 Ω resistor; cathode to GND.

---

## How to upload

1. Open **Arduino IDE**.
2. **File → Open** → navigate to `Code/SimpleMIDIMatrix/SimpleMIDIMatrix.ino`.
3. **Tools → Board** → select **Arduino Leonardo** (or Pro Micro if you have the
   SparkFun board package installed).
4. **Tools → Port** → choose the COM/serial port for your Pro Micro.
5. Click **Upload** (→).

> ⚠️ Do **not** open the original `_openMIDIStomper.ino` at the same time.  
> Each sketch lives in its own folder and is completely independent.

---

## How to change MIDI notes

Open `SimpleMIDIMatrix.ino` and find the `noteNumbers` array near the top of
the **USER-CONFIGURABLE SECTION**:

```cpp
const byte noteNumbers[8] = { 36, 37, 38, 39, 40, 41, 42, 43 };
//                            Btn1 Btn2 Btn3 Btn4 Btn5 Btn6 Btn7 Btn8
```

Replace any value with the desired MIDI note number (0–127) and re-upload.

---

## How to change the MIDI channel

Find the `MIDI_CHANNEL` constant:

```cpp
const byte MIDI_CHANNEL = 1;   // 1–16
```

Change `1` to any value from `1` to `16` and re-upload.

---

## How to change pins

### Row pins
```cpp
const byte rowPins[2] = { 15, 14 };
//                         Row0 Row1
```

### Column pins
```cpp
const byte colPins[4] = { 21, 20, 19, 16 };
//                         Col0 Col1 Col2 Col3
```

### LED pins
```cpp
const byte ledPins[8] = { 2, 3, 4, 5, 6, 7, 8, 9 };
//                        Btn1 ... Btn8
```

Button index order is `row * 4 + col`, so:
- Index 0 = Row 0, Col 0 (Button 1)
- Index 4 = Row 1, Col 0 (Button 5)

---

## How to change debounce time

```cpp
const unsigned long DEBOUNCE_MS = 10;  // milliseconds
```

Increase this value if you get false double-triggers on noisy switches.

---

## Isolation from the original firmware

This sketch is **completely independent**:

- It has no `#include` of any header from `_openMIDIStomper/` or
  `_openMIDIStomper_2/`.
- It declares its own variables and functions; nothing is shared.
- The Arduino IDE treats each folder as a separate project. Opening
  `SimpleMIDIMatrix.ino` loads **only** this sketch.
- The original firmware files are unchanged.

---

## Required library

**MIDIUSB** — included with the Arduino AVR boards package for 32U4-based
boards (Leonardo, Pro Micro).  
Install via **Tools → Manage Libraries → search "MIDIUSB"** if not present.

---

*Created by Gustavo Silveira / Nerd Musician — simplified variant of
OpenMIDIStomper.*
