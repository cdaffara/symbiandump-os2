/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Server entry points
* EKA1 & EKA2
* EKA1 & WINS is a DLL. All others are exe's
*
*/



/**
 @file Main.cpp
*/
#include <test/testexecutepipslog.h>
#include "server.h"

// Emulator EKA1 is the only DLL case
// EKA2 all variats & EKA1 target only
// E32Main calls MainL()
LOCAL_C void MainL()
/**
 * Secure variant
 * Much simpler, uses the new Rendezvous() call to sync with the client
 */
	{
	// Leave Platform security hooks in place
#if (defined __DATA_CAGING__)
	RProcess().DataCaging(RProcess::EDataCagingOn);
	RProcess().SecureApi(RProcess::ESecureApiOn);
#endif
	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	CLogServer* server = NULL;
	// Create the sever
	TRAPD(err,server = CLogServer::NewL());
	if(!err)
		{
		// Client Synchronisation different for EKA1 and EKA2
		// EKA2 really simple. Client blocks on reciprocal Rendezvous call
		RProcess::Rendezvous(KErrNone);
		// Go into the Active scheduler
		sched->Start();
		// Free the server class
		delete server;
		}
	sched->Stop() ; 
	delete sched;
	}

//GLDEF_C TInt E32Main()
//this being an OE application we shall use 
// the OE provided main signature
int main(int,char**,char* )
/**
 * @return - Standard Epoc error code on process exit
 * Process entry point. Called by client using RProcess API
 */
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(cleanup == NULL)
		{
		return KErrNoMemory;
		}
	TRAP_IGNORE(MainL());
	delete cleanup;
	__UHEAP_MARKEND;
	return KErrNone;
    }


