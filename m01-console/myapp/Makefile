KERN = kernel7
IMG = $(KERN).img
BIN = $(KERN).bin
ELF = $(KERN).elf

##
## Commands:
##
CP	= cp
RM	= rm
LN	= ln
C	= arm-none-eabi-gcc
CC	= arm-none-eabi-gcc
CPP	= arm-none-eabi-gcc
AR	= arm-none-eabi-ar
AS	= arm-none-eabi-as
LINK	= arm-none-eabi-ld
PLINK	= arm-none-eabi-objcopy

##
## Definitions:
##
#APPNAME = game

# Define the RPI hardware version: 1 is up to B+, 2 is 2, 3 is up to 3 B+
EXTRAS = -DRPI=3 -ffreestanding
# Begin Historic/Depricated/Older GCC
#RPI 3 B+
#EXTRAS = -DRPI=3 -march=armv8-a+crc -mtune=cortex-a53 -ffreestanding
#RPI 2
#EXTRAS = -DRPI=2 -march=armv7-a -mtune=cortex-a7 -ffreestanding
#RPI B+
#EXTRAS = -DRPI=1 -mcpu=arm1176jzf-s -ffreestanding
# End Historic/Depricated/Older GCC

##Warnings about everything and optimize for size or speed (-Os or -O2)
#CFLAGS = -Wall -O2 $(EXTRAS)
##Warnings and debugging build, with GDB and no optimizations (-O0)
CFLAGS = -Wall -ggdb -O0 $(EXTRAS)

ASFLAGS =
##RPI 3
#ASFLAGS = -mfpu=neon-fp-armv8 -mfloat-abi=hard
##RPI 2
#ASFLAGS = -mfpu=neon-vfpv4 -mfloat-abi=hard
##RPI B+
#ASFLAGS = -mcpu=arm1176jzf-s

#DIR = 

VPATH = ../boards/rpi:../boards/peripherals/dwc:../boards/peripherals/uart:../system:../usb
#../boards/peripherals/ethernet
#../network/core:../network/core/ipv4:../network/netif

INCLUDES = -I. -I../include -I../boards/rpi \
           -I../boards/peripherals

#           -I../include/network -I../include/network/lwip \
#           -I../include/network/ipv4 \

##
## Console application
##

# Use app.o for normal application, vectors.o for bootloader
#APP     = ../../boards/rpi/app.o \
#APP     = ../../boards/rpi/loader.o \

NET_OBJS =  \
          lan95xx.o \
          lan78xx.o \
          autoip.o \
          icmp.o \
          igmp.o \
          inet.o \
          inet_chksum.o \
          ip.o \
          ip_addr.o \
          ip_frag.o \
          def.o \
          dhcp.o \
          dns.o \
          init.o \
          mem.o \
          memp.o \
          netif.o \
          pbuf.o \
          raw.o \
          stats.o \
          sys.o \
          tftp_clnt.o \
          tcp.o \
          tcp_in.o \
          tcp_out.o \
          timers.o \
          udp.o \
          etharp.o \
          ethernetif.o 

OBJS     = board.o \
	app.o \
          framebuffer.o \
          property.o \
          host.o \
          transfer.o \
          16550.o \
          16C650.o \
          character.o \
          console.o \
          os.o \
          malloc.o \
          assert.o \
          printf.o \
          rand.o \
          screen.o \
          string.o \
          stdio.o \
          shell.o \
          timers.o \
          xmodem.o \
          device.o \
          endpoint.o \
          hub.o \
          keyboard.o \
          mouse.o \
          request.o \
          rootport.o \
          denzi_data.o \
          game_grid.o \
          virtual_world.o \
          main.o

LIBS =

##
## Implicit Targets
##
.c.o:
	$(CC) -c $(CFLAGS) $(INCLUDES) -o $@ $<

.s.o:
	$(AS) $(ASFLAGS) $(INCLUDES) -o $@ $<

.cpp.o:
	$(CPP) -c $(CFLAGS) $(INCLUDES) -o $@ $<

##
## Targets
##

all:	$(IMG)

$(ELF) : $(OBJS)
	$(LINK) -Map $(KERN).map -T ../boards/rpi/memory.map -o $(ELF) $(OBJS) $(LIBS)

$(IMG):	$(ELF)
	$(PLINK) $^ -O binary $@

clean:
	rm -f *.bin *.elf *.img *.map *.o

-include ../../Blippy.mk
