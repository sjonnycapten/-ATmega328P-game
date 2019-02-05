#include "Timer.h"

void Timer::init(uint8_t khz) {
	DDRB |= (1 << DDB1);			//Pin 9 as output for brightness of LCD
	DDRD |= (1 << DDD3);			//Pin 3 as output for IR sensor
	DDRD &= ~(1 << DDD2);			//Pin 2 as input for IR sensor
	PORTD |= (1 << PORTD2);			//Enable pullup resistor pin 2
									
	TCCR1A = 0;						//Following registers will be set to 0
	TCCR1B = 0;
	TCCR1C = 0;
	TCNT1 = 0;
	OCR1A = 0;
	OCR1B = 0;
	TIMSK1 = 0;
	TIFR1 = 0;

	TCCR2A = 0;						
	TCCR2B = 0;
	TCNT2 = 0;
	OCR2A = 0;
	OCR2B = 0;
	TIMSK2 = 0;
	TIFR2 = 0;
	ASSR = 0;
	GTCCR = 0;

	EIMSK = 0;
	EICRA = 0;
	EIFR = 0;

	ADMUX = 0;
	ADCSRA = 0;

	TCCR1A &= ~(1 << COM1A1) | (1 << COM1B1) | (1 << COM0A1) | (1 << COM0B1);	//Normal mode
	TCCR1A |= (1 << WGM10) | (1 << WGM12);										//Fast PWM 8 bit
	TCCR1B |= (1 << CS10);														//Prescaler clock/1
	TIMSK1 |= (1 << TOIE1);														//Timer overflow enable
	
	TCCR2A |= (1 << COM2B1);			//Clear on compare match mode for register B
	TCCR2A |= (1 << WGM20);				//Phase correct PWM
	TCCR2B |= (1 << WGM22);				//Waveform Generation Mode
	TCCR2B |= (1 << CS20);				//Prescaling clock/1
	OCR2A = (8000 / khz);				//PWM speed
	OCR2B = (8000 / khz / 3);

	EICRA |= (1 << ISC00);			//Any logical change of int0 will call to interrupt
	EIMSK |= (1 << INT0);			//External interrupt enable

	ADMUX &= ~(1 << MUX3) | (1 << MUX2) | (1 << MUX1) | (1 << MUX0);		//Pin A0 becomes input for ADC
	ADMUX |= (1 << REFS0);													//5V input for ADC
	ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);					//Division factor of 128 from clock
	ADCSRA |= (1 << ADEN);													//ADC enable

	sei();			//Global interrupt enable
}

void Timer::updateBrightness() {
	ADCSRA |= (1 << ADSC);				//Start conversion
	while (ADCSRA & (1 << ADSC))		//Waiting for result
		OCR1A = ADC >> 2;				//Pin 9 will be set to the result of the ADC, but the 10 bit value will be converterd to 8 bit first
}