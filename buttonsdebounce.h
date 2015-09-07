// buttonsdebounce.h for avr microcontrollers
// Version 0.02
// License GPL v 3.0 or later
/* Usage in a program including this buttonsdebounce.h:
 * #####################################################################
// avr-gcc -mmcu=atmega32 -Wall -Os -o testbuttons.elf testbuttons.c

#include <avr/io.h>		// provides avr ports and registers
#include <avr/interrupt.h>		//provides avr interrupts sei()
#include <stdint.h>		// provides uint8_t uint16_t uint32_t
#include <stdio.h>		// provides sprintf() number in lcd string
#include <stdlib.h>		// provides abs()

#define F_CPU 4000000
#define NUMBEROFBUTTONS 4	// a fixed value, less than 9
#define PULLBUTTONS PORTD	// port for pull up 1 or down 0
#define SELECTBUTTONS DDRD	// register for inputs 0 or outputs 1
#define INPUTBUTTONS PIND	// port for reading inputs
#define WHILECLOCKCOUNT 40	// ~ nr. of instructions done in while
#define NOISEPERCENTMAX 20	// 0-40% max noise avoid deriv to middle

unsigned char whichbutton=0;
unsigned char buttonreleased[NUMBEROFBUTTONS];	// many passes
unsigned char buttonpressed[NUMBEROFBUTTONS];	// only on one pass
unsigned char pressedconfidence[NUMBEROFBUTTONS];	// ..2-100-198..
uint16_t buttonderivcount[NUMBEROFBUTTONS];
uint16_t buttonderivratio = 0;	// value given in main
uint16_t clocksfortenms = 0;	// value given in main
int buttonconfidence = 0;	// value given in main
int upperoverflowconf = 0;	// value given in main
int upperconfidence = 0;	// value given in main
// buttonconfidence is number of passes through while to reach
// 10 ms as many times as noise at max percent requires
// buttonderivratio passes to change pressedconfidence by 1 overall

#include <util/delay.h>	// provides delay_ms() delay_us()
#include "buttonsdebounce.h"	// provides pollbuttons()

int main(void)
{
	clocksfortenms = F_CPU/10000;
	buttonderivratio = (NOISEPERCENTMAX/10)+4; // experimentally
	buttonconfidence = clocksfortenms/WHILECLOCKCOUNT*buttonderivratio;	// 10 ms
	upperoverflowconf = buttonconfidence*2;
	upperconfidence = upperoverflow-3;
	for (whichbutton=0;whichbutton<NUMBEROFBUTTONS;whichbutton++)
	// initialize each button variables with values
	{
		buttonreleased[whichbutton] = 1;
		buttonpressed[whichbutton] = 0;
		pressedconfidence[whichbutton] = 100;
		buttonderivcount[whichbutton] = 0;
	}
	while (1)
	{
		pollbuttons();	// at the beginning of the loop
		//[...]
		if (buttonpressed[1] != 0)
		{
			_delay_ms(100);
		}
		//[...]
	}
}
 * #####################################################################
 */

/* How it works:
 * Statistically poll the buttons at every while pass from main program.
 * When confident that button is pressed, make buttonpressed[whichbutton]
 * 1 only for one pass then keep at zero until the button is confidently
 * released and confidently pressed again.
 * pressconfidence[whichbutton] needs to reach 0 for released and 200
 * for pressed in a span of 10 ms, it will happen a little faster for
 * less noise and never while noise larger than NOISEPERCENTMAX as
 * pressconfidence[whichbutton] is getting faster to 100 because of
 * buttonderivratio than pressed or released with over max noise.
 * buttonpressed[whichbutton] is the one to be read from main program.
 * For more button ports functions with different names are needed.
 */
 
void pollbuttons(void);		// when it's called should be already declared

void pollbuttons(void)
{
	// 0 <= whichbutton < NUMBEROFBUTTONS same as pin number of port
	for (whichbutton=0;whichbutton<NUMBEROFBUTTONS;whichbutton++)
	// for each of the button independently
	{
		buttonpressed[whichbutton] = 0;		// new pass so not pressed
		// pressed is 1 on one pass after it is released then pressed
		buttonderivcount[whichbutton]++;	// count derivation
		if(buttonderivcount[whichbutton] > buttonderivratio)
		// derivation has reached its ratio so derivate
		{
			buttonderivcount[whichbutton] = 0;	// reset derivation count
			if (pressedconfidence[whichbutton] < buttonconfidence)
				pressedconfidence[whichbutton]++;	// closer to 100 by 1
			// pressedconfidence[whichbutton] == 100 remains at 100
			if (pressedconfidence[whichbutton] > buttonconfidence)
				pressedconfidence[whichbutton]--;	// closer to 100 by 1
		}
		// checking the actual noisy state of the button and setting statistics
		if(INPUTBUTTONS &=~(1 << whichbutton))
			pressedconfidence[whichbutton]--;	// more confident the button is released
		if(INPUTBUTTONS |= (1 << whichbutton))
			pressedconfidence[whichbutton]++;	// more confident the button is pressed
		// checking for confidence being reached and that it has been the other way before
		if((pressedconfidence[whichbutton] < 3) && (buttonreleased[whichbutton] == 0))
		// <<<<< confident that button is released after it has been pressed
		{
			buttonreleased[whichbutton] = 1;		// the button is considered released many passes
			pressedconfidence[whichbutton] = buttonconfidence;	// and not confident any more
			// that it is pressed, already done up, new pass so not pressed
		}
		if((pressedconfidence[whichbutton] > upperconfidence) && (buttonreleased[whichbutton] == 1))
		// >>>>> confident that button is pressed after it has been released
		{
			buttonpressed[whichbutton] = 1;			// the button is considered pressed this pass only
			pressedconfidence[whichbutton] = buttonconfidence;	// and not confident any more
			buttonreleased[whichbutton] = 0;		// that it is released
		}
		// in case it has not been the other way before don't let confidence overflow by reseting it
		// after three tries (0 1 2 ------ 198 199 200)
		if(pressedconfidence[whichbutton] < 1)
		// <<<<< confident that button is released after it has been released, try again...
		{
			pressedconfidence[whichbutton] = buttonconfidence;	// not confident any more
		}
		if(pressedconfidence[whichbutton] > upperoverflowconf)
		// >>>>> confident that button is pressed after it has been pressed, try again...
		{
			pressedconfidence[whichbutton] = buttonconfidence;	// not confident any more
		}
	}
}
