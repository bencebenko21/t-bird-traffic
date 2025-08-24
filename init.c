/*
 * init.c
 *
 * Created: 11/26/2024 5:29:11 PM
 *  Author: benko
 */ 

#include <avr/interrupt.h>
#include "init.h"

void port_init(void)
{
	DDRA=0xFF;		//PORTA FF
	DDRC=0xFF;		//PORTC FF
	DDRF=0x0F;		//PORTF 0F
	DDRE=0xCC;		//PORTE CC
}
void timer_ovf_init(void)
{
	TCCR0=1<<CS02 | 0<<CS01 | 0<<CS00;	//prescaler seg�ts�g�vel a TIMER0 overflow id?z�t�st be�ll�tjuk 2ms-ra, �gy a 0,5mp, 1mp �s a t�bbsz�r�s�k egyszer?en be�ll�that�
	TIMSK=1<<TOIE0;						//TOIE0: Timer/Counter0 Overflow Interrupt Enable
	sei();					// glob�lis interrupt enged�lyez�s
	
}
