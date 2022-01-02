
// All libraries needed for the assignment
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <board/SevenSegmentDisplay.h> /* Provide access to 7 segment display functions */
#include <atmel/pit.h>
#include <atmel/aic.h>
#include <config/AT91SAM7S256.h>   // ARM board configurations
#include <board/Button-lib.h>  /*Provides access to buttons*/
#include <board/LED-lib.h> /* Provide access to LEDs */

/* The digits on the Seven Segment LED Display are numbered right to left. DISPLAY1 is the right most digit.
 */
#define SECONDS				DISPLAY4   // leftmost display
#define TENSOFSECONDS	    DISPLAY3   // second display from left
#define HUNDREDSOFSECONDS   DISPLAY2   // third display from left
#define MILLISECONDS		DISPLAY1  // rightmost display



/* A routine to delay in milliseconds*/
void delay_ms(short ms)
{
	volatile short loop;

	while (ms-- > 0)
		for (loop = 0; loop < 2100; loop++);
}


 //The array Digit contains the binary patterns to display the digits 0 - 9 to the Seven Segment LED Display.
 // Digit[0] is the pattern for zero, Digit[1] is the pattern for one, etc.
short Digit[] = { 0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7c, 0x07, 0x7f, 0x67 };

/* The array Bars contains the binary patterns to display a single bar on a display. There are seven entries,
 * one for each bar in the display. */
short Bars[] = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40 };
short ray[] = {BUTTON5, BUTTON1, BUTTON2, BUTTON3, BUTTON4, BUTTON5, BUTTON6, BUTTON7, BUTTON8, BUTTON7,BUTTON8};  // Button array
short ledray[] = {LED5, LED1,LED2,LED3,LED4,LED5,LED6,LED7,LED8,LED7,LED8};   // LED array
k = 0;   // random number to be used to set LED/Button choice
short seconds, hertz, tensofseconds, hundredsofseconds, milliseconds;   // time variables
/*------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------------------*/
/*
 * Interrupt Routine
 *
 * This routine is called for every PIT interrupt
 */
void ISR_System(void)
{
	/*
	 * Read the PIT status register to clear the pending interrupt.
	 * We are not interested in the value so the result does not have to be assigned to anything.
	 */
    PIT_GetPIVR();

    /*
     * Now Process the interrupt.
     *
     * 5000 interrupts equals one second
     *
     * Incrementing the timer is done here
     */
    if (++hertz == 5000)
    {
    	hertz = 0;
			if (++seconds == 10)
				seconds = 0;
    }
	if ((hertz % 5) == 0)
	{
		if (++milliseconds == 10)
		{
			milliseconds = 0;
				if (++hundredsofseconds ==10)
				{
					hundredsofseconds = 0;
						if (++tensofseconds == 10)
						{
							tensofseconds = 0;
						}

				}
		}

	}
}

#define PIT_PERIOD          200 // interrupt at (5000Hz)

/* Main function - Main program is executed here */
int main()
{

	/* First we need to configure the additional functionality required */
	Configure7SegmentDisplay();

	/*
	 * Reset all the time related values to zero
	 */
	hertz = 0;   // frequency
	seconds = 0;     // time in seconds
	tensofseconds = 0;    // time in tenth of a second
	hundredsofseconds = 0;    // time in hundredth of a second
	milliseconds = 0;   // time in milliseconds


	/*
	 * Reset the Seven Segment Display.
	 */
	Set7SegmentDisplayValue(SECONDS, Digit[0]); // Displays the digit 0
	Set7SegmentDisplayValue(TENSOFSECONDS, Digit[0]); // Displays the digit 0
	Set7SegmentDisplayValue(HUNDREDSOFSECONDS, Digit[0]);  // Displays the digit 0
	Set7SegmentDisplayValue(MILLISECONDS, Digit[0]); // Displays the digit 0


	/*
	 * Initialize and enable the PIT
	 *
	 * First argument (PIT_PERIOD) is number of milliseconds between timer interrupts (maximum 20 bit number)
	 * Second argument is clock rate in MHz (BOARD_MCK is in Hz so divide by 1000000)
	 *
	 */

	PIT_Init(PIT_PERIOD, BOARD_MCK / 1000000);     //main clock

	/*
	 * Disable the interrupt on the interrupt controller
	 */
	AIC_DisableIT(AT91C_ID_SYS);

	/*
	 * Configure the AIC for PIT interrupts
	 *
	 * The PIT is a System interrupt, so we need to associate the ID of the System interrupt with the interrupt
	 * routine ISR_System.
	 */

	AIC_ConfigureIT(AT91C_ID_SYS, 0, ISR_System);

	/*
	 * Enable the interrupt on the interrupt controller
	 */

	AIC_EnableIT(AT91C_ID_SYS);

	/*
	 * Enable the PIT interrupt and start the PIT
	 *
	 * The interrupt handler is always assigned before the interrupt is enabled.
	 * Using the library function the PIT interrupt is enabled separately from the PIT itself. the interrupt
	 * is enabled first so none are missed when the timer is enabled.
	 */
    PIT_EnableIT();
    PIT_Enable();




    // declare variable to store LED number
    LEDnumber pos;




	/* Starts the display update to run. */
    while(1)
    {

    	// this loop causes LED 1 to flash until button 1 is pressed and the countdown begins
    	while(IsButtonReleased(BUTTON1))
    	{
    			SetLEDcolor(LED1, RED);
    			delay_ms(250);
    			SetLEDcolor(LED1, GREEN);
    			delay_ms(250);
    	}


    	// initialize pos to LED 8
    	pos = LED8;

    	// this sets the LEDs to light up after button 1 is pressed
    	if(IsButtonPressed(BUTTON1))
    	{
    		SetAllLEDs(LEDsAllRed);
    		delay_ms(250);

    	}

    	// loop that runs when the button is pressed and starts the LED countdown
    	for(pos = 8; pos >= 0; pos--)
    	{
    		while(IsButtonPressed(BUTTON1) || IsButtonPressed(BUTTON2) ||IsButtonPressed(BUTTON3) ||IsButtonPressed(BUTTON4) ||IsButtonPressed(BUTTON5) ||IsButtonPressed(BUTTON6) ||IsButtonPressed(BUTTON7) ||IsButtonPressed(BUTTON8))
    		{
    			pos = pos;
    		}
    		delay_ms(250);
    		SetLEDcolor(pos, OFF);  // turns of the LED after button press
    	}

    	// re-initialize the display values to zero before the loop runs
    	hertz = 0;
    	seconds = 0;
    	tensofseconds = 0;
    	hundredsofseconds = 0;
    	milliseconds = 0;

    	// set the randomly selected LED to green
    	SetLEDcolor(ledray[k], GREEN);

    	/* this loop runs until the selected random led button is pressed*/
    	while(IsButtonReleased(ray[k]))
		{
    		Set7SegmentDisplayValue(DISPLAY4, Digit[seconds]+0x80);
			Set7SegmentDisplayValue(DISPLAY3, Digit[tensofseconds]);
			Set7SegmentDisplayValue(DISPLAY2, Digit[hundredsofseconds]);
			Set7SegmentDisplayValue(DISPLAY1, Digit[milliseconds]);

			if(milliseconds == 9 && hundredsofseconds == 9 && tensofseconds == 9 && seconds == 9)
						{
											milliseconds = 9;
											break;
						}
		}

		SetLEDcolor(ledray[k], OFF);
		k = tensofseconds;
    }
}
