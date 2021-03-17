# Sewing Speed Control Driver

Enable more precise control of a sewing machine with Arduino and custom PCB.

## Background and Motivation
It can be difficult to keep speed steady with the generic "universal" foot pedals. The pedals are sensitive to small changes and speed can quickly get out of control. The lower leg gets fatigued trying to keep pedal in the perfect position, especially when the workspace has with less than ideal ergonomics.

To address the above challenges, I changed the foot switch to be simply on/off instead of the standard slow to ultra-fast analog range. Speed is now hand set by adjusting a rotary knob located near the work surface.

## Technical Details
**DANGER: This project involves 120V AC Mains voltage which can be deadly!**

TODO: Detailed writeup

## Materials
- Sewing machine with AC motor (I have a Janome HD-1000 with 120V 0.75A motor)
- Arduino Uno
- Foot switch ([example](https://www.adafruit.com/product/423))
- BT136-600E triac ([data sheet](https://www.mouser.com/datasheet/2/848/bt136-600e-1520534.pdf))
- MOC3021 optocoupler ([data sheet](https://optoelectronics.liteon.com/upload/download/DS-70-99-0019/MOC302X%20series%20201606.pdf))
- 4N25 optocoupler ([data sheet](https://optoelectronics.liteon.com/upload/download/DS-70-99-0010/4N2X%20series%20Datasheet%201115.pdf))
- KBP310G bridge rectifier ([data sheet](https://www.mouser.com/datasheet/2/115/KBP304G-KBP310G-1536400.pdf))
- 2x 47kΩ 1W resistors
- 330Ω 1W resistor
- 2x 10kΩ 1/4W resistors
- 4.7kΩ 1/4W resistor
- 2.2kΩ 1/4W resistor
- 330Ω 1/4W resistor
- 10kΩ rotary potentiometer
- 0.1uF 630VDC/280VAC polypropylene film capacitor
- 2x 1x2 screw terminal blocks
- 4 pin right-angle header
- 1A fuse and fuse holder
- TO-220 heat sink for triac

## Future Work
- Save and select preset speed settings
- Fine tune phase control to improve motor operation
- Make SMD version
