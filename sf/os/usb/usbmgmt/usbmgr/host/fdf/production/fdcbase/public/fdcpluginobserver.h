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

#ifndef FDCPLUGINOBSERVER_H
#define FDCPLUGINOBSERVER_H

#include <e32base.h>

/**
Interface for FDCs to call into the FDF.
The public methods are non-virtual and exported, so that they can be added to
without breaking BC for existing (non-rebuilt) FDCs.
*/
class MFdcPluginObserver
	{
	// So only a specific class internal to the FDF *can* derive from this M
	// class.
	friend class CFdcProxy;

private:
	inline MFdcPluginObserver() {}

public:
	/**
	Retrieves the token for an interface and claims ownership of the
	interface.

	This method may only be called during execution of the FDC's
	implementation of MFdcInterfaceV1::Mfi1NewFunction. If it is called at
	any other time the FDC will be panicked.

	If aInterface is not one of the interfaces listed in the array
	'aInterfaces', passed to the FDC's implementation of
	MFdcInterfaceV1::Mfi1NewFunction, then the FDC will be panicked.

	In its implementation of MFdcInterfaceV1::Mfi1NewFunction, the FDC
	is expected to make at least one call to TokenForInterface. If it does
	not, it will be panicked.

	The FDC is expected to make sufficient calls to TokenForInterface to claim
	ownership of interfaces and to collect the tokens necessary to open
	RUsbInterface handles on those interfaces.
	The FDC must take a complete Function's worth of interfaces, and only a
	single Function's worth of interfaces, depending on how the relevant USB
	Class defines Functions.

	WARNING:If the token is equal to 0 then the FDC should not make any attempt
	to open any RUsbInterface handles with it because that means that the device has been
	disconnected (cable removed or detached because was draining too much current)
	thus will not being able to use the device

	@param aInterface The interface number. This will come from aInterfaces as
	passed in a call to MFdcInterfaceV1::Mfi1NewFunction.
	@return The token for the interface
	*/
	IMPORT_C TUint32 TokenForInterface(TUint8 aInterface);

	/**
	Retrieves the language IDs supported by the given device (as expressed in
	the device's string descriptor zero).
	@param aDeviceId This is the device ID as given in
	MFdcInterfaceV1::Mfi1NewFunction. If an invalid device ID is used then
	the method will leave with KErrNotFound.
	@return Non-ownership of an array of lang IDs.
	*/
	IMPORT_C const RArray<TUint>& GetSupportedLanguagesL(TUint aDeviceId);

	/**
	Retrieves the manufacturer string descriptor from the given device with
	the given language ID.
	@param aDeviceId This is the device ID as given in
	MFdcInterfaceV1::Mfi1NewFunction.
	@param aLangId The language ID of the desired string.
	@param aString On return, the desired string.
	@return Error.
	*/
	IMPORT_C TInt GetManufacturerStringDescriptor(TUint aDeviceId, TUint aLangId, TName& aString);

	/**
	Retrieves the product string descriptor from the given device with the
	given language ID.
	@param aDeviceId This is the device ID as given in
	MFdcInterfaceV1::Mfi1NewFunction.
	@param aLangId The language ID of the desired string.
	@param aString On return, the desired string.
	@return Error.
	*/
	IMPORT_C TInt GetProductStringDescriptor(TUint aDeviceId, TUint aLangId, TName& aString);

	/**
	Retrieves the serial number string descriptor from the given device with
	the given language ID.
	@param aDeviceId This is the device ID as given in
	MFdcInterfaceV1::Mfi1NewFunction.
	@param aLangId The language ID of the desired string.
	@param aString On return, the desired string.
	@return Error.
	*/
	IMPORT_C TInt GetSerialNumberStringDescriptor(TUint aDeviceId, TUint aLangId, TName& aString);

private:
	/**
	@see TokenForInterface.
	*/
	virtual TUint32 MfpoTokenForInterface(TUint8 aInterface) = 0;
	/**
	@see GetSupportedLanguagesL.
	*/
	virtual const RArray<TUint>& MfpoGetSupportedLanguagesL(TUint aDeviceId) = 0;
	/**
	@see GetManufacturerStringDescriptor.
	*/
	virtual TInt MfpoGetManufacturerStringDescriptor(TUint aDeviceId, TUint aLangId, TName& aString) = 0;
	/**
	@see GetProductStringDescriptor.
	*/
	virtual TInt MfpoGetProductStringDescriptor(TUint aDeviceId, TUint aLangId, TName& aString) = 0;
	/**
	@see GetSerialNumberStringDescriptor.
	*/
	virtual TInt MfpoGetSerialNumberStringDescriptor(TUint aDeviceId, TUint aLangId, TName& aString) = 0;
	};

#endif // FDCPLUGINOBSERVER_H
