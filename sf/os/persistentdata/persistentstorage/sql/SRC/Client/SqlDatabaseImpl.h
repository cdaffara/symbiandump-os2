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

#ifndef __SQLDATABASEIMPL_H__
#define __SQLDATABASEIMPL_H__

#include <sqldb.h>				//RSqlDatabase::TIsolationLevel, TSqlColumnType
#include "SqlDbSession.h"		//RSqlDbSession, TSqlSrvFunction (via included SqlUtil.h)

//Forward declarations
class CSqlSecurityPolicy;

/**
CSqlDatabaseImpl implements RSqlDatabase.

It can be used for:
 - establishing a connection with the database;
 - attaching/detaching a database to/from the existing connection;
 - executing SQL statements or a set of SQL statements, separated with ";";
 - deleting databases;
 - retrieving last error messages;
 - setting the isolation level;
 - returning the database security policy;

@see RSqlDatabase

@internalComponent
*/
NONSHARABLE_CLASS(CSqlDatabaseImpl) : public CBase
	{
public:
	static CSqlDatabaseImpl* NewL(TSqlSrvFunction aFunction, const TDesC& aDbFileName, 
								  const CSqlSecurityPolicy* aSecurityPolicy, const TDesC8* aConfig = NULL);
	virtual ~CSqlDatabaseImpl();
	inline TInt Attach(const TDesC& aDbFileName, const TDesC& aDbName);
	inline TInt Detach(const TDesC& aDbName);
	static inline TInt Copy(const TDesC& aSrcDbFileName, const TDesC& aDestDbFileName);
	static inline TInt Delete(const TDesC& aDbFileName);
	CSqlSecurityPolicy* CloneSecurityPolicyL();
	inline TInt SetIsolationLevel(RSqlDatabase::TIsolationLevel aIsolationLevel);
	inline TInt Exec(const TDesC16& aSqlStmt);
	inline TInt Exec(const TDesC8& aSqlStmt);
	inline void Exec(const TDesC16& aSqlStmt, TRequestStatus& aStatus);
	inline void Exec(const TDesC8& aSqlStmt, TRequestStatus& aStatus);
	inline TPtrC LastErrorMessage();
	inline TInt64 LastInsertedRowId(); 
	inline RSqlDbSession& Session();
	inline TInt ExecScalarFullSelectL(const TDesC16& aSqlStmt, TSqlColumnType aType, TDes8& aRes);
	inline TInt ExecScalarFullSelectL(const TDesC8& aSqlStmt, TSqlColumnType aType, TDes8& aRes);
	inline TBool InTransaction();
	inline TInt Size();
	inline TInt Size(RSqlDatabase::TSize& aSize, const TDesC& aDbName);
	TInt Compact(TInt aSize, const TDesC& aDbName);
	void Compact(TInt aSize, const TDesC& aDbName, TRequestStatus& aStatus);
		
private:
	inline CSqlDatabaseImpl();
	void ConstructL(TSqlSrvFunction aFunction, const TDesC& aDbFileName, 
					const CSqlSecurityPolicy* aSecurityPolicy = NULL, const TDesC8* aConfig = NULL);

private:
	RSqlDbSession 		iDbSession;
	
	};

#include "SqlDatabaseImpl.inl"

#endif //__SQLDATABASEIMPL_H__
