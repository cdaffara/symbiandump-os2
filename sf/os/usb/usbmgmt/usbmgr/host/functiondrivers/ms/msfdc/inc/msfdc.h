/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef MSFDC_H
#define MSFDC_H

#include <e32base.h>
#include <usbhost/internal/fdcinterface.h>
#ifndef __DUMMYFDF__
#include <usbhost/internal/fdcplugin.h>
#include "msmmclient.h"
#else
#include <dummymsmmclient.h>
#include <usbhost/internal/dummyfdcplugin.h>
#endif

/**
  The CMsFdc class
 
  Implements FDC that sends attach/disattach notifications to Mass
  Storage Mount Manager.
 */
NONSHARABLE_CLASS(CMsFdc) : public CFdcPlugin, public MFdcInterfaceV1
	{

public:
	static CMsFdc* NewL(MFdcPluginObserver& aObserver);
	~CMsFdc();


private:
	CMsFdc(MFdcPluginObserver& aObserver);
	void ConstructL();

private: // from CFdcPlugin
	TAny* GetInterface(TUid aUid);
	TInt GetDefaultLanguageL(TUint aDeviceId, TUint& aDefaultLangId);

private: // from MFdcInterfaceV1
	TInt Mfi1NewFunction(TUint aDeviceId,
		const TArray<TUint>& aInterfaces,
		const TUsbDeviceDescriptor& aDeviceDescriptor,
		const TUsbConfigurationDescriptor& aConfigurationDescriptor);
	void Mfi1DeviceDetached(TUint aDeviceId);
private:
	RMsmmSession iMsmmSession;
	};

#endif // MSFDC_H
