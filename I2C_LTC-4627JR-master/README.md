# I2C control of a Lite-On LTC4627-JR (4-digit 7-segment LED)

The goal of this project is to create a small module that interfaces a 4-digit 7-segment LED to an I2C bus.

The original motivation is to allow a Raspberry Pi to display a few hexadecimal numbers with minimal overhead. Send a few bytes over I2C and this module will handle the rest.

Parts:

* The LED unit is the Lite-On LTC4627-JR. 
It has some features in addition to the standard 4-digit 7-segment design:
1. This is actually an 8-segment LED because of the 7-segments for numerals plus a LED for the optional decimal point.
2. This is actually a 5-digit display because of a few additional symbols on the surface. It appears to have been designed for a clock display so there are three additional LEDs controlled as a "fifth digit":  Two LEDs for displaying the ":" in 12:00 and a third probably to designate AM/PM.

* The brains of this operation will be a PIC 16F18345

* Current-limiting resistors * 8, one for each of the segments.

* NPN transistors * 5, one for each of the digits.
