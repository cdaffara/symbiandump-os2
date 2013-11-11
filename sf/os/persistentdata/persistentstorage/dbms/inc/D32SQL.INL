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

// Class RSqlLiteral
inline TBool RSqlLiteral::IsNull() const
	{return iType==ENull;}
inline TInt32 RSqlLiteral::Int32() const
	{__ASSERT(iType==EInt32);return iVal.iInt32;}
inline TUint32 RSqlLiteral::Uint32() const
	{__ASSERT(iType==EUint32);return iVal.iUint32;}
inline const TInt64& RSqlLiteral::Int64() const
	{__ASSERT(iType==EInt64);return iVal.iInt64;}
inline const TReal32& RSqlLiteral::Real32() const
	{__ASSERT(iType==EReal32);return iVal.iReal32;}
inline const TReal64& RSqlLiteral::Real64() const
	{__ASSERT(iType==EReal64);return iVal.iReal64;}
inline const TTime& RSqlLiteral::Time() const
	{__ASSERT(iType==ETime);return iVal.iTime;}
inline const TPtrC RSqlLiteral::DesC() const
	{__ASSERT(iType==EPtr);return TPtrC(iVal.iPtr.iPtr,iVal.iPtr.iEnd-iVal.iPtr.iPtr);}
inline const TText* RSqlLiteral::Ptr() const
	{__ASSERT(iType==EPtr);return iVal.iPtr.iPtr;}
inline const TText* RSqlLiteral::End() const
	{__ASSERT(iType==EPtr);return iVal.iPtr.iEnd;}
inline const HBufC8& RSqlLiteral::Text8() const
	{__ASSERT(iType==EBuf8);return *(const HBufC8*)iVal.iAlloc;}
inline const HBufC16& RSqlLiteral::Text16() const
	{__ASSERT(iType==EBuf16);return *(const HBufC16*)iVal.iAlloc;}
inline const HMatcherPattern8& RSqlLiteral::Pattern8() const
	{__ASSERT(iType==EPattern8);return *(const HMatcherPattern8*)iVal.iAlloc;}
inline const HMatcherPattern16& RSqlLiteral::Pattern16() const
	{__ASSERT(iType==EPattern16);return *(const HMatcherPattern16*)iVal.iAlloc;}

// Class RSqlColumnList
inline RSqlColumnList::RSqlColumnList()
	:RArray<TPtrC>(EGranularity)
	{}

// Class CSqlSearchCondition
inline CSqlSearchCondition::TType CSqlSearchCondition::NodeType() const
	{return iType;}
inline CSqlMultiNode* CSqlSearchCondition::MultiNode()
	{__ASSERT(iType==EAnd||iType==EOr);return STATIC_CAST(CSqlMultiNode*,this);}
inline CSqlNullPredicate* CSqlSearchCondition::NullPredicate()
	{__ASSERT(iType==EIsNull||iType==EIsNotNull);return STATIC_CAST(CSqlNullPredicate*,this);}
inline CSqlCompPredicate* CSqlSearchCondition::CompPredicate()
	{__ASSERT(iType==ELess||iType==ELessEqual||iType==EEqual||iType==EGreaterEqual||iType==EGreater||iType==ENotEqual);return STATIC_CAST(CSqlCompPredicate*,this);}
inline CSqlLikePredicate* CSqlSearchCondition::LikePredicate()
	{__ASSERT(iType==ELike||iType==ENotLike);return STATIC_CAST(CSqlLikePredicate*,this);}

// Class CSqlMultiNode
inline TInt CSqlMultiNode::Count() const
	{return iEnd-iLeaves;}
inline CSqlSearchCondition* CSqlMultiNode::SubNode(TInt aIndex) const
	{__ASSERT(TUint(aIndex)<TUint(Count()));return iLeaves[aIndex];}
inline void CSqlMultiNode::SetSubNode(TInt aIndex,CSqlSearchCondition* aSearchCondition) 
	{__ASSERT(TUint(aIndex)<TUint(Count()));iLeaves[aIndex]=aSearchCondition;}

// Class CSqlBoundNode
inline TBool CSqlBoundNode::IsBound() const
	{return iSource!=0;}
inline TDbColNo CSqlBoundNode::ColNo() const
	{__ASSERT(IsBound());return iColumn.iBound.iNumber;}
inline TDbColType CSqlBoundNode::ColType() const
	{return iColumn.iBound.iType;}

// Class CSqlLiteralNode 
inline const RSqlLiteral& CSqlLiteralNode::Value() const
	{return iLiteral;}
inline RSqlLiteral& CSqlLiteralNode::Value()
	{return iLiteral;}

// Class CSqlQuery
inline const TDesC& CSqlQuery::Table() const
	{return iTable;}
inline TBool CSqlQuery::HasColumnList() const
	{return iColumns.Count()!=0;}
inline const RSqlColumnList& CSqlQuery::ColumnList() const
	{return iColumns;}
inline TBool CSqlQuery::HasSearchCondition() const
	{return iSearchCondition!=0;}
inline CSqlSearchCondition& CSqlQuery::SearchCondition()
	{__ASSERT(HasSearchCondition());return *iSearchCondition;}
inline CSqlSearchCondition* CSqlQuery::AdoptSearchCondition()
	{__ASSERT(HasSearchCondition());CSqlSearchCondition* rr=iSearchCondition;iSearchCondition=0;return rr;}
inline TBool CSqlQuery::HasSortSpecification() const
	{return iSortSpecification!=0;}
inline CDbKey& CSqlQuery::SortSpecification()
	{__ASSERT(HasSortSpecification());return *iSortSpecification;}

// Class CSqlDMLStatement
inline CSqlQuery& CSqlDMLStatement::Query()
	{return iQuery;}
inline CSqlValues& CSqlDMLStatement::Values()
	{__ASSERT(iValues);return *iValues;}
inline CSqlValues* CSqlDMLStatement::AdoptValues()
	{__ASSERT(iValues);CSqlValues* v=iValues;iValues=NULL;return v;}
inline TBool CSqlDMLStatement::HasValues() const
	{return iValues!=NULL;}

// Class CSqlModifyStatement
inline TBool CSqlModifyStatement::IsUpdate() const
	{return HasValues();}

//class TSqlParser2

inline Sql::TStatementType TSqlParser2::StatementType() const
	{
	return iStatementType;
	}

inline const TDesC& TSqlParser2::TableName() const
	{
	return iTableName;
	}
