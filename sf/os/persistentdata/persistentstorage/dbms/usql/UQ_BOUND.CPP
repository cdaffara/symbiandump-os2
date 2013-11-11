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

// class CSqlBoundNode

CSqlBoundNode::CSqlBoundNode(TType aType,const TDesC& aColumn)
	: CSqlSearchCondition(aType)
	{
	iColumn.iName.Set(aColumn);
	}

void CSqlBoundNode::BindL(const RDbTableRow& aSource)
	{
	const HDbColumnSet& columns=aSource.Table().Def().Columns();
	TDbColNo col=columns.ColNoL(iColumn.iName());
	if (col==KDbNullColNo)
		__LEAVE(KErrNotFound);
	iSource=&aSource;
	iColumn.iBound.iNumber=col;
	iColumn.iBound.iType=columns[col].Type();
	}

const TDesC& CSqlBoundNode::ColumnName() const
//
// Return the column name
// If not bound it is embedded, otherwise lookup via the column def
//
	{
	if (IsBound())
		return *iSource->Table().Def().Columns()[iColumn.iBound.iNumber].iName;
	return (const TPtrC&)iColumn.iName;
	}

MStreamBuf& CSqlBoundNode::StreamLC(const TDbBlob& aBlob) const
	{
	__ASSERT(!aBlob.IsInline());
	return *iSource->Table().BlobsL()->ReadLC(aBlob.Id(),ColType());
	}

