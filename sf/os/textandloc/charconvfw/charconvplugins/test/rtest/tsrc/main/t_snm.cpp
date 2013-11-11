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
#include "t_snm.h"

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
@SYMTestCaseID          SYSLIB-CHARCONV-CT-0547
@SYMTestCaseDesc        Testing SNM files
@SYMTestPriority        Medium
@SYMTestActions         Tests for conversions from UIDs to  Standard names and MIB enums
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
void CT_SNM::DoE32MainL()
	{
	RFs fileServerSession;
	CleanupClosePushL(fileServerSession);
	User::LeaveIfError(fileServerSession.Connect());
	CCnvCharacterSetConverter* characterSetConverter=CCnvCharacterSetConverter::NewLC();
	INFO_PRINTF1(_L(" @SYMTestCaseID:SYSLIB-CHARCONV-CT-0547 Testing SNM files (conversion between UIDs and Standard names/MIB enums) "));
	test(characterSetConverter->ConvertStandardNameOfCharacterSetToIdentifierL(_L8("ANSI_X3.4-1968"), fileServerSession)==KCharacterSetIdentifierAscii);
	test(characterSetConverter->ConvertStandardNameOfCharacterSetToIdentifierL(_L8("iso-ir-6"), fileServerSession)==KCharacterSetIdentifierAscii);
	test(characterSetConverter->ConvertStandardNameOfCharacterSetToIdentifierL(_L8("ANSI_X3.4-1986"), fileServerSession)==KCharacterSetIdentifierAscii);
	test(characterSetConverter->ConvertStandardNameOfCharacterSetToIdentifierL(_L8("ISO_646.irv:1991"), fileServerSession)==KCharacterSetIdentifierAscii);
	test(characterSetConverter->ConvertStandardNameOfCharacterSetToIdentifierL(_L8("ISO646-US"), fileServerSession)==KCharacterSetIdentifierAscii);
	test(characterSetConverter->ConvertStandardNameOfCharacterSetToIdentifierL(_L8("us"), fileServerSession)==KCharacterSetIdentifierAscii);
	test(characterSetConverter->ConvertStandardNameOfCharacterSetToIdentifierL(_L8("IBM367"), fileServerSession)==KCharacterSetIdentifierAscii);
	test(characterSetConverter->ConvertStandardNameOfCharacterSetToIdentifierL(_L8("cp367"), fileServerSession)==KCharacterSetIdentifierAscii);
	test(characterSetConverter->ConvertStandardNameOfCharacterSetToIdentifierL(_L8("csASCII"), fileServerSession)==KCharacterSetIdentifierAscii);
	test(characterSetConverter->ConvertMibEnumOfCharacterSetToIdentifierL(3, fileServerSession)==KCharacterSetIdentifierAscii);
	test(characterSetConverter->ConvertStandardNameOfCharacterSetToIdentifierL(_L8("DUMMY CHARACTER SET"), fileServerSession)==0x000eb205);
	test(characterSetConverter->ConvertMibEnumOfCharacterSetToIdentifierL(9415824, fileServerSession)==0x000eb205);
	test(characterSetConverter->ConvertMibEnumOfCharacterSetToIdentifierL(103, fileServerSession)==KCharacterSetIdentifierUtf7);
	test(characterSetConverter->ConvertMibEnumOfCharacterSetToIdentifierL(104, fileServerSession)==KCharacterSetIdentifierUtf7);
	test(characterSetConverter->ConvertMibEnumOfCharacterSetToIdentifierL(18, fileServerSession)==KCharacterSetIdentifierEucJpPacked);
	test(characterSetConverter->ConvertMibEnumOfCharacterSetToIdentifierL(17, fileServerSession)==KCharacterSetIdentifierShiftJis);
	test(characterSetConverter->ConvertMibEnumOfCharacterSetToIdentifierL(2026, fileServerSession)==KCharacterSetIdentifierBig5);
	test(characterSetConverter->ConvertMibEnumOfCharacterSetToIdentifierL(57, fileServerSession)==KCharacterSetIdentifierGbk);
	test(characterSetConverter->ConvertMibEnumOfCharacterSetToIdentifierL(2025, fileServerSession)==KCharacterSetIdentifierGbk);
	test(characterSetConverter->ConvertMibEnumOfCharacterSetToIdentifierL(4, fileServerSession)==KCharacterSetIdentifierIso88591);
	test(characterSetConverter->ConvertMibEnumOfCharacterSetToIdentifierL(1000, fileServerSession)==KCharacterSetIdentifierUcs2);
	test(characterSetConverter->ConvertStandardNameOfCharacterSetToIdentifierL(_L8("UTF-8"), fileServerSession)==KCharacterSetIdentifierUtf8);
	test(characterSetConverter->ConvertStandardNameOfCharacterSetToIdentifierL(_L8("windows-1252"), fileServerSession)==KCharacterSetIdentifierCodePage1252);
	test(characterSetConverter->ConvertStandardNameOfCharacterSetToIdentifierL(_L8("iso-ir-58"), fileServerSession)==KCharacterSetIdentifierGbk);
	test(characterSetConverter->ConvertStandardNameOfCharacterSetToIdentifierL(_L8("chinese"), fileServerSession)==KCharacterSetIdentifierGbk);
	test(characterSetConverter->ConvertStandardNameOfCharacterSetToIdentifierL(_L8("CHINESE"), fileServerSession)==KCharacterSetIdentifierGbk);
	test(characterSetConverter->ConvertStandardNameOfCharacterSetToIdentifierL(_L8("Big5"), fileServerSession)==KCharacterSetIdentifierBig5);
	test(characterSetConverter->ConvertStandardNameOfCharacterSetToIdentifierL(_L8("csISO2022JP"), fileServerSession)==KCharacterSetIdentifierIso2022Jp);
	test(characterSetConverter->ConvertStandardNameOfCharacterSetToIdentifierL(_L8("JIS_Encoding"), fileServerSession)==KCharacterSetIdentifierJis);
	test(characterSetConverter->ConvertCharacterSetIdentifierToMibEnumL(KCharacterSetIdentifierEucJpPacked, fileServerSession)==18);
	test(characterSetConverter->ConvertCharacterSetIdentifierToMibEnumL(KCharacterSetIdentifierUtf7, fileServerSession)==103);
	test(characterSetConverter->ConvertCharacterSetIdentifierToMibEnumL(KCharacterSetIdentifierGbk, fileServerSession)==113);
	HBufC8* const standardName1=characterSetConverter->ConvertCharacterSetIdentifierToStandardNameL(KCharacterSetIdentifierUtf8, fileServerSession);
	test(*standardName1==_L8("UTF-8"));
	delete standardName1;
	HBufC8* const standardName2=characterSetConverter->ConvertCharacterSetIdentifierToStandardNameL(KCharacterSetIdentifierShiftJis, fileServerSession);
	test(*standardName2==_L8("Shift_JIS"));
	delete standardName2;
	HBufC8* const standardName3=characterSetConverter->ConvertCharacterSetIdentifierToStandardNameL(KCharacterSetIdentifierBig5, fileServerSession);
	test(*standardName3==_L8("Big5"));
	delete standardName3;
	CleanupStack::PopAndDestroy(1); // characterSetConverter

	//Add more test steps for DEF085268. When a related charset ID with Name is not found in rom drive
	//and system hasn't been scanned yet, it will rescan the system. Otherwise, it will return 0.
	characterSetConverter=CCnvCharacterSetConverter::NewLC();
	test(characterSetConverter->ConvertStandardNameOfCharacterSetToIdentifierL(_L8("NONEXIST CHARACTER SET 1"), fileServerSession)==0);
	test(characterSetConverter->ConvertStandardNameOfCharacterSetToIdentifierL(_L8("DUMMY CHARACTER SET"), fileServerSession)==0x000eb205);
	test(characterSetConverter->ConvertStandardNameOfCharacterSetToIdentifierL(_L8("NONEXIST CHARACTER SET 2"), fileServerSession)==0);
	CleanupStack::PopAndDestroy(1); // characterSetConverter

	//Add more test steps for DEF085268. When a related charset name with ID is not found in rom drive
	//and system hasn't been scanned yet, it will rescan the system. Otherwise, it will return NULL.
	characterSetConverter=CCnvCharacterSetConverter::NewLC();
	HBufC8* const standardName4=characterSetConverter->ConvertCharacterSetIdentifierToStandardNameL(0xFFFFFFFE, fileServerSession);
	test(standardName4==NULL);
	delete standardName4;
	HBufC8* const standardName5=characterSetConverter->ConvertCharacterSetIdentifierToStandardNameL(0x000eb205, fileServerSession);
	test(*standardName5==_L8("Dummy character set"));
	delete standardName5;
	HBufC8* const standardName6=characterSetConverter->ConvertCharacterSetIdentifierToStandardNameL(0xFFFFFFFF, fileServerSession);
	test(standardName6==NULL);
	delete standardName6;
	CleanupStack::PopAndDestroy(1); // characterSetConverter


	//Add more test steps for DEF085268. When a related charset ID with MibEnum is not found in rom drive
	//and system hasn't been scanned yet, it will rescan the system. Otherwise, it will return 0.
	characterSetConverter=CCnvCharacterSetConverter::NewLC();
	test(characterSetConverter->ConvertMibEnumOfCharacterSetToIdentifierL(9999998, fileServerSession)==0);
	test(characterSetConverter->ConvertMibEnumOfCharacterSetToIdentifierL(9415824, fileServerSession)==0x000eb205);
	test(characterSetConverter->ConvertMibEnumOfCharacterSetToIdentifierL(9999999, fileServerSession)==0);
	CleanupStack::PopAndDestroy(1); // characterSetConverter

	//Add more test steps for DEF085268. When a related charset MibEnum with ID is not found in rom drive
	//and system hasn't been scanned yet, it will rescan the system. Otherwise, it will return 0.
	characterSetConverter=CCnvCharacterSetConverter::NewLC();
	test(characterSetConverter->ConvertCharacterSetIdentifierToMibEnumL(0xFFFFFFFE, fileServerSession)==0);
	test(characterSetConverter->ConvertCharacterSetIdentifierToMibEnumL(0x000eb205, fileServerSession)==9415824);
	test(characterSetConverter->ConvertCharacterSetIdentifierToMibEnumL(0xFFFFFFFF, fileServerSession)==0);
	CleanupStack::PopAndDestroy(2); // characterSetConverter and file session

	}

CT_SNM::CT_SNM()
    {
    SetTestStepName(KTestStep_T_SNM);
    }

TVerdict CT_SNM::doTestStepL()
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

