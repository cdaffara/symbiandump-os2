// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "UQ_STD.H"

// Class Sql

CSqlQuery* Sql::ParseQueryLC(const TDesC& aSql)
	{
	TSqlParser parser(aSql);
	return parser.QueryLC();
	}

CSqlSearchCondition* Sql::ParseSearchConditionLC(const TDesC& aSql)
	{
	TSqlParser parser(aSql);
	return parser.SearchConditionLC();
	}

CSqlDDLStatement* Sql::ParseDDLStatementLC(const TDesC& aSql)
	{
	TSqlParser parser(aSql);
	return parser.DDLStatementLC();
	}

CSqlDMLStatement* Sql::ParseDMLStatementLC(const TDesC& aSql)
	{
	TSqlParser parser(aSql);
	return parser.DMLStatementLC();
	}

Sql::TStatementType Sql::Type(const TDesC& aSql)
	{
	TSqlParser parser(aSql);
	return parser.Type();
	}
