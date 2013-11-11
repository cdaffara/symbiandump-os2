// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __SQLDBSESSION_H__
#define __SQLDBSESSION_H__

#include <sqldb.h>			//RSqlDatabase::TIsolationLevel
#include "SqlUtil.h"		//TSqlSrvFunction

//Forward declarations
struct sqlite3;
class RSqlStatementSession;
class TSqlResourceTestData;
class CSqlSecurityPolicy;
class RSqlBufFlat;
class TSqlFhCmdFunctor;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////                RSqlDbSession           /////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
RSqlDbSession class is used for establishing a connection with the database server.
It can be used also for: 
 - executing SQL statements or a set of SQL statements, separated with ";";
 - attaching/detaching databases;
 - copying databases;
 - deleting databases;
 - retrieving last error messages;
 
@internalComponent
*/
NONSHARABLE_CLASS(RSqlDbSession) : public RSessionBase
	{
	//RSqlDbSession class hides all details about the communication with the database server.
	//No database specific header files should be seen or used outside the session class.
	
	friend class RSqlStatementSession;
	friend class TSqlResourceTestData;
	
public:	
	inline RSqlDbSession();
	TInt Connect(TSqlSrvFunction aFunction, const TDesC& aDbFileName, const TDesC8& aSecurityPolicyData, const TDesC8* aConfig);
	TInt Attach(const TDesC& aDbFileName, const TDesC& aDbName);
	inline TInt Detach(const TDesC& aDbName);
	static TInt CopyDatabase(const TDesC& aSrcDbFileName, const TDesC& aDestDbFileName);
	static TInt DeleteDatabase(const TDesC& aDbFileName);
	TPtrC LastErrorMessage();
	TInt64 LastInsertedRowId(); 
	inline TInt Exec(const TDesC& aSqlStmt);
	inline TInt Exec(const TDesC8& aSqlStmt);
	inline void Exec(const TDesC& aSqlStmt, TRequestStatus& aStatus);
	inline void Exec(const TDesC8& aSqlStmt, TRequestStatus& aStatus);
	inline TInt SetIsolationLevel(RSqlDatabase::TIsolationLevel aIsolationLevel);
	TInt GetSecurityPolicy(RSqlBufFlat& aSecurityPolicyBuf);
	inline TInt ExecScalarFullSelect(const TDesC& aSqlStmt, TSqlColumnType aType, TDes8& aRes);
	inline TInt ExecScalarFullSelect(const TDesC8& aSqlStmt, TSqlColumnType aType, TDes8& aRes);
	void Close();

	inline TInt SendReceive(TInt aFunction);
	inline void SendReceive(TInt aFunction, TRequestStatus& aStatus);
	inline TInt SendReceive(TInt aFunction,const TIpcArgs& aArgs);
	inline void SendReceive(TInt aFunction,const TIpcArgs& aArgs, TRequestStatus& aStatus);

private:
	enum TDbfAction {EDbfCreate, EDbfOpen};
	TInt DoCreateSession();
	TInt InitResourceTestData();
	TInt CreateAndSendFileHandle(TSqlFhCmdFunctor& aFhCmdSender, TDbfAction aDbfAction);
	TInt DoConnect(TSqlSrvFunction aFunction, const TDesC& aDbFileName, const TDesC8& aSecurityPolicyData, const TDesC8* aConfig);
	
private:
	HBufC*	iLastErrorMessage;

	};

#include "SqlDbSession.inl"

#endif //__SQLDBSESSION_H__
