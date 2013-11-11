// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Contains implementation of CTestNormaliseUriStep class
// 
//

/**
 @file TESTNORMALISEURISTEP.CPP
 @internalTechnology
*/

// User Include
#include "TestNormaliseUriStep.h"
// System Include
#include <uriutils.h>
#include <uri8.h>

//constants
const TInt KAlreadyNormalised 			= 0;
const TInt KPercentEncoding				= 1;
const TInt KCaseNormalisation			= 2;
const TInt KRemoveDotegments			= 3;
const TInt KPercentCaseNormalisation 	= 12;
const TInt KPercentRemoveDotegments 	= 13;
const TInt KCaseRemoveDotegments 		= 23;
const TInt KAll 						= 123;

/**
  Constructor: Sets the test step name.
  @internalTechnology
  @test
*/
CTestNormaliseUriStep::CTestNormaliseUriStep()
	{
	//Call base class method to set human readable name for test step
	SetTestStepName(KTestNormaliseUriStep);
	}

/**
  Destructor
  @internalTechnology
  @test
*/
CTestNormaliseUriStep::~CTestNormaliseUriStep()
	{
	
	}

/**
  Does the main functionality test.
  Here, reads values from INI file and calls NormaliseAndCompareUriL
  @internalTechnology 
  @param		None
  @return		EPass or EFail indicating the success or failure of the test step
*/
TVerdict CTestNormaliseUriStep::doTestStepL()
	{
	__UHEAP_MARK;
	INFO_PRINTF1(_L("\n"));
	// Get necessary information from INI file

	TPtrC uri;
	TPtrC expUri;
	TInt RetCode;
	
	if(!GetStringFromConfig(ConfigSection(), 	KIniUri, uri) ||
	   !GetStringFromConfig(ConfigSection(), KIniExpectedUri, expUri) ||
	   !GetIntFromConfig(ConfigSection(), KRetCode, RetCode) )
		{
		ERR_PRINTF3(_L("Problem in reading values from ini.			\
						\nExpected fields are: \n%S\n%S\n"),&KIniUri, &KIniExpectedUri);
		SetTestStepResult(EFail);
		}
	else
		{// No problem in reading values from INI file. Proceed.
	    TRAPD(err, NormaliseAndCompareUriL(uri,expUri));
	    if(err != KErrNone)
		    {
	        ERR_PRINTF2(_L("Test step failed: Leave occured in NormaliseAndCompareUriL fun: %D\n"), err);
	 	    SetTestStepResult(EFail);	
		    }
		 }
	__UHEAP_MARKEND;
	switch (RetCode)
		{
		case KAlreadyNormalised :
			{
			INFO_PRINTF1(_L("\nAlready in Normalised Form"));
			break;	
			}
		case KPercentEncoding :
			{
			INFO_PRINTF1(_L("\nNormalised through PercentEncoding"));
			break;	
			}
		case KCaseNormalisation :
			{
			INFO_PRINTF1(_L("\nNormalised through CaseNormalisation"));
			break;	
			}
		case KRemoveDotegments :
			{
			INFO_PRINTF1(_L("\nNormalised through RemoveDotegments"));
			break;	
			}
		case KPercentCaseNormalisation :
			{
			INFO_PRINTF1(_L("\nNormalised through PercentEncoding and CaseNormalisation"));
			break;	
			}
		case KPercentRemoveDotegments :
			{
			INFO_PRINTF1(_L("\nNormalised through PercentEncoding and RemoveDotegments"));
			break;	
			}
		case KCaseRemoveDotegments :
			{
			INFO_PRINTF1(_L("\nNormalised through CaseNormalisation and RemoveDotegments"));
			break;	
			}
		case KAll :
			{
			INFO_PRINTF1(_L("\nNormalised through CaseNormalisation,PercentEncoding and RemoveDotegments"));
			break;	
			}
		default:
			{
			//do nothing
			break;	
			}
		}
	INFO_PRINTF1(_L("\nTest of UriNormalisation is Executed\n"));
	return TestStepResult();
	}
	
/**
  Performs syntax normalisation and compares actual & expected result.
  @param		aUri A reference to actual Uri to be normalised
  @param		aExpUri A reference to expected uri.
*/	
void CTestNormaliseUriStep::NormaliseAndCompareUriL(const TDesC& aUri,const TDesC& aExpUri)
	{
    CUri8* cUri8=NULL;
	TRAPD(err, cUri8=UriUtils::CreateUriL(aUri));
	if(err != KErrNone)
		{
		ERR_PRINTF2(_L("Test step failed: Leave occured in Uri object creation: %D\n"), err);
		SetTestStepResult(EFail);
		}
	else
		{
		const TUriC8& tUri(cUri8->Uri());
		CUri8* normalisedUri = NULL; 
		CleanupStack::PushL(cUri8);
		INFO_PRINTF1(_L("Calling UriUtils::NormaliseUriL which does syntax normalisation.")); 	
		TRAPD(error, normalisedUri=UriUtils::NormaliseUriL(tUri));
		if(error == KErrNone)
			{
			INFO_PRINTF1(_L("URI normalised.")); 	
			CleanupStack::PushL(normalisedUri);
			HBufC8* uriBuf = HBufC8::NewLC(aExpUri.Length());
			TPtr8 expUri8(uriBuf->Des());
			expUri8.Copy(aExpUri);
			// Comparing actual & expected uris.
			if(normalisedUri->Uri().UriDes().Compare(expUri8)!=0)
				{
				ERR_PRINTF1(_L("Test step failed: actualUri is not matched with expectedUri."));
				SetTestStepResult(EFail);
				}
			CleanupStack::PopAndDestroy(3, cUri8);
			}
		}
	}


/**
  Constructor: Sets the test step name.
  @internalTechnology
  @test
*/
CTestNormaliseUriOomStep::CTestNormaliseUriOomStep()
	{
	//Call base class method to set human readable name for test step
	SetTestStepName(KTestOomNormaliseUriStep);
	}

/**
  Destructor
  @internalTechnology
  @test
*/
CTestNormaliseUriOomStep::~CTestNormaliseUriOomStep()
	{
	
	}

/**
  Does the main functionality and OOM test of a test step.
  @internalTechnology 
  @param		None
  @return		EPass or EFail indicating the success or failure of the test step
*/
TVerdict CTestNormaliseUriOomStep::doTestStepL()
	{
	__UHEAP_MARK;
	TPtrC uri;
	if(!GetStringFromConfig(ConfigSection(), 	KIniUri, uri) )
		{
		ERR_PRINTF2(_L("Problem in reading values from ini.			\
						\nExpected fields are: \n%S\n"),&KIniUri);
		SetTestStepResult(EFail);
		}
	else
		{// No problem in reading values from INI file. Proceed.
	    CUri8* cUri8=NULL;
		TRAPD(err, cUri8=UriUtils::CreateUriL(uri));
		if(err != KErrNone)
			{
			ERR_PRINTF2(_L("Test step failed: Leave occured in Uri object creation: %D\n"), err);
			SetTestStepResult(EFail);
			}
		else
			{
			const TUriC8& tUri(cUri8->Uri());
			CleanupStack::PushL(cUri8);
			INFO_PRINTF1(_L("Calling UriUtils::NormaliseUriL which does syntax normalisation.")); 	
			TInt ret = KErrNoMemory;
			TInt failAt = 0;
			while(ret != KErrNone)
				{
				failAt++;
				INFO_PRINTF2(_L("OOM test step: %d\n"),failAt);

				__UHEAP_SETFAIL( RHeap::EDeterministic, failAt );
				__UHEAP_MARK;
				CUri8* normalisedUri = NULL;
				TRAP( ret, normalisedUri = UriUtils::NormaliseUriL(tUri));
				delete normalisedUri;
				__UHEAP_MARKEND;
				__UHEAP_RESET;
				if((ret == KErrNoMemory) || (ret == KErrNone))
					{
					INFO_PRINTF2(_L("Error/RetCode : %D\n"),ret);
					}
				}
			CleanupStack::PopAndDestroy(cUri8);
			}
		}
	__UHEAP_MARKEND;
	return TestStepResult();
	}
		
