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
*/

#ifndef DEVICEPROXY_H
#define DEVICEPROXY_H

#include <e32base.h>
#include "usbhoststack.h"

#ifdef __OVER_DUMMYUSBDI__
#include <usbhost/dummyusbdi/d32usbdi_hubdriver.h>
#else
#include <d32usbdi_hubdriver.h>
#endif

class TDeviceEvent;
class TOtgDescriptor;

NONSHARABLE_CLASS(CDeviceProxy) : public CBase
	{
public:
	/** Link between elements of this type in a TSglQue. */
	TSglQueLink iLink;

public:
	static CDeviceProxy* NewL(RUsbHubDriver& aHubDriver, TUint aDeviceId);
	~CDeviceProxy();

public: // querying information from the device
	TInt GetDeviceDescriptor(TUsbDeviceDescriptor& aDescriptor);
	TInt GetConfigurationDescriptor(TUsbConfigurationDescriptor& aDescriptor) const;
	const RArray<TUint>& GetSupportedLanguages() const;
	void GetManufacturerStringDescriptorL(TUint32 aLangId, TName& aString) const;
	void GetProductStringDescriptorL(TUint32 aLangId, TName& aString) const;
	void GetSerialNumberStringDescriptorL(TUint32 aLangId, TName& aString) const;
	
	void GetOtgDescriptorL(TOtgDescriptor& aDescriptor) const;
	void SetOtgDescriptorL(const TUsbOTGDescriptor& aDescriptor);

public: // management
	TInt GetTokenForInterface(TUint aIndex, TUint32& aToken) const;
	TUint DeviceId() const;
	void SetDriverLoadingEventData(TDriverLoadStatus aStatus, TInt aError = KErrNone);
	TInt Suspend();

	// These methods TAKE OWNERSHIP of the relevant objects.
	TDeviceEvent* GetAttachmentEventObject();
	TDeviceEvent* GetDriverLoadingEventObject();
	TDeviceEvent* GetDetachmentEventObject();
	inline void SetMultipleDriversFlag()
	{
		iMultipleDriversFound = ETrue;
	}

	inline TBool MultipleDriversFlag()
	{
		return iMultipleDriversFound;
	}
	inline void SetHasIADFlag()
	{
		iHasIAD = ETrue;
	}
	inline TBool HasIADFlag()
	{
		return iHasIAD;
	}

private:
	CDeviceProxy(TUint aDeviceId);
	void ConstructL(RUsbHubDriver& aHubDriver);

private: // utility
	void Log();
	void ReadStringDescriptorsL();
	void PopulateStringDescriptorsL(TUint8 aStringDescriptorIndex, RArray<TName>& aStringArray);
	void GetStringDescriptorFromUsbdL(TUint32 aLangId, TName& aString, TUint8 aStringDescriptorIndex) const;
	void GetStringDescriptorFromCacheL(TUint32 aLangId, TName& aString, const RArray<TName>& aStringArray) const;

private: // owned
	RUsbDevice iHandle;
	const TUint iId;

	TDeviceEvent* iAttachmentEvent;
	TDeviceEvent* iDriverLoadingEvent;
	TDeviceEvent* iDetachmentEvent;

	// The design of string descriptor access in this class deserves some 
	// explanation.
	// In the simplest implementation of the FDF, we handle a device 
	// attachment without getting or storing any string descriptors from the 
	// device. If the FDF doesn't load any drivers for the device, it must 
	// suspend the device to save power. If USBMAN, subsequent to this, 
	// queries any string descriptors, the FDF will have to resume the device 
	// first (string descriptors are not requested or cached by USBD). 
	// Resuming a device is an asynchronous operation. We must therefore 
	// either make querying a string descriptor asynchronous as far as USBMAN 
	// is concerned (which would clearly make the relevant APIs much less 
	// friendly) or find another way round. [For completeness: this 
	// asynchronicity would rely on 
	// RHubDriver::QueueDeviceStateChangeNotification.]
	// Solution 1: if the FDF doesn't load drivers for the device, wait until 
	// USBSVR has queried string descriptors from us before suspending the 
	// device. Con: USBMAN only queries string descriptors when its client (in 
	// licensee-land) does so. We can't make any assumptions about their doing 
	// it in a timely manner or indeed at all. 
	// Solution 2: use User::After to give the device time to resume 
	// synchronously. Con: this is a major no-no in a production thread which 
	// must remain responsive to user requests and lower-level events. 
	// Mentioned only for completeness.
	// Solution 3 (implemented): request and cache all supported string 
	// descriptors at attachment time. String descriptors are typically small 
	// (a dozen characters, say), we only support 3 of them (manufacturer, 
	// product and serial number- not all of which may actually be supported 
	// by a particular device) and there is usually only 1 language, if that. 
	// Having cached the string descriptors, it is trivial to look them up 
	// later when the client wants.
	// These arrays are tied together, i.e. iManufacturerStrings[2] is the 
	// manufacturer string in the language with ID iLangIds[2]. Note that this 
	// may be KNullDesC if the device doesn't support the manufacturer string 
	// descriptor, but there's still an entry in the array for it.
	RArray<TUint> iLangIds;
	RArray<TName> iManufacturerStrings;
	RArray<TName> iProductStrings;
	RArray<TName> iSerialNumberStrings;
	TBool		  iHasIAD;
	TBool         iMultipleDriversFound;
	TOtgDescriptor* iOtgDescriptor; // Owned
	};

#endif // DEVICEPROXY_H
