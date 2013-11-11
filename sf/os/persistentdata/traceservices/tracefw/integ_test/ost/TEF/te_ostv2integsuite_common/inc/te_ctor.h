// Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef TE_CTOR
#define TE_CTOR
#include <e32std.h>
#include "te_dataparameters.h"

// dummy/unimportant value, only used for initialisation
_LIT(KTestCtorString, "ABCDE" );

class TTestCtor
    {
public:
    TTestCtor( TUint32 aArg1, TUint32 aArg2 );

    TTestCtor( TUint32 aArg1 );
 
private:
    // define arbitrary data members
    TUint32     iNum1;
    TUint32     iNum2;
    TBuf<10>    iStr;
    };

#endif //TE_CTOR
