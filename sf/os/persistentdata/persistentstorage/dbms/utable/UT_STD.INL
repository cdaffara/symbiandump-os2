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

// Class RDbIndexes
inline RDbIndexes::RDbIndexes()
	: iRep(_FOFF(CDbTableIndexDef,iLink))
	{}
// Class HDbColumnSet
inline HDbColumnSet::TIterator HDbColumnSet::ColumnL(const TDesC& aColumn)
	{return CONST_CAST(TIterator,((const HDbColumnSet*)this)->ColumnL(aColumn));}

// Class RDbTableSchema
inline RDbTableSchema::RDbTableSchema()
	: iRep(_FOFF(CDbTableDef,iLink))
	{}
inline void RDbTableSchema::Close()
	{Discard();}
inline TBool RDbTableSchema::IsLoaded() const
	{return iLoaded;}
inline void RDbTableSchema::Remove(CDbTableDef* aDef)
	{iRep.Remove(*aDef);}

// Class CDbTable
inline void CDbTable::Discard()
	{__ASSERT(iRef==ECached);Release();}
inline TBool CDbTable::IsIdle() const
	{return iRef==EIdle;}
inline TBool CDbTable::InUse() const
	{return iRef>=0;}
inline RDbCache& CDbTable::Cache()
	{return Database().iCache;}

// Class CDbTable::TValid
inline CDbTable& CDbTable::TValid::Table() const
	{return iTable;}
inline CDbTable::TValid::operator TAny*() const
	{return Table().iDatabase;}
inline RDbTransaction& CDbTable::TValid::Transaction() const
	{return Table().Database().Transaction();}

// Class RDbTables
inline RDbTables::RDbTables()
	: iRep(_FOFF(CDbTable,iLink))
	{}
inline TBool RDbTables::IsEmpty() const
	{return iRep.IsEmpty();}
inline void RDbTables::Add(CDbTable& aTable)
	{iRep.AddLast(aTable);}
inline void RDbTables::Remove(CDbTable& aTable)
	{iRep.Remove(aTable);}
inline RDbTables::operator TSglQueBase&()
	{return iRep;}

// Class RDbTransaction
inline RDbTransaction::RDbTransaction()
	:iMaxLock(1)
	{
	__ASSERT(iDatabase==0);
	__ASSERT(iAction==EDbReadLock);
	__ASSERT(iLockState==EDbReadLock);
	__ASSERT(iSharers==0);
	__ASSERT(iLockCount==0);
	__ASSERT(iUpdaters==0);
	__ASSERT(iNotifier==0);
	}
inline void RDbTransaction::Open(CDbTableDatabase& aDatabase)
	{__ASSERT(!iDatabase);iDatabase=&aDatabase;}
inline const TDbGeneration& RDbTransaction::RollbackGeneration() const
	{return iRollback;}
inline TBool RDbTransaction::IsLocked() const
	{return iLockCount>0;}
inline CDbTableDatabase& RDbTransaction::Database()
	{__ASSERT(iDatabase);return *iDatabase;}
inline void RDbTransaction::ReadyL() const
	{if (iLockState&EFailed) __LEAVE(KErrDied);}

// Class CDbTableDatabase
inline void CDbTableDatabase::Open()
	{++iRef;}
inline void CDbTableDatabase::AddTable(CDbTable& aTable)
	{iTables.Add(aTable);}

// Class CDbTableSource
inline void CDbTableSource::SetIterator(CDbRecordIter* anIter)
	{iIter=anIter;}
inline CDbTable& CDbTableSource::Table()
	{return iRow.Table();}
inline const RDbTableRow& CDbTableSource::Row()
	{return iRow;}

// Class CDbDataStage
inline void CDbDataStage::SetSource(CDbDataSource* aSource)
	{__ASSERT(!iSource);iSource=aSource;}
inline CDbDataSource& CDbDataStage::Source()
	{return *iSource;}
inline const CDbDataSource& CDbDataStage::Source() const
	{return *iSource;}

// Class CDbRestrictStage
inline void CDbRestrictStage::SetRestriction(CSqlSearchCondition* aRestriction)
	{__ASSERT(!iRestriction);iRestriction=aRestriction;}
inline CSqlSearchCondition& CDbRestrictStage::Restriction() const
	{__ASSERT(iRestriction);return *iRestriction;}

// Class RDbAccessPlan
inline RDbAccessPlan::RDbAccessPlan(CSqlQuery* aQuery,TDbTextComparison aComparison)
	:iPlans(EGranularity),iComparison(aComparison),iQuery(aQuery)
	{__DEBUG(iSource=NULL;iTable=NULL;)}
inline RDbAccessPlan::RDbAccessPlan()
	:iPlans(EGranularity),iComparison(EDbCompareNormal)
	{__DEBUG(iSource=NULL;iTable=NULL;)}
inline CDbTable& RDbAccessPlan::Table()
	{__ASSERT(iTable);return *iTable;}
inline CDbDataSource& RDbAccessPlan::Source()
	{__ASSERT(iSource);return *iSource;}
inline CDbDataSource* RDbAccessPlan::Adopt()
	{__ASSERT(iSource);CDbDataSource* s=iSource;iSource=NULL;return s;}

// Class RDbAccessPlan::TPlan
inline RDbAccessPlan::TPlan::TPlan() :
	iFlags(0),
	iIndex(0),
	iLower(0),
	iUpper(0)
	{
	}

inline TBool RDbAccessPlan::TPlan::operator!=(const TPlan& aPlan) const
	{return !(iFlags==aPlan.iFlags && iIndex==aPlan.iIndex && iUpper==aPlan.iUpper && iLower==aPlan.iLower);}

// Class RDbAccessPlan::CDbCompPredicateList
inline RDbAccessPlan::CDbCompPredicateList::CDbCompPredicateList(const CDbTableDef& aTableDef,TDbTextComparison aComparison)
	: CArrayFixFlat<CSqlCompPredicate*>(EGranularity),iTableDef(aTableDef),iComparison(aComparison),iRestriction(EFalse)
	{}
inline TBool RDbAccessPlan::CDbCompPredicateList::IsRestriction()
	{return iRestriction;}

// Class CDbTableCursor
inline CDbBlobSpace& CDbTableCursor::BlobsL() const
	{CDbBlobSpace* b=iValid.Table().BlobsL();__ASSERT(b);return *b;}
inline void CDbTableCursor::AddSink()
	{__ASSERT((iFlags&EWriteBuf)==0);iFlags|=EWriteBuf;}
inline void CDbTableCursor::ReleaseSink()
	{__ASSERT(iFlags&EWriteBuf);iFlags&=~EWriteBuf;}
inline void CDbTableCursor::AddSource()
	{__ASSERT(iReadBuf<EMaxReadBuf);++iReadBuf;}
inline void CDbTableCursor::ReleaseSource()
	{__ASSERT(iReadBuf>0);--iReadBuf;}

// Class CDbTableCursor::HReadBuf
inline void CDbTableCursor::HReadBuf::Set(MStreamBuf* aHost)
	{__ASSERT(!iHost);iHost=aHost;}

// Class CDbTableDatabase::CIncremental
inline RDbTransaction& CDbTableDatabase::CIncremental::Transaction()
	{return iTransaction;}
inline CDbTableDatabase& CDbTableDatabase::CIncremental::Database()
	{return Transaction().Database();}
inline void CDbTableDatabase::CIncremental::Construct(CStepper* aStepper)
	{iStepper=aStepper;}
inline TBool CDbTableDatabase::CIncremental::IsCommitted() const
	{return iState==ECommitted;}

// Class CDbTableDatabase::CCreateIndex
inline CDbTableDatabase::CCreateIndex::CCreateIndex(RDbTransaction& aTransaction)
	: CIncrementalDDL(aTransaction)
	{}
// Class CDbTableDatabase::CDropIndex
inline CDbTableDatabase::CDropIndex::CDropIndex(RDbTransaction& aTransaction)
	: CIncrementalDDL(aTransaction)
	{}
// Class CDbTableDatabase::CDropTable
inline CDbTableDatabase::CDropTable::CDropTable(RDbTransaction& aTransaction)
	: CIncrementalDDL(aTransaction)
	{}
// Class CDbTableDatabase::CAlterTable
inline CDbTableDatabase::CAlterTable::CAlterTable(RDbTransaction& aTransaction,CDbTableDef& aDef)
	: CIncrementalDDL(aTransaction), iDef(aDef)
	{}
// Class CDbIncrementalDML
inline CDbIncrementalDML::CDbIncrementalDML(RDbAccessPlan& aPlan)
	:iTable(aPlan.Table()),iSource(aPlan.Adopt())
	{__ASSERT(iState==EInitialising);}
inline RDbTransaction& CDbIncrementalDML::Transaction()
	{return iTable.Database().Transaction();}
inline TBool CDbIncrementalDML::IsUpdate() const
	{return iValues!=NULL;}
inline void CDbIncrementalDML::SetState(TState aState)
	{iState=aState;}

// Class CDbTableDatabase::CInterface
inline CDbTableDatabase& CDbTableDatabase::CInterface::Database()
	{return iDatabase;}

// Class CDbTableDatabase::CSource
inline CDbTableDatabase& CDbTableDatabase::CSource::Database()
	{return iDatabase;}
