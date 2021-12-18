#include <AltSoftSerial_Config.h>


uint8_t ASS_TICK_LITTLE_TIME;
uint32_t nOverflow=0;


counter_type AltSoftSerial_configureTimer(uint32_t cycles_per_bit, counter_type MAX_COUNTS_PER_BIT)
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

ISR(TIMER1_OVF_vect)
{
	nOverflow++;
}

uint32_t timer1_millis()
{
	return nOverflow*1000/244;	// TODO Not very accurate
}
