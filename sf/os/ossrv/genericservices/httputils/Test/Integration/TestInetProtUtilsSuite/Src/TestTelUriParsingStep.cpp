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
// Contains implementation of CTestTelUriParsingStep class
// 
//

/**
 @file
 @internalTechnology 
*/

// User Include
#include "TestTelUriParsingStep.h"
// System Include
#include <uri8.h>
#include <uriutilscommon.h>

/**
  Constructor: Sets the test step name.
  @internalTechnology
  @test
*/
CTestTelUriParsingStep::CTestTelUriParsingStep()
	{
	//Call base class method to set human readable name for test step
	SetTestStepName(KTestTelUriParsingStep);
	}

/**
  Destructor
  @internalTechnology
  @test
*/
CTestTelUriParsingStep::~CTestTelUriParsingStep()
	{

	}

/**
  Does the main functionality of a test step.
  Here, reads values from INI file 
  and calls TestTelUriParsingAndCompareL() which does parsing of a tel uri.
  @internalTechnology 
  @param		None
  @return		EPass or EFail indicating the success or failure of the test step
*/
TVerdict CTestTelUriParsingStep::doTestStepL()
	{
	__UHEAP_MARK;
	INFO_PRINTF1(_L("\n"));
	// Get necessary information from INI file
	TPtrC telUri;
	TPtrC expTelScheme;
	TPtrC expTelPath;
	
	if(!GetStringFromConfig(ConfigSection(), 	KIniUri, telUri) ||
	   !GetStringFromConfig(ConfigSection(), KIniExpectedTelScheme, expTelScheme) ||
	   !GetStringFromConfig(ConfigSection(), KIniExpectedTelPath, expTelPath) )
		{
		ERR_PRINTF4(_L("Problem in reading values from ini.			\
						\nExpected fields are: \n%S\n%S\n%S\n"
					  ),&KIniUri, &KIniExpectedTelScheme, &KIniExpectedTelPath
				   );
		SetTestStepResult(EFail);
		}
	else
		{
		// No problem in reading values from INI file. Proceed.
	    // Calling TestTelUriParsingL which does parisng of tel uri and comparing results.
	    TRAPD(err, TestTelUriParsingAndCompareL(telUri,expTelScheme, expTelPath));
	    if(err != KErrNone)
			{
			ERR_PRINTF2(_L("Test step failed: Tel URI parsing failed: %D\n"), err);
			SetTestStepResult(EFail);	
			}
		}
	__UHEAP_MARKEND;
	INFO_PRINTF1(_L("\nTest of TelUriParsing is Executed\n"));
	return TestStepResult();
	}	

/**
  Performs tel uri parsing and compares actual & expected result.
  @param		aUri A reference to tel-uri to be parsed.
  @param		aScheme A reference to expected scheme component of tel-uri.
  @param		aPath A reference to expected path component of tel-uri.
*/	
void CTestTelUriParsingStep::TestTelUriParsingAndCompareL(const TDesC16& aUri, const TDesC16& aScheme, const TDesC16& aPath)
	{
	// Make 8-bit copies
	HBufC8* uriBuf = HBufC8::NewLC(aUri.Length());
	TPtr8 uri8Bit(uriBuf->Des());
	uri8Bit.Copy(aUri);

	TUriParser8 uriParser;
	TInt error = KErrNone;
	error = uriParser.Parse(uri8Bit);
	// Is this a valid Uri?
	if( error != KErrNone )
		{
        ERR_PRINTF2(_L("Tel URI parsing failed: %D\n"), error);
	    SetTestStepResult(EFail);
		User::LeaveIfError(error);
		}

	// Check scheme...
	TInt result = TestComponentL(uriParser, aScheme, EUriScheme);
	if( result != 0 )
		{
		ERR_PRINTF1(_L("Scheme component not matched \n"));
		User::LeaveIfError(KUriUtilsErrDifferentScheme);
		}

	// Check path...
	result = TestComponentL(uriParser, aPath, EUriPath);
	if( result != 0)
		{
		ERR_PRINTF1(_L("Path component not matched \n"));
		User::LeaveIfError(KUriUtilsErrDifferentPath);
		}
		
	//tel-uri Should not contain Host, UserInfo, Port, Query and Fragment components.
	// Check host...
	const TDesC8& host = uriParser.Extract(EUriHost);
	if( (host.Length() != 0) || (host.Size() != 0) )
		{
	    ERR_PRINTF1(_L("host component is not empty, but it should be empty. \n"));
	    User::LeaveIfError(KUriUtilsErrDifferentHost);
		}

	//Check UserInfo...
	const TDesC8& userInfo = uriParser.Extract(EUriUserinfo);
	if( (userInfo.Length() != 0) || (userInfo.Size() != 0) )
		{
	    ERR_PRINTF1(_L("UserInfo component is not empty, but it should be empty. \n"));
	    User::LeaveIfError(KUriUtilsErrDifferentUserInfo);
		}
		
	//Check port...
    const TDesC8& uriPort = uriParser.Extract(EUriPort);
	if( (uriPort.Length() != 0) || (uriPort.Size() != 0) )
		{
	    ERR_PRINTF1(_L("Port component is not empty, but it should be empty. \n"));
	    User::LeaveIfError(KUriUtilsErrDifferentPort);
		}

    //Check query...
    const TDesC8& uriQuery = uriParser.Extract(EUriQuery);
	if( (uriQuery.Length() != 0) || (uriQuery.Size() != 0) )
		{
	    ERR_PRINTF1(_L("Query component is not empty, but it should be empty. \n"));
	    User::LeaveIfError(KUriUtilsErrDifferentQuery);
		}
	
	//Check fragment...
	const TDesC8& uriFrag = uriParser.Extract(EUriFragment);
	if( (uriFrag.Length() != 0) || (uriFrag.Size() != 0) )
		{
	    ERR_PRINTF1(_L("Fragment component is not empty, but it should be empty. \n"));
	    User::LeaveIfError(KUriUtilsErrDifferentFragment);
		}
	CleanupStack::PopAndDestroy(uriBuf); 
	}

/**
  It extracts and compares with expected component for specified TUriComponent.
  @param		aParser A reference to parsed tel-uri.
  @param		aExpectedComponent A reference to expected component of tel-uri.
  @param		aComponent Enumeration of TUriComponent.
*/
TInt CTestTelUriParsingStep::TestComponentL(const TUriParser8& aParser, const TDesC16& aExpectedComponent,TUriComponent aComponent)
	{
	HBufC8* expectedBuf = HBufC8::NewLC(aExpectedComponent.Length());
	TPtr8 expected8Bit = expectedBuf->Des();
	expected8Bit.Copy(aExpectedComponent);
	TInt result = aParser.Extract(aComponent).Compare(expected8Bit);
	CleanupStack::PopAndDestroy(expectedBuf);
	return result;
	}



