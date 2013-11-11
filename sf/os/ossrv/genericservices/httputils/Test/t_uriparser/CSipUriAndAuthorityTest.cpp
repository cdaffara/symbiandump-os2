// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

#include "CSipUriAndAuthorityTest.h"
#include "CUriAndAuthorityTest.h"
#include <uriutilscommon.h>
#include "SipUriAndAuthorityTests.h"

#include <e32base.h>
#include <f32file.h>

CSipUriAndAuthorityTest* CSipUriAndAuthorityTest::NewLC(CIpuTestHarness* aTestHarness)
	{
	CSipUriAndAuthorityTest* self = new (ELeave) CSipUriAndAuthorityTest(aTestHarness);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CSipUriAndAuthorityTest* CSipUriAndAuthorityTest::NewL(CIpuTestHarness* aTestHarness)
	{
	CSipUriAndAuthorityTest* self = CSipUriAndAuthorityTest::NewLC(aTestHarness);
	CleanupStack::Pop(self);
	return self;
	}

CSipUriAndAuthorityTest::CSipUriAndAuthorityTest(CIpuTestHarness* aTestHarness)
: iTestHarness(aTestHarness)
	{
	}

void CSipUriAndAuthorityTest::ConstructL()
	{
	}

CSipUriAndAuthorityTest::~CSipUriAndAuthorityTest()
	{
	}

//
//
//	SIP Uri tests
//
//
void CSipUriAndAuthorityTest::DoTestsL()
	{
	//allredy pushed
	CUriAndAuthorityTest* uriAndAuthorityTest = CUriAndAuthorityTest::NewLC(iTestHarness);

	// Test Sips TUriParser Component Extraction
	uriAndAuthorityTest->TestUriComponentExtractionL(KCompsSip0, KSchemeSip0, KUserInfoSip0, KHostSip0, KPortSip0, KPathSip0, KQuerySip0, KFragmentSip0);
	uriAndAuthorityTest->TestUriComponentExtractionL(KCompsSip1, KSchemeSip1, KUserInfoSip1, KHostSip1, KPortSip1, KPathSip1, KQuerySip1, KFragmentSip1);
	uriAndAuthorityTest->TestUriComponentExtractionL(KCompsSip2, KSchemeSip2, KUserInfoSip2, KHostSip2, KPortSip2, KPathSip2, KQuerySip2, KFragmentSip2);
	uriAndAuthorityTest->TestUriComponentExtractionL(KCompsSip3, KSchemeSip3, KUserInfoSip3, KHostSip3, KPortSip3, KPathSip3, KQuerySip3, KFragmentSip3);
	uriAndAuthorityTest->TestUriComponentExtractionL(KCompsSip4, KSchemeSip4, KUserInfoSip4, KHostSip4, KPortSip4, KPathSip4, KQuerySip4, KFragmentSip4);
	uriAndAuthorityTest->TestUriComponentExtractionL(KCompsSip5, KSchemeSip5, KUserInfoSip5, KHostSip5, KPortSip5, KPathSip5, KQuerySip5, KFragmentSip5);		
	
	// Sip URI construction test
	uriAndAuthorityTest->TestUriSetComponentL(KSipUri_Scheme0, KSipUri_Host0, KSipUri_Userinfo0, KSipUri_Port0, KSipUri_Path0, KSipUri_Query0, KSipUri_Fragment0,
		KSipUri_SchemeComp0, KSipUri_HostComp0, KSipUri_UserinfoComp0, KSipUri_PortComp0, KSipUri_PathComp0, KSipUri_QueryComp0, KSipUri_FragmentComp0);
	
/** No Fragment part exist in Sip Uri's as per RFC3261
	// Sip URI destruction test
	uriAndAuthorityTest->TestUriRemoveComponentL(KSipUri_NoScheme0, KSipUri_NoHost0, KSipUri_NoUserinfo0, KSipUri_NoPort0, KSipUri_NoPath0,
		KSipUri_NoQuery0, KSipUri_NoFragment0, KSipUri_Whole0);
*/
	// Same test as above, but with an IPv6 format host
	// Sip URI construction test
	uriAndAuthorityTest->TestUriSetComponentL(KSipUri_Scheme0, KSipUri_IPv6Host0, KSipUri_IPv6UserInfo0, KSipUri_IPv6Port0, KSipUri_IPv6Path0, KSipUri_IPv6Query0, KSipUri_IPv6Fragment0,
		KSipUri_SchemeComp0, KUri_IPv6HostComp, KSipUri_UserinfoComp0, KSipUri_PortComp0, KSipUri_PathComp0, KSipUri_QueryComp0, KSipUri_FragmentComp0);

/** No Fragment part exist in Sip Uri's as per RFC3261
	// Sip URI destruction test
	uriAndAuthorityTest->TestUriRemoveComponentL(KSipUri_IPv6NoScheme0, KSipUri_IPv6NoHost0, KSipUri_IPv6NoUserinfo0, KSipUri_IPv6NoPort0, KSipUri_IPv6NoPath0,
		KSipUri_IPv6NoQuery0, KSipUri_IPv6NoFragment0, KSipUri_IPv6Whole0);
*/	
	CleanupStack::PopAndDestroy(uriAndAuthorityTest);
	
	//Sip URI validation
	TInt error=KErrNone;
	error=OpenSipURIFile();
	if (KErrNone!=error)
		{
		iTestHarness->StartTestL(_L("Test TUriParser Component "));
		SipLogError(error, KCompsSipUriFileSystemError);
		}
	else
		{
		error=SipUriValidationL();		
	   	if (KErrNone!=error)
			{
			iTestHarness->StartTestL(_L("Test TUriParser Component Validation "));
			switch (error)
				{
			case KErrNotFound:
				{
				iTestHarness->LogIt(KCompsSipUriNoValidate);	
				}
				break;
			case KErrBadName:
				{
				iTestHarness->LogIt(KCompsSipUriNoValidateData);
				}
				break;
			default:
				break;
				}
			iTestHarness->EndTest(KErrNone);	
			}	
		
		if (iFoundEquivalence)
			{
			error=SipUriEquivalenceL();
			//No Sip URI Equivalence data
			if (KErrBadName==error)
				{
				iTestHarness->StartTestL(_L("Test TUriParser Component Equivalence "));
				iTestHarness->LogIt(KCompsSipUriNoEquivalenceData);
				iTestHarness->EndTest(KErrNone);
				}
			}
		else
			{
			iTestHarness->StartTestL(_L("Test TUriParser Component Equivalence "));
			iTestHarness->LogIt(KCompsSipUriNoEquivalence);
			iTestHarness->EndTest(KErrNone);
			error=KErrNone;
			}
		iFsSession.Close();
		}
	}
	
//
//
//	Open SIP Uri data file
//
//
TInt CSipUriAndAuthorityTest::OpenSipURIFile()
	{
	RFile file;
	TInt error=KErrNone;

	error = iFsSession.Connect();
	if (KErrNone!=error)
		{
		return error;			
		}
		
	error = file.Open(iFsSession, KCompsSipUriFileName, EFileRead);
	if (KErrNone!=error)
		{
		iFsSession.Close();
		return error;			
		}

	iTUriParserFile.Set(file);
	return error;
	}
	
//
//
//	SIP Uri Logger
//
//
void CSipUriAndAuthorityTest::SipLogError(TInt aError, const TDesC16& aSipUri) const
	{
	TBool wrongError=EFalse;
	
	//compare the Equivalent SIP URI to look for the correct error, these SIP URIs must be in T_UriParserSipUri.txt
	//All equal
	if (0==aSipUri.Compare(KCompsSipUriEqual0) && KErrNone != aError 
		|| 0==aSipUri.Compare(KCompsSipUriEqual1) && KErrNone != aError 
		|| 0==aSipUri.Compare(KCompsSipUriEqual2) && KErrNone != aError)
		{
		iTestHarness->LogIt(KCompsSipUriValidMess0);
		wrongError=ETrue;
		}
	//All not equal		
	if (0==aSipUri.Compare(KCompsSipUriNotEqual0) && KUriUtilsErrDifferentUserInfo == aError 
		|| 0==aSipUri.Compare(KCompsSipUriNotEqual1) && KUriUtilsErrDifferentPath  == aError 
		|| 0==aSipUri.Compare(KCompsSipUriNotEqual2) && KUriUtilsErrDifferentPort  == aError )
		{
		iTestHarness->LogIt(KCompsSipUriEqualMess1);
		}

	//compare the SIP URIs to look for the correct error returned by CUri for validation
	if (0==aSipUri.Compare(KCompsSipUriNotEqual1) && KUriUtilsErrDifferentPath  != aError )
		{
		iTestHarness->LogIt(KCompsSipUriInvalidMess9);
		wrongError=ETrue;
		}


	if (0==aSipUri.Compare(KCompsSipUriValid0) && KErrNone!= aError
		|| 0==aSipUri.Compare(KCompsSipUriValid1) && KErrNone!= aError)
		{
		iTestHarness->LogIt(KCompsSipUriValidMess0);
		wrongError=ETrue;
		}

	if (0==aSipUri.Compare(KCompsSipUriInvalid0) && KErrNotSupported != aError)
		{
		iTestHarness->LogIt(KCompsSipUriInvalidMess0);
		wrongError=ETrue;
		}
	
	if( 0==aSipUri.Compare(KCompsSipUriInvalid5) && KUriUtilsErrInvalidParam!= aError
		|| 0==aSipUri.Compare(KCompsSipUriInvalid6) && KUriUtilsErrInvalidParam!= aError)
		{
		iTestHarness->LogIt(KCompsSipUriInvalidMess1);
		wrongError=ETrue;
		}
	
	if (0==aSipUri.Compare(KCompsSipUriInvalid3) && KUriUtilsErrInvalidHost!= aError)
		{
		iTestHarness->LogIt(KCompsSipUriInvalidMess2);
		wrongError=ETrue;
		}
		
	if (0==aSipUri.Compare(KCompsSipUriInvalid2) && KUriUtilsErrInvalidHeaders != aError)
		{
		iTestHarness->LogIt(KCompsSipUriInvalidMess8);
		wrongError=ETrue;
		}

	if (0==aSipUri.Compare(KCompsSipUriInvalid1) && KUriUtilsErrInvalidPort != aError
		|| 0==aSipUri.Compare(KCompsSipUriInvalid4) && KUriUtilsErrInvalidPort!= aError
		|| 0==aSipUri.Compare(KCompsSipUriNotEqual2) && KUriUtilsErrDifferentPort  != aError)
		{
		iTestHarness->LogIt(KCompsSipUriInvalidMess3);
		wrongError=ETrue;
		}

	if (0==aSipUri.Compare(KCompsSipUriInvalid7) && KUriUtilsErrInvalidUserInfo!= aError
		|| 0==aSipUri.Compare(KCompsSipUriNotEqual0) && KUriUtilsErrDifferentUserInfo != aError)
		{
		iTestHarness->LogIt(KCompsSipUriInvalidMess7);
		wrongError=ETrue;
		}

	if (wrongError)
		{
		iTestHarness->LogIt(KCompsSipUriInvalidMess5);
		}
	
	LogErrorMessage(aSipUri, aError);
	
	if (wrongError)
		{
		iTestHarness->EndTest(aError);
		}		
	else
		{
		iTestHarness->EndTest(KErrNone);	
		}
	}

//
//
//	SIP Uri Validation Test
//
//
TInt CSipUriAndAuthorityTest::SipUriValidationL()
	{
	TBuf<256> data;
	TInt error=KErrNone;
	iFoundEquivalence=EFalse;
	
	//read the Valid section name in
	error=iTUriParserFile.Read(data);
	while(0!=data.Compare(KCompsSipUriValidate) && KErrNone==error)
		{
		if (0==data.Compare(KCompsSipUriEquivalence))
			{
			iFoundEquivalence=ETrue;
			break;
			}
		data.Zero();
		error=iTUriParserFile.Read(data);
		}
		
	//No Validate Section, KErrEof
	if (0!=data.Compare(KCompsSipUriValidate))
		{
		return KErrNotFound;
		}
	
	//get the validation Data
	error=iTUriParserFile.Read(data);
	
	//no data
	if (KErrEof==error)
		{
		return KErrBadName;
		}	
	
	//found Equivalence
	if (0==data.Compare(KCompsSipUriEquivalence))
		{
		iFoundEquivalence=ETrue;
		return KErrBadName;
		}	

	while (KErrNone==error)
		{
		//check for comments
		while(data.Left(2)==(KCompsSipUriComment))
			{
			data.Zero();
			iTUriParserFile.Read(data);
			}
		if (0==data.Compare(KCompsSipUriEquivalence))
			{
			iFoundEquivalence=ETrue;
			break;
			}
		
		//validate the 16 bit Sip URI first
		iTestHarness->StartTestL(_L("Test TUriParser Component Validation (16-bit  SIP URI)"));
		iTestHarness->LogIt(_L("Uri - %S"), &data);
		error = DoSipUriValidation <TUriParser16>(data);
		SipLogError(error, data);

		// Make 8-bit copies and validate this SIP URI
		HBufC8* uriBuf = HBufC8::NewLC(data.Length());
		TPtr8 uri8Bit = uriBuf->Des();
		uri8Bit.Copy(data);

		iTestHarness->StartTestL(_L("Test TUriParser Component Validation (8-bit  SIP URI)"));	
		iTestHarness->LogIt(_L("Uri - %S"), &data);
		error = DoSipUriValidation <TUriParser8>(uri8Bit);
		SipLogError(error, data);
		
		data.Zero();
		CleanupStack::PopAndDestroy(1, uriBuf);// uriBuf
		
		error=iTUriParserFile.Read(data);
		}
	return KErrNone;		
	}

//
//
//	SIP Uri Equivalence Test
//
//
TInt CSipUriAndAuthorityTest::SipUriEquivalenceL()
	{
	TBuf<256> valueA;
	TBuf<256> valueB;
	TBool sipUriError=EFalse;
	TInt errorA=KErrNone;
	TInt errorB=KErrNone;
	TInt error=KErrNone;
	
	//get the Equivalence Data
	errorA=iTUriParserFile.Read(valueA);
	while(valueA.Left(2)==(KCompsSipUriComment) && KErrNone==errorA)
		{
		valueA.Zero();
		errorA=iTUriParserFile.Read(valueA);
		}
	
	errorB=iTUriParserFile.Read(valueB);	
	//no Equivalence Data
	if (KErrEof==errorA || KErrEof==errorB)
		{
		return KErrBadName;
		}	
	
	//read the Equivalence Sip URI
	while (KErrNone==errorA  && KErrNone==errorB)
		{
		//check for comments	
		while(valueA.Left(2)==(KCompsSipUriComment))
			{
			valueA.Zero();
			errorA=iTUriParserFile.Read(valueA);
			}
		while(valueB.Left(2)==(KCompsSipUriComment))
			{
			valueB.Zero();
			errorB=iTUriParserFile.Read(valueB);
			}
		
		//validate the 16 bit Sip URI first
		iTestHarness->StartTestL(_L("Test TUriParser Component Equivalence (16-bit  SIP)"));	
		iTestHarness->LogIt(_L("Uri - %S"), &valueA);
		iTestHarness->LogIt(_L("Uri - %S"), &valueB);
		
		error= DoSipUriEquivalence<TUriParser16>(valueA, valueB, sipUriError);
		//check for parsing errors on both Sip URIs
		if (KErrBadSipUriA==sipUriError)
			{
			SipLogError(error, valueA);
			}
		if (KErrBadSipUriB==sipUriError)
			{
			SipLogError(error, valueB);
			}
		SipLogError(error, valueA);
		
		// Make 8-bit copies and validate this SIP URI
		HBufC8* uriBufA = HBufC8::NewLC(valueA.Length());
		TPtr8 uri8BitA = uriBufA->Des();
		uri8BitA.Copy(valueA);

		HBufC8* uriBufB = HBufC8::NewLC(valueB.Length());
		TPtr8 uri8BitB = uriBufB->Des();
		uri8BitB.Copy(valueB);

		iTestHarness->StartTestL(_L("Test TUriParser Component Equivalence (8-bit  SIP)"));	
		iTestHarness->LogIt(_L("Uri - %S"), &valueA);
		iTestHarness->LogIt(_L("Uri - %S"), &valueB);
		error = DoSipUriEquivalence<TUriParser8>(uri8BitA, uri8BitB, sipUriError);
		//check for parsing errors on both Sip URIs
		if (KErrBadSipUriA==sipUriError)
			{
			SipLogError(error, valueA);
			}
		if (KErrBadSipUriA==sipUriError)
			{
			SipLogError(error, valueB);
			}
		SipLogError(error, valueA);
		
		CleanupStack::PopAndDestroy(2, uriBufA);// uriBufA, uriBufB
		
		errorA=iTUriParserFile.Read(valueA);
		errorB=iTUriParserFile.Read(valueB);
		}
	return KErrNone;
	}

//
//
//	SIP Uri Logger
//
//
void CSipUriAndAuthorityTest::LogErrorMessage(const TDesC16& aSipUri, const TInt aError)const
	{
	switch (aError)
		{
		case KErrNotFound:
			iTestHarness->LogIt(aSipUri);
		        break;
		        
		case KErrNotSupported:
			iTestHarness->LogIt(_L("Not Supported"));
		        break;
		        
		case KUriUtilsErrInvalidScheme:
		case KUriUtilsErrDifferentScheme:
			iTestHarness->LogIt(_L("Invalid Scheme"));	
		        break;
		        
		case KUriUtilsErrInvalidUserInfo:
		case KUriUtilsErrDifferentUserInfo:
			iTestHarness->LogIt(_L("Invalid UserInfo"));	
		        break;
		        
		case KUriUtilsErrInvalidHost:
		case KUriUtilsErrDifferentHost:
			iTestHarness->LogIt(_L("Invalid Host"));	
		        break;
		        
		case KUriUtilsErrInvalidPort:
		case KUriUtilsErrDifferentPort:
			iTestHarness->LogIt(_L("Invalid Port"));	
		        break;
		        
		case KUriUtilsErrInvalidPath:
		case KUriUtilsErrDifferentPath:
			iTestHarness->LogIt(_L("Invalid Path"));	
		        break;
		        
		case KUriUtilsErrInvalidParam:
			iTestHarness->LogIt(_L("Invalid Parameter"));	
		        break;
		        
		case KUriUtilsErrInvalidQuery:
		case KUriUtilsErrDifferentQuery:
			iTestHarness->LogIt(_L("Invalid Query"));	
		        break;
		        
		case KUriUtilsErrInvalidHeaders:
			iTestHarness->LogIt(_L("Invalid Headers"));	
		        break;
		        
		case KUriUtilsErrInvalidFragment:
		case KUriUtilsErrDifferentFragment:
			iTestHarness->LogIt(_L("Invalid Fragment"));	
			break;
			
		default:
			break;
		}
		
	//log correct message for equivalence
	if (iFoundEquivalence && KErrNone==aError)
		{
		iTestHarness->LogIt(KCompsSipUriEqualMess0);
		}
	else if (iFoundEquivalence && KErrNone!=aError)
		{
		iTestHarness->LogIt(KCompsSipUriEqualMess1);
		}
	}

//
//
//	Implementation of LOCAL functions
//
//

template<class TUriParserType, class TDesCType>
TInt DoSipUriValidation(const TDesCType& aSipUri)
	{
	TUriParserType uriParser;
	TInt error = uriParser.Parse(aSipUri);
	// Is this a valid SIP Uri?
	if( KUriUtilsErrInvalidUri == error)
		{
		return error;
		}

	//Validate the Sip URI
	return(uriParser.Validate());
	}

template<class TUriParserType, class TDesCType>
TInt DoSipUriEquivalence(const TDesCType& aSipUriA, const TDesCType& aSipUriB,  TInt &aSipUriError)
	{
	TUriParserType uriParserA;
	TUriParserType uriParserB;

	TBool errorA = uriParserA.Parse(aSipUriA);
	// Is this a valid SIP Uri?
	if( KUriUtilsErrInvalidUri == errorA)
		{
		aSipUriError=KErrBadSipUriA;
		return errorA;
		}
		
	TBool errorB = uriParserB.Parse(aSipUriB);
	// Is this a valid SIP Uri?
	if( KUriUtilsErrInvalidUri == errorB)
		{
		aSipUriError=KErrBadSipUriB;
		return errorB;
		}
	
	return(uriParserA.Equivalent(uriParserB));
	}

//End of file
