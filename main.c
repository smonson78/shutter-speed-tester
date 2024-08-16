#include <string.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>
#include <util/delay_basic.h>
#include <stdint.h>

#include "serial.h"
#include "timer1.h"
#include "timer2.h"

#define DELAY_1MS do { _delay_loop_2(F_CPU/4000); } while(0)

void delay(int time)
{
   while (time--)
   {
       /* 1msec delay */
       DELAY_1MS;
   }
}

// Clock runs at 1/250th of a second and TIMER1 counter will reach 63999 in this time at 16MHz
volatile uint32_t clock;
ISR(TIMER1_COMPA_vect)
{
    clock++;
}

void serial_send_number(uint32_t number, uint8_t pad) {
    uint8_t buf[16];
    uint8_t len = 0;
    if (number == 0) {
        while (pad-- > 1) {
            serial_sendchar('0');            
        }
        serial_sendchar('0');
        return;
    }
    while (number) {
        buf[len++] = (number % 10) + '0';
        number /= 10;
    }
    while (pad-- > len) {
        serial_sendchar('0');        
    }
    while (len) {
        serial_sendchar(buf[--len]);
    }
}

void setup()
{
    timer1_init();
    timer1_start();
    //timer2_init();
    serial_init();

    // A2, A3 and A4 in Arduino numbering are PORTC2, PORTC3, PORTC4 
    
    sei();
}

// Pin states for detecting changes
uint8_t left = _BV(2), centre = _BV(4), right = _BV(3);

// Whether a capture is running or not
uint8_t running = 0;

// Start times
uint32_t left_start, centre_start, right_start;
// End times
uint32_t left_end, centre_end, right_end;

// Detected
uint8_t left_seen = 0, centre_seen = 0, right_seen = 0;

uint32_t build_time(uint32_t time1, uint16_t time2) {
    // Divide by 16 so we're dealing with 1uS units
    time2 >>= 4;
    return (time1 * 4000) + time2;
}

// decimals must be at least 1 or else this needs rewriting
void serial_send_decimal(uint32_t num, uint32_t decimals) {
    uint32_t integer_part = num;
    uint32_t decimal_part_modulus = 1;
    for (uint8_t i = decimals; i > 0; i--) {
        integer_part /= 10;
        decimal_part_modulus *= 10;
    }

    serial_send_number(integer_part, 0);
    serial_sendchar('.');
    serial_send_number(num % decimal_part_modulus, decimals);
}

void display_stats() {
    // Direction of curtain travel
    uint8_t dir = left_start < right_start;

    // If a leaf shutter, only display central shutter time
    uint8_t leaf = 0;
    if (!left_seen || !right_seen) {
        leaf = 1;
    }

    if (leaf) {
        serial_send("(Leaf shutter) open time: ");
        serial_send_decimal(centre_end - centre_start, 3);
        serial_send("ms [1/");
        serial_send_decimal(10000000.0 / (centre_end - centre_start), 1);
        serial_send("]\n\r");
    } else {
        serial_send("(Horizontal plane shutter)\n\rOpen time: L: ");
        serial_send_decimal(left_end - left_start, 3);
        serial_send("ms - C: ");
        serial_send_decimal(centre_end - centre_start, 3);
        serial_send("ms - R: ");
        serial_send_decimal(right_end - right_start, 3);
        serial_send("ms [1/");
        serial_send_decimal(10000000.0 / (centre_end - centre_start), 1);
        serial_send("]\n\r");

        serial_send("Curtain 1 transit time: ");
        serial_send_decimal(dir ? right_start - left_start : left_start - right_start, 3);
        serial_send("ms\n\r");

        serial_send("Curtain 2 transit time: ");
        serial_send_decimal(dir ? right_end - left_end : left_end - right_end, 3);
        serial_send("ms\n\r");
    }
}

void main_loop()
{
    // LOW means light, HIGH means dark
    uint8_t new_left = PINC & _BV(2);
    uint8_t new_centre = PINC & _BV(4);
    uint8_t new_right = PINC & _BV(3);

    cli();
    uint32_t time1 = clock;
    uint32_t time2 = TCNT1;
    sei();    

    // Light falls one one of the sensors in idle state
    if (!running && (!new_left || !new_centre || !new_right)) {
        // Bring the clock down to zero
        cli();
        clock = 0;
        sei();
        time1 = 0;

        // Start a new capture
        running = 1;
        left_seen = 0;
        centre_seen = 0;
        right_seen = 0;

        serial_send("---\n\r");
    }

    // Pin changes:

    if (new_left != left) {
        if (new_left) {
            left_end = build_time(time1, time2);
            left_seen = 1;
        } else {
            left_start = build_time(time1, time2);
        }
        left = new_left;
    }

    if (new_centre != centre) {
        if (new_centre) {
            centre_end = build_time(time1, time2);
            centre_seen = 1;
        } else {
            centre_start = build_time(time1, time2);
        }
        centre = new_centre;
    }

    if (new_right != right) {
        if (new_right) {
            right_end = build_time(time1, time2);
            right_seen = 1;
        } else {
            right_start = build_time(time1, time2);
        }
        right = new_right;
    }

    // All sensors have transitioned to dark in busy state
    if (running && left_seen && centre_seen && right_seen) {
        // End of capture
        display_stats();
        running = 0;
    }

    // 2 seconds have passed in busy state
    if (running && time1 > 500) {
        serial_send("Timed out\n\r");
        display_stats();
        running = 0;
    }
}

int main()
{
    setup();
   
    serial_send("Start\n\r");
    while (1) {
        main_loop();
    }

    return 0;
}
