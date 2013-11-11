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
 @internalComponent Internal Symbian test code
*/

#include "TFntStoreServer.h"
#include "T_FNTMEM.H"
#include "T_FSOPEN.H"
#include "T_FontBitmap.H"
#include "T_MatchFonts.h"
#include "T_GlyphTreeDel.h"
#include "T_OpenFontTypes.h"
#include "T_FontMetrics.h"
#include "T_GetNearestFont.h"
#include "T_GetNearestFontCapability.h"
#include "T_SetDefaultTypefaceName.h"
#include "t_corruptfonts.h"
#include "T_FNT.H" 
#include "T_SHAPERCACHE.H"
#include "T_LinkedFonts.h"
#include "t_cachedeletion.h"
#include "T_LinkedFonts2.h"
#include "t_linkedfontsmemory.h"
#include "t_linkedfontscomparison.h"
#include "t_linkedfontscapability.h"
#include "T_LinkedFontUpdateTests.h"
#include "T_CACHE.H" 
#include "T_FontSessionCache.h"

CTFntStoreServer* CTFntStoreServer::NewL()
/**
   @return - Instance of the test server
   Same code for Secure and non-secure variants
   Called inside the MainL() function to create and start the
   CTestServer derived server.
 */
	{
	CTFntStoreServer * server = new (ELeave) CTFntStoreServer();
	CleanupStack::PushL(server);
	
	// Get server name from process so we can use SetCap to change the capabilites and use this server with a different filename
	RProcess handle = RProcess();
	TParsePtrC serverName(handle.FileName());
	
	// CServer base class call
	server->StartL(serverName.Name());
		
	CleanupStack::Pop(server);
	return server;
	}


LOCAL_C void MainL()
//
// Secure variant
// Much simpler, uses the new Rendezvous() call to sync with the client
//
	{
 	RProcess().DataCaging(RProcess::EDataCagingOn);
	RProcess().SecureApi(RProcess::ESecureApiOn);
	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	CTFntStoreServer* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CTFntStoreServer::NewL());
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
	if (err)
	    {
		RDebug::Print(_L("CTFntStoreServer::MainL - Error: %d"), err);
	   	User::Panic(_L("CTFntStoreServer"),err);
	    }
	delete cleanup;
	__UHEAP_MARKEND;
	return KErrNone;
    }

CTestStep* CTFntStoreServer::CreateTestStep(const TDesC& aStepName)
/**
   @return - A CTestStep derived instance
   Secure and non-secure variants
   Implementation of CTestServer pure virtual
 */
	{
	if (aStepName == KTFntStep)
        {
        return new CTFntStep();
        }
	else if(aStepName == KTFntMemStep)
		{
		return new CTFntMemStep();
		}
	else if(aStepName == KTFsOpenStep)
		{
		return new CTFsOpenStep();
		}
	else if(aStepName == KTFontBitmapStep)
		{
		return new CTFontBitmapStep();
		}
	else if(aStepName == KTMatchFontsStep)
		{
		return new CTMatchFontsStep();
		}
	else if(aStepName == KTGlyphTreeDelStep)
		{
		return new CTGlyphTreeDelStep();
		}
	else if(aStepName == KTOpenFontTypesStep)
		{
		return new CTOpenFontTypesStep();
		}
	else if(aStepName == KTFontMetricsStep)
		{
		return new CTFontMetricsStep();
		}
	else if(aStepName == KTGetNearestFontStep)
		{
		return new CTGetNearestFontStep();
		}
	else if(aStepName == KTGetNearestFontCapabilityStep)	// These tests have no capabilities and are expected to cause panics i.e. they will show up pass in the log files
		{
		return new CTGetNearestFontCapabilityStep();
		}
	else if(aStepName == KTSetDefaultTypefaceNameStep)	// These tests have no capabilities and are expected to cause panics i.e. they will show up pass in the log files
		{
		return new CTSetDefaultTypefaceNameStep();
		}
	else if(aStepName == KTCorruptFontRejectionStep)
		{
		return new CTCorruptFontRejectionStep();
		}
	else if(aStepName == KTShaperCacheStep)
		{
		return new CTShaperCacheStep();
		}
	else if (aStepName == KTCacheDeletionStep)	
		{
		return new CTCacheDeletionStep();
		}
	else if (aStepName == KTLinkedFonts2Step)
		{
		return new CTLinkedFonts2Step();
		}
	else if (aStepName == KTLinkedFontsMemoryStep)
		{
        return new CTLinkedFontsMemoryStep();
        }
    else if (aStepName == KTLinkedFontsComparisonStep)
        {
        return new CTLinkedFontsComparisonStep();
        }
    else if (aStepName == KTLinkedFontsCapabilityStep)
        {
        return new CTLinkedFontsCapabilityStep();
        }
    else if (aStepName == KTLinkedFontsUpdateStage1Step)
        {
        return new CTLinkedFontsUpdateStage1Step();
        }
    else if (aStepName == KTLinkedFontsUpdateStage2Step)
        {
        return new CTLinkedFontsUpdateStage2Step();
        }
    else if (aStepName == KTCacheStep)
        {
        return new CTCacheStep();
        }
    else if (aStepName == KTFontSessionCacheStep)
        {
        return new CTFontSessionCacheStep();
        }
	RDebug::Printf("Test Step is either mis-spelt or doesn't exist - see CTFntStoreServer::CreateTestStep");
	return NULL;
	}

