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


#ifndef __T_PARTIAL_H__
#define __T_PARTIAL_H__

//#include <w32std.h>
#include <test/testexecutestepbase.h>

class CT_PARTIAL : public CTestStep
    {
public:
    CT_PARTIAL();
protected:  
    TVerdict doTestStepL();
private:
    void TestPARTIAL();
    void TestConvertingToUtf(TUint8* aUtfBuffer, TInt aMaximumLengthOfUtfDescriptor, const TDesC16& aUnicode, TBool aBoolParameter, TInt aNumberOfUnicodeItemsExpectedToBeConverted, const TDesC8& aFirstHalfOfUtfExpected, const TDesC8& aSecondHalfOfUtfExpected);
    void TestUtf7StatePreservation(const TDesC8& aUtf7);
    void TestConvertingToUtf(TUint8* aUtfBuffer, TInt aMaximumLengthOfUtfDescriptor, const TDesC16& aUnicode, TInt aNumberOfUnicodeItemsExpectedToBeConverted, const TDesC8& aFirstHalfOfUtfExpected, const TDesC8& aSecondHalfOfUtfExpected);
    };

_LIT(KTestStep_T_PARTIAL, "T_PARTIAL");

#endif
