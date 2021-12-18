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

#ifndef AltSoftSerial_h
#define AltSoftSerial_h

#include <inttypes.h>
// Configure compiler so that the platform specific include file is found.
#include <AltSoftSerial_Config.h>

class AltSoftSerial
{
public:
	AltSoftSerial() { }
	~AltSoftSerial() { end(); }
	/** Configure hardware and start receiver and transmitter. @param baud baud rate in signal/seconds */
	void begin(uint32_t baud);
	/**
	 * Configure hardware and start receiver and transmitter.
	 * @param baud baud baud rate in signal/seconds
	 * @param nBit number of bits per sample (valid values: 1-14 but start, stop data and parity must fit 16 bits)
	 * @param parity 0:no parity 1:odd parity 2:even parity
	 * @param nStopBit number of stop bits (valid values: 1, 2) */
	void begin(uint32_t baud, uint8_t nBit,  uint8_t parity, uint8_t nStopBit);
	/** Stop receiver and transmitter. Hardware is not reset to original state: pins remain configured for serial.
	 * Timer remains counting but timer interrupts are disabled.
	 */
	void end();
	int peek();
	int read();
	int available();
	int availableForWrite();
	void write(uint8_t byte) { writeByte(byte); }
//	void flush() { flushInput(); }
	void flushInput();
	/** Blocking wait until all output is sent from the output buffer. */
	void flushOutput();
	/** Same as flushOutput. */
	void flushWrite();
	/** Is the transmit logic running? */
	bool isTxOn();
	bool overflow() { bool r = timing_error; timing_error = false; return r; }
	static bool timing_error;
private:
	/**
	 * Configure hardware and start receiver and transmitter.
	 * @param cycles_per_bit number of CPU cycles (or the time base for the timer used) per a single bit time
	 * @param nBit number of bits per sample (valid values: 1-14 but start, stop data and parity must fit 16 bits)
	 * @param parity 0:no parity 1:odd parity 2:even parity
	 * @param nStopBit number of stop bits (valid values: 1, 2) */
	static void init(uint32_t cycles_per_bit, uint8_t nBit,  uint8_t parity, uint8_t nStopBit);
	/**
	 * Send a single data.
	 * @param byte if data_type is not uint8_t then it may be more than 8 bit
	 */
	static void writeByte(data_type byte);
};

#endif
