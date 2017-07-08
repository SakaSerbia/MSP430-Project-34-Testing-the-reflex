/**
 * @file main.c
 * @author Stefan Tešanović 675/2016
 * @date 10/05/2017
 * @version 1.3
 *
 * @details Write a program that tests the reflex. One of the 4 diodes is randomly switched on and a certain time remains on. 
 * If during that time interval presses a button corresponding to that diodes, gets 3 points, otherwise one point is lost. 
 * Display the points scored to implement on the LCD display.
 *
 * <b>Modified by:</b> Stefan Tesanovic on 07/07/2017 \n
 * <i>Modification description:</i> Translate comments on English.\n
 * \n
 *
 */

#include <msp430.h> 
#include <time.h>
#include <stdlib.h>
#include "stdio.h"
#include "lcd16x2.h"

/** A constant that defines one second, we was use ACLK=32768 Hz */
#define ONE_SEC 32768

/** A constant that defines half a second, TA0IV_TACCR1 */
#define HALF_SEC 18000

/** A constant used to debug the keys */
#define TIMER_DEB 1819

/** A constant that determines how long one period in our game lasts  */
#define GLOBAL_PERIOD 3

/** We are implementing the initialization of the LCD display */
#define LCD_clear       (0x01)
#define LCD_4bit_init   (0x32)
#define LCD_init        (0x33)
#define LCD_4bit        (0x28)
#define LCD_control     (0x0C)
#define LCD_entry_mode  (0x06)
#define LCD_set_DDRAM   (0x80)
#define LCD_OUT         (P8OUT)

/** Changeable which show the result in test and is printed on the LCD display */
volatile int score = 0;         

/** Changeable which show the test level and is printed on the LCD display */
volatile int game_level = 1;    


volatile double num_sec = 0;      

/** The moment when we turn on the diode */
volatile double TIME_TO_TURN_ON = 0.5;

/** The moment when we turn off the diodes */
volatile double TIME_TO_TURN_OFF = 2.5;

/** The number of diode what is turn on */
volatile char diode_num = 0;    

/** A flag indicating whether the diodes are turned on or not */
volatile char diode_on = 0;    

/** A flag indicating whether is game mode or not */
volatile char mode_game = 0;

/** A flag indicating whether is game start or not */
volatile char mode_start = 3;

/** A flag indicating whether is game win or not */
volatile char mode_win = 0;

/** A flag indicating of geting 3 points */
volatile char add3 = 0;

/** A flag indicating of losing 1 points */
volatile char sub1 = 0;

/**
 * @brief Main
 *
 */

int main(void)
{

    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer

    P2DIR &= ~(BIT4 | BIT5 | BIT6 | BIT7);  // button are input
    P4DIR |= BIT3 | BIT4 | BIT5 | BIT6;     // diode are output
    P8DIR = 0xFC;                           // lcd output
    P2IE |= BIT4 | BIT5 | BIT6 | BIT7;      // we allow interruptions
    P2IES |= BIT4 | BIT5 | BIT6 | BIT7;     
    P8OUT = 0x00;
    P4OUT = 0x00;

    __delay_cycles(2000);

    LCD_OUT |= 0xFC;
    LCD_OUT &= 0x00;
    send_command(LCD_init);
    send_command(LCD_4bit_init);
    send_command(LCD_4bit);
    send_command(LCD_control);
    send_command(LCD_clear);
    send_command(LCD_entry_mode);
    send_command(LCD_set_DDRAM);

    // TimerA
    TA0CCR0 = ONE_SEC;
    TA0CCR1 = HALF_SEC;
    TA0CCTL1 = CCIE;
    TA0CTL = TASSEL_1 + MC_1 + TAIE;	// Up Mode + ACLK

    // TimerB
    TB0CCTL0 = CCIE;

    // randum
    srand(time(NULL));

    __bis_SR_register(GIE);		// A global permit for masking interruptions

    while(1)
    {
        if (add3)
        {
			diode_on = 0;
            add3=0;
            score+=3;
        }

        if (sub1)
        {
			diode_on = 0;
            sub1=0;
            score--;
        }

        game_level=score / 10 + 1;

        switch(game_level)
        {
			case 1:
				TIME_TO_TURN_OFF = 2.5;
				break;
			case 2:
				TIME_TO_TURN_OFF = 2;
				break;
			case 3:
				TIME_TO_TURN_OFF = 1.5;
				break;
			case 4:
				mode_game = 0;
				mode_win = 6;
				score = 0;
				break;
			default:
				TIME_TO_TURN_OFF = 2.5;
				break;
        }

        if (mode_game)
        {
                send_position(0, 0);
                send_string("Game level: ");

                send_position(1, 0);
                send_string("Points:    ");

                if(score < 0)
                {
                    send_position(0, 12);
                    send_string("----");

                    send_position(1, 11);
                    send_string("-");
                    send_position(1, 12);
                    send_number(-score);
                }
                else
                {
                    send_position(0, 12);
                    send_number(game_level);

                    send_position(1, 11);
                    send_string(" ");
                    send_position(1, 12);
                    send_number(score);
                }
            }
			
            if (mode_win)
            {
                send_position(0, 0);
                send_string("CONGRATULATIONS ");

                send_position(1, 0);
                send_string("   YOU    WIN   ");
            }
			
            if (mode_start)
            {
                send_position(0, 0);
                send_string(" GAME START IN  ");

                send_position(1, 0);
                send_string("      ");

                send_position(1, 6);
                send_number(mode_start);

                send_position(1, 10);
                send_string("      ");
            }
	};
}

/**
 * @brief Interrupt timer A1
 *
 */
#pragma vector=TIMER0_A1_VECTOR
__interrupt void Timer_A1(void)
{
    switch (TA0IV)
        {
        case TA0IV_NONE: break;			// Vector 0 - 0x00=TA0IV_NONE
        case TA0IV_TACCR1:              // Vector 2 - 0x02=TA0IV_TACCR1
            if (mode_game)
            {
                num_sec+=0.5;                               
                if (num_sec == TIME_TO_TURN_ON)
                {
                    diode_num = rand()%4;
                    switch(diode_num)
                    {
                    case 0:
                        P4OUT = BIT3;
                        break;
                    case 1:
                        P4OUT = BIT4;
                        break;
                    case 2:
                        P4OUT = BIT5;
                        break;
                    default:
                        P4OUT = BIT6;
                        break;
                    }
                    diode_on = 1;
                }
                if(num_sec == TIME_TO_TURN_OFF)
                {
                    if(diode_on == 1)
                    {
                        sub1=1;
                        P4OUT &= ~(BIT3 | BIT4 | BIT5 | BIT6);    // turn off diode
                        diode_on = 0;
                    }
                }
            }
        case 0x04: break;
        case 0x06: break;
        case 0x08: break;
        case 0x0A: break;
        case 0x0C: break;
        case 0x0E:
            if (mode_game)
            {
                num_sec+=0.5;
                if(num_sec == TIME_TO_TURN_OFF)
                {
                    if(diode_on == 1)
                    {
                        diode_on = 0;
                        sub1=1;
                        P4OUT &= ~(BIT3 | BIT4 | BIT5 | BIT6);    // turn off diode
                    }
                }
                if(num_sec == GLOBAL_PERIOD)
                {
                     num_sec = 0;
                }
            }
            if (mode_start)
            {
                mode_start--;
                if (mode_start==0)
                {
                    mode_game=1;
                    TIME_TO_TURN_OFF = 2.5;
                    score=0;
                }
            }
            if (mode_win)
           {
               mode_win--;
               if (mode_win==0) mode_start=3;
           }

        default: break;
        }
}


/**
 * @brief Interrupt for port (button) writen in assembler
 *
 */
/*
#pragma vector=PORT2_VECTOR
__interrupt void P2ISR(void)
{
    TA0CTL = MC__STOP;

    TB0CCR0 = TIMER_DEB;
    TB0CTL= MC__UP | TACLR | TASSEL__ACLK;

    P2IFG &= ~(BIT4 | BIT5 | BIT6 | BIT7);          // delete flag

}
*/

/**
 * @brief Interrupt timer B0
 *
 */

#pragma vector=TIMER0_B0_VECTOR
__interrupt void Timer_B0(void)
{
	if (diode_on)
	{
		switch(diode_num)
		{
			case 0:
				if ((P2IN & BIT4)==0) add3=1;
				else sub1=1;
				break;

			case 1:
				if ((P2IN & BIT5)==0) add3=1;
				else sub1=1;
				break;

			case 2:
				if ((P2IN & BIT6)==0) add3=1;
				else sub1=1;
				break;

			case 3:
				if  ((P2IN & BIT7)==0) add3=1;
				else sub1=1;
				break;

			default:
				break;
		}
	}

	P4OUT &= ~(BIT3 | BIT4 | BIT5 | BIT6);		// turn off diode
	TB0CTL = MC__STOP;

	// TimerA
	TA0CCR0 = ONE_SEC;
	TA0CCR1 = HALF_SEC;
	TA0CCTL1 = CCIE;
	TA0CTL = TASSEL_1 + MC_1 + TAIE;            // Up Mode + ACLK

}


