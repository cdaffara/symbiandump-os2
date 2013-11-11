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

#include "US_STD.H"
#include "U32STD_DBMS.H"

// Class CDbStoreIndex::CRecover

NONSHARABLE_CLASS(CDbStoreIndex::CRecover) : public CDbTableDatabase::CStepper
	{
private:
	enum {EClearStep=1};
public:
	CRecover(CDbStoreDatabase& aDatabase,const CDbStoreDef& aTable,const CDbStoreIndexDef& anIndex);
	~CRecover();
	static inline TInt Steps(TInt aCardinality);
private:
	TInt StepL(TInt aStep);
private:
	CDbStoreDatabase& iDatabase;
	const CDbStoreDef& iTable;
	const CDbStoreIndexDef& iIndex;
	CDbTableDatabase::CBuildIndex* iBuild;
	};

inline TInt CDbStoreIndex::CRecover::Steps(TInt aCardinality)
	{return CDbTableDatabase::CBuildIndex::Steps(aCardinality)+EClearStep;}

CDbStoreIndex::CRecover::CRecover(CDbStoreDatabase& aDatabase,const CDbStoreDef& aTable,const CDbStoreIndexDef& anIndex)
	: iDatabase(aDatabase),iTable(aTable),iIndex(anIndex)
	{}

CDbStoreIndex::CRecover::~CRecover()
	{
	delete iBuild;
	}

TInt CDbStoreIndex::CRecover::StepL(TInt aStep)
	{
	if (iBuild==NULL)
		{	// first step
		iBuild=CDbTableDatabase::CBuildIndex::NewL(iDatabase,iTable,iIndex);
		((CDbStoreIndex&)iBuild->Index()).DiscardL();
		return aStep-EClearStep;
		}
	aStep=iBuild->StepL(aStep);
	if (aStep==0)
		{	// final step
		iDatabase.IndexRecoveredL();
		delete iBuild;
		iBuild=NULL;
		}
	return aStep;
	}

// Class CDbStoreIndex::CRepair

NONSHARABLE_CLASS(CDbStoreIndex::CRepair) : public CDbTableDatabase::CStepper
	{
private:
	enum {ERepairStep=1};
public:
	CRepair(CDbStoreDatabase& aDatabase,const CDbStoreDef& aTable,const CDbStoreIndexDef& anIndex);
	static inline TInt Steps();
private:
	TInt StepL(TInt aStep);
private:
	CDbStoreDatabase& iDatabase;
	const CDbStoreDef& iTable;
	const CDbStoreIndexDef& iIndex;
	};

inline TInt CDbStoreIndex::CRepair::Steps()
	{return ERepairStep;}

CDbStoreIndex::CRepair::CRepair(CDbStoreDatabase& aDatabase,const CDbStoreDef& aTable,const CDbStoreIndexDef& anIndex)
	: iDatabase(aDatabase),iTable(aTable),iIndex(anIndex)
	{}

TInt CDbStoreIndex::CRepair::StepL(TInt)
	{
	CDbStoreIndex* index=CDbStoreIndex::NewL(iDatabase,iIndex,iTable);
	CleanupStack::PushL(index);
	index->OpenL();
	index->RepairL();
	CleanupStack::PopAndDestroy();
	return 0;
	}

// Class CDbStoreDatabase

CDbTableDatabase::CStepper* CDbStoreDatabase::RecoverL(TInt& aStep)
	{
	TSglQueIterC<CDbStoreDef> iter(SchemaL());	// ensure we have the schema
	TInt count=0;
	const CDbStoreDef* tDef;
	while ((tDef=iter++)!=NULL)
		count+=tDef->Indexes().Count();
	CMultiStepper* mStepper=CMultiStepper::NewL(count);
	CleanupStack::PushL(mStepper);
// damaged ones first (as they are invoked last
	count=0;
	iter.SetToFirst();
	while ((tDef=iter++)!=NULL)
		{
		TInt records=CDbStoreRecords::CardinalityL(Store(),*tDef);
		TSglQueIterC<CDbStoreIndexDef> xIter(tDef->Indexes().AsQue());
		for (const CDbStoreIndexDef* xDef;(xDef=xIter++)!=NULL;)
			{
			if (!CDbStoreIndex::IsDamagedL(*this,*xDef))
				continue;
			// rebuild from scratch
			CDbStoreIndex::CRecover* rebuild=new(ELeave) CDbStoreIndex::CRecover(*this,*tDef,*xDef);
			mStepper->AddStepper(rebuild,rebuild->Steps(records));
			++count;
			}
		}
	if (count==0)
		{	// no indexes are actually damaged
		aStep=0;
		CleanupStack::PopAndDestroy();
		return NULL;
		}
	// repair the other ones after a reclaim
	iter.SetToFirst();
	while ((tDef=iter++)!=NULL)
		{
		TSglQueIterC<CDbStoreIndexDef> xIter(tDef->Indexes().AsQue());
		for (const CDbStoreIndexDef* xDef;(xDef=xIter++)!=NULL;)
			{
			if (CDbStoreIndex::IsDamagedL(*this,*xDef))
				continue;
			// repair from sequence set
			CDbStoreIndex::CRepair* repair=new(ELeave) CDbStoreIndex::CRepair(*this,*tDef,*xDef);
			mStepper->AddStepper(repair,repair->Steps());
			}
		}
	iPagePool->Discard();
	iPagePool->ReclaimAllL();	// recover all page pool space
	aStep=mStepper->TotalSteps();
	CleanupStack::Pop();
	return mStepper;
	}
