/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
#ifndef __T_AUTODETECT_H__
#define __T_AUTODETECT_H__

//#include <w32std.h>
#include <test/testexecutestepbase.h>
#include <charconv.h>

class CT_AUTODETECT : public CTestStep
    {
public:
    CT_AUTODETECT();
protected:  
    TVerdict doTestStepL();
private:
    void ReadDesc(TDes8& aDes, const TDesC& aFilename, RFs& aFs);
    void TestAutodetectL(const CArrayFix<CCnvCharacterSetConverter::SCharacterSet>& aArrayOfCharacterSetsAvailable, const TDesC8& aSample, const TUint& aResult);
    void TestAutodetect1L( 
            const CArrayFix<CCnvCharacterSetConverter::SCharacterSet>& aArrayOfCharacterSetsAvailable,
            const TDesC8& aSample, 
            const TUint& aResult,
            CCnvCharacterSetConverter* aConverter);
    void TestConvertibleToCharacterL(const TUint aCharsetIdentifier, const CArrayFix<CCnvCharacterSetConverter::SCharacterSet>& aArrayOfCharacterSetsAvailable, const TDesC8& aSample, const TInt flag);
    TInt TestConvertibleToCharacterReturnResultL(const TUint aCharsetIdentifier, const CArrayFix<CCnvCharacterSetConverter::SCharacterSet>& aArrayOfCharacterSetsAvailable, const TDesC8& aSample);
    void TestConvertibleToCharacterWithSpecificResultL(const TUint aCharsetIdentifier, const CArrayFix<CCnvCharacterSetConverter::SCharacterSet>& aArrayOfCharacterSetsAvailable, const TDesC8& aSample, const TInt aResult);
    void TestAutoDetectFromFileL(const TDesC16& aFileName, const TUint& aResult );
    void TestConvertFromFileL(const TDesC16& aFileName, const TUint& aCharacterSetIdentifier );
    void TestAutoDetectionPerformanceL(const CArrayFix<CCnvCharacterSetConverter::SCharacterSet>* aArrayOfCharacterSetsAvailable);
    void TestForUtf8BomL();
    void DoE32MainL();    
    };

_LIT(KTestStep_T_AUTODETECT, "T_AUTODETECT");

#endif //__T_AUTODETECT_H__
