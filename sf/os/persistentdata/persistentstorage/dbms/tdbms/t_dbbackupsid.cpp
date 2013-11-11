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
// DBMS - Backup&Restore SID test.
// The test assume that drive C is presented in the system and is not a ROM drive.
// Please, ensure that t_predbsc test is executed before T_DBMSBackupSID test!
// Please, ensure that t_postdbsc test is executed after T_DBMSBackupSID test!
// 
//

#include <d32dbms.h>
#include <e32test.h>

//Global definitions
RTest 				TheTest(_L("t_dbbackupsid"));
RDbs 				TheDbs;

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
//Test macros and functions
static void Check(TInt aValue, TInt aLine)
	{
	if(!aValue)
		{
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
#define TEST(arg) 					Check((arg), __LINE__)
#define TEST2(aValue, aExpected) 	Check(aValue, aExpected, __LINE__)

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

/**
@SYMTestCaseID SYSLIB-DBMS-CT-0036
@SYMTestCaseDesc Backup&restore Secure ID test.
                 This function tests RDbs::GetBackupPath() with different sets of input
                 parameters: invalid and valid.
@SYMTestPriority High
@SYMTestActions  RDbs::GetBackupPath() test.
@SYMTestExpectedResults The test must not fail.
@SYMPREQ PREQ277
*/
static void TestBackupSID()
	{
	TBuf<KMaxPath> backupPath;
	//Invalid SID
	TSecureId backupSID = 0;
	TUid dbUid = {0x1133557A};
	_LIT(KDbName, "C:abcd.db");
	TheTest.Printf(_L("--Invalid backup SID: 0\n"));
	TInt err = TheDbs.GetBackupPath(backupSID, KDbName, dbUid, backupPath);
	TEST2(err, KErrArgument);
	backupSID = (TUint32)ECapability_None;
	TheTest.Printf(_L("--Invalid backup SID: ECapability_None\n"));
	err = TheDbs.GetBackupPath(backupSID, KDbName, dbUid, backupPath);
	TEST2(err, KErrArgument);
	//Invalid UID
	backupSID = 0x30003000;
	dbUid.iUid = 0xFB34010F;
	TheTest.Printf(_L("--Invalid UID: no security policy for it\n"));
	err = TheDbs.GetBackupPath(backupSID, KDbName, dbUid, backupPath);
	TEST2(err, KErrNotFound);
	dbUid.iUid = 0;
	TheTest.Printf(_L("--Invalid UID: 0\n"));
	err = TheDbs.GetBackupPath(backupSID, KDbName, dbUid, backupPath);
	TEST2(err, KErrArgument);
	//Invalid database name
	dbUid.iUid = 0x1133557A;
	TheTest.Printf(_L("--Null database name\n"));
	err = TheDbs.GetBackupPath(backupSID, KNullDesC, dbUid, backupPath);
	TEST2(err, KErrArgument);
	TheTest.Printf(_L("--Database name: no drive\n"));
	_LIT(KDbName2, "abcd.db");
	err = TheDbs.GetBackupPath(backupSID, KDbName2, dbUid, backupPath);
	TEST2(err, KErrArgument);
	//Valid arguments
	TheTest.Printf(_L("--Valid GetBackupPath() arguments\n"));
	err = TheDbs.GetBackupPath(backupSID, KDbName, dbUid, backupPath);
	TEST2(err, KErrNone);
	TheTest.Printf(_L("--Path: %S\n"), &backupPath);
	_LIT(KExpectedPath, "C:\\PRIVATE\\100012A5\\DBS_1133557A_ABCD.DB");
	TEST(backupPath.CompareF(KExpectedPath) == 0);
    //Valid arguments, but old format security policy file (without "backup" section)
	TSecureId backupSID3 = 1;
	TUid dbUid3 = {0x11335578};
	_LIT(KDbName3, "C:Z.db");
	TheTest.Printf(_L("--Valid GetBackupPath() arguments, old format file\n"));
	err = TheDbs.GetBackupPath(backupSID3, KDbName3, dbUid3, backupPath);
	TEST2(err, KErrPermissionDenied);
	}

//DEF063053  Additional method required on RDbs API for Secure Backup
static void DEF063053L()
	{
	//Invalid SID
	TheTest.Printf(_L("--Invalid backup SID: 0\n"));
	TSecureId backupSID = 0;
	TUid dbUid = {0x1133557A};
	CDbStrings* paths = NULL;
	TRAPD(err, paths = TheDbs.BackupPathsL(backupSID, dbUid));
	TEST2(err, KErrArgument);
	TEST(!paths);

	TheTest.Printf(_L("--Invalid backup SID: ECapability_None\n"));
	backupSID = (TUint32)ECapability_None;
	TRAP(err, paths = TheDbs.BackupPathsL(backupSID, dbUid));
	TEST2(err, KErrArgument);
	TEST(!paths);

	TheTest.Printf(_L("--Invalid backup SID: not matching the SID in security policy\n"));
	backupSID = (TUint32)0x109345FF;
	TRAP(err, paths = TheDbs.BackupPathsL(backupSID, dbUid));
	TEST2(err, KErrPermissionDenied);
	TEST(!paths);

	//Invalid UID
	TheTest.Printf(_L("--Invalid UID: no security policy for it\n"));
	backupSID = 0x30003000;
	dbUid.iUid = 0xFB34010F;
	TRAP(err, paths = TheDbs.BackupPathsL(backupSID, dbUid));
	TEST2(err, KErrArgument);
	TEST(!paths);

	TheTest.Printf(_L("--Invalid UID: 0\n"));
	dbUid.iUid = 0;
	TRAP(err, paths = TheDbs.BackupPathsL(backupSID, dbUid));
	TEST2(err, KErrArgument);
	TEST(!paths);

	//Valid arguments
	TheTest.Printf(_L("--Valid BackupPathL() arguments\n"));
	backupSID = 0x30003000;
	dbUid.iUid = 0x1133557A;
	TRAP(err, paths = TheDbs.BackupPathsL(backupSID, dbUid));
	TEST2(err, KErrNone);
	TEST(paths != NULL);
	for(TInt i=0;i<paths->Count();++i)
		{
		const TDesC& dd = (*paths)[i];
		RDebug::Print(_L("  Database %d: %S\r\n"), i + 1, &dd);
		}
	delete paths;
	paths = NULL;

	//Valid arguments, but no databases having the supplied security policy UID.
	TheTest.Printf(_L("--Valid BackupPathL() arguments, no databases\n"));
	backupSID = 0x30003001;
	dbUid.iUid = 0x11335577;
	TRAP(err, paths = TheDbs.BackupPathsL(backupSID, dbUid));
	TEST2(err, KErrNone);
	TEST(paths != NULL);
	TEST(paths->Count() == 0);
	delete paths;
	}

static void DoRunL()
	{
	TheTest.Start(_L(" @SYMTestCaseID:SYSLIB-DBMS-CT-0036 Backup&restore SID "));
	::TestBackupSID();

	TheTest.Next(_L("DEF063053 Additional method required on RDbs API for Secure Backup"));
	::DEF063053L();
	}

TInt E32Main()
    {
	__UHEAP_MARK;
	CTrapCleanup* tc = CTrapCleanup::New();
	TEST(tc != NULL);

	TheTest.Title();

	TInt err = TheDbs.Connect();
	TEST2(err, KErrNone);

	TRAP(err, ::DoRunL());
	TEST2(err, KErrNone);

	TheDbs.Close();

	TheTest.End();
	TheTest.Close();

	delete tc;

	__UHEAP_MARKEND;
	User::Heap().Check();
	return KErrNone;
    }
