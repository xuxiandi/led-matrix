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

unsigned long seed = 6243;
unsigned long myRandom( unsigned long max )
    {
    seed = ( seed * 1664525 + 1013904223 );
    return ( seed >> 16 ) % max; // Use MSB bits if possible
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

volatile int atRow = 0;
void displayNextRow()
    {
    // TODO: Stay in idle mode if any of the top lines are blank

    if( atRow == 7 )
        {
        digitalWrite( 8, LOW );
        __asm__( "nop\n" "nop\n" "nop\n" "nop\n" );
        __asm__( "nop\n" "nop\n" "nop\n" "nop\n" );
        digitalWrite( 8, HIGH );
        __asm__( "nop\n" "nop\n" "nop\n" "nop\n" );
        __asm__( "nop\n" "nop\n" "nop\n" "nop\n" );
        atRow = 0;
        }

    digitalWrite( 8, LOW );
    __asm__( "nop\n" "nop\n" "nop\n" "nop\n" );
    __asm__( "nop\n" "nop\n" "nop\n" "nop\n" );
    digitalWrite( 8, HIGH );
    PORTD = ~rowStates[atRow];
    __asm__( "nop\n" "nop\n" "nop\n" "nop\n" );
    __asm__( "nop\n" "nop\n" "nop\n" "nop\n" );

    atRow++;
    }

volatile int ax = 0;
volatile int ay = 0;
void doTick()
    {
    cli();

    // Rain
    /*for( int i = 7; i >= 1; i-- )
        rowStates[i] = rowStates[i-1];
    rowStates[0] = ( 1 << myRandom( 7 ));*/

    // Random switching
    for( int i = 0; i < 5; i++ )
        {
        int x = myRandom( 7 );
        int y = myRandom( 7 );
        setState( x, y, !getState( x, y ));
        }

    // Swipe
    /*ax++;
    if( ax == 8 )
        {
        ax = 0;
        ay++;
        if( ay == 8 )
            ay = 0;
        }
    setState( ax, ay, !getState( ax, ay ));*/
    }

volatile unsigned long intCounter = 0;
ISR(TIMER2_OVF_vect)
    {
    displayNextRow();

    intCounter++;
    if( intCounter == 5000 )
        {
        doTick();
        intCounter = 0;
        }
    }

int main()
    {
    //------------------------------------------------------------------------
    // Initialization

    init();

    // Timer2 Settings: Timer Prescaler /8, mode 0
    // Timer clock = 16MHz/8 = 2Mhz or 0.5us
    TCCR2A = 0;
    TCCR2B = 0 << CS22 | 1 << CS21 | 0 << CS20;

    // Timer2 Overflow Interrupt Enable
    TIMSK2 = 1 << TOIE2;

    //------------------------------------------------------------------------
    // Setup

    // Set up directions
    DDRB |= ( 1 << COUNT_RST );
    DDRB |= ( 1 << COUNT_CLK );
    DDRD  = B11111111;

    // Make sure we start the counter from zero by resetting it
    cli(); // Prevent the display interrupt from going out of sync
    digitalWrite( 9, HIGH );
    __asm__( "nop\n" "nop\n" "nop\n" "nop\n" );
    __asm__( "nop\n" "nop\n" "nop\n" "nop\n" );
    digitalWrite( 9, LOW );
    __asm__( "nop\n" "nop\n" "nop\n" "nop\n" );
    __asm__( "nop\n" "nop\n" "nop\n" "nop\n" );
    atRow = 0;
    sei();

    //------------------------------------------------------------------------
    // Main loop

    while( true )
        {
        }

    return 0;
    }
