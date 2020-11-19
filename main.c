#include <msp430.h> 
#define ACLK 0x0100     // Timer_A ACLK source
#define BCLK 0x0200     // Timer_B BCLK source
#define UP 0x0010       // Timer_A UP mode

/************************************************************************************
 * Purpose:
 * 	This exemplifies the use of Interrupts (instead of polling) and ISRs 
 * 	(Interrupt Service Routines). The main fxn sets up all the buttons and LEDs.
 * 	The Interrupt vectors are at the bottom and self-explanatory.
 * Returns:
 * 	None
 * Parameters:
 * 	None
 * Notes:
 * 	None
 ***********************************************************************************/
int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// Stop Watchdog timer
	PM5CTL0 &= ~LOCKLPM5;       	// Enable inputs and outputs

	// Setup clock 1
	TA0CCR0 = 14043;                // Count to 3 seconds
	TA0CTL = ACLK | MC__UP | ID__8; // Use AuxClock, up mode, Change clockRate
	TA0CCTL0 = CCIE;                // Enable interrupt for Timer_0

	P1DIR |= BIT0;                  // set red LED as output
	P1OUT |= BIT0;                  // red LED on

	// Setup clock 2
	TA1CCR0 = 4681;			// Count to 1 second. 
	TA1CTL = ACLK | MC__UP | ID__8;
	TA1CCTL0 = CCIE;

	P9DIR |= BIT7;                  // set green LED as output
	P9OUT |= BIT7;                  // green LED on

	// Configure GPIO (Buttons)
	P1DIR &= ~BIT1;             	// direct button1 pin as out
	P1DIR &= ~BIT2;             	// direct button2 pin as out

	P1OUT = BIT1;
	P1OUT |= BIT2;

	P1REN = BIT1;
	P1REN |= BIT2;

	P1IES = BIT1;       		// P1.1 Hi/Lo edge
	P1IFG = 0;          		// Clear all P1 interrupt flags
	P1IE = BIT1;        		// P1.1 interrupt enabled

	_BIS_SR(GIE);                   // Activate enabled interrupts
	
	while(1);                       // wait for interrupt
}

// Timer0 Interrupt Service Routine
#pragma vector=TIMER0_A0_VECTOR // ISR put in controller program memory
__interrupt void Timer0_ISR (void)
{
    P1OUT ^= BIT0; // Toggle red LED
}

#pragma vector=TIMER1_A0_VECTOR // ISR put in controller program memory
__interrupt void Timer1_ISR (void)
{
    P9OUT ^= BIT7; // Toggle green LED
}

#pragma vector=PORT1_VECTOR
__interrupt void Port_1 (void)
{
    if((BIT1 & P1IN) == 0) {// Is P1.1 button pushed?
        P1OUT ^= BIT0;          //   Red LED Toggle
    }

    if((BIT2 & P1IN) == 0) {
        P9OUT ^= BIT7;         // Green LED Toggle
    }
}
