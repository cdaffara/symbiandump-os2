// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Redirection server for STDLIB test programs
// Implement a redirection server which uses ... a console.
// 
//

#include <e32std.h>
#include <e32cons.h>
#include <redircli.h>
#include <redircliinternal.h>
#include "CTEST.H"
#include "testconsole.h"

_LIT(KReDirServerName,"RedirServer");

struct rendezvous 
	{
	RThread iCaller;
	TRequestStatus* iStatus;
	};


TInt threadFunction(TAny* aPtr)
//
// Create and run an active scheduler containing a RedirectionServer
//
	{
	CTrapCleanup* TheTrapCleanup=CTrapCleanup::New();

	struct rendezvous* rvp = (struct rendezvous *)aPtr;
	TInt ret=KErrNone;
	// start scheduler and server
	CActiveScheduler *pA=new CActiveScheduler;
	if (pA!=NULL) 
		{
		CActiveScheduler::Install(pA);
		// ***** TRAP(ret, CRedirServer::NewL(CTestConsoleFactory::NewL());
		TRAP(ret, CRedirServer2::NewL(CTestConsoleFactory::NewL()));
//		ret=KErrNotSupported;
		}
	// signal to the caller that we've started (or failed!)
	rvp->iCaller.RequestComplete(rvp->iStatus,ret);
	if (ret==KErrNone)
		{
		// start fielding requests from clients
		CActiveScheduler::Start();
		}
	// finished
	delete TheTrapCleanup;
	return(KErrNone);
	}


EXPORT_C int start_redirection_server()
//
// Try to start a redirection server thread, assuming there isn't one already
//
	{
	RRedirSession2 probe;
	TInt err=probe.Connect();
	probe.Close();
	if (err==KErrNone)
		return KErrNone;	// server already exists
	TRequestStatus status(KRequestPending);
	struct rendezvous rv;
	rv.iCaller.Duplicate(RThread(),EOwnerProcess);
	rv.iStatus=&status;
	RThread server;
	err=server.Create(KReDirServerName,threadFunction,0x2000,NULL,&rv);
	if (err==KErrNone) 
		{
		server.Resume();
		User::WaitForRequest(status);
		err=status.Int();
		server.Close();
		}
	rv.iCaller.Close();
	return err;
	}



