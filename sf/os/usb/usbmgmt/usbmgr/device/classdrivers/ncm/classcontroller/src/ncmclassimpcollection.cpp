/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
 * @file
 * @internalComponent
 */

#include <e32base.h>
#include <e32std.h>
#include <ecom/ecom.h>
#include <ecom/implementationproxy.h>
#include "ncmclasscontroller.h"

// Define the private interface UIDs
const TImplementationProxy UsbCCImplementationTable[] =
    {
#ifdef OVERDUMMY_NCMCC
    IMPLEMENTATION_PROXY_ENTRY(0x2002EA9F, CNcmClassController::NewL),    
#else
	IMPLEMENTATION_PROXY_ENTRY(0x2002c305, CNcmClassController::NewL),
#endif // OVERDUMMY_NCMCC
    };

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount = sizeof(UsbCCImplementationTable) / sizeof(TImplementationProxy);

    return UsbCCImplementationTable;
    }
