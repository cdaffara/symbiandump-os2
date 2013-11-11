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

#ifndef REFFDC_H
#define REFFDC_H

#include <e32base.h>
#include <usbhost/internal/fdcplugin.h>
#include <usbhost/internal/fdcinterface.h>

NONSHARABLE_CLASS(CRefFdc) : public CFdcPlugin, public MFdcInterfaceV1
	{
public:
	static CRefFdc* NewL(MFdcPluginObserver& aObserver);
	~CRefFdc();

private:
	CRefFdc(MFdcPluginObserver& aObserver);
	void ConstructL();
	
private: // from CFdcPlugin
	TAny* GetInterface(TUid aUid);

private: // from MFdcInterfaceV1
	TInt Mfi1NewFunction(TUint aDeviceId,
		const TArray<TUint>& aInterfaces,
		const TUsbDeviceDescriptor& aDeviceDescriptor,
		const TUsbConfigurationDescriptor& aConfigurationDescriptor);
	void Mfi1DeviceDetached(TUint aDeviceId);

private: // utility
	void NewFunctionL(TUint aDeviceId,
		const TArray<TUint>& aInterfaces,
		const TUsbDeviceDescriptor& aDeviceDescriptor,
		const TUsbConfigurationDescriptor& aConfigurationDescriptor);
	};

#endif // REFFDC_H
