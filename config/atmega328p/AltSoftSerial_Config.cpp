#include <AltSoftSerial_Config.h>


uint8_t ASS_TICK_LITTLE_TIME;
uint32_t nOverflow=0;
uint32_t timeMultiplier;
uint32_t timeMultiplierMicros;

#define TIME_MULTIPLIER ((256ll*256ll*1000ll*65536ll)/(F_CPU))
#define TIME_MULTIPLIER_MICROS ((1000ll*256ll*1000ll*65536ll)/(F_CPU))

counter_type AltSoftSerial_configureTimer(uint32_t cycles_per_bit, counter_type MAX_COUNTS_PER_BIT)
{
	ASS_TICK_LITTLE_TIME=8;
	if (cycles_per_bit < MAX_COUNTS_PER_BIT) {
		DEBUG_Send_str("cycles_no_prescale\n");
		DEBUG_Send_uint32(cycles_per_bit);
		CONFIG_TIMER_NOPRESCALE();
		ASS_TICK_LITTLE_TIME=64;
		timeMultiplier=TIME_MULTIPLIER;
		timeMultiplierMicros=TIME_MULTIPLIER_MICROS;
	} else {
		cycles_per_bit /= 8;
		//Serial.printf("cycles_per_bit/8 = %d\n", cycles_per_bit);
		if (cycles_per_bit < MAX_COUNTS_PER_BIT) {
			DEBUG_Send_str("cycles_prescale_8\n");
			CONFIG_TIMER_PRESCALE_8();
			timeMultiplier=TIME_MULTIPLIER/8;
			timeMultiplierMicros=TIME_MULTIPLIER_MICROS/8;
		} else {
			cycles_per_bit /= 8;
			if (cycles_per_bit < MAX_COUNTS_PER_BIT) {
				DEBUG_Send_str("cycles_prescale_64\n");
				CONFIG_TIMER_PRESCALE_64();
				timeMultiplier=TIME_MULTIPLIER/64;
				timeMultiplierMicros=TIME_MULTIPLIER_MICROS/64;
			} else {
				cycles_per_bit /= 4;
				if (cycles_per_bit < MAX_COUNTS_PER_BIT) {
					DEBUG_Send_str("cycles_prescale_256\n");
					CONFIG_TIMER_PRESCALE_256();
					timeMultiplier=TIME_MULTIPLIER/256;
					timeMultiplierMicros=TIME_MULTIPLIER_MICROS/256;
				}else
				{
					if (cycles_per_bit < MAX_COUNTS_PER_BIT) {
						DEBUG_Send_str("cycles_prescale_1024\n");
						CONFIG_TIMER_PRESCALE_1024();
						timeMultiplier=TIME_MULTIPLIER/1024;
						timeMultiplierMicros=TIME_MULTIPLIER_MICROS/1024;
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

ISR(TIMER1_OVF_vect)
{
	nOverflow++;
}

uint32_t timer1_millis()
{
	uint64_t ret;
	uint8_t tifr;
	uint16_t tcnt;

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		ret=nOverflow;
		tcnt=TCNT1;
		tifr=TIFR1;
	}
	if(tcnt<32000 && ((tifr&_BV(TOV1))!=0))
	{
		// Overflow not yet counted
		ret++;
	}
	ret<<=16;
	ret+=tcnt;
	ret*=timeMultiplier;
	ret>>=32;
	return ret;
}
uint32_t timer1_micros()
{
	uint64_t ret;
	uint8_t tifr;
	uint16_t tcnt;

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		ret=nOverflow;
		tifr=TIFR1;
		tcnt=TCNT1;
	}
	if(tcnt<32000 && ((tifr&_BV(TOV1))!=0))
	{
		// Overflow not yet counted
		ret++;
	}
	ret<<=16;
	ret+=tcnt;
	ret*=timeMultiplierMicros;
	ret>>=24;
	return ret;
}

