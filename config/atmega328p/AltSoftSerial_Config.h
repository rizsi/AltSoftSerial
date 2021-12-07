/** Target specific configuration of AltSoftSerial. Configure to use with ATMega328p chip or compatible.
 */
#ifndef ALT_SOFT_SERIAL_CONFIG_ATMEGA328P_H
#define ALT_SOFT_SERIAL_CONFIG_ATMEGA328P_H
// ATOMIC_BLOCK(ATOMIC_RESTORESTATE) is defined in here
#include <util/atomic.h>

// Disable debugging - hardware UART can be connected to see the debug messages through these macros
#define DEBUG_Send_str(str)
#define DEBUG_Send_uint32(v)

/** Integer type of the counter value used to drive the software serial. */
typedef uint16_t counter_type;
/** Integer type of a single byte value sent through serial. If 9 or 10 bit mode is used then it must be at least uint16_t If 8 bit mode is used then it can be uint8_t*/
typedef uint8_t data_type;

/** Number of ticks that will compensate the execution time of setting up output compare match.
 * Because we always do it in interrupt disabled block a few CPU cycles is enough.
 * Value is set when prescaler is set up. Value is selected to be at least 64 CPU cycles
 * that is always enough to properly set up the output compare registers.
 */
static uint8_t ASS_TICK_LITTLE_TIME;
/** The base frequency of the system.
 * The base frequency of the counter/timer hardware used to implement serial.
 * This is normally the CPU clock.
 * This is used as time base to configure timer counters and input capture counter.
 */
#define ALTSS_BASE_FREQ F_CPU

/// Size of the transmit buffer counted in data_type elements
#define TX_BUFFER_SIZE 68
/// Size of the receive  buffer counted in data_type elements
#define RX_BUFFER_SIZE 80


/** Get the current value of the timer used to time this serial. */
#define GET_TIMER_COUNT()		(TCNT1)

/** Setup pins for serial: TX to output, RX to input */
#define SETUP_PINS() \
	DDRB&=~0b00000001; /* ICP1 - PB0 to input pullup */ \
	PORTB|=0b00000001;	\
	PORTB|=0b00000010;	\
	DDRB|=0b00000010;	/* OC1A - PB1 to output high */


/** Set timeout absolute value when receiver timer must be executed */
#define SET_COMPARE_B(val)		(OCR1B = (val))
/** Get timeout absolute value when receiver timer must be (was) executed */
#define GET_COMPARE_B()		(OCR1B)
/** Enable receiver timer interrupt */
#define ENABLE_INT_COMPARE_B()	(TIFR1 = (1<<OCF1B), TIMSK1 |= (1<<OCIE1B))
/** Disable Timer used for receiver. */
#define DISABLE_INT_COMPARE_B()	(TIMSK1 &= ~(1<<OCIE1B))

#define CAPTURE_INTERRUPT		TIMER1_CAPT_vect
#define COMPARE_A_INTERRUPT		TIMER1_COMPA_vect
#define COMPARE_B_INTERRUPT		TIMER1_COMPB_vect
/** Header of ISR function executed after bit signal is put to output by ouput compare hardware */
#define ISR_COMPARE_A_INTERRUPT() ISR(COMPARE_A_INTERRUPT)
/** Interrupt routine that is executed when receiver timer was elapsed. */
#define ISR_COMPARE_B_INTERRUPT() ISR(COMPARE_B_INTERRUPT)
/** Header of ISR function executed after a receive signal edge was detected */
#define ISR_CAPTURE_INTERRUPT() ISR(CAPTURE_INTERRUPT)



/** Enable input capture and its interrupt */
#define ENABLE_INT_INPUT_CAPTURE()	(TIFR1 = (1<<ICF1), TIMSK1 = (1<<ICIE1))
/** Disable input capture and its interrupt */
#define DISABLE_INT_INPUT_CAPTURE()	(TIMSK1 &= ~(1<<ICIE1))
/** Get the timestamp of the interrupt capture */
#define GET_INPUT_CAPTURE()		(ICR1)

/** Configure input capture to detect a falling edge */
#define CONFIG_CAPTURE_FALLING_EDGE()	(TCCR1B &= ~(1<<ICES1))
/** Configure input capture to detect a rising edge */
#define CONFIG_CAPTURE_RISING_EDGE()	(TCCR1B |= (1<<ICES1))

/** The maximum value of the counter before it overflows to 0. */
#define COUNTER_MAX_VALUE 65535

/** Enable timer interrupt on match on timer used for bits output */
#define ENABLE_INT_COMPARE_A()	(TIFR1 = (1<<OCF1A), TIMSK1 |= (1<<OCIE1A))
/** On output compare match the output is set to low */
#define CONFIG_MATCH_CLEAR()		(TCCR1A = (TCCR1A | (1<<COM1A1)) & ~(1<<COM1A0))
/** On output compare match the output is set to high */
 #define CONFIG_MATCH_SET()		(TCCR1A = TCCR1A | ((1<<COM1A1) | (1<<COM1A0)))
/** On output compare match do nothing */
#define CONFIG_MATCH_NORMAL()		(TCCR1A = TCCR1A & ~((1<<COM1A1) | (1<<COM1A0)))
/** Disable interrupt on output compare match */
#define DISABLE_INT_COMPARE_A()	(TIMSK1 &= ~(1<<OCIE1A))

/** Set the output compare value to the desired value */
#define SET_COMPARE_A(val)		(OCR1A = (val))

/** Read the currently set compare match value */
#define GET_COMPARE_A()		(OCR1A)

#define CONFIGURE_TIMER_PRESCALER()

/** Timer/counter set up with no prescaler */
#define CONFIG_TIMER_NOPRESCALE()	(TIMSK1 = 0, TCCR1A = 0, TCCR1B = (1<<ICNC1) | (1<<CS10))
/** Timer/counter set up with prescaler of 8 */
#define CONFIG_TIMER_PRESCALE_8()	(TIMSK1 = 0, TCCR1A = 0, TCCR1B = (1<<ICNC1) | (1<<CS11))
/** Timer/counter set up with prescaler of 64 */
#define CONFIG_TIMER_PRESCALE_64()	(TIMSK1 = 0, TCCR1A = 0, TCCR1B = (1<<ICNC1) | (1<<CS11) | (1<<CS10))
/** Timer/counter set up with prescaler of 256 */
#define CONFIG_TIMER_PRESCALE_256()	(TIMSK1 = 0, TCCR1A = 0, TCCR1B = (1<<ICNC1) | (1<<CS12))
/** Timer/counter set up with prescaler of 1024 */
#define CONFIG_TIMER_PRESCALE_1024()    (TIMSK1 = 0, TCCR1A = 0, TCCR1B = (1<<ICNC1) | (1<<CS12) | (1<<CS10))

/**
 * Configure timer so that we use most of the available precision of the timer but ticks_per_bit is always
 * less than MAX_COUNTS_PER_BIT
 * In case of ATMega328 we can choose several clock prescaler configurations. This function selects the best fit one.
 * @param cycles_per_bit required baud rate defined by the number of system cycles while a bit signal time
 * @param MAX_COUNTS_PER_BIT the maximum count for a single bit with current settings (depending on the number signals within a single byte)
 * @return ticks_per_bit the number of ticks for a single bit time with the current configuration.
 *    In case a divisor was set up then this is the divided value of cycles_per_bit
 *    0 means no available configuration found.
 */
static inline counter_type AltSoftSerial_configureTimer(uint32_t cycles_per_bit, counter_type MAX_COUNTS_PER_BIT)
{
	ASS_TICK_LITTLE_TIME=8;
	if (cycles_per_bit < MAX_COUNTS_PER_BIT) {
		DEBUG_Send_str("cycles_no_prescale\n");
		DEBUG_Send_uint32(cycles_per_bit);
		CONFIG_TIMER_NOPRESCALE();
		ASS_TICK_LITTLE_TIME=64;
	} else {
		cycles_per_bit /= 8;
		//Serial.printf("cycles_per_bit/8 = %d\n", cycles_per_bit);
		if (cycles_per_bit < MAX_COUNTS_PER_BIT) {
			DEBUG_Send_str("cycles_prescale_8\n");
			CONFIG_TIMER_PRESCALE_8();
		} else {
			cycles_per_bit /= 8;
			if (cycles_per_bit < MAX_COUNTS_PER_BIT) {
				DEBUG_Send_str("cycles_prescale_64\n");
				CONFIG_TIMER_PRESCALE_64();
			} else {
				cycles_per_bit /= 4;
				if (cycles_per_bit < MAX_COUNTS_PER_BIT) {
					DEBUG_Send_str("cycles_prescale_256\n");
					CONFIG_TIMER_PRESCALE_256();
				}else
				{
					if (cycles_per_bit < MAX_COUNTS_PER_BIT) {
						DEBUG_Send_str("cycles_prescale_1024\n");
						CONFIG_TIMER_PRESCALE_1024();
					}else
					{
						return 0; // baud rate too low for AltSoftSerial
					}
				}
			}
		}
	}
	return (counter_type)cycles_per_bit;
}

#endif // ALT_SOFT_SERIAL_CONFIG_ATMEGA328P_H
