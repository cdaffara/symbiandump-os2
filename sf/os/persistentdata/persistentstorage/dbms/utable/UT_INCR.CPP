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

// Class CDbTableDatabase::CMultiStepper

inline CDbTableDatabase::CMultiStepper::CMultiStepper( TInt aCount )
 :	iStepper( iSteppers - 1 ), iEnd( iSteppers + aCount )
	{}

CDbTableDatabase::CMultiStepper::~CMultiStepper()
	{
	SStepper* const end = iEnd;
	for ( SStepper* iter = iSteppers; iter < end; ++iter )
		delete iter->iStepper;
	}

EXPORT_C CDbTableDatabase::CMultiStepper* CDbTableDatabase::CMultiStepper::NewL( TInt aStepperCount )
	{
	return new( ELeave, sizeof( SStepper ) * ( aStepperCount - 1 ) ) CMultiStepper( aStepperCount );	// get the extra size for the entries, leaves on error
	}

EXPORT_C void CDbTableDatabase::CMultiStepper::AddStepper( CStepper* aStepper, TInt aStep )
	{
	++iStepper;
	__ASSERT( iStepper < iEnd );
	iStepper->iStepper = aStepper;
	iStepper->iStep = aStep;
	}

EXPORT_C TInt CDbTableDatabase::CMultiStepper::TotalSteps()
//
// Count the number of steps, and normalise the step counts
//
	{
	TInt steps = 0;
	SStepper* const end = iStepper;
	for( SStepper* stepper = iSteppers; stepper <= end; ++stepper )
		{
		TInt step = stepper->iStep;
		stepper->iStep = steps;
		steps += step;
		}
	return steps;
	}

TInt CDbTableDatabase::CMultiStepper::StepL( TInt aStep )
	{
	SStepper* stepper = iStepper;
	__ASSERT( stepper != 0 && stepper >= iSteppers );
	TInt base;
	for ( ; ; )
		{
		base = stepper->iStep;
		if ( aStep > base )
			break;
		if ( --stepper < iSteppers )
			return 0;
		iStepper = stepper;
		}
	return stepper->iStepper->StepL( aStep - base ) + base;
	}

// Class CDbTableDatabase::CIncremental

CDbTableDatabase::CIncremental::CIncremental(RDbTransaction& aTransaction)
	: iTransaction(aTransaction)
	{
	Database().Open();		// we reference the database
	__ASSERT(iState==ERunning);
	}

CDbTableDatabase::CIncremental::~CIncremental()
	{
	delete iStepper;
	Database().Close();		// drop the database reference
	}

TBool CDbTableDatabase::CIncremental::NextL(TInt& aStep)
//
// if step is 1 then invopke the last step otherwise do
//
	{
	__ASSERT(!IsCommitted());
	if (iState==EFailed)
		__LEAVE(KErrDied);
	iState=EFailed;
	if (aStep!=ELastStep)
		{
		aStep=DoNextL(aStep);
		iState=ERunning;
		}
	else
		{
		DoLastL();
		aStep=0;
		iState=ECommitted;
		}
	return 0;	// return 0 for DDL incremental operations
	}

TInt CDbTableDatabase::CIncremental::DoNextL(TInt aStep)
//
// default use of stepper object (re-normalised the step count)
//
	{
	return iStepper->StepL(aStep-ELastStep)+ELastStep;
	}

// Class CDbTableDatabase::CIncrementalDDL

CDbTableDatabase::CIncrementalDDL::CIncrementalDDL(RDbTransaction& aTransaction)
	: CIncremental(aTransaction)
	{
	Transaction().DDLBegin();
	}

CDbTableDatabase::CIncrementalDDL::~CIncrementalDDL()
	{
	if (!IsCommitted())
		Transaction().DDLRollback();
	}

void CDbTableDatabase::CIncrementalDDL::DoLastL()
	{
	Transaction().DDLCommitL();
	}

// Class CDbTableDatabase::CBuildIndex

EXPORT_C CDbTableDatabase::CBuildIndex* CDbTableDatabase::CBuildIndex::NewL(CDbTableDatabase& aDatabase,const CDbTableDef& aTable,const CDbTableIndexDef& anIndex)
	{
	CBuildIndex* self=new(ELeave) CBuildIndex();
	CleanupStack::PushL(self);
	CDbTableSource* source=self->iSource=aDatabase.TableSourceL(aTable.Name());
	CDbTable& table=source->Table();
	source->SetIterator(table.IteratorL());
	self->iIndex=table.RecordIndexL(anIndex);
	self->iIndex->OpenL();
	CleanupStack::Pop();
	return self;
	}

CDbTableDatabase::CBuildIndex::CBuildIndex()
	: iNext(EDbFirst)
	{}

CDbTableDatabase::CBuildIndex::~CBuildIndex()
	{
	delete iIndex;
	delete iSource;
	}

EXPORT_C TInt CDbTableDatabase::CBuildIndex::Steps(TInt aCardinality)
	{
	return aCardinality+1;
	}

EXPORT_C TInt CDbTableDatabase::CBuildIndex::StepsL()
	{
	return Steps(iSource->CountL());
	}

TInt CDbTableDatabase::CBuildIndex::StepL(TInt aStep)
//
// One step on an incremental index build
//
	{
	for (TInt inc=1;;++inc)
		{
		TDbRecordId id;
		if (iSource->CDbDataSource::GotoL(iNext,id)!=CDbDataSource::ESuccess)
			break;	// run out of data
		iSource->ReadRowL(id);
		if (!iIndex->InsertL(id,iSource->Row()))
			__LEAVE(KErrAlreadyExists);		// duplicate key - fail
		iNext=EDbNext;
		if (inc==ERecordsPerStep)
			return Max(aStep-inc,1);
		}
	iIndex->FlushL();
	return 0;
	}

// Class CDbTableDatabase::CCreateIndex

CDbTableDatabase::CCreateIndex* CDbTableDatabase::CCreateIndex::NewLC(RDbTransaction& aTransaction)
	{
	CCreateIndex* self=new(ELeave) CCreateIndex(aTransaction);
	CleanupStack::PushL(self);
	return self;
	}

TInt CDbTableDatabase::CCreateIndex::ConstructL(const CDbTableDef& aTable,const CDbTableIndexDef& anIndex)
	{
	CBuildIndex* builder=CBuildIndex::NewL(Database(),aTable,anIndex);
	Construct(builder);
	return builder->StepsL();
	}

// Class CDbTableDatabase::CDropIndex

CDbTableDatabase::CDropIndex::~CDropIndex()
	{
	delete iDef;
	}

CDbTableDatabase::CDropIndex* CDbTableDatabase::CDropIndex::NewL(RDbTransaction& aTransaction,const CDbTableDef& aTable,CDbTableIndexDef* anIndex,TInt& aStep)
	{
	CDropIndex* self=new(ELeave) CDropIndex(aTransaction);
	CleanupStack::PushL(self);
	self->Construct(aTransaction.Database().IndexDiscarderL(aTable,*anIndex,aStep));
	self->iDef=anIndex;
	aStep+=ELastStep;
	CleanupStack::Pop();
	return self;
	}

// Class CDbTableDatabase::CDropTable

CDbTableDatabase::CDropTable::~CDropTable()
	{
	Database().Release(*iDef);		// discard the table using this definition
	delete iDef;
	}

CDbTableDatabase::CDropTable* CDbTableDatabase::CDropTable::NewL(RDbTransaction& aTransaction,CDbTableDef* aTable,TInt& aStep)
	{
	CDropTable* self=new(ELeave) CDropTable(aTransaction);
	CDbTableDatabase& database=aTransaction.Database();
	CleanupStack::PushL(self);
	CMultiStepper* mstepper=CMultiStepper::NewL(aTable->Indexes().Count()+1);
	self->Construct(mstepper);
	CStepper* stepper=database.RecordDiscarderL(*aTable,aStep);
	mstepper->AddStepper(stepper,aStep);
	TSglQueIterC<CDbTableIndexDef> iter(aTable->Indexes().AsQue());
	for (const CDbTableIndexDef* xDef;(xDef=iter++)!=0;)
		{
		stepper=database.IndexDiscarderL(*aTable,*xDef,aStep);
		mstepper->AddStepper(stepper,aStep);
		}
	aStep=mstepper->TotalSteps()+ELastStep;
	self->iDef=aTable;
	CleanupStack::Pop();
	return self;
	}

// Class CDbTableDatabase::CAlterTable

CDbTableDatabase::CAlterTable::~CAlterTable()
	{
	delete iNewSet;
	}

void CDbTableDatabase::CAlterTable::ConstructL(const CDbColSet& aNewDef,TInt& aStep)
//
// get all the deleted columns
// check changes to columns still present
// get all the new columns
// construct a new columns set based on the changes
//
	{
//
// flag all columns as dropped initially
	HDbColumnSet& columns=iDef.Columns();
	HDbColumnSet::TIterator iter=columns.Begin();
	HDbColumnSet::TIteratorC const end=columns.End();
	do
		{
		iter->iFlags=TDbColumnDef::EDropped;
		} while (++iter<end);
//
// look for additions and changes
	CDbColSet* change=CDbColSet::NewLC();
	CDbColSet* add=CDbColSet::NewLC();
	for (TDbColSetIter iterNew(aNewDef);iterNew;++iterNew)
		{
		const TDbCol& col=*iterNew;
		TDbColumnDef* def=columns.ColumnL(iterNew->iName);
		if (!def)	// a new column
			add->AddL(col);
		else
			{	// see if the definition has changed
			if (def->iAttributes!=col.iAttributes)
				__LEAVE(KErrArgument);		// can't change attributes
			TUint8 flag=0;
			if (def->iType!=col.iType)
				flag=TDbColumnDef::EChangedType;
			else if (def->iType>=EDbColText8 && col.iMaxLength!=KDbUndefinedLength && col.iMaxLength!=def->iMaxLength)
				flag=TDbColumnDef::EChangedLen;
			def->iFlags=flag;
			if (flag)
				change->AddL(col);	// column has changed
			}
		}
//
// check that all marked columns are not indexed
//
	iter=columns.Begin();
	do
		{
		if (iter->iFlags && iDef.IsIndexed(*iter->iName))
			__LEAVE(KErrArgument);		// can't remove indexed column
		} while (++iter<end);
//
	iNewSet=HDbColumnSet::NewL(aNewDef.Count());
	iDef.AlteredColumnSetL(*iNewSet,*change,*add);
	CleanupStack::PopAndDestroy(2);	// add, change
	Construct(Database().TableAlterL(iDef,*iNewSet,aStep));
	}

CDbTableDatabase::CAlterTable* CDbTableDatabase::CAlterTable::NewL(RDbTransaction& aTransaction,CDbTableDef& aTable,const CDbColSet& aNewDef,TInt& aStep)
//
// Check the validity of the new definition before asking the implentation to provide
// the incremental class
//
	{
	CAlterTable* self=new(ELeave) CAlterTable(aTransaction,aTable);
	CleanupStack::PushL(self);
	self->ConstructL(aNewDef,aStep);
	aStep+=ELastStep;
	CleanupStack::Pop();
	return self;
	}

void CDbTableDatabase::CAlterTable::DoLastL()
//
// last step is to change the definition
//
	{
	iDef.ExchangeColumnSet(iNewSet);
	iNewSet=0;
	CIncrementalDDL::DoLastL();
	}

// Class CDbTableDatabase::CUtility

inline CDbTableDatabase::CUtility::CUtility(RDbTransaction& aTransaction,CDbDatabase::TUtility aType)
	: CIncremental(aTransaction)
	{Transaction().UtilityBegin(aType);}

CDbTableDatabase::CUtility::~CUtility()
	{
	if (!IsCommitted())
		Transaction().UtilityRollback();
	}

CDbTableDatabase::CUtility* CDbTableDatabase::CUtility::NewL(RDbTransaction& aTransaction,CDbDatabase::TUtility aType,TInt& aStep)
	{
	CUtility* self=new(ELeave) CUtility(aTransaction,aType);
	CleanupStack::PushL(self);
	self->Construct(aTransaction.Database().UtilityL(aType,aStep));
	aStep+=ELastStep;
	CleanupStack::Pop();
	return self;
	}

void CDbTableDatabase::CUtility::DoLastL()
	{
	Transaction().UtilityCommitL();
	}
