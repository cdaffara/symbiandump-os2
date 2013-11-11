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


#include <e32std.h>		 // GLDEF_C

#include "HelloTracePanic.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "HelloTracePanicTraces.h"
#endif

GLDEF_C void Panic(THelloTraceDllPanic aPanic)
    {
    OstTrace1( TRACE_FATAL, PANIC, "Panic(%{THelloTraceDllPanic})", (TUint) aPanic );    
    User::Panic(_L("HelloTraceDll"), aPanic);
    }
