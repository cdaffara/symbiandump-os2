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

#ifndef T_DBPLATSECUTL_H
#define T_DBPLATSECUTL_H

#include <d32dbms.h>
#include "t_dbsecuritypolicy.h"

//Forward declarations
class RTest;

//Table column definition structure. 
struct TColDef
	{
	const TText*	iName;
	TDbColType		iType;
	TInt			iAttributes;
	};

typedef void (*TCleanupFunc)();

//Test utility class. Define TheDbscUtils instance somewhere in your test code.
class TDBSCUtils
	{
public:
	TDBSCUtils(RTest& aTest, TCleanupFunc aFunc);
	
	static CDbColSet* CreateColSetLC(const TColDef* aColDef);
	static CDbKey* CreateKeyLC(const TDesC& aColName);
	static void CreateTableL(RDbNamedDatabase& aDb, const TDesC& aTblName, const TColDef* aColDef);

	static RDbNamedDatabase CreateDatabase(RDbs& aDbs, TUid aPolicyUid, const TDesC& aDbName);
	static TInt DeleteDatabase(RDbs& aDbs, TUid aPolicyUid, const TDesC& aDbName);

	static TBool IsDatabaseThereL(RDbs& aDbs, TUid aPolicyUid, TDriveNumber aDriveNumber, const TDesC& aDatabaseName);
	static TBool HasCapability(const TCompiledSecurityPolicy& aPolicy, TCapability aCapability);

	static void Check(TInt aValue, TInt aLine);
	static void Check(TInt aValue, TInt aExpected, TInt aLine);

private:
	RTest&			iTest;
	TCleanupFunc	iFunc;	
	};

#define TEST(arg) TDBSCUtils::Check((arg), __LINE__)
#define TEST2(aValue, aExpected) TDBSCUtils::Check(aValue, aExpected, __LINE__)

extern TDBSCUtils TheDbscUtils;

#endif//T_DBPLATSECUTL_H
