#ifndef __TIMER1_H
#define __TIMER1_H

#include <avr/io.h>

// Normal mode: counts from 0 to 65535 then repeats
//#define TIMER1_MODE0

// CTC (clear time on compare) mode: counts from 0 to OCR1A
#define TIMER1_MODE4

// CTC (clear timer on compare) mode: counts from 0 to ICR1
//#define TIMER1_MODE12 

// 250Hz at 16MHz
#define TIMER1_TOP 63999

// Enable interrupts
#define TIMER1_ENABLE_INT_OCIE1A
//#define TIMER1_ENABLE_INT_OCIE1B
//#define TIMER1_ENABLE_INT_TOIE1

// Clock divider
#define TIMER1_CLK_DIV_1
//#define TIMER1_CLK_DIV_8
//#define TIMER1_CLK_DIV_64
//#define TIMER1_CLK_DIV_1024


// -------------------------------------------------

void timer1_init();
void timer1_start();
void timer1_stop();

#endif
