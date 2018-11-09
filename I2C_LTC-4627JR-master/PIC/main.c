/**
  Generated Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This is the main file generated using PIC10 / PIC12 / PIC16 / PIC18 MCUs 

  Description:
    This header file provides implementations for driver APIs for all modules selected in the GUI.
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs  - 1.45
        Device            :  PIC16F18345
        Driver Version    :  2.00
    The generated drivers are tested against the following:
        Compiler          :  XC8 1.35
        MPLAB             :  MPLAB X 3.40
*/

/*
    (c) 2016 Microchip Technology Inc. and its subsidiaries. You may use this
    software and any derivatives exclusively with Microchip products.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
    WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
    PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION
    WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION.

    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
    BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
    FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
    ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
    THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.

    MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE
    TERMS.
*/

#include "mcc_generated_files/mcc.h"

// Local variables

// Data received via I2C for what we should display
static uint8_t displayBuffer[DISPLAY_BUFFER_SIZE];

// Bit pattern to display each of the digits.
static uint8_t digitPatterns[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

// Lookup table to translate numbers/letters into LED bit patterns
//
// The bit pattern mimics the one defined in the MAX7219 driver chip
//
//        A
//      -----
//     |     |
//   F |     | B            Byte
//     |  G  |              MSB                LSB
//      -----               7  6  5  4  3  2  1  0
//     |     |              DP A  B  C  D  E  F  G
//   E |     | C
//     |     |
//      -----
//        D       DP
//
// Since the LTC-4627JR LED module is a common-anode design, each of the bit
// controlling the segment is a cathode. Meaning the LED energizes when the
// bit is set to zero.
//  
//   0xFF = everything is dark.       0x00 = all segments are illuminated.

// Given a value in 0x0 - 0xF, decode into the bit pattern used to display that
// digit.
uint8_t decodeHex(uint8_t value)
{
    // 0000 0      0100 4      1000 8      1100 C
    // 0001 1      0101 5      1001 9      1101 D
    // 0010 2      0110 6      1010 A      1110 E
    // 0011 3      0111 7      1011 B      1111 F
    static uint8_t decoded[] = {
        0x81, // 0 = 1000 0001 
        0xCF, // 1 = 1100 1111
        0x92, // 2 = 1001 0010
        0x86, // 3 = 1000 0110
        0xCC, // 4 = 1100 1100
        0xA4, // 5 = 1010 0100
        0xA0, // 6 = 1010 0000
        0x8F, // 7 = 1000 1111
        0x80, // 8 = 1000 0000
        0x8C, // 9 = 1000 1100
        0x88, // A = 1000 1000
        0xE0, // B = 1110 0000
        0xB1, // C = 1011 0001
        0xC2, // D = 1100 0010
        0xB0, // E = 1011 0000
        0xB8  // F = 1011 1000
    };
    
    if (value <= 0xF)
    {
        return decoded[value];
    }
    else
    {
        return 0xFF; // Active low means 0xFF is dark.
    }
}

// Given a value in ASCII 0x00 - 0x7F, decode into the bit pattern used to
// display that ASCII character. Given the limited nature of a 7-segment
// LED only a few will return actual bit patterns. Most of them will return
// the pattern for space (0xFF))
uint8_t decodeASCII(uint8_t asciiValue)
{
    // Range of numbers 0-9
    if (asciiValue >= 0x30 && asciiValue <= 0x39)
    {
        // Subtracting the ascii value of '0' returns hex index for decodeHex
        return decodeHex(asciiValue - 0x30);
    }
    // Range of uppercase letters A-F
    if (asciiValue >= 0x41 && asciiValue <= 0x46)
    {
        // Subtracting the ascii of 'A' plus 10 returns hex index for decodeHex.
        return decodeHex(asciiValue - 0x41 + 10);
    }
    // Range of lowercase letters A-F
    if (asciiValue >= 0x61 && asciiValue <= 0x66)
    {
        // Subtracting the ascii of 'a' plus 10 returns hex index for decodeHex.
        return decodeHex(asciiValue - 0x61 + 10);
    }
    return 0xFF; // Blank space
}

// Called by a timer ISR to illuminate another digit in the display.
void nextDigit(void)
{
    static uint8_t digit = 0;

    switch(digit)
    {
        case 0:
            DIG_L_SetLow();
            LATC = digitPatterns[0];
            DIG_1_SetHigh();
            digit = 1;
            break;
        case 1:
            DIG_1_SetLow();
            LATC = digitPatterns[1];
            DIG_2_SetHigh();
            digit = 2;
            break;
        case 2:
            DIG_2_SetLow();
            LATC = digitPatterns[2];
            DIG_3_SetHigh();
            digit = 3;
            break;
        case 3:
            DIG_3_SetLow();
            LATC = digitPatterns[3];
            DIG_4_SetHigh();
            digit = 4;
            break;
        case 4:
            DIG_4_SetLow();
            LATC = digitPatterns[4];
            DIG_L_SetHigh();
            digit = 0;
            break;
        default:
            DIG_1_SetLow();
            DIG_2_SetLow();
            DIG_3_SetLow();
            DIG_4_SetLow();
            DIG_L_SetLow();
            LATC = 0x0;
            digit = 0;
            break;
    }
}

/*
                         Main application
 */
void main(void)
{
    size_t dataIndex = 0;

    // initialize the device
    SYSTEM_Initialize();

    // When using interrupts, you need to set the Global and Peripheral Interrupt Enable bits
    // Use the following macros to:

    // Enable the Global Interrupts
    INTERRUPT_GlobalInterruptEnable();

    // Enable the Peripheral Interrupts
    INTERRUPT_PeripheralInterruptEnable();

    // Disable the Global Interrupts
    //INTERRUPT_GlobalInterruptDisable();

    // Disable the Peripheral Interrupts
    //INTERRUPT_PeripheralInterruptDisable();

    digitPatterns[0] = 0xC9; // Nonstandard "II" in a single digit
    digitPatterns[1] = decodeASCII('C');
    digitPatterns[2] = decodeASCII('4');
    digitPatterns[3] = decodeASCII('2');
    digitPatterns[4] = 0xFF;
    
    while (1)
    {
        if(I2C1_DataAvailable())
        {
            I2C1_CopyDisplayBuffer(displayBuffer);

            // Digit 5 are dark unless specifically illuminated
            digitPatterns[4] = 0xFF;

            dataIndex = 0;
            for (size_t digit = 0; digit < 4; digit++)
            {
                digitPatterns[digit] = decodeASCII(displayBuffer[dataIndex]);
                dataIndex++;
                if (displayBuffer[dataIndex]=='.')
                {
                    // Light up the decimal point
                    digitPatterns[digit] = digitPatterns[digit] & 0x7F;
                    dataIndex++;
                }
                if (digit==1 && displayBuffer[dataIndex]==':')
                {
                    // Light up the colon
                    digitPatterns[4] = digitPatterns[4] & 0x9F;
                    dataIndex++;
                }
                if (digit==2 && (displayBuffer[dataIndex]=='\'') || (displayBuffer[dataIndex]=='`'))
                {
                    // Light up the upper dot thingie
                    digitPatterns[4] = digitPatterns[4] & 0xEF;
                    dataIndex++;
                }
            }
        }
    }
}
/**
 End of File
*/