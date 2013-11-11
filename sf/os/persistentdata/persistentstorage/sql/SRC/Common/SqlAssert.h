// Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __SQLASSERT_H__
#define __SQLASSERT_H__

#include <e32std.h>
#include "SqlPanic.h"   //TSqlPanic

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Enable _SQLPROFILER if you want to use the TSqlResourceProfiler functions. Do not forget the same macro declaration in os_symbian.cpp file.

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//All macros in this header will have a non-void definition only if the OST_TRACE_COMPILER_IN_USE macro
//is defined. 
//In order to get the traces enabled, the OST_TRACE_COMPILER_IN_USE macro has to be defined in
//OstTraceDefinitions.h file. 
//After that, the trace output can be redirected by defining _SQL_RDEBUG_PRINT or specific categories
//of traces can be enabled/disabled.

//Enable _SQL_RDEBUG_PRINT if you want to redirect the tracing output via RDebug::Print()
//#define _SQL_RDEBUG_PRINT

//Enable _SQL_BORDER_TRACE_ENABLED if you want to get the entry/exit traces compiled in the binary
//#define _SQL_BORDER_TRACE_ENABLED

//Enable _SQL_INTERNALS_TRACE_ENABLED if you want to get the internal traces compiled in the binary
//#define _SQL_INTERNALS_TRACE_ENABLED

//Enable _SQL_BUR_TRACE_ENABLED if you want to get the backup&restore traces compiled in the binary
//#define _SQL_BUR_TRACE_ENABLED

//Enable _SQL_COMPACT_TRACE_ENABLED if you want to get the background compaction traces compiled in the binary
//#define _SQL_COMPACT_TRACE_ENABLED

//Enable _SQL_SESSION_TRACE_ENABLED if you want to get the client and server sessions traces compiled in the binary
//#define _SQL_SESSION_TRACE_ENABLED

//Enable _SQL_AUTHORIZER_TRACE_ENABLED if you want to trace the authorizer parameters
//#define _SQL_AUTHORIZER_TRACE_ENABLED

//Enable _SQL_BLOB_TRACE_ENABLED if you want to trace the server side BLOB calls
//#define _SQL_BLOB_TRACE_ENABLED

#ifdef _DEBUG
    //Enable _SQL_PANIC_TRACE_ENABLED if you want to get more detailed output regarding panics
    //#define _SQL_PANIC_TRACE_ENABLED

    //Enable _SQL_LEAVE_TRACE_ENABLED if you want to get more detailed output regarding leaving locations
    //#define _SQL_LEAVE_TRACE_ENABLED
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Wrap every string (file name, file path, etc.) you want to trace, in a __SQLPRNSTR()/__SQLPRNSTR8() macro. 
//There is a difference how RDebug::Print() and OstTraceExt<n>() work.
#if defined _SQL_RDEBUG_PRINT
    const TInt KSqlMaxPrnStrLen = 512;    
    #define __SQLPRNSTR(des)  &des
    const TDesC* SqlDes8to16Ptr(const TDesC8& aDes, TDes& aOut);
	#define __SQLPRNSTR8(des, out)  SqlDes8to16Ptr(des, out) 
#else
    #define __SQLPRNSTR(des)  des
    #define __SQLPRNSTR8(des, out) des
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Forward declarations
class RMessage2;

/**
Set of useful functions to print diagnostic messages on the console when an error/leaving occurs.

@internalComponent
*/
class TSqlUtil
	{
	friend void UtilFileNameTest();
	
public:
	static void GetTimeStr(TDes& aWhere);
	static TInt Panic(const TText* aFile, TInt aLine, TInt aPanicCode, TUint aHandle);
	static void Leave(const TText* aFile, TInt aLine, TInt aError, TUint aHandle);
	static TInt LeaveIfError(const TText* aFile, TInt aLine, TInt aError, TUint aHandle);
	static void* LeaveIfNull(const TText* aFile, TInt aLine, void* aPtr, TUint aHandle);
	static TInt PanicClientL(const TText* aFile, TInt aLine, const RMessage2& aMessage, TInt aPanicCode, TUint aHandle);
	
private:
	static TPtrC FileName(const TText* aFile);
	
	};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define __SQLSTRING(str) _S(str)

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//__SQLPANIC/__SQLPANIC2 macro is used for printing out additional information when panic occurs in SQL:
//source file name, line number, "this" pointer, panic category.
#define __SQLPANIC(aPanicCode)      TSqlUtil::Panic(__SQLSTRING(__FILE__), __LINE__, aPanicCode, (TUint)this)
#define __SQLPANIC2(aPanicCode)     TSqlUtil::Panic(__SQLSTRING(__FILE__), __LINE__, aPanicCode, 0)

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//This macro should be used to leave with "err" error code.
//In debug mode prints the file name and the line number where the leaving occurs and then leaves.
//In release mode only leaves.
#define __SQLLEAVE(err)             TSqlUtil::Leave(__SQLSTRING(__FILE__), __LINE__, err, (TUint)this)
#define __SQLLEAVE2(err)            TSqlUtil::Leave(__SQLSTRING(__FILE__), __LINE__, err, 0)

//This macro should be used to leave with "err" error code, if the error code is negative.
//In debug mode prints the file name and the line number where the leaving occurs and then leaves.
//In release mode only leaves.
#define __SQLLEAVE_IF_ERROR(err)    TSqlUtil::LeaveIfError(__SQLSTRING(__FILE__), __LINE__, err, (TUint)this)
#define __SQLLEAVE_IF_ERROR2(err)   TSqlUtil::LeaveIfError(__SQLSTRING(__FILE__), __LINE__, err, 0)

//This macro should be used to leave with KErrNoMemory if "ptr" argument is NULL.
//In debug mode prints the file name and the line number where the leaving occurs and then leaves.
//In release mode only leaves.
#define __SQLLEAVE_IF_NULL(ptr)     TSqlUtil::LeaveIfNull(__SQLSTRING(__FILE__), __LINE__, ptr, (TUint)this)

//This macro should be used to panic the client and leave if "expr" condition is not satisfied.
//In debug mode prints the file name and the line number where the leaving occurs and then 
//panics the client and leaves.
//In release mode only panics the client and leaves.
#define __SQLPANIC_CLIENT(expr, msg, panicCode)  (void)((expr) || TSqlUtil::PanicClientL(__SQLSTRING(__FILE__), __LINE__, msg, panicCode, (TUint)this))
#define __SQLPANIC_CLIENT2(expr, msg, panicCode) (void)((expr) || TSqlUtil::PanicClientL(__SQLSTRING(__FILE__), __LINE__, msg, panicCode, 0))

//===================================================================================

#ifdef _SQL_BORDER_TRACE_ENABLED
    #define SQL_TRACE_BORDER(trace)   trace
#else
    #define SQL_TRACE_BORDER(trace)   do {} while(0)
#endif

#ifdef _SQL_INTERNALS_TRACE_ENABLED
    #define SQL_TRACE_INTERNALS(trace)   trace
#else
    #define SQL_TRACE_INTERNALS(trace)   do {} while(0)
#endif

#ifdef _SQL_BUR_TRACE_ENABLED
    #define SQL_TRACE_BUR(trace)   trace
#else
    #define SQL_TRACE_BUR(trace)   do {} while(0)
#endif

#ifdef _SQL_COMPACT_TRACE_ENABLED
    #define SQL_TRACE_COMPACT(trace)   trace
#else
    #define SQL_TRACE_COMPACT(trace)   do {} while(0)
#endif

#ifdef _SQL_SESSION_TRACE_ENABLED
    #define SQL_TRACE_SESSION(trace)   trace
#else
    #define SQL_TRACE_SESSION(trace)   do {} while(0)
#endif

#ifdef _SQL_AUTHORIZER_TRACE_ENABLED
    #define SQL_TRACE_AUTHORIZER(trace)   trace
#else
    #define SQL_TRACE_AUTHORIZER(trace)   do {} while(0)
#endif

#ifdef _SQL_BLOB_TRACE_ENABLED
    #define SQL_TRACE_BLOB(trace)   trace
#else
    #define SQL_TRACE_BLOB(trace)   do {} while(0)
#endif

//===================================================================================

#endif//__SQLASSERT_H__
