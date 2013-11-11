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

#ifndef __T_KOI8U_H__
#define __T_KOI8U_H__

//#include <w32std.h>
#include <test/testexecutestepbase.h>

class CT_KOI8U : public CTestStep
    {
public:
    CT_KOI8U();
protected:  
    TVerdict doTestStepL();
private:
    void DoE32MainL();
    void ReadDescL(TDes8& aDes, const TDesC& aFilename, RFs& aFs);
    void Merge_Big(TDesC8& aSource, TDes16& aTarget);
    
    };

_LIT(KTestStep_T_KOI8U, "T_KOI8U");

#endif //__T_KOI8U_H__
