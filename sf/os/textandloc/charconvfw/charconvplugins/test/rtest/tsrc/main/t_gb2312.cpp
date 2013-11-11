/*
* Copyright (c) 1997-2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include "t_gb2312.h"

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
@SYMTestCaseID          SYSLIB-CHARCONV-CT-0502
@SYMTestCaseDesc        Tests for converting GB 2312-80 character set to Unicode
@SYMTestPriority        Medium
@SYMTestActions         Tests for simple GB 2312-80 round trips,
						Tests for varying the default Endianness
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
void CT_GB2312::DoE32MainL()
	{
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
	INFO_PRINTF1(_L(" @SYMTestCaseID:SYSLIB-CHARCONV-CT-0502 Testing simple GB 2312-80 round trips "));
	characterSetConverter->PrepareToConvertToOrFromL(KCharacterSetIdentifierGb2312, *arrayOfCharacterSetsAvailable, fileServerSession);
	TBuf8<256> originalGb2312;
	TBuf16<256> originalUnicode;
	TBuf8<256> generatedGb2312;
	TBuf16<256> generatedUnicode;
	TInt state=CCnvCharacterSetConverter::KStateDefault;
	//
	INFO_PRINTF1(_L("Empty descriptor"));
	originalUnicode=_L16("");
	test(characterSetConverter->ConvertFromUnicode(generatedGb2312, originalUnicode)==0);
	test(generatedGb2312==_L8(""));
	test(characterSetConverter->ConvertToUnicode(generatedUnicode, generatedGb2312, state)==0);
	test(state==CCnvCharacterSetConverter::KStateDefault);
	test(generatedUnicode==originalUnicode);
	INFO_PRINTF1(_L("A couple of sample characters"));
	originalGb2312=_L8("\xb9\xa4\xc8\xcb");
	test(characterSetConverter->ConvertToUnicode(generatedUnicode, originalGb2312, state)==0);
	test(state==CCnvCharacterSetConverter::KStateDefault);
	test(generatedUnicode.Length()==2);
	test(generatedUnicode[0]==0x5de5);
	test(generatedUnicode[1]==0x4eba);
	test(characterSetConverter->ConvertFromUnicode(generatedGb2312, generatedUnicode)==0);
	test(generatedGb2312==originalGb2312);
	INFO_PRINTF1(_L("Converting trunctated GB 2312-80 which ends half-way through a multi-byte character"));
	originalGb2312=_L8("\xb9\xa4\xc8");
	test(characterSetConverter->ConvertToUnicode(generatedUnicode, originalGb2312, state)==1);
	test(state==CCnvCharacterSetConverter::KStateDefault);
	test(generatedUnicode.Length()==1);
	test(generatedUnicode[0]==0x5de5);
	originalGb2312=_L8("\xc8");
	test(characterSetConverter->ConvertToUnicode(generatedUnicode, originalGb2312, state)==CCnvCharacterSetConverter::EErrorIllFormedInput);
	INFO_PRINTF1(_L("Testing varying the default endianness"));
	originalGb2312=_L8("\xb4\xf2");
	characterSetConverter->SetDefaultEndiannessOfForeignCharacters(CCnvCharacterSetConverter::ELittleEndian);
	test(characterSetConverter->ConvertToUnicode(generatedUnicode, originalGb2312, state)==0);
	test(state==CCnvCharacterSetConverter::KStateDefault);
	test(generatedUnicode.Length()==1);
	test(generatedUnicode[0]==0x6253);
	test(characterSetConverter->ConvertFromUnicode(generatedGb2312, generatedUnicode)==0);
	test(generatedGb2312==originalGb2312);
	characterSetConverter->SetDefaultEndiannessOfForeignCharacters(CCnvCharacterSetConverter::EBigEndian);
	test(characterSetConverter->ConvertToUnicode(generatedUnicode, originalGb2312, state)==0);
	test(state==CCnvCharacterSetConverter::KStateDefault);
	test(generatedUnicode.Length()==1);
	test(generatedUnicode[0]==0x6253);
	test(characterSetConverter->ConvertFromUnicode(generatedGb2312, generatedUnicode)==0);
	test(generatedGb2312==originalGb2312);
	INFO_PRINTF1(_L("Testing a many-to-one conversion"));
	originalUnicode.SetLength(4);
	originalUnicode[0]=0x30fb;
	originalUnicode[1]=0x00b7;
	originalUnicode[2]=0x2014;
	originalUnicode[3]=0x2015;
	test(characterSetConverter->ConvertFromUnicode(generatedGb2312, originalUnicode)==0);
	test(generatedGb2312==_L8("\xa1\xa4\xa1\xa4\xa1\xaa\xa1\xaa"));
	test(characterSetConverter->ConvertToUnicode(generatedUnicode, generatedGb2312, state)==0);
	test(state==CCnvCharacterSetConverter::KStateDefault);
	test(generatedUnicode.Length()==4);
	test(generatedUnicode[0]==0x00b7);
	test(generatedUnicode[1]==0x00b7);
	test(generatedUnicode[2]==0x2015);
	test(generatedUnicode[3]==0x2015);
	INFO_PRINTF1(_L("Testing GB2312 being aliased to GBK"));
	TUint charSetId = characterSetConverter->ConvertStandardNameOfCharacterSetToIdentifierL(_L8("GB2312"), fileServerSession);
	test(charSetId==KCharacterSetIdentifierGbk);
	test(characterSetConverter->ConvertStandardNameOfCharacterSetToIdentifierL(_L8("csGB2312"), fileServerSession)==KCharacterSetIdentifierGbk);
	test(characterSetConverter->ConvertStandardNameOfCharacterSetToIdentifierL(_L8("iso-ir-58"), fileServerSession)==KCharacterSetIdentifierGbk);
	test(characterSetConverter->ConvertStandardNameOfCharacterSetToIdentifierL(_L8("chinese"), fileServerSession)==KCharacterSetIdentifierGbk);
	test(characterSetConverter->ConvertStandardNameOfCharacterSetToIdentifierL(_L8("csISO58GB231280"), fileServerSession)==KCharacterSetIdentifierGbk);
	test(characterSetConverter->ConvertMibEnumOfCharacterSetToIdentifierL(57, fileServerSession)==KCharacterSetIdentifierGbk);
	test(characterSetConverter->ConvertMibEnumOfCharacterSetToIdentifierL(2025, fileServerSession)==KCharacterSetIdentifierGbk);
	characterSetConverter->PrepareToConvertToOrFromL(charSetId, *arrayOfCharacterSetsAvailable, fileServerSession);
	originalUnicode.SetLength(3);
	originalUnicode[0]=0x9556; // this has a corresponding GB2312 character
	originalUnicode[1]=0x9555; // this has a corresponding GBK character not in GB2312...
	originalUnicode[2]=0x9394; // ... as does this
	test(characterSetConverter->ConvertFromUnicode(generatedGb2312, originalUnicode)==0);
	test(generatedGb2312.Length()==6);
	test(generatedGb2312==_L8("\xef\xda\xe9\x46\xe6\x67"));
	CleanupStack::PopAndDestroy(3); // arrayOfCharacterSetsAvailable and characterSetConverter and fileServerSession
	}

CT_GB2312::CT_GB2312()
    {
    SetTestStepName(KTestStep_T_GB2312);
    }

TVerdict CT_GB2312::doTestStepL()
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

