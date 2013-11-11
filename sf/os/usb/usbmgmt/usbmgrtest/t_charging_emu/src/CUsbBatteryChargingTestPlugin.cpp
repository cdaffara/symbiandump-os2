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

#include <ecom/implementationproxy.h>

#include <dummyldd.h>
#define __D32USBC_H__ // ensure that d32usbc is ignored, and dummyldd used instead
#include "CUsbBatteryChargingTestPlugin.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "CUsbBatteryChargingTestPluginTraces.h"
#endif


// Define the private interface UIDs
const TImplementationProxy ImplementationTable[] =
	{										
	IMPLEMENTATION_PROXY_ENTRY(0x1020DEA8, CUsbBatteryChargingTestPlugin::NewL),
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

	return ImplementationTable;
	}

CUsbBatteryChargingTestPlugin* CUsbBatteryChargingTestPlugin::NewL(MUsbmanExtensionPluginObserver& aObserver)
	{
    OstTrace0( TRACE_NORMAL, CUSBBATTERYCHARGINGTESTPLUGIN_NEWL, _L8("NewL Test plug-n"));
    
	CUsbBatteryChargingTestPlugin* self = new(ELeave) CUsbBatteryChargingTestPlugin(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CUsbBatteryChargingTestPlugin::CUsbBatteryChargingTestPlugin(MUsbmanExtensionPluginObserver& aObserver)
: CUsbBatteryChargingPlugin(aObserver) {}

void CUsbBatteryChargingTestPlugin::GetPluginInfo(TPluginTestInfo& aInfo)
	{
	aInfo.iPluginState = iPluginState;
	aInfo.iAvailableMilliAmps = iAvailableMilliAmps;
	aInfo.iDeviceState = iDeviceState;
	aInfo.iCurrentIndexRequested = iCurrentIndexRequested;
	aInfo.iRequestedCurrentValue = iRequestedCurrentValue;
	}

TAny* CUsbBatteryChargingTestPlugin::GetInterface(TUid aUid)
	{
	TAny* ret = NULL;
    ret = CUsbBatteryChargingPlugin::GetInterface(aUid);
    if(!ret)
        {
	if (aUid == KUidUsbBatteryChargingTestPluginInterface2)
		{
		ret = reinterpret_cast<TAny*>(
			static_cast<MUsbBatteryChargingTestPluginInterface2*>(this)
			);
		}
        }
	return ret;
	}
