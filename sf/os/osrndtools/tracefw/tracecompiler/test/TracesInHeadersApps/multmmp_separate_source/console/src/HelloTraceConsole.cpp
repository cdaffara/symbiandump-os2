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


#include <e32cons.h>			// Console

#include "HelloTraceConsole.h"
#include "HelloTraceExample.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "HelloTraceConsoleTraces.h"
#endif


_LIT(KTextConsoleTitle, "HelloTrace");
_LIT(KTextPressAnyKey, " [press any key]\n");

LOCAL_D CConsoleBase* console; // write all messages to this

LOCAL_C void DoExamples()
    {
    TInt err = KErrNone;

    console->Printf(_L("Before main tracing examples ...\n"));

    console->Printf(_L(" Before type examples ..."));
    console->Printf(KTextPressAnyKey);
    console->Getch();
    TRAP(err, HelloTraceExample::JustTypes());    
    if (err)
        console->Printf(_L(" Failed with error %d\n"), err);

    console->Printf(_L(" Before Interface examples ..."));
    console->Printf(KTextPressAnyKey);
    console->Getch();
    TRAP(err, HelloTraceExample::Interface());    
    if (err)
        console->Printf(_L(" Failed with error %d\n"), err);
    
    console->Printf(_L(" Before FnEntryExit examples ..."));
    console->Printf(KTextPressAnyKey);
    console->Getch();
    TRAP(err, HelloTraceExample::FnEntryExit());    
    if (err)
        console->Printf(_L(" Failed with error %d\n"), err);    
    }

LOCAL_C void DoStartL()
    {
    // Create active scheduler (to run active objects)
    CActiveScheduler* scheduler = new (ELeave) CActiveScheduler();
    CleanupStack::PushL(scheduler);
    CActiveScheduler::Install(scheduler);

    DoExamples();
    
    // Delete active scheduler
    CleanupStack::PopAndDestroy(scheduler);
    }

//  Global Functions

GLDEF_C TInt E32Main()
    {
    OstTraceFunctionEntry0( E32MAIN_ENTRY );
    
    // Create cleanup stack
    __UHEAP_MARK;
    CTrapCleanup* cleanup = CTrapCleanup::New();

    // Create output console
    TRAPD(createError, 
          console = Console::NewL(KTextConsoleTitle, 
                                  TSize(KConsFullScreen, KConsFullScreen)));    
    if (createError)
        {
        delete cleanup;
        OstTrace1( TRACE_ERROR, E32MAIN_CREATE_ERROR, "Failed to create console error: %d", createError);
        OstTraceFunctionExit0( E32MAIN_EXIT_ERR );
        return createError;
        }

    TRAPD(mainError, DoStartL());    
    if (mainError)
        {
        OstTrace1( TRACE_ERROR, E32MAIN_START_ERROR, "DoStartL failed with error: %d", mainError);
        console->Printf(_L("DoStartL failed with error: %d\n\n"), mainError);
        }
    else 
        {
        console->Printf(_L("Before panic tracing example ..."));
        console->Printf(KTextPressAnyKey);
        console->Getch();
    
        OstTrace0( TRACE_FLOW, E32MAIN_BEFORE_PANIC, "About to cause a panic ..." );
        HelloTraceExample::PanicTrace();
        
        console->Printf(KTextPressAnyKey);
        console->Getch();
        }

    delete console;
    delete cleanup;
    __UHEAP_MARKEND;
    
    OstTraceFunctionExit0( E32MAIN_EXIT );
    return KErrNone;
    }

