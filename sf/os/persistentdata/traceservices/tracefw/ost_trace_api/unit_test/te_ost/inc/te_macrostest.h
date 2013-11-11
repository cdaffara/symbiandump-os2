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

#ifndef __TE_MACROSTEST_H__
#define __TE_MACROSTEST_H__

#include <test/testexecutestepbase.h>
#include "te_suite_step_base.h"

_LIT(KOstMacrosEnabledStep, "COstMacrosEnabledStep");
const TInt KTraceIds = 20;
#define  MyTestGroupId 130
#define  MyTestTraceId 8519680

#define  MyTestTraceId1 MyTestTraceId
#define  MyTestTraceId2 MyTestTraceId
#define  MyTestTraceId3 MyTestTraceId
#define  MyTestTraceId4 MyTestTraceId
#define  MyTestTraceId5 MyTestTraceId
#define  MyTestTraceId6 MyTestTraceId
#define  MyTestTraceId7 MyTestTraceId
#define  MyTestTraceId8 MyTestTraceId
#define  MyTestTraceId9 MyTestTraceId
#define  MyTestTraceId10 MyTestTraceId
#define  MyTestTraceId11 MyTestTraceId
#define  MyTestTraceId12 MyTestTraceId
#define  MyTestTraceId13 MyTestTraceId
#define  MyTestTraceId14 MyTestTraceId
#define  MyTestTraceId15 MyTestTraceId
#define  MyTestTraceId16 MyTestTraceId
#define  MyTestTraceId17 MyTestTraceId
#define  MyTestTraceId18 MyTestTraceId
#define  MyTestTraceId19 MyTestTraceId
#define  MyTestTraceId20 MyTestTraceId
#define  MyTestTraceId21 MyTestTraceId
#define  MyTestTraceId22 MyTestTraceId
#define  MyTestTraceId23 MyTestTraceId
#define  MyTestTraceId24 MyTestTraceId
#define  MyTestTraceId25 MyTestTraceId
#define  MyTestTraceId26 MyTestTraceId
#define  MyTestTraceId27 MyTestTraceId, aParamForOstTraceFunctionEntryExt
_LIT(KDefaultTraceText, "Test Trace");
_LIT8(KDefaultTraceText8, "Test trace");


class COstMacrosEnabledStep : public CTe_UTraceSuiteStepBase
    {
public:
    // Constructor and destructor
    COstMacrosEnabledStep();
    ~COstMacrosEnabledStep();

    //from CTe_ULoggerMCLSuiteStepBase
    virtual TVerdict doTestStepPreambleL();
    virtual TVerdict doTestStepL();
    virtual TVerdict doTestStepPostambleL();

    //testing methods
    virtual TInt TraceContentTestL(TUint32 aTraceId, TBool aCheckParam1, TBool aCheckBigData, TInt aGenFuncNum=0);

    //data
protected:
    RBTrace iBtrace;
    TUint32 iParams[5];
    TBuf8<32> iTraceDataLong;

private:
    template<typename T> T ReadTypeFromBuffer(const TUint8* aData, TInt& aOffset);
    TInt BasicMacroTestL(const TInt8 aParamForOstTraceFunctionEntryExt=0);
    TUint32 ExtractId(TUint32 aId, TInt8 aParamForOstTraceFunctionEntryExt);
    TInt8 ExtractFunctionParameter(TUint32 aId, TInt8 aParamForOstTraceFunctionEntryExt);
    };

#endif


