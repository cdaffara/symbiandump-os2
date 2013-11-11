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


//  Include Files  

#include "MultipleMmpApp.h"
#include <kernel/kernel.h>
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "MultipleMmpApp2Traces.h"
#endif


DECLARE_STANDARD_EXTENSION()
    {
    /****************************TRACE STATEMENTS*********************************/
    OstTrace0( TRACE_FLOW, TEST1_MAINL, "MainL()" );
    OstTrace0( TRACE_FLOW, TEST2_MAINL, "MainL()" );    
    /*****************************************************************************/
    
    CallSomeMoreTraceStatements();
    
    return 0;
    }
