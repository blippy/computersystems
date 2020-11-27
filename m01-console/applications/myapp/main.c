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
#include <stdbool.h>
#include <usb/request.h>
#include <usb/device.h>

//#include <fatfs/ff.h>

#include <yasetjmp.h>

void sd_test();
extern int GameStart(const char *command);

extern int OgSp;

jmp_buf jump_buffer;


int ScreenUp, GameUp, UsbUp, NetUp;

//#include <setjmp.h>

#if ENABLE_USB_HID

static char KeyIn;

unsigned int usbKbdCheck(void)
{
  return KeyIn;
}

char usbKbdGetc(void)
{
  char key = KeyIn;
  putchar(key);

  KeyIn = 0;
  return key;
}

void KeyPressedHandler(const char ascii)
{
  KeyIn = ascii;
  putchar(KeyIn);
}

void custom_flush()
{
}

#endif /* ENABLE_USB_HID */

#if ENABLE_USB
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

#endif /* ENABLE_USB */


_Noreturn void a(int count)
{
	printf("a(%d) called\n", count);
	longjmp(jump_buffer, count+1);
}

int myinit(const char *command)
{
  if (ScreenUp)
  {
    StdioState = &ConsoleState;
    ScreenClear();

    puts("\nAdventure awaits! 2");
    volatile int count = 0;
    if(setjmp(jump_buffer) != 5) {
	    a(++count);
    }
  }
  else
    puts("Video screen not initialized");

  return TASK_FINISHED;
}

jmp_buf jump_buffer;

#if 0
bool f_exists(const char* path)
{
	FIL file;
	FRESULT res = f_open(&file, path, FA_READ  | FA_OPEN_EXISTING);
	if(res != FR_OK) return 0;

	f_close(&file);
	return 1;
}
#endif

extern int KeyboardUp(const char *command);

int letsgo_task(const char *command)
{
	// wait until keyboard is enabled
	extern int KeyboardEnabled;
	if(!KeyboardEnabled) 
		return TASK_IDLE;

	puts("I think we're finally ready to go. Type something, and I will echo");
	return TASK_FINISHED;
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

  bzero(&ConsoleState, sizeof(ConsoleState));
  Console(&ConsoleState);
  ConsoleState.flush = custom_flush;
  ConsoleState.getc = usbKbdGetc;
  ConsoleState.check = usbKbdCheck;
  StdioState = &ConsoleState;

  /* Initialize the base software interface to the hardware. */
  BoardInit();

#if ENABLE_OS
  // Initialize the Operating System (OS) and create system tasks
  OsInit();


#if 0
#if ENABLE_UART0
  StdioState = &Uart0State;
  TaskNew(0, ShellPoll, &Uart0State);
#elif ENABLE_UART1
  StdioState = &Uart1State;
  TaskNew(0, ShellPoll, &Uart1State);
#endif
#endif

  //KeyboardUp(NULL); // doesn't seem to help

  //ConsoleEnableKeyboard();  // seems to actually cause a crash
  // Initialize the timer and LED tasks
  TaskNew(1, TimerPoll, &TimerStart);
  TaskNew(MAX_TASKS - 1, LedPoll, &LedState);
#endif /* ENABLE_OS */

  void* mem = malloc(10); // just a test - seems to work, too
  *(char*)  mem = 0;

  // Initialize user devices

#if ENABLE_AUTO_START
#if ENABLE_VIDEO
  /* Initialize screen and console. */
  ScreenInit();
  ScreenUp = TRUE;

#endif
#if ENABLE_USB
  UsbHostStart(NULL);
#endif
#endif

  /* display the introductory splash */
  puts("Game application");
  putu32(OgSp);
  puts(" : stack pointer");

  //GameStart(0);
  myinit(0);

  if(0) sd_test(); // surpess for now
  //TaskNew(0, KeyboardUp, 0);
  puts("Guess we're about ready to go 1");


  TaskNew(100, letsgo_task, 0);
#if ENABLE_OS
  /* run the non-interruptive priority loop scheduler */
  OsStart();
#elif ENABLE_SHELL
  SystemShell();
#else
  puts("Press any key to exit application");
  uart0_getc();
#endif

  /* on OS exit say goodbye (never gets here...)*/
  puts("Goodbye");
  return 0;
}
