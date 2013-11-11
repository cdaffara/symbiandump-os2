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
#include "t_extendedsms.h"

#ifdef __WINS__
_LIT(KInputUnicodeFilename, "c:\\test\\data\\extendedsms_uni_input.dat");
_LIT(KInputForeignFilename, "c:\\test\\data\\extendedsms_for_input.dat");
_LIT(KExpectUnicodeFilename, "c:\\test\\data\\extendedsms_uni_expect.dat");
_LIT(KExpectForeignFilename, "c:\\test\\data\\extendedsms_for_expect.dat");
#else
_LIT(KInputUnicodeFilename, "z:\\test\\data\\extendedsms_uni_input.dat");
_LIT(KInputForeignFilename, "z:\\test\\data\\extendedsms_for_input.dat");
_LIT(KExpectUnicodeFilename, "z:\\test\\data\\extendedsms_uni_expect.dat");
_LIT(KExpectForeignFilename, "z:\\test\\data\\extendedsms_for_expect.dat");
#endif

static CCnvCharacterSetConverter* characterSetConverter;
static RFs fileServerSession;
static TBuf8<1254> temp;
static  TBuf16<627> originalUnicode;
static  TBuf8<627> generatedForeign;
static  TBuf16<627> generatedUnicode;
	
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


//read test data from file
void CT_EXTENDEDSMS::ReadDescL(TDes8& aDes, const TDesC& aFilename, RFs& aFs)
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
void CT_EXTENDEDSMS::Unicode_Big(TDesC8& aSource, TDes16& aTarget)
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
@SYMTestCaseID				SYSLIB-CHARCONV-UT-4003
@SYMTestCaseDesc			Testcase to test new converter name can be listed
@SYMTestPriority			High
@SYMTestActions				Test for CreateArrayOfCharacterSetsAvailableLC() 
@SYMTestExpectedResults		The test passed, if Available character sets are listed in epocwind.out
@SYMPREQ					PREQ2090
@SYMREQ						REQ10364
*/
void CT_EXTENDEDSMS::ListAlltheConvertersL()
	{
	CArrayFix<CCnvCharacterSetConverter::SCharacterSet>* arrayOfCharacterSetsAvailable=\
		CCnvCharacterSetConverter::CreateArrayOfCharacterSetsAvailableLC(fileServerSession);
	INFO_PRINTF1(_L(" @SYMTestCaseID:SYSLIB-CHARCONV-UT-4003 Available Converters\n "));
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
@SYMTestCaseID				SYSLIB-CHARCONV-UT-4004
@SYMTestCaseDesc			Testcase to test new converter name can be prepared for converting
@SYMTestPriority			High
@SYMTestActions				Test for PrepareToConvertToOrFromL()
@SYMTestExpectedResults		The test passed, If EAvailable is returned
@SYMPREQ					PREQ2090
@SYMREQ						REQ10364
*/
void CT_EXTENDEDSMS::PrepareNewConverterL()
	{
	INFO_PRINTF1(_L(" @SYMTestCaseID:SYSLIB-CHARCONV-UT-4004 Preparing the new converter "));
	CCnvCharacterSetConverter::TAvailability avaible = 	characterSetConverter->PrepareToConvertToOrFromL(KCharacterSetIdentifierExtendedSms7Bit, fileServerSession);
	test( CCnvCharacterSetConverter::EAvailable == avaible );
	}

/**
@SYMTestCaseID				SYSLIB-CHARCONV-UT-4005
@SYMTestCaseDesc			Convert Unicode code to foreign character set
@SYMTestPriority			High
@SYMTestActions				Test for ConvertFromUnicode()
@SYMTestExpectedResults		Test is passed if the generated cod and Expected code is same
@SYMPREQ					PREQ2090
@SYMREQ						REQ10364					
*/
void CT_EXTENDEDSMS::ConvertFromUnicodeToForeignL()
	{
	INFO_PRINTF1(_L(" @SYMTestCaseID:SYSLIB-CHARCONV-UT-4005 Encoding from Unicode to Foreign "));
	ReadDescL(temp, KInputUnicodeFilename, fileServerSession);
	Unicode_Big(temp, originalUnicode); 
	test(characterSetConverter->ConvertFromUnicode(generatedForeign, originalUnicode) == 0);
	ReadDescL(temp, KExpectForeignFilename, fileServerSession);
	test(generatedForeign == temp);
	}

/**
@SYMTestCaseID				SYSLIB-CHARCONV-UT-4006
@SYMTestCaseDesc			Convert foreign character set to Unicode code 
@SYMTestPriority			High
@SYMTestActions				Test for ConvertToUnicode() with legal foreign characters, 0x00 - 0x7F
@SYMTestExpectedResults		Test is passed if contents of generatedUnicode buffer with OriginalUnicode buffer are same.
@SYMPREQ					PREQ2090
@SYMREQ						REQ10364
*/
void CT_EXTENDEDSMS::ConvertFromForeignToUnicodeL()
	{
	INFO_PRINTF1(_L(" @SYMTestCaseID:SYSLIB-CHARCONV-UT-4006 Encoding from Foreign to Unicode "));	
	ReadDescL(generatedForeign, KInputForeignFilename, fileServerSession);
	TInt state=CCnvCharacterSetConverter::KStateDefault;
	test( 0 == characterSetConverter->ConvertToUnicode(generatedUnicode, generatedForeign, state));
	ReadDescL(temp, KExpectUnicodeFilename, fileServerSession);
	originalUnicode.Zero();
	Unicode_Big(temp, originalUnicode); 
	test(generatedUnicode == originalUnicode);
	}	

/**
@SYMTestCaseID				SYSLIB-CHARCONV-UT-4007
@SYMTestCaseDesc			Convert out of range characters from foreign character set to Unicode 
@SYMTestPriority			High
@SYMTestActions				Test for ConvertToUnicode() for some out of range foreign characters 0x80 - 0xFF
@SYMTestExpectedResults		The test is passed if generated code is same as expected code
@SYMPREQ					PREQ2090
@SYMREQ						REQ10364
*/
void CT_EXTENDEDSMS::ConvertOutOfRangeForeignCharacters()
	{
	//test foreign chararcter point which is not in the scope, e.g. 0x80 in GSM03.38, which should always mapped to 0xFFFD
	INFO_PRINTF1(_L(" @SYMTestCaseID:SYSLIB-CHARCONV-UT-4007 Testing Characters not in the Scope "));	
	generatedForeign.SetLength(1);
	generatedForeign[0] = 0x81;
	TInt state=CCnvCharacterSetConverter::KStateDefault;
	_LIT16(KExpected2, "\xfffd"); 	//the expected foreign
	test( 0 == characterSetConverter->ConvertToUnicode(generatedUnicode, generatedForeign, state));
	test( generatedUnicode == KExpected2 );
	}

/**
@SYMTestCaseID				SYSLIB-CHARCONV-UT-4008
@SYMTestCaseDesc			Convert illegal character from foreign character set to unicode 
@SYMTestPriority			High
@SYMTestActions				Test for ConvertToUnicode() with illegal foreign characters 0x1B
@SYMTestExpectedResults		Test is passed if KErrCorrupt is returned
@SYMPREQ					PREQ2090
@SYMREQ						REQ10364
*/
void CT_EXTENDEDSMS::ConvertIllegalForeignCharacter()
	{
	//test 0x1b
	INFO_PRINTF1(_L(" @SYMTestCaseID:SYSLIB-CHARCONV-UT-4008 Testing EErrorIllFormedInput "));	
	generatedForeign.SetLength(1);
	generatedForeign[0] = 0x1b; 		//the escape character
	TInt state=CCnvCharacterSetConverter::KStateDefault;	
	test( KErrCorrupt == characterSetConverter->ConvertToUnicode(generatedUnicode, generatedForeign, state));
	}

void CT_EXTENDEDSMS::DoE32MainL()
	{
	CleanupClosePushL(fileServerSession);
	User::LeaveIfError(fileServerSession.Connect());
	characterSetConverter=CCnvCharacterSetConverter::NewLC();
	
	ListAlltheConvertersL();
	PrepareNewConverterL();
	ConvertFromUnicodeToForeignL();
	ConvertFromForeignToUnicodeL();
	ConvertOutOfRangeForeignCharacters();
	ConvertIllegalForeignCharacter();

	CleanupStack::PopAndDestroy(2);
	}

CT_EXTENDEDSMS::CT_EXTENDEDSMS()
    {
    SetTestStepName(KTestStep_T_EXTENDEDSMS);
    }

TVerdict CT_EXTENDEDSMS::doTestStepL()
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
