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
#include "t_cp949.h"

//  Local Functions
///////////////////////////////////////////////////////////////////////////////////////
/*
Below four plugins all use cp949table.dll, and first three are same converter.
cp949 (code page 949) 0x200100FF
ksc5601 (Korean) 0x200113CD
EUCKR (EUC-KR) 0x2000E526
iso2022kr (Korean) 0x20010101
 */

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

void CT_CP949::TestConversionToUnicodeFromCP949(CCnvCharacterSetConverter& aCharacterSetConverter, const TDesC16& aExpectedUnicode, const TDesC8& aOriginalCP949, TInt aExpectedResult)
    {
    TInt state=CCnvCharacterSetConverter::KStateDefault;
    TBuf16<KBufferLength> generatedUnicode;
    const TInt returnValue=aCharacterSetConverter.ConvertToUnicode(generatedUnicode, aOriginalCP949, state);
    test(returnValue == aExpectedResult );
    test(generatedUnicode==aExpectedUnicode);
    }

void CT_CP949::TestConversionFromUnicodeToCP949(CCnvCharacterSetConverter& aCharacterSetConverter, const TDesC16& aOriginalUnicode, const TDesC8& aExpectedCP949, TInt aExpectedResult)
    {
    TBuf8<KBufferLength> generatedCP949;
    const TInt returnValue=aCharacterSetConverter.ConvertFromUnicode(generatedCP949, aOriginalUnicode);
    test(returnValue == aExpectedResult);
    test(generatedCP949==aExpectedCP949);
    }

void CT_CP949::TestConversion( CCnvCharacterSetConverter& aCharacterSetConverter )
{
    TestConversionFromUnicodeToCP949(aCharacterSetConverter, _L16("\x0079"), _L8("\x79"));
    TestConversionToUnicodeFromCP949(aCharacterSetConverter, _L16("\x0079"), _L8("\x79"));

    TestConversionFromUnicodeToCP949(aCharacterSetConverter, _L16("\xAC02"), _L8("\x81\x41"));
    TestConversionToUnicodeFromCP949(aCharacterSetConverter, _L16("\xAC02"), _L8("\x81\x41"));
}

void CT_CP949::TestConversionIso2022kr( CCnvCharacterSetConverter& aCharacterSetConverter )
{
    TestConversionFromUnicodeToCP949(aCharacterSetConverter, _L16("\x0079"), _L8("\x1b\x24\x43\x0f\x79"));
    TestConversionToUnicodeFromCP949(aCharacterSetConverter, _L16("\x0079"), _L8("\x1b\x24\x43\x0f\x79"));

    TestConversionFromUnicodeToCP949(aCharacterSetConverter, _L16("\xc7b8"), _L8("\x1b\x24\x43\x0e\x20\x0f\x41"));
    TestConversionToUnicodeFromCP949(aCharacterSetConverter, _L16("\xc7b8\x0079"), _L8("\x1b\x24\x43\x0f\xa0\x41\x79"));

    TestConversionFromUnicodeToCP949(aCharacterSetConverter, _L16("\xAC02"), _L8("\x1b\x24\x43\x0f\x81\x41"));
    TestConversionToUnicodeFromCP949(aCharacterSetConverter, _L16("\xAC02"), _L8("\x1b\x24\x43\x0f\x81\x41"));
}

void CT_CP949::DoE32MainL()
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
        
    INFO_PRINTF1(_L("Testing CP949"));
    characterSetConverter->PrepareToConvertToOrFromL(KCharacterSetIdentifierCP949, *arrayOfCharacterSetsAvailable, fileServerSession);
    TestConversion( *characterSetConverter );

    INFO_PRINTF1(_L("Testing ksc5601"));
    characterSetConverter->PrepareToConvertToOrFromL(KCharacterSetIdentifierKsc5601, *arrayOfCharacterSetsAvailable, fileServerSession);
    TestConversion( *characterSetConverter );

    INFO_PRINTF1(_L("Testing EUCKR"));
    characterSetConverter->PrepareToConvertToOrFromL(KCharacterSetIdentifierEUCKR, *arrayOfCharacterSetsAvailable, fileServerSession);
    TestConversion( *characterSetConverter );

    INFO_PRINTF1(_L("Testing Iso2022kr"));
    characterSetConverter->PrepareToConvertToOrFromL(KCharacterSetIdentifierIso2022kr, *arrayOfCharacterSetsAvailable, fileServerSession);
    TestConversionIso2022kr( *characterSetConverter );

    CleanupStack::PopAndDestroy(3);
        }

CT_CP949::CT_CP949()
    {
    SetTestStepName(KTestStep_T_CP949);
    }

TVerdict CT_CP949::doTestStepL()
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

