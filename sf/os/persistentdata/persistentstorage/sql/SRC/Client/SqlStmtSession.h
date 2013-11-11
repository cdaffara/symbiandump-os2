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

#ifndef __SQLSTMTSESSION_H__
#define __SQLSTMTSESSION_H__

#include <e32std.h>
#include "SqlDbSession.h"		//RSqlDbSession
#include "SqlBufFlat.h"			//RSqlBufFlat
#include "IPCBuf.h"				//HIpcBuf

/**
RSqlStatementSession class is used for "prepare once execute many times" supplied to PrepareL() SQL 
statement.
RSqlStatementSession::PrepareL() must be called for preparing the SQL statement.
If the SQL statement contains parameters, Bind() must be called before calling Next() or Exec() 
to bind the parameter values.
RSqlStatementSession class shall not be used for executing a set of SQL statements, separated with ";".
PrepareL() will report this as a KErrArgument error.

RSqlStatementSession class hides all details about the communication with the database server.
No database specific header files should be seen or used outside the session class.

Usage patterns:

1) SQL statement which does not return a record set.

@code
	RSqlStatementSession sess;
	TInt err = sess.Prepare(...);
	...
	RSqlBufFlat paramBuf;
	<Initialize "paramBuf" object>;
begin:
	err = sess.Bind(paramBuf);
	err = sess.Exec();
	err = sess.Reset();
	<Now a new set of parameter values can be prepared and the statement can be executed again - from the "begin" label>;
	
	sess.Close();
@endcode

2) SQL statement which returns a record set.

@code
	RSqlStatementSession sess;
	TInt err = sess.Prepare(...);
	...
	RSqlBufFlat paramBuf;
	<Initialize "paramBuf" object>;
	RSqlBufFlat columnBuf;
	<Initialize "columnBuf" object. The element count must match the columns count>;
begin:
	err = sess.Bind(paramBuf);
	while(sess.Next(columnBuf) == KSqlAtRow)
		{
		<process the record data - "columnBuf" set>;
		}
	err = sess.Reset();
	<Now a new set of parameter values can be prepared and the statement can be executed again - from the "begin" label>;
	
	sess.Close();
@endcode

@internalComponent
*/
NONSHARABLE_CLASS(RSqlStatementSession)
	{
public:	
	inline RSqlStatementSession();
	TInt Prepare(RSqlDbSession& aDbSession, const TDesC& aSqlStmt, TInt& aColumnCount, TInt& aParamCount);
	TInt Prepare(RSqlDbSession& aDbSession, const TDesC8& aSqlStmt, TInt& aColumnCount, TInt& aParamCount);
	void Close();
	
	inline TInt Reset();
	inline TInt Exec();
	inline void Exec(TRequestStatus& aStatus);
	inline TInt BindExec(const RSqlBufFlat& aParamBuf);
	inline void BindExec(const RSqlBufFlat& aParamBuf, TRequestStatus& aStatus);
	inline TInt Next(RSqlBufFlat& aColumnBuf);
	TInt BindNext(const RSqlBufFlat& aParamBuf, RSqlBufFlat& aColumnBuf);
	TInt GetNames(TSqlSrvFunction aFunction, RSqlBufFlat& aNameBuf);
	inline TInt ReadColumnValue(TInt aColumnIndex, TDes8& aBuf);
	
	inline MStreamBuf* ColumnSourceL(TInt aColumnIndex);
	inline MStreamBuf* ParamSinkL(TSqlSrvFunction aFunction, TInt aParamIndex);

	TInt GetDeclColumnTypes(RSqlBufFlat& aDeclColumnTypeBuf);	
	
private:
	TInt DoBindNext(TSqlSrvFunction aFunction, TIpcArgs& aIpcArgs, RSqlBufFlat& aColumnBuf);
	TInt Retry(RSqlBufFlat& aBufFlat, TInt aSize, TSqlBufFlatType aWhat);
	inline RSqlDbSession& DbSession() const;

private:
	TInt			iHandle;
	RSqlDbSession*	iDbSession;

	};

#include "SqlStmtSession.inl"

#endif//__SQLSTMTSESSION_H__
