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
* MemSpyServer Main class
*/

// System includes
#include <e32std.h>
#include <e32base.h>
#include <f32file.h>

// Engine includes
#include <memspy/engine/memspyengine.h>

// User includes
//#include "MemSpyServer.h"

static void RunServerL()
    {
    // Scheduler
    CActiveScheduler* scheduler = new (ELeave) CActiveScheduler();
    CleanupStack::PushL( scheduler );    
    CActiveScheduler::Install( scheduler );    
    
    // F32
    RFs fsSession;
    User::LeaveIfError(fsSession.Connect());
    CleanupClosePushL(fsSession);
    
    // MemSpyEngine initialization
    CMemSpyEngine* engine = CMemSpyEngine::NewL(fsSession);
    CleanupStack::PushL(engine);
    
    // Play nicely with external processes
    RProcess::Rendezvous( KErrNone );       

    // Start wait loop.
    CActiveScheduler::Start();
    
    // Tidy up
    CleanupStack::PopAndDestroy( 3, scheduler );
    }

TInt E32Main()
    {
    __UHEAP_MARK;

    CTrapCleanup* cleanupTrap = CTrapCleanup::New();
    
	TInt r = KErrNoMemory;
	if ( cleanupTrap )
		{
		TRAPD(err, RunServerL());		
		if  ( err != KErrNone )
		        {
		        RDebug::Print(_L("[MemSpyCmdLine] E32Main() - MemSpyServer - error: %d"), err);
		        }
		delete cleanupTrap;
		}  

    __UHEAP_MARKEND;

    return r;
    }
