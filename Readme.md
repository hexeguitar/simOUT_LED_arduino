## simOUT Arduino LED driver

**Work still in progress**

Arduino Leonardo or Pro-Mico (ATMEGA32U4) based simOut/HomeSimCockpit LED driver.

Requires TimerOne library.
Master module is always based on ATMEGA32U4: Arduino Leonardo or Pro-Mico.
Slave modules may use cheaper Nano or the same 32U4 boards.

Serial port settings:
- 57600 Baud
- 8 data bits
- 2 stop bits
- no parity

Requires HomeSimCocpit 1.1.1.2 to drive 6 digits or 48 LEDs.

Master->Slave connections:
GND - GND
5V - 5V
TX - RX

![alt text][pic1]


**SimOUT_LEDs** - basic program, configured as MASTER for ATMEGA32U4 (Arduino Leonardo/Pro-Micro).
**simOUT_LEDs_slave328** - program configured as SLAVE for Arduino Nano (ATMEGA328P).


------
(c) 09.2016 by Piotr Zapart
www.hexeguitar.com

License:
Creative Commons - Attribution - ShareAlike 3.0
[http://creativecommons.org/licenses/by-sa/3.0/](http://creativecommons.org/licenses/by-sa/3.0/)

[pic1]: pics/ProMicro_simOUTleds.png "ProMico as simOUT LED driver"

