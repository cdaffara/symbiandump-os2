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
// DBMS server proxy database
// 
//

#include "SD_STD.H"

// Class CDbsDatabase


/**
SYMBIAN_REMOVE_TRIVIAL_ENCRYPTION version of the method.
Standard phase-one factory method for creation of CDbsDatabase instances.
@param aDbs A reference to the client side DBMS session object
@param aDatabase Database file path
@param aFormat Database format string
@return A pointer to teh created CDbsDatabase instance.
@leave System-wide error code, including KErrNoMemory
*/
CDbsDatabase* CDbsDatabase::NewL(const RDbs& aDbs, const TDesC& aDatabase, const TDesC& aFormat)
	{
	CDbsDatabase* self=new(ELeave) CDbsDatabase;
	CleanupStack::PushL(self);
	
	TIpcArgs args(&aDatabase,&aFormat,TIpcArgs::ENothing,NULL);
	self->ConstructL(aDbs,args);	// two phase construction
	CleanupStack::Pop();			// self
	return self;
	}

/**
SYMBIAN_REMOVE_TRIVIAL_ENCRYPTION version of the method.
Standard phase-two construction method for creation of CDbsDatabase instances.
@param aDbs A reference to the client side DBMS session object
@param aArgs A reference to the packed parameters needed for the database instance creation.
@leave System-wide error code, including KErrNoMemory
*/
void CDbsDatabase::ConstructL(const RDbs& aDbs, TIpcArgs& aArgs)
	{
	iObject.OpenL(aDbs,EDbsOpenDatabase,aArgs);
	}
	

CDbsDatabase::~CDbsDatabase()
	{
	iObject.Close();
	}

TInt CDbsDatabase::Destroy()
	{
	return iObject.SendReceive(EDbsDatabaseDestroy);
	}

TInt CDbsDatabase::Begin()
	{
	return iObject.SendReceive(EDbsDatabaseBegin);
	}

TInt CDbsDatabase::Commit()
	{
	return iObject.SendReceive(EDbsDatabaseCommit);
	}

void CDbsDatabase::Rollback()
	{
	iObject.SendReceive(EDbsDatabaseRollback);
	}

TInt CDbsDatabase::Property(CDbDatabase::TProperty aProperty)
	{
	return iObject.SendReceive(EDbsDatabaseProperty,TIpcArgs(aProperty));
	}

void CDbsDatabase::CreateTableL(const TDesC& aName,const CDbColSet& aColSet,const CDbKey* aPrimaryKey)
	{
	TPtrC8 colset(TDbsParam::PrepareLC(aColSet));
	TIpcArgs m(&aName,&colset,0);
	TPtrC8 primary;
	if (aPrimaryKey)
		{
		primary.Set(TDbsParam::PrepareLC(*aPrimaryKey));
		m.Set(2,&primary);
		}
	iObject.SendReceiveL(EDbsDatabaseCreateTable,m);
	if (aPrimaryKey)
		CleanupStack::PopAndDestroy();	// primary
	CleanupStack::PopAndDestroy();	// colset
	}

void CDbsDatabase::TablesL(CDbTableNames& aNames)
	{
	TIpcArgs m;
	RReadStream in(HDbsBuf::NewLC(iObject,EDbsDatabaseTables,m));
	in>>aNames;
	CleanupStack::PopAndDestroy();	// dbsbuf
	}

void CDbsDatabase::ColumnsL(CDbColSet& aColSet,const TDesC& aName)
	{
	TIpcArgs m(&aName);
	RReadStream in(HDbsBuf::NewLC(iObject,EDbsDatabaseColumns,m));
	in>>aColSet;
	CleanupStack::PopAndDestroy();
	}

void CDbsDatabase::IndexesL(CDbIndexNames& aNames,const TDesC& aTable)
	{
	TIpcArgs m(&aTable);
	RReadStream in(HDbsBuf::NewLC(iObject,EDbsDatabaseIndexes,m));
	in>>aNames;
	CleanupStack::PopAndDestroy();	// dbsbuf
	}

void CDbsDatabase::KeysL(CDbKey& aKey,const TDesC& aName,const TDesC& aTable)
	{
	TIpcArgs args(&aName,&aTable);
	RReadStream in(HDbsBuf::NewLC(iObject,EDbsDatabaseKeys,args));
	in>>aKey;
	CleanupStack::PopAndDestroy();	// dbsbuf
	}

CDbNotifier* CDbsDatabase::OpenNotifierL()
	{
	return CDbsNotifier::NewL(iObject);
	}

CDbIncremental* CDbsDatabase::OpenUtilityL(CDbDatabase::TUtility aType,TInt& aStep)
	{
	TIpcArgs args(aType);
	return CDbsIncremental::NewL(iObject,EDbsDatabaseOpenUtility,args,aStep);
	}

CDbIncremental* CDbsDatabase::OpenDropTableL(const TDesC& aTable,TInt& aStep)
	{
	TIpcArgs args(&aTable);
	return CDbsIncremental::NewL(iObject,EDbsDatabaseOpenDropTable,args,aStep);
	}

CDbIncremental* CDbsDatabase::OpenAlterTableL(const TDesC& aTable,const CDbColSet& aNewDef,TInt& aStep)
	{
	TPtrC8 colset(TDbsParam::PrepareLC(aNewDef));
	TIpcArgs args(&aTable,&colset);
	CDbIncremental* inc=CDbsIncremental::NewL(iObject,EDbsDatabaseOpenAlterTable,args,aStep);
	CleanupStack::PopAndDestroy();	// colset
	return inc;
	}

CDbIncremental* CDbsDatabase::OpenCreateIndexL(const TDesC& aName,const TDesC& aTable,const CDbKey& aKey,TInt& aStep)
	{
	TPtrC8 key(TDbsParam::PrepareLC(aKey));
	TIpcArgs args(&aName,&aTable,&key);
	CDbIncremental* inc=CDbsIncremental::NewL(iObject,EDbsDatabaseOpenCreateIndex,args,aStep);
	CleanupStack::PopAndDestroy();	// key
	return inc;
	}

CDbIncremental* CDbsDatabase::OpenDropIndexL(const TDesC& aName,const TDesC& aTable,TInt& aStep)
	{
	TIpcArgs args(&aName,&aTable);
	return CDbsIncremental::NewL(iObject,EDbsDatabaseOpenDropIndex,args,aStep);
	}

CDbIncremental* CDbsDatabase::OpenExecuteL(const TDesC& aSql,TDbTextComparison aComparison,TInt& aInit)
	{
	TIpcArgs args(&aSql,aComparison);
	return CDbsIncremental::NewL(iObject,EDbsDatabaseExecute,args,aInit);
	}

CDbCursor* CDbsDatabase::PrepareViewL(const TDbQuery& aQuery,const TDbWindow& aWindow,RDbRowSet::TAccess aAccess)
	{
	const TDesC& des = aQuery.Query();
	TPckgC<TDbWindow> pckg(aWindow);
	TIpcArgs args(&des,aQuery.Comparison()|(aAccess<<8),&pckg);
	return CDbsCursor::NewL(iObject,EDbsDatabasePrepareView,args);
	}

CDbCursor* CDbsDatabase::OpenTableL(const TDesC& aName,RDbRowSet::TAccess aAccess)
	{
	TIpcArgs args(&aName,aAccess);
	return CDbsCursor::NewL(iObject,EDbsDatabaseOpenTable,args);
	}
