#define F_CPU 12000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
// #include <stdio.h>		// provides sprintf() which displays number on LCD	// not required here
// #include "lcdcontrolusbasp.h"							// not required here

//  You can make a 7 LED segment watch that has 4 digits with this components:
//  - four 7 segments and 1 point LED display,
//  - two CD4067B one for left field and one for right field
//  - four flip-flop LED driving gates made with eight ULN2803A which have 2.7 Kohm input resistors inside them
//  - one 1.5A PNP transistor
//  - one 1.5A NPN transistor
//  - one 1 uF capacitor
//  - three 10 Kohm resistors
//  - 32 560 ohm resistors
//  - 32 4.7 Kohm resistors
//  - 32 47 nF capacitors
//  - 32 1N4148 diodes
//  - two 1N4007 diodes
//
//
//  Schematic for controlling one LED segment with one CD4067 OUT channel:
//
//
//       VCC-RESET >-----------+------------------------------------------------------------------+
//                             |                                                                  |
//       Connected to a        |                                                                  |
//       switch to VCC         |                                                                  |
//       controlled by the     []                                                                 []
//       microcontroller       [] R1 = 560 ohm                                                    [] R2 = 4.7 Kohm
//       (1.5A PNP - NPN       []                                                                 []
//       transistor pair and   |                                                                  |
//       10 Kohm resistors)    |                                                                  |
//                             |                                                                  |
//             +---------------0-----------------------------+      +-----------------------------+
//             |               |                              \    /                              |
//             |               |                               \  /                               |
//             |               | STARTUP LATCH ON               \/              STARTUP LATCH OFF |
//             |             +-+----------+                     /\                     +----------+-+
//             |             | C          |                    /  \                    |          C |
//          +--+--+          |            |                   /    \                   |            |
//           \   /  LED      |   ULN    B +------------------0      0------------------+ B    ULN   |
//            \ / segment    |  2803A     |                  |      |                  |     2803A  |
//          ---V---          |            |                  |      |                  |            |
//             | common      | common E   |                  |      |                  |   common E |
//             | cathode     +----+-------+                  |      |                  +-------+----+
//             |                  |                          |      |                          |
//             |                  |                          |      |                          |
//             |                  |        +-----------------+      +-----------+              |
//             |                  |        |                                    |              |
//             |                  |        |                                    |              +--------------------+
//             |                  |     +--+--+                                 |                                   |
//             |                  |      \   /                             -----+-----                              |
//             |                  |       \ /   D1 = 1N4148                -----+-----                              |
//             |                  |     ---V---                                 |  C1 = 47 nF                       |
//             |                  |        |                                    |                                   |
//             |                  |        |                                    |   Prevents STARTUP LATCH OFF      |
//             |                  |       / \                                   |   from turning ON at startup      |
//             |                  |   Whenever this is pulled to low, the       |   by delaying it's UBE rise       |
//             |                  |   LED is latched ON and turning it OFF      |   and so STARTUP LATCH ON has     |
//             |                  |   requires that VCC-RESET is turned         |   more time to turn ON for sure   |
//             |                  |   OFF for a short time then ON again.       |   and LED is shorted and is OFF   |
//             |                  |   Connected to a CD4067B OUT channel.       |   awaiting to be latched ON.      |
//             |                  |                                             |                                   |
//             |                  |                                             |                                   |
//          ---+---               |                                          ---+---                                |
//            GND                 |                                            GND                                  |
//                                |                                                                                 |
//                                |                                                                                 |
//                                +--------------------+----------------------+-------------------------------------+
//                                                     |                      |
//                                                     |                      |   For proper latching the common
//                                                     |                      |   emmitors of the ULN2803A need to be
//                                                     |                      |   at a voltage of about 1.4 V because
//                                                  +--+--+                   |   a CD4067B has a low logic level of
//                                                   \   /                    |   0.8 V also the VCC-RESET's turn OFF
//                                                    \ /   D2 = 1N4007       |   unlatch efectiveness is increased.
//                                                  ---V---                   |
//                                                     |                      |
//                                                     |                 -----+-----
//                                                     |                 -----+-----
//                                                     |                      |  C2 = 1 uF
//                                                  +--+--+                   |
//                                                   \   /                    |
//                                                    \ /   D3 = 1N4007       |
//                                                  ---V---                   |
//                                                     |                      |
//                                                     |                      |
//                                                     |                      |
//                                                     |                      |
//                                                  ---+---                ---+---
//                                                    GND                    GND



 Microcontroller Pins:

// - 4 pins connected to both CD4067B inputs for selection of channel 0 - 15
// - 1 pin for resetting all flip flops by cycling their VCC - all LED's off
// - 1 pin for channel state of left field CD4067B
// - 1 pin for channel state of right field CD4067B
// - 1 pin for potentiometer 8 bit field value change 0 to 255 - 
//         0 -  35 = 36 steps = fast decrease
//        36 -  71 = 36 steps = normal decrease
//        72 - 107 = 36 steps = slow decrease
//       108 - 147 = 40 steps = keep constant
//       148 - 183 = 36 steps = slow increase
//       184 - 219 = 36 steps = normal increase
//       220 - 255 = 36 steps = fast increase
//  - 1 pin for multifunctional button
//        - one short press = date mode for 10 seconds, another short press within two seconds = year mode for 10 seconds
//        - another short press in yeart mode = back to time mode
//        - one long press in time / date / year mode = select left field to change, another long press select right field to change,
//        - another long press = back to left field to change
//        - short press in select mode = cancel select mode, show current date / year for another 10 seconds if that was the last mode

// To be continued...
