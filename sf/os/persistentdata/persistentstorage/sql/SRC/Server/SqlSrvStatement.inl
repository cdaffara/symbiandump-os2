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
// ///////////////                HSqlSrvStmtParamBuf                        ////////////////////////////
// 
//

/**
 
 Creates a new HSqlSrvStmtParamBuf instance.
 
 @param aStatement A reference to the CSqlSrvStatement object, which needs the parameter data.
 @param aParamIndex Parameter index, zero based.
 @param aDataType Parameter value type - binary, text8 or text16.
 @param aBufType IPC stream buffer or a simple "bind param" buffer
 
 @return A pointer to the created HSqlSrvStmtParamBuf instance.
 
 @leave KErrNoMemory, an out of memory condition has occurred;
 
 @panic SqlDb 4 In _DEBUG mode. Parameter index negative.
*/
inline HSqlSrvStmtParamBuf* HSqlSrvStmtParamBuf::NewL(CSqlSrvStatement& aStatement, TInt aParamIndex, 
													  HSqlSrvStmtParamBuf::TDataType aDataType, HSqlSrvStmtParamBuf::TBufType aBufType)
	{
	__ASSERT_DEBUG(aParamIndex >= 0, __SQLPANIC2(ESqlPanicBadArgument));
	HSqlSrvStmtParamBuf* self = new (ELeave) HSqlSrvStmtParamBuf(aStatement, aParamIndex, aDataType, aBufType);
	self->PushL();
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

/**
Resets the current HSqlSrvStmtParamBuf instance.

The internal buffer will be resized, if its capacity is bigger than HSqlSrvStmtParamBuf::EExpandSize * 2 bytes.

@param aDataType Parameter value type - binary, text8 or text16.
@param aBufType IPC stream buffer or a simple "bind param" buffer

@panic SqlDb 2 In _DEBUG mode. Invalid HSqlSrvStmtParamBuf object.
*/
inline void HSqlSrvStmtParamBuf::Reset(HSqlSrvStmtParamBuf::TDataType aDataType, HSqlSrvStmtParamBuf::TBufType aBufType)
	{
	__ASSERT_DEBUG(iBuf != NULL, __SQLPANIC(ESqlPanicInvalidObj));
	
	iStatementFinalized = EFalse;
	iAlive = ETrue;
	iDataType = aDataType;
	iBufType = aBufType;
	iSynchDone = EFalse;
	
	iBuf->Delete(0, iBuf->Size());
	if(iBuf->Capacity() > (HSqlSrvStmtParamBuf::EExpandSize * 2))
		{
		TRAPD(err, iBuf->SetReserveL(HSqlSrvStmtParamBuf::EExpandSize * 2));//the buffer size is minimized, the call can't fail
		__ASSERT_ALWAYS(err == KErrNone, __SQLPANIC(ESqlPanicInternalError));
		}
	Set(*iBuf, 0, MStreamBuf::ERead | MStreamBuf::EWrite);
	}

/**
Stores the aData into the buffer, from position 0.
This function is used when the buffer type is not IPC.

@param aData The data to be stored into the buffer

@leave KErrNoMemory, an out of memory condition has occurred;

@panic SqlDb 2 In _DEBUG mode. Invalid HSqlSrvStmtParamBuf object.
@panic SqlDb 7 In _DEBUG mode. The buffer type is not HSqlSrvStmtParamBuf::EBufSimpleBind
*/
inline const TPtrC8 HSqlSrvStmtParamBuf::SetDataL(const TDesC8& aData)
	{
	__ASSERT_DEBUG(iBuf != NULL, __SQLPANIC(ESqlPanicInvalidObj));
	__ASSERT_DEBUG(iBufType == HSqlSrvStmtParamBuf::EBufSimpleBind, __SQLPANIC(ESqlPanicInternalError));
	iBuf->ResizeL(aData.Length());
	iBuf->Write(0, aData);
	//If the size is 0, then return KNullDesC8, where an empty string is hold, not a null one ("" instead of NULL)
	// was return iBuf->Size() == 0 ? KNullDesC8() : iBuf->Ptr(0);
	if (iBuf->Size() == 0)
		{
		return KNullDesC8();
		}
	return iBuf->Ptr(0);
	}

/**
Returns a 8-bit pointer to the parameter data.

@return 8-bit pointer to the parameter data.

@panic SqlDb 2 In _DEBUG mode. Invalid HSqlSrvStmtParamBuf object.
*/
inline const TPtrC8 HSqlSrvStmtParamBuf::Data() const
	{
	__ASSERT_DEBUG(iBuf != NULL, __SQLPANIC(ESqlPanicInvalidObj));
	//If the size is 0, then return KNullDesC8, where an empty string is hold, not a null one ("" instead of NULL)
	// was return iBuf->Size() == 0 ? KNullDesC8() : iBuf->Ptr(0);
	if (iBuf->Size() == 0)
		{
		return KNullDesC8();
		}
	return iBuf->Ptr(0);
	}

/**
@return Buffered parameter value type - binary, text8 or text16.
*/
inline HSqlSrvStmtParamBuf::TDataType HSqlSrvStmtParamBuf::DataType() const
	{
	return iDataType;		
	}

/**
Returns the parameter index.

@panic SqlDb 7 In _DEBUG mode. Negative parameter index.

@return Parameter index, zero based.
*/
inline TInt HSqlSrvStmtParamBuf::ParamIndex() const
	{
	__ASSERT_DEBUG(iParamIndex >= 0, __SQLPANIC(ESqlPanicInternalError));
	return iParamIndex;
	}

/**
Initializes the HSqlSrvStmtParamBuf instance data members.

@param aStatement A reference to the CSqlSrvStatement object, which needs the parameter data.
@param aParameterIndex Parameter index, zero based.
@param aDataType Parameter value type - binary, text8 or text16.
@param aBufType IPC stream buffer or a simple "bind param" buffer
*/
inline HSqlSrvStmtParamBuf::HSqlSrvStmtParamBuf(CSqlSrvStatement& aStatement, TInt aParamIndex, 
												HSqlSrvStmtParamBuf::TDataType aDataType, HSqlSrvStmtParamBuf::TBufType aBufType) :
	iStatement(aStatement),
	iBuf(NULL),
	iParamIndex(aParamIndex),
	iStatementFinalized(EFalse),
	iAlive(EFalse),
	iDataType(aDataType),
	iBufType(aBufType),
	iSynchDone(EFalse)
	{
	}

/**
HSqlSrvStmtParamBuf - second phase construction.
Constructs the internal CFlatBuf object.

@leave KErrNoMemory, an out of memory condition has occurred;
*/
inline void HSqlSrvStmtParamBuf::ConstructL()
	{
	iBuf = CBufFlat::NewL(EExpandSize);
	Set(*iBuf, 0, MStreamBuf::ERead | MStreamBuf::EWrite);
	iAlive = ETrue;
	}

//////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////   CSqlSrvStatement class    ////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////

/**
Executes the SQL statement moving the cursor to the next row if there is a row available.

@return KSqlErrStmtExpired, statement expired (if new functions or collating sequences are 
							registered or if an authorizer function is added or changed);
		KErrNone, the operation completed successfully;
		KSqlAtRow, the next record data is ready for processing by the caller;
		KSqlAtEnd, no more records;
		KSqlErrBusy, database file is locked;
		KSqlErrGeneral, run-time error. Next() should not be called anymore;
		KSqlErrMisuse, Next() called after KSqlAtEnd or KSqlErrGeneral returned by the previous Next() call;
		KErrNoMemory, an out of memory condition has occurred. The statement will be reset.

@panic SqlDb 2 In _DEBUG mode. Invalid (not created) CSqlSrvStatement object.
@panic SqlDb 7 In _DEBUG mode. SQLITE internal error. (SQLITE_ERROR, followed by a sqlite3_reset(), which returns SQLITE_OK)
*/	
inline TInt CSqlSrvStatement::Next()
	{
	__ASSERT_DEBUG(iStmtHandle != NULL, __SQLPANIC(ESqlPanicInvalidObj));
	TInt err = ::StmtNext(iStmtHandle);
	iBufFlatType = static_cast <TSqlBufFlatType> (-1);
	iBufFlat.ResetAndMinimize();
	return err;
	}

/**
Resets the prepared SQL statement to its initial state and makes it ready to be executed again.

Any SQL statement parameters that had values bound to them, retain their values.

@return KErrNone,  the operation completed successfully;
		KSqlErrStmtExpired, Statement expired (if new functions or collating sequences are 
							registered or if an authorizer function is added or changed)

@panic SqlDb 2 In _DEBUG mode. Invalid (not created) CSqlSrvStatement object.
*/	
inline TInt CSqlSrvStatement::Reset()
	{
	__ASSERT_DEBUG(iStmtHandle != NULL, __SQLPANIC(ESqlPanicInvalidObj));
	iBufFlatType = static_cast <TSqlBufFlatType> (-1);
	iBufFlat.ResetAndMinimize();
	return ::StmtReset(iStmtHandle);
	}

/**
Executes the prepared SQL statement.

@return KErrNone, the operation completed successfully;
		KSqlErrStmtExpired, statement expired (if new functions or collating sequences are 
							registered or if an authorizer function is added or changed);
		KErrNoMemory, an out of memory condition has occurred. The statement will be reset.

@panic SqlDb 2 In _DEBUG mode. Invalid (not created) CSqlSrvStatement object.
@panic SqlDb 7 In _DEBUG mode. SQLITE internal error. (SQLITE_ERROR, followed by a sqlite3_reset(), which returns SQLITE_OK)
*/	
inline TInt CSqlSrvStatement::Exec()
	{
	__ASSERT_DEBUG(iStmtHandle != NULL, __SQLPANIC(ESqlPanicInvalidObj));
	TInt err = ::StmtExec(iStmtHandle);
	iBufFlatType = static_cast <TSqlBufFlatType> (-1);
	iBufFlat.ResetAndMinimize();
	return err;
	}

/**
@panic SqlDb 2 In _DEBUG mode. Invalid (not created) CSqlSrvStatement object.
*/
inline const RSqlBufFlat& CSqlSrvStatement::BufFlatL(TSqlBufFlatType aWhat) const
	{
	__ASSERT_DEBUG(iStmtHandle != NULL, __SQLPANIC(ESqlPanicInvalidObj));
	if(aWhat != iBufFlatType)
		{
		__SQLLEAVE(KErrArgument);
		}
	return iBufFlat;
	}
			
/**
*/	
inline CSqlSrvStatement::CSqlSrvStatement()
	{
	}

/**
Initializes CSqlSrvStatement instance.

@param aDbHandle The database handle
@param aSqlStmt 16-bit SQL statement, zero-terminated string

@leave KErrNoMemory, an out of memory condition has occurred;
	   KErrArgument, bad argument, for example - the SQL string contains more than one SQL statements;
                  Note that the function may also leave with some other database specific 
                  errors categorised as ESqlDbError.

@panic SqlDb 4 In _DEBUG mode. aDbHandle is NULL.
*/	
inline void CSqlSrvStatement::ConstructL(sqlite3* aDbHandle, const TDesC16& aSqlStmt)
	{
	__ASSERT_DEBUG(aDbHandle != NULL, __SQLPANIC(ESqlPanicBadArgument));
	iStmtHandle = ::StmtPrepare16L(aDbHandle, aSqlStmt);
	DoCommonConstructL();
	}
	
/**
Initializes CSqlSrvStatement instance.

@param aDbHandle The database handle
@param aSqlStmt 8-bit SQL statement, zero-terminated string

@leave KErrNoMemory, an out of memory condition has occurred;
	   KErrArgument, bad argument, for example - the SQL string contains more than one SQL statements;
                  Note that the function may also leave with some other database specific 
                  errors categorised as ESqlDbError.

@panic SqlDb 4 In _DEBUG mode. aDbHandle is NULL.
*/	
inline void CSqlSrvStatement::ConstructL(sqlite3* aDbHandle, const TDesC8& aSqlStmt)
	{
	__ASSERT_DEBUG(aDbHandle != NULL, __SQLPANIC(ESqlPanicBadArgument));
	iStmtHandle = ::StmtPrepare8L(aDbHandle, aSqlStmt);
	DoCommonConstructL();
	}
