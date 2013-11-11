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

#include <e32std.h>
#include "featmgrtlsdata.h"
#include "featmgrresourcetester.h"

#define UNUSED_ARG(arg) arg = arg

//Defined in featurecontrol.cpp
extern CFeatMgrTlsData* TlsData();

#pragma BullseyeCoverage off

/** 
*/
EXPORT_C void TFeatMgrResourceTester::Mark()
    {
#ifdef EXTENDED_FEATURE_MANAGER_TEST
    CFeatMgrTlsData* tlsData = ::TlsData();
    tlsData->ResourceMark();
#endif    
    }

/** 
*/
EXPORT_C void TFeatMgrResourceTester::Check()
    {
#ifdef EXTENDED_FEATURE_MANAGER_TEST
    CFeatMgrTlsData* tlsData = ::TlsData();
    tlsData->ResourceCheck();
#endif    
    }

/** 
*/
EXPORT_C TInt TFeatMgrResourceTester::Count()
    {
#ifdef EXTENDED_FEATURE_MANAGER_TEST
    CFeatMgrTlsData* tlsData = ::TlsData();
    return tlsData->ResourceCount();
#else
    return -1;
#endif    
    }

/** 
*/
EXPORT_C void TFeatMgrResourceTester::SetHeapFailure(TInt aAllocFailType, TInt aRate)
    {
#ifdef EXTENDED_FEATURE_MANAGER_TEST
    CFeatMgrTlsData* tlsData = ::TlsData();
    return tlsData->SetHeapFailure(aAllocFailType, aRate);
#else    
    UNUSED_ARG(aAllocFailType);
    UNUSED_ARG(aRate);
#endif    
    }

#pragma BullseyeCoverage on
