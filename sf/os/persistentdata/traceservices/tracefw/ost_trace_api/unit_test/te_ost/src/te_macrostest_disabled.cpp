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

#include <d32btrace.h>
#include <opensystemtrace.h>
#include <e32btrace.h>
#include "te_macrostest_disabled.h"
#include "te_testtraces.h"



COstMacrosDisabledStep::COstMacrosDisabledStep()
    {
    SetTestStepName(KOstMacrosEnabledStep);
    }


COstMacrosDisabledStep::~COstMacrosDisabledStep()
    {
    }


TInt COstMacrosDisabledStep::TraceContentTestL(TUint32 /*aTraceId*/, TBool /*aCheckParam1*/, TBool /*aCheckBigData*/, TInt /*aGenFuncNum*/)
    {
    INFO_PRINTF1(_L("*** COstMacrosDisabledStep::TraceContentTestL ***"));
    TInt size = 0;
    TUint8* data;
    User::After(10000); //allow Btrace to collect data

    size = iBtrace.GetData(data);
    if(size)
        {
        const TUint8* endPtr = data+size;
        while(data < endPtr)
            {
            TUint8 category = data[BTrace::ECategoryIndex];
            if(category == EXTRACT_GROUP_ID(MyTestTraceId))
                {
                iBtrace.DataUsed();
                INFO_PRINTF1(_L("ERROR: traces with category KBTraceCategoryOpenSystemTrace should not exists!"));
                return KErrGeneral;
                }
            data = BTrace::NextRecord((TAny*)data);
            }
        }

    return KErrNone;
    }




