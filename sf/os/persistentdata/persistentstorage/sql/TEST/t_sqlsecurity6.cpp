// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
//
#include <e32test.h>
#include <bautils.h>
#include <sqldb.h>

///////////////////////////////////////////////////////////////////////////////////////

RSqlDatabase TheDb;
RTest TheTest(_L("t_sqlsecurity6 test"));

///////////////////////////////////////////////////////////////////////////////////////

void DestroyTestEnv()
	{
	TheDb.Close();
	(void)RSqlDatabase::Delete(_L("c:[00009876]"));
	}

///////////////////////////////////////////////////////////////////////////////////////
//Test macros and functions
void Check1(TInt aValue, TInt aLine)
	{
	if(!aValue)
		{
		DestroyTestEnv();
		RDebug::Print(_L("*** Boolean expression evaluated to false. Line %d\r\n"), aLine);
		TheTest(EFalse, aLine);
		}
	}
void Check2(TInt aValue, TInt aExpected, TInt aLine)
	{
	if(aValue != aExpected)
		{
		DestroyTestEnv();
		RDebug::Print(_L("*** Line %d, Expected error: %d, got: %d\r\n"), aLine, aExpected, aValue);
		TheTest(EFalse, aLine);
		}
	}
#define TEST(arg) ::Check1((arg), __LINE__)
#define TEST2(aValue, aExpected) ::Check2(aValue, aExpected, __LINE__)

///////////////////////////////////////////////////////////////////////////////////////

/**
@SYMTestCaseID          PDS-SQL-CT-4199
@SYMTestCaseDesc        Invalid secure database UIDs test.
@SYMTestPriority        High
@SYMTestActions         The test attempts to create a secure database using invalid UIDs:
						- with length less than 8 hes digits;
						- with invalid characters in the UID;
						- wiht database name containing the UID only;
@SYMTestExpectedResults Test must not fail
@SYMDEF                 DEF145236
*/  
void InvalidSecureUidsTest()
	{
	RSqlSecurityPolicy policy;
	TInt err = policy.Create(TSecurityPolicy::EAlwaysPass);
	TEST2(err, KErrNone);
	
	//The UID in the database name is too short. It must be 8 hex digits UID. 
	err = TheDb.Create(_L("c:[9876]t_sqlsecurity6.db"), policy);
	TEST2(err, KErrArgument);
	
	//Short UID, used as a database name. 
	err = TheDb.Create(_L("c:[9876]"), policy);
	TEST2(err, KErrArgument);

	//Invalid UID. 
	err = TheDb.Create(_L("c:[KH0A0Q0J]"), policy);
	TEST2(err, KErrArgument);
	
	//UID, used as a database name. 
	err = TheDb.Create(_L("c:[00009876]"), policy);
	TEST2(err, KErrNone);
	err = TheDb.Exec(_L("CREATE TABLE A(I INTEGER)"));
	TEST(err >= 0);
	TheDb.Close();
	err = RSqlDatabase::Delete(_L("c:[00009876]"));
	TEST2(err, KErrNone);
	
	policy.Close();
	}

void DoTestsL()
	{
	TheTest.Start(_L("@SYMTestCaseID:PDS-SQL-CT-4199 Invalid secure UIDs test"));
	InvalidSecureUidsTest();
	}

TInt E32Main()
	{
	TheTest.Title();
	
	CTrapCleanup* tc = CTrapCleanup::New();
	TheTest(tc != NULL);
	
	__UHEAP_MARK;
		
	TRAPD(err, DoTestsL());
	DestroyTestEnv();
	TEST2(err, KErrNone);

	__UHEAP_MARKEND;
	
	TheTest.End();
	TheTest.Close();
	
	delete tc;

	User::Heap().Check();
	return KErrNone;
	}
