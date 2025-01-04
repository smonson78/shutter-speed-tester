#ifndef __TIMER1_H
#define __TIMER1_H

#include <avr/io.h>

// Normal mode: counts from 0 to 65535 then repeats
//#define TIMER1_MODE0

// CTC (clear time on compare) mode: counts from 0 to OCR1A
//#define TIMER1_MODE4

// CTC (clear timer on compare) mode: counts from 0 to ICR1
//#define TIMER1_MODE12 

// Phase & frequency correct mode: counts from 0 to ICR1 then back down again
#define TIMER1_MODE8

// 5kHz at 16MHz (because it does it twice)
#define TIMER1_TOP 1600

// Enable interrupts
//#define TIMER1_ENABLE_INT_OCIE1A
//#define TIMER1_ENABLE_INT_OCIE1B
//#define TIMER1_ENABLE_INT_TOIE1

// Clock divider
#define TIMER1_CLK_DIV_1
//#define TIMER1_CLK_DIV_8
//#define TIMER1_CLK_DIV_64
//#define TIMER1_CLK_DIV_1024

// Enable pin output on OC1A
#define TIMER1_OC1A_OUTPUT


// -------------------------------------------------

void timer1_init();
void timer1_start();
void timer1_stop();

#endif
