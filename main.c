#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

void init (void);
void pwmInit (void);
void move (unsigned int a,unsigned int b,unsigned int c,unsigned int d);
void usartInit (void);
void timerInit(void);
void adcInit(void);
unsigned int adcRead(void);
void serialSend(unsigned char character);
void detectObstacle(void);
void checkColour(void);
void avgCompare(unsigned int rTotal,unsigned int bTotal, unsigned int gTotal);

static volatile unsigned short count;

ISR( INT0_vect )
{
	
	if(PIND & (1 << 2)) {
		TCNT2 = 0;									// Clear Timer counter 
	}
	else  {
		count = TCNT2;
	}
}

int main(void){

	init();
	pwmInit();
	usartInit();
	timerInit();
	adcInit();
	
	//serialSend('f');
	  
	move (255,0,230,0);// forward
	
	for(unsigned int i = 0; i<5; i++){
		detectObstacle();
		checkColour();
	}	
	move (0,255,255,0);// turn right
	_delay_ms(590);
	
	move (255,0,230,0);// forward
	
	for(unsigned int i = 0; i<4; i++){
		detectObstacle();
		checkColour();
	}
	move (255,0,0,255);// turn left
	_delay_ms(595);
	move (255,0,230,0);// forward
	
	for(unsigned int i = 0; i<4; i++){
		detectObstacle();
		checkColour();
	}	
	move (255,0,0,255);// turn left
	_delay_ms(595);
	
	move (255,0,230,0);// forward
	
	for(unsigned int i = 0; i<9; i++){
		detectObstacle();
		checkColour();
	}	
	move (255,0,0,255);// turn left
	_delay_ms(595);
	move (255,0,230,0);// forward
	
	for(unsigned int i = 0; i<4; i++){
		detectObstacle();
		checkColour();
	}	
	move (255,0,0,255);// turn left
	_delay_ms(595);
	
	move (255,0,230,0);// forward
	
	for(unsigned int i = 0; i<4; i++){
		detectObstacle();
		checkColour();
	}
	
	move (0,255,255,0);// turn right
	_delay_ms(590);
	
	move (255,0,230,0);// forward
	
	for(unsigned int i = 0; i<4; i++){
		detectObstacle();
		checkColour();
	}	
		
	move(0,0,0,0);  // stop
}
void init(void){ // initialization

	DDRD |= 1<<PD5|1<<PD6;
	DDRB |= 1<<PB1|1<<PB2|1<<PB3|1<<PB4|1<<PB5;
	
	DDRD |= 0<<PD2|1<<PD3; 
}

void pwmInit(void){ // PWM initialization

	TCCR0A = (1<< COM0A1) | (1<<COM0B1) | (1<<WGM01) | (1<<WGM00); // timer0 
	TCCR0B = (1<<CS00); //no prescaler
		
	TCCR1A = (1<< COM1A1) | (1<<COM1B1) | (1<<WGM12) | (1<<WGM10); // timer1
	TCCR1B = (1<<CS10); //no prescaler

}
void timerInit(void){
	
	EIMSK |= (1<<INT0); 							//enable external interrupt
	EICRA |= (1<<ISC00); 							// Any logical change on INT0 generates an interrupt request.
													//Config and enable Timer1
	TCCR2B |= 1<<CS20|1<<CS21|1<<CS22; 
	
	sei();

}
void usartInit(void){ 								// USART initialization

	UCSR0B = (1<<TXEN0) | (1<<RXEN0); 				// configure transmitter and reciever
	UCSR0C |= (1<<UCSZ01)|(1<<UCSZ00);	 			// Select UCSRC0, Select transmit/receive msg bit-size
	
	UBRR0L = 103; 									//set baudrate

}

void adcInit(void){										// Adc initialization

    ADMUX = (1<<REFS0)|(1<<MUX0);					// use AVCC and and A as input to adc 
	ADCSRA = (1<<ADEN)|(1<<ADPS1)|(1<<ADPS0); 	// ADC presclaer 128

}
	
void move(unsigned int a,unsigned int b,unsigned int c,unsigned int d){

	OCR0A = a;
	OCR0B = b;
	OCR1A = c;
	OCR1B = d;
	//_delay_ms(1000);

}

void serialSend(unsigned char character){			// print in serial monitor

	while(!(UCSR0A&(1<<UDRE0)));					// Wait until UDR empty
	
		UDR0 = character; 							// Load message byte to UDR register to be sent
	while(!(UCSR0A&(1<<TXC0))); 					// Wait until msg sent

}

void detectObstacle(void){
	
	PORTD |=  (1 << PD3);
	_delay_us( 15 );							// transmit at least 10 us trigger pulse to the HC-SR04 Trig Pin.
	PORTD &= ~(1 << PD3);
	
	if (count>0 && count<15)
		serialSend('1');
	else
		serialSend('0');
		
	_delay_ms(150);
	
}

void checkColour(void){
	
	unsigned int r;										// to save red value
	unsigned int b;										// to save blue value
	unsigned int g;										// to save green value
	
	unsigned int rTotal = 0;
	unsigned int bTotal = 0;
	unsigned int gTotal = 0;
	
	
	for( unsigned int j=0; j<10; j++){				// creating a loop to get 10 values

		PORTB |= (1<<PB3);							// turn on red
		_delay_ms(30);
		r = adcRead();								// get ADC reading	
				
		PORTB &=~(1<<PB3);
		PORTB |= (1<<PB4);
		_delay_ms(30);								// turn on blue
		b = adcRead();								// get ADC reading	
				
		PORTB &=~(1<<PB4);
		PORTB |= (1<<PB5);							// turn on green
		_delay_ms(30);
		g = adcRead();								// get ADC reading	
				
		PORTB &=~(1<<PB5);
		
		rTotal += r;								// incrementing
		gTotal += g;
		bTotal += b;
	}
	
	avgCompare(rTotal,bTotal,gTotal);			// average calculation
	
}

unsigned int adcRead(void){								// read ADC value

	ADCSRA |= (1<<ADSC);							// statrt conversion
	while(ADCSRA & (1<<ADSC));						// wait while convertion finishes
	return ADC;
}

void avgCompare(unsigned int rTotal,unsigned int bTotal, unsigned int gTotal){

	unsigned int rAvg = 0;
	unsigned int bAvg = 0;
	unsigned int gAvg = 0;
	
	unsigned int rTreshold = 500;						// treshold values
	unsigned int bTreshold = 600;
	unsigned int gTreshold = 600;
	
	rAvg = rTotal/10;								// calculate average
	bAvg = bTotal/10;
	gAvg = gTotal/10;
	
	if(rAvg>rTreshold && bAvg<bTreshold && gAvg<gTreshold)		// red
		serialSend('5');
	if(rAvg<rTreshold && bAvg<bTreshold && gAvg>gTreshold)			// green
		serialSend('6');
	if(rAvg<rTreshold && bAvg>bTreshold && gAvg>gTreshold)			// blue
		serialSend('7');
}
