// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Name		: test_timer_temp.cpp
// Exe source file
//



#include <e32base.h>
#include <e32std.h>
#include "te_upttimer.h"
#include "test_timer_temp.h"
#include <e32cons.h>			// Console

//  Global Variables
_LIT(KTextConsoleTitle, "Console");
_LIT(KTextFailed, " failed, leave code = %d");
_LIT(KTextPressAnyKey, " [press any key]\n");
_LIT(KTime, " returned time  = %d");

LOCAL_D CConsoleBase* console;  // write all messages to this


LOCAL_C void MainL()
	{
	CActiveScheduler* scheduler = new (ELeave) CActiveScheduler();
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler);

	CUptTimer testTimer;

	TInt time= testTimer.Test();
	console->Printf(KTime, time);
	
	// Delete active scheduler
	CleanupStack::PopAndDestroy(scheduler);
	}

GLDEF_C TInt E32Main()
	{
	// Create cleanup stack
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	// Create output console
	TRAPD(createError, console = Console::NewL(KTextConsoleTitle, TSize(KConsFullScreen,KConsFullScreen)));
	if (createError)
		return createError;

	
	// Run application code inside TRAP harness, wait keypress when terminated
	TRAPD(mainError, MainL());
	if (mainError)
		console->Printf(KTextFailed, mainError);
	console->Printf(KTextPressAnyKey);
	console->Getch();
	
	delete console;
	delete cleanup;
	__UHEAP_MARKEND;
	return KErrNone;

	
	}

// eof

