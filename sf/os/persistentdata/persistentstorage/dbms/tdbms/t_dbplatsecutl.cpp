// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// DBMS security policy - testing new APIs - utility functions
// 
//

#include <e32test.h>
#include "t_dbplatsecutl.h"

//Supply your a pointer to your cleanup function or NULL.
TDBSCUtils::TDBSCUtils(RTest& aTest, TCleanupFunc aFunc) :
	iTest(aTest),
	iFunc(aFunc)
	{
	}

//Create a CDbColSet instance. The caller is responsible for deleting it.
CDbColSet* TDBSCUtils::CreateColSetLC(const TColDef* aColDef)
	{
	CDbColSet* set = CDbColSet::NewLC();
	for(;aColDef->iName;++aColDef)
		{
		TDbCol col(TPtrC(aColDef->iName), aColDef->iType);
		col.iAttributes = aColDef->iAttributes;
		set->AddL(col);
		}
	return set;
	}

//Create a CDbKey instance. The caller is responsible for deleting it.
CDbKey* TDBSCUtils::CreateKeyLC(const TDesC& aColName)
	{
	CDbKey* key = CDbKey::NewLC();
	key->AddL(aColName);
	return key;
	}

//Create a DBMS table.
void TDBSCUtils::CreateTableL(RDbNamedDatabase& aDb, const TDesC& aTblName, const TColDef* aColDef)
	{
	CDbColSet* colset = TDBSCUtils::CreateColSetLC(aColDef);
	TInt err = aDb.CreateTable(aTblName, *colset);
	TEST2(err, KErrNone);
	CleanupStack::PopAndDestroy(colset);
	}

// Create secure shared database. The caller is responsible for closing it.
RDbNamedDatabase TDBSCUtils::CreateDatabase(RDbs& aDbs, TUid aPolicyUid, const TDesC& aDbName)
	{
	_LIT(KSecure, "SECURE");
	TBuf<32> format;
	format.Copy(KSecure);
	format.Append(aPolicyUid.Name());
	RDbNamedDatabase db;
	TInt err = db.Create(aDbs, aDbName, format);
	TEST2(err, KErrNone);
	return db;
	}

// Delete secure shared database.
TInt TDBSCUtils::DeleteDatabase(RDbs& aDbs, TUid aPolicyUid, const TDesC& aDbName)
	{
	RDebug::Print(_L("Deleting %X \"%S\" database.\n"), aPolicyUid.iUid, &aDbName);
	TInt err = aDbs.DeleteDatabase(aDbName, aPolicyUid);
	if(err != KErrNone && err != KErrNotFound) 
		{
		RDebug::Print(_L("Error %d deleting \"%S\" database.\n"), err, &aDbName);
		}
	return err;
	}

//Check if a secure shared database with "aDatabaseName" name exists.
//aDatabaseName format: <name>.<ext>
TBool TDBSCUtils::IsDatabaseThereL(RDbs& aDbs, TUid aPolicyUid, TDriveNumber aDriveNumber, const TDesC& aDatabaseName)
	{
	CDbDatabaseNames* dbNames = aDbs.DatabaseNamesL(aDriveNumber, aPolicyUid);
	TEST(dbNames != NULL);
	TBool found = EFalse;
	for(TInt i=0;i<dbNames->Count();++i)
		{
		const TDesC& dbName = (*dbNames)[i];
		if(aDatabaseName.CompareF(dbName) == 0)
			{
			found = ETrue;
			break;
			}
		}
	delete dbNames;
	return found;
	}

//Check if the supplied aPolicy parameter has aCapability capability.
TBool TDBSCUtils::HasCapability(const TCompiledSecurityPolicy& aPolicy, TCapability aCapability)
	{
	TInt maxCount = 0;
	if(aPolicy.Type() == TSecurityPolicy::ETypeS3 || 
	   aPolicy.Type() == TSecurityPolicy::ETypeV3 ||
	   aPolicy.Type() == TSecurityPolicy::ETypeC3)
		{
		maxCount = 3;	
		}
	else if(aPolicy.Type() == TSecurityPolicy::ETypeC7)
		{
		maxCount = 7;	
		}
	else
		{
		TEST(0);
		}
	for(TInt i=0;i<maxCount;++i)
		{
		if(aPolicy.Capability(i) == aCapability)
			{
			return ETrue;	
			}
		}
	return EFalse;
	}

//If aValue is 0 then the method prints an error message and terminates the application.
//It will do a test cleanup before the termination, if iFunc data member is not null.
void TDBSCUtils::Check(TInt aValue, TInt aLine)
	{
	if(!aValue)
		{
		if(TheDbscUtils.iFunc)
			{
			TheDbscUtils.iFunc();
			}
		TheDbscUtils.iTest(EFalse, aLine);
		}
	}
	
//If aValue != aExpected then the method prints an error message and terminates the application.
//It will do a test cleanup before the termination, if iFunc data member is not null.
void TDBSCUtils::Check(TInt aValue, TInt aExpected, TInt aLine)
	{
	if(aValue != aExpected)
		{
		RDebug::Print(_L("*** Expected error: %d, got: %d\r\n"), aExpected, aValue);
		if(TheDbscUtils.iFunc)
			{
			TheDbscUtils.iFunc();
			}
		TheDbscUtils.iTest(EFalse, aLine);
		}
	}
