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
// test for DEF043452 Memory leak in ecomserver
// This file contains test for DEF043452 Memory leak in ecomserver
// 
//

#include <e32test.h>
#include <f32file.h>
#include <bautils.h>

#include "Interface.h" // interface to Plugins
#include "RegistryData.h"
#include "DefaultResolver.h"
#include "EComResolverParams.h"
#include "DriveInfo.h"
#include "RegistryResolveTransaction.h"

LOCAL_D RTest test(_L("t_defaultresolver.exe"));

/**
@SYMTestCaseID          SYSLIB-ECOM-CT-0700
@SYMTestCaseDesc	    Tests for defect number DEF043452
						Tests for memory leak in ecom server
@SYMTestPriority 	    High
@SYMTestActions  	    Simulates a request to list implementation of unknown interface
						Checks for any memory leak in the CDefaultResolver class
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
LOCAL_C void DoTestsL()
	{
	__UHEAP_MARK;

	CRegistryData* rd=NULL;
	/** CRegistryResolveTransaction */
 	CRegistryResolveTransaction* registryResolveTransaction = NULL;
	CDefaultResolver* dr=NULL;

	RFs TheFs;

	RImplInfoArray* ifArray=new(ELeave) RImplInfoArray;

	TEComResolverParams trp;

	//Connecting to file server
	TInt error=TheFs.Connect();
	test(error==KErrNone);

	rd=CRegistryData::NewL(TheFs);
 	// Set up client request and extended interfaces
	TClientRequest clientReq;
	RArray<TUid> extendedInterfaces;
	CleanupClosePushL(extendedInterfaces);
	// construct the registry resolve transaction object here
	TBool capability= ETrue;
 	registryResolveTransaction = CRegistryResolveTransaction::NewL(*rd,extendedInterfaces,clientReq,capability);
 	CleanupStack::PushL(registryResolveTransaction);

	dr=CDefaultResolver::NewL(*registryResolveTransaction);

	//Passing an unknown interface uid
	TUid ifUid = {0x10009DC3};

	const TInt KNoOfFailure=2;
	//Simulating a request to list implementation of unknown interface
	//with no memory leak in the CDefaultResolver class
	for (TInt i=0;i<KNoOfFailure;i++)
		{
		TRAPD(err,ifArray=dr->ListAllL(ifUid,trp));
		test(err==KEComErrNoInterfaceIdentified);
		test(ifArray->Count()==0);
		}

	CleanupStack::PopAndDestroy(registryResolveTransaction);
	CleanupStack::PopAndDestroy(&extendedInterfaces);

	delete ifArray;
	delete dr;
	delete rd;

	// TheFs.Close() is moved here because CRegistryData will
	// use the file server session to close the RFileReadStream.
	TheFs.Close();

	__UHEAP_MARKEND;

	}



GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	test.Title();
	test.Start(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0700 T_DEFAULTRESOLVER tests. "));

	// get clean-up stack
	CTrapCleanup* cleanup = CTrapCleanup::New();

	//install the scheduler
	CActiveScheduler* scheduler = new(ELeave)CActiveScheduler;
	CActiveScheduler::Install(scheduler);

	TRAPD(err,DoTestsL());
	test(err==KErrNone);
	delete scheduler;
	delete cleanup;

	test.End();
	test.Close();

	__UHEAP_MARKEND;
	return(0);
	}
