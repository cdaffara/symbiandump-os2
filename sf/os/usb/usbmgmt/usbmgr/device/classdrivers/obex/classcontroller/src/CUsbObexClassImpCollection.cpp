/*
* Copyright (c) 1997-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Defines the implementation collection for the USB Obex class controller.
*
*/

/**
 @file
*/

#include <e32base.h>
#include <e32std.h>
#include <ecom/ecom.h>
#include <ecom/implementationproxy.h>
#include "CUsbObexClassController.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "CUsbObexClassImpCollectionTraces.h"
#endif


// Define the private interface UIDs
const TImplementationProxy UsbCCImplementationTable[] =
    {
	IMPLEMENTATION_PROXY_ENTRY(0x101fbf28, CUsbObexClassController::NewL),
    };

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
	OstTraceFunctionEntry0( CUSBOBEXCLASSIMPCOLLECTION_IMPLEMENTATIONGROUPPROXY_ENTRY );
    aTableCount = sizeof(UsbCCImplementationTable) / sizeof(TImplementationProxy);
    OstTraceFunctionExit0( CUSBOBEXCLASSIMPCOLLECTION__IMPLEMENTATIONGROUPPROXY_EXIT );
    return UsbCCImplementationTable;
    }

