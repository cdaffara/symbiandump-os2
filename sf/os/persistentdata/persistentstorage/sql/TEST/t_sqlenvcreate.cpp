// Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// SQL security - copying secure test databases to C:
// Please, ensure that t_sqlenvcreate test is executed before the other sql tests
// 
//

#include <e32test.h>
#include <bautils.h>

RTest TheTest(_L("t_sqlenvcreate - copying secure test databases to C:"));

_LIT(KSqlServerPrivateDirC, "c:\\private\\10281e17\\");

_LIT(KDbZFileName1, "z:\\private\\10281e17\\[21212122]AADb2.db");
_LIT(KDbZFileName2, "z:\\private\\10281e17\\[21212122]BBDb2.db");
_LIT(KDbZFileName3, "z:\\private\\10281e17\\[21212125]T_AB.db");
_LIT(KDbZFileName4, "z:\\private\\10281e17\\[1111CCCC]T_AB.db");
_LIT(KDbZFileName5, "z:\\private\\10281e17\\[21212125]T_OneDefPolicy.db");

_LIT(KDbCFileName1, "c:\\private\\10281e17\\[21212122]AADb2.db");
_LIT(KDbCFileName2, "c:\\private\\10281e17\\[21212122]BBDb2.db");
_LIT(KDbCFileName3, "c:\\private\\10281e17\\[21212125]T_AB.db");
_LIT(KDbCFileName4, "c:\\private\\10281e17\\[1111CCCC]T_AB.db");
_LIT(KDbCFileName5, "c:\\private\\10281e17\\[21212125]T_OneDefPolicy.db");

_LIT(KDb1org, "z:\\private\\10281e17\\[98765432]t_nosettings.db");
_LIT(KDb1, "c:\\private\\10281e17\\[98765432]t_nosettings.db");

_LIT(KDb2org, "z:\\private\\10281e17\\[98765432]t_emptysettings.db");
_LIT(KDb2, "c:\\private\\10281e17\\[98765432]t_emptysettings.db");

_LIT(KDb3org, "z:\\private\\10281e17\\[98765432]t_invversion.db");
_LIT(KDb3, "c:\\private\\10281e17\\[98765432]t_invversion.db");

_LIT(KDb4org, "z:\\private\\10281e17\\[98765432]t_nocollation.db");
_LIT(KDb4, "c:\\private\\10281e17\\[98765432]t_nocollation.db");

_LIT(KDb5org, "z:\\private\\10281e17\\[98765432]t_longcollation.db");
_LIT(KDb5, "c:\\private\\10281e17\\[98765432]t_longcollation.db");

_LIT(KDb6org, "z:\\private\\10281e17\\[98765432]t_nodefaultpolicy.db");
_LIT(KDb6, "c:\\private\\10281e17\\[98765432]t_nodefaultpolicy.db");

_LIT(KDb7org, "z:\\private\\10281e17\\[98765432]t_invobject.db");
_LIT(KDb7, "c:\\private\\10281e17\\[98765432]t_invobject.db");

_LIT(KDb8org, "z:\\private\\10281e17\\[98765432]t_2defaultpolicies.db");
_LIT(KDb8, "c:\\private\\10281e17\\[98765432]t_2defaultpolicies.db");

_LIT(KPrivateSubDir, "c:\\private\\10281e17\\cfg-TestDir.db\\");

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
//Test macros and functions
static void Check(TInt aValue, TInt aLine)
	{
	if(!aValue)
		{
		RDebug::Print(_L("*** Line %d\r\n"), aLine);
		TheTest(EFalse, aLine);
		}
	}
static void Check(TInt aValue, TInt aExpected, TInt aLine)
	{
	if(aValue != aExpected)
		{
		RDebug::Print(_L("*** Expected error: %d, got: %d\r\n"), aExpected, aValue);
		TheTest(EFalse, aLine);
		}
	}
#define TEST(arg) ::Check((arg), __LINE__)
#define TEST2(aValue, aExpected) ::Check(aValue, aExpected, __LINE__)

///////////////////////////////////////////////////////////////////////////////////////

void DoRun()
	{
    RFs fs;
	TInt err = fs.Connect();
	TEST2(err, KErrNone);

	TheTest.Start(_L(" @SYMTestCaseID:SYSLIB-SQL-LEGACY-T_SQLENVCREATE-0001 Copy secure databases from Z: to C: "));

	//Create a subdir in the private datacage. The SQL production code should properly detects
	//KPrivateSubDir is a directory not a database file
	err = fs.MkDir(KPrivateSubDir);
	TEST(err == KErrNone || err == KErrAlreadyExists);
	
	err = BaflUtils::CopyFile(fs, KDbZFileName1, KDbCFileName1);
	TEST2(err, KErrNone);
	err = fs.SetAtt(KDbCFileName1, 0, KEntryAttReadOnly);
	TEST2(err, KErrNone);

	err = BaflUtils::CopyFile(fs, KDbZFileName2, KDbCFileName2);
	TEST2(err, KErrNone);
	err = fs.SetAtt(KDbCFileName2, 0, KEntryAttReadOnly);
	TEST2(err, KErrNone);

	err = BaflUtils::CopyFile(fs, KDbZFileName3, KDbCFileName3);
	TEST2(err, KErrNone);
	err = fs.SetAtt(KDbCFileName3, 0, KEntryAttReadOnly);
	TEST2(err, KErrNone);

	err = BaflUtils::CopyFile(fs, KDbZFileName4, KDbCFileName4);
	TEST2(err, KErrNone);
	err = fs.SetAtt(KDbCFileName4, 0, KEntryAttReadOnly);
	TEST2(err, KErrNone);

	err = BaflUtils::CopyFile(fs, KDbZFileName5, KDbCFileName5);
	TEST2(err, KErrNone);
	err = fs.SetAtt(KDbCFileName5, 0, KEntryAttReadOnly);
	TEST2(err, KErrNone);

	err = BaflUtils::CopyFile(fs, KDb1org, KDb1);
	TEST2(err, KErrNone);
	err = fs.SetAtt(KDb1, 0, KEntryAttReadOnly);
	TEST2(err, KErrNone);

	err = BaflUtils::CopyFile(fs, KDb2org, KDb2);
	TEST2(err, KErrNone);
	err = fs.SetAtt(KDb2, 0, KEntryAttReadOnly);
	TEST2(err, KErrNone);

	err = BaflUtils::CopyFile(fs, KDb3org, KDb3);
	TEST2(err, KErrNone);
	err = fs.SetAtt(KDb3, 0, KEntryAttReadOnly);
	TEST2(err, KErrNone);

	err = BaflUtils::CopyFile(fs, KDb4org, KDb4);
	TEST2(err, KErrNone);
	err = fs.SetAtt(KDb4, 0, KEntryAttReadOnly);
	TEST2(err, KErrNone);

	err = BaflUtils::CopyFile(fs, KDb5org, KDb5);
	TEST2(err, KErrNone);
	err = fs.SetAtt(KDb5, 0, KEntryAttReadOnly);
	TEST2(err, KErrNone);

	err = BaflUtils::CopyFile(fs, KDb6org, KDb6);
	TEST2(err, KErrNone);
	err = fs.SetAtt(KDb6, 0, KEntryAttReadOnly);
	TEST2(err, KErrNone);

	err = BaflUtils::CopyFile(fs, KDb7org, KDb7);
	TEST2(err, KErrNone);
	err = fs.SetAtt(KDb7, 0, KEntryAttReadOnly);
	TEST2(err, KErrNone);

	err = BaflUtils::CopyFile(fs, KDb8org, KDb8);
	TEST2(err, KErrNone);
	err = fs.SetAtt(KDb8, 0, KEntryAttReadOnly);
	TEST2(err, KErrNone);

	fs.Close();
	}

void CreateSqlServerPrivateDir()
    {
    RFs fs;
	TInt err = fs.Connect();
	TEST2(err, KErrNone);

	TRAP(err, BaflUtils::EnsurePathExistsL(fs, KSqlServerPrivateDirC));
	TEST2(err, KErrNone);

	fs.Close();
	}

TInt E32Main()
    {
	TheTest.Title();

	CTrapCleanup* tc = CTrapCleanup::New();

	__UHEAP_MARK;

	CreateSqlServerPrivateDir();

	DoRun();

	__UHEAP_MARKEND;

	TheTest.End();
	TheTest.Close();

	delete tc;

	User::Heap().Check();
	return KErrNone;
    }
