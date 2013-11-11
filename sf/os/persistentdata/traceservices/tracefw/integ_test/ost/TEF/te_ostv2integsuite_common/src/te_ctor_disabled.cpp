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
// Test class generating trace data from contructor with/without initialiser list.
// used in (logging) disabled mode.
//

#include "te_ctor.h"
#include "te_ctor_disabledTraces.h"

TTestCtor::TTestCtor( TUint32 aArg1, TUint32 aArg2 )
:
    iNum1( aArg1 ),
    iNum2( aArg2 ),
    iStr( KTestCtorString )
    {
    OstTraceFunctionEntryExt( CTESTCTOR_INITLIST_TRACE, KData1 );
    }

TTestCtor::TTestCtor( TUint32 aArg1 )
    {
    iNum1 =  aArg1;
    iNum2 = 0;
    iStr = KTestCtorString;

    OstTraceFunctionEntryExt( CTESTCTOR_NO_INITLIST_TRACE, KData1 );
    }
