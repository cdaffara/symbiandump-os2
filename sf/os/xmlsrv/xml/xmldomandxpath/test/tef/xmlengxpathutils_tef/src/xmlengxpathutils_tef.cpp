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
 @file xmlengxpathutils_tef.cpp
 @internalTechnology
*/

#include "xmlengxpathutils_tef.h"
#include <e32base.h>
#include <e32std.h>
#include <e32cons.h>			// Console


#include "CXmlEngXPathUtilsTestServer.h"

//  Local Functions

LOCAL_C void MainL()
{
	CActiveScheduler* sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	
	CXmlEngXPathUtilsTestServer* server = NULL;
	
	// Create the CTestServer derived server
	
	TRAPD(err,server = CXmlEngXPathUtilsTestServer::NewL());
	if (KErrNone == err)
		{
		// Sync with the client and enter the active scheduler
		RProcess::Rendezvous(KErrNone);
		sched->Start();
		}
	delete server;
	delete sched;
}



//  Global Functions

GLDEF_C TInt E32Main()
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


