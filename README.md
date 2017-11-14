# Car RGB

This project is a simple microcontroller-based system built to control car interior RGB LEDs.

## How it works?
You can turn the LEDs on/off and change their color with just one pushable knob (a rotary encoder with button).
You can turn them on/off only, when ignition key is at least in accessory position.
To turn it on/off just press the knob button.

### Color changing:
To change color, keep the button pressed for about 1 second. LEDs should blink white. Now, by rotating the knob you change hue. Short button press switches it to saturation mode - rotate the knob to change saturation. Another short press is a brightness mode.
To save color, long press the button. Now short pressing the button again changes turning LEDs on and off.

The LEDs turn on automatically when you open the door, just like ceiling lights, regardless of ignition key position.

##### Caution! If you don't save selected color, it will not react to ignition key position or door open. This may lead to battery discharge.

## Hardware
This is designed to work with 12V RGB LED strips with common anode.
Connections needed:
- Power +12V (directly from battery, fused)
- GND
- Ignition key (+12V when ignition key in acc position)
- Door open (GND when door are open)

Microcontroller (ATXmega32E5) connections:
###### Port D:
Pins 0, 1 - Encoder channels A, B
Pin 2 - Encoder button
Pin 4 - Door sensor relay
Pin 5 - Ignition key relay

Circuit schematic: COMING SOON
PCB board: COMING SOON

## Software
Atmel Studio project, Atmel toolchain, compiler: avr-g++ with `-std=c++11` flag (uses C++11 features)

Hardware timers used:
-TCD5 for encoder
-TCC5 for main and keyboard events
-TCC4 for RGB PWM

Uses modified MegaSuperDebonuce library for button support (PL): http://mikrokontrolery.blogspot.com/2011/04/jezyk-c-biblioteka-obsluga-klawiatury.html