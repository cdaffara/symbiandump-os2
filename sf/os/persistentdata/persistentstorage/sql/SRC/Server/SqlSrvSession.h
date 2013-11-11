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

#ifndef __SQLSRVSESSION_H__
#define __SQLSRVSESSION_H__

#include <e32base.h>				//CSession2
#include "IPCStream.h"				//HIpcStream
#include "SqlUtil.h"				//TSqlSrvFunction
#include "SqlSrvObjContainer.h"		//RDbObjContainer
#include "SqlSrvDatabase.h"			//CSqlSrvDatabase
#include "SqlSrvResourceProfiler.h"

//Forward declarations
class CSqlServer;
class CSqlSrvStatement;
class MStreamBuf;
class RMessage2;
class CSqlSecurityPolicy;

/**
SQL server side session class.

All client side requests come through a CSqlSrvSession instance which unpacks the client data,
performs the requested operation invoking the appropriate server side objects and functions and
packs and returns back to the client the results from the performed operation.

CSqlSrvSession class owns the following set of data:
- A CSqlSrvDatabase instance - one per session;
- A RDbObjContainer instance - container for statement handles (CSqlSrvStatement class);
- A RDbObjContainer instance - container for IPC stream hadles (large data block transfer operations)
                               (HIpcStream class);
                  
CSqlSrvSession instances shall never be created via a direct call of CSqlSrvSession::NewL().
It is always the server, which creates a new session instance as a responce to the creation of a client side
session instance (CSqlServer::NewSessionL() function).

@see CSqlSrvDatabase
@see CSqlSrvStatement
@see RDbObjContainer
@see HIpcStream
@see CSqlServer
@see CSqlServer::NewSessionL()
@see CSession2

@internalComponent
*/
NONSHARABLE_CLASS(CSqlSrvSession) : public CSession2
	{
public:
	static CSqlSrvSession* NewL();
	virtual ~CSqlSrvSession();
	inline CSqlServer& Server();
	
private:	
	inline CSqlSrvSession();
	inline void ConstructL();
	//CSession2::ServiceL() implementation and overriden CSession2::ServiceError().
	virtual void ServiceL(const RMessage2& aMessage);
	virtual void ServiceError(const RMessage2& aMessage, TInt aError);
	//Profiler operations
	void ProfilerQueryL(const RMessage2& aMessage);
	//Database operations
	void DbCreateObjectL(const RMessage2& aMessage, TSqlSrvFunction aFunction);
	void DbCreateObjectFromHandleL(const RMessage2& aMessage);
	void DbAttachL(const RMessage2& aMessage);
	void DbAttachFromHandleL(const RMessage2& aMessage);
	void DbDetachL(const RMessage2& aMessage);
	inline void DbDestroyObject();
	void DbCopyFileL(const RMessage2& aMessage);
	void DbDeleteFileL(const RMessage2& aMessage);
	TInt DbLastErrorMessageL(const RMessage2& aMessage);
	void DbLastInsertedRowIdL(const RMessage2& aMessage);
	inline TInt DbExecSql8L(const RMessage2& aMessage);
	inline TInt DbExecSql16L(const RMessage2& aMessage);
	inline void DbSetIsolationLevelL(const RMessage2& aMessage);
	TInt DbGetSecurityPolicyL(const RMessage2& aMessage);
	TInt DbScalarFullSelectL(const RMessage2& aMessage, TBool aIsText16);
	TInt DbBlobSourceL(const RMessage2& aMessage);
	TBool DbInTransaction(const RMessage2& aMessage);
	TInt DbSizeL(const RMessage2& aMessage);
	void DbSize2L(const RMessage2& aMessage);
	TInt DbCompactL(const RMessage2& aMessage);
	void DbReserveDriveSpaceL();
	void DbFreeReservedSpace();
	void DbGetReserveAccessL();
	void DbReleaseReserveAccess();
	//Statement operations
	TInt StmtPrepareL(const RMessage2& aMessage, TBool aIsText16);
	TInt StmtExecL(const RMessage2& aMessage, TInt aStmtHandle, TSqlSrvFunction aFunction);
	TInt StmtNextL(const RMessage2& aMessage, TInt aStmtHandle, TSqlSrvFunction aFunction);
	TInt StmtNamesL(const RMessage2& aMessage, TInt aStmtHandle, TSqlSrvFunction aFunction);
	TInt StmtColumnSourceL(const RMessage2& aMessage, TInt aStmtHandle);
	TInt StmtParamSinkL(const RMessage2& aMessage, TInt aStmtHandle, TSqlSrvFunction aFunction);
	void StmtGetBufFlatL(const RMessage2& aMessage, TInt aStmtHandle);
	void StmtColumnValueL(const RMessage2& aMessage, TInt aStmtHandle);
	void DoStmtBindL(const RMessage2& aMessage, CSqlSrvStatement& aStmt);
	TInt StmtDeclColumnTypesL(const RMessage2& aMessage, TInt aStmtHandle);
	//Helper methods
	TInt NewOutputStreamL(const RMessage2& aMessage, MStreamBuf* aStreamBuf);
	TDes8& ReadString8ZL(const RMessage2& aMessage, TInt aArgNum, TInt aByteLen);
	TDes16& ReadString16ZL(const RMessage2& aMessage, TInt aArgNum, TInt aCharLen);
	TDes16& ReadString16L(const RMessage2& aMessage, TInt aArgNum, TInt aCharLen);
	CSqlSecurityPolicy* CreateSecurityPolicyL(const TDesC8& aSecurotyPolicyData);
	virtual TInt CountResources();
	void Extract(const RMessage2& aMessage, TSqlSrvFunction& aFunction, TInt& aHandle);
	TInt GetColumnValueL(const RMessage2& aMessage, CSqlSrvStatement& aStmt, TSqlColumnType aColType);
	void ExtractNameL(RDesReadStream& aStrm, TDes8& aNameOut, const TDesC& aEmptyNameSubstitute = KNullDesC);
	//Resource allocation testing related
	inline TBool ActivateDbTestMode(TInt aHeapFailureMode, TInt aFailedAllocNumber);
	inline void StopDbTestMode();
	inline void DbResourceMark();
	inline void DbResourceEnd(const RMessage2& aMessage);
	inline void DbSetAllocFail(TInt aHeapFailureMode, TInt aFailedAllocNumber);
	inline void DbSetDelayedAllocFail();
private:
	CSqlSrvDatabase*					iDatabase;				//The database object (one per session)
	RDbObjContainer<CSqlSrvStatement>	iStatements;			//Container for SQL statement handles
	RDbObjContainer<HIpcStream>			iIpcStreams;			//Container for IPC stream hansdles
	TInt								iDbResourceTestMode;	//Non-zero if this session is in test mode (memory leaks simulation/detection)
	TInt								iFailedAllocNumber;		//The failing allocation number - If iDbResourceTestMode is OR-ed with 0x1000
	TBool								iDriveSpaceReserved;	//True if this session reserved drive space
	TBool								iDriveSpaceInUse;		//True if the client has been given an access to the reserved drive space
	TDriveNumber 						iDrive;					//the drive number, where the database file is
	TUint                               iIpcCallCounter;
#ifdef _SQLPROFILER
    TSqlSrvIpcTraceData                 iIpcTraceData[KIpcTraceTypeCount];
#endif
	};

#include "SqlSrvSession.inl"

#endif//__SQLSRVSESSION_H__
