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

#include "CTextUtilsTest.h"

#include <uriutilscommon.h>
#include <inetprottextutils.h>

#include "TextUtilsTestCommon.h"
#include "TextUtilsTests.h"

CTextUtilsTest* CTextUtilsTest::NewLC(CIpuTestHarness* aTestHarness)
	{
	CTextUtilsTest* self = new (ELeave) CTextUtilsTest(aTestHarness);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CTextUtilsTest* CTextUtilsTest::NewL(CIpuTestHarness* aTestHarness)
	{
	CTextUtilsTest* self = CTextUtilsTest::NewLC(aTestHarness);
	CleanupStack::Pop(self);
	return self;
	}

CTextUtilsTest::CTextUtilsTest(CIpuTestHarness* aTestHarness)
: iTestHarness(aTestHarness)
	{
	}

void CTextUtilsTest::ConstructL()
	{
	}

CTextUtilsTest::~CTextUtilsTest()
	{
	}

void CTextUtilsTest::DoTestsL()
	{

	TestTextUtilsRemoveL(KRemove0_base, KRemove0_left, KRemove0_left_consumed,
						KRemove0_right, KRemove0_right_consumed,
						KRemove0_both, KRemove0_both_consumed);

	TestTextUtilsDesToIntConversionL(KDecRep0, KNumericValue0, KIsValid0);
	TestTextUtilsDesToIntConversionL(KDecRep1, KNumericValue1, KIsValid1);
	TestTextUtilsDesToIntConversionL(KDecRep2, KNumericValue2, KIsValid2);
	TestTextUtilsDesToIntConversionL(KDecRep3, KNumericValue3, KIsValid3);
	
	TestTextUtilsIntToDesConversionL(KNumericValue0, KDecRep0);
	TestTextUtilsIntToDesConversionL(KNumericValue1, KDecRep1);

	_LIT(KWhiteSpace," \t");
	TestTextUtilsDesToHexConversionL(KWhiteSpace, 0, EFalse);

	TestTextUtilsDesToHexConversionL(KHexRep0, KNumericValue0, KIsValid0);
	TestTextUtilsDesToHexConversionL(KHexRep1, KNumericValue1, KIsValid1);
	TestTextUtilsDesToHexConversionL(KHexRep2, KNumericValue2, KIsValid2);
	TestTextUtilsDesToHexConversionL(KHexRep3, KNumericValue3, KIsValid3);

	TestTextUtilsHexToDesConversionL(KNumericValue0, KHexRep0);
	TestTextUtilsHexToDesConversionL(KNumericValue1, KHexRep1);

	TestTextUtilsExtractQuotedStringL(KQuotedBuffer0, KQuotedString0, KQuotedRemainder0, KExpectedError0);
	TestTextUtilsExtractQuotedStringL(KQuotedBuffer1, KQuotedString1, KQuotedRemainder1, KExpectedError1);
	TestTextUtilsExtractQuotedStringL(KQuotedBuffer2, KQuotedString2, KQuotedRemainder2, KExpectedError2);
	TestTextUtilsExtractQuotedStringL(KQuotedBuffer3, KQuotedString3, KQuotedRemainder3, KExpectedError3);

	TestTextUtilsExtractTokenFromListL(
									  KListBuffer0,
									  KListToken0_0,
									  KListToken0_1,
									  KListToken0_2,
									  KListRemainder0,
									  KListSeparator0
									  );

	TestTextUtilsExtractTokenFromListL(
									  KListBuffer1,
									  KListToken1_0,
									  KListToken1_1,
									  KListToken1_2,
									  KListRemainder1,
									  KListSeparator1
									  );

	TestTextUtilsExtractTokenFromListL(
									  KListBuffer2,
									  KListToken2_0,
									  KListToken2_1,
									  KListToken2_2,
									  KListRemainder2,
									  KListSeparator2
									  );

	TestTextUtilsExtractTokenFromListL(
									  KListBuffer3,
									  KListToken3_0,
									  KListToken3_1,
									  KListToken3_2,
									  KListRemainder3,
									  KListSeparator3
									  );
	}

void CTextUtilsTest::TestTextUtilsRemoveL(const TDesC& aBase, 
										  const TDesC& aRemoveLeft, TInt aConsumedLeft,
										  const TDesC& aRemoveRight, TInt aConsumedRight,
										  const TDesC& aRemoveBoth, TInt aConsumedBoth)
	{
	iTestHarness->StartTestL(_L("Test TextUtils Remove (16-bit)"));

	TInt error = DoTestTextUtilsRemove<TPtrC16>(aBase, aRemoveLeft, aConsumedLeft,
												aRemoveRight, aConsumedRight,
												aRemoveBoth, aConsumedBoth);

	iTestHarness->EndTest(error);

	// Make 8-bit copy
	HBufC8* baseBuf = HBufC8::NewLC(aBase.Length());
	TPtr8 base8Bit = baseBuf->Des();
	base8Bit.Copy(aBase);

	HBufC8* removeLeftBuf = HBufC8::NewLC(aRemoveLeft.Length());
	TPtr8 removeLeft8Bit = removeLeftBuf->Des();
	removeLeft8Bit.Copy(aRemoveLeft);

	HBufC8* removeRightBuf = HBufC8::NewLC(aRemoveRight.Length());
	TPtr8 removeRight8Bit = removeRightBuf->Des();
	removeRight8Bit.Copy(aRemoveRight);

	HBufC8* removeBothBuf = HBufC8::NewLC(aRemoveBoth.Length());
	TPtr8 removeBoth8Bit = removeBothBuf->Des();
	removeBoth8Bit.Copy(aRemoveBoth);

	iTestHarness->StartTestL(_L("Test TextUtils Remove (8-bit)"));

	error = DoTestTextUtilsRemove<TPtrC8>(base8Bit, removeLeft8Bit, aConsumedLeft,
											removeRight8Bit, aConsumedRight,
											removeBoth8Bit, aConsumedBoth);

	iTestHarness->EndTest(error);

	CleanupStack::PopAndDestroy(4, baseBuf);	// baseBuf, removeLeftBuf, removeRightBuf, removeBothBuf
	}

void CTextUtilsTest::TestTextUtilsDesToIntConversionL(const TDesC& aDes, TInt aInt, TBool aValid)
	{
	iTestHarness->StartTestL(_L("Test TextUtils ConvertDescriptorToInt (16-bit)"));

	TInt error = DoTestTextUtilsDesToIntConversion(aDes, aInt, aValid);

	iTestHarness->EndTest(error);

	// Make 8-bit copy
	HBufC8* desBuf = HBufC8::NewLC(aDes.Length());
	TPtr8 des8Bit = desBuf->Des();
	des8Bit.Copy(aDes);
	
	iTestHarness->StartTestL(_L("Test TextUtils ConvertDescriptorToInt (8-bit)"));

	error = DoTestTextUtilsDesToIntConversion(des8Bit, aInt, aValid);

	iTestHarness->EndTest(error);

	CleanupStack::PopAndDestroy(desBuf);	// desBuf
	}

void CTextUtilsTest::TestTextUtilsIntToDesConversionL(TInt aInt, const TDesC& aDes)
	{
	iTestHarness->StartTestL(_L("Test TextUtils ConvertIntToDescriptor (16-bit)"));

	TInt error = DoTestTextUtilsIntToDesConversion<HBufC16>(aInt, aDes);

	iTestHarness->EndTest(error);

	// Make 8-bit copy
	HBufC8* desBuf = HBufC8::NewLC(aDes.Length());
	TPtr8 des8Bit = desBuf->Des();
	des8Bit.Copy(aDes);
	
	iTestHarness->StartTestL(_L("Test TextUtils ConvertIntToDescriptor (8-bit)"));

	error = DoTestTextUtilsIntToDesConversion<HBufC8>(aInt, des8Bit);

	iTestHarness->EndTest(error);

	CleanupStack::PopAndDestroy(desBuf);	// desBuf
	}



void CTextUtilsTest::TestTextUtilsDesToHexConversionL(const TDesC& aDes, TInt aHexNumber, TBool aValid)
	{
	iTestHarness->StartTestL(_L("Test TextUtils ConvertDescriptorToHex (16-bit)"));

	TInt error = DoTestTextUtilsDesToHexConversion(aDes, aHexNumber, aValid);
	iTestHarness->EndTest(error);

	// Test 8 bit version
	TestTextUtilsDesToHexConversion8L(aDes, aHexNumber, aValid);
	}
	

void CTextUtilsTest::TestTextUtilsDesToHexConversion8L(const TDesC& aDes, TInt aHexNumber, TBool aValid)
	{
	HBufC8* desBuf = HBufC8::NewLC(aDes.Length());
	TPtr8 des8Bit = desBuf->Des();
	des8Bit.Copy(aDes);
	
	iTestHarness->StartTestL(_L("Test TextUtils ConvertDescriptorToHex (8-bit)"));

	TInt error = DoTestTextUtilsDesToHexConversion(des8Bit, aHexNumber, aValid);

	iTestHarness->EndTest(error);

	CleanupStack::PopAndDestroy(desBuf);	// desBuf
	}
	


void CTextUtilsTest::TestTextUtilsHexToDesConversionL(TInt aHex, const TDesC& aDes)
	{
	iTestHarness->StartTestL(_L("Test TextUtils ConvertHexToDescriptor (16-bit)"));

	TInt error = DoTestTextUtilsHexToDesConversion<HBufC16>(aHex, aDes);

	iTestHarness->EndTest(error);

	// Make 8-bit copy
	HBufC8* desBuf = HBufC8::NewLC(aDes.Length());
	TPtr8 des8Bit = desBuf->Des();
	des8Bit.Copy(aDes);
	
	iTestHarness->StartTestL(_L("Test TextUtils ConvertHexToDescriptor (8-bit)"));

	error = DoTestTextUtilsHexToDesConversion<HBufC8>(aHex, des8Bit);

	iTestHarness->EndTest(error);

	CleanupStack::PopAndDestroy(desBuf);	// desBuf
	}

void CTextUtilsTest::TestTextUtilsExtractQuotedStringL(
													  const TDesC&	aBuffer, 
													  const TDesC&	aString, 
													  const TDesC&	aRemainder,
													  TInt			aExpectedError
													  )
	{
	iTestHarness->StartTestL(_L("Test TextUtils ExtractQuotedStringL (16-bit)"));

	TInt error = DoTestExtractQuotedStringL<TPtrC>(aBuffer, aString, aRemainder, aExpectedError);

	iTestHarness->EndTest(error);

	// Make 8-bit copy
	HBufC8* buffer = HBufC8::NewLC(aBuffer.Length());
	buffer->Des().Copy(aBuffer);

	HBufC8* string = HBufC8::NewLC(aString.Length());
	string->Des().Copy(aString);

	HBufC8* remainder = HBufC8::NewLC(aRemainder.Length());
	remainder->Des().Copy(aRemainder);

	iTestHarness->StartTestL(_L("Test TextUtils ExtractQuotedStringL (8-bit)"));

	error = DoTestExtractQuotedStringL<TPtrC8>(*buffer, *string, *remainder, aExpectedError);

	iTestHarness->EndTest(error);

	CleanupStack::PopAndDestroy(3, buffer);
	}

void CTextUtilsTest::TestTextUtilsExtractTokenFromListL(
													   const TDesC& aBuffer,
													   const TDesC& aToken0,
													   const TDesC& aToken1,
													   const TDesC& aToken2,
													   const TDesC& aRemainder,
													   const TDesC& aSeparator
													   )
	{
	iTestHarness->StartTestL(_L("Test TextUtils ExtractTokenFromList, multiple separator (16-bit)"));

	TInt error = DoExtractTokenFromListMultipleSeparator<TPtrC>(aBuffer, aToken0, aToken1, aToken2, aRemainder, aSeparator);

	iTestHarness->EndTest(error);

	// Make 8-bit copy
	HBufC8* buffer = HBufC8::NewLC(aBuffer.Length());
	buffer->Des().Copy(aBuffer);

	HBufC8* token0 = HBufC8::NewLC(aToken0.Length());
	token0->Des().Copy(aToken0);

	HBufC8* token1 = HBufC8::NewLC(aToken1.Length());
	token1->Des().Copy(aToken1);

	HBufC8* token2 = HBufC8::NewLC(aToken2.Length());
	token2->Des().Copy(aToken2);

	HBufC8* remainder = HBufC8::NewLC(aRemainder.Length());
	remainder->Des().Copy(aRemainder);

	HBufC8* separator = HBufC8::NewLC(aSeparator.Length());
	separator->Des().Copy(aSeparator);

	iTestHarness->StartTestL(_L("Test TextUtils ExtractTokenFromList, multiple separator (8-bit)"));

	error = DoExtractTokenFromListMultipleSeparator<TPtrC8>(*buffer, *token0, *token1, *token2, *remainder, *separator);

	iTestHarness->EndTest(error);

	CleanupStack::PopAndDestroy(6, buffer);
	}

void CTextUtilsTest::TestTextUtilsExtractTokenFromListL(
													   const TDesC& aBuffer,
													   const TDesC& aToken0,
													   const TDesC& aToken1,
													   const TDesC& aToken2,
													   const TDesC& aRemainder,
													   TChar		aSeparator
													   )
	{
	iTestHarness->StartTestL(_L("Test TextUtils ExtractTokenFromList, single separator (16-bit)"));

	TInt error = DoExtractTokenFromListSingleSeparator<TPtrC>(aBuffer, aToken0, aToken1, aToken2, aRemainder, aSeparator);

	iTestHarness->EndTest(error);

	// Make 8-bit copy
	HBufC8* buffer = HBufC8::NewLC(aBuffer.Length());
	buffer->Des().Copy(aBuffer);

	HBufC8* token0 = HBufC8::NewLC(aToken0.Length());
	token0->Des().Copy(aToken0);

	HBufC8* token1 = HBufC8::NewLC(aToken1.Length());
	token1->Des().Copy(aToken1);

	HBufC8* token2 = HBufC8::NewLC(aToken2.Length());
	token2->Des().Copy(aToken2);

	HBufC8* remainder = HBufC8::NewLC(aRemainder.Length());
	remainder->Des().Copy(aRemainder);

	iTestHarness->StartTestL(_L("Test TextUtils ExtractTokenFromList, single separator (8-bit)"));

	error = DoExtractTokenFromListSingleSeparator<TPtrC8>(*buffer, *token0, *token1, *token2, *remainder, aSeparator);

	iTestHarness->EndTest(error);

	CleanupStack::PopAndDestroy(5, buffer);
	}

/*
 *	Local methods
 */

template<class TPtrCType, class TDesCType>
LOCAL_C TInt DoTestTextUtilsRemove(const TDesCType& aBase, 
								   const TDesCType& aRemoveLeft, TInt aConsumedLeft,
								   const TDesCType& aRemoveRight, TInt aConsumedRight,
								   const TDesCType& aRemoveBoth, TInt aConsumedBoth)
	{
	// Do remove left...
	TPtrCType data = aBase;
	TInt consumed = InetProtTextUtils::RemoveWhiteSpace(data, InetProtTextUtils::ERemoveLeft);
	// Check...
	if( data.Compare(aRemoveLeft) != 0 || consumed != aConsumedLeft )
		{
		return KErrNotFound;
		}
	// Do remove right...
	data.Set(aBase);
	consumed = InetProtTextUtils::RemoveWhiteSpace(data, InetProtTextUtils::ERemoveRight);
	// Check...
	if( data.Compare(aRemoveRight) != 0 || consumed != aConsumedRight )
		{
		return KErrNotFound;
		}
	// Do remove both...
	data.Set(aBase);
	consumed = InetProtTextUtils::RemoveWhiteSpace(data, InetProtTextUtils::ERemoveBoth);
	// Check...
	if( data.Compare(aRemoveBoth) != 0 || consumed != aConsumedBoth )
		{
		return KErrNotFound;
		}
	return KErrNone;
	}

template<class TDesCType>
LOCAL_C TInt DoTestTextUtilsDesToIntConversion(const TDesCType& aDes, TInt aInt, TBool aValid)
	{
	TInt theInt;
	TInt error = InetProtTextUtils::ConvertDescriptorToInt(aDes, theInt);
	if( (error < KErrNone && aValid) || (error >= KErrNone && !aValid) )
		{
		return KErrNotFound;
		}
	if( aValid && theInt != aInt )
		{
		return KErrNotFound;
		}
	return KErrNone;
	}

template<class HBufCType, class TDesCType>
LOCAL_C TInt DoTestTextUtilsIntToDesConversion(TInt aInt, const TDesCType& aDes)
	{
	HBufCType* theDes = NULL;
	TRAPD(error, InetProtTextUtils::ConvertIntToDescriptorL(aInt, theDes));
	if( error != KErrNone )
		{
		return KErrNotFound;
		}
	if( theDes->CompareF(aDes) != 0 )
		{
		delete theDes;
		return KErrNotFound;
		}
	delete theDes;
	return KErrNone;
	}

template<class TDesCType>
LOCAL_C TInt DoTestTextUtilsDesToHexConversion(const TDesCType& aDes, TInt aHex, TBool aValid)
	{
	TInt theHex;
	TInt error = InetProtTextUtils::ConvertDescriptorToHex(aDes, theHex);
	if( (error < KErrNone && aValid) || (error >= KErrNone && !aValid) )
		{
		return KErrNotFound;
		}
	if( aValid && theHex != aHex )
		{
		return KErrNotFound;
		}
	return KErrNone;
	}

template<class HBufCType, class TDesCType>
LOCAL_C TInt DoTestTextUtilsHexToDesConversion(TInt aHex, const TDesCType& aDes)
	{
	HBufCType* theDes = NULL;
	TRAPD(error, InetProtTextUtils::ConvertHexToDescriptorL(aHex, theDes));
	if( error != KErrNone )
		{
		return KErrNotFound;
		}
	if( theDes->CompareF(aDes) != 0 )
		{
		delete theDes;
		return KErrNotFound;
		}
	delete theDes;
	return KErrNone;
	}

template<class TPtrCType, class TDesCType>
LOCAL_C TInt DoTestExtractQuotedStringL(const TDesCType& aBuffer, const TDesCType& aString, const TDesCType& aRemainder, TInt aExpectedError)
	{
	TPtrCType buffer(aBuffer);
	TPtrCType string;
	TRAPD( error, InetProtTextUtils::ExtractQuotedStringL(buffer, string) );

	if( error == KErrNone && aExpectedError == KErrNone)
		{
		if( aString.Compare(string) == 0 && aRemainder.Compare(buffer) == 0 )
			{
			// Passed
			return KErrNone;
			}
		else
			{
			// Failed
			return KErrNotFound;
			}
		}
	else if( error == aExpectedError )
		{
		// Got expected error - passed
		return KErrNone;
		}
	else
		{
		// Failed
		return KErrNotFound;
		}
	}

template<class TPtrCType, class TDesCType>
LOCAL_C TInt DoExtractTokenFromListMultipleSeparator(
												const TDesCType& aBuffer,
												const TDesCType& aToken0,
												const TDesCType& aToken1,
												const TDesCType& aToken2,
												const TDesCType& aRemainder,
												const TDesCType& aSeparator
												)
	{
	// get the first token...
	TPtrCType buffer = aBuffer;
	TPtrCType token;
	InetProtTextUtils::ExtractNextTokenFromList(buffer, token, aSeparator);

	// Is it correct?
	if( token.Compare(aToken0) != 0 )
		{
		// Failed
		return KErrNotFound;
		}
	// get the second token
	InetProtTextUtils::ExtractNextTokenFromList(buffer, token, aSeparator);

	// Is it correct?
	if( token.Compare(aToken1) != 0 )
		{
		// Failed
		return KErrNotFound;
		}
	// get the third token
	InetProtTextUtils::ExtractNextTokenFromList(buffer, token, aSeparator);

	// Is it correct?
	if( token.Compare(aToken2) != 0 )
		{
		// Failed
		return KErrNotFound;
		}
	// Check that the remainder is correct
	if( buffer.Compare(aRemainder) != 0 )
		{
		// Failed
		return KErrNotFound;
		}
	return KErrNone;
	}

template<class TPtrCType, class TDesCType>
LOCAL_C TInt DoExtractTokenFromListSingleSeparator(
												const TDesCType& aBuffer,
												const TDesCType& aToken0,
												const TDesCType& aToken1,
												const TDesCType& aToken2,
												const TDesCType& aRemainder,
												TChar			 aSeparator
												)
	{
	// get the first token...
	TPtrCType buffer = aBuffer;
	TPtrCType token;
	InetProtTextUtils::ExtractNextTokenFromList(buffer, token, aSeparator);

	// Is it correct?
	if( token.Compare(aToken0) != 0 )
		{
		// Failed
		return KErrNotFound;
		}
	// get the second token
	InetProtTextUtils::ExtractNextTokenFromList(buffer, token, aSeparator);

	// Is it correct?
	if( token.Compare(aToken1) != 0 )
		{
		// Failed
		return KErrNotFound;
		}
	// get the third token
	InetProtTextUtils::ExtractNextTokenFromList(buffer, token, aSeparator);

	// Is it correct?
	if( token.Compare(aToken2) != 0 )
		{
		// Failed
		return KErrNotFound;
		}
	// Check that the remainder is correct
	if( buffer.Compare(aRemainder) != 0 )
		{
		// Failed
		return KErrNotFound;
		}
	return KErrNone;
	}
