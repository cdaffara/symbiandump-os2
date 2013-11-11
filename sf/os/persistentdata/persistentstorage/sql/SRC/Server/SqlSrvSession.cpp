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
// NTT DOCOMO, INC - Fix for Bug 1915 "SQL server panics when using long column type strings"
//
// Description:
//

#include "SqlSrvMain.h"			//CSqlServer
#include "SqlSrvSession.h"		//CSqlSrvSession
#include "SqlSrvStatement.h"	//CSqlSrvStatement, HSqlSrvStmtParamBuf
#include "SqlSecurityImpl.h"	//CSqlSecurityPolicy
#include "SqlSrvUtil.h"			//Global server functions
#include "SqlUtil.h"			// config length
#include "SqlSrvDriveSpace.h"	//CSqlDriveSpace, RSqlDriveSpaceCol
#include "SqlSrvBlob.h"
#include "SqlResourceProfiler.h"
#include "SqlCompact.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "SqlSrvSessionTraces.h"
#endif
#include "SqlTraceDef.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma BullseyeCoverage off

#ifdef _DEBUG

const TInt KDelayedDbHeapFailureMask = 0x1000;

//Puts the database connection in a test mode
//Returns true if the heap failure simulation has to be delayed untill the database is opened
//Initialises iDbResourceTestMode and iFailedAllocNumber (if delayed simulation) data members
inline TBool CSqlSrvSession::ActivateDbTestMode(TInt aHeapFailureMode, TInt aFailedAllocNumber)
	{
	iDbResourceTestMode = aHeapFailureMode;
	if(aHeapFailureMode & KDelayedDbHeapFailureMask)
		{
		iFailedAllocNumber = aFailedAllocNumber;
		return ETrue;
		}
	return EFalse;
	}

//If the database connection is in a test mode then the macro will reset the heap allocation failure type.
//and stop the test mode.
inline void CSqlSrvSession::StopDbTestMode()
	{
	if(iDbResourceTestMode)
		{
		iDbResourceTestMode = 0;
		User::__DbgSetAllocFail(RHeap::EUser, RHeap::ENone, 0);
		}
	}

//If the database connection is in a test mode then the function will mark the allocated by the 
//server resources.
inline void CSqlSrvSession::DbResourceMark()
	{
	if(iDbResourceTestMode)
		{
		ResourceCountMarkStart();
		}
	}

//If the database connection is in a test mode then the macro will check the allocated by the server resources,
//comparing their count with the resource count at the moment DbResourceMark() has been called.
//The client will be panicked if the resource count now is different.
inline void CSqlSrvSession::DbResourceEnd(const RMessage2& aMessage)
	{
	if(iDbResourceTestMode)
		{
		ResourceCountMarkEnd(aMessage);
		}
	}

//Executes the heap simulation failure.
inline void CSqlSrvSession::DbSetAllocFail(TInt aHeapFailureMode, TInt aFailedAllocNumber)
	{
	TInt mode = aHeapFailureMode & (KDelayedDbHeapFailureMask - 1);
	if(mode >= RAllocator::EBurstRandom && mode <= RAllocator::EBurstFailNext)
	    {
	    const TUint KBurst = 50; 
	    User::__DbgSetBurstAllocFail(RHeap::EUser, static_cast <RHeap::TAllocFail> (mode), aFailedAllocNumber, KBurst);
	    }
	else
	    {
        User::__DbgSetAllocFail(RHeap::EUser, static_cast <RHeap::TAllocFail> (mode), aFailedAllocNumber);
	    }
	}
	
//Executes the delayed heap simulation failure, if the connection is in test mode
inline void CSqlSrvSession::DbSetDelayedAllocFail()
	{
	if(iDbResourceTestMode & KDelayedDbHeapFailureMask)
		{
	    TInt mode = iDbResourceTestMode & (KDelayedDbHeapFailureMask - 1);
	    if(mode >= RAllocator::EBurstRandom && mode <= RAllocator::EBurstFailNext)
	        {
	        const TUint KBurst = 50; 
	        User::__DbgSetBurstAllocFail(RHeap::EUser, static_cast <RHeap::TAllocFail> (mode), iFailedAllocNumber, KBurst);
	        }
	    else
	        {
	        User::__DbgSetAllocFail(RHeap::EUser, static_cast <RHeap::TAllocFail> (mode), iFailedAllocNumber);
	        }
		}
	}
	
#else //_DEBUG

inline TBool CSqlSrvSession::ActivateDbTestMode(TInt, TInt)
	{
	return EFalse;
	}

inline void CSqlSrvSession::StopDbTestMode()
	{
	}

inline void CSqlSrvSession::DbResourceMark()
	{
	}

inline void CSqlSrvSession::DbResourceEnd(const RMessage2&)
	{
	}

inline void CSqlSrvSession::DbSetAllocFail(TInt, TInt)
	{
	}
	
inline void CSqlSrvSession::DbSetDelayedAllocFail()
	{
	}

#endif//_DEBUG

#pragma BullseyeCoverage on

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
Searches aContainer for an object identified by aHandle.
If such object exists, a reference to it is returned.
If there is no object, the client gets a panic.

@panic SqlDb 4 Client panic. Invalid aHandle parameter value (zero, negative or out of range).

@internalComponent
*/
template <class T> T& SqlSessObjFind(RDbObjContainer<T>& aContainer, TInt aHandle, const RMessage2& aMessage)
	{
	T* obj = aContainer.Find(aHandle);
	__SQLPANIC_CLIENT2(obj != NULL, aMessage, ESqlPanicBadArgument);
	return *obj;
	}

//This function return true, if there is free disk space on drive where the main database file is.
static TBool HasFreeDiskSpace(RFs& aFs, TDriveNumber aDrive)
	{
	TVolumeInfo volInfo;
	TInt err = aFs.Volume(volInfo, aDrive);
	if(err == KErrNone)
		{
		const TInt64 KDiskSpaceThreshold = 1024 * 4;
		return volInfo.iFree > KDiskSpaceThreshold;
		}
	return ETrue;
	}
	
//If aError is KSqlErrFull and  there is no free disk space, then KSqlErrFull is converted to KErrDiskFull
//and returned.
static TInt ConvertSqlFull2DiskFullErr(TInt aError, RFs& aFs, TDriveNumber aDrive)
	{
	if(aError == KSqlErrFull && !HasFreeDiskSpace(aFs, aDrive))
		{
		aError = KErrDiskFull;
		SQL_TRACE_SESSION(OstTrace1(TRACE_INTERNALS, CONVERTSQLFULL2DISKFULLERRROR, "0;ConvertSqlFull2DiskFullErr;aError=KSqlErrFull;!HasFreeDiskSpace();aDrive=%d", (TInt)aDrive));
		}
	return aError;
	}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
Creates a new instance of CSqlSrvSession class.

This function shall never be called directly.
It is CSqlServer responsibility to create a new server side session object as a responce to the criation of a
client side session instance.

@return A pointer to the created CSqlSrvSession instance.

@leave KErrNoMemory, an out of memory condition has occurred;

@see CSqlServer
@see CSqlServer::NewSessionL()
*/
CSqlSrvSession* CSqlSrvSession::NewL()
	{
	SQL_TRACE_SESSION(OstTrace0(TRACE_INTERNALS, CSQLSRVSESSION_NEWL_ENTRY, "Entry;0;CSqlSrvSession::NewL"));
	CSqlSrvSession* self = new (ELeave)	CSqlSrvSession;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	SQL_TRACE_SESSION(OstTrace1(TRACE_INTERNALS, CSQLSRVSESSION_NEWL_EXIT, "Exit;0x%X;CSqlSrvSession::NewL", (TUint)self));
	return self;
	}

/**
Frees the allocated by CSqlSrvSession instance resources and memory.
*/
CSqlSrvSession::~CSqlSrvSession()
	{
	SQL_TRACE_SESSION(OstTraceExt2(TRACE_INTERNALS, CSQLSRVSESSION_CSQLSRVSESSION2_ENTRY, "Entry;0x%X;CSqlSrvSession::~CSqlSrvSession;iDatabase=0x%X", (TUint)this, (TUint)iDatabase));
	StopDbTestMode();
	DbFreeReservedSpace();
	iIpcStreams.Close();
	iStatements.Close();
	delete iDatabase;
	SQL_TRACE_SESSION(OstTrace1(TRACE_INTERNALS, CSQLSRVSESSION_CSQLSRVSESSION2_EXIT, "Exit;0x%X;CSqlSrvSession::~CSqlSrvSession", (TUint)this));
	}

/**
Receives and dispatches all client side requests.

CSession2::ServiceL() implementation.

@param aMessage Client message containing the request (function code and data)

@leave The function may leave with some database specific 
	   errors categorised as ESqlDbError or system-wide error codes.

@see CSession2::ServiceL()
*/
void CSqlSrvSession::ServiceL(const RMessage2& aMessage)
	{
	TSqlSrvFunction funcCode = ESqlSrvTestBase;
	TInt handle = 0;
	Extract(aMessage, funcCode, handle);
	TInt retCode = KErrNone;
	if(funcCode >= ESqlSrvDbBase)
		{
		SQLPROFILER_REPORT_IPC(ESqlIpcRq, 0);
		}
	__SQLTRACE_SESSIONEXPR(++iIpcCallCounter);
    SQL_TRACE_SESSION(OstTraceExt4(TRACE_INTERNALS, CSQLSRVSESSION_SERVICEL_ENTRY, "Entry;0x%X;CSqlSrvSession::ServiceL;aMessage.Handle()=0x%X;funcCode=0x%X;iIpcCallCounter=%u", (TUint)this, (TUint)aMessage.Handle(), (TUint)funcCode, iIpcCallCounter));
	SQLPROFILER_IPC_START(iIpcCallCounter, iDatabase ? (TUint)iDatabase->RawDbHandle() : 0);
	switch(funcCode)
		{
		//////////////////////  resource check operations  ///////////////////////////
		case ESqlSrvResourceMark:
			ResourceCountMarkStart();
			break;
		case ESqlSrvResourceCheck:
			ResourceCountMarkEnd(aMessage);
			break;
		case ESqlSrvResourceCount:
			retCode = CountResources();					//Returns the recourse count
			break;
		case ESqlSrvSetDbHeapFailure:
			if(ActivateDbTestMode(aMessage.Int0(), aMessage.Int1()))
				{
				break;	
				}
		case ESqlSrvSetHeapFailure:
			DbSetAllocFail(aMessage.Int0(), aMessage.Int1());
			break;
		//////////////////////   profiling operations  //////////////////////////////////
		case ESqlSrvProfilerStart:
			TSqlSrvResourceProfiler::StartL(aMessage);
			break;
		case ESqlSrvProfilerStop:
			TSqlSrvResourceProfiler::StopL(aMessage);
			break;
		case ESqlSrvProfilerReset:
			TSqlSrvResourceProfiler::ResetL(aMessage);
			break;
		case ESqlSrvProfilerQuery:
			ProfilerQueryL(aMessage);
			break;
		//////////////////////   database operations //////////////////////////////////
		case ESqlSrvDbCreate:
		case ESqlSrvDbCreateSecure:
		case ESqlSrvDbOpen:
			DbResourceMark();
			DbCreateObjectL(aMessage, funcCode);
			DbSetDelayedAllocFail();
			break;
		case ESqlSrvDbOpenFromHandle:
			DbResourceMark();
			DbCreateObjectFromHandleL(aMessage);
			DbSetDelayedAllocFail();
			break;
		case ESqlSrvDbAttach:
			DbAttachL(aMessage);
			break;
		case ESqlSrvDbAttachFromHandle:
			DbAttachFromHandleL(aMessage);
			break;
		case ESqlSrvDbDetach:
			DbDetachL(aMessage);
			break;
		case ESqlSrvDbClose:
			DbDestroyObject();
			DbResourceEnd(aMessage);
			StopDbTestMode();
			break;
		case ESqlSrvDbCopy:
			DbResourceMark();
			DbCopyFileL(aMessage);
			break;
		case ESqlSrvDbDelete:
			DbResourceMark();
			DbDeleteFileL(aMessage);
			break;
		case ESqlSrvLastErrorMsg:
			retCode = DbLastErrorMessageL(aMessage);//may return that the client buffer is not big enough for the message
			break;
		case ESqlSrvDbLastInsertedRowId:
			DbLastInsertedRowIdL(aMessage);
			break;
		case ESqlSrvDbExec8:
			retCode = DbExecSql8L(aMessage);		//returns the count of affected records
			break;
		case ESqlSrvDbExec16:
			retCode = DbExecSql16L(aMessage);	//returns the count of affected records
			break;
		case ESqlSrvDbSetIsolationLevel:
			DbSetIsolationLevelL(aMessage);
			break;
		case ESqlSrvDbGetSecurityPolicy:
			retCode = DbGetSecurityPolicyL(aMessage);//may return that the client buffer is not big enough for the security policy
			break;
		case ESqlSrvDbScalarFullSelect8:
			retCode = DbScalarFullSelectL(aMessage, EFalse);//may return that the client buffer is not big enough for the column value
			break;
		case ESqlSrvDbScalarFullSelect16:
			retCode = DbScalarFullSelectL(aMessage, ETrue);//may return that the client buffer is not big enough for the column value
			break;
		case ESqlSrvDbInTransaction:
			retCode = DbInTransaction(aMessage);	//Returns whether the database in in transaction or not - true/false
			break;
		case ESqlSrvDbSize:
			retCode = DbSizeL(aMessage);			//Returns the database size
			break;
		case ESqlSrvDbSize2:
			DbSize2L(aMessage);
			break;
		case ESqlSrvDbCompact:
			retCode = DbCompactL(aMessage);			//Returns the amount of the removed free database space
			break;
		//////////////////////   reserved drive space management ////////////////////////
		case ESqlSrvDbReserveDriveSpace:
			__SQLPANIC_CLIENT(iDatabase != NULL, aMessage, ESqlPanicInvalidObj);
			DbReserveDriveSpaceL();
			break;
		case ESqlSrvDbFreeReservedSpace:
			__SQLPANIC_CLIENT(iDatabase != NULL, aMessage, ESqlPanicInvalidObj);
			DbFreeReservedSpace();
			break;
		case ESqlSrvDbGetReserveAccess:
			__SQLPANIC_CLIENT(iDatabase != NULL, aMessage, ESqlPanicInvalidObj);
			DbGetReserveAccessL();
			break;
		case ESqlSrvDbReleaseReserveAccess:
			__SQLPANIC_CLIENT(iDatabase != NULL, aMessage, ESqlPanicInvalidObj);
			DbReleaseReserveAccess();
			break;
		//////////////////////   BLOB source ///////////////////////////////////////////
		case ESqlSrvDbBlobSource:
			retCode = DbBlobSourceL(aMessage);		//Returns the BLOB handle
			break;
		//////////////////////   statement operations //////////////////////////////////
		case ESqlSrvStmtPrepare8:
			retCode = StmtPrepareL(aMessage, EFalse);//returns the statement handle
			break;
		case ESqlSrvStmtPrepare16:
			retCode = StmtPrepareL(aMessage, ETrue);//returns the statement handle
			break;
		case ESqlSrvStmtClose:
			iStatements.Remove(handle);
			break;
		case ESqlSrvStmtReset:
			retCode = ::SqlSessObjFind(iStatements, handle, aMessage).Reset();//May return that the statement has expired
			break;
		case ESqlSrvStmtExec:
		case ESqlSrvStmtAsyncExec:
		case ESqlSrvStmtBindExec:
		case ESqlSrvStmtAsyncBindExec:
			retCode = StmtExecL(aMessage, handle, funcCode);//returns the count of affected records
			break;
		case ESqlSrvStmtNext:
		case ESqlSrvStmtBindNext:
			retCode = StmtNextL(aMessage, handle, funcCode);//returns a non-negative number if the client side buffer is too small
			break;
		case ESqlSrvStmtColumnNames:
		case ESqlSrvStmtParamNames:
			retCode = StmtNamesL(aMessage, handle, funcCode);//returns a non-negative number if the client side buffer is too small
			break;
		case ESqlSrvStmtColumnSource:
			retCode = StmtColumnSourceL(aMessage, handle);//returns an IPC stream handle
			break;
		case ESqlSrvStmtBinParamSink:
		case ESqlSrvStmtTxtParamSink16:
			retCode = StmtParamSinkL(aMessage, handle, funcCode);//returns an IPC stream handle
			break;
		case ESqlSrvStmtBufFlat:
			StmtGetBufFlatL(aMessage, handle);
			break;
		case ESqlSrvStmtColumnValue:
			StmtColumnValueL(aMessage, handle);
			break;
		case ESqlSrvStmtDeclColumnTypes:
			retCode = StmtDeclColumnTypesL(aMessage, handle);
			break;
		//////////////////////   stream operations //////////////////////////////////
		case ESqlSrvStreamRead:
			retCode = ::SqlSessObjFind(iIpcStreams, handle, aMessage).ReadL(aMessage);
			break;
		case ESqlSrvStreamWrite:
			::SqlSessObjFind(iIpcStreams, handle, aMessage).WriteL(aMessage);
			break;
		case ESqlSrvStreamSize:
			retCode = ::SqlSessObjFind(iIpcStreams, handle, aMessage).SizeL();
			break;
		case ESqlSrvStreamSynch:
			::SqlSessObjFind(iIpcStreams, handle, aMessage).SynchL();
			break;
		case ESqlSrvStreamClose:
			iIpcStreams.Remove(handle);
			break;
		//////////////////////                     //////////////////////////////////
		default:	
			retCode = KErrNotSupported;
			break;
		}
	Server().Compactor().RestartTimer();
	Server().MinimizeBuffers();		
	if(!aMessage.IsNull())
		{
		aMessage.Complete(retCode);
		}
	SQL_TRACE_SESSION(OstTraceExt4(TRACE_INTERNALS, CSQLSRVSESSION_SERVICEL_EXIT, "Exit;0x%X;CSqlSrvSession::ServiceL;funcCode=0x%X;retCode=%d;iIpcCallCounter=%u", (TUint)this, (TUint)funcCode, retCode, iIpcCallCounter));
	}

/**
If aError is KErrBadDescriptor, then panic the client, else - default error handling.
KErrBadDescriptor error may be thrown from "message write" operations, if the client supplied a bad
descriptor to the server.
*/
void CSqlSrvSession::ServiceError(const RMessage2& aMessage, TInt aError)
 	{
	SQL_TRACE_SESSION(OstTraceExt4(TRACE_INTERNALS, CSQLSRVSESSION_SERVICEERROR_ENTRY, "Entry;0x%X;CSqlSrvSession::ServiceError;aMessage.Function()=0x%X;aError=%d;iIpcCallCounter=%u", (TUint)this, (TUint)aMessage.Function(), aError, iIpcCallCounter));
	Server().MinimizeBuffers();		
	aError = ::ConvertSqlFull2DiskFullErr(aError, Server().FileData().Fs(), iDrive);
 	if(aError == KErrBadDescriptor)
 		{
		//The __SQLPANIC_CLIENT() macro cannot be used here because it calls a leaving function. A leaving call
		//from a leaving call will terminate the server.
		_LIT(KPanicCategory, "SqlDb");
		aMessage.Panic(KPanicCategory, ESqlPanicBadDescriptor);
 		}
 	SQLPROFILER_IPC_ERROR(iIpcCallCounter, static_cast <TSqlSrvFunction> (KSqlSrvFunctionMask & aMessage.Function()), 
 	       iDatabase ? (TUint)iDatabase->RawDbHandle() : 0, aError);
 	CSession2::ServiceError(aMessage, aError);
 	SQL_TRACE_SESSION(OstTraceExt3(TRACE_INTERNALS, CSQLSRVSESSION_SERVICEERROR_EXIT, "Exit;0x%X;CSqlSrvSession::ServiceError;aMessage.Function()=0x%X;iIpcCallCounter=%u", (TUint)this, (TUint)aMessage.Function(), iIpcCallCounter));
   	}

///////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////          Profiler  operations           ///////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma BullseyeCoverage off

/**
Retrieves the counter values for the specified profiling counter.

@leave  KErrNone, the operation completed successfully,
        KErrOverflow, the receiving buffer size is too small;
                  One of the other system-wide error codes may also be returned.

@see TSqlResourceProfiler

Usage of the IPC call arguments:
 - Arg 0: [in]  profiling counter type, one of the TSqlResourceProfiler::TSqlCounter enum item values.
 - Arg 1: [in]  the size of the buffer for the profiling counter values.
 - Arg 2: [out] the buffer for the profiling counter values.

@panic SqlDb 2 Client panic. iDatabase is NULL (the database object is not created yet).
*/
void CSqlSrvSession::ProfilerQueryL(const RMessage2& aMessage)
	{
	__SQLPANIC_CLIENT(iDatabase != NULL, aMessage, ESqlPanicInvalidObj);
	if(aMessage.Int0() == TSqlResourceProfiler::ESqlCounterConfig)
		{
		const TInt KConfigBufLen = 128;
		if(aMessage.Int1() < KConfigBufLen)
			{
			__SQLLEAVE(KErrOverflow);	
			}
		TBuf8<KConfigBufLen> res;
		iDatabase->QueryConfigL(res);
		aMessage.WriteL(2, res);
		}
	else
		{
		TSqlSrvResourceProfiler::QueryL(aMessage);
		}				
	}

#pragma BullseyeCoverage on

///////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////          Database operations           ///////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////

/**
Processes the request for creating/opening a database.

The function initializes iDatabase and iDrive data members.

Usage of the IPC call arguments:
Arg 0: [in]  database file name length in 16-bit characters
Arg 1: [in]  database file name
Arg 2: [in]  PPPPCCCC, where PPPP is the security policy length, CCCC is the config string length.
Arg 3: [in]  security policies buffer | config string

@panic SqlDb 1 Client panic. iDatabase is not NULL (it has been created already)
@panic SqlDb 4 Client panic. Negative or too big config string length
@panic SqlDb 4 Client panic. Negative security policy length, or zero length if the request is to create a secure database 
*/
void CSqlSrvSession::DbCreateObjectL(const RMessage2& aMessage, TSqlSrvFunction aFunction)
	{
	__SQLPANIC_CLIENT(!iDatabase, aMessage, ESqlPanicObjExists);
	const TInt KSecurityPolicyLen = (aMessage.Int2() & 0x7fff0000) >> 16;
    //If the security policy length is negative then this is a programming error.
    __SQLPANIC_CLIENT(KSecurityPolicyLen >= 0, aMessage, ESqlPanicBadArgument);
	const TInt KConfigStringLen = aMessage.Int2() & 0xffff;
	//If KConfigStringLen is invalid then this is a programming error. 
	//If the client sends a too big config string - this is handled in the client side session.
    __SQLPANIC_CLIENT((TUint)KConfigStringLen <= KSqlSrvMaxConfigStrLen, aMessage, ESqlPanicBadArgument);
	RBuf8 securityAndConfigBuf;
	CleanupClosePushL(securityAndConfigBuf);
	if((KSecurityPolicyLen + KConfigStringLen) > 0)
	    {
	    securityAndConfigBuf.CreateL(KSecurityPolicyLen + KConfigStringLen);
        aMessage.ReadL(3, securityAndConfigBuf); 
        SQLPROFILER_REPORT_IPC(ESqlIpcRead, (KSecurityPolicyLen + KConfigStringLen));
	    }
	TSqlSrvFileData& fileData = Server().FileData();
	TPtrC8 configStr(KNullDesC8);
	if(KConfigStringLen > 0)
	    {
	    configStr.Set(securityAndConfigBuf.Mid(KSecurityPolicyLen));//the first part of the buffer is for the security policies
	    }
	fileData.SetL(aMessage, aMessage.Int0(), 1, &configStr);
	iDrive = fileData.Drive();
	switch(aFunction)
		{
		case ESqlSrvDbCreate:
			if(fileData.IsSecureFileNameFmt())
				{
				__SQLLEAVE(KErrArgument);	
				}
			iDatabase = CSqlSrvDatabase::CreateL(fileData);
			break;
		case ESqlSrvDbCreateSecure:
			{
		    __SQLPANIC_CLIENT(KSecurityPolicyLen > 0, aMessage, ESqlPanicBadArgument);
			if(!fileData.IsSecureFileNameFmt())
				{
				__SQLLEAVE(KErrArgument);	
				}
			//The caller can create a secure database which secure UID matches his secure UID.
			if(fileData.SecureUid() != aMessage.SecureId())
				{
				__SQLLEAVE(KErrPermissionDenied);	
				}
			CSqlSecurityPolicy* policy = CreateSecurityPolicyL(securityAndConfigBuf.Left(KSecurityPolicyLen));
			iDatabase = CSqlSrvDatabase::CreateSecureL(fileData, policy);
			}
			break;
		case ESqlSrvDbOpen:
			iDatabase = CSqlSrvDatabase::OpenL(fileData);
			break;
		default:
			__SQLLEAVE(KErrArgument);	
			break;
		}
	CleanupStack::PopAndDestroy(&securityAndConfigBuf);
	}

/**
Processes the request for opening a database from file handle.
The server expects that the database to be opened/created is in the applicatio's private data cage.

Usage of the IPC call arguments:
 - Arg 0: [in]  The 32 bits of the argument are used as follow:
 @code
 MSB                                                                                 LSB
 31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0
 Ro Cr  C  C  C  C  C  C  C  C  C  C  C  C  C  C  F  F  F  F  F  F F F F F F F F F F F 
 @endcode
 Where:
 @code
  - "Ro" - read-only flag, true if the file is read-only;
  - "Cr" - create/open flag, true if the file was created, false if the file was opened;
  - "C"  - config string length in 16-bit characters;
  - "F"  - database file name length in 16-bit characters;
 @endcode
 - Arg 1: [in]  database file name | config string
 - Arg 2: [in]  file session handle
 - Arg 3: [in]  database file handle

@panic SqlDb 1 Client panic. iDatabase is not NULL (it has been created already)
*/
void CSqlSrvSession::DbCreateObjectFromHandleL(const RMessage2& aMessage)
	{
	__SQLPANIC_CLIENT(!iDatabase, aMessage, ESqlPanicObjExists);
	const TBool KReadOnly = (aMessage.Int0() & 0x80000000) != 0;
	const TBool KCreated = (aMessage.Int0() & 0x40000000) != 0;
	const TInt KDbFileNameLen = aMessage.Int0() & 0x0000FFFF;
	const TInt KConfigStringLen = (aMessage.Int0() & 0x3FFF0000) >> 16;
    __SQLPANIC_CLIENT((TUint)KConfigStringLen <= KSqlSrvMaxConfigStrLen, aMessage, ESqlPanicBadArgument);
    __SQLPANIC_CLIENT((TUint)KDbFileNameLen <= KMaxFileName, aMessage, ESqlPanicBadArgument);
	TDes16& buffer = Server().GetBuf16L(KDbFileNameLen + KConfigStringLen);
	aMessage.ReadL(1, buffer);
	SQLPROFILER_REPORT_IPC(ESqlIpcRead, ((KDbFileNameLen + KConfigStringLen) * sizeof(TText)));
	TFileName dbFileName;
	dbFileName.Copy(buffer.LeftTPtr(KDbFileNameLen));
	TBuf8<KSqlSrvMaxConfigStrLen> configStr;
	if(KConfigStringLen > 0)
		{
		configStr.Copy(buffer.MidTPtr(KDbFileNameLen, KConfigStringLen));
		}
	TSqlSrvFileData& fileData = Server().FileData();
	fileData.SetFromHandleL(aMessage, dbFileName, KCreated, KReadOnly, &configStr);
	iDrive = fileData.Drive();
	iDatabase = CSqlSrvDatabase::OpenL(fileData);
	}

/**
Processes the request for copying a database.

Only the database creator can copy the database if the database is a secure database.

Usage of the IPC call arguments:
Arg 0: [in]  source database file name length
Arg 1: [in]  source database file name
Arg 2: [in]  destination database file name length
Arg 3: [in]  destination database file name
*/
void CSqlSrvSession::DbCopyFileL(const RMessage2& aMessage)
	{
	const TInt KDbCnt = 2;											//"2" - because we have 2 dbases: src and dest
	const TInt KSrcDbIdx = 0;
	const TInt KDestDbIdx = 1;
	TInt fileNameLen[KDbCnt] = {aMessage.Int0(), aMessage.Int2()};
	TSqlSrvFileData& fileData = Server().FileData();
	TUint dbSecureFlag[KDbCnt];
	TUid  dbSID[KDbCnt] = {KNullUid, KNullUid};
	TFileName dbFileName[KDbCnt];
	//Initialize dbSecureFlag[], dbSID[] and dbFileName[] array elements
	for(TInt i=0;i<KDbCnt;++i)									
		{
		fileData.SetL(aMessage, fileNameLen[i], i * KDbCnt + 1);	//"i * KDbCnt + 1" is the RMessage2 parameter number: 1 for src db, 3 for dest db
		dbSecureFlag[i] = fileData.IsSecureFileNameFmt() ? 1 : 0;
		if(dbSecureFlag[i])
			{
			dbSID[i] = fileData.SecureUid();
			}
		dbFileName[i].Copy(fileData.FileName());
		}
	//It is not allowed to copy non-secure to a secure or secure to a non-secure database.
	if(dbSecureFlag[KSrcDbIdx] ^ dbSecureFlag[KDestDbIdx])
		{
		__SQLLEAVE(KErrPermissionDenied);	
		}
	//If this is a secure database "copy" operation, then...
	if(dbSecureFlag[KSrcDbIdx])
		{
		TUid callerSid = aMessage.SecureId();
		//A secure database can be copied only by its owner (database SID matches caller SID).
		if(callerSid != dbSID[KSrcDbIdx] || callerSid != dbSID[KDestDbIdx])
			{
			__SQLLEAVE(KErrPermissionDenied);	
			}
		}
	//Copy the database
	CFileMan* fileMan = CFileMan::NewL(fileData.Fs());
	CleanupStack::PushL(fileMan);
	__SQLLEAVE_IF_ERROR(fileMan->Copy(dbFileName[KSrcDbIdx], dbFileName[KDestDbIdx]));
	//"Copy" operation executed without errors. Now it is a time to turn off the read-only
	//flag of the target file (which may be on if the source file is on a read-only drive)
	__SQLLEAVE_IF_ERROR(fileData.Fs().SetAtt(dbFileName[KDestDbIdx], 0, KEntryAttReadOnly));
	CleanupStack::PopAndDestroy(fileMan);
	}

/**
Processes the request for deleting a database.

Only the database creator can delete the database if the database is a secure database.

Usage of the IPC call arguments:
Arg 0: [in]  database file name length
Arg 1: [in]  database file name
*/
void CSqlSrvSession::DbDeleteFileL(const RMessage2& aMessage)
	{	
	TSqlSrvFileData& fileData = Server().FileData();
	fileData.SetL(aMessage, aMessage.Int0(), 1);
	if(fileData.IsSecureFileNameFmt())
		{
		//A secure database can be deleted only by its owner (database SID matches caller SID).
		if(fileData.SecureUid() != aMessage.SecureId())
			{
			__SQLLEAVE(KErrPermissionDenied);	
			}
		}
	__SQLTRACE_SESSIONVAR(TPtrC fname(fileData.FileName()));
	SQL_TRACE_SESSION(OstTraceExt2(TRACE_INTERNALS, CSQLSRVSESSION_DBDELETEFILEL, "0x%X;CSqlSrvSession::DbDeleteFileL;file='%S'", (TUint)this, __SQLPRNSTR(fname)));
	__SQLLEAVE_IF_ERROR(fileData.Fs().Delete(fileData.FileName()));
	}

/**
Processes the request for retrieving the last error message.

If the client side buffer size is not big enough, the function returns the needed 
buffer size + KSqlClientBufOverflowCode.
In this case the client must increase the buffer and try again to get the message.

Usage of the IPC call arguments:
Arg 0: [in]		Message buffer length in 16-bit characters
Arg 1: [in/out]	Message buffer

@panic SqlDb 2 Client panic. iDatabase is NULL (the database object is not created yet).
@panic SqlDb 4 Client panic. Negative client message buffer length (Arg 0).
*/
TInt CSqlSrvSession::DbLastErrorMessageL(const RMessage2& aMessage)
	{
	__SQLPANIC_CLIENT(iDatabase != NULL, aMessage, ESqlPanicInvalidObj);
	TPtrC msg = iDatabase->LastErrorMessage();
	TInt msgLen = msg.Length();
	TInt bufSize = aMessage.Int0();
	__SQLPANIC_CLIENT(bufSize >= 0, aMessage, ESqlPanicBadArgument);
	if(msgLen <= bufSize)
		{
		aMessage.WriteL(1, msg);
		SQLPROFILER_REPORT_IPC(ESqlIpcWrite, (msgLen * sizeof(TText)));
		return 0;
		}
	return msgLen + KSqlClientBufOverflowCode;
	}
	
/**
Processes the request for retrieving the last inserted ROWID of this database connection.

Usage of the IPC call arguments:
Arg 0: [in/out]	Receiving buffer

@panic SqlDb 2 Client panic. The database object is not yet created yet (iDatabase is NULL).
*/
void CSqlSrvSession::DbLastInsertedRowIdL(const RMessage2& aMessage)
	{
	__SQLPANIC_CLIENT(iDatabase != NULL, aMessage, ESqlPanicInvalidObj);
	TInt64 rowid = iDatabase->LastInsertedRowId();
	aMessage.WriteL(0, TPtrC8(reinterpret_cast <const TUint8*> (&rowid), sizeof(rowid)));
	SQLPROFILER_REPORT_IPC(ESqlIpcWrite, sizeof(rowid));
	}

/**
Processes the request for retrieving the database security policies.

The method leaves with KErrNotSupported if the database is not a secure database.

If the client side buffer size is not big enough, the function returns the needed 
buffer size + KSqlClientBufOverflowCode.
In this case the client must increase the buffer and try again to get the database security policy.

Usage of the IPC call arguments:
Arg 0: [in]		security policy buffer length in bytes
Arg 1: [in/out]	buffer for the database security policies

@panic SqlDb 2 Client panic. iDatabase is NULL (the database object is not created yet).
*/
TInt CSqlSrvSession::DbGetSecurityPolicyL(const RMessage2& aMessage)
	{
	__SQLPANIC_CLIENT(iDatabase != NULL, aMessage, ESqlPanicInvalidObj);
	const CSqlSecurityPolicy* securityPolicy = iDatabase->SecurityPolicy();
	if(!securityPolicy)
		{
		__SQLLEAVE(KErrNotSupported);
		}
	const RSqlBufFlat& bufFlat = securityPolicy->BufFlat();
	TInt size = bufFlat.Size();
	if(size <= aMessage.Int0())
		{
		aMessage.WriteL(1, bufFlat.BufDes());
		SQLPROFILER_REPORT_IPC(ESqlIpcWrite, size);
		return 0;
		}
	return size + KSqlClientBufOverflowCode;
	}

/**
If an error occurs during the execution the function leaves with the error code.
Possible non-leaving return values:
 - KErrNone              - the function has completed successfully;
 - Positive return value - the length of the column, which means - the destination buffer is too small.
                           This return value is possible only with text or binary columns.

@panic SqlDb 2 Client panic. iDatabase is NULL (the database object is not created yet).

Usage of the IPC call arguments: 
Arg 0: [in]		(8/16-bit character length of SQL statement) | (expected column value type << 24).
Arg 1: [in]		SQL statement.
Arg 2: [in]		Byte max length of the receiving buffer
Arg 3: [in/out]	The receiving buffer
*/
TInt CSqlSrvSession::DbScalarFullSelectL(const RMessage2& aMessage, TBool aIsText16)
	{
	__SQLPANIC_CLIENT(iDatabase != NULL, aMessage, ESqlPanicInvalidObj);
	TUint sqlLen = static_cast <TUint> (aMessage.Int0()) & 0x00FFFFFF;
	TSqlColumnType  colType = static_cast <TSqlColumnType> ((static_cast <TUint> (aMessage.Int0()) & 0xFF000000) >> 24);
	TInt columnCount = -1;
	TInt paramCount = -1;
	CSqlSrvStatement*  stmt = aIsText16 ? 
								CSqlSrvStatement::NewLC(iDatabase->RawDbHandle(), ReadString16ZL(aMessage, 1, sqlLen), columnCount, paramCount) :
								CSqlSrvStatement::NewLC(iDatabase->RawDbHandle(), ReadString8ZL(aMessage, 1, sqlLen), columnCount, paramCount);
	if(columnCount != 1 || paramCount != 0)
		{
		__SQLLEAVE(KErrArgument);	
		}
	TInt err = stmt->Next();
	if(err == KSqlAtRow)
		{
		err = GetColumnValueL(aMessage, *stmt, colType);
		}
	else
		{
		__SQLLEAVE(err == KSqlAtEnd ? KErrNotFound : err);
		}
	CleanupStack::PopAndDestroy(stmt);
	return err;
	}

/**
@return True, if the database is in transaction, false otherwise.

@panic SqlDb 2 Client panic. iDatabase is NULL (the database object is not created yet).
*/
TBool CSqlSrvSession::DbInTransaction(const RMessage2& aMessage)
	{
	__SQLPANIC_CLIENT(iDatabase != NULL, aMessage, ESqlPanicInvalidObj);
	return iDatabase->InTransaction();
	}

/**
Main database size in bytes.

@return Main database size in bytes.

@leave KErrNoMemory, an out of memory condition has occurred,
                     Note that the function may also leave with some other system wide errors or 
                     database specific errors categorised as ESqlDbError,
	   KErrTooBig,   The database is very big and the size cannot fit in a 32-bit signed integer.

@panic SqlDb 2 Client panic. iDatabase is NULL (the database object is not created yet).
*/
TInt CSqlSrvSession::DbSizeL(const RMessage2& aMessage)
	{
	__SQLPANIC_CLIENT(iDatabase != NULL, aMessage, ESqlPanicInvalidObj);
	TInt64 size = iDatabase->SizeL();
	if(size > KMaxTInt)
		{
		__SQLLEAVE(KErrTooBig);
		}
	return size;
	}

/**
Retrieves the database size and free space.

Usage of the IPC call arguments:
Arg 0: [in/out]	Points to a RSqlDatabase::TSize object, where the database size and free space values
			    will be copied.
Arg 1: [in]		The database name length in 16-bit characters
Arg 2: [in]		The attached database name or KNullDesC for the main database

@leave KErrNoMemory, an out of memory condition has occurred,
                     Note that the function may also leave with some other system wide errors or 
                     database specific errors categorised as ESqlDbError.
       KErrBadName, Invalid database name

@panic SqlDb 2 Client panic. iDatabase is NULL (the database object is not created yet).
*/
void CSqlSrvSession::DbSize2L(const RMessage2& aMessage)
	{
	__SQLPANIC_CLIENT(iDatabase != NULL, aMessage, ESqlPanicInvalidObj);
	const TInt KDbNameLen = aMessage.Int1();
	if((TUint)KDbNameLen > KMaxFileName)
		{
		__SQLLEAVE(KErrBadName);
		}
	TPtrC dbName(KNullDesC);
	if(KDbNameLen > 0)
		{
		dbName.Set(ReadString16L(aMessage, 2, KDbNameLen));
		}
	TPckgBuf<RSqlDatabase::TSize> data;
	data().iSize = iDatabase->SizeL(dbName);
	data().iFree = iDatabase->FreeSpaceL(dbName);
	aMessage.WriteL(0, data);
	SQLPROFILER_REPORT_IPC(ESqlIpcWrite, sizeof(RSqlDatabase::TSize));
	}

/**
Runs database compaction.

Usage of the IPC call arguments:
Arg 0: [in]	    How much space in bytes should be compacted, all free pages should be removed if the
				parameter value is RSqlDatabase::EMaxCompaction.
				Note that the requested space to be compacted will be rounded up to the nearest page count,
				e.g. request for removing 1 byte will remove one free page from the database file. 
Arg 1: [in]		The database name length in characters
Arg 2: [in]		The attached database name or KNullDesC for the main database

@return The size of the removed free space

@panic SqlDb 2 Client panic. iDatabase is NULL (the database object is not created yet).
*/
TInt CSqlSrvSession::DbCompactL(const RMessage2& aMessage)
	{
	__SQLPANIC_CLIENT(iDatabase != NULL, aMessage, ESqlPanicInvalidObj);
	const TInt KSize = aMessage.Int0();
	if(KSize < 0)
		{
		if(KSize != RSqlDatabase::EMaxCompaction)
			{
			__SQLLEAVE(KErrArgument);
			}
		}
	if(KSize == 0)
		{
		return 0;	
		}
	const TInt KDbNameLen = aMessage.Int1();
	if((TUint)KDbNameLen > KMaxFileName)
		{
		__SQLLEAVE(KErrBadName);
		}
	TPtrC dbName(KNullDesC);
	if(KDbNameLen > 0)
		{
		dbName.Set(ReadString16L(aMessage, 2, KDbNameLen));
		}
	return iDatabase->CompactL(KSize, dbName);
	}
	
/**
Usage of the IPC call arguments:
Arg 0: [in]		requested size of the space to be reserved - not used

The function leaves with KErrAlreadyExists if a drive space has been reserved already by this session.
*/
void CSqlSrvSession::DbReserveDriveSpaceL()
	{
	if(iDriveSpaceReserved)
		{
		__SQLLEAVE(KErrAlreadyExists);
		}
	RSqlDriveSpaceCol& driveSpaceCol = Server().DriveSpaceCol();
    if(!driveSpaceCol.Find(iDrive))
    	{
    	(void)driveSpaceCol.AddL(iDrive);
    	}
	
	iDriveSpaceReserved = ETrue;
	//Only iDriveSpaceReserved is set, nothing more needs to be done, because RSqlDriveSpaceCol::AddL() will
	//reserve a drive space on the specified drive.
	//Although it looks like that the implementation can ommit "iDriveSpaceReserved" flag, this flag plays important
	//role, because it is used to ensure that every "reserve drive space" request is matched by a "free drive space"
	//call.
	}
	
/**
If the client has been given an access to the reserved drive space, that access will be released.
*/
void CSqlSrvSession::DbFreeReservedSpace()
	{
	DbReleaseReserveAccess();
	iDriveSpaceReserved = EFalse;
	}
	
/**
The function leaves with KErrInUse if an access to the reserved drive space has been given to the client.
The function leaves with KErrNotFound if no drive space has been reserved for the drive, where the database file is.
*/
void CSqlSrvSession::DbGetReserveAccessL()
	{
	if(iDriveSpaceInUse)
		{
		__SQLLEAVE(KErrInUse);	
		}
	if(!iDriveSpaceReserved)
		{
		__SQLLEAVE(KErrNotFound);
		}
	CSqlDriveSpace* driveSpace = Server().DriveSpaceCol().Find(iDrive);
   	if(!driveSpace)
   		{
		__SQLLEAVE(KErrNotFound);
   		}
   	driveSpace->GetAccessL();
   	iDriveSpaceInUse = ETrue;
	}
	
/**
Releases the drive space reserve if it has been in use by this session (resp. client).
*/
void CSqlSrvSession::DbReleaseReserveAccess()
	{
	if(iDriveSpaceInUse)
		{
		CSqlDriveSpace* driveSpace = Server().DriveSpaceCol().Find(iDrive);
		if(driveSpace)
			{
   			driveSpace->ReleaseAccess();
			}
   		iDriveSpaceInUse = EFalse;
		}
	}

/**
Processes the request for attaching a secure or non-secure database.

Usage of the IPC call arguments: 
Arg 0: [in]	Database file name length (counted in 16-bit characters).
Arg 1: [in]	Database file name.
Arg 2: [in]	Logical database name length (counted in 16-bit characters).
Arg 3: [in]	Logical database name.

@panic SqlDb 2 Client panic. iDatabase is NULL (the database object is not created yet).
*/
void CSqlSrvSession::DbAttachL(const RMessage2& aMessage)
	{
	__SQLPANIC_CLIENT(iDatabase != NULL, aMessage, ESqlPanicInvalidObj);
	TSqlSrvFileData& fileData = Server().FileData();
	fileData.SetL(aMessage, aMessage.Int0(), 1);
	TInt logicalDbNameLen = aMessage.Int2();
	if(logicalDbNameLen < 1 || logicalDbNameLen > KMaxFileName)
		{
		__SQLLEAVE(KErrBadName);
		}
	iDatabase->AttachDbL(fileData, ReadString16L(aMessage, 3, logicalDbNameLen));
	}

/**
Processes the request for attaching a database using file session and file handles sent by the client.

Usage of the IPC call arguments:
 - Arg 0: [in]  The 32 bits of the argument are used as follow:
 @code
 MSB                                                                                 LSB
 31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0
 Ro  F  F  F  F  F  F  F  F  F  F  F  F  F  F  F  F  F  F  F  F  F F F F F F F F F F F
 @endcode
 Where:
 @code
  - "Ro" - read-only flag, true if the file is read-only;
  - "F"  - database file name length in 16-bit characters;
 @endcode
Arg 1: [in]  db names buffer
Arg 2: [in]  file session handle
Arg 3: [in]  database file handle

@panic SqlDb 2 Client panic. iDatabase is NULL (the database object is not created yet).
@panic SqlDb 4 Client panic. Invalid IPC data, an indication of a problme in client side sql library.
*/
void CSqlSrvSession::DbAttachFromHandleL(const RMessage2& aMessage)
	{
	__SQLPANIC_CLIENT(iDatabase != NULL, aMessage, ESqlPanicInvalidObj);
	//Read-only flag, buffer length, buffer allocation
	TBool readOnly = (aMessage.Int0() & 0x80000000) != 0;
	const TInt KBufLen = aMessage.Int0() & 0x7FFFFFFF;
    __SQLPANIC_CLIENT(KBufLen > 0, aMessage, ESqlPanicBadArgument);
	HBufC8* buf = HBufC8::NewLC(KBufLen);
	TPtr8 bufPtr = buf->Des();
	aMessage.ReadL(1, bufPtr);
	SQLPROFILER_REPORT_IPC(ESqlIpcRead, KBufLen);
	if(KBufLen != bufPtr.Length())
		{
		__SQLLEAVE(KErrArgument);
		}
	RDesReadStream in(bufPtr);
	TDes& dbFileName = Server().FileNameBuf();
	TDes16& dbName = Server().GetBuf16L(KMaxFileName);
	in >> dbFileName;
	in >> dbName;
	CleanupStack::PopAndDestroy(buf);
	TSqlSrvFileData& fileData = Server().FileData();
	fileData.SetFromHandleL(aMessage, dbFileName, EFalse, readOnly);
	iDatabase->AttachDbL(fileData, dbName);
	}
	
/**
Processes the request for detaching a secure or non-secure database.

Usage of the IPC call arguments: 
Arg 0: [in]	Logical database name length (counted in 16-bit characters).
Arg 1: [in]	Logical database name.

@panic SqlDb 2 Client panic. iDatabase is NULL (the database object is not created yet).
*/
void CSqlSrvSession::DbDetachL(const RMessage2& aMessage)
	{
	__SQLPANIC_CLIENT(iDatabase != NULL, aMessage, ESqlPanicInvalidObj);
	TInt logicalDbNameLen = aMessage.Int0();
	if(logicalDbNameLen < 1 || logicalDbNameLen > KMaxFileName)
		{
		__SQLLEAVE(KErrBadName);
		}
	iDatabase->DetachDbL(ReadString16L(aMessage, 1, logicalDbNameLen));
	}

/**
Reads a 16-bit string from the specified stream and returns it in zero-terminated
8-bit format in aNameOut.
If the string is of zero length then the substitute string provided will be used instead.

@param aStrm 				The read stream
@param aNameOut 			The output parameter that will contain the string read
@param aEmptyNameSubstitute The substitute string to use if the string to be read from
							the stream is zero length

@leave KErrNoMemory, 		 An out of memory condition has occurred;
	   KErrArgument, 		 The UTF-16 to UTF-8 string conversion failed;	
	   KErrBadName,	 		 The string has an invalid length;
*/
void CSqlSrvSession::ExtractNameL(RDesReadStream& aStrm, TDes8& aNameOut, const TDesC& aEmptyNameSubstitute)
	{
	 TBool replace = EFalse;
	 TInt32 len;
	 aStrm >> len;

	 if(len == 0)
	 	{
	   	if(aEmptyNameSubstitute.Length() > 0)
			{
		 	len = aEmptyNameSubstitute.Length();
	 	 	replace = ETrue;
			}
	 	else
			{
			__SQLLEAVE(KErrBadName);
			}
	  	}
	 __ASSERT_DEBUG(len > 0, __SQLPANIC2(ESqlPanicInternalError));//The "if" above should have hanled the case with "len == 0"
	 if((TUint)len > KMaxFileName)
	  {
	  __SQLLEAVE(KErrBadName);
	  }

	 HBufC* buf = HBufC::NewLC(len + 1);
	 TPtr ptr = buf->Des();
	 if(replace)
	 	{
	  	ptr.Copy(aEmptyNameSubstitute);
	  	}
	 else
	 	{
	  	aStrm >> ptr;
	 	}
	 ptr.Append(0);
	 
	 if(!::UTF16ZToUTF8Z(ptr, aNameOut))
	  {
	  __SQLLEAVE(KErrArgument);
	  }
	  
	 CleanupStack::PopAndDestroy(buf);
	 }

/**
Processes the request for creating an IPC stream for accessing the content of a blob column.

@param aMessage The client request wrapped in an IPC message

@return The blob stream handle

@leave KErrNoMemory, 		 An out of memory condition has occurred;
	   KErrArgument, 		 The ROWID is invalid or UTF-16 to UTF-8 string conversion failed;	
	   KErrBadDescriptor 	 The transferred data is bigger than the specified length;
	   KErrBadName,	 		 The table name, column name or database name has an invalid length;
       KErrPermissionDenied, The client does not have the required security capabilites for this operation; 						 
       						 Note that the function may also leave with some other system wide errors or 
                     		 database specific errors categorised as ESqlDbError.

@panic SqlDb 2 Client panic. The database object is not yet created (iDatabase is NULL).
@panic SqlDb 3 Client panic. Failed to create a blob stream handle.
@panic SqlDb 4 Client panic. IPC buffer length is 0.

Usage of the IPC call arguments:
Arg 0: [in]	    The length of the IPC data buffer
Arg 1: [in]	    IPC data buffer containing blob parameters: table name, column name, rowid, mode, database name.
Arg 2: [out]	IPC buffer containing the blob stream handle
*/
TInt CSqlSrvSession::DbBlobSourceL(const RMessage2& aMessage)
	{	
	__SQLPANIC_CLIENT(iDatabase != NULL, aMessage, ESqlPanicInvalidObj);
	
	TInt ipcPrmLen = aMessage.Int0();
	__SQLPANIC_CLIENT(ipcPrmLen > 0, aMessage, ESqlPanicBadArgument);
	
	iIpcStreams.AllocL();
	
	TDes8& ipcPrmDes = ReadString8ZL(aMessage, 1, ipcPrmLen);
	RDesReadStream strm(ipcPrmDes);
	
	TBuf8<KMaxFileName + 1> tblName;
	ExtractNameL(strm, tblName);
	
	TBuf8<KMaxFileName + 1> colName;
	ExtractNameL(strm, colName);
	
	TInt64 rowId;
	strm >> rowId;
	if(rowId == -1)
		{
		rowId = iDatabase->LastInsertedRowId();
		}
	if(rowId <= 0)
		{
		__SQLLEAVE(KErrArgument);
		}

	TInt32 tmp;	
	strm >> tmp;
	TBool isReadOnly = tmp != 0;
	
	TBuf8<KMaxFileName + 1> dbName;
	ExtractNameL(strm, dbName, KMainDb16);
						
	strm.Close();
			 		
	// If the database is secure then check that the client has the required capabilities for the operation
	TInt dbOpType = isReadOnly ? SQLITE_READ : SQLITE_UPDATE;
	if(CSqlSrvDatabase::AuthorizeCallback(iDatabase, dbOpType, (char*)tblName.Ptr(), (char*)colName.Ptr(), (char*)dbName.Ptr(), '\0') != SQLITE_OK)	
		{
		__SQLLEAVE(KErrPermissionDenied);	
		}

	// Create the stream buffer
	HBlobBuf* blobBuf = HBlobBuf::NewL(iDatabase->RawDbHandle(), dbName, tblName, colName, rowId, isReadOnly ? HBlobBuf::EReadOnly : HBlobBuf::EReadWrite);
	blobBuf->PushL();
		
	// Return the blob size to the client
	TPckgBuf<TIpcStreamBuf> ipcBuf;
	TInt size = blobBuf->SizeL();
	ipcBuf().iExt = size;
	
	// If this is a read stream then return the first client buffer-full of data
	TInt len = 0;
	if(isReadOnly && (size > 0))
		{
		len = Min(size, KIpcBufSize);
		blobBuf->ReadL(ipcBuf().iData, len);	
		}
	
	// Create the stream object
	HIpcStream* ipcStream = new (ELeave) HIpcStream(blobBuf, len);
	TInt strmHandle = iIpcStreams.Add(ipcStream);
	__SQLPANIC_CLIENT(strmHandle > 0, aMessage, ESqlPanicBadHandle);
	CleanupStack::Pop(blobBuf);

	// Send the size and data to the client
	aMessage.WriteL(2, ipcBuf);
	SQLPROFILER_REPORT_IPC(ESqlIpcWrite, size);

	return strmHandle;
	}
	
///////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////          Statement operations           //////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////

/**
Processes the request for preparing a 8/16-bit SQL statement.

@panic SqlDb 2 Client panic. iDatabase is NULL (the database object is not created yet).
@panic SqlDb 3 Client panic. Internal error - invalid statement handle.

Usage of the IPC call arguments:
Arg 0: [out]	Column count and parameter count
Arg 1: [in]		8/16-bit character length of SQL statement
Arg 2: [in]		SQL statement
*/
TInt CSqlSrvSession::StmtPrepareL(const RMessage2& aMessage, TBool aIsText16)
	{
	__SQLPANIC_CLIENT(iDatabase != NULL, aMessage, ESqlPanicInvalidObj);
	iStatements.AllocL();
	TInt columnCount = -1;
	TInt paramCount = -1;
	TUint len = static_cast <TUint> (aMessage.Int1());
	CSqlSrvStatement*  stmt = aIsText16 ? 
						CSqlSrvStatement::NewLC(iDatabase->RawDbHandle(), ReadString16ZL(aMessage, 2, len), columnCount, paramCount) :
						CSqlSrvStatement::NewLC(iDatabase->RawDbHandle(), ReadString8ZL(aMessage, 2, len), columnCount, paramCount);
	TPckgBuf<TSqlIpcData> data;
	data().iPrm1 = static_cast <TUint32> (columnCount);
	data().iPrm2 = static_cast <TUint32> (paramCount);
	aMessage.WriteL(0, data);
	SQLPROFILER_REPORT_IPC(ESqlIpcWrite, sizeof(TSqlIpcData));
	TInt stmtHandle = iStatements.Add(stmt);
	__SQLPANIC_CLIENT(stmtHandle > 0, aMessage, ESqlPanicBadHandle);
	CleanupStack::Pop(stmt);
	return stmtHandle;
	}

/**
Processes the request for executing the SQL statement.

@param aFunction ESqlSrvStmtExec, ESqlSrvStmtAsyncExec, ESqlSrvStmtBindExec, ESqlSrvStmtBindExecRowId, ESqlSrvStmtAsyncBindExec

Usage of the IPC call arguments:
Arg 0: [in]    parameter buffer length in bytes (if aFunction == ESqlSrvStmtBindExec || aFunction == ESqlSrvStmtAsyncBindExec)
Arg 1: [in]    parameter buffer 		(if aFunction == ESqlSrvStmtBindExec || aFunction == ESqlSrvStmtAsyncBindExec)
*/
TInt CSqlSrvSession::StmtExecL(const RMessage2& aMessage, TInt aStmtHandle, TSqlSrvFunction aFunction)
	{	
	CSqlSrvStatement& stmt = ::SqlSessObjFind(iStatements, aStmtHandle, aMessage);
	if(aFunction == ESqlSrvStmtBindExec || aFunction == ESqlSrvStmtAsyncBindExec)
		{
		DoStmtBindL(aMessage, stmt);
		}
	__SQLLEAVE_IF_ERROR(stmt.Exec());
	return iDatabase->LastChangesCount();
	}

/**
Processes the request for moving the SQL statement on the next record.

If the call does not fail, the only valid acceptable return codes should be KSqlAtRow and KSqlAtEnd.

@panic SqlDb 7 In _DEBUG mode. The call completed with no error but the return code is not KSqlAtRow or KSqlAtEnd.

Usage of the IPC call arguments:
Arg 0: [in]    parameter buffer length in bytes (if aFunction == ESqlSrvStmtBindNext)
Arg 1: [in]    parameter buffer 		(if aFunction == ESqlSrvStmtBindNext)
Arg 2: [in]    client side column buffer length in bytes
Arg 3: [out]   column buffer
*/
TInt CSqlSrvSession::StmtNextL(const RMessage2& aMessage, TInt aStmtHandle, TSqlSrvFunction aFunction)
	{
	CSqlSrvStatement& stmt = ::SqlSessObjFind(iStatements, aStmtHandle, aMessage);
	if(aFunction == ESqlSrvStmtBindNext)
		{
		DoStmtBindL(aMessage, stmt);
		}
	TInt err = stmt.Next();
	if(err == KSqlAtRow)
		{
		const RSqlBufFlat& bufFlat = stmt.ColumnValuesL();
		TInt size = bufFlat.Size();
		if(size > aMessage.Int2())
			{
			return size + KSqlClientBufOverflowCode;
			}
		aMessage.WriteL(3, bufFlat.BufDes());
		SQLPROFILER_REPORT_IPC(ESqlIpcWrite, size);
		}
	__SQLLEAVE_IF_ERROR(err);
	__ASSERT_DEBUG(err == KSqlAtRow || err == KSqlAtEnd, __SQLPANIC(ESqlPanicInternalError));
	return err;
	}

/**
Processes the request for retrieving the statement column or parameter names.

If the client side buffer size is not big enough, the function returns the size + KSqlClientBufOverflowCode.
In this case the client must increase the buffer and try again to get the buffer only.

Usage of the IPC call arguments:
Arg 0: [in]		size of the client side buffer for the names (in bytes)
Arg 1: [out]	ipc buffer, column or parameter names
*/
TInt CSqlSrvSession::StmtNamesL(const RMessage2& aMessage, TInt aStmtHandle, TSqlSrvFunction aFunction)
	{
	CSqlSrvStatement& stmt = ::SqlSessObjFind(iStatements, aStmtHandle, aMessage);
	const RSqlBufFlat& namesBuf = aFunction == ESqlSrvStmtParamNames ? stmt.ParamNamesL() : stmt.ColumnNamesL();
	TInt size = namesBuf.Size();
	if(size <= aMessage.Int0())
		{
		aMessage.WriteL(1, namesBuf.BufDes());
		SQLPROFILER_REPORT_IPC(ESqlIpcWrite, size);
		return 0;
		}
	return size + KSqlClientBufOverflowCode;
	}

/**
Processes the request for accessing a large column value as a stream of bytes/characters.

Usage of the IPC call arguments:
Arg 0: [in]		column index (0 based)
Arg 2: [out]	ipc buffer, column source
*/
TInt CSqlSrvSession::StmtColumnSourceL(const RMessage2& aMessage, TInt aStmtHandle)
	{
	CSqlSrvStatement& stmt = ::SqlSessObjFind(iStatements, aStmtHandle, aMessage);
	TInt columnIndex = aMessage.Int0();
	TPtrC8 columnSource;
	TInt err = stmt.ColumnSource(columnIndex, columnSource);
	__SQLLEAVE_IF_ERROR(err);
	HIpcReadBuf* ipcBuf = HIpcReadBuf::NewL(columnSource);
	return NewOutputStreamL(aMessage, ipcBuf);
	}

/**
Processes the request for setting a large parameter value from a stream of bytes or 8/16-bit characters.

@panic SqlDb 3 Client panic. Internal error - invalid stream handle.

Usage of the IPC call arguments:
Arg 0: [in]		parameter index (0 based)
Arg 2: [out]	ipc buffer, parameter source
*/
TInt CSqlSrvSession::StmtParamSinkL(const RMessage2& aMessage, TInt aStmtHandle, TSqlSrvFunction aFunction)
	{
	iIpcStreams.AllocL();
	TInt parameterIndex = aMessage.Int0();
	CSqlSrvStatement& stmt = ::SqlSessObjFind(iStatements, aStmtHandle, aMessage);
	HSqlSrvStmtParamBuf::TDataType dataType = aFunction == ESqlSrvStmtBinParamSink ? HSqlSrvStmtParamBuf::EBinary : HSqlSrvStmtParamBuf::EText16;
	HSqlSrvStmtParamBuf* paramBuf = stmt.GetParamBufL(parameterIndex, dataType, HSqlSrvStmtParamBuf::EBufIpcStream);
	HIpcStream* ipcStream = new (ELeave) HIpcStream(paramBuf, 0);
	TInt strmHandle = iIpcStreams.Add(ipcStream);
	__SQLPANIC_CLIENT(strmHandle > 0, aMessage, ESqlPanicBadHandle);
	return strmHandle;
	}

/**
Usage of the IPC call arguments:
Arg 0: [in]		the client side buffer length in bytes
Arg 1: [out]	refers to a place where the buffer data will be copied to.
*/
void CSqlSrvSession::StmtGetBufFlatL(const RMessage2& aMessage, TInt aStmtHandle)
	{
	CSqlSrvStatement& stmt = ::SqlSessObjFind(iStatements, aStmtHandle, aMessage);
	const RSqlBufFlat& bufFlat = stmt.BufFlatL(static_cast <TSqlBufFlatType> (aMessage.Int0()));
	aMessage.WriteL(1, bufFlat.BufDes());
	SQLPROFILER_REPORT_IPC(ESqlIpcWrite, bufFlat.Size());
	}

/**
Usage of the IPC call arguments:
Arg 0: [in]		column index
Arg 1: [in]		column buffer length in bytes
Arg 2: [in/out]	column buffer
*/
void CSqlSrvSession::StmtColumnValueL(const RMessage2& aMessage, TInt aStmtHandle)
	{
	CSqlSrvStatement& stmt = ::SqlSessObjFind(iStatements, aStmtHandle, aMessage);
	TPtrC8 columnSource;
	TInt columnIndex = aMessage.Int0();
	TInt err = stmt.ColumnSource(columnIndex, columnSource);
	__SQLLEAVE_IF_ERROR(err);
	TInt len = aMessage.Int1();
	if(columnSource.Length() > len)
		{
		TPtr8 ptr(const_cast <TUint8*> (columnSource.Ptr()), columnSource.Length(), columnSource.Length());
		ptr.SetLength(len);
		aMessage.WriteL(2, ptr);
		SQLPROFILER_REPORT_IPC(ESqlIpcWrite, len);
		__SQLLEAVE(KErrOverflow);	
		}
	else
		{
		aMessage.WriteL(2, columnSource);
		SQLPROFILER_REPORT_IPC(ESqlIpcWrite, len);
		}
	}

/**
Usage of the IPC call arguments:
Arg 0: [in]    parameter buffer length in bytes
Arg 1: [in]    parameter buffer
*/
void CSqlSrvSession::DoStmtBindL(const RMessage2& aMessage, CSqlSrvStatement& aStmt)
	{
	TInt prmLen = aMessage.Int0();
	RSqlBufFlat& prmBuf = Server().GetFlatBufL(prmLen);
	aMessage.ReadL(1, prmBuf.BufPtr());
	SQLPROFILER_REPORT_IPC(ESqlIpcRead, prmLen);
	aStmt.BindL(prmBuf);
	}

/**
Processes the request for retrieving the statement declared column type names.

If the client side buffer size is not big enough, the function returns the size + KSqlClientBufOverflowCode.
In this case the client must increase the buffer and try again to get the buffer only

Usage of the IPC call arguments:
Arg 0: [in]    	input buffer max length in 16-bit characters
Arg 1: [out]	ipc buffer, declared column type names
*/
TInt CSqlSrvSession::StmtDeclColumnTypesL(const RMessage2& aMessage, TInt aStmtHandle)
	{	
	CSqlSrvStatement& stmt = ::SqlSessObjFind(iStatements, aStmtHandle, aMessage);
	const RSqlBufFlat& declColumnTypesBuf = stmt.GetDeclColumnTypesL();
	TInt size = declColumnTypesBuf.Size();
	if(size <= aMessage.Int0())
		{
		aMessage.WriteL(1, declColumnTypesBuf.BufDes());
		return 0;
		}
	return size + KSqlClientBufOverflowCode;
	}


//////////////////////////////////////////////////////////////////////////////////////////////////////////	
////////////////////////              Helper methods                     /////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////	

/**
Creates a new output IPC stream object using the aStreamBuf parameter as a stream buffer (stream data source).

This method immediately pushes aStreamBuf onto the cleanup stack before creating a new output IPC 
stream and so callers of this method should ensure that aStreamBuf is not already on the cleanup stack.

Returns the handle of the created stream.

@panic SqlDb 3 Client panic. Internal error - invalid stream handle.

Usage of the IPC call arguments:
Arg 2: [in/out]   IPC buffer
*/
TInt CSqlSrvSession::NewOutputStreamL(const RMessage2& aMessage, MStreamBuf* aStreamBuf)
	{
	aStreamBuf->PushL();
	iIpcStreams.AllocL();
	TInt size = aStreamBuf->SizeL();
	__ASSERT_DEBUG(size >= 0, __SQLPANIC(ESqlPanicInternalError));
	TPckgBuf<TIpcStreamBuf> ipcBuf;
    // read the first buffer-full
    TInt len = Min(size, KIpcBufSize);
    aStreamBuf->ReadL(ipcBuf().iData, len);
	TInt handle = 0;
	if(size > KIpcBufSize)
		{								// create the stream object
		HIpcStream* ipcStream = new (ELeave) HIpcStream(aStreamBuf, KIpcBufSize);
		handle = iIpcStreams.Add(ipcStream);
		__SQLPANIC_CLIENT(handle > 0, aMessage, ESqlPanicBadHandle);
		CleanupStack::Pop(aStreamBuf);
		}
	else								// no more data to send
		{
		CleanupStack::PopAndDestroy(aStreamBuf);
		}
    ipcBuf().iExt = size;
    aMessage.WriteL(2, ipcBuf);
    SQLPROFILER_REPORT_IPC(ESqlIpcWrite, size);
	return handle;
	}

/**
Reads a 8-bit string with "aByteLen" bytes length, which is in "aArgNum" argument of aMessage.
The string will be zero terminated after the "read" operation.
Returns TDes8 reference pointing to the zero-terminated string.

@panic SqlDb 3 Client panic. The string length is not equal to aByteLen. If happens then it is an indication of a 
                             problem inside client side sql library.
@panic SqlDb 4 Client panic. Negative aByteLen value.
*/
TDes8& CSqlSrvSession::ReadString8ZL(const RMessage2& aMessage, TInt aArgNum, TInt aByteLen)
	{
	__SQLPANIC_CLIENT(aByteLen >= 0, aMessage, ESqlPanicBadArgument);
	TDes8& buf = Server().GetBuf8L(aByteLen + 1);
	aMessage.ReadL(aArgNum, buf);
	SQLPROFILER_REPORT_IPC(ESqlIpcRead, aByteLen);
    __SQLPANIC_CLIENT(buf.Length() == aByteLen, aMessage, ESqlPanicBadHandle);
	buf.Append(TChar(0));
	return buf;
	}

/**
Reads a 16-bit string with "aCharLen" character length, which is in "aArgNum" argument of aMessage.
The string will be zero terminated after the "read" operation.
Returns TDes16 reference pointing to the zero-terminated string.

@panic SqlDb 3 Client panic. The string length is not equal to aCharLen. If happens then it is an indication of a 
                             problem inside client side sql library.
@panic SqlDb 4 Client panic. Negative aCharLen value.
*/
TDes16& CSqlSrvSession::ReadString16ZL(const RMessage2& aMessage, TInt aArgNum, TInt aCharLen)
	{
	__SQLPANIC_CLIENT(aCharLen >= 0, aMessage, ESqlPanicBadArgument);
	TDes16& buf = Server().GetBuf16L(aCharLen + 1);
	aMessage.ReadL(aArgNum, buf);
	SQLPROFILER_REPORT_IPC(ESqlIpcRead, (aCharLen * sizeof(TText)));
    __SQLPANIC_CLIENT(buf.Length() == aCharLen, aMessage, ESqlPanicBadHandle);
	buf.Append(TChar(0));
	return buf;
	}

/**
Reads a 16-bit string with "aCharLen" character length, which is in "aArgNum" argument of aMessage.
Returns TDes16 reference pointing to the string.

@panic SqlDb 3 Client panic. The string length is not equal to aCharLen. If happens then it is an indication of a 
                             problem inside client side sql library.
@panic SqlDb 4 Client panic. Negative aCharLen value.
*/
TDes16& CSqlSrvSession::ReadString16L(const RMessage2& aMessage, TInt aArgNum, TInt aCharLen)
	{
	__SQLPANIC_CLIENT(aCharLen >= 0, aMessage, ESqlPanicBadArgument);
	TDes16& buf = Server().GetBuf16L(aCharLen);
	aMessage.ReadL(aArgNum, buf);
	SQLPROFILER_REPORT_IPC(ESqlIpcRead, (aCharLen * sizeof(TText)));
    __SQLPANIC_CLIENT(buf.Length() == aCharLen, aMessage, ESqlPanicBadHandle);
	return buf;
	}

/**
The method constructs a CSqlSecurityPolicy object from the passed as an argument descriptor.

@param aSecurityPolicyData A descriptor with the security policy data.

@return A pointer to the created CSqlSecurityPolicy instance.

@leave KErrNoMemory, out of memory condition has occured.
*/
CSqlSecurityPolicy* CSqlSrvSession::CreateSecurityPolicyL(const TDesC8& aSecurityPolicyData)
	{
	TSecurityPolicy defaultPolicy(TSecurityPolicy::EAlwaysFail);
	CSqlSecurityPolicy* dbPolicy = CSqlSecurityPolicy::NewLC(defaultPolicy);
	RSqlBufFlat& bufFlat = dbPolicy->BufFlat();
    __SQLLEAVE_IF_ERROR(bufFlat.ReAlloc(aSecurityPolicyData.Length()));
	bufFlat.BufPtr().Copy(aSecurityPolicyData);
	CleanupStack::Pop(dbPolicy);
	return dbPolicy;
	}

/**
Reports how many objects are allocated by the client.
If the database connection is not in a test mode, the allocated memory cells count will be ignored.
*/
TInt CSqlSrvSession::CountResources()
	{
	return iStatements.Count() + iIpcStreams.Count() + (iDbResourceTestMode ? User::CountAllocCells() : 0);
	}

/**
Extracts from aMessage:
- function code;
- stream or statement handle;
The function will panic the client if aMessage contains bad function code or bad handle encoded in it.
*/
void CSqlSrvSession::Extract(const RMessage2& aMessage, TSqlSrvFunction& aFunction, TInt& aHandle)
	{
	TInt msgCode = aMessage.Function();
	aFunction = static_cast <TSqlSrvFunction> (KSqlSrvFunctionMask & msgCode);
	//All operations with code > ESqlSrvDbDelete require valid iDatabase object.
	if(aFunction > ESqlSrvDbDelete)
		{
		__SQLPANIC_CLIENT(iDatabase != NULL, aMessage, ESqlPanicInvalidObj);
		}
	if(aFunction >= ESqlSrvStmtClose)
		{
		//Extracting handle and handle type from the message code
		TSqlSrvHandleType handleType = static_cast <TSqlSrvHandleType> (msgCode & KSqlSrvHandleTypeMask);
		aHandle = (msgCode & KSqlSrvHandleMask) >> KSqlSrvHandleShiftBits;
		__SQLPANIC_CLIENT(aHandle > 0, aMessage, ESqlPanicBadArgument);
		if(aFunction < ESqlSrvStreamBase)
			{
			__SQLPANIC_CLIENT(handleType == ESqlSrvStatementHandle, aMessage, ESqlPanicBadArgument);
			}
		else
			{
			__SQLPANIC_CLIENT(handleType == ESqlSrvStreamHandle, aMessage, ESqlPanicBadArgument);
			}
		}
	}

/**
The function reads aStmt column 0 value and copies it into the client buffer, accessed via aMessage argument.

If an error occurs during the execution the function leaves with the error code.
Possible non-leaving return values:
 - KErrNone              - the function has completed successfully;
 - Positive return value - the length of the column, which means - the destination buffer is too small.
                           This return value is possible only with text or binary columns.

Usage of the IPC call arguments: 
Arg 0: [in]		(8/16-bit character length of SQL statement) | (expected column value type << 24).
Arg 1: [in]		SQL statement.
Arg 2: [in]		Byte max length of the receiving buffer
Arg 3: [in/out]	The receiving buffer
*/
TInt CSqlSrvSession::GetColumnValueL(const RMessage2& aMessage, CSqlSrvStatement& aStmt, TSqlColumnType aColType)
	{
	TInt rc = KErrNone;
	switch(aColType)
		{
		case ESqlInt:
			{
			TInt val = aStmt.ColumnInt(0);
			aMessage.WriteL(3, TPtrC8(reinterpret_cast <const TUint8*> (&val), sizeof(val)));
			SQLPROFILER_REPORT_IPC(ESqlIpcWrite, sizeof(val));
			}
			break;
		case ESqlInt64:
			{
			TInt64 val = aStmt.ColumnInt64(0);
			aMessage.WriteL(3, TPtrC8(reinterpret_cast <const TUint8*> (&val), sizeof(val)));
			SQLPROFILER_REPORT_IPC(ESqlIpcWrite, sizeof(val));
			}
			break;
		case ESqlReal:
			{
			TReal val = aStmt.ColumnReal(0);
			aMessage.WriteL(3, TPtrC8(reinterpret_cast <const TUint8*> (&val), sizeof(val)));
			SQLPROFILER_REPORT_IPC(ESqlIpcWrite, sizeof(val));
			}
			break;
		case ESqlText:
		case ESqlBinary:
		default:
			{
			TPtrC8 val;
			if(aColType == ESqlText)
				{
				TPtrC textVal = aStmt.ColumnTextL(0);
				val.Set(reinterpret_cast <const TUint8*> (textVal.Ptr()), textVal.Length() * sizeof(TUint16));
				}
			else
				{
				val.Set(aStmt.ColumnBinary(0));
				}
			TInt len = val.Length();
			if(len > aMessage.Int2())
				{
				rc = aColType == ESqlText ? (TUint)len / sizeof(TUint16) : len;
				len = aMessage.Int2();
				}
			aMessage.WriteL(3, val.Left(len));
			SQLPROFILER_REPORT_IPC(ESqlIpcWrite, len);
			}
			break;
		}
	return rc;
	}
