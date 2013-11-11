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

#ifndef __T_WIN1254_H__
#define __T_WIN1254_H__

//#include <w32std.h>
#include <test/testexecutestepbase.h>

class CT_WIN1254 : public CTestStep
    {
public:
    CT_WIN1254();
protected:  
    TVerdict doTestStepL();
private:
    void DoE32MainL();
    void ReadDescL(TDes8& aDes, const TDesC& aFilename, RFs& aFs);
    void Merge_Big(TDesC8& aSource, TDes16& aTarget);
    };

_LIT(KTestStep_T_WIN1254, "T_WIN1254");

#endif //__T_WIN1254_H__
