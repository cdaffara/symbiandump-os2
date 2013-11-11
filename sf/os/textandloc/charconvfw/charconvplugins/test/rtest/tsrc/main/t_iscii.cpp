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
#include "t_iscii.h"

const TUint KCharacterSetIdentifier=KCharacterSetIdentifierIscii;

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

void CT_ISCII::TestConversionToUnicodeFromISCII(CCnvCharacterSetConverter& aCharacterSetConverter, const TDesC16& aExpectedUnicode, const TDesC8& aOriginalISCII, TInt aExpectedResult)
    {
    TInt state=CCnvCharacterSetConverter::KStateDefault;
    TBuf16<KBufferLength> generatedUnicode;
    const TInt returnValue=aCharacterSetConverter.ConvertToUnicode(generatedUnicode, aOriginalISCII, state);
    test(returnValue == aExpectedResult );
    test(generatedUnicode==aExpectedUnicode);
    }

void CT_ISCII::TestConversionToUnicodeFromTruncatedISCII(CCnvCharacterSetConverter& aCharacterSetConverter, const TDesC16&, const TDesC8& aOriginalISCII, TInt aExpectedResult)
    {
    TInt state=CCnvCharacterSetConverter::KStateDefault;
    TBuf16<KBufferLength> generatedUnicode;
    const TInt returnValue=aCharacterSetConverter.ConvertToUnicode(generatedUnicode, aOriginalISCII, state);
    test(returnValue != aExpectedResult );
    }

void CT_ISCII::TestConversionFromUnicodeToISCII(CCnvCharacterSetConverter& aCharacterSetConverter, const TDesC16& aOriginalUnicode, const TDesC8& aExpectedISCII, TInt aExpectedResult)
    {
    TBuf8<KBufferLength> generatedISCII;
    const TInt returnValue=aCharacterSetConverter.ConvertFromUnicode(generatedISCII, aOriginalUnicode);
    test(returnValue == aExpectedResult);
    test(generatedISCII==aExpectedISCII);
    }

void CT_ISCII::TestConversion( CCnvCharacterSetConverter& aCharacterSetConverter )
{
    TestConversionFromUnicodeToISCII(aCharacterSetConverter, _L16("\x0079"), _L8("\xef\x42\x79"));
    TestConversionToUnicodeFromISCII(aCharacterSetConverter, _L16("\x0079"), _L8("\x79"));

    TestConversionFromUnicodeToISCII(aCharacterSetConverter, _L16("\x0079\x007a\x0070\x0071\x0072\x0073\x0074\x0075"), _L8("\xef\x42\x79\x7a\x70\x71\x72\x73\x74\x75"));
    TestConversionToUnicodeFromISCII(aCharacterSetConverter, _L16("\x0079\x007a\x0070\x0071\x0072\x0073\x0074\x0075"), _L8("\x79\x7a\x70\x71\x72\x73\x74\x75"));
    TestConversionToUnicodeFromISCII(aCharacterSetConverter, _L16("\x0079\x007a\x0070\x0071\x0072\x0073\x0074\x0075\x0901"), _L8("\x79\x7a\x70\x71\x72\x73\x74\x75\xa1"));
    TestConversionToUnicodeFromISCII(aCharacterSetConverter, _L16("\x0079\x007a\x0070\x0071\x0072\x0073\x0074\x0075\x0901\x093d"), _L8("\x79\x7a\x70\x71\x72\x73\x74\x75\xa1\xea\xe9"));
    
    TestConversionFromUnicodeToISCII(aCharacterSetConverter, _L16("\x0901"), _L8("\xef\x42\xa1"));
    TestConversionToUnicodeFromISCII(aCharacterSetConverter, _L16("\x0901\x0079"), _L8("\xa1\x79"));
    TestConversionToUnicodeFromTruncatedISCII(aCharacterSetConverter, _L16("\x0901"), _L8("\xa1"));
    
    TestConversionFromUnicodeToISCII(aCharacterSetConverter, _L16("\x094d"), _L8("\xef\x42\xe8"));
    TestConversionFromUnicodeToISCII(aCharacterSetConverter, _L16("\x200c"), _L8("\xef\x42\xe8"));
    TestConversionToUnicodeFromISCII(aCharacterSetConverter, _L16("\x094d\x0079"), _L8("\xe8\x79"));
    
    TestConversionFromUnicodeToISCII(aCharacterSetConverter, _L16("\x0950"), _L8("\xef\x42\xa1\xe9"));
}

void CT_ISCII::DoE32MainL()
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
		
	characterSetConverter->PrepareToConvertToOrFromL(KCharacterSetIdentifier, *arrayOfCharacterSetsAvailable, fileServerSession);

    TestConversion( *characterSetConverter );
    
 	CleanupStack::PopAndDestroy(3);
        }

CT_ISCII::CT_ISCII()
    {
    SetTestStepName(KTestStep_T_ISCII);
    }

TVerdict CT_ISCII::doTestStepL()
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
