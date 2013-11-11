// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
*/
inline void CSqlSrvSession::ConstructL()
	{
	}

/**
*/
inline CSqlSrvSession::CSqlSrvSession()
	{
	}

/**
@return A reference to CSqlServer instance.
*/
inline CSqlServer& CSqlSrvSession::Server()
	{
	return *static_cast<CSqlServer*>(const_cast<CServer2*>(CSession2::Server()));
	}
	
/**
Destroys CSqlSrvDatabase instance.
*/
inline void CSqlSrvSession::DbDestroyObject()
	{
	delete iDatabase;
	iDatabase = NULL;
	}

/**
Processes request for executing 8-bit SQL statement(s).

The method requires a valid iDatabase object.

Usage of the IPC call arguments:
Arg 0: [in]	8-bit character length of SQL statement.
Arg 1: [in]	SQL statement.

@panic SqlDb 2 Client panic. iDatabase is NULL (the database object is not created yet).
*/
inline TInt CSqlSrvSession::DbExecSql8L(const RMessage2& aMessage)
	{
	__SQLPANIC_CLIENT(iDatabase != NULL, aMessage, ESqlPanicInvalidObj);
	iDatabase->ExecL(ReadString8ZL(aMessage, 1, aMessage.Int0()));
	return iDatabase->LastChangesCount();
	}

/**
Processes request for executing 16-bit SQL statement(s).

The method requires a valid iDatabase object.

Usage of the IPC call arguments:
Arg 0: [in]	16-bit character length of SQL statement.
Arg 1: [in]	SQL statement.

@panic SqlDb 2 Client panic. iDatabase is NULL (the database object is not created yet).
*/
inline TInt CSqlSrvSession::DbExecSql16L(const RMessage2& aMessage)
	{
	__SQLPANIC_CLIENT(iDatabase != NULL, aMessage, ESqlPanicInvalidObj);
	iDatabase->ExecL(ReadString16ZL(aMessage, 1, aMessage.Int0()));
	return iDatabase->LastChangesCount();
	}

/**
Processes the request for setting the database isolation level.

The method requires a valid iDatabase object.

Usage of the IPC call arguments:
Arg 0: [in]	Database isolation level.

@panic SqlDb 2 Client panic. iDatabase is NULL (the database object is not created yet).
*/
inline void CSqlSrvSession::DbSetIsolationLevelL(const RMessage2& aMessage)
	{
	__SQLPANIC_CLIENT(iDatabase != NULL, aMessage, ESqlPanicInvalidObj);
	RSqlDatabase::TIsolationLevel level = static_cast <RSqlDatabase::TIsolationLevel> (aMessage.Int0());
	iDatabase->SetIsolationLevelL(level);
	}
