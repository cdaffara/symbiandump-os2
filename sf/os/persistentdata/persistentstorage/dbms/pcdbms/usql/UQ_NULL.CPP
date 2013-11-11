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
// SQL NULL-predicate node class
// 
//

#include "UQ_STD.H"

// class CSqlNullPredicate

CSqlNullPredicate::CSqlNullPredicate(TType aType,const TDesC& aColumn)
	: CSqlBoundNode(aType,aColumn)
	{
	__ASSERT(aType==EIsNull||aType==EIsNotNull);
	}

TBool CSqlNullPredicate::EvaluateL(const TTextOps&) const
	{
	__ASSERT(IsBound());
	TBool null=Column().IsNull();
	return NodeType()==EIsNull ? null : !null;
	}

