#include "timer1.h"

void timer1_init()
{
	// Set timer mode
#if defined TIMER1_MODE0
	// Normal mode
	TCCR1A = 0;
	TCCR1B = 0;
#elif defined TIMER1_MODE4
	// Clear timer on compare (OCR1A)
	TCCR1A = 0;
	TCCR1B = _BV(WGM12);
	OCR1A = TIMER1_TOP;
#elif defined TIMER1_MODE12
	// Clear timer on compare (ICR1)
	TCCR1A = 0;
	TCCR1B = _BV(WGM13) | _BV(WGM12);
	ICR1 = TIMER1_TOP;
#endif

	// Enable selected interrupts
	TIMSK1 |= 0
#if defined TIMER1_ENABLE_INT_OCIE1A
		| _BV(OCIE1A)
#endif
#if defined TIMER1_ENABLE_INT_OCIE1B
		| _BV(OCIE1B)
#endif
#if defined TIMER1_ENABLE_INT_TOIE1
		| _BV(TOIE1)
#endif
		;
}

void timer1_start()
{
	// Restart from 0
	TCNT1 = 0;

	/* Set prescaler to start timer */
#if defined TIMER1_CLK_DIV_1
	TCCR1B |= _BV(CS10); // Clock/1 (no prescaling)
#elif defined TIMER1_CLK_DIV_8
	TCCR1B |= _BV(CS11); // Clock/8
#elif defined TIMER1_CLK_DIV_64
	TCCR1B |= _BV(CS10) | _BV(CS11);
#elif defined TIMER1_CLK_DIV_1024
	TCCR1B |= _BV(CS10) | _BV(CS12);
#endif
}

void timer1_stop()
{
	/* Unset prescaler bits to stop timer */
	TCCR1B &= ~(_BV(CS10) | _BV(CS11) | _BV(CS12));
}

