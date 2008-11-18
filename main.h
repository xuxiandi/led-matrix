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

#ifndef MAIN_H
#define MAIN_H

#include <avr/io.h>

// PORTC:
#define SWC 0
#define SWB 1
#define SWA 2

#define SWITCH_DELAY 2500

volatile uint8_t display[7][7];
volatile uint8_t displayPass;
volatile uint8_t mode;
volatile unsigned long modePeriod; // TODO: Set up mode function
volatile bool switches[3];
volatile unsigned int switch_delay[3];

volatile uint8_t ax;
volatile uint8_t ay;
volatile uint8_t atRow;
volatile unsigned long intCounter;

volatile unsigned long seed;
unsigned long myRandom( unsigned long max )
    {
    seed = ( seed * 1664525 + 1013904223 );
    return ( seed >> 16 ) % max; // Use MSB bits if possible
    }

void clearDisplay()
    {
    for( uint8_t y = 0; y < 7; y++ )
        for( uint8_t x = 0; x < 7; x++ )
            display[y][x] = 0;
    }

void doTick();
void displayNextRow();
void processInput();

#endif
