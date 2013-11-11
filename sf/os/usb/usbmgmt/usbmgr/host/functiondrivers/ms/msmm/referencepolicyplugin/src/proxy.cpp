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

// INCLUDE FILES
#include <e32std.h>
#include <ecom/implementationproxy.h>
#include <usb/usblogger.h>

#include "referencepolicyplugin.h"
#include "referenceplugin.hrh"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "proxyTraces.h"
#endif

 

// Provides a key value pair table, this is used to identify
// the correct construction function for the requested interface.
const TImplementationProxy ImplementationTable[] =
    {
	IMPLEMENTATION_PROXY_ENTRY(KUidMsmmPolicyPluginImp,
	        CReferencePolicyPlugin::NewL)
	};

// Function used to return an instance of the proxy table.
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(
        TInt& aTableCount)
    {
    OstTrace0( TRACE_NORMAL, REF_REFERENCEPOLICYPLUGIN_SRC_IMPLEMENTATIONGROUPPROXY, 
            ">>ImplementationGroupProxy()" );
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    OstTrace0( TRACE_NORMAL, REF_REFERENCEPOLICYPLUGIN_SRC_IMPLEMENTATIONGROUPPROXY_DUP1, 
                "<<ImplementationGroupProxy()" );
    return ImplementationTable;
    }

// End of file
