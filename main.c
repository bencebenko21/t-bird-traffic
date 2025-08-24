/*
 * TRAFFIC_ATMEGA_Benko_Bence.c
 *
 * Created: 11/26/2024 4:09:29 PM
 * Author : benko
 */ 

#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "main.h"

void PWM(void);
void thomasComes(void);
void buttonHandling(void);


//	   PORTA	  PORTC	      PORTE     PORTF
uint8_t controlArray[17][4] = {
	{0b10001001,0b00100100,0b10111000,0b00000000},//ALAP		0.  állapot Minden piros, ide érkezünk, amikor a nappali üzemmódba váltunk és vissza az éjszakaiba 2mp-ig
	{0b10001011,0b00100100,0b10111000,0b00000000},//1.KÖR		1.  állapot: A Piros és Sárga 1mp-ig
	{0b10001100,0b00100100,0b10110100,0b00000000},//			2.  állapot: A Zöld - G1 Zöld 5mp-ig
	{0b10001010,0b00100100,0b10110100,0b00000000},//			3.  állapot: A Sárga - G1 villog 3mp-ig
	{0b10001001,0b00100100,0b10111000,0b00000000},//			4.  állapot: Minden vissza pirosba 2mp-ig
	{0b10001001,0b00101100,0b10111000,0b00000000},//2.KÖR		5.  állapot: C Piros és Sárga 1mp-ig
	{0b10001001,0b00110000,0b01111000,0b00000001},//			6.  állapot: C Zöld, E(J) Zöld - G2 Zöld 5mp-ig
	{0b10001001,0b00101000,0b01111000,0b00000000},//			7.  állapot: C Sárga - E(J) elalszik - G2 villog 3mp-ig
	{0b10001001,0b00100100,0b10111000,0b00000000},//			8.  állapot: Minden vissza pirosba 2mp-ig
	{0b10011001,0b00100101,0b10111000,0b00000000},//3.KÖR		9.  állapot: D és B Piros és Sárga 1mp-ig
	{0b00100001,0b00100110,0b10110100,0b00000000},//			10. állapot: D és B Zöld - G1 Zöld 5mp-ig
	{0b00010001,0b00100101,0b10110100,0b00000000},//			11. állapot: D és B sárga - G1 villog 3mp-ig
	{0b10001001,0b00100100,0b10111000,0b00000000},//			12. állapot: Minden vissza pirosba 2mp-ig
	{0b10001001,0b01100100,0b10111000,0b00000000},//4.KÖR		13. állapot: E Piros és Sárga 1mp
	{0b10001001,0b10000100,0b01111000,0b00000000},//			14. állapot: E Zöld - G2 Zöld 5mp-ig
	{0b10001001,0b01000100,0b01111000,0b00000000},//			15. állapot: E Sárga - G2 villog 3mp-ig
	{0b10001001,0b00100100,0b10110100,0b00000000} //			16. állapot: Minden vissza pirosba 2mp-ig
};
uint16_t timeArray[17]={1000,500,2500,1500,1000,500,2500,1500,1000,500,2500,1500,1000,500,2500,1500,1000};
	
uint8_t controlArrayThomas[13][4] = {
	{0b10001001,0b00100100,0b10111000,0b00000000},//ALAP		0.  állapot Minden piros, ide érkezünk, amikor a nappali üzemmódba váltunk és vissza az éjszakaiba 2mp-ig
	{0b10001011,0b00100100,0b10111000,0b00000000},//1.KÖR		1.  állapot: A Piros és Sárga 1mp-ig
	{0b10001100,0b00100100,0b10110100,0b00000000},//			2.  állapot: A Zöld - G1 Zöld 5mp-ig
	{0b10001010,0b00100100,0b10110100,0b00000000},//			3.  állapot: A Sárga - G1 villog 3mp-ig
	{0b10001001,0b00100100,0b10111000,0b00000000},//			4.  állapot: Minden vissza pirosba 2mp-ig
	{0b10001001,0b00101100,0b10111000,0b00000000},//2.KÖR		5.  állapot: C Piros és Sárga 1mp-ig
	{0b10001001,0b00110000,0b01111000,0b00000000},//			6.  állapot: C Zöld - G2 Zöld 5mp-ig
	{0b10001001,0b00101000,0b01111000,0b00000000},//			7.  állapot: C Sárga - G2 villog 3mp-ig
	{0b10001001,0b00100100,0b10111000,0b00000000},//			8.  állapot: Minden vissza pirosba 2mp-ig
	{0b10001001,0b00100101,0b10111000,0b00000000},//3.KÖR		9.  állapot: D Piros és Sárga 1mp-ig
	{0b01001001,0b00100110,0b10110100,0b00000000},//			10. állapot: D és B (J) Zöld - G1 Zöld 5mp-ig
	{0b00001001,0b00100101,0b10110100,0b00000000},//			11. állapot: D sárga, B(J) elalszik - G1 villog 3mp-ig
	{0b10001001,0b00100100,0b10111000,0b00000000}//			12. állapot: Minden vissza pirosba 2mp-ig
};

uint16_t timeArrayThomas[13]={1000,500,2500,1500,1000,500,2500,1500,1000,500,2500,1500,1000};

uint8_t upDown=1;
uint16_t periodCount=0;
uint8_t period=10;
uint8_t dutyCycle=0;
uint8_t timeSlot=0;
uint8_t blink=0;
uint8_t tmp=0;
uint8_t jelenlegi_uzemmod = 0;
uint8_t elozo_uzemmod=0;
uint8_t jelenlegi_allapot = 0;
uint8_t elozo_allapot = 0;

uint8_t nightMode=1;
uint8_t gomb;
uint8_t thomasTheTankEngineButton=0;
uint8_t thomasTheTankEngine=1;
uint8_t thomasRight=0;
uint8_t thomasLeft=0;
uint8_t noThomas=0;
uint8_t tmp2=0;
uint8_t tmp3=0;
uint8_t tmp4=0;

uint8_t g1DebounceStart=0;
uint8_t g2DebounceStart=0;
uint8_t thomasButtonTimer1=0;
uint8_t thomasButtonTimer2=0;

uint16_t ido=1000;
uint8_t i=0;



int main(void)
{
    
	port_init();
	timer_ovf_init();
	PORTE = 0b00110000;
	
    while (1) 
    {
		
	}
}
ISR(TIMER0_OVF_vect)
{
	if(!(PINE & (1<<BTNV1))) g1DebounceStart=1;
	if(!(PINE & (1<<BTNV2))) g2DebounceStart=1;
	
	buttonHandling();
	thomasComes();
	PWM();

	gomb = PING;
	jelenlegi_allapot = gomb;
	
	if(jelenlegi_allapot != elozo_allapot)
	{
		
		jelenlegi_allapot = gomb;
		
		if(jelenlegi_allapot != elozo_allapot)
		{
			elozo_allapot = jelenlegi_allapot;
			

			switch (jelenlegi_allapot)
			{
				case 0x01: jelenlegi_uzemmod = 1; break;
				i=0;
				case 0x02: jelenlegi_uzemmod = 2; break;
			}
		}
	}
	
	{
		switch(jelenlegi_uzemmod)
		{

			case 1: nightMode = 0;
			break;
			case 2:
			if((i==0)||(i==1)||(i==4)||(i==8)||(i==12))
			{
				nightMode = 1;
				PORTA = 0;
				PORTC = 0;
				PORTF = 0;
				PORTE = 0b00110000;
			}
			break;
		}
	}
	
	if(nightMode==1)
	{
		
		if(thomasTheTankEngine)
		{
			if(blink)
			{
				PORTF |= (1 << LV_F);
			}
			else
			{
				PORTF &= ~(1 << LV_F);
			}
			
			if(blink)
			{
				PORTA |= (1 << LB_S) | (1 << LA_S);
				PORTC |= (1 << LD_S) | (1 << LC_S) | (1 << LE_S);
				PORTF |= (1 << LV_F);
			}
			else
			{
				PORTA &= ~((1 << LB_S) | (1 << LA_S));
				PORTC &= ~((1 << LD_S) | (1 << LC_S) | (1 << LE_S));
				PORTF &= ~(1 << LV_F);
			}
		}
		else
		{
			if(blink)
			{
				PORTF |= (1 << LV_P1);
			}
			else
			{
				PORTF &= ~(1 << LV_P1);
			}
			if(blink)
			{
				PORTF &= ~(1 << LV_P2);
			}
			else
			{
				
				PORTF |= (1 << LV_P2);
			}
			if(blink)
			{
				tmp3=0b00001000;
				tmp3 |= (1 << LA_S);
				PORTA=tmp3;
				tmp4=0b00100000;
				tmp4 |= (1 << LD_S) | (1 << LC_S);
				PORTC=tmp4;
			}
			else
			{
				tmp3=0b00001000;
				tmp3 &= ~(1 << LA_S);
				PORTA=tmp3;
				tmp4=0b00100000;
				tmp4 &= ~((1 << LD_S) | (1 << LC_S));
				PORTC=tmp4;
			}
		}
	}
	else
	{
		if(thomasTheTankEngine)
		{
			ido--;
			if(ido==0)
			{
				i++;
				ido=timeArray[i];
			}
			else
			{
				PORTA=controlArray[i][0];
				PORTC=controlArray[i][1];
				switch(i)
				{
					case 3:
					if(blink)
					{
						
						tmp2=controlArray[i][2];
						tmp2|=(1<<LG1_P);
						PORTE=tmp2;
					}
					else
					{
						tmp2=controlArray[i][2];
						tmp2&=~(1<<LG1_P);
						PORTE=tmp2;
						
						
					}
					break;
					case 7:
					if(blink)
					{
						
						tmp2=controlArray[i][2];
						tmp2|=(1<<LG2_P);
						PORTE=tmp2;
					}
					else
					{
						tmp2=controlArray[i][2];
						tmp2&=~(1<<LG2_P);
						PORTE=tmp2;
						
						
					}
					break;
					case 11:
					if(blink)
					{
						
						tmp2=controlArray[i][2];
						tmp2|=(1<<LG1_P);
						PORTE=tmp2;
					}
					else
					{
						tmp2=controlArray[i][2];
						tmp2&=~(1<<LG1_P);
						PORTE=tmp2;
						
						
					}
					break;
					case 15:
					if(blink)
					{
						
						tmp2=controlArray[i][2];
						tmp2|=(1<<LG2_P);
						PORTE=tmp2;
					}
					else
					{
						tmp2=controlArray[i][2];
						tmp2&=~(1<<LG2_P);
						PORTE=tmp2;
						
						
					}
					break;
					default:PORTE=controlArray[i][2]; break;
				}
				
				PORTF=controlArray[i][3];
				
				if(thomasTheTankEngine)
				{
					if(blink)
					{
						PORTF |= (1 << LV_F);
					}
					else
					{
						PORTF &= ~(1 << LV_F);
					}
				}
				else
				{
					if(blink)
					{
						PORTF |= (1 << LV_P1);
					}
					else
					{
						PORTF &= ~(1 << LV_P1);
					}
					if(blink)
					{
						PORTF &= ~(1 << LV_P2);
					}
					else
					{
						PORTF |= (1 << LV_P2);
					}
				}
			}
			if(i==16)
			{
				i=0;
			}
		}
		else
		{
			ido--;
			if(ido==0)
			{
				i++;
				ido=timeArrayThomas[i];
			}
			else
			{
				PORTA=controlArrayThomas[i][0];
				PORTC=controlArrayThomas[i][1];
				switch(i)
				{
					case 3:
					if(blink)
					{
						
						tmp2=controlArrayThomas[i][2];
						tmp2|=(1<<LG1_P);
						PORTE=tmp2;
					}
					else
					{
						tmp2=controlArrayThomas[i][2];
						tmp2&=~(1<<LG1_P);
						PORTE=tmp2;
						
						
					}
					break;
					case 7:
					if(blink)
					{
						
						tmp2=controlArrayThomas[i][2];
						tmp2|=(1<<LG2_P);
						PORTE=tmp2;
					}
					else
					{
						tmp2=controlArrayThomas[i][2];
						tmp2&=~(1<<LG2_P);
						PORTE=tmp2;
						
						
					}
					break;
					case 11:
					if(blink)
					{
						
						tmp2=controlArrayThomas[i][2];
						tmp2|=(1<<LG1_P);
						PORTE=tmp2;
					}
					else
					{
						tmp2=controlArrayThomas[i][2];
						tmp2&=~(1<<LG1_P);
						PORTE=tmp2;
						
						
					}
					break;
					default:PORTE=controlArray[i][2]; break;
				}
				
				PORTF=controlArrayThomas[i][3];
				
				if(thomasTheTankEngine)
				{
					if(blink)
					{
						PORTF |= (1 << LV_F);
					}
					else
					{
						PORTF &= ~(1 << LV_F);
					}
				}
				else
				{
					if(blink)
					{
						PORTF |= (1 << LV_P1);
					}
					else
					{
						PORTF &= ~(1 << LV_P1);
					}
					if(blink)
					{
						PORTF &= ~(1 << LV_P2);
					}
					else
					{
						PORTF |= (1 << LV_P2);
					}
				}
			}
			if(i==12)
			{
				i=0;
			}
		}
	}
}
void PWM(void)
{
	if(tmp<10)
	{
		tmp=0;
	}
	else
	{
		tmp++;
	}
	
	if((periodCount>=0)&&(periodCount<=250))
	{
		if(timeSlot<dutyCycle)
		{
			blink=1; 
		}
		else
		{
			blink=0;
		}
		timeSlot++;
			
		if(timeSlot==period)
		{
			timeSlot=0;
			if(upDown)
			{
				if(dutyCycle<=10)
				{
					dutyCycle=dutyCycle+1;
				}
			}
			else
			{
				if(dutyCycle>0)
				{
					dutyCycle=dutyCycle-1;
				}
			}
		}
	}
	
	if(!(tmp))
	{
		if(upDown)
		{
			periodCount++;
			if(periodCount==250) upDown=0;
		}
		else
		{
			periodCount--;
			if(periodCount==0) upDown=1;
		}
	}
}
void thomasComes(void)
{
	
	if(thomasTheTankEngine==1)
	{
		switch (thomasTheTankEngineButton)
		{
			
			case 1: thomasLeft=1; break;
			case 2: thomasRight=1; break;
		}
	}
	if(thomasLeft)
	{
		if((thomasTheTankEngineButton==1)&&(thomasTheTankEngine==1))
		{
			thomasTheTankEngine=0;
			PORTF=0;
		}
		if((thomasTheTankEngineButton==2)&&(thomasTheTankEngine==0))
		{
			thomasTheTankEngine=1;
			thomasTheTankEngineButton=0;
			thomasLeft=0;
			PORTA = 0;
			PORTC = 0;
			PORTF=0;
		}
	}
	if(thomasRight)
	{
		if((thomasTheTankEngineButton==2)&&(thomasTheTankEngine==1))
		{
			thomasTheTankEngine=0;
			PORTF=0;
		}
		if((thomasTheTankEngineButton==1)&&(thomasTheTankEngine==0))
		{
			thomasTheTankEngine=1;
			thomasTheTankEngineButton=0;
			thomasRight=0;
			PORTA = 0;
			PORTC = 0;
			PORTF=0;
		}
	}
}
void buttonHandling(void)
{
	if(g1DebounceStart) thomasButtonTimer1++;
	if(g2DebounceStart) thomasButtonTimer2++;
	
	if(thomasButtonTimer1>=250)
	{
		thomasButtonTimer1=0;
		g1DebounceStart=0;
		if((!(PINE & (1<<BTNV1))))
		{
			g1DebounceStart=1;
		}
		if(g1DebounceStart==0) thomasTheTankEngineButton=1;
	}
	
	if(thomasButtonTimer2>=250)
	{
		thomasButtonTimer2=0;
		g2DebounceStart=0;
		if((!(PINE & (1<<BTNV2))))
		{
			g2DebounceStart=1;
		}
		if(g2DebounceStart==0) thomasTheTankEngineButton=2;
	}
}