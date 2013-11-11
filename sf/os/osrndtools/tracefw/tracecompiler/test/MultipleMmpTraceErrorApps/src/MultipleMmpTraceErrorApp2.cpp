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

#include "MultipleMmpTraceErrorApp.h"
#include <e32base.h>
#include <e32std.h>
#include <e32cons.h>			// Console
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "MultipleMmpTraceErrorApp2Traces.h"
#endif

//  Constants

_LIT(KTextConsoleTitle, "Console");
_LIT(KTextFailed, " failed, leave code = %d");
_LIT(KTextPressAnyKey, " [press any key]\n");

//  Global Variables

LOCAL_D CConsoleBase* console; // write all messages to this


//  Local Functions

LOCAL_C void MainL()
    {
    /****************************SHOULD GIVE ERRORS*******************************/
    _LIT8(KTestDes,"Test Descriptor");
    TInt32 x = 555;
    TUint32 y = 666;
    
    //In these two cases Trace Compiler should give a error that an extension macro isn't necessary:
    //"TraceErrorApp.cpp, line xx: Trace does not need to use extension macro"
    OstTraceExt1( TRACE_FLOW, TEST1_MAINL, "MainL(): x=%d" , x );
    OstTraceExt1( TRACE_FLOW, TEST2_MAINL, "MainL(): y=%u" , y );
    
    //This should give an error stating that the trace name is duplicated
    //"TraceErrorApp.cpp, line xx: Trace is used from multiple places"
    OstTrace0( TRACE_FLOW, TEST3_MAINL, "MainL()" );
    OstTrace0( TRACE_FLOW, TEST3_MAINL, "MainL()" );
    
    //This should NOT give an error stating that the arguments don't match format spec
    //OstTraceExt2( TRACE_FLOW, TEST4_MAINL, "MainL(): x=%d;KTestDes=%d", x, KTestDes );

    //Wrong API i.e more parameters than it takes
    //"TraceErrorApp.cpp, line xx: Parameter count does not match the format specification"
    OstTrace0( TRACE_FLOW, TEST5_MAINL, "MainL(): x=%d" ,x);
    
    //Too many User-defined group ids should be max of 32 but there are 33 here), should get an error
    //"You have exceeded the number of Group IDs you have allocated."
    OstTrace0( MY_GROUP_ID1, TEST_MAINL6, "MainL()" );
    OstTrace0( MY_GROUP_ID2, TEST_MAINL7, "MainL()" );
    OstTrace0( MY_GROUP_ID3, TEST_MAINL8, "MainL()" );
    OstTrace0( MY_GROUP_ID4, TEST_MAINL9, "MainL()" );
    OstTrace0( MY_GROUP_ID5, TEST_MAINL10, "MainL()" );
    OstTrace0( MY_GROUP_ID6, TEST_MAINL11, "MainL()" );
    OstTrace0( MY_GROUP_ID7, TEST_MAINL12, "MainL()" );
    OstTrace0( MY_GROUP_ID8, TEST_MAINL13, "MainL()" );
    OstTrace0( MY_GROUP_ID9, TEST_MAINL14, "MainL()" );
    OstTrace0( MY_GROUP_ID10, TEST_MAINL15, "MainL()" );
    OstTrace0( MY_GROUP_ID11, TEST_MAINL16, "MainL()" );
    OstTrace0( MY_GROUP_ID12, TEST_MAINL17, "MainL()" );
    OstTrace0( MY_GROUP_ID13, TEST_MAINL18, "MainL()" );
    OstTrace0( MY_GROUP_ID14, TEST_MAINL19, "MainL()" );
    OstTrace0( MY_GROUP_ID15, TEST_MAINL20, "MainL()" );
    OstTrace0( MY_GROUP_ID16, TEST_MAINL21, "MainL()" );
    OstTrace0( MY_GROUP_ID17, TEST_MAINL22, "MainL()" );
    OstTrace0( MY_GROUP_ID18, TEST_MAINL23, "MainL()" );
    OstTrace0( MY_GROUP_ID19, TEST_MAINL24, "MainL()" );
    OstTrace0( MY_GROUP_ID20, TEST_MAINL25, "MainL()" );
    OstTrace0( MY_GROUP_ID21, TEST_MAINL26, "MainL()" );
    OstTrace0( MY_GROUP_ID22, TEST_MAINL27, "MainL()" );
    OstTrace0( MY_GROUP_ID23, TEST_MAINL28, "MainL()" );
    OstTrace0( MY_GROUP_ID24, TEST_MAINL29, "MainL()" );
    OstTrace0( MY_GROUP_ID25, TEST_MAINL30, "MainL()" );
    OstTrace0( MY_GROUP_ID26, TEST_MAINL31, "MainL()" );
    OstTrace0( MY_GROUP_ID27, TEST_MAINL32, "MainL()" );
    OstTrace0( MY_GROUP_ID28, TEST_MAINL33, "MainL()" );
    OstTrace0( MY_GROUP_ID29, TEST_MAINL34, "MainL()" );
    OstTrace0( MY_GROUP_ID30, TEST_MAINL35, "MainL()" );
    OstTrace0( MY_GROUP_ID31, TEST_MAINL36, "MainL()" );
    OstTrace0( MY_GROUP_ID32, TEST_MAINL37, "MainL()" );
    OstTrace0( MY_GROUP_ID33, TEST_MAINL38, "MainL()" );    
    /*****************************************************************************/
    
    
    /******************************SHOULD GIVE WARNINGS***************************/
    //Should get warnings from using deprecated GIDs
    //e.g. "Deprecated group TRACE_API detected. Updating to TRACE_BORDER in dictionary. Please update source code!"
    OstTrace0( TRACE_API, TEST_MAINL39, "MainL()" );  
    OstTrace0( TRACE_IMPORTANT, TEST_MAINL40, "MainL()" );  
    OstTrace0( TRACE_DETAILED, TEST_MAINL41, "MainL()" );
    OstTrace0( TRACE_DEBUG, TEST_MAINL42, "MainL()" );  
    /*****************************************************************************/
    
    CallSomeMoreTraceStatements();
    
    console->Write(_L("Hello, world!\n"));
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

