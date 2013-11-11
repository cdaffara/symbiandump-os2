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
#include "U32STD_DBMS.H"

// Class CDbTableIndexDef

EXPORT_C CDbTableIndexDef::CDbTableIndexDef()
	{}

EXPORT_C CDbTableIndexDef::~CDbTableIndexDef()
	{
	delete iName;
	}

EXPORT_C void CDbTableIndexDef::ConstructL(const TDesC& aName)
//
// Construct with an empty key
//
	{
	iName=aName.AllocL();
	}

// Class RDbIndexes

void RDbIndexes::Close()
	{
	TSglQueIter<CDbTableIndexDef> iter(iRep);
	for (CDbTableIndexDef* def;(def=iter++)!=0;)
		delete def;
	}

CDbTableIndexDef* RDbIndexes::Find(const TDesC& aName) const
//
// Locate and index on the table by name
//
	{
	TSglQueIter<CDbTableIndexDef> iter(CONST_CAST(TSglQue<CDbTableIndexDef>&,iRep));
	for (CDbTableIndexDef* def;(def=iter++)!=0;)
		if (aName.CompareF(def->Name())==0)
			return def;
	return 0;
	}

CDbTableIndexDef& RDbIndexes::FindL(const TDesC& aName) const
//
// Leave if not found
//
	{
	CDbTableIndexDef* def=Find(aName);
	if (def==NULL)
		__LEAVE(KErrNotFound);
	return *def;
	}

EXPORT_C TInt RDbIndexes::Count() const
	{
	TInt count = 0;
	TSglQueIterC<CDbTableIndexDef> iter( iRep );
	for ( ; iter++ != 0; )
		++count;
	return count;
	}

// Class TDbColumnDef

EXPORT_C void TDbColumnDef::SetL(const TDbColumnDef& aCol)
//
// Replace assignment as this can leave
//
	{
	iMaxLength=aCol.iMaxLength;
	iType=aCol.iType;
	iAttributes=aCol.iAttributes;
	iFlags=aCol.iFlags;
	iName=aCol.iName->AllocL();
	}

EXPORT_C void TDbColumnDef::SetL(const TDbCol& aCol)
	{
	iMaxLength=aCol.iMaxLength;
	iType=TUint8(aCol.iType);
	iAttributes=TUint8(aCol.iAttributes);
	iName=aCol.iName.AllocL();
	}

void TDbColumnDef::AsTDbCol(TDbCol& aColumn) const
	{
	aColumn.iType=Type();
	aColumn.iMaxLength=iMaxLength;
	aColumn.iAttributes=iAttributes;
	aColumn.iName=*iName;
	}

// Class HDbColumnSet

HDbColumnSet* HDbColumnSet::NewL(TInt aCount)
	{
	return new(User::AllocL(_FOFF(HDbColumnSet,iColumns[aCount]))) HDbColumnSet(aCount);
	}

HDbColumnSet::HDbColumnSet(TInt aCount)
	: iAttributes(0), iIndex(NULL), iEnd(&iColumns[aCount])
	{
	Mem::FillZ(&iColumns[0],aCount*sizeof(iColumns[0]));
	}

HDbColumnSet::~HDbColumnSet()
	{
	User::Free(iIndex);
	const TIteratorC end=End();
	for (TIteratorC iter=Begin();iter<end;++iter)
		delete iter->iName;
	}

EXPORT_C TInt HDbColumnSet::Count() const
	{
	return End()-Begin();
	}

void HDbColumnSet::Complete()
//
// Called on completion of the column set
//
	{
	TUint attrib=0;
	const TIteratorC end=End();
	for (TIteratorC iter=Begin();iter<end;++iter)
		{
		if (TDbCol::IsLong(iter->Type()))
			attrib|=ELongColumns;
		if (iter->iAttributes&TDbCol::EAutoIncrement)
			attrib|=EAutoIncrement;
		}
	iAttributes=attrib;
	}

EXPORT_C HDbColumnSet::TIteratorC HDbColumnSet::ColumnL(const TDesC& aColumn) const
//
// Lookup name in the name index (binary search)
//
	{
	const TIteratorC* index=IndexL();
	TInt left=0;
	TInt right=Count();
	while (left<right)
		{
		TInt mid=(left+right)>>1;
		TInt c=index[mid]->iName->CompareF(aColumn);
		if (c<0)
			left=mid+1;
		else if (c>0)
			right=mid;
		else
			return index[mid];		// matched entry
		}
	return 0;		// no match
	}

EXPORT_C TDbColNo HDbColumnSet::ColNoL(const TDesC& aColumn) const
//
// Return ordinal for given column name
//
	{
	TIteratorC col=ColumnL(aColumn);
	return col ? 1+(col-Begin()) : KDbNullColNo;
	}

const HDbColumnSet::TIteratorC* HDbColumnSet::IndexL() const
//
// Return the by-name lookup index, building it if required
//
	{
	TIteratorC* index=iIndex;
	if (!index)
		{
		CONST_CAST(TIteratorC*&,iIndex)=index=(TIteratorC*)User::AllocL(Count()*sizeof(TIteratorC));
		TInt ii=0;
		TIteratorC col=Begin();
		TIteratorC end=End();
		do
			{
			// binary search for insertion point
			TInt left=0;
			TInt right=ii;
			while (left<right)
				{
				TInt mid=(left+right)>>1;
				TInt c=index[mid]->iName->CompareF(*col->iName);
				__ASSERT(c!=0);		// names are unique
				if (c<0)
					left=mid+1;
				else
					right=mid;
				}
			// insert the entry
			Mem::Move(index+left+1,index+left,(ii-left)*sizeof(TIteratorC));
			index[left]=col;
			} while (++ii,++col<end);
		}
	return index;
	}

// Class CDbTableDef

EXPORT_C CDbTableDef::CDbTableDef()
	{}

EXPORT_C CDbTableDef::~CDbTableDef()
	{
	delete iColumns;
	delete iName;
	iIndexes.Close();
	}

EXPORT_C void CDbTableDef::ConstructL(const TDesC& aName,TInt aColumnCount)
	{
	iName=aName.AllocL();
	iColumns=HDbColumnSet::NewL(aColumnCount);
	}

void CDbTableDef::ExchangeColumnSet(HDbColumnSet* aSet)
	{
	delete iColumns;
	iColumns=aSet;
	Changed();
	}

EXPORT_C void CDbTableDef::Changed()
//
// evaluate cached info about the set
//
	{
	iColumns->Complete();
	}

const CDbTableIndexDef* CDbTableDef::FindKey(const TDesC& aColumn,TBool aFirstColumn) const
//
// Find an index which keys on the given column
//
	{
	TSglQueIterC<CDbTableIndexDef> indexes(Indexes().AsQue());
	for (const CDbTableIndexDef* def;(def=indexes++)!=0;)
		{
		const CDbKey& key=def->Key();
		TInt ii=0;
		do
			{
			if (aColumn.CompareF(key[ii].iName)==0)
				return def;
			if (aFirstColumn)
				break;
			} while (++ii<key.Count());
		}
	return 0;
	}

// Class RDbTableSchema

void RDbTableSchema::Discard()
	{
	TSglQueIter<CDbTableDef> iter(iRep);
	for (CDbTableDef* def;(def=iter++)!=0;)
		delete def;
	iRep.Reset();
	iLoaded=EFalse;
	}

CDbTableDef* RDbTableSchema::Find(const TDesC& aTable)
	{
	__ASSERT(IsLoaded());
	TSglQueIter<CDbTableDef> iter(iRep);
	for (CDbTableDef* def;(def=iter++)!=0;)
		if (aTable.CompareF(def->Name())==0)
			return def;
	return 0;
	}

CDbTableDef& RDbTableSchema::FindL(const TDesC& aTable)
//
// Leave if not found
//
	{
	CDbTableDef* def=Find(aTable);
	if (def==NULL)
		__LEAVE(KErrNotFound);
	return *def;
	}

// Class RDbTables

void RDbTables::Close()
	{
	TSglQueIter<CDbTable> iter(iRep);
	for (CDbTable* table;(table=iter++)!=0;)
		table->Discard();
	}

CDbTable* RDbTables::Find(const TDesC& aTable)
	{
	TSglQueIter<CDbTable> iter(iRep);
	for (CDbTable* table;(table=iter++)!=0;)
		if (aTable.CompareF(table->Def().Name())==0)
			return table;
	return 0;
	}
