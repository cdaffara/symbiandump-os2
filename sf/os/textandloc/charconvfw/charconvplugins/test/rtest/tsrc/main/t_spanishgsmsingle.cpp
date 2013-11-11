/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include "t_spanishgsmsingle.h"

const TUint KCharacterSetIdentifier=KCharacterSetIdentifierSpanishSingleSms7Bit;

#ifdef __WINS__
_LIT(KInputUnicodeFilename, "c:\\test\\data\\gsm7_spanish_single_uni_input.dat");
_LIT(KInputForeignFilename, "c:\\test\\data\\gsm7_spanish_single_for_input.dat");
_LIT(KExpectUnicodeFilename, "c:\\test\\data\\gsm7_spanish_single_uni_expect.dat");
_LIT(KExpectForeignFilename, "c:\\test\\data\\gsm7_spanish_single_for_expect.dat");
#else
_LIT(KInputUnicodeFilename, "z:\\test\\data\\gsm7_spanish_single_uni_input.dat");
_LIT(KInputForeignFilename, "z:\\test\\data\\gsm7_spanish_single_for_input.dat");
_LIT(KExpectUnicodeFilename, "z:\\test\\data\\gsm7_spanish_single_uni_expect.dat");
_LIT(KExpectForeignFilename, "z:\\test\\data\\gsm7_spanish_single_for_expect.dat");
#endif

static CCnvCharacterSetConverter* characterSetConverter;
static RFs fileServerSession;
static TBuf8<1600> temp;
static TBuf16<800> originalUnicode;
static TBuf8<1600> generatedForeign;
static TBuf16<800> generatedUnicode;

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


void CT_SPANISHGSMSINGLE::ReadDescL(TDes8& aDes, const TDesC& aFilename, RFs& aFs)
	{
	RFile file;
	TInt err = file.Open(aFs, aFilename, EFileRead);
	test(err == KErrNone);
	CleanupClosePushL(file);	
	err = file.Read(aDes);
	test(err == KErrNone);
	CleanupStack::PopAndDestroy(&file);
	}

//translate from unicode big endian TDesC8 to TDes16
void CT_SPANISHGSMSINGLE::Unicode_Big(TDesC8& aSource, TDes16& aTarget)
	{
	TInt length = aSource.Length();
	TInt i = 0;
	for(i=0;i<length-1;i++)
		{
		TInt32 temp = *(aSource.Ptr()+(i))*16*16 + *(aSource.Ptr()+i+1);
		aTarget.Append(temp);
		i++;
		}	
	}
        
/**
@SYMTestCaseID				TI18N-CHARCONV-UT-4045
@SYMTestCaseDesc			Testcase to test new converter name can be listed
@SYMTestPriority			High
@SYMTestActions				Test for CreateArrayOfCharacterSetsAvailableLC() 
@SYMTestExpectedResults		The test passed, if Available character sets are listed in epocwind.out
@SYMPREQ					PREQ2592
@SYMREQ						REQ12584
*/
void CT_SPANISHGSMSINGLE::ListAlltheConvertersL()
	{
	CArrayFix<CCnvCharacterSetConverter::SCharacterSet>* arrayOfCharacterSetsAvailable=\
		CCnvCharacterSetConverter::CreateArrayOfCharacterSetsAvailableLC(fileServerSession);
	INFO_PRINTF1(_L(" @SYMTestCaseID:TI18N-CHARCONV-UT-4045 Available:\n "));
	for (TInt i=arrayOfCharacterSetsAvailable->Count()-1; i>=0; --i)
		{
		const CCnvCharacterSetConverter::SCharacterSet& charactersSet=(*arrayOfCharacterSetsAvailable)[i];
		TPtrC charactersSetName(charactersSet.Name());
		if (charactersSet.NameIsFileName())
			{
			charactersSetName.Set(TParsePtrC(charactersSetName).Name());
			}
		INFO_PRINTF2(_L("    %S\n"), &charactersSetName);
		}	
	CleanupStack::PopAndDestroy(arrayOfCharacterSetsAvailable);
	}

/**
@SYMTestCaseID				TI18N-CHARCONV-UT-4046
@SYMTestCaseDesc			Testcase to test new converter can be prepared
@SYMTestPriority			High
@SYMTestActions				Test for PrepareToConvertToOrFromL()
@SYMTestExpectedResults		The test passed, If EAvailable is returned
@SYMPREQ					PREQ2592
@SYMREQ						REQ12584
*/
void CT_SPANISHGSMSINGLE::PrepareNewConverterL()
	{
	INFO_PRINTF1(_L(" @SYMTestCaseID:TI18N-CHARCONV-UT-4046 Testing new converter can be prepared "));
	CCnvCharacterSetConverter::TAvailability avaible = 	characterSetConverter->PrepareToConvertToOrFromL(KCharacterSetIdentifier, fileServerSession);
	test( CCnvCharacterSetConverter::EAvailable == avaible );
	}

/**
@SYMTestCaseID				TI18N-CHARCONV-UT-4047
@SYMTestCaseDesc			Convert Unicode code to foreign character set
@SYMTestPriority			High
@SYMTestActions				Test for ConvertFromUnicode()
@SYMTestExpectedResults		Test must not fail
@SYMPREQ					PREQ2592
@SYMREQ						REQ12584
*/
void CT_SPANISHGSMSINGLE::ConvertFromUnicodeToForeignL()
	{
	INFO_PRINTF1(_L(" @SYMTestCaseID:TI18N-CHARCONV-UT-4047 Testing encoding from Unicode to Foreign "));
	ReadDescL(temp, KInputUnicodeFilename, fileServerSession);

	Unicode_Big(temp, originalUnicode); 
	TInt test = characterSetConverter->ConvertFromUnicode(generatedForeign, originalUnicode);
	test(characterSetConverter->ConvertFromUnicode(generatedForeign, originalUnicode) == 0);
	ReadDescL(temp, KExpectForeignFilename, fileServerSession);
	test(generatedForeign == temp);

	}

/**
@SYMTestCaseID				TI18N-CHARCONV-UT-4048
@SYMTestCaseDesc			Convert foreign character set to Unicode code 
@SYMTestPriority			High
@SYMTestActions				Test for ConvertToUnicode() with legal foreign characters, 0x00 - 0x7F
@SYMTestExpectedResults		Test must not fail
@SYMPREQ					PREQ2592
@SYMREQ						REQ12584
*/
void CT_SPANISHGSMSINGLE::ConvertFromForeignToUnicodeL()
	{
	INFO_PRINTF1(_L(" @SYMTestCaseID:TI18N-CHARCONV-UT-4048 Testing encoding from Foreign to Unicode "));	
	ReadDescL(generatedForeign, KInputForeignFilename, fileServerSession);
	TInt state=CCnvCharacterSetConverter::KStateDefault;
	test( 0 == characterSetConverter->ConvertToUnicode(generatedUnicode, generatedForeign, state));
	ReadDescL(temp, KExpectUnicodeFilename, fileServerSession);
	originalUnicode.Zero();
	Unicode_Big(temp, originalUnicode); 
	test(generatedUnicode == originalUnicode);
	}	
	
/**
@SYMTestCaseID				TI18N-CHARCONV-UT-4049
@SYMTestCaseDesc			Convert illegal character from foreign character set to unicode 
@SYMTestPriority			High
@SYMTestActions				Test for ConvertToUnicode() with illegal foreign characters 0x1B
@SYMTestExpectedResults		Test must not fail
@SYMPREQ					PREQ2592
@SYMREQ						REQ12584
*/
void CT_SPANISHGSMSINGLE::ConvertIllegalForeignCharacter()
	{
	//test 0x1b
	INFO_PRINTF1(_L(" @SYMTestCaseID:TI18N-CHARCONV-UT-4049 Testing EErrorIllFormedInput "));	
	generatedForeign.SetLength(1);
	generatedForeign[0] = 0x1b; 		//the escape character
	TInt state=CCnvCharacterSetConverter::KStateDefault;	
	test( -20 == characterSetConverter->ConvertToUnicode(generatedUnicode, generatedForeign, state));
	}

/**
@SYMTestCaseID				TI18N-CHARCONV-UT-4050
@SYMTestCaseDesc			Convert out of range characters from foreign character set to Unicode 
@SYMTestPriority			High
@SYMTestActions				Test for ConvertToUnicode() for some out of range foreign characters 0x80 - 0xFF
@SYMTestExpectedResults		Test must not fail
@SYMPREQ					PREQ2592
@SYMREQ						REQ12584
*/
void CT_SPANISHGSMSINGLE::ConvertOutOfRangeForeignCharacters()
	{
	//test foreign chararcter point which is not in the scope, e.g. 0x80 in GSM03.38, which should always mapped to 0xFFFD
	INFO_PRINTF1(_L(" @SYMTestCaseID:TI18N-CHARCONV-UT-4050 Testing Characters not in the Scope "));	
	generatedForeign.SetLength(1);
	generatedForeign[0] = 0x80;
	TInt state=CCnvCharacterSetConverter::KStateDefault;
	_LIT16(KExpected2, "\xfffd"); 	//the expected foreign
	test( 0 == characterSetConverter->ConvertToUnicode(generatedUnicode, generatedForeign, state));
	test( generatedUnicode == KExpected2 );
	}

void CT_SPANISHGSMSINGLE::DoE32MainL()
	{
	CleanupClosePushL(fileServerSession);
	User::LeaveIfError(fileServerSession.Connect());
	characterSetConverter=CCnvCharacterSetConverter::NewLC();
	
	ListAlltheConvertersL();
	PrepareNewConverterL();
	ConvertFromUnicodeToForeignL();
	ConvertFromForeignToUnicodeL();
	ConvertIllegalForeignCharacter();
	ConvertOutOfRangeForeignCharacters();
	
	CleanupStack::PopAndDestroy(2);
	}


CT_SPANISHGSMSINGLE::CT_SPANISHGSMSINGLE()
    {
    SetTestStepName(KTestStep_T_SPANISHGSMSINGLE);
    }

TVerdict CT_SPANISHGSMSINGLE::doTestStepL()
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
