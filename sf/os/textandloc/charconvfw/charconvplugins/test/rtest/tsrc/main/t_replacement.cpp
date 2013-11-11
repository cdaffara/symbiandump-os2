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
#include "t_replacement.h"


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

/**
@SYMTestCaseID          SYSLIB-CHARCONV-CT-0546
@SYMTestCaseDesc        Tests for the conversion of uncovertible Unicode characters
@SYMTestPriority        Medium
@SYMTestActions         Tests for the replacement of uncovertible Unicode characters
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
void CT_REPLACEMENT::DoE32MainL()
	{
    INFO_PRINTF1(_L(" @SYMTestCaseID:SYSLIB-CHARCONV-CT-0546 "));
	RFs fileServerSession;
	CleanupClosePushL(fileServerSession);
	User::LeaveIfError(fileServerSession.Connect());
	CCnvCharacterSetConverter* characterSetConverter=CCnvCharacterSetConverter::NewLC();
	CArrayFix<CCnvCharacterSetConverter::SCharacterSet>* arrayOfCharacterSetsAvailable=CCnvCharacterSetConverter::CreateArrayOfCharacterSetsAvailableLC(fileServerSession);
	INFO_PRINTF1(_L("Available:\n"));
	for (TInt i=arrayOfCharacterSetsAvailable->Count()-1; i>=0; --i)
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
	characterSetConverter->PrepareToConvertToOrFromL(KCharacterSetIdentifierGb2312, *arrayOfCharacterSetsAvailable, fileServerSession);
	INFO_PRINTF1(_L("Testing various values for iReplacementForUnconvertibleUnicodeCharacters"));
	//
	INFO_PRINTF1(_L("Testing iReplacementForUnconvertibleUnicodeCharacters being long"));
	{
	TBuf16<50> originalUnicode;
	originalUnicode.Format(_L16("Here is a Georgian character - %c"), 0x10da);
	characterSetConverter->SetReplacementForUnconvertibleUnicodeCharactersL(_L8("[something quite long]"));
	const TInt KLengthOfForeignBuffer=100;
	TUint8 foreignBuffer[KLengthOfForeignBuffer];
	for (TInt i=31; ; ++i)
		{
		test(i<=KLengthOfForeignBuffer);
		TPtr8 generatedForeign(foreignBuffer, i);
		TInt numberOfUnconvertibleCharacters;
		TInt indexOfFirstUnconvertibleCharacter;
		const TInt returnValue=characterSetConverter->ConvertFromUnicode(generatedForeign, originalUnicode, numberOfUnconvertibleCharacters, indexOfFirstUnconvertibleCharacter);
		if (i==53)
			{
			test(returnValue==0);
			test(generatedForeign==_L8("Here is a Georgian character - [something quite long]"));
			test(numberOfUnconvertibleCharacters==1);
			test(indexOfFirstUnconvertibleCharacter==31);
			break;
			}
		test(returnValue==1);
		test(generatedForeign==_L8("Here is a Georgian character - "));
		test(numberOfUnconvertibleCharacters==0);
		test(indexOfFirstUnconvertibleCharacter==-1); // not something that the API guarantees (it's undefined if numberOfUnconvertibleCharacters==0), but we'll check it any
		}
	}
	INFO_PRINTF1(_L("Testing lots of unconvertible Unicode characters"));
	{
	characterSetConverter->SetReplacementForUnconvertibleUnicodeCharactersL(_L8("?Q"));
	TInt numberOfUnconvertibleCharacters;
	TInt indexOfFirstUnconvertibleCharacter;
	TInt i;
	TPtr16 originalUnicode(HBufC16::NewMaxLC(10+(sizeof(CCnvCharacterSetConverter::TArrayOfAscendingIndices)/sizeof(TUint16)))->Des());
	TPtr8 generatedForeign(HBufC8::NewLC(originalUnicode.Length()*sizeof(TUint16))->Des());
	originalUnicode.Fill(0x7535);
	test(characterSetConverter->ConvertFromUnicode(generatedForeign, originalUnicode, numberOfUnconvertibleCharacters, indexOfFirstUnconvertibleCharacter)==0);
	test(generatedForeign.Length()>0);
	test(generatedForeign.Length()==originalUnicode.Length()*STATIC_CAST(TInt, sizeof(TUint16)));
	test(numberOfUnconvertibleCharacters==0);
	for (i=generatedForeign.Length()-1; i>=0; i-=2)
		{
		test(generatedForeign[i-1]==0xb5);
		test(generatedForeign[i]==0xe7);
		}
	originalUnicode.Fill(0x0f0a); // a Tibetan character, not in GB 2312-80
	const TInt returnValue=characterSetConverter->ConvertFromUnicode(generatedForeign, originalUnicode, numberOfUnconvertibleCharacters, indexOfFirstUnconvertibleCharacter);
	test(returnValue==0);
	test(generatedForeign.Length()>0);
	test(generatedForeign.Length()==(originalUnicode.Length()-returnValue)*STATIC_CAST(TInt, sizeof(TUint16)));
	test(numberOfUnconvertibleCharacters==25);
	test(indexOfFirstUnconvertibleCharacter==0);
	for (i=generatedForeign.Length()-1; i>=0; i-=2)
		{
		test(generatedForeign[i-1]=='?');
		test(generatedForeign[i]=='Q');
		}
	CleanupStack::PopAndDestroy(2); // generatedForeign and originalUnicode
	}
	CleanupStack::PopAndDestroy(3); // arrayOfCharacterSetsAvailable and characterSetConverter and fileServerSession
	}

CT_REPLACEMENT::CT_REPLACEMENT()
    {
    SetTestStepName(KTestStep_T_REPLACEMENT);
    }

TVerdict CT_REPLACEMENT::doTestStepL()
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

