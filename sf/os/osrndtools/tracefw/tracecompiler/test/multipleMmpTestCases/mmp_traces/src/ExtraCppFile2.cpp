/*
 * ExtraCppFile.cpp
 *
 *  Created on: Dec 7, 2009
 *      Author: cajagu
 */


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

#include "MultipleMmpApp.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "ExtraCppFile2Traces.h"
#endif

void CallSomeMoreTraceStatements()
    {
    TInt32 x = 555;
    TUint32 y = 666;

    OstTrace1( TRACE_FLOW, TEST1_CALLMORE, "CallSomeMoreTraceStatements(): x=%d" , x );
    OstTrace1( TRACE_FLOW, TEST2_CALLMORE, "CallSomeMoreTraceStatements(): y=%u" , y );
    }
