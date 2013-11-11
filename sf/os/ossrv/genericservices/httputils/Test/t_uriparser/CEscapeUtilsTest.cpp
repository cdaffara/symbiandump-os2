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

#include "CEscapeUtilsTest.h"

#include <e32base.h>

#include "EscapeUtilsTestCommon.h"
#include "EscapeUtilsTests.h"
#include <tconvbase64.h>

CEscapeUtilsTest* CEscapeUtilsTest::NewLC(CIpuTestHarness* aTestHarness)
	{
	CEscapeUtilsTest* self = new (ELeave) CEscapeUtilsTest(aTestHarness);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CEscapeUtilsTest* CEscapeUtilsTest::NewL(CIpuTestHarness* aTestHarness)
	{
	CEscapeUtilsTest* self = CEscapeUtilsTest::NewLC(aTestHarness);
	CleanupStack::Pop(self);
	return self;
	}

CEscapeUtilsTest::CEscapeUtilsTest(CIpuTestHarness* aTestHarness)
: iTestHarness(aTestHarness)
	{
	}

void CEscapeUtilsTest::ConstructL()
	{
	}

CEscapeUtilsTest::~CEscapeUtilsTest()
	{
	}

void CEscapeUtilsTest::DoTestsL()
	{
	TestEscapeUtilsEscapeEncodeL(KEscapeDecoded0, KEscapeEncoded0_auth, EscapeUtils::EEscapeAuth);
	TestEscapeUtilsEscapeEncodeL(KEscapeDecoded0, KEscapeEncoded0_path, EscapeUtils::EEscapePath);
	TestEscapeUtilsEscapeEncodeL(KEscapeDecoded0, KEscapeEncoded0_query, EscapeUtils::EEscapeQuery);
	TestEscapeUtilsEscapeEncodeL(KEscapeDecoded0, KEscapeEncoded0_normal, EscapeUtils::EEscapeNormal);
	TestEscapeUtilsEscapeEncodeL(KEscapeDecoded0, KEscapeEncoded0_url, EscapeUtils::EEscapeUrlEncoded);

	TestEscapeUtilsEscapeEncodeL(KEscapeDecoded1, KEscapeEncoded1_auth, EscapeUtils::EEscapeAuth);
	TestEscapeUtilsEscapeEncodeL(KEscapeDecoded1, KEscapeEncoded1_path, EscapeUtils::EEscapePath);
	TestEscapeUtilsEscapeEncodeL(KEscapeDecoded1, KEscapeEncoded1_query, EscapeUtils::EEscapeQuery);
	TestEscapeUtilsEscapeEncodeL(KEscapeDecoded1, KEscapeEncoded1_normal, EscapeUtils::EEscapeNormal);
	TestEscapeUtilsEscapeEncodeL(KEscapeDecoded1, KEscapeEncoded1_url, EscapeUtils::EEscapeUrlEncoded);


	TestEscapeUtilsEscapeDecodeL(KEscapeEncoded0_auth, KEscapeDecoded0);
	TestEscapeUtilsEscapeDecodeL(KEscapeEncoded0_path, KEscapeDecoded0);
	TestEscapeUtilsEscapeDecodeL(KEscapeEncoded0_query, KEscapeDecoded0);
	TestEscapeUtilsEscapeDecodeL(KEscapeEncoded0_normal, KEscapeDecoded0);

	TestEscapeUtilsEscapeDecodeL(KEscapeEncoded1_auth, KEscapeDecoded1);
	TestEscapeUtilsEscapeDecodeL(KEscapeEncoded1_path, KEscapeDecoded1);
	TestEscapeUtilsEscapeDecodeL(KEscapeEncoded1_query, KEscapeDecoded1);
	TestEscapeUtilsEscapeDecodeL(KEscapeEncoded1_normal, KEscapeDecoded1);

	// Test cases for section 2.9.1.2 //

	TestEscapeUtilsEscapeEncodeL(KEscapeDecoded2, KEscapeEncoded2_auth, EscapeUtils::EEscapeAuth);
	TestEscapeUtilsEscapeEncodeL(KEscapeDecoded2, KEscapeEncoded2_path, EscapeUtils::EEscapePath);
	TestEscapeUtilsEscapeEncodeL(KEscapeDecoded2, KEscapeEncoded2_query, EscapeUtils::EEscapeQuery);
	TestEscapeUtilsEscapeEncodeL(KEscapeDecoded2, KEscapeEncoded2_normal, EscapeUtils::EEscapeNormal);
	TestEscapeUtilsEscapeEncodeL(KEscapeDecoded2, KEscapeEncoded2_url, EscapeUtils::EEscapeUrlEncoded);

	TestEscapeUtilsEscapeEncodeL(KEscapeDecoded3, KEscapeEncoded3_auth, EscapeUtils::EEscapeAuth);
	TestEscapeUtilsEscapeEncodeL(KEscapeDecoded3, KEscapeEncoded3_path, EscapeUtils::EEscapePath);
	TestEscapeUtilsEscapeEncodeL(KEscapeDecoded3, KEscapeEncoded3_query, EscapeUtils::EEscapeQuery);
	TestEscapeUtilsEscapeEncodeL(KEscapeDecoded3, KEscapeEncoded3_normal, EscapeUtils::EEscapeNormal);
	TestEscapeUtilsEscapeEncodeL(KEscapeDecoded3, KEscapeEncoded3_url, EscapeUtils::EEscapeUrlEncoded);

	TestEscapeUtilsEscapeEncodeL(KEscapeDecoded4, KEscapeEncoded4_auth, EscapeUtils::EEscapeAuth);
	TestEscapeUtilsEscapeEncodeL(KEscapeDecoded4, KEscapeEncoded4_path, EscapeUtils::EEscapePath);
	TestEscapeUtilsEscapeEncodeL(KEscapeDecoded4, KEscapeEncoded4_query, EscapeUtils::EEscapeQuery);
	TestEscapeUtilsEscapeEncodeL(KEscapeDecoded4, KEscapeEncoded4_normal, EscapeUtils::EEscapeNormal);
	TestEscapeUtilsEscapeEncodeL(KEscapeDecoded4, KEscapeEncoded4_url, EscapeUtils::EEscapeUrlEncoded);

	TestEscapeUtilsEscapeEncodeL(KEscapeDecoded5, KEscapeEncoded5_auth, EscapeUtils::EEscapeAuth);
	TestEscapeUtilsEscapeEncodeL(KEscapeDecoded5, KEscapeEncoded5_path, EscapeUtils::EEscapePath);
	TestEscapeUtilsEscapeEncodeL(KEscapeDecoded5, KEscapeEncoded5_query, EscapeUtils::EEscapeQuery);
	TestEscapeUtilsEscapeEncodeL(KEscapeDecoded5, KEscapeEncoded5_normal, EscapeUtils::EEscapeNormal);
	TestEscapeUtilsEscapeEncodeL(KEscapeDecoded5, KEscapeEncoded5_url, EscapeUtils::EEscapeUrlEncoded);

	TestEscapeUtilsEscapeEncodeL(KEscapeDecoded6, KEscapeEncoded6_auth, EscapeUtils::EEscapeAuth);
	TestEscapeUtilsEscapeEncodeL(KEscapeDecoded6, KEscapeEncoded6_path, EscapeUtils::EEscapePath);
	TestEscapeUtilsEscapeEncodeL(KEscapeDecoded6, KEscapeEncoded6_query, EscapeUtils::EEscapeQuery);
	TestEscapeUtilsEscapeEncodeL(KEscapeDecoded6, KEscapeEncoded6_normal, EscapeUtils::EEscapeNormal);
	TestEscapeUtilsEscapeEncodeL(KEscapeDecoded6, KEscapeEncoded6_url, EscapeUtils::EEscapeUrlEncoded);

	TestEscapeUtilsEscapeEncodeL(KEscapeDecoded7, KEscapeEncoded7_auth, EscapeUtils::EEscapeAuth);
	TestEscapeUtilsEscapeEncodeL(KEscapeDecoded7, KEscapeEncoded7_path, EscapeUtils::EEscapePath);
	TestEscapeUtilsEscapeEncodeL(KEscapeDecoded7, KEscapeEncoded7_query, EscapeUtils::EEscapeQuery);
	TestEscapeUtilsEscapeEncodeL(KEscapeDecoded7, KEscapeEncoded7_normal, EscapeUtils::EEscapeNormal);
	TestEscapeUtilsEscapeEncodeL(KEscapeDecoded7, KEscapeEncoded7_url, EscapeUtils::EEscapeUrlEncoded);


	TestEscapeUtilsEscapeDecodeL(KEscapeEncoded2_auth, KEscapeDecoded2);
	TestEscapeUtilsEscapeDecodeL(KEscapeEncoded2_path, KEscapeDecoded2);
	TestEscapeUtilsEscapeDecodeL(KEscapeEncoded2_query, KEscapeDecoded2);
	TestEscapeUtilsEscapeDecodeL(KEscapeEncoded2_normal, KEscapeDecoded2);
	TestEscapeUtilsEscapeDecodeL(KEscapeEncoded2_url, KEscapeDecoded2);

	TestEscapeUtilsEscapeDecodeL(KEscapeEncoded3_auth, KEscapeDecoded3);
	TestEscapeUtilsEscapeDecodeL(KEscapeEncoded3_path, KEscapeDecoded3);
	TestEscapeUtilsEscapeDecodeL(KEscapeEncoded3_query, KEscapeDecoded3);
	TestEscapeUtilsEscapeDecodeL(KEscapeEncoded3_normal, KEscapeDecoded3);
	TestEscapeUtilsEscapeDecodeL(KEscapeEncoded3_url, KEscapeDecoded3);

	TestEscapeUtilsEscapeDecodeL(KEscapeEncoded4_auth, KEscapeDecoded4);
	TestEscapeUtilsEscapeDecodeL(KEscapeEncoded4_path, KEscapeDecoded4);
	TestEscapeUtilsEscapeDecodeL(KEscapeEncoded4_query, KEscapeDecoded4);
	TestEscapeUtilsEscapeDecodeL(KEscapeEncoded4_normal, KEscapeDecoded4);
	TestEscapeUtilsEscapeDecodeL(KEscapeEncoded4_url, KEscapeDecoded4);

	TestEscapeUtilsEscapeDecodeL(KEscapeEncoded5_auth, KEscapeDecoded5);
	TestEscapeUtilsEscapeDecodeL(KEscapeEncoded5_path, KEscapeDecoded5);
	TestEscapeUtilsEscapeDecodeL(KEscapeEncoded5_query, KEscapeDecoded5);
	TestEscapeUtilsEscapeDecodeL(KEscapeEncoded5_normal, KEscapeDecoded5);
	TestEscapeUtilsEscapeDecodeL(KEscapeEncoded5_url, KEscapeDecoded5);

	TestEscapeUtilsEscapeDecodeL(KEscapeEncoded6_auth, KEscapeDecoded6);
	TestEscapeUtilsEscapeDecodeL(KEscapeEncoded6_path, KEscapeDecoded6);
	TestEscapeUtilsEscapeDecodeL(KEscapeEncoded6_query, KEscapeDecoded6);
	TestEscapeUtilsEscapeDecodeL(KEscapeEncoded6_normal, KEscapeDecoded6);
	TestEscapeUtilsEscapeDecodeL(KEscapeEncoded6_url, KEscapeDecoded6);

	TestEscapeUtilsEscapeDecodeL(KEscapeEncoded7_auth, KEscapeDecoded7);
	TestEscapeUtilsEscapeDecodeL(KEscapeEncoded7_path, KEscapeDecoded7);
	TestEscapeUtilsEscapeDecodeL(KEscapeEncoded7_query, KEscapeDecoded7);
	TestEscapeUtilsEscapeDecodeL(KEscapeEncoded7_normal, KEscapeDecoded7);
	TestEscapeUtilsEscapeDecodeL(KEscapeEncoded7_url, KEscapeDecoded7);

	//

	TestEscapeUtilsUnicodeUtf8ConversionsL(KUnicode_0, KUtf8_0);

	TestEscapeUtilsIsExcludedCharL(KExcludedChar0, KExcludedChar0_excluded);
	TestEscapeUtilsIsExcludedCharL(KExcludedChar1, KExcludedChar1_excluded);
	TestEscapeUtilsIsExcludedCharL(KExcludedChar2, KExcludedChar2_excluded);
	TestEscapeUtilsIsExcludedCharL(KExcludedChar3, KExcludedChar3_excluded);
	TestEscapeUtilsIsExcludedCharL(KExcludedChar4, KExcludedChar4_excluded);

	TestEscapeUtilsIsEscapeTripleL(KEscapeTriple0(), KEscapeTriple0_valid, KEscapeTriple0_value);
	TestEscapeUtilsIsEscapeTripleL(KEscapeTriple1(), KEscapeTriple1_valid, KEscapeTriple1_value);
	TestEscapeUtilsIsEscapeTripleL(KEscapeTriple2(), KEscapeTriple2_valid, KEscapeTriple2_value);
	
	//TConvBase64 Test
	TestEscapeUtilsBase64ConversionL(KEscapeDecoded2);
	}

void CEscapeUtilsTest::TestEscapeUtilsEscapeEncodeL(const TDesC& aData, const TDesC& aExpected, EscapeUtils::TEscapeMode aEscapeMode)
	{
	iTestHarness->StartTestL(_L("Test EscapeUtils EscapeEncode (16-bit)"));

	TInt error = DoTestEscapeUtilsEscapeEncode<HBufC16>(aData, aExpected, aEscapeMode);

	iTestHarness->EndTest(error);

	// Make 8-bit copy
	HBufC8* dataBuf = HBufC8::NewLC(aData.Length());
	TPtr8 data8Bit = dataBuf->Des();
	data8Bit.Copy(aData);

	HBufC8* expectedBuf = HBufC8::NewLC(aExpected.Length());
	TPtr8 expected8Bit = expectedBuf->Des();
	expected8Bit.Copy(aExpected);

	iTestHarness->StartTestL(_L("Test EscapeUtils EscapeEncode (8-bit)"));

	error = DoTestEscapeUtilsEscapeEncode<HBufC8>(data8Bit, expected8Bit, aEscapeMode);

	iTestHarness->EndTest(error);

	CleanupStack::PopAndDestroy(2, dataBuf);	// dataBuf, expectedBuf
	}

void CEscapeUtilsTest::TestEscapeUtilsEscapeDecodeL(const TDesC& aData, const TDesC& aExpected)
	{
	iTestHarness->StartTestL(_L("Test EscapeUtils EscapeDecode (16-bit)"));

	TInt error = DoTestEscapeUtilsEscapeDecode<HBufC16>(aData, aExpected);

	iTestHarness->EndTest(error);

	// Make 8-bit copy
	HBufC8* dataBuf = HBufC8::NewLC(aData.Length());
	TPtr8 data8Bit = dataBuf->Des();
	data8Bit.Copy(aData);

	HBufC8* expectedBuf = HBufC8::NewLC(aExpected.Length());
	TPtr8 expected8Bit = expectedBuf->Des();
	expected8Bit.Copy(aExpected);

	iTestHarness->StartTestL(_L("Test EscapeUtils EscapeDecode (8-bit)"));

	error = DoTestEscapeUtilsEscapeDecode<HBufC8>(data8Bit, expected8Bit);

	iTestHarness->EndTest(error);

	CleanupStack::PopAndDestroy(2, dataBuf);	// dataBuf, expectedBuf
	}

void CEscapeUtilsTest::TestEscapeUtilsUnicodeUtf8ConversionsL(const TDesC& aUnicode, const TDesC& aUtf8)
	{
	// Make 8-bit copy of utf8
	HBufC8* utf8Buf = HBufC8::NewLC(aUtf8.Length());
	utf8Buf->Des().Copy(aUtf8);

	// Escape decode
	HBufC8* unescapedUtf8Buf = EscapeUtils::EscapeDecodeL(*utf8Buf);
	CleanupStack::PushL(unescapedUtf8Buf);
	TPtr8 unescapedUtf8 = unescapedUtf8Buf->Des();

	iTestHarness->StartTestL(_L("Test EscapeUtils ConvertFromUnicodeToUtf8"));

	HBufC8* convertedUtf8 = NULL;
	TRAPD(error, convertedUtf8 = EscapeUtils::ConvertFromUnicodeToUtf8L(aUnicode));
	if( error == KErrNone )
		{
		// Check is expected
		if( convertedUtf8->Compare(unescapedUtf8) != 0 )
			{
			error = KErrNotFound;
			}
		delete convertedUtf8;
		}
	iTestHarness->EndTest(error);

	iTestHarness->StartTestL(_L("Test EscapeUtils ConvertToUnicodeFromUtf8"));

	HBufC16* convertedUnicode = NULL;
	TRAP(error, convertedUnicode = EscapeUtils::ConvertToUnicodeFromUtf8L(unescapedUtf8));
	if( error == KErrNone )
		{
		// Check is expected
		if( convertedUnicode->Compare(aUnicode) != 0 )
			{
			error = KErrNotFound;
			}
		delete convertedUnicode;
		}
	iTestHarness->EndTest(error);

	CleanupStack::PopAndDestroy(2, utf8Buf);	// utf8Buf, unescapedUtf8Buf
	}

void CEscapeUtilsTest::TestEscapeUtilsIsExcludedCharL(TChar aChar, TBool aIsExcluded)
	{
	iTestHarness->StartTestL(_L("Test EscapeUtils IsExcludedChar"));

	TBool excluded = EscapeUtils::IsExcludedChar(aChar);

	TInt error = KErrNone;
	if( (!excluded && aIsExcluded) | (excluded && !aIsExcluded) )
		{
		error = KErrNotFound;
		}
	iTestHarness->EndTest(error);
	}

void CEscapeUtilsTest::TestEscapeUtilsIsEscapeTripleL(const TDesC& aData, TBool aIsTriple, TInt aValue)
	{
	iTestHarness->StartTestL(_L("Test EscapeUtils IsEscapeTriple (16-Bit)"));

	TInt error = DoTestEscapeUtilsIsEscapeTriple(aData, aIsTriple, aValue);

	iTestHarness->EndTest(error);

	// Make 8-bit copy
	HBufC8* dataBuf = HBufC8::NewLC(aData.Length());
	TPtr8 data8Bit = dataBuf->Des();
	data8Bit.Copy(aData);

	iTestHarness->StartTestL(_L("Test EscapeUtils IsEscapeTriple (8-Bit)"));

	error = DoTestEscapeUtilsIsEscapeTriple(data8Bit, aIsTriple, aValue);

	iTestHarness->EndTest(error);

	CleanupStack::PopAndDestroy(dataBuf);	// dataBuf
	}

void CEscapeUtilsTest::TestEscapeUtilsBase64ConversionL(const TDesC& aData)
	{	
	iTestHarness->StartTestL(_L("Test EscapeUtils Base64 Conversion Check"));
	
	TInt error = KErrNotFound;
	HBufC8* dataBuf = HBufC8::NewMaxLC (aData.Length());
	TPtr8 dataPtr(dataBuf->Des());
	
	HBufC8* encodedBuf = HBufC8::NewMaxLC(aData.Length() * 2);
	TPtr8 encodedPtr(encodedBuf->Des());

	TBase64 codec;
	codec.Encode(dataPtr, encodedPtr);	
	
	HBufC8* decodedBuf = HBufC8::NewMaxLC(aData.Length());
	TPtr8 decodedPtr(decodedBuf->Des());
	
	codec.Decode(encodedPtr, decodedPtr);
	if(decodedPtr.Compare(dataPtr) == 0)
		{
			error = KErrNone;
		}
	iTestHarness->EndTest(error);
	
	CleanupStack::PopAndDestroy(decodedBuf);	// decodedBuf
	CleanupStack::PopAndDestroy(encodedBuf);	// encodedBuf
	CleanupStack::PopAndDestroy(dataBuf);		// dataBuf			
	}
	
//
//
//	Implementation of LOCAL functions
//
//

template<class HBufCType, class TDesCType>
LOCAL_C TInt DoTestEscapeUtilsEscapeEncode(const TDesCType& aData, const TDesCType& aExpected, EscapeUtils::TEscapeMode aEscapeMode)
	{
	// Do escaping
	HBufCType* escaped = NULL;
	TRAPD(error, escaped = EscapeUtils::EscapeEncodeL(aData, aEscapeMode));
	if( error != KErrNone )
		{
		return KErrNotFound;
		}
	// Check with expected
	if( escaped->Compare(aExpected) != 0 )
		{
		delete escaped;
		return KErrNotFound;
		}
	delete escaped;
	return KErrNone;
	}

template<class HBufCType, class TDesCType>
LOCAL_C TInt DoTestEscapeUtilsEscapeDecode(const TDesCType& aData, const TDesCType& aExpected)
	{
	// Do unescaping
	HBufCType* unescaped = NULL;
	TRAPD(error, unescaped = EscapeUtils::EscapeDecodeL(aData));
	if( error != KErrNone )
		{
		return KErrNotFound;
		}
	// Check with expected
	if( unescaped->Compare(aExpected) != 0 )
		{
		delete unescaped;
		return KErrNotFound;
		}
	delete unescaped;
	return KErrNone;
	}

template<class TDesCType>
LOCAL_C TInt DoTestEscapeUtilsIsEscapeTriple(const TDesCType& aData, TBool aIsTriple, TInt aValue)
	{
	// Check for the triple
	TInt value;
	TBool triple = EscapeUtils::IsEscapeTriple(aData, value);
	if( (!triple && aIsTriple) | (triple && !aIsTriple) )
		{
		return KErrNotFound;
		}
	if( triple && value != aValue )
		{
		return KErrNotFound;
		}
	return KErrNone;
	}
