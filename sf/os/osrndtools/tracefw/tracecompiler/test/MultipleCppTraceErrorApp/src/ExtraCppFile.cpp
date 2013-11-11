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
/*
 * ExtraCppFile.cpp
 *
 *  Created on: Dec 7, 2009
 *      Author: cajagu
 */

#include "MultipleCppTraceErrorApp.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "ExtraCppFileTraces.h"
#endif

void CallSomeMoreTraceStatements()
    {
    TInt32 x = 555;
    TUint32 y = 666;
    
    //In these two cases Trace Compiler should give a error that an extension macro isn't necessary:
    //"TraceErrorApp.cpp, line xx: Trace does not need to use extension macro"
    OstTraceExt1( TRACE_FLOW, TEST1_CALLMORE, "CallSomeMoreTraceStatements(): x=%d" , x );
    OstTraceExt1( TRACE_FLOW, TEST2_CALLMORE, "CallSomeMoreTraceStatements(): y=%u" , y );
    
    //Wrong API i.e more parameters than it takes
    //"TraceErrorApp.cpp, line xx: Parameter count does not match the format specification"
    OstTrace0( TRACE_FLOW, TEST3_CALLMORE, "CallSomeMoreTraceStatements(): x=%d" ,x);
    }
