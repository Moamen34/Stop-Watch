/*
 * stop_watch.c
 *
 *  Created on: Sep 11, 2024
 *      Author: moamen
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
unsigned char sec=0, min=0 ,hr=0;
unsigned char disaplay[]={0,1,2,3,4,5,6,7,8,9};
unsigned char sec_digit1=0 ,sec_digit2=0 ,min_digit1=0 ,min_digit2=0 ,hr_digit2=0 ,hr_digit1=0;
unsigned char countdown_mode =0;
unsigned char prev_pinb7_state=1;
unsigned char flag=0;
unsigned char pause_flag=0;
unsigned char prev_button1_state=1;
unsigned char prev_button2_state=1;
unsigned char prev_button3_state=1;
unsigned char prev_button4_state=1;
unsigned char prev_button5_state=1;
unsigned char prev_button6_state=1;
void increment_segment(void);
void decrement_segment(void);

/*
uint8_t is_button_pressed(void)
{
	if(!(PINB &(1<<7)))
	{
		_delay_ms(18);
		if(!(PINB &(1<<7)))
		{
			while(!(PINB &(1<<7)));
			_delay_ms(18);
			return 1;
		}
	}
	return 0;
}
*/

void check_mode_switch(void)
{

	if (!(PINB & (1 << PB7))) {
		// Button is pressed (logic low)
			            if (prev_pinb7_state == 1) {  // Check for rising edge (button was released before)
                            if(pause_flag==1)
			                countdown_mode=!countdown_mode;

			                _delay_ms(50);  // Simple debounce delay
			            }
			            prev_pinb7_state = 0;  // Set state to indicate button is pressed
			        } else {
			        	prev_pinb7_state = 1;  // Button is released
			        }

}
ISR(TIMER1_COMPA_vect)
{

	flag=1;

}

ISR(INT0_vect)
{
	sec_digit1=0 ,sec_digit2=0 ,min_digit1=0 ,min_digit2=0 ,hr_digit2=0 ,hr_digit1=0;
	sec=0, min=0 ,hr=0,TCNT1=0,PORTD &=~(1<<0);
}
ISR(INT1_vect)
{
	TCCR1B &=~(1<<CS10)&~(1<<CS11)&~(1<<CS12);
	pause_flag=1;

}
ISR(INT2_vect)
{
	TCCR1B |=(1<<CS12);
	pause_flag=0;


}
void reset_using_INT0(void)
{
	DDRD &= ~(1<<2);
	PORTD |=(1<<PD2);
	MCUCR |= (1<<ISC01);
	MCUCR &= ~(1<<ISC00);
	GICR |= (1<<6);
	GIFR |=(1<<INTF0);

}
void pause_using_INT1(void)
{
	DDRD &= ~(1<<3);
	PORTD &= ~(1<<PD3);
	MCUCR |= (1<<ISC11) | (1<<ISC10);
	GICR |= (1<<7);
	GIFR |=(1<<INTF1);

}
void resume_using_INT2(void)
{
	   DDRB &= ~(1<<PB2);
		PORTB |=(1<<PB2);
		MCUCSR |=(1<<ISC2);
		GICR |= (1<<5);
		GIFR |=(1<<INTF2);
}
void timer_1_on(void)
{

	TCNT1 =0;
	TCCR1A |=(1<<FOC1A);


	TCCR1B =(1<<WGM12) | (1<<CS12);
	OCR1A = 62500;
	TIMSK |= (1<<OCIE1A);

}
void increment_segment(void)
{
	if (hr == 23 && min == 59 && sec == 59) {
		hr=0 , min=0 , sec=0;
		_delay_ms(2);
	}
	sec++;

	        if (sec > 59)
	        {
	            sec = 0;
	            min++;
	        }
	        if (min > 59)
	        {
	            min = 0;
	            hr++;
	        }
	        if (hr > 23)
	        {
	            hr = 0;
	        }
	        sec_digit2 = sec / 10;
	            sec_digit1 = sec - sec_digit2 * 10;
	            min_digit2 = min / 10;
	            min_digit1 = min - min_digit2 * 10;
	            hr_digit2 = hr / 10;
	            hr_digit1 = hr - hr_digit2 * 10;
}

void decrement_segment()
{
	if (hr == 0 && min == 0 && sec == 0) {
	        PORTD |= (1 << PD0);


	        return;
	    }


	        if (sec == 0)
	        {
	            sec = 59;
	            if (min == 0)
	            {
	                min = 59;
	                if (hr > 0)
	                {
	                    hr--;
	                }
	            }
	            else
	            {
	                min--;
	            }
	        }
	        else
	        {
	            sec--;
	        }
	                        sec_digit2 = sec / 10;
	        	            sec_digit1 = sec - sec_digit2 * 10;
	        	            min_digit2 = min / 10;
	        	            min_digit1 = min - min_digit2 * 10;
	        	            hr_digit2 = hr / 10;
	        	            hr_digit1 = hr - hr_digit2 * 10;
}
void hrs_inc(void)
{
	if (!(PINB & (1 << PB1))) {  // Button is pressed (logic low)
	            if (prev_button1_state == 1) {  // Check for rising edge (button was released before)
	                hr++;
	                if (hr > 23) {  // If hours exceed 23, reset to 00
	                                hr = 0;
	                }
	                hr_digit2 = hr / 10;
	                    hr_digit1 = hr - hr_digit2 * 10; // Display the count on PORTC

	                _delay_ms(50);  // Simple debounce delay
	            }
	            prev_button1_state = 0;  // Set state to indicate button is pressed
	        } else {
	            prev_button1_state = 1;  // Button is released
	        }
}
void hrs_dec(void)
{
	if (!(PINB & (1 << PB0))) {  // Button is pressed (logic low)
		            if (prev_button2_state == 1) {  // Check for rising edge (button was released before)
		                hr--;
		                if (hr == 0) {  // If hours exceed 23, reset to 00
		                                hr = 0;
		                            }
		                hr_digit2 = hr / 10;
		                    hr_digit1 = hr - hr_digit2 * 10; // Display the count on PORTC

		                _delay_ms(50);  // Simple debounce delay
		            }
		            prev_button2_state = 0;  // Set state to indicate button is pressed
		        } else {
		            prev_button2_state = 1;  // Button is released
		        }
}
void min_inc(void)
{
	if (!(PINB & (1 << PB4))) {  // Button is pressed (logic low)
		            if (prev_button3_state == 1) {  // Check for rising edge (button was released before)
		                min++;
		                if (min > 59) {  // If hours exceed 23, reset to 00
		                                min = 0;
		                            }

		                min_digit2 = min / 10;
		                    min_digit1 = min - min_digit2 * 10; // Display the count on PORTC

		                    			_delay_ms(2);
		                _delay_ms(50);  // Simple debounce delay
		            }
		            prev_button3_state = 0;  // Set state to indicate button is pressed
		        } else {
		            prev_button3_state = 1;  // Button is released
		        }
}
void min_dec(void)
{
	if (!(PINB & (1 << PB3))) {  // Button is pressed (logic low)
		            if (prev_button4_state == 1) {  // Check for rising edge (button was released before)
		                min--;
		                if (min == 0) {  // If hours exceed 23, reset to 00
		                                min = 0;
		                            }

		                min_digit2 = min / 10;
		                    min_digit1 = min - min_digit2 * 10; // Display the count on PORTC

		                _delay_ms(50);  // Simple debounce delay
		            }
		            prev_button4_state = 0;  // Set state to indicate button is pressed
		        } else {
		            prev_button4_state = 1;  // Button is released
		        }
}
void sec_inc(void)
{
	if (!(PINB & (1 << PB6))) {  // Button is pressed (logic low)
			            if (prev_button5_state == 1) {  // Check for rising edge (button was released before)
			                sec++;
			                if (sec > 59) {  // If hours exceed 23, reset to 00
			                                sec = 0;
			                            }

			                sec_digit2 = sec / 10;
			                    sec_digit1 = sec - sec_digit2 * 10; // Display the count on PORTC

			                _delay_ms(50);  // Simple debounce delay
			            }
			            prev_button5_state = 0;  // Set state to indicate button is pressed
			        } else {
			            prev_button5_state = 1;  // Button is released
			        }
}
void sec_dec(void)
{
	if (!(PINB & (1 << PB5))) {  // Button is pressed (logic low)
				            if (prev_button6_state == 1) {  // Check for rising edge (button was released before)
				                sec--;
				                if (sec == 0) {  // If hours exceed 23, reset to 00
				                      sec=0;
				                            }


				                _delay_ms(50);  // Simple debounce delay
				            }
				            prev_button6_state = 0;  // Set state to indicate button is pressed
				        } else {
				            prev_button6_state = 1;  // Button is released
				        }
}
int main(void)
{
	DDRD |= (1<<0) | (1<<4) | (1<<5);
	DDRC = (1<<0) | (1<<1) | (1<<2) | (1<<3);
    DDRB &=~(1<<7)&~(1<<0)&~(1<<1)&~(1<<3)&~(1<<4)&~(1<<5)&~(1<<6);
   PORTB |=(1<<1)|(1<<3)|(1<<0)|(1<<4)|(1<<5)|(1<<6);
   PORTB |=(1<<7);

	SREG |=(1<<7);
	timer_1_on();
	reset_using_INT0();
	pause_using_INT1();
	resume_using_INT2();
DDRA |= (1<<5);
PORTA |=(1<<5);
while(1)
{

	PORTA &=~(1<<PA5)&~(1<<PA3)&~(1<<PA2)&~(1<<PA1)&~(1<<PA0);
			PORTA |=(1<<PA4);
			PORTC = disaplay [sec_digit2];
			_delay_ms(2);


			PORTA &=~(1<<PA4)&~(1<<PA3)&~(1<<PA2)&~(1<<PA1)&~(1<<PA0);
			PORTA |=(1<<PA5);
			PORTC = disaplay [sec_digit1];
			_delay_ms(2);

			PORTA &=~(1<<PA4)&~(1<<PA3)&~(1<<PA5)&~(1<<PA1)&~(1<<PA0);
			PORTA |=(1<<PA2);
			PORTC = disaplay [min_digit2];
			_delay_ms(2);

			PORTA &=~(1<<PA4)&~(1<<PA5)&~(1<<PA2)&~(1<<PA1)&~(1<<PA0);
			PORTA |=(1<<PA3);
			PORTC = disaplay [min_digit1];
			_delay_ms(2);

			PORTA &=~(1<<PA4)&~(1<<PA3)&~(1<<PA2)&~(1<<PA1)&~(1<<PA5);
			PORTA |=(1<<PA0);
			PORTC = disaplay [hr_digit2];
			_delay_ms(2);

			PORTA &=~(1<<PA4)&~(1<<PA3)&~(1<<PA2)&~(1<<PA5)&~(1<<PA0);
			PORTA |=(1<<PA1);
			PORTC = disaplay [hr_digit1];
			_delay_ms(2);



	check_mode_switch();

			if(flag==1)
			{

if (!(PINB & (1<<7))||countdown_mode==1)
{
	decrement_segment();


}
else if((PINB & (1<<7)))
{

	increment_segment();
}

				flag=0;
			}
if (countdown_mode==0)
{
	PORTD |=(1<<4);
	PORTD &=~(1<<5);
}
if(!(PINB & (1<<7))||countdown_mode==1)
{
	PORTD |=(1<<5);
	PORTD &=~(1<<4);
}


hrs_inc();
hrs_dec();
min_inc();
min_dec();
sec_inc();
sec_dec();

}
}

