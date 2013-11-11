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

#ifndef __T_PORTUGUESESINGLE_H__
#define __T_PORTUGUESESINGLE_H__

//#include <w32std.h>
#include <test/testexecutestepbase.h>

class CT_PORTUGUESESINGLE : public CTestStep
    {
public:
    CT_PORTUGUESESINGLE();
protected:  
    TVerdict doTestStepL();
private:
    void DoE32MainL();
    void ConvertOutOfRangeForeignCharacters();
    void ConvertIllegalForeignCharacter();
    void ConvertFromForeignToUnicodeL();
    void ConvertFromUnicodeToForeignL();
    void PrepareNewConverterL();
    void ListAlltheConvertersL();
    void Unicode_Big(TDesC8& aSource, TDes16& aTarget);
    void ReadDescL(TDes8& aDes, const TDesC& aFilename, RFs& aFs);   
    };

_LIT(KTestStep_T_PORTUGUESESINGLE, "T_PORTUGUESESINGLE");

#endif //__T_PORTUGUESESINGLE_H__
