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

#include "main.h"

// Main Arduino include
#include "WProgram.h"

#include <avr/interrupt.h>

void doTick()
    {
    // Run program
    if( mode == 0 )
        { // Rain
        modePeriod = 1000;
        for( uint8_t i = 6; i >= 1; i-- )
            rowStates[i] = rowStates[i-1];
        rowStates[0] = ( 1 << myRandom( 7 ));
        }
    else if( mode == 1 )
        { // Random switching
        modePeriod = 5000;
        for( uint8_t i = 0; i < 5; i++ )
            {
            uint8_t x = myRandom( 7 );
            uint8_t y = myRandom( 7 );
            setState( x, y, !getState( x, y ));
            }
        }
    else if( mode == 2 )
        { // Swipe
        modePeriod = 250;
        setState( ax, ay, !getState( ax, ay ));
        if( ++ax == 7 )
            {
            ax = 0;
            if( ++ay == 7 )
                ay = 0;
            }
        }
    }

void displayNextRow()
    {
    // TODO: Stay in idle mode if any of the top lines are blank

    PORTD = B01111111; // To prevent ghosting
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

void processInput()
    {
    // Decrement switch counters
    if( switch_delay[0] )
        switch_delay[0]--;
    if( switch_delay[1] )
        switch_delay[1]--;
    if( switch_delay[2] )
        switch_delay[2]--;

    // Process switches
    if( switches[0] )
        {
        if( mode++ == 2 )
            mode = 0;
        // Clear and force next tick
        ax = 0; ay = 0;
        clearDisplay();
        intCounter = modePeriod;
        }
    else if( switches[2] )
        {
        if( mode-- == 0 )
            mode = 2;
        // Clear and force next tick
        ax = 0; ay = 0;
        clearDisplay();
        intCounter = modePeriod;
        }

    // Clear switch registers
    switches[0] = false;
    switches[1] = false;
    switches[2] = false;
    }

ISR(TIMER2_OVF_vect)
    {
    displayNextRow();

    processInput();

    intCounter++;
    if( intCounter >= modePeriod )
        {
        intCounter = 0;
        doTick();
        }
    }

int main()
    {
    //------------------------------------------------------------------------
    // Initialization

    // Global variables
    mode = 0;
    modePeriod = 1000;
    seed = 6243;
    ax = 0;
    ay = 0;
    atRow = 0;
    intCounter = 0;

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
    DDRB |= B00000011;
    DDRC |= ( 1 << SWA );
    DDRC |= ( 1 << SWB );
    DDRC |= ( 1 << SWC );
    DDRD  = B01111111;

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
        if( !switch_delay[0] && ( PINC & ( 1 << SWA )))
            {
            switch_delay[0] = SWITCH_DELAY;
            switches[0] = true;
            }
        if( !switch_delay[1] && ( PINC & ( 1 << SWB )))
            {
            switch_delay[1] = SWITCH_DELAY;
            switches[1] = true;
            }
        if( !switch_delay[2] && ( PINC & ( 1 << SWC )))
            {
            switch_delay[2] = SWITCH_DELAY;
            switches[2] = true;
            }
        }

    return 0;
    }
