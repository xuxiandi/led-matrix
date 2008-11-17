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

int rowStates[7];

unsigned long seed;
unsigned long myRandom( unsigned long max )
    {
    seed = ( seed * 1664525 + 1013904223 );
    return seed % max;
    }

void setState( int row, int col, bool state )
    {
    if(state)
        rowStates[row] |= ( 1 << col );
    else
        rowStates[row] &= ~( 1 << col );
    }

bool getState( int row, int col )
    {
    return !!( rowStates[row] & ( 1 << col ));
    }

int main()
    {
    //------------------------------------------------------------------------
    // Initialization

    init();

    //------------------------------------------------------------------------
    // Setup

    seed = 6243;

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

    int count = 25;
    while( true )
        {
        if( !--count )
            {
            count = 25;

            // Rain
            //for( int i = 7; i >= 1; i-- )
            //    rowStates[i] = rowStates[i-1];
            //rowStates[0] = ( 1 << myRandom( 7 ));

            // Random switching
            for( int i = 0; i < 5; i++ )
                {
                int x = myRandom( 7 );
                int y = myRandom( 7 );
                setState( x, y, !getState( x, y ));
                }
            }

        //---------------------------------------------------
        // The following will output 001 010 100 on the LEDs

        digitalWrite( 8, LOW );
        delay(1);
        digitalWrite( 8, HIGH );
        PORTD = ~rowStates[0];
        delay(1);

        digitalWrite( 8, LOW );
        delay(1);
        digitalWrite( 8, HIGH );
        PORTD = ~rowStates[1];
        delay(1);

        digitalWrite( 8, LOW );
        delay(1);
        digitalWrite( 8, HIGH );
        PORTD = ~rowStates[2];
        delay(1);

        digitalWrite( 8, LOW );
        delay(1);
        digitalWrite( 8, HIGH );
        PORTD = ~rowStates[3];
        delay(1);

        digitalWrite( 8, LOW );
        delay(1);
        digitalWrite( 8, HIGH );
        PORTD = ~rowStates[4];
        delay(1);

        digitalWrite( 8, LOW );
        delay(1);
        digitalWrite( 8, HIGH );
        PORTD = ~rowStates[5];
        delay(1);

        digitalWrite( 8, LOW );
        delay(1);
        digitalWrite( 8, HIGH );
        PORTD = ~rowStates[6];
        delay(1);

        // Return to off state
        digitalWrite( 8, LOW );
        delay(1);
        digitalWrite( 8, HIGH );
        delay(1);
        }

    return 0;
    }
