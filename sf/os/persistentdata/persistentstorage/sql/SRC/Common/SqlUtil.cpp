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

#include <e32svr.h>
#include "SqlAssert.h"
#include <sqldb.h>		//ESqlAtRow, ESqlAtEnd, ESqlErrGeneral
#include "sqlite3.h"	//SQLITE_OK, SQLITE_ROW, SQLITE_DONE
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "SqlUtilTraces.h"
#endif
#include "SqlTraceDef.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const TInt KSqlLeavePanic = -359;//The (last-1) error code from the reserved area for the SQL component.

#define UNUSED_ARG(arg) arg = arg
#define UNUSED_DES(arg) arg

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if defined OST_TRACE_COMPILER_IN_USE &&  defined _SQL_RDEBUG_PRINT

/**
This class has been added here to avoid the crashes when _SQL_RDEBUG_PRINT macro is defined but the
data to be printed out is too big and cannot fit into the buffer with size KSqlMaxPrnStrLen.
@internalComponent   
*/
class TSqlDes16Overflow : public TDes16Overflow
    {
public:    
    virtual void Overflow(TDes16& /*aDes*/)
        {
        }
    };

//Replaces:
// 1) "%lld" with "%ld"
// 2) "%s" with "%S"
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

void SqlPrintf(TInt /*aGroupName*/, TInt /*aTraceName*/, const char* aFormat, ...)
    {
    VA_LIST list;
    VA_START(list, aFormat);
    TBuf<128> format;
    _LIT(KTraceIdent, "SQL;");
    format.Copy(TPtrC8((const TUint8*)aFormat));
    format.Insert(0, KTraceIdent);
    format.Append(_L("\r\n"));
    _LIT(KOstI64Fmt, "%lld");
    _LIT(KDbgPrnI64Fmt, "%ld");
    ReplaceFmtSpec(format, KOstI64Fmt, KDbgPrnI64Fmt);
    _LIT(KOstDes8Fmt, "%s");
    _LIT(KDbgPrnDesFmt, "%S");
    ReplaceFmtSpec(format, KOstDes8Fmt, KDbgPrnDesFmt);
    TBuf<KSqlMaxPrnStrLen> buf;
    TSqlDes16Overflow overflowHandler;
    buf.AppendFormatList(format, list, &overflowHandler);
#ifdef _SQL_RDEBUG_PRINT    
    RDebug::RawPrint(buf);
#endif
    }

const TDesC* SqlDes8to16Ptr(const TDesC8& aDes, TDes& aOut)
	{
	TPtrC8 ptr(aDes.Ptr(), Min(aDes.Length(), aOut.MaxLength()));
	aOut.Copy(ptr);
	return &aOut;
	}

#endif//defined OST_TRACE_COMPILER_IN_USE &&  defined _SQL_RDEBUG_PRINT 

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
SQL panic category.

@internalComponent
*/
_LIT(KPanicCategory, "SqlDb");

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
Panics the caller with aPanicCode panic code.
The call will terminate the thread where it is called from.

@param aPanicCode Panic code.

@internalComponent
*/
static void SqlPanic(TSqlPanic aPanicCode)
	{
	User::Panic(KPanicCategory, aPanicCode);
	}
	
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
Panics the client with aPanicCode panic code.
This function is used by the SQL server to panic the caller (the client).

@param aMessage Client's message
@param aPanicCode Panic code.

@leave KSqlLeavePanic

@return KErrNone

@internalComponent
*/
static TInt SqlPanicClientL(const RMessage2& aMessage, TSqlPanic aPanicCode)
	{
	aMessage.Panic(KPanicCategory, aPanicCode);
	__SQLLEAVE2(KSqlLeavePanic);
	return KErrNone;
	}	

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
The function prints out a "SQL panic" message to the console and panics the thread where it is called from.
It gives a useful information about the found error together with the source file name and line number where
it occurred.

Note: this function  will output information regarding the panic only if _SQL_PANIC_TRACE_ENABLED macro is defined  

@param aFile Source file name
@param aLine Source line number
@param aPanicCode Panic code
@param aHandle Numeric value, uniquely identfying the leaving location (the "this" pointer for example)

@return KErrNone

@internalComponent
*/  
TInt TSqlUtil::Panic(const TText* aFile, TInt aLine, TInt aPanicCode, TUint aHandle)
    {
#if defined OST_TRACE_COMPILER_IN_USE && defined _SQL_PANIC_TRACE_ENABLED
    TPtrC fname(FileName(aFile));
    OstTraceExt5(TRACE_FATAL, TSQLUTIL_PANIC, "Panic;0x%X;%S;%d;%S;%d", aHandle, __SQLPRNSTR(fname), aLine, __SQLPRNSTR(KPanicCategory), aPanicCode);
#else
    UNUSED_ARG(aFile);
    UNUSED_ARG(aLine);
    UNUSED_ARG(aHandle);
#endif      
    ::SqlPanic(static_cast <TSqlPanic> (aPanicCode));
    return KErrNone;
    }

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
The function prints out a "SQL leave" message to the console and leaves with aError error code.
It gives a usefull information about the found error together with the source file name and line number where
it occured.

Note: this function  will output information regarding the panic only if _SQL_LEAVE_TRACE_ENABLED macro is defined  

@param aFile Source file name
@param aLine Source line number
@param aError Error code
@param aHandle Numeric value, uniquely identfying the leaving location (the "this" pointer for example)

@internalComponent
*/  
void TSqlUtil::Leave(const TText* aFile, TInt aLine, TInt aError, TUint aHandle)
    {
#if defined OST_TRACE_COMPILER_IN_USE && defined _SQL_LEAVE_TRACE_ENABLED     
    TPtrC fname(FileName(aFile));
    OstTraceExt4(TRACE_ERROR, TSQLUTIL_LEAVE, "Leave;0x%X;%S;%d;Error=%d", aHandle, __SQLPRNSTR(fname), aLine, aError);
#else
    UNUSED_ARG(aFile);
    UNUSED_ARG(aLine);
    UNUSED_ARG(aHandle);
#endif
    User::Leave(aError);
    }

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
The function prints out a "SQL leave" message to the console and leaves with aError error code, if it is 
negative.
It gives a usefull information about the found error together with the source file name and line number where
it occured.

Note: this function  will output information regarding the panic only if _SQL_LEAVE_TRACE_ENABLED macro is defined  

@param aFile Source file name
@param aLine Source line number
@param aError Error code

@internalComponent
*/  
TInt TSqlUtil::LeaveIfError(const TText* aFile, TInt aLine, TInt aError, TUint aHandle)
    {
    if(aError < 0)
        {
        TSqlUtil::Leave(aFile, aLine, aError, aHandle);
        }
    return aError;
    }

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
The function prints out a "SQL leave" message to the console and leaves with KErrNoMemory if 
aPtr parameter is NULL.

Note: this function  will output information regarding the panic only if _SQL_LEAVE_TRACE_ENABLED macro is defined  

@param aFile Source file name
@param aLine Source line number
@param aPtr The pointer to be tested against NULL value.

@internalComponent
*/  
void* TSqlUtil::LeaveIfNull(const TText* aFile, TInt aLine, void* aPtr, TUint aHandle)
    {
    if(!aPtr)
        {
        TSqlUtil::Leave(aFile, aLine, KErrNoMemory, aHandle);
        }
    return aPtr;
    }

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
The function is used by the SQL server.
It prints out a "SQL panic" message to the console and panic the client.
It gives a usefull information about the found error together with the source file name and line number where
it occured.

Note: this function  will output information regarding the panic only if _SQL_PANIC_TRACE_ENABLED macro is defined  

@param aFile Source file name
@param aLine Source line number
@param aMessage The client message, which processing caused the panic.
@param aPanicCode Error code

@leave KSqlLeavePanic

@return KErrNone;

@internalComponent
*/  
TInt TSqlUtil::PanicClientL(const TText* aFile, TInt aLine, const RMessage2& aMessage, TInt aPanicCode, TUint aHandle)
    {
#if defined OST_TRACE_COMPILER_IN_USE && defined  _SQL_PANIC_TRACE_ENABLED
    TPtrC fname(FileName(aFile));
    OstTraceExt5(TRACE_FATAL, TSQLUTIL_PANICCLIENTL, "Panic;%X;%S;%d;%S;%d", aHandle, __SQLPRNSTR(fname), aLine, __SQLPRNSTR(KPanicCategory), aPanicCode);
#else
    UNUSED_ARG(aFile);
    UNUSED_ARG(aLine);
    UNUSED_ARG(aHandle);
#endif      
    return ::SqlPanicClientL(aMessage, static_cast <TSqlPanic> (aPanicCode));
    }

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
Processes SQL database error code and OS error code and returns unified error code.
If aSqlError == SQLITE_ROW then the function returns KSqlAtRow.
If aSqlError == SQLITE_DONE then the function returns KSqlAtEnd.
If aSqlError == SQLITE_NOMEM then the function returns KErrNoMemory.
If aOsError != KErrNone then the function returns aOsError.
Otherwise the function converts aSqlError to one of error codes in [KSqlErrGeneral..KSqlErrStmtExpired] range.

@param aSqlError SQL database error code.
@param aOsError OS error code.

@return Database specific error code.

@panic SqlDb 4 in debug mode - if aSqlError < 0
@panic SqlDb 4 in debug mode - if aOsError > 0

@internalComponent
*/
TInt Sql2OsErrCode(TInt aSqlError, TInt aOsError)
	{

	__ASSERT_DEBUG(aSqlError >= SQLITE_OK && aOsError <= KErrNone, __SQLPANIC2(ESqlPanicBadArgument));
	TInt err = KErrNone;
	if(aOsError == KErrDiskFull)
		{//Whatever is the aSqlError value, even SQLITE_OK, never ignore KErrDiskFull errors
		 //(For example: ROLLBACK statement execution, when the disk is full).
		err = aOsError;
		}
	else if(aSqlError == SQLITE_ROW)
		{
		err = KSqlAtRow;
		}
	else if(aSqlError == SQLITE_DONE)
		{
		err = KSqlAtEnd;
		}
	else if(aSqlError == SQLITE_NOMEM)
		{
		err = KErrNoMemory;
		}
	else if(aSqlError == SQLITE_AUTH)
		{
		err = KErrPermissionDenied;
		}
	else if(aSqlError == SQLITE_NOTADB)
		{
		err = KSqlErrNotDb;	
		}
	else if(aSqlError > SQLITE_OK)
		{
		err = aOsError != KErrNone ? aOsError : KSqlErrGeneral - aSqlError + 1;
		}
	return err;
	}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if defined OST_TRACE_COMPILER_IN_USE && (defined _SQL_PANIC_TRACE_ENABLED || defined _SQL_LEAVE_TRACE_ENABLED) 

/**
The function creates and returns TPtrC object which points to aFile parameter.

@param aFile File name
@return TPtrC object pointing to aFile parameter.

@internalComponent
*/	
TPtrC TSqlUtil::FileName(const TText* aFile)
	{
	TPtrC p(aFile);
	TInt ix = p.LocateReverse('\\');
	if(ix<0)
		ix=p.LocateReverse('/');
	if(ix>=0)
		p.Set(p.Mid(1+ix));
	return p;
	}

#endif //defined OST_TRACE_COMPILER_IN_USE && (defined _SQL_PANIC_TRACE_ENABLED || defined _SQL_LEAVE_TRACE_ENABLED)
