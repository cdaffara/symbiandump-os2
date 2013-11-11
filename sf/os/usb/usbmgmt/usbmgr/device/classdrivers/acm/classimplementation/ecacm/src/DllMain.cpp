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
*
*/

#include <e32std.h>
#include <cs_port.h>
#include "AcmPortFactory.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "DllMainTraces.h"
#endif


extern "C" IMPORT_C CSerial* LibEntryL(void);	

EXPORT_C CSerial* LibEntryL()
/**
 * Lib main entry point
 */
	{	
	OstTraceFunctionEntry0( DLLMAIN_LIBENTRYL_ECACM_ENTRY );
	return (CAcmPortFactory::NewL());
	}

//
// End of file
