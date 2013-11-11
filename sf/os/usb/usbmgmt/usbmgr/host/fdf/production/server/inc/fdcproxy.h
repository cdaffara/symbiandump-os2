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

#ifndef FDCPROXY_H
#define FDCPROXY_H

#include <e32base.h>
#include <usbhost/internal/fdcpluginobserver.h>
#include <usb/usblogger.h>

class CFdcPlugin;
class MFdcInterfaceV1;
class CImplementationInformation;
class TUsbDeviceDescriptor;
class TUsbConfigurationDescriptor;
class CFdf;

/**
CFdcProxy is a class internal to the FDF. 
It is used to interface to CFdcPlugins, providing for 
(a) re-use of CFdcPlugin objects for multiple devices, and 
(b) easy finding of required CFdcPlugins by caching the data on which they 
match USB interfaces. 
*/
NONSHARABLE_CLASS(CFdcProxy) : public CBase, public MFdcPluginObserver
	{
public:
	/** Link between elements of this type in a TSglQue. */
	TSglQueLink iLink;

public:
	static CFdcProxy* NewL(CFdf& aFdf, CImplementationInformation& aImplInfo);
	~CFdcProxy();

public:
	const TDesC8& DefaultDataField() const;
	TInt NewFunction(TUint aDeviceId,
	const RArray<TUint>& aInterfaces,
	const TUsbDeviceDescriptor& aDeviceDescriptor,
	const TUsbConfigurationDescriptor& aConfigurationDescriptor);
	void DeviceDetached(TUint aDeviceId);
	TUid ImplUid() const;
	TInt Version() const;
	TInt DeviceCount() const;
	void MarkForDeletion();
	void UnmarkForDeletion();
	TBool MarkedForDeletion() const;
	TBool RomBased() const;

private:
	CFdcProxy(CFdf& aFdf);
	void ConstructL(CImplementationInformation& aImplInfo);

private: // from MFdcPluginObserver
	TUint32 MfpoTokenForInterface(TUint8 aInterface);
	const RArray<TUint>& MfpoGetSupportedLanguagesL(TUint aDeviceId);
	TInt MfpoGetManufacturerStringDescriptor(TUint aDeviceId, TUint aLangId, TName& aString);
	TInt MfpoGetProductStringDescriptor(TUint aDeviceId, TUint aLangId, TName& aString);
	TInt MfpoGetSerialNumberStringDescriptor(TUint aDeviceId, TUint aLangId, TName& aString);

private: // utilities
	void Invariant() const;
	void Log() const;
	void NewFunctionL(TUint aDeviceId,
	const RArray<TUint>& aInterfaces,
	const TUsbDeviceDescriptor& aDeviceDescriptor,
	const TUsbConfigurationDescriptor& aConfigurationDescriptor);
	void CheckDeviceIdL(TUint aDeviceId) const;

private: // unowned
	CFdf& iFdf;
	
private: // owned
	TUid				iImplementationUid;
	TInt				iVersion;
	RBuf8				iDefaultData;
	RArray<TUint> 		iDeviceIds;
	CFdcPlugin* 		iPlugin;
	MFdcInterfaceV1*	iInterface;
	TBool 				iInMfi1NewFunction;
	TInt 				i0thInterface;
	TBool				iMarkedForDeletion;
	TBool				iRomBased;
	};

#endif // FDCPROXY_H
