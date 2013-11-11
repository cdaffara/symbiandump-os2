// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "CUriUtilsTest.h"

#include <e32base.h>
#include <uriutils.h>

#include "UriUtilsTests.h"

CUriUtilsTest* CUriUtilsTest::NewLC(CIpuTestHarness* aTestHarness)
	{
	CUriUtilsTest* self = new (ELeave) CUriUtilsTest(aTestHarness);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CUriUtilsTest* CUriUtilsTest::NewL(CIpuTestHarness* aTestHarness)
	{
	CUriUtilsTest* self = CUriUtilsTest::NewLC(aTestHarness);
	CleanupStack::Pop(self);
	return self;
	}

CUriUtilsTest::CUriUtilsTest(CIpuTestHarness* aTestHarness)
: iTestHarness(aTestHarness)
	{
	}

void CUriUtilsTest::ConstructL()
	{
	}

CUriUtilsTest::~CUriUtilsTest()
	{
	}

void CUriUtilsTest::DoTestsL()
	{
	TestUriUtilsConvertToInternetFormL(KDisplayForm0, KInternetForm0);
	TestUriUtilsConvertToDisplayFormDeprecatedL(KInternetForm0, KDisplayForm0);
	TestUriUtilsConvertToDisplayFormL(KInternetForm0, KValidDisplayForm0);

	TestUriUtilsConvertToInternetFormL(KDisplayForm1, KInternetForm1);
	TestUriUtilsConvertToDisplayFormDeprecatedL(KInternetForm1, KDisplayForm1);
	TestUriUtilsConvertToDisplayFormL(KInternetForm1, KValidDisplayForm1);

	TestUriUtilsHasInvalidCharsL(KInvalidChars0, KInvalid0);
	TestUriUtilsHasInvalidCharsL(KInvalidChars1, KInvalid0);
	TestUriUtilsHasInvalidCharsL(KInvalidChars2, KInvalid0);
	TestUriUtilsHasInvalidCharsL(KInvalidChars3, KInvalid0);
	TestUriUtilsHasInvalidCharsL(KInvalidChars4, KInvalid0);
	TestUriUtilsHasInvalidCharsL(KInvalidChars5, KInvalid0);
	TestUriUtilsHasInvalidCharsL(KInvalidChars6, KInvalid0);
	TestUriUtilsHasInvalidCharsL(KInvalidChars7, KInvalid0);
	TestUriUtilsHasInvalidCharsL(KInvalidChars8, KInvalid0);
	TestUriUtilsHasInvalidCharsL(KInvalidChars9, KInvalid0);
	TestUriUtilsHasInvalidCharsL(KInvalidChars10, KInvalid0);
	TestUriUtilsHasInvalidCharsL(KInvalidChars11, KInvalid0);
 
	TestUriUtilsHasInvalidCharsL(KInvalidChars12, KInvalid1);
	TestUriUtilsHasInvalidCharsL(KInvalidChars13, KInvalid1);
	TestUriUtilsHasInvalidCharsL(KInvalidChars14, KInvalid1);
	TestUriUtilsHasInvalidCharsL(KInvalidChars15, KInvalid1);
	TestUriUtilsHasInvalidCharsL(KInvalidChars16, KInvalid1);
	TestUriUtilsHasInvalidCharsL(KInvalidChars17, KInvalid1);
	TestUriUtilsHasInvalidCharsL(KInvalidChars18, KInvalid1);
	TestUriUtilsHasInvalidCharsL(KInvalidChars19, KInvalid1);
	TestUriUtilsHasInvalidCharsL(KInvalidChars20, KInvalid1);
	TestUriUtilsHasInvalidCharsL(KInvalidChars21, KInvalid1);
	TestUriUtilsHasInvalidCharsL(KInvalidChars22, KInvalid1);
	TestUriUtilsHasInvalidCharsL(KInvalidChars23, KInvalid1);
	TestUriUtilsHasInvalidCharsL(KInvalidChars24, KInvalid1);
	TestUriUtilsHasInvalidCharsL(KInvalidChars25, KInvalid1);
	TestUriUtilsHasInvalidCharsL(KInvalidChars26, KInvalid1);
	TestUriUtilsHasInvalidCharsL(KInvalidChars27, KInvalid1);
	TestUriUtilsHasInvalidCharsL(KInvalidChars28, KInvalid1);


	//
	// Tests for 2.9.1.3 Host-Type checking

	// All these are IPv6 tests
	TestUriUtilsHostTypeL(KHostTypeIPv6Format1, UriUtils::EIPv6Host);			   
	TestUriUtilsHostTypeL(KHostTypeIPv6Format2, UriUtils::EIPv6Host);
	TestUriUtilsHostTypeL(KHostTypeIPv6Format3, UriUtils::EIPv6Host);
	TestUriUtilsHostTypeL(KHostTypeIPv6Format4, UriUtils::EIPv6Host);
	TestUriUtilsHostTypeL(KHostTypeIPv6Format5, UriUtils::EIPv6Host);
	TestUriUtilsHostTypeL(KHostTypeIPv6Format6, UriUtils::EIPv6Host);
	TestUriUtilsHostTypeL(KHostTypeIPv6Format7, UriUtils::EIPv6Host);

	TestUriUtilsHostTypeL(KHostTypeIPv4Format, UriUtils::EIPv4Host);

	// Invalid format IPv4 hosts should be considered Text hosts
	TestUriUtilsHostTypeL(KHostTypeIPv4InvalidFormat1, UriUtils::ETextHost);
	TestUriUtilsHostTypeL(KHostTypeIPv4InvalidFormat2, UriUtils::ETextHost);

	TestUriUtilsHostTypeL(KHostTypeTextFormat1, UriUtils::ETextHost);
	TestUriUtilsHostTypeL(KHostTypeTextFormat2, UriUtils::ETextHost);
	//
	//
	}

void CUriUtilsTest::TestUriUtilsConvertToInternetFormDeprecatedL(const TDesC& aData, const TDesC& aExpected)
	{
	// Make 8-bit copy
	HBufC8* expectedBuf = HBufC8::NewLC(aExpected.Length());
	TPtr8 expected8Bit = expectedBuf->Des();
	expected8Bit.Copy(aExpected);

	iTestHarness->StartTestL(_L("Test UriUtils ConvertToInternetForm (deprecated)"));

	TUriParser16 parser;
	parser.Parse(aData);
	CUri8* uri = NULL;
	TRAPD(error, uri = UriUtils::ConvertToInternetFormL(parser));
	if( error == KErrNone )
		{
		if( uri->Uri().UriDes().Compare(expected8Bit) != 0 )
			{
			error = KErrNotFound;
			}
		delete uri;
		}
	iTestHarness->EndTest(error);

	CleanupStack::PopAndDestroy(expectedBuf);	// expectedBuf
	}
void CUriUtilsTest::TestUriUtilsConvertToInternetFormL(const TDesC& aData, const TDesC& aExpected)
	{
	// Make 8-bit copy
	HBufC8* expectedBuf = HBufC8::NewLC(aExpected.Length());
	TPtr8 expected8Bit = expectedBuf->Des();
	expected8Bit.Copy(aExpected);

	iTestHarness->StartTestL(_L("Test UriUtils ConvertToInternetForm"));

	CUri8* uri = NULL;
	TRAPD(error, uri = UriUtils::CreateUriL(aData));
	if( error == KErrNone )
		{
		if( uri->Uri().UriDes().Compare(expected8Bit) != 0 )
			{
			error = KErrNotFound;
			}
		delete uri;
		}
	iTestHarness->EndTest(error);

	CleanupStack::PopAndDestroy(expectedBuf);	// expectedBuf	
	TestUriUtilsConvertToInternetFormDeprecatedL(aData, aExpected );
	}

void CUriUtilsTest::TestUriUtilsConvertToDisplayFormDeprecatedL(const TDesC& aData, const TDesC& aExpected)
	{
	// Make 8-bit copy
	HBufC8* dataBuf = HBufC8::NewLC(aData.Length());
	TPtr8 data8Bit = dataBuf->Des();
	data8Bit.Copy(aData);

	iTestHarness->StartTestL(_L("Test UriUtils ConvertToInternetForm (deprecated)"));

	TUriParser8 parser;
	parser.Parse(data8Bit);
	CUri16* uri = NULL;
	TRAPD(error, uri = UriUtils::ConvertToDisplayFormL(parser));
	if( error == KErrNone )
		{
		if( uri->Uri().UriDes().Compare(aExpected) != 0 )
			{
			error = KErrNotFound;
			}
		delete uri;
		}
	iTestHarness->EndTest(error);

	CleanupStack::PopAndDestroy(dataBuf);	// dataBuf
	}
	
void CUriUtilsTest::TestUriUtilsConvertToDisplayFormL(const TDesC& aData, const TDesC& aExpected)
	{
	// Make 8-bit copy
	HBufC8* dataBuf = HBufC8::NewLC(aData.Length());
	TPtr8 data8Bit = dataBuf->Des();
	data8Bit.Copy(aData);

	iTestHarness->StartTestL(_L("Test UriUtils ConvertToInternetForm"));

	TUriParser8 parser;
	parser.Parse(data8Bit);
	HBufC* uri = NULL;
	TRAPD(error, uri = parser.DisplayFormL());
	if( error == KErrNone )
		{
		if( uri->Compare(aExpected) != 0 )
			{
			error = KErrNotFound;
			}
		delete uri;
		}
	iTestHarness->EndTest(error);

	CleanupStack::PopAndDestroy(dataBuf);	// dataBuf
	}

void CUriUtilsTest::TestUriUtilsHasInvalidCharsL(const TDesC16& aData, TBool aHasInvalidChars)
	{
	iTestHarness->StartTestL(_L("Test UriUtils HasInvalidChars (16-bit)"));
	
	TBool invalid = UriUtils::HasInvalidChars(aData);

	TInt error = KErrNone;
	if( (!invalid && aHasInvalidChars) | (invalid && !aHasInvalidChars) )
		{
		error = KErrNotFound;
		}
	iTestHarness->EndTest(error);
	}

void CUriUtilsTest::TestUriUtilsHasInvalidCharsL(const TDesC8& aData, TBool aHasInvalidChars)
	{
	iTestHarness->StartTestL(_L("Test UriUtils HasInvalidChars (8-bit)"));
	
	TBool invalid = UriUtils::HasInvalidChars(aData);

	TInt error = KErrNone;
	if( (!invalid && aHasInvalidChars) | (invalid && !aHasInvalidChars) )
		{
		error = KErrNotFound;
		}
	iTestHarness->EndTest(error);
	}

void CUriUtilsTest::TestUriUtilsHostTypeL(const TDesC8& aData, UriUtils::TUriHostType aHostType)
	{
	iTestHarness->StartTestL(_L("Test UriUtils HostType (8-bit)"));
	
	UriUtils::TUriHostType hostType = UriUtils::HostType(aData);

	TInt error = KErrNone;
	if ( hostType != aHostType )
		{
		error = KErrNotFound;	
		}	
	iTestHarness->EndTest(error);
	}
	
void CUriUtilsTest::TestUriUtilsHostTypeL(const TDesC16& aData, UriUtils::TUriHostType aHostType)
	{
	iTestHarness->StartTestL(_L("Test UriUtils HostType (16-bit)"));

	UriUtils::TUriHostType hostType = UriUtils::HostType(aData);

	TInt error = KErrNone;
	if ( hostType != aHostType )
		{
		error = KErrNotFound;
		}	
	iTestHarness->EndTest(error);
	}
