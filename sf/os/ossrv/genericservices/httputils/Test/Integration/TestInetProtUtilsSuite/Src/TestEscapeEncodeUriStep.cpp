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
// Contains implementation of CTestEscapeEncodeUriStep class
// 
//

/**
 @file
 @internalTechnology 
*/

// User Include
#include "TestEscapeEncodeUriStep.h"
// System Include
#include <escapeutils.h>
/**
  Constructor: Sets the test step name.
  @internalTechnology
  @test
*/
CTestEscapeEncodeUriStep::CTestEscapeEncodeUriStep()
	{
	//Call base class method to set human readable name for test step
	SetTestStepName(KTestEscapeEncodeUriStep);
	}

/**
  Destructor
  @internalTechnology
  @test
*/
CTestEscapeEncodeUriStep::~CTestEscapeEncodeUriStep()
	{

	}

/**
  Does the main functionality of a test step.
  Here, reads values from INI file and calls EscEncodeAndCompareUriL
  @internalTechnology 
  @param		None
  @return		EPass or EFail indicating the success or failure of the test step
*/
TVerdict CTestEscapeEncodeUriStep::doTestStepL()
	{
	__UHEAP_MARK;
	INFO_PRINTF1(_L("\n"));
	// Get necessary information from INI file
	
	TPtrC decodedComponent; // to be encoded.
	TPtrC expEncodedUri;
	TPtrC expEncodedAuthority;
	TPtrC expEncodedPath;
	TPtrC expEncodedQuery;
	TPtrC expEncodedNormal;
	
	if(!GetStringFromConfig(ConfigSection(), KIniEscapeDecoded, decodedComponent) ||
	   !GetStringFromConfig(ConfigSection(), KIniEscapeEncodedUri, expEncodedUri) ||
	   !GetStringFromConfig(ConfigSection(), KIniEscapeEncodedAuthority, expEncodedAuthority) ||
	   !GetStringFromConfig(ConfigSection(), KIniEscapeEncodedPath, expEncodedPath) ||
	   !GetStringFromConfig(ConfigSection(), KIniEscapeEncodedQuery, expEncodedQuery) ||
       !GetStringFromConfig(ConfigSection(), KIniEscapeEncodedNormal, expEncodedNormal)	  )
		{
		ERR_PRINTF7(_L("Problem in reading values from ini.			\
						\nExpected fields are: \n%S\n%S\n%S\n%S\n%S\n%S\n"
					  ),&KIniEscapeDecoded, &KIniEscapeEncodedUri, &KIniEscapeEncodedAuthority, &KIniEscapeEncodedPath, &KIniEscapeEncodedQuery, &KIniEscapeEncodedNormal
				   );
		SetTestStepResult(EFail);
		}
	else
		{// No problem in reading values from INI file. Proceed.
	    TRAPD(err, EscEncodeAndCompareUriL(decodedComponent,expEncodedUri,expEncodedAuthority,expEncodedPath,expEncodedQuery,expEncodedNormal));
	    if(err != KErrNone)
			{
			ERR_PRINTF2(_L("Test step failed: Leave occured in EscEncodeAndCompareUriL fun: %D\n"), err);
			SetTestStepResult(EFail);	
			}
		 }
	__UHEAP_MARKEND;
	INFO_PRINTF1(_L("\nTest of EscapeEncodingUri is Executed\n"));
	return TestStepResult();
	}

/**
  Performs escape encoding in all modes and compares actual & expected result.
  @param		aDecodeComp A descriptor with the data to encode.
  @param		aExpEncodedUrl An expected descriptor for the escape Mode EEscapeUrlEncoded.
  @param		aExpEncodedAut An expected descriptor for the escape Mode EEscapeAuth.
  @param		aExpEncodedPath An expected descriptor for the escape Mode EEscapePath.
  @param		aExpEncodedQuery An expected descriptor for the escape Mode EEscapeQuery.
  @param		aExpEncodedNormal An expected descriptor for the escape Mode EEscapeNormal.
*/	
void CTestEscapeEncodeUriStep::EscEncodeAndCompareUriL(const TDesC& aDecodeComp,const TDesC& aExpEncodedUrl,const TDesC& aExpEncodedAut,const TDesC& aExpEncodedPath,const TDesC& aExpEncodedQuery,const TDesC& aExpEncodedNormal)
	{
	HBufC8* decodeBuf = HBufC8::NewLC(aDecodeComp.Length());
	TPtr8 decode8Bit(decodeBuf->Des());
	decode8Bit.Copy(aDecodeComp);
	//Authority mode Encoding 
	TestComponentEncodingL(decode8Bit, aExpEncodedAut, EscapeUtils::EEscapeAuth );
     //Path mode Encoding
    TestComponentEncodingL(decode8Bit, aExpEncodedPath, EscapeUtils::EEscapePath );
    //Query mode Encoding
    TestComponentEncodingL(decode8Bit, aExpEncodedQuery, EscapeUtils::EEscapeQuery );
    //Normal mode Encoding
    TestComponentEncodingL(decode8Bit, aExpEncodedNormal, EscapeUtils::EEscapeNormal );
    //Url mode Encoding
    TestComponentEncodingL(decode8Bit, aExpEncodedUrl, EscapeUtils::EEscapeUrlEncoded );
    
    CleanupStack::PopAndDestroy(decodeBuf);
    }
	
/**
  Performs escape encoding the mode specifed and compares actual & expected result.
  @param		aDecode8Bit A descriptor with the data to encode.
  @param		aExpEncodedComponent An expected descriptor for the escape mode specified.
  @param		aMode An enum specifying the escape mode.
*/
void CTestEscapeEncodeUriStep::TestComponentEncodingL(const TDesC8& aDecode8Bit, const TDesC& aExpEncodedComponent, EscapeUtils::TEscapeMode aMode )
	{
	//Component Encoding 
	HBufC8* expEncodedComponentBuf = HBufC8::NewLC(aExpEncodedComponent.Length());
	TPtr8 expEncodedComponent8Bit(expEncodedComponentBuf->Des());
	expEncodedComponent8Bit.Copy(aExpEncodedComponent);

    HBufC8* encodedComponent=EscapeUtils::EscapeEncodeL(aDecode8Bit,aMode);
    if(expEncodedComponent8Bit.Compare(*encodedComponent)!=0)
	    {
	    switch (aMode)
		    {
		    case EscapeUtils::EEscapeAuth:
		    	{
		    	ERR_PRINTF1(_L("Test step failed: EEscapeAuth - EscapeEncoding done wrongly.\n"));
		    	break;
		    	}
		    case EscapeUtils::EEscapePath:
		    	{
		    	ERR_PRINTF1(_L("Test step failed: EEscapePath - EscapeEncoding done wrongly.\n"));
		    	break;
		    	}
		    case EscapeUtils::EEscapeQuery:
		    	{
		    	ERR_PRINTF1(_L("Test step failed: EEscapeQuery - EscapeEncoding done wrongly.\n"));
		    	break;
		    	}
		    case EscapeUtils::EEscapeNormal:
		    	{
		    	ERR_PRINTF1(_L("Test step failed: EEscapeNormal - EscapeEncoding done wrongly.\n"));
		    	break;
		    	}
		    case EscapeUtils::EEscapeUrlEncoded:
		    	{
		    	ERR_PRINTF1(_L("Test step failed: EEscapeUrlEncoded - EscapeEncoding done wrongly.\n"));
		    	break;
		    	}
		    default:
		    	{
		    	//do nothing
		    	break;	
		    	}
		    }
	    
		SetTestStepResult(EFail);
	    }
    delete encodedComponent;
    CleanupStack::PopAndDestroy(expEncodedComponentBuf);	
	}


