// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Include Files  
// 
//

/**
 @file xmlengdom_tef.cpp
 @internalTechnology
*/

#include "xmlengdom_tef.h"
#include "CXmlEngDomTestServer.h"

#include <e32base.h>
#include <e32std.h>
#include <e32cons.h>			// Console


LOCAL_C void MainL()
{
	CActiveScheduler* sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	CXmlEngDomTestServer* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CXmlEngDomTestServer::NewL());
	if (KErrNone == err)
		{
		// Sync with the client and enter the active scheduler
		RProcess::Rendezvous(KErrNone);
		sched->Start();
		}
	delete server;
	delete sched;
}


GLDEF_C TInt E32Main()
/**
 * @return - Standard Epoc error code on exit
 */
{
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if (cleanup == NULL)
		{
		return KErrNoMemory;
		}
	TRAPD(err,MainL())
	;
	delete cleanup;
	
	__UHEAP_MARKEND;
	return err;
}



