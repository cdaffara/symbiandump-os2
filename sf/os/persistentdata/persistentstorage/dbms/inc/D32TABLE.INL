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

// Class CDbTableIndexDef
inline const TDesC& CDbTableIndexDef::Name() const
	{return *iName;}
inline const CDbKey& CDbTableIndexDef::Key() const
	{return iKey;}
inline CDbKey& CDbTableIndexDef::Key()
	{return iKey;}
// Class RDbIndexes
inline void RDbIndexes::Add(CDbTableIndexDef* aDef)
	{iRep.AddLast(*aDef);}
inline void RDbIndexes::Remove(CDbTableIndexDef* aDef)
	{iRep.Remove(*aDef);}
inline const TSglQueBase& RDbIndexes::AsQue() const
	{return iRep;}

//Class TDbColumnDef
inline TDbColType TDbColumnDef::Type() const
	{return TDbColType(iType);}

// Class HDbColumnSet
inline HDbColumnSet::TIterator HDbColumnSet::Begin()
	{return &iColumns[0];}
inline HDbColumnSet::TIteratorC HDbColumnSet::Begin() const
	{return &iColumns[0];}
inline HDbColumnSet::TIteratorC HDbColumnSet::End() const
	{return iEnd;}
inline TBool HDbColumnSet::HasLongColumns() const
	{return iAttributes&ELongColumns;}
inline TBool HDbColumnSet::HasAutoIncrement() const
	{return iAttributes&EAutoIncrement;}
inline const TDbColumnDef& HDbColumnSet::operator[](TDbColNo aColNo) const
	{return iColumns[aColNo-1];}

// Class CDbTableDef
inline const HDbColumnSet& CDbTableDef::Columns() const
	{return *iColumns;}
inline HDbColumnSet& CDbTableDef::Columns()
	{return *iColumns;}
inline const TDesC& CDbTableDef::Name() const
	{return *iName;}
inline RDbIndexes&  CDbTableDef::Indexes()
	{return iIndexes;}
inline const RDbIndexes& CDbTableDef::Indexes() const
	{return iIndexes;}
inline const CDbTableIndexDef* CDbTableDef::Key(const TDesC& aColumn) const
	{return FindKey(aColumn,ETrue);}
inline TBool CDbTableDef::IsIndexed(const TDesC& aColumn) const
	{return FindKey(aColumn,EFalse)!=NULL;}

// Class RDbTableSchema
inline void RDbTableSchema::Add(CDbTableDef* aDef)
	{iRep.AddLast(*aDef);}
inline RDbTableSchema::operator TSglQueBase&()
	{return iRep;}
inline void RDbTableSchema::Loaded()
	{iLoaded=ETrue;}
inline TBool RDbTableSchema::IsEmpty() const
	{return iRep.IsEmpty();}

// Class TDbGenerationMark
inline void TDbGenerationMark::Construct(const TDbGeneration& aGeneration)
	{iGeneration=&aGeneration;iMark=aGeneration;}
inline void TDbGenerationMark::Mark()
	{iMark=*iGeneration;}
inline TBool TDbGenerationMark::Changed() const
	{return iMark!=*iGeneration;}
// Class CDbTable
inline TBool CDbTable::IsActive() const
	{return iDatabase!=0;}
inline const CDbTableDef& CDbTable::Def() const
	{__ASSERT_DEBUG(iDef,User::Invariant());return *iDef;}
inline CDbTableDatabase& CDbTable::Database()
	{__ASSERT_DEBUG(iDatabase,User::Invariant());return *iDatabase;}
inline const TDbGeneration& CDbTable::Generation() const
	{return iGeneration;}
// Class RDbTableRow
inline RDbTableRow::RDbTableRow()
	: iTable(0)
	{}
inline CDbTable& RDbTableRow::Table() const
	{return *iTable;}

// Class CDbTableDatabase
inline RDbTransaction& CDbTableDatabase::Transaction()
	{return iTransaction;}
inline RDbTableSchema& CDbTableDatabase::Schema()
	{return iSchema;}
// Class CDbTableDatabase::CBuildIndex
inline CDbRecordIndex& CDbTableDatabase::CBuildIndex::Index()
	{return *iIndex;}
