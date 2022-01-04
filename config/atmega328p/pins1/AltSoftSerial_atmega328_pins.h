/** Setup pins for serial: TX to output, RX to input */
#define SETUP_PINS() \
	DDRB&=~0b00000001; /* ICP1 - PB0 to input pullup */ \
	PORTB|=0b00000001;	\
	PORTB|=0b00000010;	\
	DDRB|=0b00000010;	/* OC1A - PB1 to output high */


/** Set timeout absolute value when receiver timer must be executed */
#define SET_TIMEOUT(val)		(OCR1B = (val))
/** Get timeout absolute value when receiver timer must be (was) executed */
#define GET_TIMEOUT()		(OCR1B)
/** Enable receiver timer interrupt */
#define ENABLE_INT_TIMEOUT()	(TIFR1 = (1<<OCF1B), TIMSK1 |= (1<<OCIE1B))
/** Disable Timer used for receiver. */
#define DISABLE_INT_TIMEOUT()	(TIMSK1 &= ~(1<<OCIE1B))

/** Header of ISR function executed after bit signal is put to output by ouput compare hardware */
#define ISR_OUTPUT_INTERRUPT() ISR(TIMER1_COMPA_vect)
/** Interrupt routine that is executed when receiver timer was elapsed. */
#define ISR_TIMEOUT_INTERRUPT() ISR(TIMER1_COMPB_vect)

/** Enable timer interrupt on match on timer used for bits output */
#define ENABLE_INT_OUTPUT()	(TIFR1 = (1<<OCF1A), TIMSK1 |= (1<<OCIE1A))
/** On output compare match the output is set to low */
#define CONFIG_MATCH_CLEAR()		(TCCR1A = (TCCR1A | (1<<COM1A1)) & ~(1<<COM1A0))
/** On output compare match the output is set to high */
 #define CONFIG_MATCH_SET()		(TCCR1A = TCCR1A | ((1<<COM1A1) | (1<<COM1A0)))
/** On output compare match do nothing */
#define CONFIG_MATCH_NORMAL()		(TCCR1A = TCCR1A & ~((1<<COM1A1) | (1<<COM1A0)))
/** Disable interrupt on output compare match */
#define DISABLE_INT_OUTPUT()	(TIMSK1 &= ~(1<<OCIE1A))

/** Set the output compare value to the desired value */
#define SET_OUTPUT(val)		(OCR1A = (val))

/** Read the currently set compare match value */
#define GET_OUTPUT()		(OCR1A)
