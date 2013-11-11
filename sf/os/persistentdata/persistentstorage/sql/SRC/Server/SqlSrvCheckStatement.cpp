// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32std.h>
#include "SqlSrvStrings.h"

_LIT(KCreateSql, "CREATE");
_LIT(KIndexSql,  "INDEX");
_LIT(KIfSql,     "IF");
_LIT(KNotSql,    "NOT");
_LIT(KExistsSql, "EXISTS");
_LIT(KDotSql,    ".");


/**
Determines whether the specified string is an SQL statement that is supported as a config operation.
Currently only 'CREATE INDEX' SQL statements are supported. If necessary adds qualifyed DB name  to index name.

@param aStatementIn The string to be checked to determine if it is a supported SQL statement
@param aDbName Logical database name: "main" for the main database or attached database name
@param aStatementOut Output parameter. Buffer for the constructed statement (if the statement is supported).

@return True if the aStatementIn statement is supported, aStatementOut will contain the zero-terminated
		aStatementIn statement with the database name in it, if aDbName is a name of an attached database
*/	
TBool IsStatementSupported(const TDesC& aStatementIn, const TDesC& aDbName, TDes& aStatementOut)
	{	
	//'CREATE INDEX' is the only statement currently supported.
	//We must allow for whitespace between the 'CREATE' and the 'INDEX'
	TLex stmtParser(aStatementIn);
	TPtrC firstToken = stmtParser.NextToken(); // extract the first token of the statement
	if(firstToken.CompareF(KCreateSql()) != 0)
		{
		return EFalse;	
		}
	//The first token is 'CREATE', now skip any whitespace between 
	//it and the next token and ensure that the next token is 'INDEX'
	TPtrC secondToken = stmtParser.NextToken(); // skip any whitespace and extract the next token
	if(secondToken.CompareF(KIndexSql()) != 0)
		{
		return EFalse;	
		}
	//The second token is 'INDEX'
	TPtrC curToken = stmtParser.NextToken();
	// skip optional [IF NOT EXISTS]
	if(curToken.CompareF(KIfSql()) == 0)
		{
		curToken.Set(stmtParser.NextToken());
		if(curToken.CompareF(KNotSql()) != 0)
			{
			return EFalse;	
			}
		curToken.Set(stmtParser.NextToken());
		if(curToken.CompareF(KExistsSql()) != 0)
			{
			return EFalse;	
			}
		curToken.Set(stmtParser.NextToken());
		}
	// we got '[database-name.]index-name'
	if(curToken.Find(KDotSql)==KErrNotFound && aDbName.CompareF(KMainDb16) != 0) // just index-name
		{
		// Add DB name prefix before table name in the case of CREATE INDEX
		aStatementOut.Copy(aStatementIn.Ptr(), curToken.Ptr()-aStatementIn.Ptr());
		aStatementOut.Append(aDbName);
		aStatementOut.Append(KDotSql);
		aStatementOut.Append(curToken);
		aStatementOut.Append(stmtParser.Remainder());
		}
	else
		{
		aStatementOut.Copy(aStatementIn);
		}
	aStatementOut.Append(TChar(0)); // SQLite requires statements to be zero-terminated
	return ETrue;
	}
