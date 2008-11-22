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
        for( uint8_t y = 6; y >= 1; y-- )
            for( uint8_t x = 0; x < 7; x++ )
                display[y][x] = display[y-1][x];
        for( uint8_t x = 0; x < 7; x++ )
            display[0][x] = 0;
        display[0][myRandom( 7 )] = myRandom( 3 );
        }
    else if( mode == 1 )
        { // Random switching
        modePeriod = 5000;
        for( uint8_t i = 0; i < 5; i++ )
            {
            uint8_t x = myRandom( 7 );
            uint8_t y = myRandom( 7 );
            display[y][x] = myRandom( 3 );
            }
        }
    else if( mode == 2 )
        { // Swipe
        modePeriod = 250;
        display[ay][ax]++;
        if( display[ay][ax] > 2 )
            display[ay][ax] = 0;
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

    bool passIdle = false;
    if( atRow == 7 )
        {
        passIdle = true;
        atRow = 0;
        displayPass++;
        if( displayPass > 8 ) // "Half-brightness" is actually 1/8
            displayPass = 0;
        }

    // Process next row before switching to get maximum brightness
    uint8_t row = 0;
    for( uint8_t x = 0; x < 7; x++ )
        {
        if( display[atRow][x] == 2 ||
          ( display[atRow][x] == 1 && !displayPass ))
            row |= ( 1 << x );
        }

    PORTD = B01111111; // To prevent ghosting
    if( passIdle )
        {
        digitalWrite( 8, LOW );
        digitalWrite( 8, HIGH );
        }

    digitalWrite( 8, LOW );
    digitalWrite( 8, HIGH );
    PORTD = ~row;

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
    clearDisplay();
    displayPass = 0;
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

#if BOARD_REVISION >= 2
    // Activate internal pull-up on input switches
    PORTC |= ( 1 << SWA );
    PORTC |= ( 1 << SWB );
    PORTC |= ( 1 << SWC );
#endif

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
#if BOARD_REVISION >= 2
        if( !switch_delay[0] && ( PINC & ( 1 << SWA )))
#else
        if( !switch_delay[0] && !( PINC & ( 1 << SWA )))
#endif
            {
            switch_delay[0] = SWITCH_DELAY;
            switches[0] = true;
            }
#if BOARD_REVISION >= 2
        if( !switch_delay[1] && ( PINC & ( 1 << SWB )))
#else
        if( !switch_delay[1] && !( PINC & ( 1 << SWB )))
#endif
            {
            switch_delay[1] = SWITCH_DELAY;
            switches[1] = true;
            }
#if BOARD_REVISION >= 2
        if( !switch_delay[2] && ( PINC & ( 1 << SWC )))
#else
        if( !switch_delay[2] && !( PINC & ( 1 << SWC )))
#endif
            {
            switch_delay[2] = SWITCH_DELAY;
            switches[2] = true;
            }
        }

    return 0;
    }
