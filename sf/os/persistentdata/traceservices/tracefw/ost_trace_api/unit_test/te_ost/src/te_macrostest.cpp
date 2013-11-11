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
#include "te_macrostest.h"
#include "te_testtraces.h"



COstMacrosEnabledStep::COstMacrosEnabledStep()
    {
    SetTestStepName(KOstMacrosEnabledStep);
    }


COstMacrosEnabledStep::~COstMacrosEnabledStep()
    {
    }


TVerdict COstMacrosEnabledStep::doTestStepPreambleL()
    {
    INFO_PRINTF1(_L("*** This test case is aimed to test OST macros ***"));

    //defining test values
    //MyTestGroupId = 345; //just to make OST macros call possible
    //MyTestTraceId = 789; //this is what we'll test agains trace data
    //for(TInt i=0; i<KTraceIds; ++i)
      //  MyTestTraceIds = MyTestTraceId;
    iParams[0] = 1;
    iParams[1] = 2;
    iParams[2] = 3;
    iParams[3] = 4;
    iParams[4] = 5;
    iTraceDataLong.Fill('a', 32);

    INFO_PRINTF1(_L("INFO: Configuring BTrace..."));
    if(iBtrace.Open() != KErrNone)
        User::Invariant();

    iBtrace.DataUsed();
    //set btrace filters
    iBtrace.SetFilter(BTrace::ECpuUsage, EFalse); //just to be sure we won't see buffer overflow situation
    iBtrace.SetFilter(EXTRACT_GROUP_ID(MyTestTraceId), ETrue); //group id
    iBtrace.SetFilter2(KOstTraceComponentID,ETrue);
    iBtrace.SetMode(RBTrace::EEnable); //start BTrace

    return TestStepResult();
    }



TVerdict COstMacrosEnabledStep::doTestStepL()
    {
    INFO_PRINTF1(_L("*** COstMacrosEnabledStep::doTestStepL ***"));

    if(BasicMacroTestL() != KErrNone)
        {
        INFO_PRINTF1(_L("ERROR: Basic macro test failed!"));
        SetTestStepResult(EFail);
        }

    return TestStepResult();
    }


TVerdict COstMacrosEnabledStep::doTestStepPostambleL()
    {
    INFO_PRINTF1(_L("*** COstMacrosEnabledStep::doTestStepPostambleL ***"));

    INFO_PRINTF1(_L("INFO: Stopping Btrace..."));
    iBtrace.SetMode(0); //stop BTrace
    iBtrace.Close();

    return TestStepResult();
    }

/**
 * We assume thet in these tests all OST macros are comiled in (OST_TRACE_COMPILER_IN_USE macro is defined)
 */
TInt COstMacrosEnabledStep::BasicMacroTestL(const TInt8 aParamForOstTraceFunctionEntryExt)
    {
    TInt error = KErrNone;
    INFO_PRINTF1(_L("*** COstMacrosEnabledStep::BasicMacroTestL***"));

    INFO_PRINTF1(_L("INFO: executing basic test 1"));
    OstTraceDef0( OST_TRACE_CATEGORY, MyTestGroupId, MyTestTraceId1, KDefaultTraceText );
    if(TraceContentTestL(MyTestTraceId1, EFalse, EFalse) != KErrNone)
        error = KErrGeneral;


    INFO_PRINTF1(_L("INFO: executing basic test 2"));
    OstTraceDef1( OST_TRACE_CATEGORY, MyTestGroupId, MyTestTraceId2, "Test Trace %u", iParams[0] );
    if(TraceContentTestL(MyTestTraceId2, ETrue, EFalse) != KErrNone)
    	error = KErrGeneral;


    INFO_PRINTF1(_L("INFO: executing basic test 3"));
    OstTraceDefData( OST_TRACE_CATEGORY, MyTestGroupId, MyTestTraceId3, KDefaultTraceText, iTraceDataLong.Ptr(), iTraceDataLong.Length() );
    if(TraceContentTestL(MyTestTraceId3, EFalse, ETrue) != KErrNone)
    	error = KErrGeneral;


    INFO_PRINTF1(_L("INFO: executing basic test 4"));
    OstTrace0( MyTestGroupId, MyTestTraceId4, KDefaultTraceText );
    if(TraceContentTestL(MyTestTraceId4, EFalse, EFalse) != KErrNone)
    	error = KErrGeneral;


    INFO_PRINTF1(_L("INFO: executing basic test 5"));
    OstTrace1( MyTestGroupId, MyTestTraceId5, "Test Trace %u", iParams[0] );
    if(TraceContentTestL(MyTestTraceId5, ETrue, EFalse) != KErrNone)
    	error = KErrGeneral;


    INFO_PRINTF1(_L("INFO: executing basic test 6"));
    OstTraceData( MyTestGroupId, MyTestTraceId6, KDefaultTraceText, iTraceDataLong.Ptr(), iTraceDataLong.Length() );
    if(TraceContentTestL(MyTestTraceId6, EFalse, ETrue) != KErrNone)
    	error = KErrGeneral;


    INFO_PRINTF1(_L("INFO: executing basic test 7"));
    OstTraceFunctionEntry0( MyTestTraceId7 );
    if(TraceContentTestL(MyTestTraceId7, EFalse, EFalse) != KErrNone)
    	error = KErrGeneral;


    INFO_PRINTF1(_L("INFO: executing basic test 8"));
    OstTraceFunctionEntry1( MyTestTraceId8, this );
    if(TraceContentTestL(MyTestTraceId8, EFalse, EFalse) != KErrNone)
    	error = KErrGeneral;


    INFO_PRINTF1(_L("INFO: executing basic test 9"));
    OstTraceFunctionExit0( MyTestTraceId9 );
    if(TraceContentTestL(MyTestTraceId9, EFalse, EFalse) != KErrNone)
    	error = KErrGeneral;


    INFO_PRINTF1(_L("INFO: executing basic test 10"));
    OstTraceFunctionExit1( MyTestTraceId10, this );
    if(TraceContentTestL(MyTestTraceId10, EFalse, EFalse) != KErrNone)
    	error = KErrGeneral;


    INFO_PRINTF1(_L("INFO: executing basic test 11"));
    OstTraceEventStart0( MyTestTraceId11, 0 );
    if(TraceContentTestL(MyTestTraceId11, EFalse, EFalse) != KErrNone)
    	error = KErrGeneral;


    INFO_PRINTF1(_L("INFO: executing basic test 12"));
    OstTraceEventStart1( MyTestTraceId12, "Test Trace %u", iParams[0] );
    if(TraceContentTestL(MyTestTraceId12, ETrue, EFalse) != KErrNone)
    	error = KErrGeneral;


    INFO_PRINTF1(_L("INFO: executing basic test 13"));
    OstTraceEventStop( MyTestTraceId13, 0, (TUint32)MyTestTraceId11);
    if(TraceContentTestL(MyTestTraceId13, EFalse, EFalse) != KErrNone)
    	error = KErrGeneral;


    INFO_PRINTF1(_L("INFO: executing basic test 14"));
    OstTraceState0( (TUint32)MyTestTraceId14, "Test State Name", "Test State Value");
    if(TraceContentTestL(MyTestTraceId14, EFalse, EFalse) != KErrNone)
    	error = KErrGeneral;


    INFO_PRINTF1(_L("INFO: executing basic test 15"));
    OstTraceState1( MyTestTraceId15, "Test State Name", "Test State Value", this );
    if(TraceContentTestL(MyTestTraceId15, EFalse, EFalse) != KErrNone)
    	error = KErrGeneral;

    INFO_PRINTF1(_L("INFO: executing basic test 16"));
    OstTraceDefExt1( OST_TRACE_CATEGORY, MyTestGroupId, MyTestTraceId16, "Test Trace %hhd", iParams[0] );


    INFO_PRINTF1(_L("INFO: executing basic test 17"));
    OstTraceDefExt2( OST_TRACE_CATEGORY, MyTestGroupId, MyTestTraceId17, "Test Trace %u %u", iParams[0], iParams[1] );


    INFO_PRINTF1(_L("INFO: executing basic test 18"));
    OstTraceDefExt3( OST_TRACE_CATEGORY, MyTestGroupId, MyTestTraceId18, "Test Trace %u %u %u", iParams[0], iParams[1], iParams[2] );


    INFO_PRINTF1(_L("INFO: executing basic test 19"));
    OstTraceDefExt4( OST_TRACE_CATEGORY, MyTestGroupId, MyTestTraceId19, "Test Trace %u %u %u %u", iParams[0], iParams[1], iParams[2], iParams[3] );


    INFO_PRINTF1(_L("INFO: executing basic test 20"));
    OstTraceDefExt5( OST_TRACE_CATEGORY, MyTestGroupId, MyTestTraceId20, "Test Trace %u %u %u %u %u", iParams[0], iParams[1], iParams[2], iParams[3], iParams[4] );

    INFO_PRINTF1(_L("INFO: executing basic test 21"));
    OstTraceExt1( MyTestGroupId, MyTestTraceId21, "Test Trace %hhd", iParams[0] );
    if(TraceContentTestL(MyTestTraceId21, ETrue, EFalse) != KErrNone)
    	error = KErrGeneral;


    INFO_PRINTF1(_L("INFO: executing basic test 22"));
    OstTraceExt2( MyTestGroupId, MyTestTraceId22, "Test Trace %u %u", iParams[0], iParams[1] );
    if(TraceContentTestL(MyTestTraceId22, EFalse, EFalse, 2) != KErrNone)
    	error = KErrGeneral;


    INFO_PRINTF1(_L("INFO: executing basic test 23"));
    OstTraceExt3( MyTestGroupId, MyTestTraceId23, "Test Trace %u %u %u", iParams[0], iParams[1], iParams[2] );
    if(TraceContentTestL(MyTestTraceId23, EFalse, EFalse, 3) != KErrNone)
    	error = KErrGeneral;


    INFO_PRINTF1(_L("INFO: executing basic test 24"));
    OstTraceExt4( MyTestGroupId, MyTestTraceId24, "Test Trace %u %u %u %u", iParams[0], iParams[1], iParams[2], iParams[3] );
    if(TraceContentTestL(MyTestTraceId24, EFalse, EFalse, 4) != KErrNone)
    	error = KErrGeneral;


    INFO_PRINTF1(_L("INFO: executing basic test 25"));
    OstTraceExt5( MyTestGroupId, MyTestTraceId25, "Test Trace %u %u %u %u %u", iParams[0], iParams[1], iParams[2], iParams[3], iParams[4] );
    if(TraceContentTestL(MyTestTraceId25, EFalse, EFalse, 5) != KErrNone)
    	error = KErrGeneral;


    INFO_PRINTF1(_L("INFO: executing basic test 26"));
    OstTraceFunctionExitExt( MyTestTraceId26, this, error);
    if(TraceContentTestL(MyTestTraceId26, EFalse, EFalse) != KErrNone)
    	error = KErrGeneral;


    INFO_PRINTF1(_L("INFO: executing basic test 27"));
    OstTraceFunctionEntryExt( MyTestTraceId27, (TUint32)this);
      if(TraceContentTestL(ExtractId(MyTestTraceId27), EFalse, EFalse) != KErrNone)
      	error = KErrGeneral;
      if(ExtractFunctionParameter(MyTestTraceId27)!=aParamForOstTraceFunctionEntryExt)
      	error = KErrGeneral;

    return error;
    }


TInt COstMacrosEnabledStep::TraceContentTestL(TUint32 aTraceId, TBool aCheckParam1, TBool aCheckBigData,  TInt aGenFuncNum)
    {
    TInt size = 0;
    TInt error = KErrNone;
    TUint8* data;
    User::After(10000); //allow Btrace to collect data
    size = iBtrace.GetData(data);
    if(size)
        {
        //check trace data content
        TBool categoryOk, contentOk, uidOk;
        categoryOk = contentOk = uidOk = EFalse;
        TInt offset = 0;
        const TUint8* endPtr = data+size;
        while(data < endPtr)
            {
            categoryOk = contentOk = uidOk = EFalse;
            offset = 0;

            //read trace header
            TUint8 flags = data[BTrace::EFlagsIndex];
            TUint8 category = data[BTrace::ECategoryIndex];
            TUint8 subCategory = data[BTrace::ESubCategoryIndex];
            offset += 4;

            if(category==EXTRACT_GROUP_ID(MyTestTraceId) || subCategory==EOstTrace)
                {
                categoryOk = ETrue;

                //skip unwanted trace-metadata
                //read header extension
                if(flags & BTrace::EHeader2Present)
                    ReadTypeFromBuffer<TInt32>(data, offset);
                if(flags & BTrace::ETimestampPresent)
                    ReadTypeFromBuffer<TInt32>(data, offset);
                if(flags & BTrace::ETimestamp2Present)
                    ReadTypeFromBuffer<TInt32>(data, offset);
                if(flags & BTrace::EContextIdPresent)
                    ReadTypeFromBuffer<TInt32>(data, offset);
                if(flags & BTrace::EPcPresent)
                    ReadTypeFromBuffer<TInt32>(data, offset);
                if(flags & BTrace::EExtraPresent)
                    ReadTypeFromBuffer<TInt32>(data, offset);

                //check componentID
                TUint32* traceContent = (TUint32*)(data+offset);
                if(*traceContent == KOstTraceComponentID)
                    uidOk = ETrue;
                else
                    INFO_PRINTF3(_L("ERROR: componentID incorrect: 0x%x vs 0x%x"), (TInt)(*traceContent), KOstTraceComponentID);
                offset+=sizeof(TUint32);

                //check traceID value
                traceContent = (TUint32*)(data+offset);
                if(*traceContent == aTraceId)
                    contentOk = ETrue;
                else
                    INFO_PRINTF3(_L("ERROR: trace content incorrect: 0x%x vs 0x%x"), (TInt)(*traceContent), aTraceId);

                //check parameter 1 passed to OST macro
                if(aCheckParam1)
                    {
                    offset+=sizeof(TUint32);
                    traceContent = (TUint32*)(data+offset);
                    if(*traceContent != iParams[0])
                        {
                        INFO_PRINTF3(_L("ERROR: iParam1 incorrect: 0x%x vs 0x%x"), (TInt)(*traceContent), iParams[0]);
                      	error = KErrGeneral;
                        }
                    }

                //check big data sent via ost macro
                if(aCheckBigData)
                    {
                    offset+=sizeof(TUint32);
                    traceContent = (TUint32*)(data+offset);
                    TPtrC8 ptr((const unsigned char*)traceContent, iTraceDataLong.Length());
                    if(iTraceDataLong.Compare(ptr) != KErrNone)
                        {
                        INFO_PRINTF3(_L("ERROR: data sent via ost macro is incorrect! [%S] vs [%S]"), &ptr, &iTraceDataLong);
                        error = KErrGeneral;
                        }
                    }

                //checking functions generated by Trace compiler
                if(aGenFuncNum > 0)
                    {
                    INFO_PRINTF2(_L("INFO: checking gen num %d"), aGenFuncNum);
                    for(TInt i=0; i<aGenFuncNum; ++i)
                        {
                        offset+=sizeof(TUint32);
                        traceContent = (TUint32*)(data+offset);
                        if(*traceContent != iParams[i])
                            {
                            INFO_PRINTF4(_L("ERROR: iParam %d incorrect: 0x%x vs 0x%x"), i, (TInt)(*traceContent), iParams[i]);
                            error = KErrGeneral;
                            }
                        }
                    }

                }

            if(categoryOk && contentOk && uidOk)
                {
                iBtrace.DataUsed();
                break;
                }

            data = BTrace::NextRecord((TAny*)data);
            }

        iBtrace.DataUsed();
        //final check
        if(!categoryOk || !contentOk || !uidOk)
            {
            INFO_PRINTF4(_L("ERROR: category %d contentOk: %d, uidOk %d"), categoryOk, contentOk, uidOk);
            return KErrGeneral;
            }
        }
    else
        {
        INFO_PRINTF1(_L("ERROR: fatal error!"));
        return KErrGeneral;
        }

    return error;
    }



template<typename T>
T COstMacrosEnabledStep::ReadTypeFromBuffer(const TUint8* aData, TInt& aOffset)
    {
    T p = *((T*)(aData+aOffset));
    aOffset += sizeof(T);
    return p;
    }

TUint32 COstMacrosEnabledStep::ExtractId(TUint32 aId, TInt8 /*aParamForOstTraceFunctionEntryExt*/)
	{
	return aId;
	}
TInt8 COstMacrosEnabledStep::ExtractFunctionParameter(TUint32 /*aId*/, TInt8 aParamForOstTraceFunctionEntryExt)
	{
	return aParamForOstTraceFunctionEntryExt;
	}

