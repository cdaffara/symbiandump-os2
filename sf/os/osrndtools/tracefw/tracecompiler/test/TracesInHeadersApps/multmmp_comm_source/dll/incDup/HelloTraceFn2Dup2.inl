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
 
#ifndef HELLOTRACEFN2DUP2_H_
#define HELLOTRACEFN2DUP2_H_
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "HelloTraceFn2Dup2Traces.h"
#endif

inline void CHelloTraceFn::SimpleDup2()
    {
    OstTraceFunctionEntry1( CHELLOTRACEFN_SIMPLE2_DUP2_ENTRY, this );
    // ...
    OstTraceFunctionExit1( CHELLOTRACEFN_SIMPLE2_DUP2_EXIT, this );
    }

inline void CHelloTraceFn::OutputsParamsDup2(TInt aParam, TFnEnum aEnumParam)
    {
    OstTraceFunctionEntryExt( CHELLOTRACEFN_OUTPUTSPARAMS2_DUP2_ENTRY, this );
    Simple();
    OstTraceFunctionExit1( CHELLOTRACEFN_OUTPUTSPARAMS2_DUP2_EXIT, this );
    }

inline void CHelloTraceFn::StaticOutputsParamsDup2L(TInt aParam)
    {
    OstTraceFunctionEntryExt( CHELLOTRACEFN_STATICOUTPUTSPARAMS2_DUP2_ENTRY, NULL );
    CHelloTraceFn* example = CHelloTraceFn::NewLC();
    example->SimpleDup2();    
    CleanupStack::PopAndDestroy(example);
    OstTraceFunctionExit0( CHELLOTRACEFN_STATICOUTPUTSPARAMS2_DUP2_EXIT );    
    }

inline void CHelloTraceFn::OutputsSignedIntegersDup2(TInt8 aParam8,   TInt16 aParam16, 
                                          TInt32 aParam32, TInt64 aParam64)
    {
    OstTraceFunctionEntryExt( CHELLOTRACEFN_OUTPUTSSIGNEDINTEGERS2_DUP2_ENTRY, this );
    // ...
    OstTraceFunctionExit1( CHELLOTRACEFN_OUTPUTSSIGNEDINTEGERS2_DUP2_EXIT, this );
    }

inline void CHelloTraceFn::OutputsUnsignedIntegersDup2(TUint8 aParam8,   TUint16 aParam16, 
                                            TUint32 aParam32, TUint64 aParam64)
    {
    OstTraceFunctionEntryExt( CHELLOTRACEFN_OUTPUTSUNSIGNEDINTEGERS2_DUP2_ENTRY, this );
    // ...
    OstTraceFunctionExit1( CHELLOTRACEFN_OUTPUTSUNSIGNEDINTEGERS2_DUP2_EXIT, this );
    }


inline TInt CHelloTraceFn::OutputsTIntReturnValueDup2()
    {
    OstTraceFunctionEntry1( CHELLOTRACEFN_OUTPUTSTINTRETURNVALUE2_DUP2_ENTRY, this );
    TInt ret = KErrNone;
    // ...
    OstTraceFunctionExitExt( CHELLOTRACEFN_OUTPUTSTINTRETURNVALUE2_DUP2_EXIT, this, ret );
    return ret;
    }

inline TInt64 CHelloTraceFn::OutputsTInt64ReturnValueDup2()
    {
    OstTraceFunctionEntry1( CHELLOTRACEFN_OUTPUTSTINT64RETURNVALUE2_DUP2_ENTRY, this );
    TInt64 ret = (TInt64) 2 << 40;
    // ...
    OstTraceFunctionExitExt( CHELLOTRACEFN_OUTPUTSTINT64RETURNVALUE2_DUP2_EXIT, this, ret );
    return ret;
    }

inline CHelloTraceFn::TFnEnum CHelloTraceFn::OutputsTFnEnumReturnValueDup2()
    {
    OstTraceFunctionEntry1( CHELLOTRACEFN_OUTPUTSTFNENUMRETURNVALUE2_DUP2_ENTRY, this );
    TFnEnum ret = EExample1;
    // ...
    OstTraceFunctionExitExt( CHELLOTRACEFN_OUTPUTSTFNENUMRETURNVALUE2_DUP2_EXIT, this, ret );
    return ret;    
    }

inline CActive* CHelloTraceFn::OutputsUnknownPtrTypeDup2(CActive* aActive)
    {
    OstTraceFunctionEntryExt( CHELLOTRACEFN_OUTPUTSUNKNOWNPTRTYPE2_DUP2_ENTRY, this );    
    // ...
    OstTraceFunctionExitExt( CHELLOTRACEFN_OUTPUTSUNKNOWNPTRTYPE2_DUP2_EXIT, this, (TUint) aActive );
    return aActive;
    }

inline CActive& CHelloTraceFn::OutputsUnknownRefTypeDup2(CActive& aActive)
    {
    OstTraceFunctionEntryExt( CHELLOTRACEFN_OUTPUTSUNKNOWNREFTYPE2_DUP2_ENTRY, this );
    // ...
    OstTraceFunctionExitExt( CHELLOTRACEFN_OUTPUTSUNKNOWNREFTYPE2_DUP2_EXIT, this, (TUint) &aActive );    
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
inline void CHelloTraceFn::OutputMissingParamsDup2(TInt aUsedParam, TInt /*aCommentParam*/, TInt)
    {
    OstTraceFunctionEntryExt( CHELLOTRACEFN_OUTPUTMISSINGPARAMS2_DUP2_ENTRY, this );
    // ...
    OstTraceFunctionExit1( CHELLOTRACEFN_OUTPUTMISSINGPARAMS2_DUP2_EXIT, this );
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
inline void CHelloTraceFn::PreProcessedFnDup2(TInt aDbgParam)
    {
    OstTraceExt2( TRACE_FLOW, CHELLOTRACEFN_PREPROCESSEDFN2_DUP2_ENTRY_1, "CHelloTraceFn::PreProcessedFn: > CHelloTraceFn::PreProcessedFn;aDbgParam=%d;this=%x", (TInt) aDbgParam, (TUint) this);
#else
    inline void CHelloTraceFn::PreProcessedFnDup2()
    {
    OstTrace1( TRACE_FLOW, CHELLOTRACEFN_PREPROCESSEDFN2_DUP2_ENTRY_2, "CHelloTraceFn::PreProcessedFn: > CHelloTraceFn::PreProcessedFn;this=%x", (TUint) this);
#endif
    SimpleDup2();
    OstTrace1( TRACE_FLOW, CHELLOTRACEFN_PREPROCESSEDFN2_DUP2_EXIT, "CHelloTraceFn::PreProcessedFn: < CHelloTraceFn::PreProcessedFn;this=%x", (TUint) this);
    }

#endif /* HELLOTRACEFN2DUP2_H_ */
