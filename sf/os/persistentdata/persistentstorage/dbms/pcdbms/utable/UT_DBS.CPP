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
// Class CDbTableDatabase::CInterface

//
// ctor. Add the initial reference to the database engine
//
CDbTableDatabase::CInterface::CInterface(CDbTableDatabase& aDatabase)
	:iDatabase(aDatabase)
	{
	aDatabase.Open();
	}

//
// dtor. remove our reference from the database engine
//
CDbTableDatabase::CInterface::~CInterface()
	{
	if (Database().Transaction().InTransaction(*this))
		Rollback();			// release the lock if we have it
	Database().Close();
	}

void CDbTableDatabase::CInterface::PrepareDDLL()
	{
	Database().Transaction().DDLPrepareL(*this);
	}

//
// destroy the database (database must be empty)
//
TInt CDbTableDatabase::CInterface::Destroy()
	{
	__ASSERT(Database().Schema().IsEmpty());
	TRAPD(r,Database().DestroyL());
	return r;
	}

//
// Initiate a transaction on the database
//
TInt CDbTableDatabase::CInterface::Begin()
	{
	TRAPD(r,Database().Transaction().BeginL(*this));
	return r;
	}

//
// Commit a transaction on the database
//
TInt CDbTableDatabase::CInterface::Commit()
	{
	TRAPD(r,Database().Transaction().CommitL(*this));
	return r;
	}

//
// Rollback a failed transaction on the database
//
void CDbTableDatabase::CInterface::Rollback()
	{
	Database().Transaction().Rollback(*this);
	}

//
// Report a requested property
//
TInt CDbTableDatabase::CInterface::Property(CDbDatabase::TProperty aProperty)
	{
	if (aProperty==EInTransaction)
		return Database().Transaction().InTransaction(*this) ? 1 : 0;
//
	return Database().Property(aProperty);
	}

void CDbTableDatabase::CInterface::CreateTableL(const TDesC& aName,const CDbColSet& aColSet,const CDbKey* aPrimaryKey)
	{
	PrepareDDLL();
	Database().DoCreateTableL(aName,aColSet,aPrimaryKey);
	}

CDbCursor* CDbTableDatabase::CInterface::PrepareViewL(const TDbQuery& aQuery,const TDbWindow& aWindow,RDbRowSet::TAccess anAccess)
	{
	return Database().PrepareViewL(aQuery,aWindow,anAccess);
	}

CDbCursor* CDbTableDatabase::CInterface::OpenTableL(const TDesC& aName,RDbRowSet::TAccess anAccess)
	{
	return Database().PrepareTableL(aName,anAccess);
	}

CDbIncremental* CDbTableDatabase::CInterface::OpenDropTableL(const TDesC& aTable,TInt& aStep)
	{
	PrepareDDLL();
	return Database().OpenDropTableL(aTable,aStep);
	}

CDbIncremental* CDbTableDatabase::CInterface::OpenAlterTableL(const TDesC& aTable,const CDbColSet& aNewDef,TInt& aStep)
	{
	PrepareDDLL();
	return Database().OpenAlterTableL(aTable,aNewDef,aStep);
	}

CDbIncremental* CDbTableDatabase::CInterface::OpenCreateIndexL(const TDesC& aName,const TDesC& aTable,const CDbKey& aKey,TInt& aStep)
	{
	PrepareDDLL();
	return Database().OpenCreateIndexL(aName,aTable,aKey,aStep);
	}

CDbIncremental* CDbTableDatabase::CInterface::OpenDropIndexL(const TDesC& aName,const TDesC& aTable,TInt& aStep)
	{
	PrepareDDLL();
	return Database().OpenDropIndexL(aName,aTable,aStep);
	}

//
// Create a database utility. Check the corresponding property to see if it is viable
//
CDbIncremental* CDbTableDatabase::CInterface::OpenUtilityL(CDbDatabase::TUtility aType,TInt& aStep)
	{
	if (!Property(TProperty(-aType)))
		{	// nothing to do, or not capable
		aStep=0;
		return 0;
		}
	RDbTransaction& t=Database().Transaction();
	t.UtilityPrepareL(*this);
	if (aType==ERecover)
		Database().Release();		// must not have any tables open during recovery
	return CUtility::NewL(t,aType,aStep);
	}

CDbIncremental* CDbTableDatabase::CInterface::OpenExecuteL(const TDesC& aSql,TDbTextComparison aComparison,TInt& aInit)
	{
	switch (Sql::Type(aSql))
		{
	default:
		__ASSERT(0);
	case Sql::ENone:
		__LEAVE(KErrArgument);
	case Sql::EDDL:
		{
		CDbIncremental* inc=Sql::ParseDDLStatementLC(aSql)->ExecuteL(*this,aComparison,aInit);
		CleanupStack::PopAndDestroy();		// statement
		return inc;
		}
	case Sql::EDML:
		{
		CSqlDMLStatement* statement=Sql::ParseDMLStatementLC(aSql);
		RDbTransaction& t=Database().Transaction();
		t.DMLCheckL();			// ensure we can open a cursor
		t.DMLPrepareL(*this);
		CDbIncremental* inc=statement->ExecuteL(Database(),aComparison,aInit);
		CleanupStack::PopAndDestroy();		// statement
		return inc;
		}
		}
	}

//
// Open a client-side notifier object
//
CDbNotifier* CDbTableDatabase::CInterface::OpenNotifierL()
	{
	return Database().Transaction().NotifierL();
	}

//
// list the tables on the database
//
void CDbTableDatabase::CInterface::TablesL(CDbTableNames& aNames)
	{
	TSglQueIterC<CDbTableDef> iter(Database().SchemaL());
	for (const CDbTableDef* def;(def=iter++)!=0;)
		aNames.AddL(def->Name());
	}

//
// build a column set for the table
//
void CDbTableDatabase::CInterface::ColumnsL(CDbColSet& aColSet,const TDesC& aName)
	{
	TDbCol col;
	const HDbColumnSet& set=Database().SchemaL().FindL(aName).Columns();
	HDbColumnSet::TIteratorC iter=set.Begin();
	const HDbColumnSet::TIteratorC end=set.End();
	do
		{
		iter->AsTDbCol(col);
		aColSet.AddL(col);
		} while (++iter<end);
	}

//
// List the indexes on a table
//
void CDbTableDatabase::CInterface::IndexesL(CDbIndexNames& aNames,const TDesC& aTable)
	{
	TSglQueIterC<CDbTableIndexDef> iter(Database().SchemaL().FindL(aTable).Indexes().AsQue());
	for (const CDbTableIndexDef* def;(def=iter++)!=0;)
		aNames.AddL(def->Name());
	}

//
// build a key for the index
//
void CDbTableDatabase::CInterface::KeysL(CDbKey& aKey,const TDesC& aName,const TDesC& aTable)
	{
	const CDbKey& key=Database().SchemaL().FindL(aTable).Indexes().FindL(aName).Key();
	TInt max=key.Count();
	for (TInt ii=0;ii<max;++ii)
		aKey.AddL(key[ii]);
	if (key.IsUnique())
		aKey.MakeUnique();
	if (key.IsPrimary())
		aKey.MakePrimary();
	aKey.SetComparison(key.Comparison());
	}

// Class CDbTableDatabase::CSource

//
// ctor. Add a reference to the database engine
//
CDbTableDatabase::CSource::CSource(CDbTableDatabase& aDatabase)
	:iDatabase(aDatabase)
	{
	aDatabase.Open();
	}

//
// dtor. remove our reference from the database engine
//
CDbTableDatabase::CSource::~CSource()
	{
	Database().Close();
	}


//SYMBIAN_REMOVE_TRIVIAL_ENCRYPTION version of the method
CDbDatabase* CDbTableDatabase::CSource::AuthenticateL()
	{
	Database().AuthenticateL();
	return Database().InterfaceL();
	}


CDbNotifier* CDbTableDatabase::CSource::OpenNotifierL()
	{
	return Database().Transaction().NotifierL();
	}

// Class CDbTableDatabase

EXPORT_C CDbTableDatabase::CDbTableDatabase()
	{
	iCache.Open();
	iTransaction.Open(*this);
	}

EXPORT_C CDbTableDatabase::~CDbTableDatabase()
	{
	__ASSERT(iRef==0);
	iTransaction.Close();
	iTables.Close();
	iSchema.Close();
	iCache.Close();
	}

void CDbTableDatabase::Close()
	{
	__ASSERT(iRef>0);
	if (--iRef==0)
		delete this;
	}

//
// Construct the implementation interface for the database framework
// On success, the interface takes an owning reference to this
//
EXPORT_C CDbDatabase* CDbTableDatabase::InterfaceL()
	{
	return new(ELeave) CInterface(*this);
	}

//
// Construct the implementation interface for the database framework
// On success, the interface takes an owning reference to this
//
EXPORT_C CDbSource* CDbTableDatabase::SourceL()
	{
	return new(ELeave) CSource(*this);
	}

//
// Load the schema if required
//
EXPORT_C RDbTableSchema& CDbTableDatabase::SchemaL()
	{
	RDbTableSchema& schema=Schema();
	if (!schema.IsLoaded())
		{
		schema.Discard();
		LoadSchemaL();
		schema.Loaded();
		}
	return schema;
	}

//
// Find any tables which are now idle, and cache them
// Caching such a table can result in *any* table being deleted
//  and so the iteration must restart every time an idle table is cached
//
void CDbTableDatabase::CheckIdle()
	{
	while (!iTables.IsEmpty())
		{
		for (TSglQueIter<CDbTable> iter(iTables);;)
			{
			CDbTable* table=iter++;
			if (table==0)		// no more idle tables
				return;
			if (table->IsIdle())
				{	// idle->cache transition can modify the collection
				table->Idle();
				break;
				}
			}
		}
	// If there are no tables left open, then the database can go idle
	Idle();
	}

//
// Does nothing by default
//
EXPORT_C void CDbTableDatabase::Idle()
	{
    }

//
// default implementation
//
EXPORT_C TInt CDbTableDatabase::Property(CDbDatabase::TProperty aProperty)
	{
	if (aProperty<0)
		return 0;		// utilities are not available with table database
	__ASSERT(aProperty!=CDbDatabase::EInTransaction);
	return KErrNotSupported;
	}

//
// default implementation, should never be invoked
//
EXPORT_C CDbTableDatabase::CStepper* CDbTableDatabase::UtilityL(CDbDatabase::TUtility,TInt&)
	{
	__ASSERT(0);
	return 0;
	}

void CDbTableDatabase::FlushL(TDbLockType aLock)
	{
	if (aLock>=EDbWriteLock)
		{
		TSglQueIter<CDbTable> iter(iTables);
		for (CDbTable* table;(table=iter++)!=0;)
			table->FlushL();
		}
	}

void CDbTableDatabase::Abandon(TDbLockType aLock)
	{
	if (aLock>=EDbWriteLock)
		{
		TSglQueIter<CDbTable> iter(iTables);
		for (CDbTable* table;(table=iter++)!=0;)
			table->Abandon();
		}
	if (aLock==EDbSchemaLock)
		{
		Release();			// ensure no-one is using the old schema
		Schema().Discard();
		}
	}

//
// Discard a table if it is using this definition
//
void CDbTableDatabase::Release(const CDbTableDef& aDef)
	{
	TSglQueIter<CDbTable> iter(iTables);
	for (CDbTable* table;(table=iter++)!=0;)
		{
		if (&table->Def()==&aDef)
			{
			table->Release();
			break;
			}
		}
	}

//
// Discard all open tables as the schema has been modified or discarded
//
void CDbTableDatabase::Release()
	{
	TSglQueIter<CDbTable> iter(iTables);
	for (CDbTable* table;(table=iter++)!=0;)
		table->Release();
	}

//
// Remove a table from the open set
//
void CDbTableDatabase::RemoveTable(CDbTable& aTable)
	{
	iTables.Remove(aTable);
	if (!Transaction().IsLocked() && iTables.IsEmpty())
		Idle();
	}

CDbTableSource* CDbTableDatabase::TableSourceL(const TDesC& aTableName)
	{
	CDbTableSource* rec=new(ELeave) CDbTableSource;
	CDbTable* table=iTables.Find(aTableName);
	if (table)
		table->Open();		// a new reference to it
	else
		{
		CleanupStack::PushL(rec);
		table=TableL(SchemaL().FindL(aTableName));
		CleanupStack::Pop();	// table source
		}
	rec->Construct(table);
	iCache.Flush();			// check-point for object cache membership
	return rec;
	}

//
// Prepare the data pipeline
//
CDbCursor* CDbTableDatabase::PrepareViewL(const TDbQuery& aQuery,const TDbWindow& aWindow,RDbRowSet::TAccess aAccess)
	{
	Transaction().DMLCheckL();	// ensure we can open a cursor
	CSqlQuery *query=Sql::ParseQueryLC(aQuery.Query());
	RDbAccessPlan plan(query,aQuery.Comparison());
	plan.BuildLC(*this,aAccess,aWindow);
	CDbTableCursor* cursor=CDbTableCursor::NewL(plan,aAccess);
	CleanupStack::Pop();			// the plan
	CleanupStack::PopAndDestroy();	// the query
	return cursor;
	}

//
// Prepare the data pipeline
//
CDbCursor* CDbTableDatabase::PrepareTableL(const TDesC& aTable,RDbRowSet::TAccess aAccess)
	{
	Transaction().DMLCheckL();	// ensure we can open a cursor
	RDbAccessPlan plan;
	plan.BuildLC(*this,aTable,aAccess);
	CDbTableCursor* cursor=CDbTableCursor::NewL(plan,aAccess);
	CleanupStack::Pop();			// the plan
	return cursor;
	}

void CDbTableDatabase::DoCreateTableL(const TDesC& aName,const CDbColSet& aColSet,const CDbKey* aPrimaryKey)
	{
	if (SchemaL().Find(aName))
		__LEAVE(KErrAlreadyExists);
// validate the colset set passed in
	Validate::NameL(aName);
	Validate::ColSetL(aColSet);
	Transaction().DDLBeginLC();
	iSchema.Add(CreateTableL(aName,aColSet,aPrimaryKey));
	Transaction().DDLCommitL();
	CleanupStack::Pop();		// rollback not required
	}

//
// Create index definition and prepare to build it
//
CDbTableDatabase::CIncremental* CDbTableDatabase::OpenCreateIndexL(const TDesC& aName,const TDesC& aTable,const CDbKey& aKey,TInt& aStep)
	{
	CDbTableDef& tDef=SchemaL().FindL(aTable);
	RDbIndexes& indexes=tDef.Indexes();
	if (indexes.Find(aName)!=NULL)
		__LEAVE(KErrAlreadyExists);
	if (indexes.Count()==KDbTableMaxIndexes)
		__LEAVE(KErrNotSupported);
	Validate::NameL(aName);
	Validate::KeyL(aKey,tDef.Columns());
	if (aKey.IsPrimary())
		__LEAVE(KErrArgument);	// cannot create a primary index

	CCreateIndex* builder=CCreateIndex::NewLC(Transaction());
	CDbTableIndexDef* xDef=CreateIndexL(tDef,aName,aKey);
	tDef.Indexes().Add(xDef);
	Release(tDef);				// release any table using the old schema
	aStep=builder->ConstructL(tDef,*xDef);
	CleanupStack::Pop();	// builder
	return builder;
	}

//
// Remove index from schema first, then remove index data
//
CDbTableDatabase::CIncremental* CDbTableDatabase::OpenDropIndexL(const TDesC& aName,const TDesC& aTable,TInt& aStep)
	{
	CDbTableDef& tDef=SchemaL().FindL(aTable);
	CDbTableIndexDef& xDef=tDef.Indexes().FindL(aName);
//
	Release(tDef);				// release any table using the old schema
	CDropIndex* drop=CDropIndex::NewL(Transaction(),tDef,&xDef,aStep);
	tDef.Indexes().Remove(&xDef);
	return drop;
	}

CDbTableDatabase::CIncremental* CDbTableDatabase::OpenDropTableL(const TDesC& aTable,TInt& aStep)
	{
	CDbTableDef& tDef=SchemaL().FindL(aTable);
//
	Release(tDef);				// release the tables using the old schema
	CDropTable* drop=CDropTable::NewL(Transaction(),&tDef,aStep);
	iSchema.Remove(&tDef);
	return drop;
	}

//
// Create an incremental table altering object
//
CDbTableDatabase::CIncremental* CDbTableDatabase::OpenAlterTableL(const TDesC& aTable,const CDbColSet& aNewDef,TInt& aStep)
	{
	CDbTableDef& tDef=SchemaL().FindL(aTable);
	Validate::ColSetL(aNewDef);
	Release(tDef);				// release the tables using the old schema
	return CAlterTable::NewL(Transaction(),tDef,aNewDef,aStep);
	}

//
// Reserved for future development
//
EXPORT_C void CDbTableDatabase::Reserved_1()
	{
    }

//
// Reserved for future development
//
EXPORT_C void CDbTableDatabase::Reserved_2()
	{
    }
