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
//volatile uint32_t clock;
//ISR(TIMER1_COMPA_vect)
//{
//    clock++;
//}

// void serial_send_number(uint32_t number, uint8_t pad) {
//     uint8_t buf[16];
//     uint8_t len = 0;
//     if (number == 0) {
//         while (pad-- > 1) {
//             serial_sendchar('0');            
//         }
//         serial_sendchar('0');
//         return;
//     }
//     while (number) {
//         buf[len++] = (number % 10) + '0';
//         number /= 10;
//     }
//     while (pad-- > len) {
//         serial_sendchar('0');        
//     }
//     while (len) {
//         serial_sendchar(buf[--len]);
//     }
// }

void setup()
{
    timer1_init();
    timer1_start();
    //timer2_init();
    serial_init();

    // Output pin for PWM is B1
    DDRB |= _BV(1);

    // Input pins with pullup enabled for selector knob
    PORTD |= _BV(2) | _BV(3) | _BV(4) | _BV(5) | _BV(6) | _BV(7);
    PORTB |= _BV(0);

    DDRC &= ~(_BV(1) | _BV(2) | _BV(3) | _BV(4) | _BV(5));
    PORTC |= _BV(1) | _BV(2) | _BV(3) | _BV(4) | _BV(5);

    sei();
};

uint8_t last_setting = 0;

int get_selector() {
    if ((PIND & _BV(2)) == 0) {
        return 1;
    } else if ((PIND & _BV(3)) == 0) {
        return 2;
    } else if ((PIND & _BV(4)) == 0) {
        return 3;
    } else if ((PIND & _BV(5)) == 0) {
        return 4;
    } else if ((PIND & _BV(6)) == 0) {
        return 5;
    } else if ((PIND & _BV(7)) == 0) {
        return 6;
    } else if ((PINB & _BV(0)) == 0) {
        return 7;
    } else if ((PINC & _BV(3)) == 0) {
        // Was on A7, PC7
        return 8;
    } else if ((PINC & _BV(4)) == 0) {
        // Was on A6, PC6
        return 9;
    } else if ((PINC & _BV(5)) == 0) {
        return 10;
    } else if ((PINC & _BV(2)) == 0) {
        return 11;
    } else if ((PINC & _BV(1)) == 0) {
        return 12;
    }

    // Undefined, shouldn't happen
    return 0;
}

uint16_t brightness[12] = {
    1587, // EV 5
    1581, // EV 6
    1450, 
    1400, 
    1350, 
    1300, 
    1250, 
    1200, 1150, 1100, 
    900, // EV 17 
    0,
};

int main()
{
    setup();
   
    // lower values = higher light output
    // 400  = 75%
    // 800  = 50%
    // 1200 = 25%
    //OCR1A = 800; // 50%
    OCR1A = 1599;

    while (1) {
        // Get dial setting
        uint8_t new_setting = get_selector();

        if (new_setting != last_setting) {
            last_setting = new_setting;

            // Update OCR1A
            switch (new_setting) {
                case 0:
                    // do nothing
                    //OCR1A = 1599;
                    break;
                default:
                    OCR1A = brightness[new_setting - 1];
                    break;
            }
        }

        // Off
        //PORTB &= ~_BV(1);
        //OCR1A = 1500;

        //delay(750);

        // On
        //PORTB |= _BV(1);
        //OCR1A = 1400; // 75%

        //delay(250);
    }

    return 0;
}
