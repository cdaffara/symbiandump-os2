/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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


/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/


#include "TFT.H"
#include "THindiExtensionApi.h"
#include "tfreetypedefects.h"
 
/* Path to the script

z:\GraphicsTest\FreeTypetest.script

*/

_LIT(KServerName,"TFreeTypeServer");

CTFreeTypeServer* CTFreeTypeServer::NewL()
/**
   @return - Instance of the test server
   Same code for Secure and non-secure variants
   Called inside the MainL() function to create and start the
   CTestServer derived server.
 */
	{
	CTFreeTypeServer * server = new (ELeave) CTFreeTypeServer();
	CleanupStack::PushL(server);
	// CServer base class call
	server->StartL(KServerName);
	CleanupStack::Pop(server);
	return server;
	}


LOCAL_C void MainL()
//
// Secure variant
// Much simpler, uses the new Rendezvous() call to sync with the client
//
	{
#if (defined __DATA_CAGING__)
 	RProcess().DataCaging(RProcess::EDataCagingOn);
	RProcess().SecureApi(RProcess::ESecureApiOn);
#endif
	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	CTFreeTypeServer* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CTFreeTypeServer::NewL());
	if(!err)
		{
		// Sync with the client and enter the active scheduler
		RProcess::Rendezvous(KErrNone);
		sched->Start();
		}
	delete server;
	delete sched;
	}

/** @return - Standard Epoc error code on process exit
    Secure variant only
    Process entry point. Called by client using RProcess API
*/
GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(cleanup == NULL)
		{
		return KErrNoMemory;
		}
	TRAPD(err,MainL());
	// This if statement is here just to shut up RVCT, which would otherwise warn
	// that err was set but never used
	if (err)
	    {
	    err = KErrNone;
	    }
	delete cleanup;
	__UHEAP_MARKEND;
	return KErrNone;
    }

CTestStep* CTFreeTypeServer::CreateTestStep(const TDesC& aStepName)
/**
   @return - A CTestStep derived instance
   Secure and non-secure variants
   Implementation of CTestServer pure virtual
 */
	{
	if(aStepName == KTFtStep)
		{
		return new CTFtStep();
		}
	else if(aStepName == KTSurrogateRasterizedStep)
		{
		return new CTSurrogateRasterizedStep();
		}
	else if(aStepName == KTestRasterizeGlyphStep)
		{
		return new CTestRasterizeGlyph();
		}
	else if(aStepName == KTestGlyphIndexStep)
		{
		return new CTestGlyphIndex();
		}
	else if(aStepName == KTestGlyphPointInHintedPixels)
		{
		return new CTestGlyphPointInHintedPixels();
		}
	else if(aStepName == KTestGlyphPointInFontUnits)
		{
		return new CTestGlyphPointInFontUnits();
		}
	else if(aStepName == KTestGetExtensionFontMetrics)
		{
		return new CTestGetExtensionFontMetrics();
		}
	else if(aStepName == KTestTrueTypeTableStep)
		{
		return new CTestTrueTypeTable();
		}
	return NULL;
	}
