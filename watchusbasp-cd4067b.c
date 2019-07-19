#define F_CPU 12000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
// #include <stdio.h>     // provides sprintf() which displays number on LCD   // not required here
// #include "lcdcontrolusbasp.h"      // not required here
//
//  You can make a 7 LED segment watch that has 4 digits with this components:
//  - four 7 segments and 1 point LED display, in my case they have a forward voltage of 3,84V so adapting for that
//  - two CD4067B one for left field and one for right field
//  - four flip-flop LED driving gates made with eight ULN2803A which have 2.7 Kohm input resistors inside them
//  - 32 cheap 500mA pnp transistors - BC640 for PARTICULAR LED RESET
//  - one 1.5 A NPN transistor BD139 for COMMON LED RESET
//  - one 1 uF capacitor
//  - one 100 Kohm resistors for the aditional BC640 base pull up
//  - 32 560 ohm resistors
//  - 74 4.7 Kohm resistors - 64 for all circuits, 6 for pulling up CD4067B inputs, 4 for COMMON LED RESET
//  - 32 47 nF capacitors
//  - 34 1N4148 diodes - two for COMMON LED RESET
//  - two 1N4007 diodes
//
//
//  For all 32 LED's at 5V, total circuit current consumption is below 400 mA.
//  Only the LED's all ON current consumption is about 160 mA
//  Each LED that is ON receives about 5 mA, and the light looks just about right.
//
//  LTSPICE IV simulation is in this repository as watchcd.asc
//
//  Schematic for controlling one LED segment with one OUT channel of CD4067B:
//
//
//     ANALOG VCC   >----------+-------------------------------------------------------------------+
//     it can be               |                                                                   |
//     higher than the         |                                                                   |
//     5V DIGITAL VCC          |                                                                   |
//     up to 9V                []                                                                  []
//                             [] R1 = 560 ohm                                                     [] R2 = 4.7 Kohm
//                             []                                                                  []
//                             |                                                                   |
//                             |                                                                   |
//                             |                                                                   |
//             +---------------0---------------------+      +--------------------------------------+
//             |               |                      \    /                                       |
//             |               |                       \  /                                        |
//             |               | STARTUP LATCH ON       \/                       STARTUP LATCH OFF |
//             |             +-+----------+             /\                              +----------+-+
//             |             | C          |            /  \                             |          C |
//          +--+--+          |            |           /    \                            |            |
//           \   /  LED      |   ULN    B +----------0      0---------------------+-----+ B    ULN   |
//            \ / segment    |  2803A     |          |      |                     |     |     2803A  |
//          ---V---          |            |          |      |                     |     |            |
//             | common      | common E   |          |      |                     |     |   common E |
//             | cathode     +----+-------+          |      |                     |     +-------+----+
//             |                  |                  |      |                     |             |
//             |                  |                  |      |                     |             |
//             |                  |                  |      |                 |  /              |
//             |                  |                  |      |                 | V/   T1         |
//             |                  |                  |      |       +---------+K    BC640       |
//             |                  |                  |      |       |         | \               |
//             |                  |                  |      |       |         |  \              |
//             |                  |                  |      |       |             |             |
//             |                  |                  |      |       |             |             |
//             |                  |                  |      |       |          ---+---          |
//             |                  |                  |      |       |             GND           |
//             |                  |                  |      |       |                           |
//             |                  |                  |      |       []                          |
//             |                  |                  |      |       []  R3 = 4.7 Kohm           |
//             |                  |                  |      |       []                          |
//             |                  |                  |      |       |                           |
//             |                  |                  |      |       |                           |
//             |                  |                  |      |       |                           |
//             |                  |                  |      |      / \                          |
//             |                  |                  |      |     PARTICULAR LED RESET          |
//             |                  |                  |      |     Because 4.7 Kohm / 32 =       |
//             |                  |                  |      |     = 146.875 ohm this signal     |
//             |                  |                  |      |     needs to be pulled LOW by     |
//             |                  |                  |      |     a NPN transistor, a common    |
//             |                  |                  |      |     circuit shown down under      |
//             |                  |                  |      |     this schematic                |
//             |                  |                  |      |                                   |
//             |                  |                  |      |                                   |
//             |                  |        +---------+      +-------------------+               |
//             |                  |        |                                    |               |
//             |                  |        |                                    |               |
//             |                  |        |                                    |               |
//             |                  |        |                                    |               +-------------------+
//             |                  |     +--+--+                                 |                                   |
//             |                  |      \   /                             -----+-----                              |
//             |                  |       \ /   D1 = 1N4148                -----+-----                              |
//             |                  |     ---V---                                 |  C1 = 47 nF                       |
//             |                  |        |                                    |                                   |
//             |                  |        |                                    |                                   |
//             |                  |        |                                    |                                   |
//             |                  |        |                                    |   Prevents STARTUP LATCH OFF      |
//             |                  |       / \                                   |   from turning ON at startup      |
//             |                  |   Whenever this is pulled to low, the       |   by delaying it's UBE rise       |
//             |                  |   LED is latched ON and turning it OFF      |   and so STARTUP LATCH ON has     |
//             |                  |   requires that RESET is pulled LOW         |   more time to turn ON for sure   |
//             |                  |   for a short time then HIGH again.         |   and LED is shorted and is OFF   |
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
//                                                   \   /                    |   0.8 V also COMMON LED RESET has 1.4V
//                                                    \ /   D2 = 1N4007       |   when pulling low. As long as the LED
//                                                  ---V---                   |   is shorted at below 3.84V it's fine.
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
//
//
//   Schematic for COMMON LED RESET circuit:
//
//
//        ANALOG VCC   >--------------------------------+
//                                                      |
//                                                      |
//                                                      |
//                                                      |
//                                                      []
//                                                      [] R1 = 4.7 Kohm
//                                                      []
//                                                      |
//                      DIGITAL                         |
//                        VCC                           |
//                                                      |
//                        \ /                           +----------------<   PARTICULAR RED RESET
//    COMMON               |                            |
//   LED RESET             |                            |
//                         |                            |
//      \ /                |                            |
//       |      +----------+                            |
//       |      |                                   |  /
//       |      |                                   | /    T1
//       |      |                  +----------------+K    BD139
//       |      |                  |                | \
//       |      []                 |                |  V
//       |      [] R2 = 4.7 Kohm   |                    |
//       |      []                 |                    |
//       |      |                  |                    |
//       |      |                  |                    |
//       |      |                  |                    |
//       |      |                  |                    |
//       +------+                  |                    |
//              |                  |                    |
//              |                  |                    |
//              |                  |                    |
//              |                  |                    |
//           +--+--+               |                    |
//            \   /    D1          |                    |
//             \ /   14148         |                    |
//           ---+---               []                   |
//              |                  [] R3 = 4.7 Kohm     |
//              |                  []                   |
//           +--+--+               |                    |
//            \   /    D2          |                    |
//             \ /   14148         |                    |
//           ---+---               |                    |
//              |                  |                    |
//              |                  |                    |
//              |                  |                    |
//              |                  |                    |
//              +------------------+                    |
//                                 |                    |
//                                 |                    |
//                                 |                    |
//                                 |                    |
//                                 []                   |
//                                 [] R4 = 4.7 Kohm     |
//                                 []                   |
//                                 |                    |
//                                 |                    |
//                                 |                    |
//                                 |                    |
//                              ---+---              ---+---
//                                GND                  GND
//
//
//
//
//
//    PCB Layout for half of a field 8 LED's, the others up to 32 are almost simetrical to this one:
//
//
//     ANALOG VCC  >-----------------+----+----+----+----+----+----+----+
//                                   |    |    |    |    |    |    |    |
//                                   |    |    |    |    |    |    |    |
//                                   |    |    |    |    |    |    |    |
//                                   |    |    |    |    |    |    |    |
//                                   []   []   []   []   []   []   []   []
//                                   []   []   []   []   []   []   []   []  R = 680 ohm
//                                   []   []   []   []   []   []   []   []
//                                   |    |    |    |    |    |    |    |
//                                   |    |    |    |    |    |    |    |
//                                   |    |    |    |    |    |    |    |
//                                   0    0    0    0    0    0    0    0   LED anode connections for separate PCB
//                                   |    |    |    |    |    |    |    |
//                                   |    |    |    |    |    |    |    |
//                                   |    |    |    |    |    |    |    |
//                                   +----+----+----+----+----+----+----0----------+----+----+----+----+----+----+----0
//                                   |    |    |    |    |    |    |    |          |    |    |    |    |    |    |    |
//                                   +----+----+----+----+----+----0----+----------+----+----+----+----+----+----0----+
//                                   |    |    |    |    |    |    |    |          |    |    |    |    |    |    |    |
//                                   +----+----+----+----+----0----+----+----------+----+----+----+----+----0----+----+
//                                   |    |    |    |    |    |    |    |          |    |    |    |    |    |    |    |
//                                   +----+----+----+----0----+----+----+----------+----+----+----+----0----+----+----+
//                                   |    |    |    |    |    |    |    |          |    |    |    |    |    |    |    |
//                                   +----+----+----0----+----+----+----+----------+----+----+----0----+----+----+----+
//                                   |    |    |    |    |    |    |    |          |    |    |    |    |    |    |    |
//                                   +----+----0----+----+----+----+----+----------+----+----0----+----+----+----+----+
//                                   |    |    |    |    |    |    |    |          |    |    |    |    |    |    |    |
//                                   +----0----+----+----+----+----+----+----------+----0----+----+----+----+----+----+
//                                   |    |    |    |    |    |    |    |          |    |    |    |    |    |    |    |
//                                   0----+----+----+----+----+----+----+----------0----+----+----+----+----+----+----+
//                                   |    |    |    |    |    |    |    |          |    |    |    |    |    |    |    |
//                                   |    |    |    |    |    |    |    |          |    |    |    |    |    |    |    |
//                                   |    |    |    |    |    |    |    |          |    |    |    |    |    |    |    |
//                                   |    |    |    |    |    |    |    |          |    |    |    |    |    |    |    |
//                             [----------------------------------------]          |    |    |    |    |    |    |    |
//                             [----------------------------------------]          |    |    |    |    |    |    |    |
//                             [-------------ULN2803A------------------(]          |    |    |    |    |    |    |    |
//                             [----------------------------------------]          |    |    |    |    |    |    |    |
//                             [----------------------------------------]          |    |    |    |    |    |    |    |
//                                   |    |    |    |    |    |    |    |          |    |    |    |    |    |    |    |
//                                   |    |    |    |    |    |    |    |          |    |    |    |    |    |    |    |
//----------------]----              |    |    |    |    |    |    |    |          |    |    |    |    |    |    |    |
//  \__/          ]                  |    |    |    |    |    |    |    |          |    |    |    |    |    |    |    |
//                ]--------1N4148----+----+----+----+----+----+----+----0          |    |    |    |    |    |    |    |
//                ]                  |    |    |    |    |    |    |    |          |    |    |    |    |    |    |    |
//                ]--------1N4148----+----+----+----+----+----+----0----+          |    |    |    |    |    |    |    |
//                ]                  |    |    |    |    |    |    |    |          |    |    |    |    |    |    |    |
//                ]--------1N4148----+----+----+----+----+----0----+----+  4  >    |    |    |    |    |    |    |    |
//                ]                  |    |    |    |    |    |    |    |  .  >    |    |    |    |    |    |    |    |
//                ]--------1N4148----+----+----+----+----0----+----+----+  7  >    |    |    |    |    |    |    |    |
//    CD4067B     ]                  |    |    |    |    |    |    |    |     >    |    |    |    |    |    |    |    |
//                ]--------1N4148----+----+----+----0----+----+----+----+  K  >    |    |    |    |    |    |    |    |
//                ]                  |    |    |    |    |    |    |    |  o  >    |    |    |    |    |    |    |    |
//                ]--------1N4148----+----+----0----+----+----+----+----+  h  >    |    |    |    |    |    |    |    |
//                ]                  |    |    |    |    |    |    |    |  m  >    |    |    |    |    |    |    |    |
//                ]--------1N4148----+----0----+----+----+----+----+----+          |    |    |    |    |    |    |    |
//                ]                  |    |    |    |    |    |    |    |          |    |    |    |    |    |    |    |
//                ]--------1N4148----0----+----+----+----+----+----+----+          |    |    |    |    |    |    |    |
//                ]                  |    |    |    |    |    |    |    |          |    |    |    |    |    |    |    |
//                ]----              |    |    |    |    |    |    |    |          |    |    |    |    |    |    |    |
//                ]                  |    |    |    |    |    |    |    |          |    |    |    |    |    |    |    |
//                ]----              |    |    |    |    |    |    |    |          |    |    |    |    |    |    |    |
//                ]            [----------------------------------------]          |    |    |    |    |    |    |    |
//----------------]----        [----------------------------------------]          |    |    |    |    |    |    |    |
//                             [-------------ULN2803A------------------(]          |    |    |    |    |    |    |    |
//                             [----------------------------------------]          |    |    |    |    |    |    |    |
//                             [----------------------------------------]          |    |    |    |    |    |    |    |
//                                   |    |    |    |    |    |    |    |          |    |    |    |    |    |    |    |
//                                   |    |    |    |    |    |    |    |          |    |    |    |    |    |    |    |
//                                   |    |    |    |    |    |    |    |          |    |    |    |    |    |    |    |
//                                   |    |    |    |    |    |    |    |          |    |    |    |    |    |    |    |
//                                   +----+----+----+----+----+----+----0----------+----+----+----+----+----+----+----0
//                                   |    |    |    |    |    |    |    |          |    |    |    |    |    |    |    |
//                                   +----+----+----+----+----+----0----+----------+----+----+----+----+----+----0----+
//                                   |    |    |    |    |    |    |    |          |    |    |    |    |    |    |    |
//                                   +----+----+----+----+----0----+----+----------+----+----+----+----+----0----+----+
//                                   |    |    |    |    |    |    |    |          |    |    |    |    |    |    |    |
//                                   +----+----+----+----0----+----+----+----------+----+----+----+----0----+----+----+
//                                   |    |    |    |    |    |    |    |          |    |    |    |    |    |    |    |
//                                   +----+----+----0----+----+----+----+----------+----+----+----0----+----+----+----+
//                                   |    |    |    |    |    |    |    |          |    |    |    |    |    |    |    |
//                                   +----+----0----+----+----+----+----+----------+----+----0----+----+----+----+----+
//                                   |    |    |    |    |    |    |    |          |    |    |    |    |    |    |    |
//                                   +----0----+----+----+----+----+----+----------+----0----+----+----+----+----+----+
//                                   |    |    |    |    |    |    |    |          |    |    |    |    |    |    |    |
//                                   0----+----+----+----+----+----+----+----------0----+----+----+----+----+----+----+
//                                   |    |    |    |    |    |    |    |          |    |    |    |    |    |    |    |
//                                   |    |    |    |    |    |    |    |          |    |    |    |    |    |    |    |
//                                   |    |    |    |    |    |    |    |          |    |    |    |    |    |    |    |
//                                   |    |    |    |    |    |    |    |          |    |    |    |    |    |    |    |
//                                   ==================================================================================
//                                       BC640 and 47 nF capacitors and GND
//                                   ==================================================================================
//
//
//
// PCB layout for 16 LED's on a 30 x 48 holes breadboard, with capacitor
//
//        ..r.r.r.r.AVCCr.r.r.r.AVCCr.r.r.r.AVCCr.r.r.r..    <---  560 ohm resistors
//        .t.t.t.t......t.t.t.t.....t.t.t.t......t.t.t.t.
//        .t.t.t.t......t.t.t.t.....t.t.t.t......t.t.t.t.    <---  BC640
//        .t.t.t.t......t.t.t.t.....t.t.t.t......t.t.t.t.
//        .4.3.1.2......7.8.6.5.....5.6.8.7......2.1.3.4.
//        ..444444445555555555.......555555555544444444..
//        ..433333345666666665.......566666666543333334..
//        ..432222345677777765.......567777776543222234..
//        A.4321uuuuuuuuuu8765.....D.5678uuuuuuuuuu1234.A
//        V.4321>>uuuuuuuu8765.....V.5678uuuuuuuu<<1234.V    <---  ULN2803A
//        C.4321>>uuuuuuuu8765.....C.5678uuuuuuuu<<1234.C
//        C.4321uuuuuuuuuu8765X....C.5678uuuuuuuuuu1234.C
//        r.4321ttt..ttte.8765XcccccD5678eettt..ttt1234..
//        r.4321ttt..ttte.8765occccco5678.ettt..ttt1234.r
//        r.4321ttt..ttte.8765occccco5678.ettt..ttt1234.r
//        r.4321ttt..ttte.8765occccco5678.ettt..ttt1234.r    <---  CD4067B and 4.7 Kohm resistors
//        r.4321ttt..ttte.8765occccco5678.ettt..ttt1234.r
//        r.4321ttt..ttte.8765occccco5678.ettt..ttt1234.r
//        r.4321ttt..ttte.8765occccco5678.ettt..ttt1234.r
//        r.4321ttt..tttee8765occccco5678.ettt..ttt1234.r
//        A.4321uuuuuuuuuu8765occccco5678uuuuuuuuuu1234.A
//        V.4321>>uuuuuuuu8765icccccG5678uuuuuuuu<<1234.V    <---  ULN2803A
//        C.4321>>uuuuuuuu8765iccccci5678uuuuuuuu<<1234.C
//        C.4321uuuuuuuuuu8765Gccccci5678uuuuuuuuuu1234.C
//        ..432222345677777765G......567777776543222234..
//        ..433333345666666665N......566666666543333334..
//        ..444444445555555555D......555555555544444444..
//        .4.3.1.2......7.8.6.5.....5.6.8.7......2.1.3.4.
//        .c.c.c.c......c.c.c.c.....c.c.c.c......c.c.c.c.    <--- 47 nF capacitors
//        .........GND..........GND..........GND.........
//
//
//
// PCB layout for 16 LED's on a 30 x 48 holes breadboard, without capacitor
//
//        ..............................................48
//        ..r..r..r..r..r..r..r..r..r..r..r..r..r..r..r..r
//        .rr.rr.rr.rr.rr.rr.rr.rr.rr.rr.rr.rr.rr.rr.rr.rr
//        .rr.rr.rr.rr.rr.rr.rr.rr.rr.rr.rr.rr.rr.rr.rr.rr
//        .rr.rr.rr.rr.rr.rr.rr.rr.rr.rr.rr.rr.rr.rr.rr.rr
//        .r..r..r..r..r..r..r..r..r..r..r..r..r..r..r..r.
//        .t..t..t..t..t..t..t..t..t..t..t..t..t..t..t..t.
//        .tt.tt.tt.tt.tt.tt.tt.tt.tt.tt.tt.tt.tt.tt.tt.tt
//        .t..t..t..t..t..t..t..t..t..t..t..t..t..t..t..t.
//        111222333444555666777888888777666555444333222111
//        ..1..2..33345666..7..8.<>.8..7..66654333..2..1..
//        ..1..22222345677777..8.<>.8..77777654322222..1..
//        ..1...xx.234567.xx...8.<>.8...xx.765432.xx...1..
//        ..1...xxuuuuuuuuux...8....8...xuuuuuuuuuxx...1..
//        ..111111>>uuuuuuu88888....88888uuuuuuu<<111111..
//        ........>>uuuuuuu..............uuuuuuu<<........
//        ........uuuuuuuuu..X..DVCC.....uuuuuuuuu........
//        r.......ttt..ttte...XccccccD..eettt..ttt.......r
//        r.......ttt..ttte...occcccco...ettt..ttt.......r
//        r.......ttt..ttte...occcccco...ettt..ttt.......r
//        r.......ttt..ttte...occcccco...ettt..ttt.......r
//        r.......ttt..ttte...occcccco...ettt..ttt.......r
//        r.......ttt..ttte...occcccco...ettt..ttt.......r
//        r.......ttt..ttte...occcccco...ettt..ttt.......r
//        r.......ttt..tttee..occcccco...ettt..ttt.......r
//        A.......uuuuuuuuu...occccco....uuuuuuuuu.......A
//        V.......>>uuuuuuu..incccccG....uuuuuuu<<.......V
//        C.......>>uuuuuuu..incccccin...uuuuuuu<<.......C
//        C.......uuuuuuuuu...Gcccccin...uuuuuuuuu.......C
//        30..............................................
//  
//
//
//  Microcontroller Pins:
//
//  - 4 pins connected to both CD4067B inputs for selection of channel 0 - 15
//  - 1 pin for resetting all flip flops by cycling their VCC - all LED's off
//  - 1 pin for channel state of left field CD4067B
//  - 1 pin for channel state of right field CD4067B
//  - 1 pin for potentiometer 8 bit field value change 0 to 255 - 
//          0 -  35 = 36 steps = fast decrease
//         36 -  71 = 36 steps = normal decrease
//         72 - 107 = 36 steps = slow decrease
//        108 - 147 = 40 steps = keep constant
//        148 - 183 = 36 steps = slow increase
//        184 - 219 = 36 steps = normal increase
//        220 - 255 = 36 steps = fast increase
//  - 1 pin for multifunctional button
//        - one short press = date mode for 10 seconds, another short press within this time = year mode for 10 seconds
//        - another short press within year mode time = back to time mode
//        - one long press in time / date / year mode = select left field to change, another long press select right field to change
//        - another long press in select mode = back to left field to change
//        - short press in select mode = cancel select mode, back to showing current date / year mode for another 10 seconds
//
// To be continued...
