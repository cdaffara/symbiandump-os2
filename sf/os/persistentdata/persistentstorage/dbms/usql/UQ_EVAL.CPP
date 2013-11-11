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
// Evaluation tree base classes
// 
//

#include "UQ_STD.H"

// class CSqlQuery

CSqlQuery::CSqlQuery()
	{}

CSqlQuery::~CSqlQuery()
	{
	iColumns.Close();
	delete iSearchCondition;
	delete iSortSpecification;
	}

CDbKey& CSqlQuery::SortSpecificationL()
	{
	CDbKey* key=iSortSpecification;
	if (!key)
		iSortSpecification=key=CDbKey::NewL();
	return *key;
	}

void CSqlQuery::RemovePredicate(CSqlCompPredicate* aNode)
	{
	CSqlSearchCondition::TType type=iSearchCondition->NodeType();
	if (type>CSqlSearchCondition::ENotLike && type<CSqlSearchCondition::ENotEqual) 
		{
		delete iSearchCondition;
		iSearchCondition=0;
		}
	else if (type==CSqlSearchCondition::EAnd)
		{
		CSqlMultiNode* multiNode=iSearchCondition->MultiNode();
		multiNode->Remove(aNode);
		iSearchCondition=CSqlMultiNode::Reduce(multiNode);
		}
	else
		__ASSERT(0);
	}


// class CSqlSearchCondition

CSqlSearchCondition::CSqlSearchCondition(TType aType)
	: iType(aType)
	{
	iIsEscape = EFalse;
	}

