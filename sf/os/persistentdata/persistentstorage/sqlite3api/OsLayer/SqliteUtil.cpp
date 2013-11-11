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
#include <e32debug.h>
#include "SqliteUtil.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "SqliteUtilTraces.h"
#endif
#include "SqliteTraceDef.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define UNUSED_ARG(arg) arg = arg
#define UNUSED_DES(arg) arg

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if defined OST_TRACE_COMPILER_IN_USE &&  defined _SQLITE_RDEBUG_PRINT

/**
This class has been added here to avoid the crashes when _SQLITE_RDEBUG_PRINT macro is defined but the
data to be printed out is too big and cannot fit into the buffer with size KSqliteMaxPrnStrLen.
@internalComponent   
*/
class TSqliteDes16Overflow : public TDes16Overflow
    {
public:    
    virtual void Overflow(TDes16& /*aDes*/)
        {
        }
    };

//Replaces:
// "%lld" with "%ld"
//These are the differences in format specification between RDebig::Print and OST functions.
//The new format spec length should be less or equal than the old format spec length.
static void ReplaceFmtSpec(TDes& aFormat, const TDesC& aFmtSpec, const TDesC& aNewFmtSpec)
	{
	TInt fmtLength = aFormat.Length();
	const TInt KDiff = aFmtSpec.Length() - aNewFmtSpec.Length();
    TPtr ptr((TText*)aFormat.Ptr(), fmtLength, fmtLength);
    TInt pos;
    while((pos = ptr.Find(aFmtSpec)) >= 0)
    	{
		ptr.Replace(pos, aFmtSpec.Length(), aNewFmtSpec);
		fmtLength -= KDiff;
		ptr.Set(ptr.MidTPtr(pos));
    	}
    aFormat.SetLength(fmtLength);
	}

void SqlitePrintf(TInt /*aGroupName*/, TInt /*aTraceName*/, const char* aFormat, ...)
    {
    VA_LIST list;
    VA_START(list, aFormat);
    TBuf<128> format;
    _LIT(KTraceIdent, "Sqlite3;");
    format.Copy(TPtrC8((const TUint8*)aFormat));
    format.Insert(0, KTraceIdent);
    format.Append(_L("\r\n"));
    _LIT(KOstI64Fmt, "%lld");
    _LIT(KDbgPrnI64Fmt, "%ld");
    ReplaceFmtSpec(format, KOstI64Fmt, KDbgPrnI64Fmt);
    TBuf<KSqliteMaxPrnStrLen> buf;
    TSqliteDes16Overflow overflowHandler;
    buf.AppendFormatList(format, list, &overflowHandler);
#ifdef _SQLITE_RDEBUG_PRINT    
    RDebug::RawPrint(buf);
#endif
    }

#endif//defined OST_TRACE_COMPILER_IN_USE &&  defined _SQLITE_RDEBUG_PRINT 

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
SQLite panic category.

@internalComponent
*/
_LIT(KSqlitePanicCategory, "Sqlite3");

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
Panics the caller with aPanicCode panic code.
The call will terminate the thread where it is called from.

@param aPanicCode Panic code.

@internalComponent
*/
static void SqlitePanic(TSqlitePanic aPanicCode)
	{
	User::Panic(KSqlitePanicCategory, aPanicCode);
	}
	
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
The function prints out a "SQLite" panic message to the console and panics the thread where it is called from.
It gives a useful information about the found error together with the source file name and line number where
it occurred.

Note: this function  will output information regarding the panic only if _SQLITE_PANIC_TRACE_ENABLED macro is defined  

@param aFile Source file name
@param aLine Source line number
@param aPanicCode Panic code
@param aHandle Numeric value, uniquely identfying the leaving location (the "this" pointer for example)

@return KErrNone

@internalComponent
*/  
TInt TSqliteUtil::Panic(const TText* aFile, TInt aLine, TInt aPanicCode, TUint aHandle)
    {
#if defined OST_TRACE_COMPILER_IN_USE && defined _SQLITE_PANIC_TRACE_ENABLED
    TPtrC fname(FileName(aFile));
    OstTraceExt5(TRACE_FATAL, TSQLUTIL_PANIC, "Panic;0x%X;%S;%d;%S;%d", aHandle, __SQLITEPRNSTR(fname), aLine, __SQLITEPRNSTR(KSqlitePanicCategory), aPanicCode);
#else
    UNUSED_ARG(aFile);
    UNUSED_ARG(aLine);
    UNUSED_ARG(aHandle);
#endif      
    ::SqlitePanic(static_cast <TSqlitePanic> (aPanicCode));
    return KErrNone;
    }

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if defined OST_TRACE_COMPILER_IN_USE && defined _SQLITE_PANIC_TRACE_ENABLED

/**
The function creates and returns TPtrC object which points to aFile parameter.

@param aFile File name
@return TPtrC object pointing to aFile parameter.

@internalComponent
*/	
TPtrC TSqliteUtil::FileName(const TText* aFile)
	{
	TPtrC p(aFile);
	TInt ix = p.LocateReverse('\\');
	if(ix<0)
		ix=p.LocateReverse('/');
	if(ix>=0)
		p.Set(p.Mid(1+ix));
	return p;
	}

#endif //defined OST_TRACE_COMPILER_IN_USE && defined _SQLITE_PANIC_TRACE_ENABLED

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
