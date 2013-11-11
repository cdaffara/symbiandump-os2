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
 @publishedPartner
 @released
*/

#ifndef FDCINTERFACE_H
#define FDCINTERFACE_H

#include <e32base.h>

class TUsbDeviceDescriptor;
class TUsbConfigurationDescriptor;

/** 
The UID of this FDC interface. 
If the FDC API ever has to change, a new UID and associated M class will be 
created. New FDC implementations may implement the new API. Old (non-updated) 
FDCs will still work because the FDF examines at run-time which interfaces are 
supported by any particular FDC.
*/
const TInt KFdcInterfaceV1 = 0x10282B4C;

/**
Mixin for the FDC API.
*/
class MFdcInterfaceV1
	{
public:
	/** 
	Called when a device is attached.

	The Function Driver Framework has determined that this Function Driver is 
	the most suitable to drive at least one of the interfaces in the device's 
	selected configuration.
	
  	Each call to this method represents an attempt by the Function Driver 
	Framework to get the Function Driver Controller to take ownership of 
	interfaces (exactly one Function) on the device.
	
	Not all the interfaces contained may be relevant to this Function Driver.
	Some interfaces may be relevant to this Function Driver, but should *not* 
	be claimed in this call, as described below.

	The device's entire device descriptor and configuration descriptor are 
	given. 

	The Function Driver Framework provides a collection (aInterfaces) of 
	interfaces which it is offering to the Function Driver Controller. The 
	Function Driver Controller MUST take ownership of the first of these by 
	calling MFdcPluginObserver::TokenForInterface (the Function Driver 
	Controller was selected by the Function Driver Framework on the basis of 
	its default_data field matching this particular interface). The Function 
	Driver Controller MUST also take ownership of as many following interfaces 
	as make up a single Function as defined by the relevant USB Class, again 
	by calling MFdcPluginObserver::TokenForInterface for them.

	The Function Driver Controller may examine interface descriptors for the 
	interfaces it is being offered using the bInterfaceNumber items in 
	aInterfaces, and the TUsbConfigurationDescriptor given 
	(aConfigurationDescriptor).

	Once exactly one Function has been claimed, the Function Driver Controller 
	may return from this method. 
	The Function Driver Framework does not make further attempts to load 
	Function Drivers for the interfaces that have been claimed, whether an 
	error is returned or not.
	If an error is returned, then (a) the FDF will send a driver loading error 
	to USBMAN, and (b) the FDC must not have opened ANY RUsbInterfaces as a 
	result of this call to Mfi1NewFunction.
	It is important for the Function Driver Controller to claim all the 
	interfaces it should, even if it finds out early on that it won't be able 
	to handle the Function for environmental or other reasons. 
	The FDC may also return an error indicating that the Class-specific 
	descriptors are incorrectly formatted.

	The FD is not required to open RUsbInterface handles during 
	Mfi1NewFunction. If it does, then it can pass any failure back as a return 
	value to Mfi1NewFunction and this will be relayed to USBMAN as a driver 
	loading error. If it does not, then the FD must itself take responsibility 
	for relaying relevant failures to the user. (This is in aid of the 'no 
	silent failures' requirement.)

	This method may be called zero or more times for each device attached, 
	depending on the interfaces present in the device's active configuration 
	and on the other FDs present on the phone. If this function is called (and 
	returns KErrNone) one or more times on a particular FDC, then 
	Mfi1DeviceDetached will be called exactly once when the device is detached.
	If on device attachment this function is called one or more times for a 
	particular FDC, and each time the FDC returns an error, then no device 
	detachment notification will be given to that FDC. Hence any RUsbInterface 
	handles successfully opened in a call to this function should be cleaned 
	up immediately if the function returns error.

	@see MFdcPluginObserver::TokenForInterface.

	@param aDeviceId This is the ID of the new device. This identifier is 
	private between the Function Driver Framework and the Function Driver. Its 
	significance is to identify a device when it is removed (see 
	Mfi1DeviceDetached). It is not equivalent to the device address or to 
	anything else.
	
	@param aInterfaces A collection of interface numbers (bInterfaceNumber) 
	which are being offered to the Function Driver Controller. The Function 
	Driver Controller uses items in this array in calls to 
	MFdcPluginObserver::TokenForInterface. Ownership of the array is retained 
	by the FDF.

	@param aDeviceDescriptor The device descriptor of the device. Ownership is 
	retained by the FDF.
	
	@param aConfigurationDescriptor The selected configuration descriptor of 
	the device. Ownership is retained by the FDF.

	@return Error. On return (ANY error code), the interfaces claimed in calls 
	to MFdcPluginObserver::TokenForInterface are claimed by this FDC. If an 
	error is returned, the FDC must cause no interface handles to be opened as 
	a result of this call. The FDF will indicate a failure to load drivers to 
	USBMAN.
	*/
	virtual TInt Mfi1NewFunction(TUint aDeviceId,
		const TArray<TUint>& aInterfaces,
		const TUsbDeviceDescriptor& aDeviceDescriptor,
		const TUsbConfigurationDescriptor& aConfigurationDescriptor) = 0;
	
	/** 
	Called when a device is detached.
	The device is one of those which have previously been notified to the 
	Function Driver in a Mfi1NewFunction call.
	Any RUsbInterface handles which were opened as a result of the 
	corresponding earlier Mfi1NewFunction call should be closed as they 
	will be useless.

	@param aDeviceId This serves to identify the device which has disappeared. 
	This will be the device ID given in a previously completed 
	Mfi1NewFunction call.
	*/
	virtual void Mfi1DeviceDetached(TUint aDeviceId) = 0;
	};

#endif // FDCINTERFACE_H
