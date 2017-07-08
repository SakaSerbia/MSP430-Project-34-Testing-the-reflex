/**
 * @file lcd16x2.c
 * @author Stefan Tešanović 675/2016
 * @date 01/05/2017
 * @version 1.0
 * @brief LCD Functions
 *
*/

#include "msp430.h"
#include "stdio.h"
#include "lcd16x2.h"

#define LCD_OUT (P8OUT)
#define LCD_MASK (0xfc)
#define LCD_MSB (0xf0)
#define LCD_RS (BIT2)					// data or command
#define LCD_EN (BIT3)					// enable
#define LCD_D4 (BIT4)
#define LCD_D5 (BIT5)
#define LCD_D6 (BIT6)
#define LCD_D7 (BIT7)

static const char red[2] = {0x00, 0x40};

/**
 * @name send_data
 * @brief Send data
 */
void send_data(char pod)
{
	LCD_OUT |= LCD_RS;
	LCD_OUT = (LCD_OUT & 0x0f) | (pod & LCD_MSB);
	LCD_OUT &= ~LCD_EN;					// enable zero
	__delay_cycles(1000);
	LCD_OUT |= LCD_EN;					// enable one
	__delay_cycles(1000);
	LCD_OUT &= ~LCD_EN;					// neable zero
	__delay_cycles(1000);
	LCD_OUT = (LCD_OUT & 0x0f) | ((pod << 4) & LCD_MSB);
	LCD_OUT &= ~LCD_EN;					// enable zero
	__delay_cycles(1000);
	LCD_OUT |= LCD_EN;					// enable one
	__delay_cycles(1000);
	LCD_OUT &= ~LCD_EN;					// neable zero
	__delay_cycles(1000);
}

/**
 * @name send_command
 * @brief Send command
 */
void send_command(char kom)
{
	LCD_OUT &= (~LCD_RS);
	LCD_OUT = (LCD_OUT & 0x0f) | (kom & LCD_MSB);
	LCD_OUT &= ~LCD_EN;					// enable zero
	__delay_cycles(1000);
	LCD_OUT |= LCD_EN;					// enable one
	__delay_cycles(1000);
	LCD_OUT &= ~LCD_EN;					// neable zero
	__delay_cycles(1000);
	LCD_OUT = (LCD_OUT & 0x0f) | ((kom << 4) & LCD_MSB);
	LCD_OUT &= ~LCD_EN;					// enable zero
	__delay_cycles(1000);
	LCD_OUT |= LCD_EN;					// enable one
	__delay_cycles(1000);
	LCD_OUT &= ~LCD_EN;					// neable zero
	__delay_cycles(1000);
}

/**
 * @name send_position
 * @brief Setting the cursor (position)
 */
void send_position(char y, char x)
{
    send_command(0x80 | red[y] | x);
}

/**
 * @name send_string
 * @brief Send string
 */
void send_string(char *string)
{
	while(*string > 0)
	{
	    send_data(*string++);
	}
}

/**
 * @name send_number
 * @brief Send number
 */
void send_number(unsigned long br)
{
	char broj[5];

	broj[0] = (br / 1000) + 48;
	broj[1] = ((br / 100) % 10) + 48;
	broj[2] = ((br / 10) % 10) + 48;
	broj[3] = (br % 10) + 48;
	broj[4] = 0;

	send_string(broj);
}




