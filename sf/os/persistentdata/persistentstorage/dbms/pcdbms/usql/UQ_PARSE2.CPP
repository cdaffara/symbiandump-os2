// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// TSqlParser2 class. "DBMS Security" related - full support.
// 
//

#include "UQ_STD.H"

/**
This method is used to extract the table name from a particular sql
string - aTableName parameter. Also, the function determines the sql string type -
Sql::EDDL, Sql::EDML or Sql::ENone (SELECT sql string).
*/
void TSqlParser2::ParseL(const TDesC& aSql)
	{
	TSqlParser sqlParser(aSql);
	iStatementType = sqlParser.Type();
	iTableName.Set(NULL, 0);
	switch(iStatementType)
		{
		case Sql::ENone: 
			//It may be a "SELECT" statement. Check the sql and get the table name.
			if(sqlParser.ParseL(ESqlKeyword_select) == ESqlAsterisk)
				{
				sqlParser.NextToken();
				}
			else
				{
				for(;;)
					{
					TSqlTokenType t = sqlParser.NextToken();
					if(t != ESqlComma)
						break;
					sqlParser.NextToken();
					}
				}
			sqlParser.ParseL(ESqlKeyword_from);
			sqlParser.IdentifierL(iTableName);
			break;
		case Sql::EDDL:
			//CREATE/DROP/ALTER statement. There is a table name, but we do not need it.
			//The caller has to have "SCHEMA" access level, so we do not bother with the table name.
			break;
		case Sql::EDML:
			//INSERT/UPDATE/DELETE statement. Get the table name.
			if(sqlParser.Parse(ESqlKeyword_insert))
				{
				sqlParser.ParseL(ESqlKeyword_into);
				}
			else if(sqlParser.Parse(ESqlKeyword_update))
				{
				}
			else 
				{
				sqlParser.ParseL(ESqlKeyword_delete);
				sqlParser.ParseL(ESqlKeyword_from);
				}
			sqlParser.IdentifierL(iTableName);
			break;
		default:
			__ASSERT(EFalse);
			break;
		}
	}
