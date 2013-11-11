/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
* Key Management Server startup code.
*
*/


/**
 @file
*/


#include "kmsserver.h"


static void RunServerL()
/**
	Allocate and start the key management server.
 */
	{
	CActiveScheduler* as = new(ELeave) CActiveScheduler;
	CleanupStack::PushL(as);
	CActiveScheduler::Install(as);

	// the server is started when it is allocated
	CKeyMgmtServer::NewLC();

	// tell launching process the server has started successfully
	RProcess::Rendezvous(KErrNone);

	CActiveScheduler::Start();

	CleanupStack::PopAndDestroy(2, as);
	}

TInt E32Main()
/**
	Sets up a cleanup stack and active scheduler before starting the server.

	@return					Symbian OS error code.  KErrNone indicates success,
							and any other value indicates failure.
 */
	{
	__UHEAP_MARK;

	// allocating a cleanup stack also installs it
	CTrapCleanup* tc = CTrapCleanup::New();
	if (tc == 0)
		return KErrNoMemory;

	TRAPD(r, RunServerL());

	delete tc;

	__UHEAP_MARKEND;

	return r;
	}
