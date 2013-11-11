/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include "t_koi8r.h"

const TUint KCharacterSetIdentifier=KCharacterSetIdentifierKOI8R;

#ifdef __WINS__
_LIT(KInputUnicodeFilename, "c:\\test\\data\\KOI8R_uni_input.dat");
_LIT(KInputForeignFilename, "c:\\test\\data\\KOI8R_for_input.dat");
_LIT(KExpectUnicodeFilename, "c:\\test\\data\\KOI8R_uni_expect.dat");
_LIT(KExpectForeignFilename, "c:\\test\\data\\KOI8R_for_expect.dat");
#else
_LIT(KInputUnicodeFilename, "z:\\test\\data\\KOI8R_uni_input.dat");
_LIT(KInputForeignFilename, "z:\\test\\data\\KOI8R_for_input.dat");
_LIT(KExpectUnicodeFilename, "z:\\test\\data\\KOI8R_uni_expect.dat");
_LIT(KExpectForeignFilename, "z:\\test\\data\\KOI8R_for_expect.dat");
#endif

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

void CT_KOI8R::ReadDescL(TDes8& aDes, const TDesC& aFilename, RFs& aFs)
	{
	RFile file;
	TInt err = file.Open(aFs, aFilename, EFileRead);
	test(err == KErrNone);
	CleanupClosePushL(file);	
	err = file.Read(aDes);
	test(err == KErrNone);
	CleanupStack::PopAndDestroy(&file);
	}

void CT_KOI8R::Merge_Big(TDesC8& aSource, TDes16& aTarget)
	{
	TInt length = aSource.Length();
	TInt i = 0;
	for(i=0;i<length-1;i++)
		{
		TInt64 temp = *(aSource.Ptr()+(i))*16*16 + *(aSource.Ptr()+i+1);
		aTarget.Append(temp);
		i++;
		}	
	}
        
void CT_KOI8R::DoE32MainL()
	{
	RFs fileServerSession;
	CleanupClosePushL(fileServerSession);
	User::LeaveIfError(fileServerSession.Connect());
	CCnvCharacterSetConverter* characterSetConverter=CCnvCharacterSetConverter::NewLC();
	CArrayFix<CCnvCharacterSetConverter::SCharacterSet>* arrayOfCharacterSetsAvailable=\
		CCnvCharacterSetConverter::CreateArrayOfCharacterSetsAvailableLC(fileServerSession);
	
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
	
	INFO_PRINTF1(_L("Encoding from Unicode to Foreign"));
	characterSetConverter->PrepareToConvertToOrFromL(KCharacterSetIdentifier, *arrayOfCharacterSetsAvailable, fileServerSession);
	TBuf8<508> temp;
	TBuf16<254> originalUnicode;
	TBuf8<254> generatedForeign;
	TBuf16<254> generatedUnicode;
	ReadDescL(temp, KInputUnicodeFilename, fileServerSession);
	Merge_Big(temp, originalUnicode);
    test(characterSetConverter->ConvertFromUnicode(generatedForeign, originalUnicode) == 0);
	ReadDescL(temp, KExpectForeignFilename, fileServerSession);
	test(generatedForeign == temp);

	INFO_PRINTF1(_L("Encoding from Foreign to Unicode"));	
	ReadDescL(generatedForeign, KInputForeignFilename, fileServerSession);
	TInt state=CCnvCharacterSetConverter::KStateDefault;
	test( 0 == characterSetConverter->ConvertToUnicode(generatedUnicode, generatedForeign, state));
	ReadDescL(temp, KExpectUnicodeFilename, fileServerSession);
	originalUnicode.Zero();
	Merge_Big(temp, originalUnicode); 
	test(generatedUnicode == originalUnicode);
	
	CleanupStack::PopAndDestroy(3);
    }

CT_KOI8R::CT_KOI8R()
    {
    SetTestStepName(KTestStep_T_KOI8R);
    }

TVerdict CT_KOI8R::doTestStepL()
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
