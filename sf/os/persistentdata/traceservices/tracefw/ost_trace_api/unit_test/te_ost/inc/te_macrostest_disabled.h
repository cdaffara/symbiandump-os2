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
// UTrace Unit Tests.
// Created: 15/05/2009
//



/**
 @file te_macrostest.h
 @internalTechnology
 @prototype
*/

#ifndef __TE_MACROSTEST_DISABLED_H__
#define __TE_MACROSTEST_DISABLED_H__

#include <test/testexecutestepbase.h>
#include "te_macrostest.h"

_LIT(KOstMacrosDisabledStep, "COstMacrosDisabledStep");

/**
 *  For this class (binary) macro OST_TRACE_COMPILER_IN_USE is not defined.
 */
class COstMacrosDisabledStep : public COstMacrosEnabledStep
    {
public:    
    // Constructor and destructor
    COstMacrosDisabledStep();
    ~COstMacrosDisabledStep();

    //from CTe_ULoggerMCLSuiteStepBase
    TInt TraceContentTestL(TUint32 aTraceId, TBool aCheckParam1, TBool aCheckBigData, TInt aGenFuncNum=0);
    
    };

#endif


