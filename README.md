# AltSoftSerial Library

## Modifications by rizsi

 * Instead of an Arduino library make it a standalone avr-gcc library
 * Remove target specific code from implementation. This implements some kind of HAL layer. Use target specific configuration header instead of ifdefs.
 * Implement parity and 2 stop bits.


## Original

Improved software emulated serial, using hardware timers for precise signal
timing and availability of CPU time for other libraries to respond to interrupts
during data AltSoftSerial data transmission and reception.

http://www.pjrc.com/teensy/td_libs_AltSoftSerial.html

![AltSoftSerial on Teensy 2.0](http://www.pjrc.com/teensy/td_libs_AltSoftSerial_2.jpg)

If you need very low baud rates, check out [SlowSoftSerial](https://forum.pjrc.com/threads/58604-SlowSoftSerial-library-for-slow-baud-rates?p=222991#post222991) by Paul Williamson.
