/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __T_ERROR_H__
#define __T_ERROR_H__

//#include <w32std.h>
#include <test/testexecutestepbase.h>
#include <e32std.h>
#include <utf.h>

typedef TInt (*FConvertFromUtfX)(TDes16& aUnicode, const TDesC8& aUtfX, TInt& aState);

typedef TInt (*FConvertToUtfX)(TDes8& aUtfX, const TDesC16& aUnicode);

class CT_ERROR : public CTestStep
    {
public:
    CT_ERROR();
protected:  
    TVerdict doTestStepL();
private:
    void TestERROR();
    void ExhaustivelyLengthTest(const TDesC16& aUnicode, const TDesC8& aUtfX, FConvertFromUtfX aConvertFromUtfX);
    void ExhaustivelyLengthTest(const TDesC8& aUtfX, const TDesC16& aUnicode, FConvertToUtfX aConvertToUtfX);
    void ExhaustivelyLengthTest(const TDesC16& aUnicode, const TDesC8& aUtf7WithOptionalDirectCharactersEncodedInBase64, const TDesC8& aUtf7WithOptionalDirectCharactersEncodedDirectly, const TDesC8& aUtf8);
    void ExhaustivelyLengthTest(const TDesC16& aUnicode, const TDesC8& aUtf7, const TDesC8& aUtf8);
    void LengthTestGeneratingUtf7();
    void LengthTestGeneratingUtf8();
    };

_LIT(KTestStep_T_ERROR, "T_ERROR");

#endif
