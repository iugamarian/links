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
//       |      |                  |                | L
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
//  Microcontroller Pins for controlling 5 fields that are 1 = hour 2 = minute 3 = day 4 = month 5 = year up to 2099 or 2199:
//
//  - 4 pins connected in parallel to five CD4067B inputs for selection of channel 0 - 15  =  B2  B3  B4  B5
//  - 1 pin for resetting all flip flops - all LED's off  =  D1
//  - 5 pins for the CD4067B fields  =  C0  C1  C2  D0  D3
//  
//  To use all 10 pins some modifications on the USBASP are required:
//  - VCC  changed to  C2
//  - GND  two pins traces cut and changed to  C0  and  C1 , 1K resistors from the USBASP LED's removed
//  - NC   changed to  D3 , the pin is easy to connect to, at a microcontroller corner corner


//  - 2 pins ADC6 and ADC7 for potentiometer 8 bit field value change 0 to 255, pins are a little harder to connect to:
//          0 -  35 = 36 steps = fast decrease
//         36 -  71 = 36 steps = normal decrease
//         72 - 107 = 36 steps = slow decrease
//        108 - 147 = 40 steps = keep constant
//        148 - 183 = 36 steps = slow increase
//        184 - 219 = 36 steps = normal increase
//        220 - 255 = 36 steps = fast increase
//
//  Optional  1 pin for multifunctional button - not used here
//        - one short press = date mode for 10 seconds, another short press within this time = year mode for 10 seconds
//        - another short press within year mode time = back to time mode
//        - one long press in time / date / year mode = select left field to change, another long press select right field to change
//        - another long press in select mode = back to left field to change
//        - short press in select mode = cancel select mode, back to showing current date / year mode for another 10 seconds
//
// To be continued...

static volatile unsigned char hours = 10;
static volatile unsigned char minutes = 0;
static volatile unsigned char seconds = 0;
static volatile unsigned char day = 11;
static volatile unsigned char month = 07;
static volatile unsigned char year = 19; // up to 254 - allow 255 reserve, display can show up to 199
static volatile unsigned char adc_value = 0;
static volatile unsigned char adc_channel = 0;
static volatile unsigned char pot_time = 0;
static volatile unsigned char pot_date = 0;
static volatile unsigned char pot_time_at_zero = 1;
static volatile unsigned char pot_date_at_zero = 1;
static volatile unsigned char interruptdetected = 0;
// need to be fast here, so no RAM usage if possible
unsigned char ledstep = 0;
unsigned char ledleftdigit1 = 0;
unsigned char ledrightdigit1 = 0;
unsigned char ledleftdigit2 = 0;
unsigned char ledrightdigit2 = 0;
unsigned char ledleftdigit3 = 0;
unsigned char ledrightdigit3 = 0;
unsigned char ledleftdigit4 = 0;
unsigned char ledrightdigit4 = 0;
unsigned char ledleftdigit5 = 0;
unsigned char ledrightdigit5 = 0;
unsigned char yearover100detected = 0;
unsigned char ledcurrentdigit1 = 0;
unsigned char ledcurrentdigit2 = 0;
unsigned char ledcurrentdigit3 = 0;
unsigned char ledcurrentdigit4 = 0;
unsigned char ledcurrentdigit5 = 0;
unsigned char pointsfortime = 0;
unsigned char pointsfordate = 0;
unsigned char ledhours = 10;
unsigned char ledminutes = 0;
unsigned char ledday = 11;
unsigned char ledmonth = 07;
unsigned char ledyear = 19; // up to 254 - allow 255 reserve, display can show up to 199


void setup() {

/////////////// SET MICROCONTROLLER REGISTERS

	// Variable to hold ADC result
	ADCSRA = (1<<ADEN) | (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);
	TCCR1B |= (1<<WGM12); // clear timer on compare (when OCR1A is reached, automatically go to 0)

//  Also check #define F_CPU clocks
//	OCR1A = 15625; // 1 second on 16mhz with prescaler 1024 => 	TCCR1B |= (1<<CS12) | (1<<CS10);
	OCR1A = 46875; // 1 second on 12mhz with prescaler 256  =>  TCCR1B |= (1<<CS12);

	TIMSK |= (1<<OCIE1A); // when OCR1A is reached, go to ISR(TIMER1_COMPA_vect) interrupt


	GICR &= ~(1 << INT0); // usbasp has a usb data pin on interrupt, don't want interrupt on button low
	sei(); // activate interrupts so (TIMER1_COMPA_vect) is available
	GICR &= ~(1 << INT0); // SEI ENABLES IT I THINK
//	TCCR1B |= (1<<CS12) | (1<<CS10); // setting prescaler also starts the counting of the timer, for 16MHz
	TCCR1B |= (1<<CS12);			// setting prescaler also starts the counting of the timer, for 12Mhz

//  Avoid using REGISTER = (1<<BIT); that makes previous settings zero (like WGM12 for example)
//  better always use for 1: REGISTER |= (1<<BIT);
//  better always use for 0: REGISTER &= ~ (1<<BIT);
//  ((1<<pin)|(1<<pin)|(1<<pin));

/////////////// SET MICROCONTROLLER PORTS

	// And now that interrupt INT0 is disabled which is pin D2 which is connected to pin B1 on USBASP,
	// setting as ignored input with pull up so no interrupt microcontriller hangs
	DDRD &= ~ (1 << 2);
	PORTD |= (1 << 2);
	// setting B0 as input and ignoring it with no pull up
	DDRB &= ~ (1<<0); // input
	PORTB &= ~ (1<<0); // no pull up
	// and setting B1 that is connected to D2 as input and ignoring it with no pull up
	DDRB &= ~ (1<<1); // input
	PORTB &= ~ (1<<1); // no pull up

//  Microcontroller Pins for controlling 5 fields that are 1 = hour 2 = minute 3 = day 4 = month 5 = year up to 2099 or 2199:
//

//  - 1 pin for resetting all flip flops - all LED's off  =  D1
//  - 5 pins for the CD4067B fields  =  C0  C1  C2  D0  D3
  DDRC |= (1<<0) | (1<<1) | (1<<2);
  PORTC |= (1<<0) | (1<<1) | (1<<2);  // Set to 1 to not trigger LED ON
// On port D we will ignore D2 which is INT0 and connected to B1
  DDRD |= (1<<3) | (1<<1) | (1<<0);
  PORTD |= (1<<3) | (1<<1) | (1<<0);  // Set to 1 to not trigger LED ON (or LED RESET which will be done later)

//  - 4 pins connected in parallel to five CD4067B inputs for selection of channel 0 - 15  =  B2  B3  B4  B5
  DDRB |= (1<<5) | (1<<4) | (1<<3) | (1<<2);        // CD4067B inputs set as microcontroller outputs
  PORTB &= ~ ((1<<5) | (1<<4) | (1<<3) | (1<<2));   // and indicate CD4067B channel 1 of 16 which is 0 of 15

/////////////// SET VARIABLES THAT ARE NOT READ YET TO KEEP CONSTANT VALUE

pot_time = 128;
pot_date = 128;

}

void updatecdselection() {

//  PORTC |= (1<<0) | (1<<1) | (1<<2);  // Set to 1 to not trigger LED ON
//  PORTD |= (1<<3) | (1<<0);  // Set to 1 to not trigger LED ON

  _delay_ms(1);  // allow time for CD4067BE flip flops speed
  PORTB = (ledstep<<2);	// select CD4067BE channel
  _delay_ms(1);  // allow time for CD4067BE flip flops speed
  // a total of 16 x 2 = 32 ms this is very good

}

void updatecdcommandled0() {

//////////////////////////////////////// field 1 of 5
	
	if( !( ( ledcurrentdigit1 == 5 ) || ( ledcurrentdigit1 == 6 ) ) ) PORTC &= ~ (1<<0);

//////////////////////////////////////// field 2 of 5

	if( !( ( ledcurrentdigit2 == 5 ) || ( ledcurrentdigit2 == 6 ) ) ) PORTC &= ~ (1<<1);

//////////////////////////////////////// field 3 of 5

	if( !( ( ledcurrentdigit3 == 5 ) || ( ledcurrentdigit3 == 6 ) ) ) PORTC &= ~ (1<<2);

//////////////////////////////////////// field 4 of 5

	if( !( ( ledcurrentdigit4 == 5 ) || ( ledcurrentdigit4 == 6 ) ) ) PORTD &= ~ (1<<0);

//////////////////////////////////////// field 5 of 5

	if( !( ( ledcurrentdigit5 == 5 ) || ( ledcurrentdigit5 == 6 ) ) ) PORTD &= ~ (1<<3);

///////////////////////////////////////////  END of fields

  _delay_ms(2); // time to switch flip flop

// get all CD4067 flip flop setting pins up so another reset will work later

  PORTC |= (1<<0) | (1<<1) | (1<<2);  // Set to 1 to not trigger LED ON
  PORTD |= (1<<3) | (1<<0);  // Set to 1 to not trigger LED ON

}

void updatecdcommandled1() {

//////////////////////////////////////// field 1 of 5

	if( !( ( ledcurrentdigit1 == 1 ) || ( ledcurrentdigit1 == 4 ) ) ) PORTC &= ~ (1<<0);

//////////////////////////////////////// field 2 of 5

	if( !( ( ledcurrentdigit2 == 1 ) || ( ledcurrentdigit2 == 4 ) ) ) PORTC &= ~ (1<<1);

//////////////////////////////////////// field 3 of 5

	if( !( ( ledcurrentdigit3 == 1 ) || ( ledcurrentdigit3 == 4 ) ) ) PORTC &= ~ (1<<2);

//////////////////////////////////////// field 4 of 5

	if( !( ( ledcurrentdigit4 == 1 ) || ( ledcurrentdigit4 == 4 ) ) ) PORTD &= ~ (1<<0);

//////////////////////////////////////// field 5 of 5

	if( !( ( ledcurrentdigit5 == 1 ) || ( ledcurrentdigit5 == 4 ) ) ) PORTD &= ~ (1<<3);

///////////////////////////////////////////  END of fields

  _delay_ms(2); // time to switch flip flop

// get all CD4067 flip flop setting pins up so another reset will work later

  PORTC |= (1<<0) | (1<<1) | (1<<2);  // Set to 1 to not trigger LED ON
  PORTD |= (1<<3) | (1<<0);  // Set to 1 to not trigger LED ON

}


void updatecdcommandled2() {

//////////////////////////////////////// field 1 of 5 show middle potentiometer position for time

if((pointsfortime > 3) && (ledstep == 2)) PORTC &= ~ (1<<0);
if((pointsfortime > 2) && (pointsfortime < 6) && (ledstep == 10)) PORTC &= ~ (1<<0);

//////////////////////////////////////// field 2 of 5 show middle potentiometer position for time

if((pointsfortime > 1) && (pointsfortime < 5) && (ledstep == 2)) PORTC &= ~ (1<<1);
if((pointsfortime > 0) && (pointsfortime < 4 ) && (ledstep == 10 )) PORTC &= ~ (1<<1);

//////////////////////////////////////// field 3 of 5 show middle potentiometer position for date

if((pointsfordate > 3) && (ledstep == 2)) PORTC &= ~ (1<<2);
if((pointsfordate > 2) && (pointsfordate < 6) && (ledstep == 10)) PORTC &= ~ (1<<2);

//////////////////////////////////////// field 4 of 5 show middle potentiometer position for date

if((pointsfordate > 1) && (pointsfordate < 5) && (ledstep == 2)) PORTD &= ~ (1<<0);
if((pointsfordate > 0) && (pointsfordate < 4 ) && (ledstep == 10 )) PORTD &= ~ (1<<0);

//////////////////////////////////////// field 5 of 5 this are the two points, used to command displaying year 20xx OFF or 21xx ON
	
	if( yearover100detected ) PORTD &= ~ (1<<3);

///////////////////////////////////////////  END of fields

  _delay_ms(2); // time to switch flip flop

// get all CD4067 flip flop setting pins up so another reset will work later

  PORTC |= (1<<0) | (1<<1) | (1<<2);  // Set to 1 to not trigger LED ON
  PORTD |= (1<<3) | (1<<0);  // Set to 1 to not trigger LED ON

}


void updatecdcommandled3() {

//////////////////////////////////////// field 1 of 5
	
	if( !( ( ledcurrentdigit1 == 1 ) || ( ledcurrentdigit1 == 2 ) || ( ledcurrentdigit1 == 3 )  || ( ledcurrentdigit1 == 7 ) ) ) PORTC &= ~ (1<<0);

//////////////////////////////////////// field 2 of 5

	if( !( ( ledcurrentdigit2 == 1 ) || ( ledcurrentdigit2 == 2 ) || ( ledcurrentdigit2 == 3 )  || ( ledcurrentdigit2 == 7 ) ) ) PORTC &= ~ (1<<1);

//////////////////////////////////////// field 3 of 5

	if( !( ( ledcurrentdigit3 == 1 ) || ( ledcurrentdigit3 == 2 ) || ( ledcurrentdigit3 == 3 )  || ( ledcurrentdigit3 == 7 ) ) ) PORTC &= ~ (1<<2);

//////////////////////////////////////// field 4 of 5

	if( !( ( ledcurrentdigit4 == 1 ) || ( ledcurrentdigit4 == 2 ) || ( ledcurrentdigit4 == 3 )  || ( ledcurrentdigit4 == 7 ) ) ) PORTD &= ~ (1<<0);

//////////////////////////////////////// field 5 of 5

	if( !( ( ledcurrentdigit5 == 1 ) || ( ledcurrentdigit5 == 2 ) || ( ledcurrentdigit5 == 3 )  || ( ledcurrentdigit5 == 7 ) ) ) PORTD &= ~ (1<<3);

///////////////////////////////////////////  END of fields

  _delay_ms(2); // time to switch flip flop

// get all CD4067 flip flop setting pins up so another reset will work later

  PORTC |= (1<<0) | (1<<1) | (1<<2);  // Set to 1 to not trigger LED ON
  PORTD |= (1<<3) | (1<<0);  // Set to 1 to not trigger LED ON

}


void updatecdcommandled4() {

//////////////////////////////////////// field 1 of 5
	
	if( !( ( ledcurrentdigit1 == 0 ) || ( ledcurrentdigit1 == 1 ) || ( ledcurrentdigit1 == 7 ) ) ) PORTC &= ~ (1<<0);

//////////////////////////////////////// field 2 of 5

	if( !( ( ledcurrentdigit2 == 0 ) || ( ledcurrentdigit2 == 1 ) || ( ledcurrentdigit2 == 7 ) ) ) PORTC &= ~ (1<<1);

//////////////////////////////////////// field 3 of 5

	if( !( ( ledcurrentdigit3 == 0 ) || ( ledcurrentdigit3 == 1 ) || ( ledcurrentdigit3 == 7 ) ) ) PORTC &= ~ (1<<2);

//////////////////////////////////////// field 4 of 5

	if( !( ( ledcurrentdigit4 == 0 ) || ( ledcurrentdigit4 == 1 ) || ( ledcurrentdigit4 == 7 ) ) ) PORTD &= ~ (1<<0);

//////////////////////////////////////// field 5 of 5

	if( !( ( ledcurrentdigit5 == 0 ) || ( ledcurrentdigit5 == 1 ) || ( ledcurrentdigit5 == 7 ) ) ) PORTD &= ~ (1<<3);

///////////////////////////////////////////  END of fields

  _delay_ms(2); // time to switch flip flop

// get all CD4067 flip flop setting pins up so another reset will work later

  PORTC |= (1<<0) | (1<<1) | (1<<2);  // Set to 1 to not trigger LED ON
  PORTD |= (1<<3) | (1<<0);  // Set to 1 to not trigger LED ON

}


void updatecdcommandled5() {

//////////////////////////////////////// field 1 of 5
	
	if( !( ledcurrentdigit1 == 2 ) ) PORTC &= ~ (1<<0);

//////////////////////////////////////// field 2 of 5

	if( !( ledcurrentdigit2 == 2 ) ) PORTC &= ~ (1<<1);

//////////////////////////////////////// field 3 of 5

	if( !( ledcurrentdigit3 == 2 ) ) PORTC &= ~ (1<<2);

//////////////////////////////////////// field 4 of 5

	if( !( ledcurrentdigit4 == 2 ) ) PORTD &= ~ (1<<0);

//////////////////////////////////////// field 5 of 5

	if( !( ledcurrentdigit5 == 2 ) ) PORTD &= ~ (1<<3);

///////////////////////////////////////////  END of fields

  _delay_ms(2); // time to switch flip flop

// get all CD4067 flip flop setting pins up so another reset will work later

  PORTC |= (1<<0) | (1<<1) | (1<<2);  // Set to 1 to not trigger LED ON
  PORTD |= (1<<3) | (1<<0);  // Set to 1 to not trigger LED ON

}


void updatecdcommandled6() {

//////////////////////////////////////// field 1 of 5
	
	if( !( ( ledcurrentdigit1 == 1 ) || ( ledcurrentdigit1 == 4 ) || ( ledcurrentdigit1 == 7 ) ) ) PORTC &= ~ (1<<0);

//////////////////////////////////////// field 2 of 5

	if( !( ( ledcurrentdigit2 == 1 ) || ( ledcurrentdigit2 == 4 ) || ( ledcurrentdigit2 == 7 ) ) ) PORTC &= ~ (1<<1);

//////////////////////////////////////// field 3 of 5

	if( !( ( ledcurrentdigit3 == 1 ) || ( ledcurrentdigit3 == 4 ) || ( ledcurrentdigit3 == 7 ) ) ) PORTC &= ~ (1<<2);

//////////////////////////////////////// field 4 of 5

	if( !( ( ledcurrentdigit4 == 1 ) || ( ledcurrentdigit4 == 4 ) || ( ledcurrentdigit4 == 7 ) ) ) PORTD &= ~ (1<<0);

//////////////////////////////////////// field 5 of 5

	if( !( ( ledcurrentdigit5 == 1 ) || ( ledcurrentdigit5 == 4 ) || ( ledcurrentdigit5 == 7 ) ) ) PORTD &= ~ (1<<3);

///////////////////////////////////////////  END of fields

  _delay_ms(2); // time to switch flip flop

// get all CD4067 flip flop setting pins up so another reset will work later

  PORTC |= (1<<0) | (1<<1) | (1<<2);  // Set to 1 to not trigger LED ON
  PORTD |= (1<<3) | (1<<0);  // Set to 1 to not trigger LED ON


}


void updatecdcommandled7() {

//////////////////////////////////////// field 1 of 5
	
	if( ( ledcurrentdigit1 == 0 ) || ( ledcurrentdigit1 == 2 ) || ( ledcurrentdigit1 == 6 ) || ( ledcurrentdigit1 == 8 ) ) PORTC &= ~ (1<<0);

//////////////////////////////////////// field 2 of 5

	if( ( ledcurrentdigit2 == 0 ) || ( ledcurrentdigit2 == 2 ) || ( ledcurrentdigit2 == 6 ) || ( ledcurrentdigit2 == 8 ) ) PORTC &= ~ (1<<1);

//////////////////////////////////////// field 3 of 5

	if( ( ledcurrentdigit3 == 0 ) || ( ledcurrentdigit3 == 2 ) || ( ledcurrentdigit3 == 6 ) || ( ledcurrentdigit3 == 8 ) ) PORTC &= ~ (1<<2);

//////////////////////////////////////// field 4 of 5

	if( ( ledcurrentdigit4 == 0 ) || ( ledcurrentdigit4 == 2 ) || ( ledcurrentdigit4 == 6 ) || ( ledcurrentdigit4 == 8 ) ) PORTD &= ~ (1<<0);

//////////////////////////////////////// field 5 of 5

	if( ( ledcurrentdigit5 == 0 ) || ( ledcurrentdigit5 == 2 ) || ( ledcurrentdigit5 == 6 ) || ( ledcurrentdigit5 == 8 ) ) PORTD &= ~ (1<<3);

///////////////////////////////////////////  END of fields


  _delay_ms(2); // time to switch flip flop

// get all CD4067 flip flop setting pins up so another reset will work later

  PORTC |= (1<<0) | (1<<1) | (1<<2);  // Set to 1 to not trigger LED ON
  PORTD |= (1<<3) | (1<<0);  // Set to 1 to not trigger LED ON

}


void led_update_fields() {

// How it works:

// 1 - set the fast working variables from the RAM interrupt friendly variables
// 2 - reset all LED flip flops turning them off for 10 ms
// 3 - through each of 16 steps see fast if LED's from the 5 CD4067BE fields need to be ON
// all from 3 have at the end get all CD4067 flip flop setting pins up so another reset will work later


// 1 - set fast working variables from RAM interrupt friendly variables
ledhours = hours;
ledminutes = minutes;
ledday = day;
ledmonth = month;
ledyear = year;

// 2 - reset all LED flip flops turning them off

  PORTD &= ~ (1<<1);
  _delay_ms(10);
  PORTD |= (1<<1);

// 3 - through each of 16 steps see fast if LED's from the 5 CD4067BE fields need to be ON

// ESTABLISHING AN ORDER ACCORDING TO MY 7 SEGMENT DISPLAY PINOUT DIAGRAM SO THAT IT'S EASY TO CONNECT THEM

//            ------------------                           ------------------
//            |       01       |                           |       09       |
//            ------------------                           ------------------
//       ------                  ------              ------                  ------
//       |    |                  |    |              |    |                  |    |
//       |    |                  |    |              |    |                  |    |
//       |    |                  |    |              |    |                  |    |
//       | 03 |                  | 00 |              | 11 |                  | 08 |
//       |    |                  |    |              |    |                  |    |
//       |    |                  |    |              |    |                  |    |
//       |    |                  |    |              |    |                  |    |
//       ------                  ------              ------                  ------
//            ------------------                           ------------------
//            |       04       |                           |       12       |
//            ------------------                           ------------------
//       ------                  ------              ------                  ------
//       |    |                  |    |              |    |                  |    |
//       |    |                  |    |              |    |                  |    |
//       |    |                  |    |              |    |                  |    |
//       | 07 |                  | 05 |              | 15 |                  | 13 |
//       |    |                  |    |              |    |                  |    |
//       |    |                  |    |              |    |                  |    |
//       |    |                  |    |              |    |                  |    |
//       ------                  ------              ------                  ------
//             ------------------      ------              ------------------      ------
//             |       06       |      | 02 |              |       14       |      | 10 |
//             ------------------      ------              ------------------      ------
//
//      

  ledleftdigit1 = ledhours/10;
  ledrightdigit1 = ledhours%10;
  ledleftdigit2 = ledminutes/10;
  ledrightdigit2 = ledminutes%10;
  ledleftdigit3 = ledday/10;
  ledrightdigit3 = ledday%10;
  ledleftdigit4 = ledmonth/10;
  ledrightdigit4 = ledmonth%10;

  yearover100detected = 0;
  if(ledyear > 99)
	{
		yearover100detected = 1;
		ledyear = 100 - ledyear;
	}

  ledleftdigit5 = ledyear/10;
  ledrightdigit5 = ledyear%10;

//  For five fields it is required to control 16 LED's

//   five fields = 32 states
//   total states for all 16 led's = 32 x 16 = 512
//   So this will require some compile flash memory...

//  Left LED segment = 0 - 7

ledcurrentdigit1 = ledleftdigit1;
ledcurrentdigit2 = ledleftdigit2;
ledcurrentdigit3 = ledleftdigit3;
ledcurrentdigit4 = ledleftdigit4;
ledcurrentdigit5 = ledleftdigit5;
  ledstep = 0;
  updatecdselection();
  updatecdcommandled0();


  ledstep = 1;
  updatecdselection();
  updatecdcommandled1();

  ledstep = 2;
  updatecdselection();
  updatecdcommandled2();

  ledstep = 3;
  updatecdselection();
  updatecdcommandled3();

  ledstep = 4;
  updatecdselection();
  updatecdcommandled4();

  ledstep = 5;
  updatecdselection();
  updatecdcommandled5();

  ledstep = 6;
  updatecdselection();
  updatecdcommandled6();

  ledstep = 7;
  updatecdselection();
  updatecdcommandled7();

//  Right LED segment = 8 - 15

ledcurrentdigit1 = ledrightdigit1;
ledcurrentdigit2 = ledrightdigit2;
ledcurrentdigit3 = ledrightdigit3;
ledcurrentdigit4 = ledrightdigit4;
ledcurrentdigit5 = ledrightdigit5;

  ledstep = 8;
  updatecdselection();
  updatecdcommandled0();

  ledstep = 9;
  updatecdselection();
  updatecdcommandled1();

  ledstep = 10;
  updatecdselection();
  updatecdcommandled2();

  ledstep = 11;
  updatecdselection();
  updatecdcommandled3();

  ledstep = 12;
  updatecdselection();
  updatecdcommandled4();

  ledstep = 13;
  updatecdselection();
  updatecdcommandled5();

  ledstep = 14;
  updatecdselection();
  updatecdcommandled6();

  ledstep = 15;
  updatecdselection();
  updatecdcommandled7();

}

//////////////////////////////////////////////////////////////////////



void potentiometerdecreaseminutes() {
if(minutes == 0)
	minutes = 59;
else
	minutes--;

}

void potentiometerincreaseminutes() {
	minutes++;
	if(minutes > 59) minutes = 0;
}

void potentiometerdecreasehours() {
if(hours == 0)
	hours = 23;
else
	hours--;

}

void potentiometerincreasehours() {

	hours++;
	if(hours > 23) hours = 0;

}
/////////////////////////////////////////////////////////////////

void potentiometerdecreasedays() {
	day--;
	if((month%2 == 1 && month < 8) || (month%2 == 0 && month >= 8)) {
		if(day < 1) {
			day = 31;
		}
	}
	else if(month == 2) {
		if(year%4 == 0) {   // leap year
			if(day < 1) {
				day = 29;
			}
		}
		else {  // configuration for february, when it is not a leap year
			if(day < 1) {
			day = 28;
			}
		}
	}
	else {
			if(day < 1) {
				day = 30;
			}
		}

}

void potentiometerincreasedays() {

	day++;
	if((month%2 == 1 && month < 8) || (month%2 == 0 && month >= 8)) {
		if(day > 31) {
			day = 1;
		}
	}
	else if(month == 2) {
		if(year%4 == 0) {   // leap year
			if(day > 29) {
				day = 1;
			}
		}
		else {  // configuration for february, when it is not a leap year
			if(day > 28) {
				day = 1;
			}
		}
	}
	else {
		if(day > 30) {
			day = 1;
		}
	}

}

void potentiometerdecreasemonths() {
	month--;
	if(month < 1) {
		month = 12;
	}
	if((month%2 == 1 && month < 8) || (month%2 == 0 && month >= 8)) {
		if(day > 31) {
			day = 31;
		}
	}
	else if(month == 2) {
		if(year%4 == 0) {   // leap year
			if(day > 29) {
				day = 29;
			}
		}
		else {  // configuration for february, when it is not a leap year
			if(day > 28) {
				day = 28;
			}
		}
	}
	else {
		if(day > 30) {
			day = 30;
		}
	}

}


//////////////////////////////////////////////////////////////
void potentiometerincreasemonths() {

	month++;
	if(month > 12) {
		month = 1;
	}
	if((month%2 == 1 && month < 8) || (month%2 == 0 && month >= 8)) {
		if(day > 31) {
			day = 31;
		}
	}
	else if(month == 2) {
		if(year%4 == 0) {   // leap year
			if(day > 29) {
				day = 29;
			}
		}
		else {  // configuration for february, when it is not a leap year
			if(day > 28) {
				day = 28;
			}
		}
	}
	else {
		if(day > 30) {
			day = 30;
		}
	}

}

void potentiometerdecreaseyears() {
	if(year < 19) // year when the watch was made
		year = 199;	// 199 for 256 years - allow 255 reserve
	else
		year--;
	if(month == 2) {
		if(year%4 == 0) {   // leap year
			if(day > 29) {
				day = 29;
			}
		}
		else {  // configuration for february, when it is not a leap year
			if(day > 28) {
				day = 28;
			}
		}
	}

}

void potentiometerincreaseyears() {

	year++;
	if(year > 199) {	// 199 for 256 years - allow 255 reserve
		year = 19; // year when the watch was made
	}
	if(month == 2) {
		if(year%4 == 0) {   // leap year
			if(day > 29) {
				day = 29;
			}
		}
		else {  // configuration for february, when it is not a leap year
			if(day > 28) {
				day = 28;
			}
		}
	}

}



void potentiometercommandtime() {

////////////////////////////////////////////////////////////////////////////////////

//          0 -  31 = 32 steps = fast decrease
//         36 -  63 = 32 steps = normal decrease
//         72 -  79 = 32 steps = slow decrease

//        160 - 191 = 32 steps = slow increase
//        192 - 223 = 32 steps = normal increase
//        224 - 255 = 32 steps = fast increase

	if(pot_time < 32)
		{
			pointsfortime = 3;
			pot_time_at_zero = 1;
			potentiometerdecreasehours();
			//led_update_fields();
		}

	if((pot_time > 31) && (pot_time < 63))
		{
			pointsfortime = 2;
			pot_time_at_zero = 1;
			potentiometerdecreaseminutes();
			potentiometerdecreaseminutes();
			//led_update_fields();
		}
	if((pot_time > 64) && (pot_time < 80))
		{
			pointsfortime = 1;
			pot_time_at_zero = 1;
			potentiometerdecreaseminutes();
			//led_update_fields();
		}

	if((pot_time > 159) && (pot_time < 192))
		{
			pointsfortime = 2;
			pot_time_at_zero = 1;
			potentiometerincreaseminutes();
			//led_update_fields();
		}
	if((pot_time > 191) && (pot_time < 224))
		{
			pointsfortime = 1;
			pot_time_at_zero = 1;
			potentiometerincreaseminutes();
			potentiometerincreaseminutes();
			//led_update_fields();
		}

	if(pot_time > 223)
		{
			pointsfortime = 3;
			pot_time_at_zero = 1;
			potentiometerincreasehours();
			//led_update_fields();
		}

_delay_ms(100);
////////////////////////////////////////////////////////////////////////////////////

}

void potentiometercommanddate() {

////////////////////////////////////////////////////////////////////////////////////

//          0 -  31 = 32 steps = fast decrease
//         36 -  63 = 32 steps = normal decrease
//         72 -  79 = 32 steps = slow decrease

//        160 - 191 = 32 steps = slow increase
//        192 - 223 = 32 steps = normal increase
//        224 - 255 = 32 steps = fast increase

	if(pot_date < 32)
		{
			pointsfordate = 3;
			pot_date_at_zero = 1;
			potentiometerdecreaseyears();
			//led_update_fields();
		}

	if((pot_date > 31) && (pot_date < 63))
		{
			pointsfordate = 2;
			pot_date_at_zero = 1;
			potentiometerdecreasemonths();
			//led_update_fields();
		}
	if((pot_date > 64) && (pot_date < 80))
		{
			pointsfordate = 1;
			pot_date_at_zero = 1;
			potentiometerdecreasedays();
			//led_update_fields();
		}

	if((pot_date > 159) && (pot_date < 192))
		{
			pointsfordate = 2;
			pot_date_at_zero = 1;
			potentiometerincreasedays();
			//led_update_fields();
		}
	if((pot_date > 191) && (pot_date < 224))
		{
			pointsfordate = 1;
			pot_date_at_zero = 1;
			potentiometerincreasemonths();
			//led_update_fields();
		}

	if(pot_date > 223)
		{
			pointsfordate = 3;
			pot_date_at_zero = 1;
			potentiometerincreaseyears();
			//led_update_fields();
		}

_delay_ms(100);
////////////////////////////////////////////////////////////////////////////////////

}

int main(void) {

	_delay_ms(100); // time to make sure microcontroller power is stabilised (increased to 5V)
	setup();

	adc_value = 0; 
	//ADMUX = (1<<REFS0) | (1<<ADLAR) | (1<<MUX2) | (1<<MUX1);
	//ADMUX &= ~ (1<<MUX0);
	ADMUX |= 0b01100110;    //Channel 6
	_delay_ms(10); // Wait 10 ms for ADMUX to stay set to it's new value ( multiplexer/demultiplexer flip flops maybe not so fast)
	ADCSRA |= (1<<ADSC); After setting ADMUX, start ADC read
	// Start conversion for the first time for channel 6
	adc_channel = 6;  // in the while loop know which channel is being read first
	led_update_fields();   // update fields for the first time
	while(1)
    	{

			if (!(ADCSRA & (1<<ADSC))) // detect that ADSC is 0 and then do what is in the accolades
			{
				// wait until conversion completes; ADSC=0 means Complete
				adc_value = ADCH;
				//Store ADC result
				// Start conversion in loop
				if (adc_channel == 6)
					{
						pot_time = adc_value;
						// Set ADCSRA Register with division factor 128, channel 7
						//ADMUX = (1<<REFS0) | (1<<ADLAR) | (1<<MUX2) | (1<<MUX1) | (1<<MUX0);

						// https://www.avrfreaks.net/forum/atmega168-read-zero-adc6-and-adc7
						// [REFS1] [REFS0] [ADLAR] [  –  ] [MUX3] [MUX2] [MUX1] [MUX0]
						// all zero initially
						// Bit 4 – Res: Reserved Bit
						// This bit is an unused bit in the Atmel ® ATmega328P, and will always read as zero.
						//ADMUX &= 0b11110000;    //ADC channel selection.
						ADMUX |= 0b01100111;    //Channel 7
						adc_channel = 7;
					}
				if (adc_channel == 7)
					{
						pot_date = adc_value;
						// Set ADCSRA Register with division factor 128, channel 6
						//ADMUX &= ~ (1<<MUX0);
						//ADMUX = (1<<REFS0) | (1<<ADLAR) | (1<<MUX2) | (1<<MUX1);
						//ADMUX &= 0b11110000;    //ADC channel selection.
						ADMUX |= 0b01100110;    //Channel 6
						adc_channel = 6;
					}
				_delay_ms(10); // Wait 10 ms for ADMUX to stay set to it's new value ( multiplexer/demultiplexer flip flops maybe not so fast)
				ADCSRA |= (1<<ADSC); // After setting ADMUX, start ADC read
			}


////////////  ACT ON POTENTIOMETER COMMANDS

//  - 2 pins ADC6 and ADC7 for potentiometer 8 bit field value change 0 to 255, pins are a little harder to connect to:
//          0 -  31 = 32 steps = fast decrease
//         36 -  63 = 32 steps = normal decrease
//         72 -  79 = 32 steps = slow decrease

//         80 - 159 = 80 steps = keep constant but display how far from the middle the potentiometer is

//        10 - 10 - [[[ 40 ]]] - 10 - 10  steps:  80 - 90 - [[[ 100 - 140 ]]] - 150 - 160

//        3|0111 = [ 0 - 79 ]   2|0011 = [ 80 - 93 ]   1|0001 = [ 94 - 107 ]   0|0000 = [[[ 108 - 131 ]]]   6|1000 = [ 132 - 145 ]   5|1100 = [ 146 - 159 ]   4|1110 = [ 160 - 255 ]

//        160 - 191 = 32 steps = slow increase
//        192 - 223 = 32 steps = normal increase
//        224 - 255 = 32 steps = fast increase

	if(pot_time < 80)
		{
			pointsfortime = 3;
			pot_time_at_zero = 1;
			potentiometercommandtime();
			led_update_fields();
		}

	if((pot_time > 79) && (pot_time < 90))
		{
			pointsfortime = 2;
			pot_time_at_zero = 1;
			led_update_fields();
		}
	if((pot_time > 89) && (pot_time < 100))
		{
			pointsfortime = 1;
			pot_time_at_zero = 1;
			led_update_fields();
		}
	if((pot_time > 99) && (pot_time < 140))
		{
			pointsfortime = 0;
			if(pot_time_at_zero == 1)
			{
				pot_time_at_zero = 0;
				led_update_fields();
			}
		}
	if((pot_time > 139) && (pot_time < 150))
		{
			pointsfortime = 6;
			pot_time_at_zero = 1;
			led_update_fields();
		}
	if((pot_time > 149) && (pot_time < 160))
		{
			pointsfortime = 5;
			pot_time_at_zero = 1;
			led_update_fields();
		}
	if(pot_time > 159)
		{
			pointsfortime = 4;
			pot_time_at_zero = 1;
			potentiometercommandtime();
			led_update_fields();
		}

//          0 -  31 = 32 steps = fast decrease
//         36 -  63 = 32 steps = normal decrease
//         72 -  79 = 32 steps = slow decrease

//         80 - 159 = 80 steps = keep constant but display how far from the middle the potentiometer is

//        10 - 10 - [[[ 40 ]]] - 10 - 10  steps:  80 - 90 - [[[ 100 - 140 ]]] - 150 - 160

//        3|0111 = [ 0 - 79 ]   2|0011 = [ 80 - 93 ]   1|0001 = [ 94 - 107 ]   0|0000 = [[[ 108 - 131 ]]]   6|1000 = [ 132 - 145 ]   5|1100 = [ 146 - 159 ]   4|1110 = [ 160 - 255 ]

//        160 - 191 = 32 steps = slow increase
//        192 - 223 = 32 steps = normal increase
//        224 - 255 = 32 steps = fast increase

	if(pot_date < 80)
		{
			pointsfordate = 3;
			pot_date_at_zero = 1;
			potentiometercommanddate();
			led_update_fields();
		}

	if((pot_date > 79) && (pot_date < 90))
		{
			pointsfordate = 2;
			pot_date_at_zero = 1;
			led_update_fields();
		}
	if((pot_date > 89) && (pot_date < 100))
		{
			pointsfortime = 1;
			pot_time_at_zero = 1;
			led_update_fields();
		}
	if((pot_date > 99) && (pot_date < 140))
		{
			pointsfordate = 0;
			if(pot_date_at_zero == 1)
			{
				pot_date_at_zero = 0;
				led_update_fields();
			}
		}
	if((pot_date > 139) && (pot_date < 150))
		{
			pointsfordate = 6;
			pot_date_at_zero = 1;
			led_update_fields();
		}
	if((pot_date > 149) && (pot_date < 160))
		{
			pointsfordate = 5;
			pot_date_at_zero = 1;
			led_update_fields();
		}
	if(pot_date > 159)
		{
			pointsfordate = 4;
			pot_date_at_zero = 1;
			potentiometercommanddate();
			led_update_fields();
		}



//	if((pot_date < 64) || (pot_date > 192))
//		{
//			pointsfordate = 0;
//		}

///////////  ACT ON INTERRUPT DETECTED AT THE PASS OF A MINUTE

	if((interruptdetected == 1) && (seconds == 0))
		{
			interruptdetected = 0;
			led_update_fields();
		}


	_delay_ms(250);		//  simulate slow / lazy program ( so the microcontroller does not get hot and decreases poissible battery lifetime ) 


	} // acolade from while(1)

	return 0;
}

/*Timer Counter 1 Compare Match A Interrupt Service Routine/Interrupt Handler*/
ISR(TIMER1_COMPA_vect)
{
	interruptdetected = 1;
	seconds++;
	if(pot_time_at_zero == 1) seconds = 0; // when potentiometer not at middle reset seconds to zero
	if(seconds > 59)
	{
		seconds = 0;
		minutes++;
	}
	if(minutes > 59)
	{
		minutes = 0;
		hours++;		
	}
	if(hours > 23) {
		hours = 0;
		day++;
	}
	if((month%2 == 1 && month < 8) || (month%2 == 0 && month >= 8)) {
		if(day > 31) {
			day = 1;
			month++;
		}
	}
	else if(month == 2) {
		if(year%4 == 0) {   // leap year
			
			if(day > 29) {
				day = 1;
				month++;
			}
		}
		else {  // configuration for february, when it is not a leap year
			if(day > 28) {
				day = 1;
				month++;
			}
		}
	}
	else {
		if(day > 30) {
			day = 1;
			month++;
		}
	}
	if(month > 12) {
		month = 1;
		year++;
	}
	if(year > 199) {	// 199 years - allow 255 reserve
		year = 19; // year when the watch was made
	}
}
