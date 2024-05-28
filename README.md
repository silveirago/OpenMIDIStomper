
# _openMIDIStomper

## Description
_openMIDIStomper is an Arduino-based MIDI controller that provides customizable MIDI message sending capabilities using buttons, potentiometers, and LEDs. This project is designed to offer flexible control over MIDI devices for musicians and hobbyists.

## Features
- **Button Control:** Send MIDI messages using button presses.
- **Potentiometer Control:** Adjust MIDI parameters with potentiometers.
- **LED Indicators:** Visual feedback using LEDs.
- **Customizable Messages:** Define your own MIDI messages and configurations.

## Getting Started

### Prerequisites
- Arduino IDE installed on your computer.
- Arduino board (compatible with the code).
- Necessary components:
  - Push buttons
  - Potentiometers
  - LEDs
  - Resistors
  - Breadboard and jumper wires

### Installation

1. **Clone the Repository:**
   ```sh
   git clone https://github.com/yourusername/_openMIDIStomper.git
   ```

2. **Open the Project:**
   Open the `_openMIDIStomper.ino` file in the Arduino IDE.

## Hardware Setup

### Components and Connections
1. **Buttons:**
   - Connect one terminal of each button in the first row to pin 15.
   - Connect one terminal of each button in the second row to pin 14.
   - Connect the other terminal of the first button in each column to pin 21.
   - Connect the other terminal of the second button in each column to pin 20.
   - Connect the other terminal of the third button in each column to pin 19.
   - Connect the other terminal of the fourth button in each column to pin 16.

2. **Potentiometers:**
   - Connect the middle terminal of each potentiometer to an analog input pin on the Arduino.
   - Connect the other two terminals to 5V and ground, respectively.

3. **LEDs:**
#### Pins Used
- **Pins**:  2, 3, 4, 5, 6, 7, 8, 9

#### Connecting LEDs

- **LED 1 (D1)**: Connect the anode to pin 2 through a 220Ω resistor and the cathode to ground.
- **LED 2 (D2)**: Connect the anode to pin 3 through a 220Ω resistor and the cathode to ground.
- **LED 3 (D3)**: Connect the anode to pin 4 through a 220Ω resistor and the cathode to ground.
- **LED 4 (D4)**: Connect the anode to pin 5 through a 220Ω resistor and the cathode to ground.
- **LED 5 (D5)**: Connect the anode to pin 6 through a 220Ω resistor and the cathode to ground.
- **LED 6 (D6)**: Connect the anode to pin 7 through a 220Ω resistor and the cathode to ground.
- **LED 7 (D7)**: Connect the anode to pin 8 through a 220Ω resistor and the cathode to ground.
- **LED 8 (D8)**: Connect the anode to pin 9 through a 220Ω resistor and the cathode to ground.

### Button Matrix

#### Pins Used
- **Rows**: Pins 15 and 14
- **Columns**: Pins 21, 20, 19, and 16

#### Wiring Diagram

- **Button 1 (B1)**: Connect one terminal to pin 15 (Row1) and the other terminal to pin 21 (Column1).
- **Button 2 (B2)**: Connect one terminal to pin 15 (Row1) and the other terminal to pin 20 (Column2).
- **Button 3 (B3)**: Connect one terminal to pin 15 (Row1) and the other terminal to pin 19 (Column3).
- **Button 4 (B4)**: Connect one terminal to pin 15 (Row1) and the other terminal to pin 16 (Column4).

- **Button 5 (B5)**: Connect one terminal to pin 14 (Row2) and the other terminal to pin 21 (Column1).
- **Button 6 (B6)**: Connect one terminal to pin 14 (Row2) and the other terminal to pin 20 (Column2).
- **Button 7 (B7)**: Connect one terminal to pin 14 (Row2) and the other terminal to pin 19 (Column3).
- **Button 8 (B8)**: Connect one terminal to pin 14 (Row2) and the other terminal to pin 16 (Column4).

### Software Configuration

1. **Button Definitions:**
   - Modify `ButtonDefinitions.h` to define the pins and settings for your buttons.

2. **Button Logic:**
   - Implement the logic for button presses in `ButtonLogic.h`.

3. **LED Control:**
   - Adjust the LED control logic in `LEDControl.h`.

4. **MIDI Messages:**
   - Configure the MIDI messages to be sent in `MIDIMessages.h`.

5. **MIDI Out:**
   - Set up the MIDI output in `MIDIOut.h`.

6. **Potentiometer Control:**
   - Define how potentiometer inputs affect MIDI messages in `PotentiometerControl.h`.

## Usage

1. **Upload the Code:**
   - Connect your Arduino board to your computer.
   - Select the appropriate board and port in the Arduino IDE.
   - Upload the `_openMIDIStomper.ino` sketch to the board.

2. **Connect to MIDI Device:**
   - Use a MIDI interface to connect the Arduino to your MIDI device or computer.

3. **Operate:**
   - Press buttons, turn potentiometers, and observe the LED feedback to control your MIDI device.

## Troubleshooting

- Ensure all connections are secure.
- Verify that the correct pins are defined in the header files.
- Check the Arduino IDE for any compilation errors.
- Consult the serial monitor for debugging messages.

## Contributing

Feel free to contribute to this project by submitting issues or pull requests. Any improvements or bug fixes are welcome.

## License

This project is licensed under the MIT License. See the LICENSE file for details.

## Acknowledgements

Thank you to the open-source community for providing valuable resources and inspiration for this project.
