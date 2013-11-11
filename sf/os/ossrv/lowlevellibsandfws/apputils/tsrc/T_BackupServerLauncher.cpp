// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 
#include <e32test.h>
#include <e32std.h>
#include "Baksrv.h"
#include "backup_std.h"
#include "BASCHED.H"

void LaunchBackupServerL()
	{
	CBaServBackupScheduler* scheduler = new (ELeave) CBaServBackupScheduler;
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler);

	CBaBackupServer* server = CBaBackupServer::NewL();

	CleanupStack::PushL(server);
	server->ConstructL();

	RProcess::Rendezvous(KErrNone);

	CActiveScheduler::Start();

	CleanupStack::PopAndDestroy(server);
	CleanupStack::PopAndDestroy(scheduler);
	
	}

GLDEF_C TInt E32Main ()
	{
   	CTrapCleanup* cleanup=CTrapCleanup::New();
   	TInt err = KErrNoMemory;
   	
   	if (cleanup)
   		{
   		TRAP(err, LaunchBackupServerL());
   		delete cleanup;
   		}
   	
	return err;
	}

