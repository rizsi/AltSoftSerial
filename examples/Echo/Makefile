# AltSoftSerial atmega328p example makefile
.PHONY: clean arduino minicom

MCU = atmega328p
MCU_DUDE = m328p
BINARYNAME = altsoftserial_echo
MHZ = 16mhz
F_CPU = 16000000

SRCS = echo.cpp ../../AltSoftSerial.cpp ../../config/atmega328p/debug_uart0.cpp

HEADERS = ../../AltSoftSerial.h ../../config/atmega328p/AltSoftSerial_Config.h

INCLUDE = -I ../../config/atmega328p/ -I ../../

all: ${BINARYNAME}.hex

${BINARYNAME}.hex: ${SRCS} ${HEADERS}
	avr-gcc -o $(BINARYNAME).elf -mmcu=$(MCU) -DF_CPU=${F_CPU} -O3 -Wl,-Map,$(BINARYNAME).map $(SRCS) $(ASM_SRCS) $(INCLUDE) -Wno-write-strings
	avr-objdump -S --disassemble $(BINARYNAME).elf >$(BINARYNAME).disassemble.txt
	avr-objcopy -j .text -j .data -O ihex $(BINARYNAME).elf $(BINARYNAME).hex
	avr-size -C --mcu=$(MCU) $(BINARYNAME).elf


# Use this target to burn the program to an Arduino UNO
arduino: all
	avrdude -carduino  -P /dev/ttyACM* -b115200 -p $(MCU_DUDE) -u -U flash:w:$(BINARYNAME).hex

clean:
	rm -f ${BINARYNAME}.hex
	rm -f ${BINARYNAME}.disassemble.txt
	rm -f ${BINARYNAME}.elf
	rm -f ${BINARYNAME}.map

#all:
#	avr-gcc -o out.elf ../AltSoftSerial.cpp ../examples/dummy.cpp -DF_CPU=16000000UL

# Launch minicom command line - you may have to configure the USB-serial port
minicom:
	minicom -D /dev/ttyACM0 -b 19200 -w
