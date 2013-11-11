// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

#include "Baksrvs.h"
#include <baksrv.h>
#include <bafl/backup_std.h>
#include <basched.h>
#include <w32std.h>

_LIT(KRomPath,"Z:\\System\\Libs\\");
_LIT(KFullBackUpServer,"eikbackupsrv.dll");

static void RunServerL()
	{

	RThread thread;
	User::LeaveIfError(User::RenameThread(__BACKUP_SERVER_NAME_V2));
    thread.SetPriority(EPriorityRealTime);
    thread.Close();
	
	RLibrary library;

	CBaServBackupScheduler* scheduler = new (ELeave) CBaServBackupScheduler;
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler);

	CBaBackupServer* server = NULL;

	TFullName threadName(_L("*"));
	threadName.Append(KWSERVThreadName);
	TFindThread findWserv(threadName);
	TFullName name;
	TBool wservLoaded = EFalse;
	if (findWserv.Next(name) != KErrNone)
		server = CBaBackupServer::NewL();
	else
		{
		User::LeaveIfError(library.Load(KFullBackUpServer, KRomPath));
		CleanupClosePushL(library);
		TLibraryFunction loadServer = library.Lookup(1);
		server = reinterpret_cast<CBaBackupServer*>((*loadServer)());
		User::LeaveIfNull(server);
		wservLoaded = ETrue;
		}
	CleanupStack::PushL(server);
	server->ConstructL();

	RProcess::Rendezvous(KErrNone);

	CActiveScheduler::Start();

	CleanupStack::PopAndDestroy(server);
	if (wservLoaded)
		CleanupStack::PopAndDestroy(); //library
	CleanupStack::PopAndDestroy(scheduler);
	}


TInt E32Main()
   //
   // Server process entry-point
   //
   	{
   	__UHEAP_MARK;
   	//
   	CTrapCleanup* cleanup=CTrapCleanup::New();
   	TInt r=KErrNoMemory;
   	if (cleanup)
   		{
 		TRAP(r,RunServerL());
   		delete cleanup;
   		}
   	//
   	__UHEAP_MARKEND;
   	return r;
   	}
 	
