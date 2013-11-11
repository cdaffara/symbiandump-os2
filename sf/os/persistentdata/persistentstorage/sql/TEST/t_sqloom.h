// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef T_SQLOOM_H
#define T_SQLOOM_H

#include <e32test.h>
#include <bautils.h>
#include <sqldb.h>

extern RTest TheTest;

extern TInt TheHandleCount1B;
extern TInt TheHandleCount2B;
extern TInt TheAllocatedCellsCountB;

const TSecurityPolicy KDefaultPolicy(ECapability_None);
const TSecurityPolicy KPolicy1(ECapabilityTrustedUI, ECapabilityDiskAdmin);
const TSecurityPolicy KPolicy2(ECapabilityTrustedUI);
const TSecurityPolicy KPolicy3(ECapabilityDiskAdmin);

// These values have been found by performing each OOM test
// with an allocation limit of 1000 and then taking a value
// which is just above the allocation failure rate.
#if defined __WINS__ ||	defined __WINSCW__
const TInt KDoCreateDatabaseOomTestAllocLimitClient = 50;
const TInt KDoCreateDatabaseOomTestAllocLimitServer = 550;
const TInt KDoDbOomTestAllocLimitClient = 50;
const TInt KDoDbOomTestAllocLimitServer = 600;
const TInt KDoDbOomTest2AllocLimitClient = 50;
const TInt KDoDbOomTest2AllocLimitServer = 500;
const TInt KBlobOomTestAllocLimitClient = 20;
const TInt KBlobOomTestAllocLimitServer = 100;
const TInt KStmtOomTestAllocLimitClient = 20;
const TInt KStmtOomTestAllocLimitServer = 100;
#else	//The test running time is above the time limit (2400 sec) set for hardware testing
const TInt KDoCreateDatabaseOomTestAllocLimitClient = 50/2;
const TInt KDoCreateDatabaseOomTestAllocLimitServer = 550/2;
const TInt KDoDbOomTestAllocLimitClient = 50/2;
const TInt KDoDbOomTestAllocLimitServer = 600/2;
const TInt KDoDbOomTest2AllocLimitClient = 50/2;
const TInt KDoDbOomTest2AllocLimitServer = 500/2;
const TInt KBlobOomTestAllocLimitClient = 20;
const TInt KBlobOomTestAllocLimitServer = 100/2;
const TInt KStmtOomTestAllocLimitClient = 20;
const TInt KStmtOomTestAllocLimitServer = 100;
#endif

_LIT(KTestDir, "c:\\test\\");
_LIT(KTestDb, "c:\\test\\t_sqloom_1.db");
_LIT(KSecureTestDb, "c:[21212121]t_sqloom_2.db");
_LIT(KAttachDb, "c:\\test\\t_sqloom_3.db");
_LIT(KSecureAttachDb, "c:[21212121]t_sqloom_4.db");
_LIT(KSecureAttachDb2, "c:[1111CCCC]t_ab.db");//Created outside this test
_LIT(KSecureDb2, "C:[21212121]t_sqloom_6.db");
_LIT(KTestDb2, "C:\\TEST\\t_sqloom_7.db");
_LIT(KPrivateTestDb, "c:\\private\\21212121\\t_sqloom_8.db");

_LIT(KTableName1, "A_tbl");
_LIT(KTableName2, "B_table");
_LIT(KTableName3, "T_CCCCCCCCCCCCCC");

///////////////////////////////////////////////////////////////////////////////////////

enum TDbAction
	{
	ENotOpenDb,
	EOpenDb
	};

enum TDbType
	{
	ENonSecureDb,
	ESecureDb
	};

enum TOomTestType
	{
	EClientSideTest,
	EServerSideTest
	};

extern TOomTestType TheOomTestType[2];

///////////////////////////////////////////////////////////////////////////////////////

void DeleteTestFiles();
void CreateTestDir();
void PrepareAttachFromHandle();

void Check(TInt64 aValue, TInt aLine);
void Check(TInt64 aValue, TInt64 aExpected, TInt aLine);
#define TEST(arg) ::Check((arg), __LINE__)
#define TEST2(aValue, aExpected) ::Check(aValue, aExpected, __LINE__)

void MarkHandles();
void CheckHandles();
void MarkAllocatedCells();
void CheckAllocatedCells();

void PrintEndOfOomTest(TOomTestType aOomTestType, TInt aFailingAllocationNo);
void SetDbHeapFailure(TOomTestType aOomTestType, TInt aFailingAllocationNo, TBool aDelayed = EFalse);
void ResetDbHeapFailure(TOomTestType aOomTestType);
void SetHeapFailure(TOomTestType aOomTestType, TInt aFailingAllocationNo);
void ResetHeapFailure(TOomTestType aOomTestType);

typedef void (*TDbFuncPtrL)(RSqlDatabase& aDb, const TDesC& aDbFileName, TDbType aDbType);
typedef void (*TStmtFuncPtrL)(RSqlDatabase& aDb, RSqlStatement& aStmt);
typedef void (*TStmtFuncPtr2L)(RSqlStatement& aStmt);
typedef void (*TScalarFullSelectFuncPtrL)(RSqlDatabase& aDb);
typedef void (*TBlobPrepareFuncPtrL)(RSqlDatabase& aDb);
typedef void (*TBlobTestFuncPtrL)(RSqlDatabase& aDb, const TDesC& aAttachDbName);

void CreateTestSecurityPolicy(RSqlSecurityPolicy& aSecurityPolicy);
void CreateAttachDb();
void DbOomTestsL(TDbType aDbType);

#endif//T_SQLOOM_H
