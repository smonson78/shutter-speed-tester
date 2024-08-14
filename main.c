#include <string.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay_basic.h>
#include <stdint.h>

#include "serial.h"
#include "timer1.h"
#include "timer2.h"

#define MAX_BUF 1500

#define DELAY_1MS do{_delay_loop_2(F_CPU/4000);}while(0)

// Digital 11 (Variable Spindle PWM) is PB3 (OC2A)
// Digital 2 (Step Pulse X Axis) is PD2
// Digital 3 (Step Pulse Y Axis) is PD3
// Digital 5 (Direction X Axis) is PD5
// Digital 6 (Direction Y Axis) is PD6
// Digital 8 (Enable) is PB0
// Digital 9 (Limit X Axis) is PB1
// Digital 10 (Limit Y Axis) is PB2

void delay(int time)
{
   while (time--)
   {
       /* 1msec delay */
       DELAY_1MS;
   }
}

volatile uint8_t running = 0;
ISR(TIMER1_COMPA_vect)
{
}

ISR(TIMER1_COMPB_vect)
{
}

void setup()
{
    timer1_init();
    timer2_init();
    serial_init();

    // ------ output pins -------
    // Stepper enable (PB0) + Laser enable (PB3)
    //PORTB = 0b00000001; // initial state: stepper drivers disabled
    //DDRB = _BV(PORTB0) | _BV(PORTB3);
    
    // X and Y Axis step (PD2 and PD3) + X and Y axis direction (PD5 and PD6)
    //PORTD = 0b00000000;
    //DDRD = _BV(PORTD2) | _BV(PORTD3) | _BV(PORTD5) | _BV(PORTD6);
    
    sei();
}

void main_loop()
{
    serial_send("Serial port working\n\r");

    delay(500);
}

int main()
{
    setup();
   
    while (1) {
        main_loop();
    }

    return 0;
}
