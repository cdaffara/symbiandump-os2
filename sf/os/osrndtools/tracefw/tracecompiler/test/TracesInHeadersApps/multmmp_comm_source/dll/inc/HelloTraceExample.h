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


#ifndef __HELLOTRACEEXAMPLE_H__
#define __HELLOTRACEEXAMPLE_H__

#include <e32std.h>

class HelloTraceExample
    {
public:
    IMPORT_C static void JustTypes();    
    IMPORT_C static void Interface();    
    IMPORT_C static void FnEntryExit();    
    IMPORT_C static void PanicTrace();    

    // @TODO add examples for state macros
    // @TODO add examples for event macros
    // @TODO add examples for OstTraceData
    // @TODO add compile-time selector examples
    // @TODO add macro wrapper examples
    };

#endif  // __HELLOTRACEEXAMPLE_H__

