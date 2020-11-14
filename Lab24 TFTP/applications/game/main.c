/*...................................................................*/
/*                                                                   */
/*   Module:  main.c                                                 */
/*   Version: 2019.0                                                 */
/*   Purpose: main function for game application                     */
/*                                                                   */
/*...................................................................*/
/*                                                                   */
/*                   Copyright 2019, Sean Lawless                    */
/*                                                                   */
/*                      ALL RIGHTS RESERVED                          */
/*                                                                   */
/* Redistribution and use in source, binary or derived forms, with   */
/* or without modification, are permitted provided that the          */
/* following conditions are met:                                     */
/*                                                                   */
/*  1. Redistributions in any form, including but not limited to     */
/*     source code, binary, or derived works, must include the above */
/*     copyright notice, this list of conditions and the following   */
/*     disclaimer.                                                   */
/*                                                                   */
/*  2. Any change or addition to this copyright notice requires the  */
/*     prior written permission of the above copyright holder.       */
/*                                                                   */
/* THIS SOFTWARE IS PROVIDED ''AS IS''. ANY EXPRESS OR IMPLIED       */
/* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES */
/* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE       */
/* DISCLAIMED. IN NO EVENT SHALL ANY AUTHOR AND/OR COPYRIGHT HOLDER  */
/* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,          */
/* EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED   */
/* TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,     */
/* DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON */
/* ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,   */
/* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY    */
/* OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE           */
/* POSSIBILITY OF SUCH DAMAGE.                                       */
/*...................................................................*/
#include <system.h>
#include <board.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <usb/request.h>
#include <usb/device.h>

#include <lfb.h>

extern int OgSp;

int ScreenUp, GameUp, UsbUp, NetUp;

static char KeyIn;

unsigned int usbKbdCheck(void)
{
	return KeyIn;
}

char usbKbdGetc(void)
{
	char key = KeyIn;
	KeyIn = 0;
	return key;
}


// seems to work!
void KeyPressedHandler(const char ascii)
{
	putchar(ascii);
	putchar(' ' );
	//putchar(ascii);
	putbyte(ascii);
	puts("");
	KeyIn = ascii;

}


int UsbHostStart(char *command)
{
	if (!UsbUp)
	{
		RequestInit();
		DeviceInit();

		if (!HostEnable())
		{
			puts("Cannot initialize USB host controller interface");
			return TASK_FINISHED;
		}
		UsbUp = TRUE;
	}
	else
		puts("USB host already initialized");

	return TASK_FINISHED;
}

void lfb_is_stdout()
{
	extern int (*_putchar)(char);
	_putchar = &fbputchar;
}

/*...................................................................*/
/*        main: Application Entry Point                              */
/*                                                                   */
/*     Returns: Exit error                                           */
/*...................................................................*/
int main(void)
{
	// Initialize hardware peripheral configuration
	NetUp = UsbUp = ScreenUp = FALSE;
	ScreenUp = TRUE;

	puts("this wont be printed because we havent set up stdout");
	/* Initialize the base software interface to the hardware. */
	BoardInit();

	lfb_init();
	lfb_is_stdout();
	//_putchar = &fbputchar;
	fbputs("test of fbputs 24");

	// Initialize the Operating System (OS) and create system tasks
	OsInit();

#if 1
#if ENABLE_UART0
	StdioState = &Uart0State;
	TaskNew(0, ShellPoll, &Uart0State);
#elif ENABLE_UART1
	StdioState = &Uart1State;
	TaskNew(0, ShellPoll, &Uart1State);
#endif
#endif


	// Initialize the timer and LED tasks
	TaskNew(1, TimerPoll, &TimerStart); // seems needed

	ScreenInit();
	puts("hello from game 2"); // ensure a console is initialised first
	UsbHostStart(NULL);

	puts("mcarter says hello 1");

	OsStart(); //in system/os.c

	puts("Goodbye"); // never reached
	return 0;
}
