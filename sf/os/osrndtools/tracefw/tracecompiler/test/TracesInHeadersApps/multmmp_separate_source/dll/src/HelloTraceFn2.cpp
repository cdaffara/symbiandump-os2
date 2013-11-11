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

 
#include "HelloTraceFn2.h"
#include "HelloTraceFn2Dup.h"
#include "HelloTraceFn2Dup2.inl"
#include "HelloTracePanic.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "HelloTraceFn2Traces.h"
#endif

CHelloTraceFn* CHelloTraceFn::NewLC()
    {
    CHelloTraceFn* self = new (ELeave) CHelloTraceFn;
    CleanupStack::PushL(self);
    return self;
    }

CHelloTraceFn* CHelloTraceFn::NewL()
    {
    CHelloTraceFn* self = CHelloTraceFn::NewLC();
    CleanupStack::Pop(self);
    return self;
    }

CHelloTraceFn::CHelloTraceFn()
    {
    OstTrace1( TRACE_NORMAL, CHELLOTRACEFN_CHELLOTRACEFN_CTOR, "[0x%08x] Constructor", this );    
    }

CHelloTraceFn::~CHelloTraceFn()
    {
    OstTrace1( TRACE_NORMAL, CHELLOTRACEFN_CHELLOTRACEFN_DTOR, "[0x%08x] Destructor", this );    
    }

void CHelloTraceFn::Simple()
    {
    OstTraceFunctionEntry1( CHELLOTRACEFN_SIMPLE_ENTRY, this );
    // ...
    OstTraceFunctionExit1( CHELLOTRACEFN_SIMPLE_EXIT, this );
    }

void CHelloTraceFn::OutputsParams(TInt aParam, TFnEnum aEnumParam)
    {
    OstTraceFunctionEntryExt( CHELLOTRACEFN_OUTPUTSPARAMS_ENTRY, this );
    Simple();
    OstTraceFunctionExit1( CHELLOTRACEFN_OUTPUTSPARAMS_EXIT, this );
    }

void CHelloTraceFn::StaticOutputsParamsL(TInt aParam)
    {
    OstTraceFunctionEntryExt( CHELLOTRACEFN_STATICOUTPUTSPARAMS_ENTRY, NULL );
    CHelloTraceFn* example = CHelloTraceFn::NewLC();
    example->Simple();    
    CleanupStack::PopAndDestroy(example);
    OstTraceFunctionExit0( CHELLOTRACEFN_STATICOUTPUTSPARAMS_EXIT );    
    }

void CHelloTraceFn::OutputsSignedIntegers(TInt8 aParam8,   TInt16 aParam16, 
                                          TInt32 aParam32, TInt64 aParam64)
    {
    OstTraceFunctionEntryExt( CHELLOTRACEFN_OUTPUTSSIGNEDINTEGERS_ENTRY, this );
    // ...
    OstTraceFunctionExit1( CHELLOTRACEFN_OUTPUTSSIGNEDINTEGERS_EXIT, this );
    }

void CHelloTraceFn::OutputsUnsignedIntegers(TUint8 aParam8,   TUint16 aParam16, 
                                            TUint32 aParam32, TUint64 aParam64)
    {
    OstTraceFunctionEntryExt( CHELLOTRACEFN_OUTPUTSUNSIGNEDINTEGERS_ENTRY, this );
    // ...
    OstTraceFunctionExit1( CHELLOTRACEFN_OUTPUTSUNSIGNEDINTEGERS_EXIT, this );
    }


TInt CHelloTraceFn::OutputsTIntReturnValue()
    {
    OstTraceFunctionEntry1( CHELLOTRACEFN_OUTPUTSTINTRETURNVALUE_ENTRY, this );
    TInt ret = KErrNone;
    // ...
    OstTraceFunctionExitExt( CHELLOTRACEFN_OUTPUTSTINTRETURNVALUE_EXIT, this, ret );
    return ret;
    }

TInt64 CHelloTraceFn::OutputsTInt64ReturnValue()
    {
    OstTraceFunctionEntry1( CHELLOTRACEFN_OUTPUTSTINT64RETURNVALUE_ENTRY, this );
    TInt64 ret = (TInt64) 2 << 40;
    // ...
    OstTraceFunctionExitExt( CHELLOTRACEFN_OUTPUTSTINT64RETURNVALUE_EXIT, this, ret );
    return ret;
    }

CHelloTraceFn::TFnEnum CHelloTraceFn::OutputsTFnEnumReturnValue()
    {
    OstTraceFunctionEntry1( CHELLOTRACEFN_OUTPUTSTFNENUMRETURNVALUE_ENTRY, this );
    TFnEnum ret = EExample1;
    // ...
    OstTraceFunctionExitExt( CHELLOTRACEFN_OUTPUTSTFNENUMRETURNVALUE_EXIT, this, ret );
    return ret;    
    }

CActive* CHelloTraceFn::OutputsUnknownPtrType(CActive* aActive)
    {
    OstTraceFunctionEntryExt( CHELLOTRACEFN_OUTPUTSUNKNOWNPTRTYPE_ENTRY, this );    
    // ...
    OstTraceFunctionExitExt( CHELLOTRACEFN_OUTPUTSUNKNOWNPTRTYPE_EXIT, this, (TUint) aActive );
    return aActive;
    }

CActive& CHelloTraceFn::OutputsUnknownRefType(CActive& aActive)
    {
    OstTraceFunctionEntryExt( CHELLOTRACEFN_OUTPUTSUNKNOWNREFTYPE_ENTRY, this );
    // ...
    OstTraceFunctionExitExt( CHELLOTRACEFN_OUTPUTSUNKNOWNREFTYPE_EXIT, this, (TUint) &aActive );    
    return aActive;
    }

// The following doesn't compile as the Trace Compiler doesn't generate
// the correct OstTraceGenExt(...) function for it
//void CHelloTraceFn::OutputMissingParamsBug(TInt /*aCommentParam*/, TInt)
//    {
//    OstTraceFunctionEntryExt( CHELLOTRACEFN_OUTPUTMISSINGPARAMSBUG_ENTRY, this );
//    // ...
//    OstTraceFunctionExit1( CHELLOTRACEFN_OUTPUTMISSINGPARAMSBUG_EXIT, this );    
//    }

// The following compiles and works but outputs the following warning
// warning: HelloTraceFn.cpp, line xxx: Parameter did not have a name and was not added to trace
// The fact that this cannot be suppressed is a known problem we're working on.
void CHelloTraceFn::OutputMissingParams(TInt aUsedParam, TInt /*aCommentParam*/, TInt)
    {
    OstTraceFunctionEntryExt( CHELLOTRACEFN_OUTPUTMISSINGPARAMS_ENTRY, this );
    // ...
    OstTraceFunctionExit1( CHELLOTRACEFN_OUTPUTMISSINGPARAMS_EXIT, this );
    }

// The following doesn't work as the Trace Compiler outputs:
// error: HelloTraceFn.cpp, line xxx: Class / function name for trace could not be determined x3
//#ifdef _DEBUG
//void CHelloTraceFn::PreProcessedFn(TInt aDbgParam)
//    {
//    OstTraceFunctionEntryExt( PRE_PROCESSED_ENTRTY_1, this );
//#else
//void CHelloTraceFn::PreProcessedFn()
//    {
//    OstTraceFunctionEntry1( PRE_PROCESSED_ENTRTY_2, this );
//#endif
//    Simple();
//    OstTraceFunctionExit1( CHELLOTRACEFN_PREPROCESSEDFN_EXIT, this );
//    }

// Workaround example code until a proper fix is provided
#ifdef _DEBUG
void CHelloTraceFn::PreProcessedFn(TInt aDbgParam)
    {
    OstTraceExt2( TRACE_FLOW, CHELLOTRACEFN_PREPROCESSEDFN_ENTRY_1, "CHelloTraceFn::PreProcessedFn: > CHelloTraceFn::PreProcessedFn;aDbgParam=%d;this=%x", (TInt) aDbgParam, (TUint) this);
#else
void CHelloTraceFn::PreProcessedFn()
    {
    OstTrace1( TRACE_FLOW, CHELLOTRACEFN_PREPROCESSEDFN_ENTRY_2, "CHelloTraceFn::PreProcessedFn: > CHelloTraceFn::PreProcessedFn;this=%x", (TUint) this);
#endif
    Simple();
    OstTrace1( TRACE_FLOW, CHELLOTRACEFN_PREPROCESSEDFN_EXIT, "CHelloTraceFn::PreProcessedFn: < CHelloTraceFn::PreProcessedFn;this=%x", (TUint) this);
    }

