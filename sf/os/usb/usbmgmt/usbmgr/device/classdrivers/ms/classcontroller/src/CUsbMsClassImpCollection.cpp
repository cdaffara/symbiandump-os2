/*
* Copyright (c) 2004-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Defines the implementation collection for the mass storage class controller.
*
*/

/**
 @file
*/

#include <e32base.h>
#include <e32std.h>
#include <ecom/ecom.h>
#include <ecom/implementationproxy.h>
#include "CUsbMsClassController.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "CUsbMsClassImpCollectionTraces.h"
#endif


// Define the private interface UIDs
const TImplementationProxy UsbCCImplementationTable[] =
    {
	IMPLEMENTATION_PROXY_ENTRY(0x10204bbc, CUsbMsClassController::NewL),
    };

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
	OstTraceFunctionEntry0( CUSBMSCLASSIMPCOLLECTION_IMPLEMENTATIONGROUPPROXY_ENTRY );
    aTableCount = sizeof(UsbCCImplementationTable) / sizeof(TImplementationProxy);
    OstTraceFunctionExit0( CUSBMSCLASSIMPCOLLECTION_IMPLEMENTATIONGROUPPROXY_EXIT );
    return UsbCCImplementationTable;
    }
