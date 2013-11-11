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
#include "FixedIdDefinitionsTestApp.h"
#include <e32base.h>
#include <e32debug.h>
#include <e32std.h>
#include <e32cons.h>
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "FixedIdDefinitionsTestAppTraces.h"
#endif
// Console
//  Constants
_LIT(KTextConsoleTitle, "Console");
_LIT(KTextFailed, " failed, leave code = %d");
_LIT(KTextPressAnyKey, " [press any key]\n");

//  Global Variables
LOCAL_D CConsoleBase* console; // write all messages to this


//  Local Functions

LOCAL_C void MainL()
    {
//     OstTrace0(GROUP1, TRACE0, "trace 0"); // TRACE_USED_IN_TEST_CASE_1
//     OstTrace0(GROUP2, TRACE1, "trace 1"); // TRACE_USED_IN_TEST_CASE_1
 
//     OstTrace0(GROUP1, TRACE0, "trace 0"); // TRACE_USED_IN_TEST_CASE_2
//     OstTrace0(GROUP2, TRACE2, "trace 2"); // TRACE_USED_IN_TEST_CASE_2 
//     OstTrace0(GROUP2, TRACE1, "trace 1"); // TRACE_USED_IN_TEST_CASE_2
//     OstTrace0(GROUP2, TRACE3, "trace 3"); // TRACE_USED_IN_TEST_CASE_2 
    
//     OstTrace0(GROUP2, TRACE2, "trace 2"); // TRACE_USED_IN_TEST_CASE_3 
//     OstTrace0(GROUP2, TRACE3, "trace 3"); // TRACE_USED_IN_TEST_CASE_3
    
//     OstTrace0(GROUP1, TRACE0, "trace 0"); // TRACE_USED_IN_TEST_CASE_4
//     OstTrace0(GROUP2, TRACE2, "trace 2"); // TRACE_USED_IN_TEST_CASE_4 
//     OstTrace0(GROUP2, TRACE1, "trace 1"); // TRACE_USED_IN_TEST_CASE_4
//     OstTrace0(TRACE_FATAL, TRACE3, "trace 3"); // TRACE_USED_IN_TEST_CASE_4

//     OstTrace0(GROUP1, TRACE0, "trace 0"); // TRACE_USED_IN_TEST_CASE_5
//     OstTrace0(GROUP2, TRACE2, "trace 2"); // TRACE_USED_IN_TEST_CASE_5 
//     OstTrace0(GROUP2, TRACE1, "trace 1"); // TRACE_USED_IN_TEST_CASE_5

//     OstTrace0(GROUP1, TRACE0, "trace 0"); // TRACE_USED_IN_TEST_CASE_6_0
//     OstTrace0(GROUP2, TRACE2, "trace 2"); // TRACE_USED_IN_TEST_CASE_6_0 
//     OstTrace0(GROUP1, TRACE0, "trace 0"); // TRACE_USED_IN_TEST_CASE_6_1 
//     OstTrace0(GROUP2, TRACE2, "trace 2"); // TRACE_USED_IN_TEST_CASE_6_1 
//     OstTrace0(GROUP2, TRACE1, "trace 1"); // TRACE_USED_IN_TEST_CASE_6_1
//     OstTrace0(GROUP1, TRACE0, "trace 0"); // TRACE_USED_IN_TEST_CASE_6_2
//     OstTrace0(GROUP2, TRACE2, "trace 2"); // TRACE_USED_IN_TEST_CASE_6_2 
//     OstTrace0(GROUP1, TRACE0, "trace 0"); // TRACE_USED_IN_TEST_CASE_6_3
//     OstTrace0(GROUP2, TRACE2, "trace 2"); // TRACE_USED_IN_TEST_CASE_6_3 
//     OstTrace0(GROUP2, TRACE1, "trace 1"); // TRACE_USED_IN_TEST_CASE_6_3

//     OstTrace0(GROUP2, TRACE0, "trace 0"); // TRACE_USED_IN_TEST_CASE_7_0
//     OstTrace0(GROUP2, TRACE2, "trace 2"); // TRACE_USED_IN_TEST_CASE_7_0 
//     OstTrace0(GROUP2, TRACE1, "trace 1"); // TRACE_USED_IN_TEST_CASE_7_0
//     OstTrace0(TRACE_NORMAL, TRACE0, "trace 0"); // TRACE_USED_IN_TEST_CASE_7_1
//     OstTrace0(GROUP2, TRACE2, "trace 2"); // TRACE_USED_IN_TEST_CASE_7_1
//     OstTrace0(GROUP2, TRACE1, "trace 1"); // TRACE_USED_IN_TEST_CASE_7_1
//     OstTrace0(GROUP1, TRACE0, "trace 0"); // TRACE_USED_IN_TEST_CASE_7_2
//     OstTrace0(GROUP2, TRACE2, "trace 2"); // TRACE_USED_IN_TEST_CASE_7_2 
//     OstTrace0(GROUP2, TRACE1, "trace 1"); // TRACE_USED_IN_TEST_CASE_7_2   
   
//     OstTrace0(TRACE_FLOW, TRACE1, "trace 1"); // TRACE_USED_IN_TEST_CASE_8      

//     OstTrace0(MY_GROUP, TRACE1, "trace 1"); // TRACE_USED_IN_TEST_CASE_9                
    }

LOCAL_C void DoStartL()
    {
    // Create active scheduler (to run active objects)
    CActiveScheduler* scheduler = new (ELeave) CActiveScheduler();
    CleanupStack::PushL(scheduler);
    CActiveScheduler::Install(scheduler);

    MainL();

    // Delete active scheduler
    CleanupStack::PopAndDestroy(scheduler);

    }

//  Global Functions

GLDEF_C TInt E32Main()
    {
    // Create cleanup stack
    __UHEAP_MARK;
    CTrapCleanup* cleanup = CTrapCleanup::New();
    // Create output console
    TRAPD(createError, console = Console::NewL(KTextConsoleTitle, TSize(
                            KConsFullScreen, KConsFullScreen)));
    if (createError)
        return createError;
    // Run application code inside TRAP harness, wait keypress when terminated
    TRAPD(mainError, DoStartL());
    if (mainError)
        console->Printf(KTextFailed, mainError);
    console->Printf(KTextPressAnyKey);
    console->Getch();

    delete console;
    delete cleanup;
    __UHEAP_MARKEND;
    return KErrNone;
    }

