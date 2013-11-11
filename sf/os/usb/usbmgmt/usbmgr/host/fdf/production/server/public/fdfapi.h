/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/

/**
 @file
 @internalComponent
*/

#ifndef FDFAPI_H
#define FDFAPI_H

#include <e32base.h>

#ifdef __OVER_DUMMYUSBDI__
const TUint32 KUsbFdfTUint = 0x10282B50;
_LIT(KUsbFdfImg, "fdf_over_dummyusbdi.exe");
_LIT(KUsbFdfServerName, "!FdfSrv_over_dummyusbdi");
#else
const TUint32 KUsbFdfTUint = 0x10282B48;
_LIT(KUsbFdfImg, "fdf.exe");
_LIT(KUsbFdfServerName, "!FdfSrv");
#endif

const TUid KUsbFdfUid = {KUsbFdfTUint};

/** Version numbers. */
const TInt8 KUsbFdfSrvMajorVersionNumber = 1;
const TInt8 KUsbFdfSrvMinorVersionNumber = 1;
const TInt16 KUsbFdfSrvBuildNumber = 0;

/** IPC messages supported by the server. */
enum
	{
	EUsbFdfSrvEnableDriverLoading										= 0,
	EUsbFdfSrvDisableDriverLoading										= 1,
	EUsbFdfSrvNotifyDeviceEvent											= 2,
	EUsbFdfSrvNotifyDeviceEventCancel									= 3,
	EUsbFdfSrvNotifyDevmonEvent											= 4,
	EUsbFdfSrvNotifyDevmonEventCancel									= 5,
	EUsbFdfSrvGetSingleSupportedLanguageOrNumberOfSupportedLanguages	= 6,
	EUsbFdfSrvGetSupportedLanguages										= 7,
	EUsbFdfSrvGetManufacturerStringDescriptor							= 8,
	EUsbFdfSrvGetProductStringDescriptor								= 9,
	EUsbFdfSrvGetOtgDescriptor                                          = 10,
	EUsbFdfSrvGetConfigurationDescriptor								= 11,
	EUsbFdfSrvDbgFailNext												= 12,
	EUsbFdfSrvDbgAlloc													= 13,
	};

/** Panic codes which which the server panics an offending client. */
enum
	{
	EBadIpc									= 0,
	ENotifyDeviceEventAlreadyOutstanding	= 1,
	ENotifyDevmonEventAlreadyOutstanding	= 2,
	EBadNotifyDeviceEventData				= 3,
	EBadNotifyDevmonEventData				= 4,
	};

#endif // FDFAPI_H
