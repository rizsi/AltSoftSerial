/* An Alternative Software Serial Library
 * http://www.pjrc.com/teensy/td_libs_AltSoftSerial.html
 * Copyright (c) 2014 PJRC.COM, LLC, Paul Stoffregen, paul@pjrc.com
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

// Original code is found on github:
// https://github.com/PaulStoffregen/AltSoftSerial
// This alternative version is: de-Arduino-ed, hardware mapping is moved to HW secific folder and different serial formats are implemented.


#include "AltSoftSerial.h"



/****************************************/
/**          Initialization            **/
/****************************************/

/** Counter ticks per bit time */
static counter_type ticks_per_bit=0;

/** Is parity enabled? */
static uint8_t ass_parity;
/** Number of data bits in a single data */
static uint8_t ass_nBit;
/** Number of signals in a single data: start+data+parity+stop */
static uint8_t ass_nSignal;
/** Mask to OR with tx_byte in case the parity is even. Stores parity and stop bits. */
static uint16_t ass_mask_even;
/** Mask to OR with tx_byte in case the parity is odd.  Stores parity and stop bits. */
static uint16_t ass_mask_odd;
/** Mask to remove parity and stop bits from the high positions. */
static data_type ass_receive_mask;

/** Index of bit currently being received. 0:idle wait for start edge
 * 1 receiving start bit (just after start edge)
 * 1+n receiving data, parity or stop bit
 */
static uint8_t rx_state;
/** The data currently being received. */
static uint16_t rx_byte;
/** Bit mask to store current received bit. */
static uint16_t rx_mask;
/** The current received bit value. Using the capture interrupt this is maintained to reflect the current state of the line. 
 * 1 is active similar to start bit (typically low but depends on invert settings)
 * 0 is inactive similar to start but (typically high but depends on invert settings)
 */
static uint8_t rx_bit = 0;
/** The timestamp at which the next bit have to be sampled. The middle of the signal. */
static uint16_t rx_target;
/** Number of ticks after the receive of data process is finished. Counted from the first edge of the start bit. Value is: start+datas+parity+stops-half. So the time is in the middle of last stop bit.
 * At this time receive of a bit is finished independent of the detected edges. */
static uint16_t rx_stop_ticks=0;
/** Receive ringbuffer head offset */
static volatile uint8_t rx_buffer_head;
/** Receive ringbuffer tail offset */
static volatile uint8_t rx_buffer_tail;
/** Receive ringbuffer data */
static volatile data_type rx_buffer[RX_BUFFER_SIZE];

/** Number of remaining signals to be sent plus 1 to wait. Indexed from ass_nSignal+1.
 * 0 means transmit is finished/not started.
 * 1 means last stop bit is being transmitted.
 * ass_nSignal+1 means that the start bit will be sent when the next output compare match happens (and we are either in idle state or during the last stop bit)
 * 1<n<ass_nSignal+1 means that we are currently transmitting the signal indexed by n (counting towards 0)
 */
static volatile uint8_t tx_state=0;
/** The current data being sent. LSB is the next signal to send. Start bit, n data bit, parity and stop bits are stored in this sample. */
static uint16_t tx_byte;
/** The head offset of the transmit ring buffer. */
static volatile uint8_t tx_buffer_head;
/** The tail offset of the transmit ring buffer. */
static volatile uint8_t tx_buffer_tail;
/** The transmit ring buffer. */
static volatile data_type tx_buffer[TX_BUFFER_SIZE];

void AltSoftSerial::begin(uint32_t baud)
{
	 init((ALTSS_BASE_FREQ + baud / 2) / baud, 8, 0, 1);
}
void AltSoftSerial::begin(uint32_t baud, uint8_t nBit,  uint8_t parity, uint8_t nStopBit)
{
	 init((ALTSS_BASE_FREQ + baud / 2) / baud, nBit, parity, nStopBit);
}


void AltSoftSerial::init(uint32_t cycles_per_bit, uint8_t nBit,  uint8_t parity, uint8_t nStopBit)
{
	ass_parity=parity;
	ass_nBit=nBit;
	ass_nSignal=1+nBit+(parity==0?0:1)+nStopBit;
	// Timer is set up when start edge is detected and must not overflow until the middle of the last stop bit.
	counter_type MAX_COUNTS_PER_BIT=((uint32_t)COUNTER_MAX_VALUE)*2/(2*ass_nSignal-1);
	ticks_per_bit = AltSoftSerial_configureTimer(cycles_per_bit, MAX_COUNTS_PER_BIT);
	rx_stop_ticks = ticks_per_bit * (ass_nSignal-1) + ticks_per_bit/2;
	/** Mask to OR with tx_byte in case the parity is even. Stores parity and stop bits. */
	uint16_t mask=0;
	uint16_t maskParity=0;
	ass_receive_mask=0;
	for(uint8_t i=0;i<nStopBit;++i)
	{
		mask<<=1;
		mask|=1;
	}
	if(parity!=0)
	{
		mask<<=1;
		maskParity=1;
	}
	for(uint8_t i=0;i<nBit;++i)
	{
		mask<<=1;
		maskParity<<=1;
		ass_receive_mask<<=1;
		ass_receive_mask|=1;
	}
	mask<<=1;
	maskParity<<=1;
	ass_mask_even=(parity==1)?(mask|maskParity):mask;	// TODO check parity
	ass_mask_odd=(parity==2)?(mask|maskParity):mask;

	SETUP_PINS();
	rx_state = 0;
	rx_bit = 0;
	rx_buffer_head = 0;
	rx_buffer_tail = 0;
	rx_byte=0;
	rx_mask=1;

	tx_state = 0;
	tx_buffer_head = 0;
	tx_buffer_tail = 0;
	ENABLE_INT_INPUT_CAPTURE();
}
bool AltSoftSerial::isTxOn()
{
	return true;	// TODO
}

void AltSoftSerial::end(void)
{
	DISABLE_INT_COMPARE_B();
	DISABLE_INT_INPUT_CAPTURE();
	flushInput();
	flushOutput();
	DISABLE_INT_COMPARE_A();
}


/****************************************/
/**           Transmission             **/
/****************************************/

/** Prepare the data for sending:
 * extend to 16 bits
 * shift to position and add start and stop bits
 * add parity if necessary
 * @return the data converted to signal bits that are just shifted out to transmit.
 */
static inline uint16_t prepareSend(data_type data)
{
	uint16_t t=((uint16_t) data)<<1;	// Lowest bit is the start bit
	if(ass_parity)
	{
		data_type c=data;
		uint8_t p=0;
		for(uint8_t i=0;i<ass_nBit;++i)
		{
			p^=c&1;
			c>>=1;
		}
		if(p)
		{
			t |= ass_mask_odd;
		}
		else
		{
			t |= ass_mask_even;
		}
	}else
	{
		t |= ass_mask_odd; // Odd and even masks are equal in this case
	}
	return t;
}
/**
 * Set the output compare hardware to output the next signal bit (start, stop, parity or data)
 * at the requested time.
 */
static inline void setupSendBit(counter_type signalTime)
{
	uint8_t bit = tx_byte & 1;
	tx_byte >>= 1;	// Shift for next bit
	// First set the output compare register
	// and only then set the operation: this makes sure that a previous stale register value will not trigger an accidental signal early
	SET_COMPARE_A(signalTime);
	if (bit) {
		CONFIG_MATCH_SET();
		PORTB|=0b00000010;
	} else {
		CONFIG_MATCH_CLEAR();
		PORTB&=~0b00000010;
	}
}


void AltSoftSerial::writeByte(data_type b)
{
	uint8_t intr_state, head;

	head = tx_buffer_head + 1;
	if (head >= TX_BUFFER_SIZE) head = 0;
	while (tx_buffer_tail == head) ; // wait until space in buffer
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		if (tx_state) {
			tx_buffer[head] = b;
			tx_buffer_head = head;
		} else {
			tx_state = ass_nSignal+1;
			tx_byte = prepareSend(b);
			setupSendBit(GET_TIMER_COUNT() + ASS_TICK_LITTLE_TIME);
			ENABLE_INT_COMPARE_A();
		}
	}
}

ISR_COMPARE_A_INTERRUPT()
{
	tx_state--; // Count transmit state
	uint8_t state = tx_state;
	if (state > 1) {
		setupSendBit(GET_COMPARE_A() + ticks_per_bit);
	}else
	{
		uint8_t head = tx_buffer_head;
		uint8_t tail = tx_buffer_tail;
		if (head == tail) {
			if (state == 1) {
				// Wait for final stop bit to finish
				SET_COMPARE_A(GET_COMPARE_A() + ticks_per_bit);
			} else {
				// Transmit fully finished - new transmit can be started at any moment from now.
				CONFIG_MATCH_NORMAL();
				DISABLE_INT_COMPARE_A();
			}
		} else {
			// Transmit of byte finished - start next byte
			if (++tail >= TX_BUFFER_SIZE) tail = 0;
			tx_buffer_tail = tail;
			tx_byte = prepareSend(tx_buffer[tail]);
			tx_state = ass_nSignal+1;
			if (state == 1)
			{
				setupSendBit(GET_COMPARE_A() + ticks_per_bit);
			}
			else
			{
				setupSendBit(GET_TIMER_COUNT() + ASS_TICK_LITTLE_TIME);
			}
		}
	}
}

void AltSoftSerial::flushOutput(void)
{
	while (tx_state) /* wait */ ;
}

void AltSoftSerial::flushWrite(void)
{
	flushOutput();
}


/****************************************/
/**            Reception               **/
/****************************************/

static inline void setupCapture(uint8_t bit)
{
	if (bit) {
		CONFIG_CAPTURE_FALLING_EDGE();
		rx_bit = 0;
	} else {
		CONFIG_CAPTURE_RISING_EDGE();
		rx_bit = 1;
	}
}

static inline void fillReceived(uint8_t currentBit, counter_type capture)
{
	uint8_t state=rx_state;
	counter_type target=rx_target;
	counter_type offset = capture - target;
	counter_type offset_overflow = COUNTER_MAX_VALUE - ticks_per_bit;
	// Edge happened after required sampling time but before overflow time

	// Edge happening before required sample time is ignored - that is ok
	// Overflow of counter can not happen because timeout would cancel receive before that happens
	while (offset < offset_overflow && state<17)
	{
		// Save bit received
		if(!currentBit)
		{
			rx_byte|=rx_mask;
		}
		rx_mask<<=1;
		state++; // Count state of receive
		target+=ticks_per_bit;
		offset = capture - target;
	}
	rx_target=target;
	rx_state=state;
}


ISR_CAPTURE_INTERRUPT()
{
	counter_type capture = GET_INPUT_CAPTURE();
	uint8_t bit = rx_bit;
	setupCapture(bit);
	uint8_t state = rx_state;
	if (state == 0) {
		if (!bit) {
			uint16_t end = capture + rx_stop_ticks;
			SET_COMPARE_B(end);
			ENABLE_INT_COMPARE_B();
			rx_target = capture + ticks_per_bit/2;
			rx_state = 1;
		}
	} else {
		fillReceived(bit, capture);
	}
	//if (GET_TIMER_COUNT() - capture > ticks_per_bit) AltSoftSerial::timing_error = true;
}

/** Decode a single data from the signal bits.
 * also check for errors (parity, start bit, stop bits).
 * @return decoded value or 32768 in case of error. (Testing for highest bit is enough because that is never 1 for a valid value).
 */
static inline uint16_t decodeData()
{
	uint16_t r=rx_byte;
	if(r & 1)
	{
		// Start bit is invalid
		return 32768;
	}
	r>>=1;
	// TODO check parity and stop bits
	return ((data_type)r)&ass_receive_mask;
}


ISR_COMPARE_B_INTERRUPT()
{
	DISABLE_INT_COMPARE_B();
	CONFIG_CAPTURE_FALLING_EDGE();
	fillReceived(rx_bit, GET_COMPARE_B());
	
	uint16_t receivedData = decodeData();
	if((receivedData & 32768) == 0)
	{
		uint8_t	head = rx_buffer_head + 1;
		if (head >= RX_BUFFER_SIZE) head = 0;
		if (head != rx_buffer_tail) {
			rx_buffer[head] = (data_type)receivedData;
			rx_buffer_head = head;
		}
	}else
	{
		// Error but be have no means to handle it for now
	}

	rx_state = 0;
	CONFIG_CAPTURE_FALLING_EDGE();
	rx_bit = 0;
	rx_byte=0;
	rx_mask=1;
}

int AltSoftSerial::read(void)
{
	uint8_t head, tail, out;

	head = rx_buffer_head;
	tail = rx_buffer_tail;
	if (head == tail) return -1;
	if (++tail >= RX_BUFFER_SIZE) tail = 0;
	out = rx_buffer[tail];
	rx_buffer_tail = tail;
	return out;
}

int AltSoftSerial::peek(void)
{
	uint8_t head, tail;

	head = rx_buffer_head;
	tail = rx_buffer_tail;
	if (head == tail) return -1;
	if (++tail >= RX_BUFFER_SIZE) tail = 0;
	return rx_buffer[tail];
}

int AltSoftSerial::available(void)
{
	uint8_t head, tail;

	head = rx_buffer_head;
	tail = rx_buffer_tail;
	if (head >= tail) return head - tail;
	return RX_BUFFER_SIZE + head - tail;
}

int AltSoftSerial::availableForWrite(void)
{ 
	uint8_t head, tail;
	head = tx_buffer_head;
	tail = tx_buffer_tail;

	if (tail > head) return tail - head;
	return TX_BUFFER_SIZE + tail - head;
};

void AltSoftSerial::flushInput(void)
{
	rx_buffer_head = rx_buffer_tail;
}

