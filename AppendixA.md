# Appendix A

The SiPeed/Lichee UART/JTAG adapter is a user friendly (labeled pins,
probe accessible, etc.), compact and low cost USB to UART/JTAG adapter.
In my experience it was packaged with quality female to female
GPIO wires ideal for use with debugging the RPi (ie. short).
This USB device is based on the FTDI FT2232D part and allows
for debugging a remote RPi target systems utilizing OpenOCD and GDB.

To turn a Windows PC into a bare metal RPi development PC requires
this adapter, or a similar one, and a cross compiler for development.
This guide describes how to build, execute (TTL/UART) and debug
(JTAG) the RPi from a Windows PC, utilizing MSYS2, the arm-none-eabi
tool chain and OpenOCD.

An Ubuntu PC version of this guide is much simpler as everything
should work out of the box with these packages installed.

```bash
sudo apt install gcc-arm-none-eabi
sudo apt install gdb-multiarch
sudo apt install openocd
```

When debugging on Ubuntu use gdb-multiarch instead of
arm-none-eabi-gdb. For example:

```bash
gdb-multiarch bootloader.elf
```

## Alternate FT2232H based hardware

USB to JTAG/TTL devices are discontinued frequently, and the
SiPeed/Lichee is no exception. If you cannot get this user friendly
part available in your area, there are alternatives. For beginners
the SiPeed/Lichee is highly recommended due to the easy of wiring.

The CJMCU-2232 FT2232HL USB to UART/JTAG is also a viable hardware
option, although it is much less user friendly. The header pins
typically need soldering and the pin labeling is generic and requires
a lookup table to translate the FT2232 style (AC/AC and BC/BD) into
JTAG and UART pins. In general use the A connectors for JTAG and
B connectors for UART and the rest of the instructions are the same.
A pin map from FT2232 to UART/JTAG is available online through other
sources.

The FT2232H Mini Module is also a good option and very similar to the
CJMCU-2232 . There header is already pinned out but it requires a USB
cable, a helpful option if short GPIO wires for JTAG are not compatible
with your work area (laptop, etc.).

The only difference relevant in our case is the location of the the
UART pins. Below is a translation from FTDI connector pin to name.
This reference can be used to wire either the CJMCU-2232 or FT2232H
Mini Module.

```bash
          Pin   | Name    | RPI Pin  | RPi GPIO
----------------|---------|----------|----------
JTAG:
  GND:    CN2-2   GND       14         GND
  TCK:    CN2-7   AD0       22         25
  TDI:    CN2-10  AD1       37         26
  TDO:    CN2-9   AD2       18         24
  TMS:    CN2-12  AD3       13         27
UART:
  GND:    CN3-2   GND       6          GND
  TX->RX: CN3-26  BD0       10         15
  RX->TX: CN3-25  BD1       8          14

Raspi to Raspi:
      RPi Pin   | RPi GPIO  RPI Pin  | RPi GPIO
----------------|---------|----------|----------
  TRST:   1       3V3       15         22

FTDI board to FTDI board:
      FT Pin   | FT name | FT Pin   | FT name
---------------|---------|----------|----------
  VCCIO: CN2-1   V3V3       CN2-11      VIO
  VCC:   CN3-1   VBUS       CN3-3       VCC
```

References:

[DS_FT2232H_Mini_Module.pdf](https://www.ftdichip.com/Support/Documents/DataSheets/Modules/DS_FT2232H_Mini_Module.pdf)

[DWelch67 ARM JTAG](https://github.com/dwelch67/raspberrypi/tree/master/armjtag)

### Install MSYS2 and launch the "MSYS2 MinGW 32-bit" shell.

[https://www.msys2.org/](https://www.msys2.org/)

Next we install 'make'. From the launched shell, issue the following
to install make:

```bash
pacman -S make
```

### Install the latest arm-none-eabi toolchain for Windows.

[https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads)

### Add path to c:/msys64/etc/profile

Add the directory to the toolchain from #2 to MSYS path.

```bash
MSYS2_PATH="/usr/local/bin:/usr/bin:/bin:/c/Program Files (x86)/GNU Arm Embedded Toolchain/9 2020-q2-update/bin"
```

### Install OpenOCD

```bash
pacman -S mingw-w64-i686-openocd
```

Unfortunately this version of OpenOCD is too dated to support the RPi
hardware. Proceed to the next step to upgrade the OpenOCD.

### Upgrade OpenOCD

To avoid building OpenOCD from scratch, download the latest
xpack-openocd (xpack-openocd-0.10.0-14-win32-x32.zip)

[https://github.com/xpack-dev-tools/openocd-xpack/releases/](https://github.com/xpack-dev-tools/openocd-xpack/releases/)

Then unzip and copy the 'contrib', 'OpenULINK' and 'scripts' folders to
c:/msys64/mingw32/share/openocd. Replace/Update any existing files.

Then copy contents of 'bin' folder to c:/msys64/mingw32/bin, again
replacing any existing files. Alternatively to installing and upgrading
OpenOCD you can build the latest from Git.

### Connect all UART and JTAG pins from SiPeed to the RPI.

The UART Rx and Tx connect to the opposite side (Rx to Tx, Tx to Rx).
All other pins are direct connect. Do not forget GND pins. See
Chapter 4 for pin numbers and GPIO's. The pins on the SiPeed are
clearly labled to avoid errors.

WARNING - power off the RPi and disconnect the USB adapter before
connecting the GPIO wires. Double check that each GPIO connection is
correct and snug before powering on. A loose wire is all it takes for
OpenOCD to fail spectacularly.

### Connect the USB SiPeed adapter to the Windows PC.

This should produce two new devices within the Device Manager,
USB Serial Converter A and B.

### Change the USB driver

The USB Serial Converter A must be reconfigured to use the WinUSB
driver in order for OpenOCD to connect and control the JTAG correctly.
By default the FT2232 based SiPeed is configured by Windows to be two
COM ports. However, it is required to replace the driver for the
primary UART (A) with WinUSB before OpenOCD can use it for JTAG.

To do this, download, install and execute the UsbDriverTool:

[https://visualgdb.com/UsbDriverTool/](https://visualgdb.com/UsbDriverTool/)

Upon execution, the UsbDriverTool Gui will appear, with a list of
current USB devices on your system. Right click the USB Serial
Converter A and choose 'Install WinUSB'.

### Execute OpenOCD

Now it is time to test OpenOCD. For example, from the directory
Lab8 Operating System\applications\bootloader of this Git
repository, the following command will initialize OpenOCD for the
SiPeed (FT2232) adapter and the connected RPi 4 target:

```bash
openocd -f ../../boards/rpi/openocd_sipeed_jtag.cfg -f ../../boards/rpi/rpi4_jtag.cfg
```

This openocd_sipeed_jtag.cfg is nothing more than a copy of the interface
configuration file openocd-usb.cfg released by OpenOCD.

openocd/scripts/interface/ftdi/openocd-usb.cfg

With OpenOCD, the first configuration file is for the adapter and the
second file is for the target board. See Chapter 4 of the Laboratory
book for more details on how to connect GDB to OpenOCD and debug the
remote RPi target. This chapter (4) is included in the free preview of
the book (link below).


### UART
 
Search the Windows Device Manager for the USB Serial Converter B and
note the COM port number. You will use this COM port number when
connecting with a Windows terminal software, such as TeraTerm or PuTTY.

Install either Tera Term or Putty now. If you want simplicity and only
intend to use with the RPi, choose Tera Term. For more features and
multiple configurations/target boards, choose Putty. The links
below are to the download page on lo4d.com.

[https://tera-term.en.lo4d.com/windows](https://tera-term.en.lo4d.com/windows)

[https://putty.en.lo4d.com/windows](https://putty.en.lo4d.com/windows)

Once installed, configure the terminal application to connect with the
COM port assigned by Windows to USB Serial Converter B.

In Tera Term, click the Setup menu and select Serial port... Then
select the COM port from the Port: drop down list. Keep the defaults
of 115200 baud 8N1 and press Ok. Once you have verified the UART
connection is a success choose Setup menu and Save settup... Save the
file as the default TERATERM.INI and this COM port will be opened
by default every time Tera Term is launched.

For PuTTY, the opening PuTTY Configuration has a Serial radio button
on the top right side, in the Connection type: section. Select Serial
and then enter the COM port (COM5 for example) in the Serial line
field, and 115200 in the Speed. Press the Open button at the bottom
to connect to the RPi over the COM port. If success you can use
the Save button to save your configuration for quick loading next
time you launch PuTTY; double click the saved name in the list to
connect.

### Happy coding and debugging!

First half of the Laboratory. Click Read Free Sample and choose
your preferred reading format (PDF recommended for PC reading).
[https://leanpub.com/computersystems_lab_rpi](https://leanpub.com/computersystems_lab_rpi)

Computer Systems book preview. Click Read Free Sample and choose
your preferred reading format (PDF recommended for PC reading).
[https://leanpub.com/computersystems](https://leanpub.com/computersystems)

