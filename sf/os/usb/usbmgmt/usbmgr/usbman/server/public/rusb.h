/*
* Copyright (c) 2003-2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
* Definitions required for RUsb
*
*/

/**
 @file
 @internalComponent
*/

#ifndef __RUSB_H__
#define __RUSB_H__

#include <e32std.h>
#include <usb/usbshared.h>

const TInt KUsbSrvMajorVersionNumber = 1;
const TInt KUsbSrvMinorVersionNumber = 1;
const TInt KUsbSrvBuildVersionNumber = 0;

const TInt KUsbMaxSupportedClasses = 64;		// Max number of supported USB classes by a personality

const TInt KUsbMaxSupportedPersonalities = 64;	// Max number of supported personalities

#ifdef __OVER_DUMMYUSBDI__ 
_LIT(KUsbServerName, "!usbman_over_dummyusbdi");
#else
#ifdef  __DUMMY_LDD__
_LIT(KUsbServerName, "!usbman_over_dummyldd");
#else
_LIT(KUsbServerName, "!usbman");
#endif
#endif

enum TUsbMessages
	{
	EUsbStart,
	EUsbStop,
	EUsbRegisterObserver,
	EUsbGetCurrentState,
	EUsbStartCancel,
	EUsbCancelObserver,
	EUsbStopCancel,
	EUsbGetCurrentDeviceState,
	EUsbRegisterServiceObserver,
	EUsbCancelServiceObserver,
	EUsbDbgMarkHeap,
	EUsbDbgCheckHeap,
	EUsbDbgMarkEnd,
	EUsbDbgFailNext,
	EUsbTryStart,
	EUsbTryStop,
	EUsbCancelInterest,
	EUsbGetCurrentPersonalityId,
	EUsbGetSupportedClasses,
	EUsbGetPersonalityIds,
	EUsbGetDescription,
	EUsbClassSupported,

	
	EUsbSetCtlSessionMode,
	EUsbBusRequest,
	EUsbBusRespondSrp,
	EUsbBusClearError,

	EUsbBusDrop,
	EUsbRegisterMessageObserver,
	EUsbCancelMessageObserver,
	EUsbRegisterHostObserver,
	EUsbCancelHostObserver,
	EUsbEnableFunctionDriverLoading,
	EUsbDisableFunctionDriverLoading,
	EUsbGetSupportedLanguages,
	EUsbGetManufacturerStringDescriptor,
	EUsbGetProductStringDescriptor,
	EUsbGetOtgDescriptor,
	EUsbDbgAlloc,
	EUsbRequestSession,
	EUsbGetPersonalityProperty
	};

_LIT(KUsbCliPncCat, "UsbMan-Client");

enum TUsbPanicClient
	{
	EUsbCreateFailure,
	EUsbPanicIllegalIPC,
	EUsbPanicRemovedExport
	};

const TUid KUsbmanSvrUid = {0x101fe1db};

#ifdef __USBMAN_NO_PROCESSES__

const TUint KUsbmanStackSize = 0x3000;			//  12KB
const TUint KUsbmanMinHeapSize = 0x1000;		//   4KB
const TUint KUsbmanMaxHeapSize = 0x40000;		// 256KB

_LIT(KUsbmanImg, "usbsvr");

#else

#ifdef __OVER_DUMMYUSBDI__ 
_LIT(KUsbmanImg, "z:\\system\\programs\\usbsvr_over_dummyusbdi.exe");
#else
#ifdef  __DUMMY_LDD__
_LIT(KUsbmanImg, "z:\\system\\programs\\usbsvr_over_dummyldd.exe");
#else
_LIT(KUsbmanImg, "z:\\system\\programs\\usbsvr.exe");
#endif
#endif


#endif //__USBMAN_NO_PROCESSES__

#endif //__RUSB_H__
