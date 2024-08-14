MCPU=atmega328p
PROGTYPE=stk500v2
PROGPORT=/dev/ttyACM0
PROG_FLAGS=-B 50 -i 50
LFUSE=0xE6
HFUSE=0xDF
AVRDUDE=avrdude -p $(MCPU) -c $(PROGTYPE) -P $(PROGPORT) $(PROG_FLAGS)

CC=avr-gcc
OBJCOPY=avr-objcopy
CFLAGS=-g -Wall -Os -mmcu=$(MCPU) -DF_CPU=16000000
#LDLIBS=-lgcc
HOSTCC=gcc

TARGET=raster

$(TARGET).hex: $(TARGET).elf
	avr-objcopy -j .text -j .data -O ihex $^ $@

$(TARGET).elf: main.o serial.o timer1.o timer2.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

flash: $(TARGET).hex
	$(AVRDUDE) -U flash:w:$^:i

fuses:
	$(AVRDUDE) -U lfuse:w:$(LFUSE):m
	$(AVRDUDE) -U hfuse:w:$(HFUSE):m

clean:
	$(RM) *.o *.elf *.hex
