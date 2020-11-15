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

int MyTask(void* data)
{
	return TASK_IDLE;
}

/*...................................................................*/
/*        main: Application Entry Point                              */
/*                                                                   */
/*     Returns: Exit error                                           */
/*...................................................................*/
int main(void)
{
	NetUp = UsbUp = ScreenUp = FALSE;
	ScreenUp = TRUE;

	puts("this wont be printed because we havent set up stdout");
	//BoardInit();

	lfb_init();
	lfb_is_stdout();
	fbputs("test of fbputs 28");

	OsInit();

	TaskNew(1, TimerPoll, &TimerStart); // seems needed
	TaskNew(1, MyTask, 0);

	ScreenInit();
	puts("hello from game 8"); // ensure a console is initialised first
	UsbHostStart(NULL);

	puts("mcarter says hello 1");
	puts("I will echo your input");

	OsStart(); //in system/os.c

#if 0
	while(1) {
		char c = usbKbdGetc();
		if(c) putchar(c);
	}
#endif

	puts("Goodbye"); // never reached
	while(1);
	return 0;
}
