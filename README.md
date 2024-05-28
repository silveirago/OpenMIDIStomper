# OpenMIDIStomper

_openMIDIStomper
Description
_openMIDIStomper is an Arduino-based MIDI controller that provides customizable MIDI message sending capabilities using buttons, potentiometers, and LEDs. This project is designed to offer flexible control over MIDI devices for musicians and hobbyists.

Features
Button Control: Send MIDI messages using button presses.
Potentiometer Control: Adjust MIDI parameters with potentiometers.
LED Indicators: Visual feedback using LEDs.
Customizable Messages: Define your own MIDI messages and configurations.
Getting Started
Prerequisites
Arduino IDE installed on your computer.
Arduino board (compatible with the code).
Necessary components:
Push buttons
Potentiometers
LEDs
Resistors
Breadboard and jumper wires
Installation
Clone the Repository:

sh
Copy code
git clone https://github.com/yourusername/_openMIDIStomper.git
Open the Project:
Open the _openMIDIStomper.ino file in the Arduino IDE.

Hardware Setup
Components and Connections
Buttons:

Connect one terminal of each button to a digital pin on the Arduino.
Connect the other terminal to ground.
Potentiometers:

Connect the middle terminal of each potentiometer to an analog input pin on the Arduino.
Connect the other two terminals to 5V and ground, respectively.
LEDs:

Connect the anode (long leg) of each LED to a digital pin on the Arduino through a current-limiting resistor.
Connect the cathode (short leg) to ground.
Software Configuration
Button Definitions:

Modify ButtonDefinitions.h to define the pins and settings for your buttons.
Button Logic:

Implement the logic for button presses in ButtonLogic.h.
LED Control:

Adjust the LED control logic in LEDControl.h.
MIDI Messages:

Configure the MIDI messages to be sent in MIDIMessages.h.
MIDI Out:

Set up the MIDI output in MIDIOut.h.
Potentiometer Control:

Define how potentiometer inputs affect MIDI messages in PotentiometerControl.h.
Usage
Upload the Code:

Connect your Arduino board to your computer.
Select the appropriate board and port in the Arduino IDE.
Upload the _openMIDIStomper.ino sketch to the board.
Connect to MIDI Device:

Use a MIDI interface to connect the Arduino to your MIDI device or computer.
Operate:

Press buttons, turn potentiometers, and observe the LED feedback to control your MIDI device.
Troubleshooting
Ensure all connections are secure.
Verify that the correct pins are defined in the header files.
Check the Arduino IDE for any compilation errors.
Consult the serial monitor for debugging messages.
Contributing
Feel free to contribute to this project by submitting issues or pull requests. Any improvements or bug fixes are welcome.

License
This project is licensed under the MIT License. See the LICENSE file for details.

Acknowledgements
Thank you to the open-source community for providing valuable resources and inspiration for this project.