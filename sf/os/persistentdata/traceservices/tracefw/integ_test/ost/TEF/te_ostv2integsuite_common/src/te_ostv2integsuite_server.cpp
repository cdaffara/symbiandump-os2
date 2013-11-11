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
#include "te_ostv2integsuite_server.h"
#include <e32base.h>
#include <e32std.h>

CTe_MCLSuiteServer* CTe_MCLSuiteServer::NewL()
	{
	CTe_MCLSuiteServer* self=new (ELeave) CTe_MCLSuiteServer;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(); // self;
	return self;
	}

CTestBlockController* CTe_MCLSuiteServer::CreateTestBlock()
	{
	CTestBlockController* controller=NULL;
	TRAP_IGNORE(
			{
			controller=new(ELeave) CTe_MCLSuiteController;
			});
	return controller;
	}


LOCAL_C void MainL()
	{
	CActiveScheduler* scheduler = new(ELeave)CActiveScheduler();
	CActiveScheduler::Install(scheduler);
	CTe_MCLSuiteServer* server=NULL;
	TRAPD(err,server=CTe_MCLSuiteServer::NewL());
	if(!err)
		{
		RProcess::Rendezvous(KErrNone);
		scheduler->Start();
		}
	delete server;
	delete scheduler;
	}

GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(cleanup==NULL)
		{
		return KErrNoMemory;
		}
	TRAP_IGNORE(MainL());
	delete cleanup;
	__UHEAP_MARKEND;
	return KErrNone;
	}
