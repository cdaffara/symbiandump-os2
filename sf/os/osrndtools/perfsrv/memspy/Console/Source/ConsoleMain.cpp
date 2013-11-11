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
#include <e32test.h>
#include <f32file.h>
#include <e32base.h>
#include <e32cons.h>
#include <f32file.h>
#include <bacline.h>

// Engine includes
#include <memspy/engine/memspyengine.h>
#include <memspysession.h>

// User includes
#include "ConsoleMenu.h"
#include "ConsoleConstants.h"


// ---------------------------------------------------------------------------
// DoMainL()
// 
// 
// ---------------------------------------------------------------------------
static void DoMainL()
    {
    // Scheduler
    CActiveScheduler* scheduler = new CActiveScheduler();
    CActiveScheduler::Install( scheduler );
    CleanupStack::PushL( scheduler );

    // F32
    RFs fsSession;
    User::LeaveIfError( fsSession.Connect() );
    CleanupClosePushL( fsSession );
    
    // Console
	CConsoleBase* console = Console::NewL( KMemSpyConsoleName, TSize( KConsFullScreen, KConsFullScreen ) );
	CleanupStack::PushL( console );

    // Engine
    RMemSpySession session;
    User::LeaveIfError(session.Connect());
    CleanupClosePushL(session);

    // Menu & event handler AO
    CMemSpyConsoleMenu::NewLC( session, *console );

    // Play nicely with external processes
    RProcess::Rendezvous( KErrNone );

    // Start wait loop - console will stop it when done.
    CActiveScheduler::Start();

    // Tidy up
    CleanupStack::PopAndDestroy( 5, scheduler );
    }
   


// ---------------------------------------------------------------------------
// E32Main()
// 
// 
// ---------------------------------------------------------------------------
GLDEF_C TInt E32Main()
    {
    __UHEAP_MARK;
    RDebug::Print(_L("[MCon] E32Main() - MEMSPY CONSOLE"));

    CTrapCleanup* cleanupTrap = CTrapCleanup::New();
    TRAPD(err, DoMainL());  
    if  ( err != KErrNone )
        {
        RDebug::Printf( " " );
        RDebug::Printf( " " );
        RDebug::Print(_L("[MCon] E32Main() - MEMSPY CONSOLE - error: %d"), err);
        if ( err == KErrAlreadyExists )
            {
            RDebug::Printf( "\t\t [MCon] NB: MemSpy and MemSpyConsoleUi cannot run simultaneously!", err);
            RDebug::Printf( " " );
            RDebug::Printf( " " );
            }
        }
    delete cleanupTrap; 

    __UHEAP_MARKEND;

    return KErrNone;
    }

