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
* Implementation of FDC plugin.
*
*/

/**
 @file
 @internalComponent
*/

#include <ecom/ecom.h>
#include <usbhost/internal/fdcplugin.h>
#include <usbhost/internal/fdcpluginobserver.h>

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "fdcpluginTraces.h"
#endif


EXPORT_C CFdcPlugin::~CFdcPlugin()
	{
    OstTraceFunctionEntry0( CFDCPLUGIN_CFDCPLUGIN_DES_ENTRY );
    
	REComSession::DestroyedImplementation(iInstanceId);
	}

EXPORT_C CFdcPlugin::CFdcPlugin(MFdcPluginObserver& aObserver)
:	iObserver(aObserver)
	{
    OstTraceFunctionEntry0( CFDCPLUGIN_CFDCPLUGIN_CONS_ENTRY );
    
	}

EXPORT_C CFdcPlugin* CFdcPlugin::NewL(TUid aImplementationUid, MFdcPluginObserver& aObserver)
	{
    OstTraceFunctionEntry0( CFDCPLUGIN_NEWL_ENTRY );
    
    OstTrace1( TRACE_NORMAL, CFDCPLUGIN_NEWL, "\t\tFDC implementation UID: 0x%08x", aImplementationUid.iUid);

	CFdcPlugin* plugin = reinterpret_cast<CFdcPlugin*>(
		REComSession::CreateImplementationL(
			aImplementationUid, 
			_FOFF(CFdcPlugin, iInstanceId),
			&aObserver
			)
		);

	OstTrace1( TRACE_NORMAL, CFDCPLUGIN_NEWL_DUP1, "\tplugin = 0x%08x", plugin);

	return plugin;
	}

EXPORT_C MFdcPluginObserver& CFdcPlugin::Observer()
	{
	return iObserver;
	}

EXPORT_C TInt CFdcPlugin::Extension_(TUint aExtensionId, TAny*& a0, TAny* a1)
	{
	return CBase::Extension_(aExtensionId, a0, a1);
	}
