/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32base.h>
#include <e32cons.h>
#include <e32test.h>
#include <e32twin.h>
#include "activeconsole.h"

LOCAL_C void ActiveTestL()
	{
	CConsoleBase* console = Console::NewL(_L("ACTIVETEST"), TSize(KConsFullScreen, KConsFullScreen));
	CleanupStack::PushL(console);

	CActiveScheduler* scheduler = new(ELeave) CActiveScheduler;
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler);

	CActiveConsole* activeConsole = CActiveConsole::NewLC(*console);
	activeConsole->RequestCharacter();
	CActiveScheduler::Start();
	// NB CActiveScheduler::Start only returns when someone somewhere has 
	// called CActiveScheduler::Stop.

	CleanupStack::PopAndDestroy(2); // activeConsole, scheduler

	console->Printf(_L("\nPress any key"));
	(void)console->Getch(); // get and ignore character
	CleanupStack::PopAndDestroy(); // console
	}

GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	TRAPD(error, ActiveTestL());
	__ASSERT_ALWAYS(!error, User::Panic(_L("E32Main"), error));
	delete cleanup;
	__UHEAP_MARKEND;
	return KErrNone;
	}
