KERN = kernel7
-include ../../Settings.mk
##
## Commands:
##
CP	= cp
RM	= rm
LN	= ln
#C	= arm-none-eabi-gcc
#CC	= arm-none-eabi-gcc
#CPP	= arm-none-eabi-gcc
#AR	= arm-none-eabi-ar
#AS	= arm-none-eabi-as
#LINK	= arm-none-eabi-ld
#PLINK	= arm-none-eabi-objcopy

##
## Definitions:
##
APPNAME = $(KERN)

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
INCLUDES = -I. -I../../include -I../../boards/rpi \
           -I../../include/network -I../../include/network/lwip \
           -I../../include/network/ipv4 -I../../boards/rpi \
           -I../../boards/peripherals

#INCLUDES += -I../../fatfs -I../..

##
## Console application
##

# Use app.o for normal application, vectors.o for bootloader
#APP     = ../../boards/rpi/app.o \
#APP     = ../../boards/rpi/loader.o \

OBJS     = ../../boards/rpi/app.o \
          ../../boards/rpi/board.o \
          ../../boards/rpi/framebuffer.o \
          ../../boards/rpi/property.o \
          ../../boards/peripherals/dwc/host.o \
          ../../boards/peripherals/dwc/transfer.o \
          ../../boards/peripherals/ethernet/lan95xx.o \
          ../../boards/peripherals/ethernet/lan78xx.o \
          ../../boards/peripherals/uart/16550.o \
          ../../boards/peripherals/uart/16C650.o \
          ../../system/character.o \
          ../../system/console.o \
	  ../../system/delays.o \
          ../../system/os.o \
          ../../system/malloc.o \
          ../../system/assert.o \
          ../../system/printf.o \
          ../../system/rand.o \
          ../../system/screen.o \
          ../../system/string.o \
          ../../system/stdio.o \
          ../../system/shell.o \
          ../../system/timers.o \
          ../../system/xmodem.o \
          ../../usb/device.o \
          ../../usb/endpoint.o \
          ../../usb/hub.o \
          ../../usb/keyboard.o \
          ../../usb/mouse.o \
          ../../usb/request.o \
          ../../usb/rootport.o \
          ../../network/core/ipv4/autoip.o \
          ../../network/core/ipv4/icmp.o \
          ../../network/core/ipv4/igmp.o \
          ../../network/core/ipv4/inet.o \
          ../../network/core/ipv4/inet_chksum.o \
          ../../network/core/ipv4/ip.o \
          ../../network/core/ipv4/ip_addr.o \
          ../../network/core/ipv4/ip_frag.o \
          ../../network/core/def.o \
          ../../network/core/dhcp.o \
          ../../network/core/dns.o \
          ../../network/core/init.o \
          ../../network/core/mem.o \
          ../../network/core/memp.o \
          ../../network/core/netif.o \
          ../../network/core/pbuf.o \
          ../../network/core/raw.o \
          ../../network/core/stats.o \
          ../../network/core/sys.o \
          ../../network/core/tftp_clnt.o \
          ../../network/core/tcp.o \
          ../../network/core/tcp_in.o \
          ../../network/core/tcp_out.o \
          ../../network/core/timers.o \
          ../../network/core/udp.o \
          ../../network/netif/etharp.o \
          ../../network/netif/ethernetif.o \
          main.o

#          denzi_data.o \
#          game_grid.o \
#          virtual_world.o \

OBJS += ../../system/setjmp.o
OBJS += sd.o
OBJS += sd-test.o

#APP += ../../fatfs/diskio.o ../../fatfs/ff.o 

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

all:	app

app:	$(OBJS)
	$(LINK) -Map $(KERN).map -T ../../boards/rpi/memory.map -o $(KERN).elf $(OBJS) $(LIBS)
	$(PLINK) $(KERN).elf -O binary $(KERN).bin
	$(CP) $(KERN).bin $(KERN).img
	echo "TOP is " $(TOP)

-include ../../Blippy.mk
