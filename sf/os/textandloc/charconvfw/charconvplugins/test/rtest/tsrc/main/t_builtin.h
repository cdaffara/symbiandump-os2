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
#ifndef __T_BUILTIN_H__
#define __T_BUILTIN_H__

//#include <w32std.h>
#include <test/testexecutestepbase.h>
#include <charconv.h>

class CT_BUILTIN : public CTestStep
    {
public:
    CT_BUILTIN();
protected:  
    TVerdict doTestStepL();
private:
    void DoE32MainL();    
    void ReadDesc(TDes8& aDes, const TDesC& aFilename, RFs& aFs);
    void TestFix_EDNJJUN_4RWH2A(CCnvCharacterSetConverter& aCharacterSetConverter, RFs& aFileServerSession, CArrayFix<CCnvCharacterSetConverter::SCharacterSet>& aArrayOfCharacterSetsAvailable);
    void Test_DEF049749(CCnvCharacterSetConverter& aCharacterSetConverter, RFs& aFileServerSession, CArrayFix<CCnvCharacterSetConverter::SCharacterSet>& aArrayOfCharacterSetsAvailable);
    void Test_DEF053900(CCnvCharacterSetConverter& aCharacterSetConverter, RFs& aFileServerSession, CArrayFix<CCnvCharacterSetConverter::SCharacterSet>& aArrayOfCharacterSetsAvailable);
    
    
    };

_LIT(KTestStep_T_BUILTIN, "T_BUILTIN");

#endif //__T_BUILTIN_H__
