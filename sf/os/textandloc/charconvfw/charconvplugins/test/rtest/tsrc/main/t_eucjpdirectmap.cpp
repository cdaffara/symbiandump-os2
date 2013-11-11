/*
* Copyright (c) 2000-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32std.h>
#include <e32base.h>
#include <f32file.h>
#include <charconv.h>
#include "t_eucjpdirectmap.h"

#define test(cond)                                  \
    {                                               \
    TBool __bb = (cond);                            \
    TEST(__bb);                                     \
    if (!__bb)                                      \
        {                                           \
        ERR_PRINTF1(_L("ERROR: Test Failed"));      \
        User::Leave(1);                             \
        }                                           \
    }

const TInt KBufferLength=100;

void CT_EUCJPDIRECTMAP::TestTruncatedConversionFromUnicodeToEucJpDirectmap(CCnvCharacterSetConverter& aCharacterSetConverter, const TDesC16& aOriginalUnicode)
	{
	for (TInt i=aOriginalUnicode.Length(); i>=0; --i)
		{
		TBuf8<KBufferLength> generatedEucJpDirectmap;
		const TInt returnValue=aCharacterSetConverter.ConvertFromUnicode(generatedEucJpDirectmap, aOriginalUnicode.Left(i));
		test(returnValue>=0);
		TBuf8<KBufferLength> generatedsecondPartOfEucJpDirectmap;
		test(aCharacterSetConverter.ConvertFromUnicode(generatedsecondPartOfEucJpDirectmap, aOriginalUnicode.Mid(i-returnValue))==0);
		generatedEucJpDirectmap.Append(generatedsecondPartOfEucJpDirectmap);
		TInt state=CCnvCharacterSetConverter::KStateDefault;
		TBuf16<KBufferLength> generatedUnicode;
		test(aCharacterSetConverter.ConvertToUnicode(generatedUnicode, generatedEucJpDirectmap, state)==0);
		test(generatedUnicode==aOriginalUnicode);
		}
	}

void CT_EUCJPDIRECTMAP::TestSplittingConvertingFromUnicodeToEucJpDirectmap(CCnvCharacterSetConverter& aCharacterSetConverter, TInt aMaximumLengthLowerLimit, TInt aMaximumLengthUpperLimit, TInt aExpectedNumberOfUnicodeCharactersNotConvertedAtSplit, TInt aExpectedLengthOfFirstPartOfEucJpDirectmap, const TDesC8& aExpectedEucJpDirectmap, const TDesC16& aOriginalUnicode)
	{
	test(aMaximumLengthLowerLimit<=aMaximumLengthUpperLimit);
	test(aMaximumLengthUpperLimit<=KBufferLength);
	TUint8 eucJpDirectmapBuffer[KBufferLength];
	for (TInt i=aMaximumLengthLowerLimit; i<=aMaximumLengthUpperLimit; ++i)
		{
		TPtr8 generatedFirstPartOfEucJpDirectmap(eucJpDirectmapBuffer, i);
		test(aCharacterSetConverter.ConvertFromUnicode(generatedFirstPartOfEucJpDirectmap, aOriginalUnicode)==aExpectedNumberOfUnicodeCharactersNotConvertedAtSplit);
		test(generatedFirstPartOfEucJpDirectmap==aExpectedEucJpDirectmap.Left(aExpectedLengthOfFirstPartOfEucJpDirectmap));
		TBuf8<KBufferLength> generatedSecondPartOfEucJpDirectmap;
		test(aCharacterSetConverter.ConvertFromUnicode(generatedSecondPartOfEucJpDirectmap, aOriginalUnicode.Right(aExpectedNumberOfUnicodeCharactersNotConvertedAtSplit))==0);
		test(generatedSecondPartOfEucJpDirectmap==aExpectedEucJpDirectmap.Mid(aExpectedLengthOfFirstPartOfEucJpDirectmap));
		TInt state=CCnvCharacterSetConverter::KStateDefault;
		TBuf16<KBufferLength> generatedUnicode;
		test(aCharacterSetConverter.ConvertToUnicode(generatedUnicode, generatedFirstPartOfEucJpDirectmap, state)==0);
		test(state==CCnvCharacterSetConverter::KStateDefault);
		TBuf16<KBufferLength> generatedSecondPartOfUnicode;
		test(aCharacterSetConverter.ConvertToUnicode(generatedSecondPartOfUnicode, generatedSecondPartOfEucJpDirectmap, state)==0);
		test(state==CCnvCharacterSetConverter::KStateDefault);
		generatedUnicode.Append(generatedSecondPartOfUnicode);
		test(generatedUnicode==aOriginalUnicode);
		}
	}

void CT_EUCJPDIRECTMAP::TestTruncatedConversionToUnicodeFromEucJpDirectmap(CCnvCharacterSetConverter& aCharacterSetConverter, const TDesC16& aExpectedUnicode, const TDesC8& aOriginalEucJpDirectmap)
	{
	for (TInt i=aOriginalEucJpDirectmap.Length(); i>=3; --i) 
		{
		TInt state=CCnvCharacterSetConverter::KStateDefault;
		TBuf16<KBufferLength> generatedUnicode;
		const TInt returnValue=aCharacterSetConverter.ConvertToUnicode(generatedUnicode, aOriginalEucJpDirectmap.Left(i), state);
		test(returnValue>=0);
		TBuf16<KBufferLength> generatedsecondPartOfUnicode;
		test(aCharacterSetConverter.ConvertToUnicode(generatedsecondPartOfUnicode, aOriginalEucJpDirectmap.Mid(i-returnValue), state)==0);
		generatedUnicode.Append(generatedsecondPartOfUnicode);
		test(generatedUnicode==aExpectedUnicode);
		}
	}

void CT_EUCJPDIRECTMAP::TestSplittingConvertingToUnicodeFromEucJpDirectmap(CCnvCharacterSetConverter& aCharacterSetConverter, TInt aMaximumLengthLowerLimit, TInt aMaximumLengthUpperLimit, TInt aExpectedNumberOfEucJpDirectmapBytesNotConvertedAtSplit, TInt aExpectedLengthOfFirstPartOfUnicode, const TDesC16& aExpectedUnicode, const TDesC8& aOriginalEucJpDirectmap)
	{
	test(aMaximumLengthLowerLimit<=aMaximumLengthUpperLimit);
	test(aMaximumLengthUpperLimit<=KBufferLength);
	TUint16 unicodeBuffer[KBufferLength];
	for (TInt i=aMaximumLengthLowerLimit; i<=aMaximumLengthUpperLimit; ++i)
		{
		TPtr16 generatedFirstPartOfUnicode(unicodeBuffer, i);
		TInt state=CCnvCharacterSetConverter::KStateDefault;
		test(aCharacterSetConverter.ConvertToUnicode(generatedFirstPartOfUnicode, aOriginalEucJpDirectmap, state)==aExpectedNumberOfEucJpDirectmapBytesNotConvertedAtSplit);
		test(generatedFirstPartOfUnicode==aExpectedUnicode.Left(aExpectedLengthOfFirstPartOfUnicode));
		test(state==CCnvCharacterSetConverter::KStateDefault);
		TBuf16<KBufferLength> generatedSecondPartOfUnicode;
		test(aCharacterSetConverter.ConvertToUnicode(generatedSecondPartOfUnicode, aOriginalEucJpDirectmap.Right(aExpectedNumberOfEucJpDirectmapBytesNotConvertedAtSplit), state)==0);
		test(generatedSecondPartOfUnicode==aExpectedUnicode.Mid(aExpectedLengthOfFirstPartOfUnicode));
		test(state==CCnvCharacterSetConverter::KStateDefault);
		TBuf8<KBufferLength> generatedEucJpDirectmap;
		test(aCharacterSetConverter.ConvertFromUnicode(generatedEucJpDirectmap, generatedFirstPartOfUnicode)==0);
		TBuf8<KBufferLength> generatedSecondPartOfEucJpDirectmap;
		test(aCharacterSetConverter.ConvertFromUnicode(generatedSecondPartOfEucJpDirectmap, generatedSecondPartOfUnicode)==0);
		generatedEucJpDirectmap.Append(generatedSecondPartOfEucJpDirectmap);
		test(generatedEucJpDirectmap==aOriginalEucJpDirectmap);
		}
	}

void CT_EUCJPDIRECTMAP::TestIsIllFormedEucJpDirectmap(CCnvCharacterSetConverter& aCharacterSetConverter, const TDesC8& aEucJpDirectmap)
	{
	TBuf16<50> generatedUnicode;
	TInt state=CCnvCharacterSetConverter::KStateDefault;
	TPtrC8 remainderOfEucJpDirectmap(aEucJpDirectmap);
	TInt lastReturnValue=KMaxTInt;
	FOREVER
		{
		const TInt returnValue=aCharacterSetConverter.ConvertToUnicode(generatedUnicode, remainderOfEucJpDirectmap, state);
		if (returnValue==CCnvCharacterSetConverter::EErrorIllFormedInput)
			{
			break;
			}
		test(returnValue>0);
		test(returnValue<lastReturnValue);
		lastReturnValue=returnValue;
		remainderOfEucJpDirectmap.Set(remainderOfEucJpDirectmap.Right(returnValue));
		}
	}

void CT_EUCJPDIRECTMAP::DoE32MainL()
	{
	RFs fileServerSession;
	CleanupClosePushL(fileServerSession);
	User::LeaveIfError(fileServerSession.Connect());
	CCnvCharacterSetConverter* characterSetConverter=CCnvCharacterSetConverter::NewLC();
	CArrayFix<CCnvCharacterSetConverter::SCharacterSet>* arrayOfCharacterSetsAvailable=CCnvCharacterSetConverter::CreateArrayOfCharacterSetsAvailableLC(fileServerSession);
	INFO_PRINTF1(_L("Available:\n"));
	TInt i;
	for (i=arrayOfCharacterSetsAvailable->Count()-1; i>=0; --i)
		{
		const CCnvCharacterSetConverter::SCharacterSet& charactersSet=(*arrayOfCharacterSetsAvailable)[i];
		characterSetConverter->PrepareToConvertToOrFromL(charactersSet.Identifier(), *arrayOfCharacterSetsAvailable, fileServerSession);
		TPtrC charactersSetName(charactersSet.Name());
		if (charactersSet.NameIsFileName())
			{
			charactersSetName.Set(TParsePtrC(charactersSetName).Name());
			}
		INFO_PRINTF2(_L("    %S\n"), &charactersSetName);
		}
	INFO_PRINTF1(_L("Testing EUC-JP (DirectMap) conversions "));
	characterSetConverter->PrepareToConvertToOrFromL(KCharacterSetIdentifierEucJpDirectmap, *arrayOfCharacterSetsAvailable, fileServerSession);
	//
	INFO_PRINTF1(_L("Empty descriptor"));
	TestSplittingConvertingFromUnicodeToEucJpDirectmap(*characterSetConverter, 0, 10, 0, 0, KNullDesC8, KNullDesC16);
	TestSplittingConvertingToUnicodeFromEucJpDirectmap(*characterSetConverter, 0, 10, 0, 0, KNullDesC16, KNullDesC8);
	INFO_PRINTF1(_L("Testing converting to EUC-JP (DirectMap)"));
	TBuf16<50> originalUnicode;
	originalUnicode.Format(_L16("Some %c%c%c%c%c%c"), 0xff9a, 0xff70, 0x6f22, 0x5b57, 0x5379, 0x5dce);
	const TPtrC8 expectedEucJpDirectmap(_S8("Some \x8e\xda\x8e\xb0\xb4\xc1\xbb\xfa\x8f\xb4\xc1\x8f\xbb\xfa"));
	TestTruncatedConversionFromUnicodeToEucJpDirectmap(*characterSetConverter, originalUnicode);
	TestSplittingConvertingFromUnicodeToEucJpDirectmap(*characterSetConverter, 0, 0, 11, 0, expectedEucJpDirectmap, originalUnicode);
	TestSplittingConvertingFromUnicodeToEucJpDirectmap(*characterSetConverter, 1, 1, 10, 1, expectedEucJpDirectmap, originalUnicode);
	TestSplittingConvertingFromUnicodeToEucJpDirectmap(*characterSetConverter, 2, 2, 9, 2, expectedEucJpDirectmap, originalUnicode);
	TestSplittingConvertingFromUnicodeToEucJpDirectmap(*characterSetConverter, 3, 3, 8, 3, expectedEucJpDirectmap, originalUnicode);
	TestSplittingConvertingFromUnicodeToEucJpDirectmap(*characterSetConverter, 4, 4, 7, 4, expectedEucJpDirectmap, originalUnicode);
	TestSplittingConvertingFromUnicodeToEucJpDirectmap(*characterSetConverter, 5, 6, 6, 5, expectedEucJpDirectmap, originalUnicode);
	TestSplittingConvertingFromUnicodeToEucJpDirectmap(*characterSetConverter, 7, 8, 5, 7, expectedEucJpDirectmap, originalUnicode);
	TestSplittingConvertingFromUnicodeToEucJpDirectmap(*characterSetConverter, 9, 10, 4, 9, expectedEucJpDirectmap, originalUnicode);
	TestSplittingConvertingFromUnicodeToEucJpDirectmap(*characterSetConverter, 11, 12, 3, 11, expectedEucJpDirectmap, originalUnicode);
	TestSplittingConvertingFromUnicodeToEucJpDirectmap(*characterSetConverter, 13, 15, 2, 13, expectedEucJpDirectmap, originalUnicode);
	TestSplittingConvertingFromUnicodeToEucJpDirectmap(*characterSetConverter, 16, 18, 1, 16, expectedEucJpDirectmap, originalUnicode);
	TestSplittingConvertingFromUnicodeToEucJpDirectmap(*characterSetConverter, 19, 30, 0, 19, expectedEucJpDirectmap, originalUnicode);
	INFO_PRINTF1(_L("Testing converting to Unicode"));
	const TPtrC8 originalEucJpDirectmap(_S8("pool\xbe\xae\xc3\xd3\x8e\xcc\x8e\xdf\x8e\xd9\x8f\xc0\xcd\x8f\xc8\xc5pool\x8e\xcc\x8e\xdf\x8e\xd9\xbe\xae\xc3\xd3\x8f\xc0\xcd\x8f\xc8\xc5\xbe\xae\xc3\xd3pool\x8f\xc0\xcd\x8f\xc8\xc5\x8e\xcc\x8e\xdf\x8e\xd9pool"));
	TBuf16<50> expectedUnicode;
	expectedUnicode.Format(_L16("pool%c%c%c%c%c%c%cpool%c%c%c%c%c%c%c%c%cpool%c%c%c%c%cpool"), 0x5c0f, 0x6c60, 0xff8c, 0xff9f, 0xff99, 0x641e, 0x6f0d, 0xff8c, 0xff9f, 0xff99, 0x5c0f, 0x6c60, 0x641e, 0x6f0d, 0x5c0f, 0x6c60, 0x641e, 0x6f0d, 0xff8c, 0xff9f, 0xff99);
	TestTruncatedConversionToUnicodeFromEucJpDirectmap(*characterSetConverter, expectedUnicode, originalEucJpDirectmap);
	TestTruncatedConversionToUnicodeFromEucJpDirectmap(*characterSetConverter, expectedUnicode.Mid(4, 2), originalEucJpDirectmap.Mid(4, 4));
	TestTruncatedConversionToUnicodeFromEucJpDirectmap(*characterSetConverter, expectedUnicode.Mid(6, 3), originalEucJpDirectmap.Mid(8, 6));
	TestTruncatedConversionToUnicodeFromEucJpDirectmap(*characterSetConverter, expectedUnicode.Mid(9, 2), originalEucJpDirectmap.Mid(14, 6));
	static const TInt numberOfCharactersInEachHomogeneousRun[13]={4, 2, 3, 2, 4, 3, 2, 2, 2, 4, 2, 3, 4};
	static const TInt numberOfBytesPerCharacterInEachHomogeneousRun[13]={1, 2, 2, 3, 1, 2, 2, 3, 2, 1, 3, 2, 1};
	TInt e=64;
	TInt u=0;
	for (i=0; i<13; ++i)
		{
		TInt j;
		for (j=0; j<numberOfCharactersInEachHomogeneousRun[i]; ++j, ++u, e-=numberOfBytesPerCharacterInEachHomogeneousRun[i])
			{
			TestSplittingConvertingToUnicodeFromEucJpDirectmap(*characterSetConverter, u, u, e, u, expectedUnicode, originalEucJpDirectmap);
			}
		}
	test(e==0);
	test(u==37);
	TestSplittingConvertingToUnicodeFromEucJpDirectmap(*characterSetConverter, u, u+10, e, u, expectedUnicode, originalEucJpDirectmap);
	INFO_PRINTF1(_L("Testing ill-formed EUC-JP (DirectMap)"));
	TestIsIllFormedEucJpDirectmap(*characterSetConverter, _L8("\xa1"));
	TestIsIllFormedEucJpDirectmap(*characterSetConverter, _L8("\xa1\xb2\xc3"));
	TestIsIllFormedEucJpDirectmap(*characterSetConverter, _L8("\xa1\xb2\x8e"));
	TestIsIllFormedEucJpDirectmap(*characterSetConverter, _L8("\xa1\xb2\x8f"));
	TestIsIllFormedEucJpDirectmap(*characterSetConverter, _L8("\xa1\xb2\x8f\xaa"));
	TestIsIllFormedEucJpDirectmap(*characterSetConverter, _L8("\x8e\xd4\x8e"));
	TestIsIllFormedEucJpDirectmap(*characterSetConverter, _L8("\x8e\xd4\x8f"));
	TestIsIllFormedEucJpDirectmap(*characterSetConverter, _L8("\x8e\xd4\x8f\xbb"));
	TestIsIllFormedEucJpDirectmap(*characterSetConverter, _L8("\x8f\xe5\x8e"));
	TestIsIllFormedEucJpDirectmap(*characterSetConverter, _L8("\x8f\xe5\x8f"));
	TestIsIllFormedEucJpDirectmap(*characterSetConverter, _L8("\x8f\xe5\x8f\xcc"));
	TestIsIllFormedEucJpDirectmap(*characterSetConverter, _L8("xyz\x8e\xd4\x8e"));
	TestIsIllFormedEucJpDirectmap(*characterSetConverter, _L8("xyz\x8e\xd4\x8f"));
	TestIsIllFormedEucJpDirectmap(*characterSetConverter, _L8("xyz\x8e\xd4\x8f\xdd"));
	TestIsIllFormedEucJpDirectmap(*characterSetConverter, _L8("xyz\x8f\xe5\x8e"));
	TestIsIllFormedEucJpDirectmap(*characterSetConverter, _L8("xyz\x8f\xe5\x8f"));
	TestIsIllFormedEucJpDirectmap(*characterSetConverter, _L8("xyz\x8f\xe5\x8f\xee"));
	TestIsIllFormedEucJpDirectmap(*characterSetConverter, _L8("\x8e "));
	TestIsIllFormedEucJpDirectmap(*characterSetConverter, _L8("\x8f "));
	TestIsIllFormedEucJpDirectmap(*characterSetConverter, _L8("\x8f\xf1 "));
	TestIsIllFormedEucJpDirectmap(*characterSetConverter, _L8("\x8e\x41"));
	TestIsIllFormedEucJpDirectmap(*characterSetConverter, _L8("\x8f\x41"));
	TestIsIllFormedEucJpDirectmap(*characterSetConverter, _L8("\x8f\xe2\x41"));
	TestIsIllFormedEucJpDirectmap(*characterSetConverter, _L8("amb\x8e "));
	TestIsIllFormedEucJpDirectmap(*characterSetConverter, _L8("amb\x8f "));
	TestIsIllFormedEucJpDirectmap(*characterSetConverter, _L8("amb\x8f\xf1 "));
	TestIsIllFormedEucJpDirectmap(*characterSetConverter, _L8("amb\x8e\x41"));
	TestIsIllFormedEucJpDirectmap(*characterSetConverter, _L8("amb\x8f\x41"));
	TestIsIllFormedEucJpDirectmap(*characterSetConverter, _L8("amb\x8f\xe2\x41"));
	TestIsIllFormedEucJpDirectmap(*characterSetConverter, _L8("\xa1 "));
	TestIsIllFormedEucJpDirectmap(*characterSetConverter, _L8("\xa1\x61"));
	TestIsIllFormedEucJpDirectmap(*characterSetConverter, _L8("eb\xa1 "));
	TestIsIllFormedEucJpDirectmap(*characterSetConverter, _L8("eb\xa1\x61"));
	
	const TPtrC8 originalEucJpDirectmapYen(_S8("pool\x5c"));
	TBuf16<10> expectedUnicodeBackSlash;
	expectedUnicodeBackSlash.Format(_L16("pool%c"), 0x005c);
    TInt state=CCnvCharacterSetConverter::KStateDefault;
    TBuf16<KBufferLength> generatedUnicodeBackSlash;
    test(characterSetConverter->ConvertToUnicode(generatedUnicodeBackSlash, originalEucJpDirectmapYen, state) == 0);
    test(generatedUnicodeBackSlash==expectedUnicodeBackSlash);
	
	CleanupStack::PopAndDestroy(3); // arrayOfCharacterSetsAvailable and characterSetConverter and fileServerSession
	}

CT_EUCJPDIRECTMAP::CT_EUCJPDIRECTMAP()
    {
    SetTestStepName(KTestStep_T_EUCJPDIRECTMAP);
    }

TVerdict CT_EUCJPDIRECTMAP::doTestStepL()
    {
    SetTestStepResult(EFail);

    __UHEAP_MARK;
    TRAPD(error1, DoE32MainL());    
    __UHEAP_MARKEND;

    if(error1 == KErrNone)
        {
        SetTestStepResult(EPass);
        }

    return TestStepResult();
    }
