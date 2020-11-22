/*...................................................................*/
/*                                                                   */
/*   Module:  flash.c                                                */
/*   Version: 2020.0                                                 */
/*   Purpose: USB Standard Flash Device driver                       */

/* 2020-11-22 mcarter so far just a copy of the mouse driver
*/

/*                                                                   */
/*...................................................................*/
/*                                                                   */
/*                   Copyright 2020, Sean Lawless                    */
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
/*                                                                   */
/* The FreeBSD Project, Copyright 1992-2020                          */
/*   Copyright 1992-2020 The FreeBSD Project.                        */
/*   Permission granted under MIT license (Public Domain).           */
/*                                                                   */
/* Embedded Xinu, Copyright (C) 2013.                                */
/*   Permission granted under Creative Commons Zero (Public Domain). */
/*                                                                   */
/* Alex Chadwick (CSUD), Copyright (C) 2012.                         */
/*   Permission granted under MIT license (Public Domain).           */
/*                                                                   */
/* Thanks to Linux/Circle/uspi, and Rene Stange specifically, for    */
/* the quality reference model and runtime register debug output.    */
/*                                                                   */
/* Additional thanks to the Raspberry Pi bare metal forum community. */
/* https://www.raspberrypi.org/forums/viewforum.php?f=72             */
/*...................................................................*/
#include <board.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <usb/hid.h>
#include <usb/request.h>
#include <usb/device.h>

#if ENABLE_USB_HID

/*...................................................................*/
/* Symbol Definitions                                                */
/*...................................................................*/
#define MOUSE_REPORT_SIZE  3

/*...................................................................*/
/* Type Definitions                                                  */
/*...................................................................*/
typedef void FlashStatusHandler(u32 buttons, int displacementX,
		int displacementY);

typedef struct FlashDevice
{
	Device device; // Must be first element in this structure

	u8 interfaceNumber;
	u8 alternateSetting;

	Endpoint *reportEndpoint;
	Endpoint *interruptEndpoint;
	Endpoint ReportEndpoint;
	Endpoint InterruptEndpoint;

	FlashStatusHandler *statusHandler;

	Request *urb;
	u8 reportBuffer[MOUSE_REPORT_SIZE];
}
FlashDevice;

/*...................................................................*/
/* Global Variables                                                  */
/*...................................................................*/
FlashDevice Flash;
FlashDevice *Flash1;
int FlashInitState = 0, FlashEnabled = 0;

extern int FlashUp(const char *command);

/*...................................................................*/
/* Static Local Functions                                            */
/*...................................................................*/

/*...................................................................*/
/* status_update: Flash status change update callback                */
/*                                                                   */
/*      Inputs: buttons is the change in buttons                     */
/*              displacementY is the flash change in Y direction     */
/*              displacementX is the flash change in X direction     */
/*...................................................................*/
static void status_update(u32 buttons, int displacementX,
		int displacementY)
{
#if ENABLE_PRINTF
	printf("(%db%dx%dy)", buttons, displacementX, displacementY);
#else
	Uart0State.putc('.');
#endif
}

/*...................................................................*/
/* configure_complete: State machine to configure a flash            */
/*                                                                   */
/*      Inputs: urb is USB Request Buffer (URB)                      */
/*              param is the flash device                            */
/*              context is unused                                    */
/*...................................................................*/
static void configure_complete(void *urb, void *param, void *pContext)
{
	FlashDevice *flash = (FlashDevice *)param;

	//always free the last URB before sending a new one
	if (urb)
		FreeRequest(urb);

	if ((flash->alternateSetting != 0) && (FlashInitState == 0))
	{
		puts("Flash alternate settings is not zero!");
		if (HostEndpointControlMessage (flash->device.host,
					flash->device.endpoint0,
					REQUEST_OUT | REQUEST_TO_INTERFACE, SET_INTERFACE,
					flash->alternateSetting,
					flash->interfaceNumber, 0, 0, configure_complete, flash) < 0)
		{
			puts("Cannot set interface");

			return;
		}
		FlashInitState = 1;
	}
	else if (FlashInitState == 2)
	{
		Flash1 = flash;
		FlashInitState = 0; //reset upon success
		if (flash->device.complete)
		{
			flash->device.complete(NULL, flash->device.comp_dev, NULL);
			flash->device.complete = NULL;
		}

#if ENABLE_AUTO_START
		// Automatically start the flash for the user if configured
		FlashUp(NULL);
#endif
	}
	else
	{
		if (HostEndpointControlMessage (flash->device.host,
					flash->device.endpoint0, REQUEST_OUT | REQUEST_CLASS |
					REQUEST_TO_INTERFACE, SET_PROTOCOL, BOOT_PROTOCOL,
					flash->interfaceNumber, 0, 0, configure_complete, flash) < 0)
		{
			puts("Cannot set boot protocol");

			// Reset flash configuraiton state upon failure
			FlashInitState = 0;
			return;
		}

		// Set flash to final configuration state
		FlashInitState = 2;
	}
}

/*...................................................................*/
/*   configure: Start flash device configuration                     */
/*                                                                   */
/*      Inputs: device is keyboard device                            */
/*              complete is callback function invoked upon completion*/
/*              param is unused                                      */
/*...................................................................*/
static int configure(Device *device, void (complete)
		(void *urb, void *param, void *context), void *param)
{
	FlashDevice *flash = (FlashDevice *)device;
	InterfaceDescriptor *interfaceDesc;
	ConfigurationDescriptor *pConfDesc;
	EndpointDescriptor *endpointDesc;

	assert (flash != 0);
	pConfDesc = (ConfigurationDescriptor *)DeviceGetDescriptor(
			flash->device.configParser, DESCRIPTOR_CONFIGURATION);

	if ((pConfDesc == 0) || (pConfDesc->numInterfaces <  1))
	{
		puts("USB flash configuration descriptor not valid");
		return FALSE;
	}

	while ((interfaceDesc = (InterfaceDescriptor *)
				DeviceGetDescriptor(flash->device.configParser,
					DESCRIPTOR_INTERFACE)) != 0)
	{
		if ((interfaceDesc->numEndpoints <  1) ||
				(interfaceDesc->interfaceClass != 0x03) || // HID Class
				(interfaceDesc->interfaceSubClass != 0x01) || // Boot Subclass
				(interfaceDesc->interfaceProtocol != 0x02))   // Flash
		{
			continue;
		}

		flash->interfaceNumber  = interfaceDesc->interfaceNumber;
		flash->alternateSetting = interfaceDesc->alternateSetting;

		endpointDesc = (EndpointDescriptor *)DeviceGetDescriptor(
				flash->device.configParser, DESCRIPTOR_ENDPOINT);
		// Skip if empty of input endpoint
		if ((endpointDesc == 0) ||
				(endpointDesc->endpointAddress & 0x80) != 0x80)
			continue;

		// Assign interrupt endpoint if attribute allows
		if ((endpointDesc->attributes & 0x3F) == 0x03)
		{
			assert (flash->interruptEndpoint == 0);
			flash->interruptEndpoint = &(flash->InterruptEndpoint);
			assert (flash->interruptEndpoint != 0);
			EndpointFromDescr(flash->interruptEndpoint, &flash->device,
					endpointDesc);
		}

		// Otherwise use the report endpoint
		else
		{
			assert (flash->reportEndpoint == 0);
			flash->reportEndpoint = &(flash->ReportEndpoint);
			assert (flash->reportEndpoint != 0);
			EndpointFromDescr(flash->reportEndpoint, &flash->device,
					endpointDesc);
		}

		continue;
	}

	if ((flash->reportEndpoint == 0) && (flash->interruptEndpoint == 0))
	{
		puts("USB flash endpoint not found");
		return FALSE;
	}

	if (!DeviceConfigure (&flash->device, configure_complete, flash))
	{
		puts("Cannot set configuration");
		return FALSE;
	}

	return TRUE;
}

/*...................................................................*/
/*  completion: Flash URB completion parses flash events             */
/*                                                                   */
/*       Input: urb is the completed USB Request Buffer (URB)        */
/*              param is the flash device                            */
/*              context is unused                                    */
/*...................................................................*/
static void completion(void *request, void *param, void *context)
{
	Request *urb = request;
	FlashDevice *flash = (FlashDevice *)context;

	assert(flash != 0);
	assert(urb != 0);
	assert (flash->urb == urb);

	if ((urb->status != 0) && (urb->resultLen == MOUSE_REPORT_SIZE) &&
			(flash->statusHandler != 0))
	{
		assert (flash->reportBuffer != 0);

		// Call report handler with x and y flash movement offsets
		(*flash->statusHandler) (flash->reportBuffer[0],
				(flash->reportBuffer[1] > 0x7F) ?
				((int)flash->reportBuffer[1] | -0x100) :
				(int)flash->reportBuffer[1],
				(flash->reportBuffer[2] > 0x7F) ?
				((int)flash->reportBuffer[2] | -0x100) :
				(int)flash->reportBuffer[2]);
	}

	// Reuse the URB
	RequestRelease(flash->urb);

	/* Attach URB to the device, preferring the interrupt endpoint. */
	if (flash->interruptEndpoint)
		RequestAttach(flash->urb, flash->interruptEndpoint,
				flash->reportBuffer, MOUSE_REPORT_SIZE, 0);
	else if (flash->reportEndpoint)
		RequestAttach(flash->urb, flash->reportEndpoint,
				flash->reportBuffer, MOUSE_REPORT_SIZE, 0);
	else
		assert(FALSE);
	RequestSetCompletionRoutine(flash->urb, completion, 0, flash);

	// Submit the request URB
	HostSubmitAsyncRequest(flash->urb, flash->device.host, NULL);
}

/*...................................................................*/
/* start_request: Initiate URB for flash events                      */
/*                                                                   */
/*       Input: flash is the flash device                            */
/*                                                                   */
/*     Returns: zero (0) one success, FALSE otherwise                */
/*...................................................................*/
static int start_request(FlashDevice *flash)
{
	assert(flash != 0);
	assert(flash->reportBuffer != 0);

	assert(flash->urb == 0);
	flash->urb = NewRequest();
	assert(flash->urb != 0);
	bzero(flash->urb, sizeof(Request));

	/* Attach URB to the device, preferring the interrupt endpoint. */
	if (flash->interruptEndpoint)
		RequestAttach(flash->urb, flash->interruptEndpoint,
				flash->reportBuffer, MOUSE_REPORT_SIZE, 0);
	else if (flash->reportEndpoint)
		RequestAttach(flash->urb, flash->reportEndpoint,
				flash->reportBuffer, MOUSE_REPORT_SIZE, 0);
	else
		return FALSE;
	RequestSetCompletionRoutine(flash->urb, completion, 0, flash);

	// Submit the request URB
	HostSubmitAsyncRequest(flash->urb, flash->device.host, NULL);
	return TRUE;
}

extern int UsbUp;

/*...................................................................*/
/* Global Functions                                                  */
/*...................................................................*/

/*...................................................................*/
/* FlashAttach: Attach a device to a flash device                    */
/*                                                                   */
/*       Input: device is the generic USB device                     */
/*                                                                   */
/*     Returns: void pointer to the new flash device                 */
/*...................................................................*/
void *FlashAttach(Device *device)
{
	FlashDevice *flash = &Flash;
	assert(flash != 0);

	FlashInitState = 0;
	FlashEnabled = 0;

	DeviceCopy(&flash->device, device);
	flash->device.configure = configure;

	flash->reportEndpoint = 0;
	flash->interruptEndpoint = 0;
	flash->statusHandler = 0;
	flash->urb = 0;
	assert(flash->reportBuffer != 0);

	return flash;
}

/*...................................................................*/
/* FlashRelease: Release a flash device                              */
/*                                                                   */
/*       Input: device is the USB flash device                       */
/*...................................................................*/
void FlashRelease(Device *device)
{
	FlashDevice *flash = (void *)device;
	assert (flash != 0);

	if (flash->reportEndpoint != 0)
	{
		EndpointRelease(flash->reportEndpoint);
		flash->reportEndpoint = 0;
	}

	if (flash->interruptEndpoint != 0)
	{
		EndpointRelease(flash->interruptEndpoint);
		flash->interruptEndpoint = 0;
	}

	DeviceRelease(&flash->device);
}

/*...................................................................*/
/*     FlashUp: Activate a discovered and configured USB flash       */
/*                                                                   */
/*       Input: command is unused                                    */
/*                                                                   */
/*     Returns: TASK_FINISHED as it is a shell command               */
/*...................................................................*/
int FlashUp(const char *command)
{
	if (!UsbUp)
		puts("USB not initialized");

	else if (!FlashEnabled)
	{
		FlashEnabled = TRUE;

		Flash1->statusHandler = status_update;
		start_request(Flash1);
	}
	else
		puts("USB not up or USB flash already initialized");

	return TASK_FINISHED;
}

#endif /* ENABLE_USB_HID */
