/** Target specific configuration of AltSoftSerial. Configure to use with ATMega328p chip or compatible.
 */
#ifndef ALT_SOFT_SERIAL_CONFIG_ATMEGA328P_H
#define ALT_SOFT_SERIAL_CONFIG_ATMEGA328P_H
// ATOMIC_BLOCK(ATOMIC_RESTORESTATE) is defined in here
#include <util/atomic.h>

// Disable debugging - hardware UART can be connected to see the debug messages through these macros
#define DEBUG_Send_str(str)
#define DEBUG_Send_uint32(v)

#include <AltSoftSerial_atmega328_pins.h>

/** Integer type of the counter value used to drive the software serial. */
typedef uint16_t counter_type;
/** Integer type of a single byte value sent through serial. If 9 or 10 bit mode is used then it must be at least uint16_t If 8 bit mode is used then it can be uint8_t*/
typedef uint8_t data_type;


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





#define CAPTURE_INTERRUPT		TIMER1_CAPT_vect
/** Header of ISR function executed after a receive signal edge was detected */
#define ISR_CAPTURE_INTERRUPT() ISR(CAPTURE_INTERRUPT)



/** Enable input capture and its interrupt */
#define ENABLE_INT_INPUT_CAPTURE()	(TIFR1 = (1<<ICF1), TIMSK1 |= (1<<ICIE1))
/** Disable input capture and its interrupt */
#define DISABLE_INT_INPUT_CAPTURE()	(TIMSK1 &= ~(1<<ICIE1), TIFR1 = (1<<ICF1))
/** Get the timestamp of the interrupt capture */
#define GET_INPUT_CAPTURE()		(ICR1)

/** Configure input capture to detect a falling edge */
#define CONFIG_CAPTURE_FALLING_EDGE()	(TCCR1B &= ~(1<<ICES1))
/** Configure input capture to detect a rising edge */
#define CONFIG_CAPTURE_RISING_EDGE()	(TCCR1B |= (1<<ICES1))

/** The maximum value of the counter before it overflows to 0. */
#define COUNTER_MAX_VALUE 65535


#define CONFIGURE_TIMER_PRESCALER()

/** Timer/counter set up with no prescaler */
#define CONFIG_TIMER_NOPRESCALE()	(TIMSK1 = _BV(TOIE1), TCCR1A = 0, TCCR1B = (1<<ICNC1) | (1<<CS10))
/** Timer/counter set up with prescaler of 8 */
#define CONFIG_TIMER_PRESCALE_8()	(TIMSK1 = _BV(TOIE1), TCCR1A = 0, TCCR1B = (1<<ICNC1) | (1<<CS11))
/** Timer/counter set up with prescaler of 64 */
#define CONFIG_TIMER_PRESCALE_64()	(TIMSK1 = _BV(TOIE1), TCCR1A = 0, TCCR1B = (1<<ICNC1) | (1<<CS11) | (1<<CS10))
/** Timer/counter set up with prescaler of 256 */
#define CONFIG_TIMER_PRESCALE_256()	(TIMSK1 = _BV(TOIE1), TCCR1A = 0, TCCR1B = (1<<ICNC1) | (1<<CS12))
/** Timer/counter set up with prescaler of 1024 */
#define CONFIG_TIMER_PRESCALE_1024()    (TIMSK1 = _BV(TOIE1), TCCR1A = 0, TCCR1B = (1<<ICNC1) | (1<<CS12) | (1<<CS10))

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
counter_type AltSoftSerial_configureTimer(uint32_t cycles_per_bit, counter_type MAX_COUNTS_PER_BIT);

/** Number of ticks that will compensate the execution time of setting up output compare match.
 * Because we always do it in interrupt disabled block a few CPU cycles is enough.
 * Value is set when prescaler is set up. Value is selected to be at least 64 CPU cycles
 * that is always enough to properly set up the output compare registers.
 */
extern uint8_t ASS_TICK_LITTLE_TIME;

/**
 * Milliseconds since TIMER1 has bees started.
 */
uint32_t timer1_millis();
/**
 * Microseconds since TIMER1 has bees started.
 */
uint32_t timer1_micros();


#endif // ALT_SOFT_SERIAL_CONFIG_ATMEGA328P_H
