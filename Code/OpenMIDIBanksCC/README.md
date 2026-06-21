# OpenMIDIBanksCC

Standalone OpenMIDIStomper firmware variant that preserves the original bank, button, LED, and matrix interaction model while changing only the outgoing MIDI assignments.

## Behavior Summary

- Short press + release sends one CC with value `127`.
- The button action uses the bank that was active when the press started.
- Long press for 1 second selects the corresponding bank.
- Long-press bank selection suppresses the button action.
- Idle LEDs show the active bank exactly like the original firmware.
- `A0` is an optional compile-time expression pedal input.
- `A10` is an optional compile-time sustain pedal input.
- MIDI channel is `16`.

## Hardware Reference

- Rows: `15, 14`
- Columns: `21, 20, 19, 16`
- LEDs: `2, 3, 4, 5, 6, 7, 8, 9`
- Expression: `A0`
- Sustain: optional `A10`
- LEDs are active-high.

Buttons are scanned as a `2x4` matrix with row pins driven `HIGH` at idle and pulled `LOW` one at a time during scan. Column pins use `INPUT_PULLUP`, so a pressed switch reads `LOW`.

## CC Table

| Bank | Buttons | A0 |
|---|---|---|
| 1 | `56, 57, 58, 59, 60, 61, 62, 63` | `64` |
| 2 | `65, 66, 67, 68, 69, 70, 71, 72` | `73` |
| 3 | `74, 75, 76, 77, 78, 79, 80, 81` | `82` |
| 4 | `83, 84, 85, 86, 87, 88, 89, 90` | `91` |
| 5 | `92, 93, 94, 95, 96, 97, 98, 99` | `100` |
| 6 | `101, 102, 103, 104, 105, 106, 107, 108` | `109` |
| 7 | `110, 111, 112, 113, 114, 115, 116, 117` | `118` |
| 8 | `119, 120, 121, 122, 123, 124, 125, 126` | `127` |

## Compile-Time Pedal Options

At the top of the sketch:

```cpp
#define ENABLE_EXPRESSION_A0  1
#define ENABLE_SUSTAIN_A10    1
```

- Enable A0: set `#define ENABLE_EXPRESSION_A0  1`
- Disable A0: set `#define ENABLE_EXPRESSION_A0  0`
- Enable sustain on A10: set `#define ENABLE_SUSTAIN_A10    1`
- Disable sustain on A10: set `#define ENABLE_SUSTAIN_A10    0`

These are compile-time options implemented with preprocessor conditionals. When a pedal feature is disabled, its state variables, reads, processing, and MIDI output are excluded from the compiled firmware, which reduces firmware size.

## Sustain Pedal (A10)

- Optional compile-time feature
- Uses `INPUT_PULLUP`
- Pedal connected between `A10` and `GND`
- Sends `CC 47`
- Press = `127`
- Release = `0`
- One message on press, one on release, with no repeats while held
- Independent from banks
- Independent from the `A0` expression pedal
- Independent from the button matrix

Verification targets:

- Press pedal -> `CC47` value `127`
- Hold pedal -> no additional messages
- Release pedal -> `CC47` value `0`
- Bank changes do not affect sustain behavior
- `A0` expression remains unchanged
- Original button-bank behavior remains unchanged

## Upload / Compile

Required library:

- `MIDIUSB`

Target board:

- SparkFun Pro Micro 5V/16MHz

CLI steps:

```sh
arduino-cli core install SparkFun:avr \
  --additional-urls https://raw.githubusercontent.com/sparkfun/Arduino_Boards/master/IDE_Board_Manager/package_sparkfun_index.json
arduino-cli compile --fqbn SparkFun:avr:promicro:cpu=16MHzatmega32U4 \
  --additional-urls https://raw.githubusercontent.com/sparkfun/Arduino_Boards/master/IDE_Board_Manager/package_sparkfun_index.json \
  Code/OpenMIDIBanksCC
```

## Isolation Statement

- No files in `Code/_openMIDIStomper/` were modified.
- No files in `Code/SimpleMIDIMatrix/` were modified by this variant.
- This firmware lives entirely inside `Code/OpenMIDIBanksCC/`.
