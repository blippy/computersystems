# Set the top level directory, which is "here".
# Uses $$ instead to $ to escape it from make
TOP := $(shell perl -MCwd -e 'my ($$m) = cwd() =~ /(^.*m01-console).*/ ; print "$$m"')

CC	= arm-none-eabi-gcc
CPP	= arm-none-eabi-gcc
AR	= arm-none-eabi-ar
AS	= arm-none-eabi-as
LINK	= arm-none-eabi-ld
PLINK	= arm-none-eabi-objcopy

EXTRAS = -DRPI=3 -ffreestanding
CFLAGS = -Wall -ggdb -O0 $(EXTRAS)
