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
	ESqliteOsPanicNullOsLayerDataPtr	= 1,
	ESqliteOsPanicInvalidWAmount 		= 2,
	ESqliteOsPanicOffset64bit 			= 3,
	ESqliteOsPanicInvalidOpType			=11,
	ESqliteOsPanicInvalidFhStr			=12,
	ESqliteOsPanicInvalidFhData			=13,
	ESqliteOsPanicInvalidArg			=14,
	ESqliteOsPanicInvalidRAmount 		=15,
	ESqliteOsPanicOsLayerDataExists		=16,
	ESqliteOsPanicInvalidDrive			=17,
	ESqliteOsPanicInvalidSectorSize		=18,
	ESqliteOsPanicInternalError			=19,
	ESqliteOsPanicNullDbFilePtr			=20,
	ESqliteOsPanicFastCounterFreq		=21,
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
	EFBufPanicRwDataLength				=116,
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
    const TDesC* SqliteDes8to16Ptr(const TDesC8& aDes);
	#define __SQLITEPRNSTR8(des)  SqliteDes8to16Ptr(des) 
#else
    #define __SQLITEPRNSTR(des)  des
    #define __SQLITEPRNSTR8(des) des
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
