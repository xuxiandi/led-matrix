/*****************************************************************************
 * Arduino-controlled 7x7 LED Matrix
 *
 * Copyright (C) 2008 Lucas Murray <lmurray@undefinedfire.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 ****************************************************************************/

// Main Arduino include
#include "WProgram.h"

#include <avr/io.h>
#include <avr/interrupt.h>

// PORTB:
#define COUNT_RST 1
#define COUNT_CLK 0

int main()
    {
    //------------------------------------------------------------------------
    // Initialization

    init();

    //------------------------------------------------------------------------
    // Setup

    // Set up directions
    DDRB |= ( 1 << COUNT_RST );
    DDRB |= ( 1 << COUNT_CLK );
    DDRD  = B11111111;

    // Make sure we start the counter from zero by resetting it
    digitalWrite( 9, HIGH );
    delay(1);
    digitalWrite( 9, LOW );
    delay(1);

    //------------------------------------------------------------------------
    // Main loop

    while( true )
        {
        // TODO: Do processing here once every 500ms (Get time)

        //---------------------------------------------------
        // The following will output 001 010 100 on the LEDs

        digitalWrite( 8, LOW );
        delay(1);
        digitalWrite( 8, HIGH );
        PORTD = ~B00000001; // Row 1 pattern
        delay(1);

        digitalWrite( 8, LOW );
        delay(1);
        digitalWrite( 8, HIGH );
        PORTD = ~B00000010; // Row 2 pattern
        delay(1);

        digitalWrite( 8, LOW );
        delay(1);
        digitalWrite( 8, HIGH );
        PORTD = ~B00000100; // Row 3 pattern
        delay(1);

        // Return to off state
        digitalWrite( 8, LOW );
        delay(1);
        digitalWrite( 8, HIGH );
        delay(1);
        }

    return 0;
    }
