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
// Contains implementation of CTestTelUriValidationStep class
// 
//

/**
 @file
 @internalTechnology 
*/

// User Include
#include "TestTelUriValidationStep.h"
// System Include
#include <uri8.h>
#include <uriutils.h>

/**
  Constructor: Sets the test step name.
  @internalTechnology
  @test
*/
CTestTelUriValidationStep::CTestTelUriValidationStep()
	{
	//Call base class method to set human readable name for test step
	SetTestStepName(KTestTelUriValidationStep);
	}

/**
  Destructor
  @internalTechnology
  @test
*/
CTestTelUriValidationStep::~CTestTelUriValidationStep()
	{

	}

/**
  Does the main functionality of a test step.
  Here, reads values from INI file 
  and calls TestTelUriValidate() which does validation of a tel uri.
  @internalTechnology 
  @param		None
  @return		EPass or EFail indicating the success or failure of the test step
*/
TVerdict CTestTelUriValidationStep::doTestStepL()
	{
	__UHEAP_MARK;
	INFO_PRINTF1(_L("\n"));
	// Get necessary information from INI file
	TPtrC telUri;
	TInt expRetCode;

	if(!GetStringFromConfig(ConfigSection(), 	KIniUri, telUri) ||
	   !GetIntFromConfig(ConfigSection(), KIniExpectedRetCode, expRetCode) )
		{
		ERR_PRINTF3(_L("Problem in reading values from ini.			\
						\nExpected fields are: \n%S\n%S\n" ),&KIniUri, &KIniExpectedRetCode );
		SetTestStepResult(EFail);
		}
	else
		{
		// No problem in reading values from INI file. Proceed.
		// Calling TestTelUriValidate which does validation of tel uri and comparing results.
	    TRAPD(err, TestTelUriValidateAndCompareL(telUri,expRetCode));
	    if(err != KErrNone)
		    {
	        ERR_PRINTF1(_L("Test step failed: Leave occured in TestTelUriValidateAndCompareL \n"));
	        SetTestStepResult(EFail);	
		    }
		}
	__UHEAP_MARKEND;
	INFO_PRINTF1(_L("\nTest of TelUri Validation is Executed"));
	return TestStepResult();
	}

/**
  Performs tel uri validation and compares actual & expected result.
  @param		aTelUri A reference to tel-uri to be validated.
  @param		aExpRetCode An expected retrn code to be compared.
*/	
void CTestTelUriValidationStep::TestTelUriValidateAndCompareL(const TDesC16& aTelUri, const TInt aExpRetCode)
	{
    CUri8* cUri8 = UriUtils::CreateUriL(aTelUri);
    const TUriC8& telUri(cUri8->Uri());
	// Validating tel uri
	TInt errCode = telUri.Validate();
    if(errCode != aExpRetCode)
		{
	  	ERR_PRINTF2(_L("Tel URI validation failed %D\n"), errCode);
	    SetTestStepResult(EFail);	
		}
	delete cUri8;
	}

/**
  Constructor: Sets the test step name.
  @internalTechnology
  @test
*/
CTestTelUriValidationOomStep::CTestTelUriValidationOomStep()
	{
	//Call base class method to set human readable name for test step
	SetTestStepName(KTestOomTelUriValidationStep);
	}

/**
  Destructor
  @internalTechnology
  @test
*/
CTestTelUriValidationOomStep::~CTestTelUriValidationOomStep()
	{

	}

/**
  Does the main functionality of a test step and performs OOM test.
  Here, reads values from INI file 
  and calls TestTelUriValidate() which does validation of a tel uri.
  @internalTechnology 
  @param		None
  @return		EPass or EFail indicating the success or failure of the test step
*/
TVerdict CTestTelUriValidationOomStep::doTestStepL()
	{
	__UHEAP_MARK;
	TPtrC telUri;
	if(!GetStringFromConfig(ConfigSection(), 	KIniUri, telUri) )
		{
		ERR_PRINTF2(_L("Problem in reading values from ini.    \
						\nExpected fields are: \n%S\n" ), &KIniUri);
		SetTestStepResult(EFail);
		}
	else
		{
		CUri8* cUri8 = UriUtils::CreateUriL(telUri);
    	const TUriC8& telUri(cUri8->Uri());
		INFO_PRINTF1(_L("Calling telUri.Validate() which does tel-Uri Validation.")); 	
		TInt ret = KErrNoMemory;
		TInt failAt = 0;
		while(ret != KErrNone)
			{
			failAt++;
			INFO_PRINTF2(_L("OOM test step: %d\n"),failAt);

			__UHEAP_SETFAIL( RHeap::EDeterministic, failAt );
			__UHEAP_MARK;
			TRAP( ret, telUri.Validate());
			__UHEAP_MARKEND;
			__UHEAP_RESET;
			if((ret == KErrNoMemory) || (ret == KErrNone))
				{
				INFO_PRINTF2(_L("Error/RetCode : %D\n"),ret);
				}
			}
		delete cUri8;
		}
	__UHEAP_MARKEND;
	INFO_PRINTF1(_L("\n OOM Test of TelUri Validation is Executed"));
	return TestStepResult();
	}


