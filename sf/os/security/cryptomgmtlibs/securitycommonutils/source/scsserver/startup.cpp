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
* Boilerplate code starts up server and exits the process when
* the server has stopped.
*
*/


/**
 @file
*/

#include <scs/scsserver.h>
#include <e32debug.h>

NONSHARABLE_CLASS(CSystemActiveScheduler) : public CActiveScheduler
	{
	virtual void Error(TInt aError) const;
	};

void CSystemActiveScheduler::Error(TInt aError) const
	{
	// Ignore the error and continue...
	RDebug::Printf("SCS- Active object failed with code %d - IGNORED\n", aError);
	}

static void RunServerL(TScsServerFactory aServerFactoryLC)
/**
	Allocate and start the session counting server.

	@param	aServerFactoryLC Factory function defined in the implementation
							EXE, which allocates an instance of the server object
							and puts it on the cleanup stack.
 */
	{
	CActiveScheduler* as = new(ELeave) CSystemActiveScheduler;
	CleanupStack::PushL(as);
	CActiveScheduler::Install(as);

	// the server is started when it is allocated
	(void) aServerFactoryLC();

	// tell launching process the server has started successfully
	RProcess::Rendezvous(KErrNone);

	CActiveScheduler::Start();

	// the active scheduler has been stopped here because there have
	// been no current sessions for the shutdown period.
	CleanupStack::PopAndDestroy(2, as);
	}

EXPORT_C TInt StartScsServer(TScsServerFactory aServerFactoryLC)
/**
	This function must be called from the server executable's E32Main function.
	It sets up a cleanup stack and active scheduler before starting the server.
	
	If the server is started successfully then this function does not return
	until the server shuts down.

	@param	aServerFactoryLC Factory function defined in the implementation
							EXE, which allocates an instance of the server object
							and puts it on the cleanup stack.
	@return					Symbian OS error code.	KErrNone indicates success,
							and any other value indicates failure.
 */
	{
	__UHEAP_MARK;

	// allocating a cleanup stack also installs it
	CTrapCleanup* tc = CTrapCleanup::New();
	if (tc == 0)
		return KErrNoMemory;

	TRAPD(r, RunServerL(aServerFactoryLC));

	delete tc;

	__UHEAP_MARKEND;

	return r;
	}

