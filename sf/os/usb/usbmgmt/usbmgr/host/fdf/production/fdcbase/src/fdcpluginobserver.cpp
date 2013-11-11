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

#include <usbhost/internal/fdcpluginobserver.h>

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "fdcpluginobserverTraces.h"
#endif


EXPORT_C TUint32 MFdcPluginObserver::TokenForInterface(TUint8 aInterface)
	{
    OstTraceFunctionEntry1( MFDCPLUGINOBSERVER_TOKENFORINTERFACE_ENTRY, this );
	
	return MfpoTokenForInterface(aInterface);
	}

EXPORT_C const RArray<TUint>& MFdcPluginObserver::GetSupportedLanguagesL(TUint aDeviceId)
	{
	return MfpoGetSupportedLanguagesL(aDeviceId);
	}

EXPORT_C TInt MFdcPluginObserver::GetManufacturerStringDescriptor(TUint aDeviceId, TUint aLangId, TName& aString)
	{
	return MfpoGetManufacturerStringDescriptor(aDeviceId, aLangId, aString);
	}

EXPORT_C TInt MFdcPluginObserver::GetProductStringDescriptor(TUint aDeviceId, TUint aLangId, TName& aString)
	{
	return MfpoGetProductStringDescriptor(aDeviceId, aLangId, aString);
	}

EXPORT_C TInt MFdcPluginObserver::GetSerialNumberStringDescriptor(TUint aDeviceId, TUint aLangId, TName& aString)
	{
	return MfpoGetSerialNumberStringDescriptor(aDeviceId, aLangId, aString);
	}
