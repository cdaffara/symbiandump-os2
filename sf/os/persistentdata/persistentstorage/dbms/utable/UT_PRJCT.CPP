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

#include "UT_STD.H"

// Class HDbColumnMap

class HDbColumnMap
	{
public:
	static HDbColumnMap* NewL(const RSqlColumnList& aSelect,const HDbColumnSet& aColumns);
	inline const TDbColNo& operator[](TDbColNo aCol) const
		{__ASSERT(aCol>0&&aCol<=iCount);return iMap[aCol-1];}
	inline TInt Count() const
		{return iCount;}
private:
	TInt iCount;
	TDbColNo iMap[1];	// at least one
	};

HDbColumnMap* HDbColumnMap::NewL(const RSqlColumnList& aSelect,const HDbColumnSet& aColumns)
	{
	TInt count=aSelect.Count();
	HDbColumnMap* self=(HDbColumnMap*)User::AllocLC(_FOFF(HDbColumnMap,iMap[count]));
	self->iCount=count;
	TDbColNo* pCol=&self->iMap[0];
	for (TInt ii=0;ii<count;++ii)
		{
		TDbColNo col=aColumns.ColNoL(aSelect[ii]);
		if (col==KDbNullColNo)
			__LEAVE(KErrNotFound);
		*pCol++=col;
		}
	CleanupStack::Pop();
	return self;
	}

// Class CDbProjectStage

CDbProjectStage::CDbProjectStage()
	{}

CDbProjectStage::~CDbProjectStage()
	{
	delete iMap;
	}

void CDbProjectStage::ConstructL(const RSqlColumnList& aSelect,const HDbColumnSet& aColumns)
//
// Build the column map and allocate a row buffer
//
	{
	iMap=HDbColumnMap::NewL(aSelect,aColumns);
	}

RDbRow* CDbProjectStage::RowBuffer()
//
// whole-row access cannot be done through a projection
//
	{
	return 0;
	}

TDbColumn CDbProjectStage::Column(TDbColNo aColNo)
	{
	return CDbDataStage::Column((*iMap)[aColNo]);
	}

TInt CDbProjectStage::ColumnCount() const
	{
	return iMap->Count();
	}

const TDbColumnDef& CDbProjectStage::ColumnDef(TDbColNo aCol) const
	{
	return CDbDataStage::ColumnDef((*iMap)[aCol]);
	}
