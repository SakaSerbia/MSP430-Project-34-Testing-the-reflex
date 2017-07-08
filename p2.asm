;-------------------------------------------------------------------------------
;    TA0CTL = MC__STOP;
;
;    TB0CCR0 = TIMER_DEB;
;    TB0CTL= MC__UP | TACLR | TASSEL__ACLK;
;
;    P2IFG &= ~(BIT4 | BIT5 | BIT6 | BIT7);          // Delete flag
;	
;-------------------------------------------------------------------------------

			.cdecls C,LIST,"msp430.h"       ; Include device header file

			.text
P2ISR

			bic.w #MC__STOP, &TA0CTL        ; Stop timer A
			mov.w #1819, &TB0CCR0			; Define the parameters for timer B
			bis.w #MC__UP|TACLR|TASSEL__ACLK, &TB0CTL

			bic.b #BIT4, &P2IFG ; Delete flag for button 1
			bic.b #BIT5, &P2IFG ; Delete flag for button 2
			bic.b #BIT6, &P2IFG ; Delete flag for button 3
			bic.b #BIT7, &P2IFG ; Delete flag for button 4

			reti

			.sect	.int42
            .short	P2ISR
