// Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "SqlAssert.h"			//ESqlPanicInvalidObj, ESqlPanicObjExists
#include "SqlDatabaseImpl.h"	//CSqlDatabaseImpl
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "SqlScalarFullSelectTraces.h"
#endif
#include "SqlTraceDef.h"

/**
Returns a reference to the implementation object of RSqlDatabase - CSqlDatabaseImpl.

@panic SqlDb 2 Create() or Open() has not previously been called on this RSqlDatabase object.

@internalComponent
*/
inline CSqlDatabaseImpl& TSqlScalarFullSelectQuery::Impl() const
	{
	__ASSERT_ALWAYS(iDatabaseImpl != NULL, __SQLPANIC(ESqlPanicInvalidObj));
	return *iDatabaseImpl;	
	}

/**
Initializes TSqlScalarFullSelectQuery data members with default values.
*/
EXPORT_C TSqlScalarFullSelectQuery::TSqlScalarFullSelectQuery() :
	iDatabaseImpl(NULL)
	{
	}
	
/**
Initializes TSqlScalarFullSelectQuery object.

@param aDatabase	A reference to the RSqlDatabase object that represents 
                    the database on which scalar fullselect queries will be executed.
*/
EXPORT_C TSqlScalarFullSelectQuery::TSqlScalarFullSelectQuery(RSqlDatabase& aDatabase) :
	iDatabaseImpl(&aDatabase.Impl())
	{
	SQL_TRACE_BORDER(OstTraceExt3(TRACE_BORDER, TSQLSCALARFULLSELECTQUERY_TSQLSCALARFULLSELECTQUERY, "0x%X;TSqlScalarFullSelectQuery::TSqlScalarFullSelectQuery;aDatabase=0x%X;iDatabaseImpl=0x%X", (TUint)this, (TUint)&aDatabase, (TUint)iDatabaseImpl));
	}
	
/**
Initializes/reinitializes TSqlScalarFullSelectQuery object.

@param aDatabase	A reference to the RSqlDatabase object that represents 
                    the database on which scalar fullselect queries will be executed.
*/
EXPORT_C void TSqlScalarFullSelectQuery::SetDatabase(RSqlDatabase& aDatabase)
	{
	iDatabaseImpl = &aDatabase.Impl();
    SQL_TRACE_BORDER(OstTraceExt3(TRACE_BORDER, TSQLSCALARFULLSELECTQUERY_SETDATABASE, "0x%X;TSqlScalarFullSelectQuery::SetDatabase;aDatabase=0x%X;iDatabaseImpl=0x%X", (TUint)this, (TUint)&aDatabase, (TUint)iDatabaseImpl));
	}
	
/**
Executes a SELECT query which is expected to return a single row consisting of
a single 32-bit integer value and returns that value.

@param aSqlStmt 16-bit SELECT sql query

@return 32-bit integer column value.

@leave  KErrNotFound, If there is no record,
        The function may leave with database specific errors categorised as ESqlDbError and
  		other system-wide error codes.
*/
EXPORT_C TInt TSqlScalarFullSelectQuery::SelectIntL(const TDesC& aSqlStmt)
	{
    SQL_TRACE_BORDER(OstTraceExt2(TRACE_BORDER, TSQLSCALARFULLSELECTQUERY_SELECTINTL_ENTRY16, "Entry;0x%X;TSqlScalarFullSelectQuery::SelectIntL-16;aSqlStmt=%S", (TUint)this, __SQLPRNSTR(aSqlStmt)));
	TInt res;
	TPtr8 ptr(reinterpret_cast <TUint8*> (&res), sizeof(res));
	(void)Impl().ExecScalarFullSelectL(aSqlStmt, ESqlInt, ptr);
    SQL_TRACE_BORDER(OstTraceExt2(TRACE_BORDER, TSQLSCALARFULLSELECTQUERY_SELECTINTL_EXIT16, "Exit;0x%X;TSqlScalarFullSelectQuery::SelectIntL-16;res=%d", (TUint)this, res));
	return res;
	}
	
/**
Executes a SELECT query which is expected to return a single row consisting of
a single 64-bit integer value and returns that value.

@param aSqlStmt 16-bit SELECT sql query

@return 64-bit integer column value.

@leave  KErrNotFound, If there is no record,
        The function may leave with database specific errors categorised as ESqlDbError and
  		other system-wide error codes.
*/
EXPORT_C TInt64 TSqlScalarFullSelectQuery::SelectInt64L(const TDesC& aSqlStmt)
	{
    SQL_TRACE_BORDER(OstTraceExt2(TRACE_BORDER, TSQLSCALARFULLSELECTQUERY_SELECTINT64L_ENTRY16, "Entry;0x%X;TSqlScalarFullSelectQuery::SelectInt64L-16;aSqlStmt=%S", (TUint)this, __SQLPRNSTR(aSqlStmt)));
	TInt64 res;
	TPtr8 ptr(reinterpret_cast <TUint8*> (&res), sizeof(res));
	(void)Impl().ExecScalarFullSelectL(aSqlStmt, ESqlInt64, ptr);
	SQL_TRACE_BORDER(OstTraceExt2(TRACE_BORDER, TSQLSCALARFULLSELECTQUERY_SELECTINT64L_EXIT16, "Exit;0x%X;TSqlScalarFullSelectQuery::SelectInt64L-16;res=%lld", (TUint)this, res));
	return res;
	}
	
/**
Executes a SELECT query which is expected to return a single row consisting of
a single real value and returns that value.

@param aSqlStmt 16-bit SELECT sql query

@return Real column value.

@leave  KErrNotFound, If there is no record,
        The function may leave with database specific errors categorised as ESqlDbError and
  		other system-wide error codes.
*/
EXPORT_C TReal TSqlScalarFullSelectQuery::SelectRealL(const TDesC& aSqlStmt)
	{
    SQL_TRACE_BORDER(OstTraceExt2(TRACE_BORDER, TSQLSCALARFULLSELECTQUERY_SELECTREALL_ENTRY16, "Entry;0x%X;TSqlScalarFullSelectQuery::SelectRealL-16;aSqlStmt=%S", (TUint)this, __SQLPRNSTR(aSqlStmt)));
	TReal res;
	TPtr8 ptr(reinterpret_cast <TUint8*> (&res), sizeof(res));
	(void)Impl().ExecScalarFullSelectL(aSqlStmt, ESqlReal, ptr);
    SQL_TRACE_BORDER(OstTrace1(TRACE_BORDER, TSQLSCALARFULLSELECTQUERY_SELECTREALL_EXIT16, "Exit;0x%X;TSqlScalarFullSelectQuery::SelectRealL-16", (TUint)this));
	return res;
	}
	
/**
Executes a SELECT query which is expected to return a single row consisting of
a single text value and copies that value to the place refered by aDest parameter.

If the destination buffer is not big enough, the function will copy as much data as 
possible and will return positive value - the character length of the text column.

@param aSqlStmt 16-bit SELECT sql query
@param aDest Refers to the place where the column data will be copied

@return KErrNone, if the function completes successfully,
		Positive value, The text column value length in characters, in case if the receiving buffer 
						is not big enough.

@leave  KErrNotFound, If there is no record,
        The function may leave with database specific errors categorised as ESqlDbError and
  		other system-wide error codes.
*/
EXPORT_C TInt TSqlScalarFullSelectQuery::SelectTextL(const TDesC& aSqlStmt, TDes& aDest)
	{
    SQL_TRACE_BORDER(OstTraceExt5(TRACE_BORDER, TSQLSCALARFULLSELECTQUERY_SELECTTEXTL_ENTRY16, "Entry;0x%X;TSqlScalarFullSelectQuery::SelectTextL-16;aSqlStmt=%S;aDest.Ptr()=0x%X;aDest.Size()=%d;aDest.MaxSize()=%d", (TUint)this, __SQLPRNSTR(aSqlStmt), (TUint)aDest.Ptr(), aDest.Size(), aDest.MaxSize()));
	TPtr8 ptr(reinterpret_cast <TUint8*> (const_cast <TUint16*> (aDest.Ptr())), aDest.MaxLength() * sizeof(TUint16));
	TInt rc = Impl().ExecScalarFullSelectL(aSqlStmt, ESqlText, ptr);
	aDest.SetLength(ptr.Length() / sizeof(TUint16));
    SQL_TRACE_BORDER(OstTraceExt2(TRACE_BORDER, TSQLSCALARFULLSELECTQUERY_SELECTTEXTL_EXIT16, "Exit;0x%X;TSqlScalarFullSelectQuery::SelectTextL-16;rc=%d", (TUint)this, rc));
	return rc;
	}
	
/**
Executes a SELECT query which is expected to return a single row consisting of
a single binary value and copies that value to the place refered by aDest parameter.

If the destination buffer is not big enough, the function will copy as much data as 
possible and will return positive value - the byte length of the binary column.

@param aSqlStmt 16-bit SELECT sql query
@param aDest Refers to the place where the column data will be copied

@return KErrNone, if the function completes successfully,
		Positive value, The binary column value length in bytes, in case if the receiving buffer 
						is not big enough.

@leave  KErrNotFound, If there is no record,
        The function may leave with database specific errors categorised as ESqlDbError and
  		other system-wide error codes.
*/
EXPORT_C TInt TSqlScalarFullSelectQuery::SelectBinaryL(const TDesC& aSqlStmt, TDes8& aDest)
	{
    SQL_TRACE_BORDER(OstTraceExt5(TRACE_BORDER, TSQLSCALARFULLSELECTQUERY_SELECTBINARYL_ENTRY16, "Entry;0x%X;TSqlScalarFullSelectQuery::SelectBinaryL-16;aSqlStmt=%S;aDest.Ptr()=0x%X;aDest.Size()=%d;aDest.MaxSize()=%d", (TUint)this, __SQLPRNSTR(aSqlStmt), (TUint)aDest.Ptr(), aDest.Size(), aDest.MaxSize()));
	TInt rc = Impl().ExecScalarFullSelectL(aSqlStmt, ESqlBinary, aDest);
    SQL_TRACE_BORDER(OstTraceExt2(TRACE_BORDER, TSQLSCALARFULLSELECTQUERY_SELECTBINARYL_EXIT16, "Exit;0x%X;TSqlScalarFullSelectQuery::SelectBinaryL-16;rc=%d", (TUint)this, rc));
	return rc;
	}
	
/**
Executes a SELECT query which is expected to return a single row consisting of
a single 32-bit integer value and returns that value.

@param aSqlStmt 8-bit SELECT sql query

@return 32-bit integer column value.

@leave  KErrNotFound, If there is no record,
        The function may leave with database specific errors categorised as ESqlDbError and
  		other system-wide error codes.
*/
EXPORT_C TInt TSqlScalarFullSelectQuery::SelectIntL(const TDesC8& aSqlStmt)
	{
	__SQLTRACE_BORDERVAR(TBuf<100> des16prnbuf);
    SQL_TRACE_BORDER(OstTraceExt2(TRACE_BORDER, TSQLSCALARFULLSELECTQUERY_SELECTINTL_ENTRY8, "Entry;0x%X;TSqlScalarFullSelectQuery::SelectIntL-8;aSqlStmt=%s", (TUint)this, __SQLPRNSTR8(aSqlStmt, des16prnbuf)));
	TInt res;
	TPtr8 ptr(reinterpret_cast <TUint8*> (&res), sizeof(res));
	(void)Impl().ExecScalarFullSelectL(aSqlStmt, ESqlInt, ptr);
    SQL_TRACE_BORDER(OstTraceExt2(TRACE_BORDER, TSQLSCALARFULLSELECTQUERY_SELECTINTL_EXIT8, "Exit;0x%X;TSqlScalarFullSelectQuery::SelectIntL-8;res=%d", (TUint)this, res));
	return res;
	}
	
/**
Executes a SELECT query which is expected to return a single row consisting of
a single 64-bit integer value and returns that value.

@param aSqlStmt 8-bit SELECT sql query

@return 64-bit integer column value.

@leave  KErrNotFound, If there is no record,
        The function may leave with database specific errors categorised as ESqlDbError and
  		other system-wide error codes.
*/
EXPORT_C TInt64 TSqlScalarFullSelectQuery::SelectInt64L(const TDesC8& aSqlStmt)
	{
	__SQLTRACE_BORDERVAR(TBuf<100> des16prnbuf);
	SQL_TRACE_BORDER(OstTraceExt2(TRACE_BORDER, TSQLSCALARFULLSELECTQUERY_SELECTINT64L_ENTRY8, "Entry;0x%X;TSqlScalarFullSelectQuery::SelectInt64L-8;aSqlStmt=%s", (TUint)this, __SQLPRNSTR8(aSqlStmt, des16prnbuf)));
	TInt64 res;
	TPtr8 ptr(reinterpret_cast <TUint8*> (&res), sizeof(res));
	(void)Impl().ExecScalarFullSelectL(aSqlStmt, ESqlInt64, ptr);
	SQL_TRACE_BORDER(OstTraceExt2(TRACE_BORDER, TSQLSCALARFULLSELECTQUERY_SELECTINT64L_EXIT8, "Exit;0x%X;TSqlScalarFullSelectQuery::SelectInt64L-8;res=%lld", (TUint)this, res));
	return res;
	}
	
/**
Executes a SELECT query which is expected to return a single row consisting of
a single real value and returns that value.

@param aSqlStmt 8-bit SELECT sql query

@return Real column value.

@leave  KErrNotFound, If there is no record,
        The function may leave with database specific errors categorised as ESqlDbError and
  		other system-wide error codes.
*/
EXPORT_C TReal TSqlScalarFullSelectQuery::SelectRealL(const TDesC8& aSqlStmt)
	{
	__SQLTRACE_BORDERVAR(TBuf<100> des16prnbuf);
    SQL_TRACE_BORDER(OstTraceExt2(TRACE_BORDER, TSQLSCALARFULLSELECTQUERY_SELECTREALL_ENTRY8, "Entry;0x%X;TSqlScalarFullSelectQuery::SelectRealL-8;aSqlStmt=%s", (TUint)this, __SQLPRNSTR8(aSqlStmt, des16prnbuf)));
	TReal res;
	TPtr8 ptr(reinterpret_cast <TUint8*> (&res), sizeof(res));
	(void)Impl().ExecScalarFullSelectL(aSqlStmt, ESqlReal, ptr);
    SQL_TRACE_BORDER(OstTrace1(TRACE_BORDER, TSQLSCALARFULLSELECTQUERY_SELECTREALL_EXIT8, "Exit;0x%X;TSqlScalarFullSelectQuery::SelectRealL-8", (TUint)this));
	return res;
	}
	
/**
Executes a SELECT query which is expected to return a single row consisting of
a single text value and copies that value to the place refered by aDest parameter.

If the destination buffer is not big enough, the function will copy as much data as 
possible and will return positive value - the character length of the text column.

@param aSqlStmt 8-bit SELECT sql query
@param aDest Refers to the place where the column data will be copied

@return KErrNone, if the function completes successfully,
		Positive value, The text column value length in characters, in case if the receiving buffer 
						is not big enough.

@leave  KErrNotFound, If there is no record,
        The function may leave with database specific errors categorised as ESqlDbError and
  		other system-wide error codes.
*/
EXPORT_C TInt TSqlScalarFullSelectQuery::SelectTextL(const TDesC8& aSqlStmt, TDes& aDest)
	{
	__SQLTRACE_BORDERVAR(TBuf<100> des16prnbuf);
    SQL_TRACE_BORDER(OstTraceExt5(TRACE_BORDER, TSQLSCALARFULLSELECTQUERY_SELECTTEXTL_ENTRY8, "Entry;0x%X;TSqlScalarFullSelectQuery::SelectTextL-8;aSqlStmt=%s;aDest.Ptr()=0x%X;aDest.Size()=%d;aDest.MaxSize()=%d", (TUint)this, __SQLPRNSTR8(aSqlStmt, des16prnbuf), (TUint)aDest.Ptr(), aDest.Size(), aDest.MaxSize()));
	TPtr8 ptr(reinterpret_cast <TUint8*> (const_cast <TUint16*> (aDest.Ptr())), aDest.MaxLength() * sizeof(TUint16));
	TInt rc = Impl().ExecScalarFullSelectL(aSqlStmt, ESqlText, ptr);
	aDest.SetLength(ptr.Length() / sizeof(TUint16));
    SQL_TRACE_BORDER(OstTraceExt2(TRACE_BORDER, TSQLSCALARFULLSELECTQUERY_SELECTTEXTL_EXIT8, "Exit;0x%X;TSqlScalarFullSelectQuery::SelectTextL-8;rc=%d", (TUint)this, rc));
	return rc;
	}
	
/**
Executes a SELECT query which is expected to return a single row consisting of
a single binary value and copies that value to the place refered by aDest parameter.

If the destination buffer is not big enough, the function will copy as much data as 
possible and will return positive value - the character length of the text column.

@param aSqlStmt 8-bit SELECT sql query
@param aDest Refers to the place where the column data will be copied

@return KErrNone, if the function completes successfully,
		Positive value, The binary column value length in bytes, in case if the receiving buffer 
						is not big enough.

@leave  KErrNotFound, If there is no record,
        The function may leave with database specific errors categorised as ESqlDbError and
  		other system-wide error codes.
*/
EXPORT_C TInt TSqlScalarFullSelectQuery::SelectBinaryL(const TDesC8& aSqlStmt, TDes8& aDest)
	{
	__SQLTRACE_BORDERVAR(TBuf<100> des16prnbuf);
    SQL_TRACE_BORDER(OstTraceExt5(TRACE_BORDER, TSQLSCALARFULLSELECTQUERY_SELECTBINARYL_ENTRY8, "Entry;0x%X;TSqlScalarFullSelectQuery::SelectBinaryL-8;aSqlStmt=%s;aDest.Ptr()=0x%X;aDest.Size()=%d;aDest.MaxSize()=%d", (TUint)this, __SQLPRNSTR8(aSqlStmt, des16prnbuf), (TUint)aDest.Ptr(), aDest.Size(), aDest.MaxSize()));
	TInt rc = Impl().ExecScalarFullSelectL(aSqlStmt, ESqlBinary, aDest);
    SQL_TRACE_BORDER(OstTraceExt2(TRACE_BORDER, TSQLSCALARFULLSELECTQUERY_SELECTBINARYL_EXIT8, "Exit;0x%X;TSqlScalarFullSelectQuery::SelectBinaryL-8;rc=%d", (TUint)this, rc));
	return rc;
	}
