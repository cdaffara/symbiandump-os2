/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
 @prototype
*/

#ifndef USBHOSTSTACK_H
#define USBHOSTSTACK_H

#include <e32base.h>

#include <usb/usbshared.h>
#include <usbhosterrors.h>

/**
RUsbHostStack is the interface for USBMAN to use the FDF.
*/
NONSHARABLE_CLASS(RUsbHostStack) : public RSessionBase
	{
public:
	IMPORT_C RUsbHostStack();
	// No destructor needed.

	IMPORT_C TInt Connect();
	IMPORT_C TVersion Version() const;

	/** Enable driver loading.
	@return Error. */
	IMPORT_C TInt EnableDriverLoading();

	/** Disable driver loading. */
	IMPORT_C void DisableDriverLoading(); // the client can't do anything with failure here.

	/** Notification of device attach, driver loading, detach.
	@param aStat Completion status.
	@param aDeviceEventInformation On successful completion, information on
	the device event.
	*/
	IMPORT_C void NotifyDeviceEvent(TRequestStatus& aStat, TDeviceEventInformation& aDeviceEventInformation);
	IMPORT_C void NotifyDeviceEventCancel();

	// Notification of non-device specific events and errors.
	IMPORT_C void NotifyDevmonEvent(TRequestStatus& aStat, TInt& aEvent);
	IMPORT_C void NotifyDevmonEventCancel();

	// Support for getting string descriptors from devices.
	// Note that the maximum length of a string descriptor is 253 bytes. This
	// is 253/2 =~ 126 Unicode characters. TName is 0x80 (= 128) Unicode
	// characters. So TName is just big enough without being excessive, or
	// worth defining our own type or length.
	IMPORT_C TInt GetSupportedLanguages(TUint aDeviceId, RArray<TUint>& aLangIds);
	IMPORT_C TInt GetManufacturerStringDescriptor(TUint aDeviceId, TUint aLangId, TName& aString);
	IMPORT_C TInt GetProductStringDescriptor(TUint aDeviceId, TUint aLangId, TName& aString);
	
	IMPORT_C TInt GetOtgDescriptor(TUint aDeviceId, TOtgDescriptor& aDescriptor);
	IMPORT_C TInt GetConfigurationDescriptor(TUint aDeviceId, TConfigurationDescriptor& aDescriptor);

	// Support for server-side out-of-memory testing. In release, these just
	// return KErrNone.

	IMPORT_C TInt __DbgFailNext(TInt aCount);
	IMPORT_C TInt __DbgAlloc();

private: // utility
	TInt DoConnect();
	TInt CopyLangIdsToArray(RArray<TUint>& aLangIds, const TDesC8& aBuffer);

private: // owned
	// Device event notification data
	TPckg<TDeviceEventInformation> iDeviceEventPckg;

	// Devmon event notification data
	TPckg<TInt> iDevmonEventPckg;
	};

#endif // USBHOSTSTACK_H
