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
// SQL parser
// 
//

#include "UQ_STD.H"
#include "D32Assert.h"

// class TSqlParser

const TInt KSqlError = -1;

TSqlParser::TSqlParser(const TDesC& aSql)
	: iSql(aSql)
	{
	NextToken();		// parse the first token
	}

TSqlTokenType TSqlParser::NextToken()
	{
	return iSql.NextToken(iToken);
	}

CSqlSearchCondition* TSqlParser::SqlError()
	{
	if (!Error())
		iToken.SetError(KErrArgument);
	return 0;
	}

TSqlTokenType TSqlParser::SqlErrorL()
//
// Report a SQL syntax error
//
	{
	return TSqlTokenType(__LEAVE_IF_ERROR(KErrArgument));
	}

TSqlTokenType TSqlParser::Parse(TSqlKeyword aKeyword)
//
// look for requested keyword, skip to the next token if found
// return the next token if found, else ESqlNoToken (==0)
//
	{
	return TSqlLexer::IsKeyword(aKeyword,iToken) ? NextToken() : ESqlNoToken;
	}

TSqlTokenType TSqlParser::ParseL(TSqlTokenType aToken)
//
// parse the desired token
//
	{
	return iToken!=aToken ? SqlErrorL() : NextToken();
	}

TSqlTokenType TSqlParser::ParseL(TSqlKeyword aKeyword)
//
// parse the desired keyword
//
	{
	TSqlTokenType t=Parse(aKeyword);
	return t==ESqlNoToken ? SqlErrorL() : t;
	}

TSqlKeyword TSqlParser::Keyword()
//
// parse a keyword
//
	{
	TSqlKeyword k=TSqlLexer::Keyword(iToken);
	if (k!=ESqlNotKeyword)
		NextToken();
	return k;
	}

TSqlTokenType TSqlParser::RightBracketL()
//
// parse a right bracket, and fail if not found
//
	{
	return ParseL(ESqlRightBracket);
	}

void TSqlParser::EndL()
//
// Check that the SQL has been fully parsed
//
	{
	if (iToken!=ESqlEos)
		SqlErrorL();
	}

TSqlTokenType TSqlParser::IdentifierL(TPtrC& aIdentifier)
//
// parse an identifer, fail if not found
//
	{
	if (iToken!=ESqlIdentifier)
		return SqlErrorL();
	const TText* p=iToken.Literal().Ptr();
	TInt len=iToken.Literal().End()-p;
	aIdentifier.Set(p,len);
	return NextToken();
	}

TPtrC TSqlParser::IdentifierL()
//
// parse an identifer, fail if not found
//
	{
	if (iToken!=ESqlIdentifier)
		SqlErrorL();
	return iToken.Literal().DesC();
	}

TSqlTokenType TSqlParser::ColumnNameL(RSqlColumnList& aList)
//
// Parse a column-identifier
//
	{
	__LEAVE_IF_ERROR(aList.Append(IdentifierL()));
	return NextToken();
	}

TSqlTokenType TSqlParser::ColumnListL(RSqlColumnList& aList)
//
// Parse a column-identifier-comma-list
//
	{
	for (;;)
		{
		TSqlTokenType t=ColumnNameL(aList);
		if (t!=ESqlComma)
			return t;
		NextToken();
		}
	}

TSqlTokenType TSqlParser::AddColumnSpecL(TDbCol& aDef)
//
// Parse an add-column-spec
//
	{
	aDef.iAttributes=0;
	aDef.iName=IdentifierL();
	NextToken();
//
	TDbColType type;
	switch (Keyword())
		{
	case ESqlKeyword_bit:
		type=EDbColBit;
		break;
	case ESqlKeyword_tinyint:
		type=EDbColInt8;
		break;
	case ESqlKeyword_smallint:
		type=EDbColInt16;
		break;
	case ESqlKeyword_integer:
		type=EDbColInt32;
		break;
	case ESqlKeyword_counter:
		aDef.iAttributes=aDef.EAutoIncrement;
		type=EDbColUint32;
		break;
	case ESqlKeyword_bigint:
		type=EDbColInt64;
		break;
	case ESqlKeyword_real:
		type=EDbColReal32;
		break;
	case ESqlKeyword_double:
		Parse(ESqlKeyword_precision);
		// fall through
	case ESqlKeyword_float:
		type=EDbColReal64;
		break;
	case ESqlKeyword_date:
	case ESqlKeyword_time:
	case ESqlKeyword_timestamp:
		type=EDbColDateTime;
		break;
	case ESqlKeyword_char:
	case ESqlKeyword_varchar:
		type=EDbColText;
		break;
	case ESqlKeyword_char8:
	case ESqlKeyword_varchar8:
		type=EDbColText8;
		break;
	case ESqlKeyword_binary:
	case ESqlKeyword_varbinary:
		type=EDbColBinary;
		break;
	case ESqlKeyword_unsigned:	// unsigned tinyint, smallint or integer
		switch (Keyword())
			{
		case ESqlKeyword_tinyint:
			type=EDbColUint8;
			break;
		case ESqlKeyword_smallint:
			type=EDbColUint16;
			break;
		case ESqlKeyword_integer:
			type=EDbColUint32;
			break;
		default:
			return SqlErrorL();
			};
		break;
	case ESqlKeyword_long:		// varchar or varbinary
		switch (Keyword())
			{
		case ESqlKeyword_varchar8:
			type=EDbColLongText8;
			break;
		case ESqlKeyword_varchar:
			type=EDbColLongText;
			break;
		case ESqlKeyword_varbinary:
			type=EDbColLongBinary;
			break;
		default:
			return SqlErrorL();
			};
		break;
	default:
		return SqlErrorL();
		}
	aDef.iType=type;
//
// get any optional length
	aDef.iMaxLength=KDbUndefinedLength;
	TSqlTokenType t=iToken.Type();
	switch (type)
		{
	case EDbColText:
	case EDbColBinary:
		if (t==ESqlLeftBracket)
			{
			if (NextToken()==ESqlLiteralInt)
				{
				iToken.Literal().ToInt32L();
				aDef.iMaxLength=iToken.Literal().Int32();
				NextToken();
				t=RightBracketL();
				}
			else
				return SqlErrorL();
			}
		break;
	default:
		break;
		}
	return t;
	}

CSqlSearchCondition* TSqlParser::SearchCondition(TInt aNot)
//
// Parse a search-condition
//
	{
	CSqlSearchCondition* left=BooleanTerm(aNot);
	if (left==0 || Parse(ESqlKeyword_or)==ESqlNoToken)
		return left;
	return CSqlMultiNode::New(aNot ? CSqlMultiNode::EAnd : CSqlMultiNode::EOr,left,SearchCondition(aNot));
	}

CSqlSearchCondition* TSqlParser::BooleanTerm(TInt aNot)
//
// Parse a boolean-term
//
	{
	CSqlSearchCondition* left=BooleanFactor(aNot);
	if (left==0 || Parse(ESqlKeyword_and)==ESqlNoToken)
		return left;
	return CSqlMultiNode::New(aNot ? CSqlMultiNode::EOr : CSqlMultiNode::EAnd,left,BooleanTerm(aNot));
	}

CSqlSearchCondition* TSqlParser::BooleanFactor(TInt aNot)
//
// Parse a boolean-factor
//
	{
	while (Parse(ESqlKeyword_not))
		aNot=~aNot;
	return BooleanPrimary(aNot);
	}

CSqlSearchCondition* TSqlParser::BooleanPrimary(TInt aNot)
//
// Parse a boolean-factor
//
	{
	// brackets only allowed in this element, so this ordering is valid
	if (iToken!=ESqlLeftBracket)
		return Predicate(aNot);
	// bracketed search condition
	NextToken();
	CSqlSearchCondition* node=SearchCondition(aNot);
	if (!node)
		return node;
	if (iToken==ESqlRightBracket)
		{
		NextToken();
		return node;
		}
	delete node;
	return SqlError();
	}

CSqlSearchCondition* TSqlParser::Predicate(TInt aNot)
//
// Parse predicate
// On null return, error will already be set
//
	{
	if (iToken!=ESqlIdentifier)	// column name
		return SqlError();
	TPtrC column(iToken.Literal().DesC());
	TSqlTokenType t=NextToken();
	if (t==ESqlIdentifier)
		{	// like-predicate or null-predicate
		switch (Keyword())
			{
		case ESqlKeyword_is:	// IS [NOT] NULL
			if (Parse(ESqlKeyword_not))
				aNot=~aNot;
			if (Parse(ESqlKeyword_null)==ESqlNoToken)
				return SqlError();
			return new CSqlNullPredicate(aNot ? CSqlNullPredicate::EIsNotNull : CSqlNullPredicate::EIsNull,column);
		case ESqlKeyword_not:	// NOT LIKE
			if (Parse(ESqlKeyword_like)==ESqlNoToken)
				return SqlError();
			aNot=~aNot;
			// drop through to Like
		case ESqlKeyword_like:	// LIKE
			{
			if (iToken!=ESqlLiteralText)
				return SqlError();

			//Following code is for the implementation of limited-ESCAPE-clause 
			const TText *next = iSql.Next(); 
			TPtrC pattern(iToken.Literal().DesC());
			RSqlLiteral tmp = iToken.Literal();
		
			NextToken(); // Searching for ESCAPE Key word

			if (Keyword() == ESqlKeyword_escape)
				{
				TInt length = pattern.Length();
				if (length <= 0 || length > (KMaxSegmentLength + 2 ))
					return SqlError();
				TPtrC escapeChar(iToken.Literal().DesC());
				if (escapeChar.Length() <= 0)
					return SqlError();
				TChar escchar = escapeChar[0];
				TText newPattern[KMaxSegmentLength + 2]; // '*' can come as first and last char
				TInt count = PatternFilter(pattern,escchar, newPattern);
				if (count <=0 )
					return SqlError();
				iToken.Literal().SetText(newPattern,(newPattern + count));
				// copy the text to RSqlLiteral as newpattern could go out of scope
				if (iToken.Literal().CopyText() != KErrNone)
					{
					return SqlError();
					}
				CSqlSearchCondition* node = new CSqlLikePredicate(aNot ? CSqlLikePredicate::ENotLike : CSqlLikePredicate::ELike,column,iToken.Literal());
				if (node)
					{
					NextToken();
					node->iIsEscape = ETrue;
					}
				return node;						
				}
			else
				{
				//Setto the previous node
				iSql.Set(next);
				CSqlSearchCondition* node = new CSqlLikePredicate(aNot ? CSqlLikePredicate::ENotLike : CSqlLikePredicate::ELike,column,tmp);
				if(node)
					NextToken();
				return node;
				
				}
			}
		default:
			return SqlError();
			}
		}
	// Comparison predicate...
	CSqlSearchCondition::TType op;
	switch (t)
		{
	case ESqlGreaterEqual:
		aNot=~aNot;
		// drop through to less
	case ESqlLess:
		op=aNot ? CSqlSearchCondition::EGreaterEqual : CSqlSearchCondition::ELess;
		break;
	case ESqlGreater:
		aNot=~aNot;
		// drop through to less equal
	case ESqlLessEqual:
		op=aNot ? CSqlSearchCondition::EGreater: CSqlSearchCondition::ELessEqual;
		break;
	case ESqlNotEqual:
		aNot=~aNot;
		// drop through to equal
	case ESqlEqual:
		op=aNot ? CSqlSearchCondition::ENotEqual : CSqlSearchCondition::EEqual;
		break;
	default:
		return SqlError();
		}
	t=NextToken();
	if (t!=ESqlLiteralInt && t!=ESqlLiteralReal && 
			t!=ESqlLiteralTime && t!=ESqlLiteralText &&
			t!=ESqlLiteralBlob )
		return SqlError();
	CSqlSearchCondition* node=new CSqlCompPredicate(op,column,iToken.Literal());
	if (node)
		NextToken();
	return node;
	}

CSqlSearchCondition* TSqlParser::SearchConditionL()
//
// Parse a search-condition
//
	{
	CSqlSearchCondition* sc=SearchCondition(0);
	if (!sc)
		{
		__LEAVE_IF_ERROR(Error());	// syntax error
		User::LeaveNoMemory();		// otherwise a OOM error
		}
	return sc;
	}

void TSqlParser::SortSpecificationL(CDbKey& aKey)
//
// Parse a sort-specification
//
	{
	for (;;)
		{
		TDbKeyCol col(IdentifierL());
		NextToken();
		if (Parse(ESqlKeyword_desc))
			col.iOrder=col.EDesc;
		else
			{
			Parse(ESqlKeyword_asc);
			col.iOrder=col.EAsc;
			}
		aKey.AddL(col);
		if (iToken!=ESqlComma)
			break;
		NextToken();
		}
	}

CSqlQuery* TSqlParser::QueryLC()
//
// Generate a CSqlQuery
//
	{
	CSqlQuery* query=new(ELeave) CSqlQuery;
	CleanupStack::PushL(query);
	if (ParseL(ESqlKeyword_select)==ESqlAsterisk)
		NextToken();
	else
		ColumnListL(query->iColumns);
	ParseL(ESqlKeyword_from);
	IdentifierL(query->iTable);
	if (Parse(ESqlKeyword_where))
		query->iSearchCondition=SearchConditionL();
	if (Parse(ESqlKeyword_order))
		{
		ParseL(ESqlKeyword_by);
		SortSpecificationL(query->SortSpecificationL());
		}
	EndL();
	return query;
	}

CSqlSearchCondition* TSqlParser::SearchConditionLC()
//
// Parse a standalone search-condition
//
	{
	CSqlSearchCondition* sc=SearchConditionL();
	CleanupStack::PushL(sc);
	EndL();
	return sc;
	}

CSqlDDLStatement* TSqlParser::CreateTableLC()
//
// Parse a CREATE TABLE statement
//
	{
	CSqlCreateTableStatement* statement=new(ELeave) CSqlCreateTableStatement;
	CleanupStack::PushL(statement);
	IdentifierL(statement->iName);
	ParseL(ESqlLeftBracket);
	TDbCol def;
	for (;;)
		{
		AddColumnSpecL(def);
		if (Parse(ESqlKeyword_not))
			{
			ParseL(ESqlKeyword_null);
			def.iAttributes|=TDbCol::ENotNull;
			}
		if (Parse(ESqlKeyword_autoincrement))
			{
			def.iAttributes|=TDbCol::EAutoIncrement;
			}
		statement->iColumns.AddL(def);
		if (iToken!=ESqlComma)
			break;
		NextToken();
		}
	RightBracketL();
	return statement;
	}

CSqlDDLStatement* TSqlParser::DropTableLC()
//
// Parse a DROP TABLE statement
//
	{
	CSqlDropTableStatement* statement=new(ELeave) CSqlDropTableStatement;
	CleanupStack::PushL(statement);
	IdentifierL(statement->iName);
	return statement;
	}

CSqlDDLStatement* TSqlParser::AlterTableLC()
//
// Parse a CREATE TABLE statement
//
	{
	CSqlAlterTableStatement* statement=new(ELeave) CSqlAlterTableStatement;
	CleanupStack::PushL(statement);
	IdentifierL(statement->iName);
	TSqlTokenType t=Parse(ESqlKeyword_add);
	if (t!=ESqlNoToken)
		{
		TDbCol def;
		if (t==ESqlLeftBracket)
			{
			NextToken();
			for (;;)
				{
				t=AddColumnSpecL(def);
				statement->iAdd.AddL(def);
				if (t!=ESqlComma)
					break;
				NextToken();
				}
			RightBracketL();
			}
		else
			{
			AddColumnSpecL(def);
			statement->iAdd.AddL(def);
			}
		}
	t=Parse(ESqlKeyword_drop);
	if (t!=ESqlNoToken)
		{
		if (t!=ESqlLeftBracket)
			ColumnNameL(statement->iDrop);
		else
			{
			NextToken();
			ColumnListL(statement->iDrop);
			RightBracketL();
			}
		}
	return statement;
	}

CSqlDDLStatement* TSqlParser::CreateIndexLC(TBool aUnique)
//
// Parse a CREATE INDEX statement
//
	{
	CSqlCreateIndexStatement* statement=new(ELeave) CSqlCreateIndexStatement;
	CleanupStack::PushL(statement);
	if (aUnique)
		statement->iKey.MakeUnique();
	IdentifierL(statement->iName);
	ParseL(ESqlKeyword_on);
	IdentifierL(statement->iTable);
	ParseL(ESqlLeftBracket);
	SortSpecificationL(statement->iKey);
	RightBracketL();
	statement->iKey.SetComparison(EDbCompareNormal);
	TSqlTokenType collate=Parse(ESqlKeyword_collate);
	if ( collate != ESqlNoToken )
		{
		TSqlTokenType normal=Parse(ESqlKeyword_normal);
		if ( normal == ESqlNoToken )
			{
			TSqlTokenType collated=Parse(ESqlKeyword_collated);
			if ( collated == ESqlNoToken )
				{
				TSqlTokenType folded=Parse(ESqlKeyword_folded);
				if ( folded != ESqlNoToken)
					{
					statement->iKey.SetComparison(EDbCompareFolded);
					}
				}
			else 
				{
				statement->iKey.SetComparison(EDbCompareCollated);
				}
			}
		}
	return statement;
	}

CSqlDDLStatement* TSqlParser::DropIndexLC()
//
// Parse a DROP INDEX statement
//
	{
	CSqlDropIndexStatement* statement=new(ELeave) CSqlDropIndexStatement;
	CleanupStack::PushL(statement);
	IdentifierL(statement->iName);
	ParseL(ESqlKeyword_from);
	IdentifierL(statement->iTable);
	return statement;
	}

CSqlDDLStatement* TSqlParser::DDLStatementLC()
	{
	CSqlDDLStatement* statement;
	if (Parse(ESqlKeyword_create))
		{
		if (Parse(ESqlKeyword_table))
			statement=CreateTableLC();
		else
			{
			TSqlTokenType unique=Parse(ESqlKeyword_unique);
			ParseL(ESqlKeyword_index);
			statement=CreateIndexLC(unique);
			}
		}
	else if (Parse(ESqlKeyword_drop))
		{
		if (Parse(ESqlKeyword_table))
			statement=DropTableLC();
		else
			{
			ParseL(ESqlKeyword_index);
			statement=DropIndexLC();
			}
		}
	else
		{
		ParseL(ESqlKeyword_alter);
		ParseL(ESqlKeyword_table);
		statement=AlterTableLC();
		}
	EndL();
	return statement;
	}

TSqlTokenType TSqlParser::ColumnValueL(CSqlValues& aValues)
//
// parse a column-value and add to aValues
//
	{
	switch (iToken.Type())
		{
	case ESqlLiteralInt:
	case ESqlLiteralReal:
	case ESqlLiteralTime:
	case ESqlLiteralText:
	case ESqlLiteralBlob:
		aValues.AddL(iToken.Literal());
		return NextToken();
	case ESqlIdentifier:	// NULL
		{
		TSqlTokenType t=ParseL(ESqlKeyword_null);
		aValues.AddL(RSqlLiteral());		// default c'ted RSqlLiteral is null
		return t;
		}
	default:	// SQL error
		return SqlErrorL();
		}
	}

CSqlDMLStatement* TSqlParser::InsertStatementLC()
//
// parse an insert-statement
//
	{
	ParseL(ESqlKeyword_into);
	CSqlInsertStatement* statement=CSqlInsertStatement::NewLC();
	if (IdentifierL(statement->iQuery.iTable)==ESqlLeftBracket)
		{
		NextToken();
		ColumnListL(statement->iQuery.iColumns);
		RightBracketL();
		}
	ParseL(ESqlKeyword_values);
	ParseL(ESqlLeftBracket);
	CSqlValues& values=statement->ValuesL();
	while (ColumnValueL(values)==ESqlComma)
		NextToken();
	RightBracketL();
	return statement;
	}

CSqlDMLStatement* TSqlParser::UpdateStatementLC()
//
// parse an update-statement
//
	{
	CSqlModifyStatement* statement=CSqlModifyStatement::NewLC();
	IdentifierL(statement->iQuery.iTable);
	ParseL(ESqlKeyword_set);
	CSqlValues& values=statement->ValuesL();
	for (;;)
		{
		ColumnNameL(statement->iQuery.iColumns);
		ParseL(ESqlEqual);
		if (ColumnValueL(values)!=ESqlComma)
			break;
		NextToken();
		}
	if (Parse(ESqlKeyword_where))
		statement->iQuery.iSearchCondition=SearchConditionL();
	return statement;
	}

CSqlDMLStatement* TSqlParser::DeleteStatementLC()
//
// parse a delete-statement
//
	{
	ParseL(ESqlKeyword_from);
	CSqlModifyStatement* statement=CSqlModifyStatement::NewLC();
	IdentifierL(statement->iQuery.iTable);
	if (Parse(ESqlKeyword_where))
		statement->iQuery.iSearchCondition=SearchConditionL();
	return statement;
	}

CSqlDMLStatement* TSqlParser::DMLStatementLC()
	{
	CSqlDMLStatement* statement;
	if (Parse(ESqlKeyword_insert))
		statement=InsertStatementLC();
	else if (Parse(ESqlKeyword_update))
		statement=UpdateStatementLC();
	else 
		{
		ParseL(ESqlKeyword_delete);
		statement=DeleteStatementLC();
		}
	EndL();
	return statement;
	}

Sql::TStatementType TSqlParser::Type()
	{
	TSqlKeyword k=TSqlLexer::Keyword(iToken);
	if (k==ESqlKeyword_create || k==ESqlKeyword_alter || k==ESqlKeyword_drop)
		return Sql::EDDL;
	if (k==ESqlKeyword_insert || k==ESqlKeyword_update || k==ESqlKeyword_delete)
		return Sql::EDML;
	return Sql::ENone;
	}
	
TInt TSqlParser::PatternFilter(const TDesC& aPattern,const TChar aEscape,TText *aNewPatternBuffer )
	{
	TInt length = aPattern.Length();
	TInt count =0;
	
	// Ensure that the pattern begins and ends with an '*'
	if ((length < 2) || (aPattern[0] != KMatchAny || aPattern[length-1] != KMatchAny))
 		{
 		return KSqlError;	
 		}	

	for (TInt i = 1 ; i< length -1 ;++i)
		{
		if (aPattern[i]== (TUint)aEscape) 
			{
			if ((aPattern[i + 1] == KMatchAny) || (aPattern[i + 1] == KMatchOne) || (aPattern[i + 1] == (TUint)aEscape))
				{
				i++;
				aNewPatternBuffer[count++] = aPattern[i];
				}
			else
				{
				return KSqlError;  
				}
			}
		else
			{
			if ((aPattern[i] == KMatchAny || aPattern[i] == KMatchOne)  )
				{
				return KSqlError;
				}
			else
				{
				aNewPatternBuffer[count++] = aPattern[i];
				}
			}					
		}

	return count;
	}
