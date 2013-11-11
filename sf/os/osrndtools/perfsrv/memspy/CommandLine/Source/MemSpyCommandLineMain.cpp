/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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

// System includes
#include <e32std.h>
#include <f32file.h>
#include <e32base.h>
#include <e32cons.h>
#include <f32file.h>
#include <bacline.h>

// User includes
#include "MemSpyCommandLine.h"
#include "MemSpyCommands.h"

// ---------------------------------------------------------------------------
// DoMainL()
// 
// 
// ---------------------------------------------------------------------------
static void DoMainL()
    {
    // Scheduler
    CActiveScheduler* scheduler = new (ELeave) CActiveScheduler();
    CActiveScheduler::Install( scheduler );
    CleanupStack::PushL( scheduler );
    
    // Get command line 
    CCommandLineArguments* args = CCommandLineArguments::NewLC();
    
    //--
    CConsoleBase* console = Console::NewL( KMemSpyCLIName, TSize( KConsFullScreen, KConsFullScreen ) );
    CleanupStack::PushL( console );
    //--
    
    // Command line manager
    CMemSpyCommandLine* commandLineMgr = CMemSpyCommandLine::NewLC( *console );

    // Play nicely with external processes
    RProcess::Rendezvous( KErrNone );

    // Perform op
    commandLineMgr->PerformOpL( *args );  
        
    // Tidy up
    //CleanupStack::PopAndDestroy( 3, scheduler ); // scheduler, args, commandLineMgr
    CleanupStack::PopAndDestroy( 4 ); // scheduler, args,  console, commandLineMgr
    }
   


// ---------------------------------------------------------------------------
// E32Main()
// 
// 
// ---------------------------------------------------------------------------
GLDEF_C TInt E32Main()
    {
    __UHEAP_MARK;

#ifdef _DEBUG
    RDebug::Print(_L("[MemSpyCmdLine] E32Main() - MEMSPY COMMANDLINE - Thread %d is STARTING"), (TUint) RThread().Id() );
#endif

    CTrapCleanup* cleanupTrap = CTrapCleanup::New();
    TRAPD(err, DoMainL());  
    if  ( err != KErrNone )
        {
        RDebug::Print(_L("[MemSpyCmdLine] E32Main() - MEMSPY COMMANDLINE - error: %d"), err);
        }
    delete cleanupTrap; 

    __UHEAP_MARKEND;

#ifdef _DEBUG
    RDebug::Print(_L("[MemSpyCmdLine] E32Main() - MEMSPY COMMANDLINE - Thread %d is ENDING"), (TUint) RThread().Id() );
#endif

    return err;
    }

