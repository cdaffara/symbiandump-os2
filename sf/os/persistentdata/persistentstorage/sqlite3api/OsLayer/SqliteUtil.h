// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
#ifndef SQLITEUTIL_H
#define SQLITEUTIL_H

#include <e32std.h>

/**
Panic codes - used by asserts in the OS porting layer and the file buffer.

@see KPanicCategory

@internalComponent
*/
enum TSqlitePanic
	{
	ESqliteOsPanicFsCreationError		= 1,
	ESqliteOsPanicMutexCreationError	= 2,
	ESqliteOsPanicInvalidFs				= 3,
	ESqliteOsPanicNullPls1				= 4,
	ESqliteOsPanicNullPls2				= 5,
	ESqliteOsPanicNullPls3				= 6,
	ESqliteOsPanicNullPls4				= 7,
	ESqliteOsPanicAssert				= 8,
	ESqliteOsPanicMaxKeysExceeded		= 9,
	ESqliteOsPanicBufferSizeExceeded	=10,
	ESqliteOsPanicNullKey				=11,
	ESqliteOsPanicWsdBufSize			=12,
	ESqliteOsPanicWsdEntryCount			=13,
	ESqliteOsPanicInternalError			=19,
	ESqliteOsPanicNullDbFilePtr			=20,
	ESqliteOsPanicInvalidLock			=21,
	ESqliteOsPanicInvalidMutexType		=22,
	ESqliteOsPanicMutexLockCounter		=23,
	ESqliteOsPanicMutexOwner			=24,
	//
	EFBufPanicCapacity					=101,
	EFBufPanicNullBuf					=102,
	EFBufPanicBufLen					=103,
	EFBufPanicFilePos					=104,
	EFBufPanicFileSize					=105,
	EFBufPanicFileHandle				=106,
	EFBufPanicFsHandle					=107,
	EFBufPanicMsgHandle					=108,
	EFBufPanicMsgIndex					=109,
	EFBufPanicFileNameLen				=110,
	EFBufPanicNullThis					=111,
	EFBufPanicDirty						=112,
	EFBufPanicNextReadFilePos			=113,
	EFBufPanicNextReadFilePosHits		=114,
	EFBufPanicFileBlockSize				=115,
	};

////////////////////////////////////////////////////////////////////////////////////////////

//All macros in this header will have a non-void definition only if the OST_TRACE_COMPILER_IN_USE macro
//is defined. 
//In order to get the traces enabled, the OST_TRACE_COMPILER_IN_USE macro has to be defined in
//OstTraceDefinitions.h file. 
//After that, the trace output can be redirected by defining _SQL_RDEBUG_PRINT or specific categories
//of traces can be enabled/disabled.

//Enable _SQLITE_RDEBUG_PRINT if you want to redirect the OS porting layer and file buffer tracing output via RDebug::Print()
//#define _SQLITE_RDEBUG_PRINT

//Enable _SQLITE_OS_TRACE_ENABLED if you get the OS porting layer traces compiled in the binary
//#define _SQLITE_OS_TRACE_ENABLED

//Enable _SQLITE_FBUF_TRACE_ENABLED if you get the file buffer traces compiled in the binary
//#define _SQLITE_FBUF_TRACE_ENABLED

#ifdef _DEBUG
	//Enable _SQLITE_PANIC_TRACE_ENABLED if you want to get more detailed output regarding panics
	//#define _SQLITE_PANIC_TRACE_ENABLED
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Wrap every string (file name, file path, etc.) you want to trace, in a __SQLITEPRNSTR()/__SQLITEPRNSTR8() macro. 
//There is a difference how RDebug::Print() and OstTraceExt<n>() work.
#if defined _SQLITE_RDEBUG_PRINT
    const TInt KSqliteMaxPrnStrLen = 512;    
    #define __SQLITEPRNSTR(des)  &des
#else
    #define __SQLITEPRNSTR(des)  des
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
Set of useful functions to print diagnostic messages on the console when a panic occurs.

@internalComponent
*/
class TSqliteUtil
	{
public:
	static TInt Panic(const TText* aFile, TInt aLine, TInt aPanicCode, TUint aHandle);
	
private:
	static TPtrC FileName(const TText* aFile);
	
	};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define __SQLITESTRING(str) _S(str)

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//__SQLITEPANIC/__SQLITEPANIC2 macro is used for printing out additional information when panic occurs in SQLite OS porting layer and the file buffer:
//source file name, line number, "this" pointer, panic category.
#define __SQLITEPANIC(aPanicCode)      TSqliteUtil::Panic(__SQLITESTRING(__FILE__), __LINE__, aPanicCode, (TUint)this)
#define __SQLITEPANIC2(aPanicCode)     TSqliteUtil::Panic(__SQLITESTRING(__FILE__), __LINE__, aPanicCode, 0)

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef _SQLITE_OS_TRACE_ENABLED
    #define SQLITE_TRACE_OS(trace)   trace
#else
    #define SQLITE_TRACE_OS(trace)   do {} while(0)
#endif

#ifdef _SQLITE_FBUF_TRACE_ENABLED
    #define SQLITE_TRACE_FBUF(trace)   trace
#else
    #define SQLITE_TRACE_FBUF(trace)   do {} while(0)
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif //SQLITEUTIL_H
