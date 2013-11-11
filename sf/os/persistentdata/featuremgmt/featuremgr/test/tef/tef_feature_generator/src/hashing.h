// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//
 
#if (!defined __TEFEFM_HASHING_H__)
#define __TEFEFM_HASHING_H__

#include <e32base.h>
#include <e32hashtab.h>

class CEFMHashUtil : public CBase
    {
public:
    CEFMHashUtil(){};
    ~CEFMHashUtil();
    TPtrC* GetStringFromNum(TInt aErrNum);
    TInt GetNumFromString(TPtrC aErrMsg);
protected:
    RHashMap<TInt, TPtrC> ihash;
    };

class CEFMTEFAction : public CEFMHashUtil
    {
public:
    CEFMTEFAction();
    };

#endif  // __TEFEFM_HASHING_H__

