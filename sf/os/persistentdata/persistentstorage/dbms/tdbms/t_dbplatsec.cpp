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
// DBMS security policy - testing new APIs
// All tests assume that drive C is presented in the system and is not a ROM drive.
// Please, ensure that t_dbenvcreate test is executed before t_dbplatsec<N>/t_dbplatsecperf tests!
// Please, ensure that t_dbenvdestroy test is executed after t_dbplatsec<N>/t_dbplatsecperf tests!
// 
//

#include <d32dbms.h>
#include <e32test.h>
#include "t_dbplatsecdef.h"
#include "t_dbplatsecutl.h"
#include "t_dbplatsecdb.h"
#include "t_dbplatsectbl.h"

//Global definitions
RTest 				TheTest(_L("t_dbplatsec : DBMS platform security testing"));
RDbs 				TheDbs;
RDbNamedDatabase 	TheDb1;
RDbNamedDatabase 	TheDb2;
RDbTable			TheTbl1;
RDbTable			TheTbl2;

void 		DeleteDatabases();
TDBSCUtils 	TheDbscUtils(TheTest, &DeleteDatabases);

void DeleteDatabases()
	{
	TheTbl1.Close();
	TheTbl2.Close();
	TheDb2.Close();
	TheDb1.Close();
	TDBSCUtils::DeleteDatabase(TheDbs, KSecureDbUid, KDb1Name);
	TDBSCUtils::DeleteDatabase(TheDbs, KSecureDbUid, KDb2Name);
	TDBSCUtils::DeleteDatabase(TheDbs, KSecureDbUid, KDb3Name);
	}

static void GetDatabaseRPolicyTest()
	{
	TSecurityPolicy policy;
	TInt err = TheDbs.GetDatabasePolicy(KSecureDbUid, RDbs::EReadPolicy, policy);
	TEST2(err, KErrNone);
	TCompiledSecurityPolicy testSecurityPolicy(policy);
	TSecurityPolicy::TType type = testSecurityPolicy.Type();
	TEST(type == TSecurityPolicy::ETypeC3);
	TEST(TDBSCUtils::HasCapability(testSecurityPolicy, ECapabilityCommDD));
	TEST(TDBSCUtils::HasCapability(testSecurityPolicy, ECapabilityPowerMgmt));
	TEST(TDBSCUtils::HasCapability(testSecurityPolicy, ECapabilityTrustedUI));
	}

static void GetDatabaseWPolicyTest()
	{
	TSecurityPolicy policy;
	TInt err = TheDbs.GetDatabasePolicy(KSecureDbUid, RDbs::EWritePolicy, policy);
	TEST2(err, KErrNone);
	TCompiledSecurityPolicy testSecurityPolicy(policy);
	TSecurityPolicy::TType type = testSecurityPolicy.Type();
	TEST(type == TSecurityPolicy::ETypeC3);
	TEST(TDBSCUtils::HasCapability(testSecurityPolicy, ECapabilityMultimediaDD));
	TEST(TDBSCUtils::HasCapability(testSecurityPolicy, ECapabilityReadDeviceData));
	TEST(TDBSCUtils::HasCapability(testSecurityPolicy, static_cast <TCapability> (TUint8(ECapability_None))));
	}

static void GetDatabaseSPolicyTest()
	{
	TSecurityPolicy policy;
	TInt err = TheDbs.GetDatabasePolicy(KSecureDbUid, RDbs::ESchemaPolicy, policy);
	TEST2(err, KErrNone);
	TCompiledSecurityPolicy testSecurityPolicy(policy);
	TSecurityPolicy::TType type = testSecurityPolicy.Type();
	TEST(type == TSecurityPolicy::ETypeC7);
	TEST(TDBSCUtils::HasCapability(testSecurityPolicy, ECapabilityWriteDeviceData));
	TEST(TDBSCUtils::HasCapability(testSecurityPolicy, ECapabilityDRM));
	TEST(TDBSCUtils::HasCapability(testSecurityPolicy, ECapabilityProtServ));
	TEST(TDBSCUtils::HasCapability(testSecurityPolicy, ECapabilityDiskAdmin));
	TEST(TDBSCUtils::HasCapability(testSecurityPolicy, ECapabilityNetworkControl));
	TEST(TDBSCUtils::HasCapability(testSecurityPolicy, ECapabilityWriteUserData));
	TEST(TDBSCUtils::HasCapability(testSecurityPolicy, ECapabilitySwEvent));
	}

static void GetTablePolicyTest(const TDesC& aTableName,
							   RDbs::TPolicyType aPolicyType,
							   TSecurityPolicy::TType aType,
							   TSecureId aSecureId,
							   TVendorId aVendorId,
							   const TCapability* aCapabilities, TInt aCapCount)
	{
	TSecurityPolicy policy;
	TInt err = TheDbs.GetTablePolicy(KSecureDbUid, aTableName, aPolicyType, policy);
	TEST2(err, KErrNone);
	TCompiledSecurityPolicy testSecurityPolicy(policy);
	TEST(testSecurityPolicy.Type() == aType);
	for(TInt i=0;i<aCapCount;++i)
		{
		TEST(TDBSCUtils::HasCapability(testSecurityPolicy, aCapabilities[i]));
		}
	if(testSecurityPolicy.Type() == TSecurityPolicy::ETypeS3)
		{
		TEST(testSecurityPolicy.SecureId() == aSecureId);
		}
	else if(testSecurityPolicy.Type() == TSecurityPolicy::ETypeV3)
		{
		TEST(testSecurityPolicy.VendorId() == aVendorId);
		}
	}

//Tests RDbs::GetDatabasePolicy()/RDbs::GetTablePolicy() with invalid arguments
static void InvalidPolicyArgTestsL()
	{
	TheTest.Printf(_L("--Invalid UID\n"));
	const TUid KInvalidUid = {0x490122};
	TSecurityPolicy policy;
	TInt err = TheDbs.GetDatabasePolicy(KInvalidUid, RDbs::EWritePolicy, policy);
	TEST2(err, KErrArgument);
	err = TheDbs.GetDatabasePolicy(KNullUid, RDbs::EWritePolicy, policy);
	TEST2(err, KErrArgument);
	err = TheDbs.GetTablePolicy(KInvalidUid, KTableName1, RDbs::EWritePolicy, policy);
	TEST2(err, KErrArgument);
	err = TheDbs.GetTablePolicy(KNullUid, KTableName1, RDbs::EWritePolicy, policy);
	TEST2(err, KErrArgument);

	TheTest.Printf(_L("--Invalid policy type\n"));
	err = TheDbs.GetDatabasePolicy(KSecureDbUid, static_cast <RDbs::TPolicyType> (100), policy);
	TEST2(err, KErrArgument);
	err = TheDbs.GetTablePolicy(KSecureDbUid, KTableName2, static_cast <RDbs::TPolicyType> (101), policy);
	TEST2(err, KErrArgument);

	TheTest.Printf(_L("--Table name, which is not in the policy file\n"));
	_LIT(KMissingTableName, "AAAAAA");
	err = TheDbs.GetTablePolicy(KSecureDbUid, KMissingTableName, RDbs::EWritePolicy, policy);
	TEST2(err, KErrNone);//This must be the related database policy
	TSecurityPolicy dbPolicy;
	err = TheDbs.GetDatabasePolicy(KSecureDbUid, RDbs::EWritePolicy, dbPolicy);
	TEST2(err, KErrNone);
	TCompiledSecurityPolicy sp1(policy);
	TCompiledSecurityPolicy sp2(dbPolicy);
	TEST(sp1 == sp2);

	TheTest.Printf(_L("--Table name - zero length\n"));
	_LIT(KInvTableName, "");
	err = TheDbs.GetTablePolicy(KSecureDbUid, KInvTableName, RDbs::EWritePolicy, policy);
	TEST2(err, KErrArgument);

	TheTest.Printf(_L("--Table name - Schema policy request\n"));
	err = TheDbs.GetTablePolicy(KSecureDbUid, KTableName3, RDbs::ESchemaPolicy, policy);
	TEST2(err, KErrNotSupported);
	}

//Returns a valid drive number, but not presented in the system
static TDriveNumber FirstNotPresentedDriveNumber()
	{
	RFs fs;
	TEST2(fs.Connect(), KErrNone);
	TDriveNumber drvNumber = static_cast <TDriveNumber> (1000);
	for(TInt i=EDriveA;i<=EDriveZ;++i)
		{
		TDriveInfo driveInfo;
		if(fs.Drive(driveInfo, static_cast <TDriveNumber> (i)) == KErrNone)
			{
			if(driveInfo.iType == EMediaNotPresent)
				{
				drvNumber = static_cast <TDriveNumber> (i);
				break;
				}
			}
		}
	fs.Close();
	TEST(drvNumber <= EDriveZ);
	return drvNumber;
	}

//Returns the first valid ROM drive number.
static TDriveNumber FirstROMDriveNumber()
	{
	RFs fs;
	TEST2(fs.Connect(), KErrNone);
	TDriveNumber drvNumber = static_cast <TDriveNumber> (1000);
	for(TInt i=EDriveA;i<=EDriveZ;++i)
		{
		TDriveInfo driveInfo;
		if(fs.Drive(driveInfo, static_cast <TDriveNumber> (i)) == KErrNone)
			{
			if(driveInfo.iDriveAtt & KDriveAttRom)
				{
				drvNumber = static_cast <TDriveNumber> (i);
				break;
				}
			}
		}
	fs.Close();
	TEST(drvNumber <= EDriveZ);
	return drvNumber;
	}

//Tests RDbs::DatabaseNamesL() with invalid arguments
static void InvalidPolicyArgTests2L()
	{
	CDbDatabaseNames* dbNames = NULL;

	TheTest.Printf(_L("--Invalid UID\n"));
	const TUid KInvalidUid = {0x490122};
	TRAPD(err, dbNames = TheDbs.DatabaseNamesL(EDriveC, KInvalidUid));
	TEST2(err, KErrArgument);
	TEST(dbNames == NULL);

	TheTest.Printf(_L("--Null UID\n"));
	TRAP(err, dbNames = TheDbs.DatabaseNamesL(EDriveC, KNullUid));
	TEST2(err, KErrArgument);
	TEST(dbNames == NULL);

	TheTest.Printf(_L("--Invalid drive\n"));
	TRAP(err, dbNames = TheDbs.DatabaseNamesL(static_cast <TDriveNumber> (-1), KSecureDbUid));
	TEST2(err, KErrBadName);
	TEST(dbNames == NULL);
	TRAP(err, TheDbs.DatabaseNamesL(static_cast <TDriveNumber> (1765), KSecureDbUid));
	TEST2(err, KErrBadName);
	TEST(dbNames == NULL);

	TheTest.Printf(_L("--Valid drive number, but not presented in the system\n"));
	TDriveNumber driveNumber = ::FirstNotPresentedDriveNumber();
	TRAP(err, dbNames = TheDbs.DatabaseNamesL(driveNumber, KSecureDbUid));
	TEST2(err, KErrNotReady);
	TEST(dbNames == NULL);
	}

/**
@SYMTestCaseID SYSLIB-DBMS-CT-0003
@SYMTestCaseDesc Test RDbs::GetDatabasePolicy(), RDbs::GetTablePolicy()
                 getting database or table security policies.
@SYMTestPriority High
@SYMTestActions  A call with valid arguments and result check.
				 A call with an invalid UID.
				 A call with an invalid security policy type.
				 A call with a table name, not presented in the policy file.
				 A call requesting schema security policy of a table.
@SYMTestExpectedResults The test must not fail.
@SYMREQ          REQ2429
                 DBMS shall provide an API to apply security policies to database tables.
*/
static void GetPolicyTestL()
	{
	TheTest.Printf(_L("--Simple GetDatabasePolicy() tests\n"));
	::GetDatabaseRPolicyTest();
	::GetDatabaseWPolicyTest();
	::GetDatabaseSPolicyTest();

	::InvalidPolicyArgTestsL();

	TheTest.Printf(_L("--Simple GetTablePolicy() tests\n"));
	TCapability caps1[] = {ECapabilityCommDD, ECapabilityPowerMgmt, ECapabilityTrustedUI};
	TCapability caps2[] = {ECapabilityMultimediaDD, ECapabilityReadDeviceData};
	::GetTablePolicyTest(KTableName1, RDbs::EReadPolicy, TSecurityPolicy::ETypeC3, 0, 0, caps1, 3);
	::GetTablePolicyTest(KTableName1, RDbs::EWritePolicy, TSecurityPolicy::ETypeC3, 0, 0, caps2, 2);

	TCapability caps3[] = {static_cast <TCapability> (TUint8(ECapability_None))};
	::GetTablePolicyTest(KTableName2, RDbs::EReadPolicy, TSecurityPolicy::ETypeC3, 0, 0, caps3, 1);
	::GetTablePolicyTest(KTableName2, RDbs::EWritePolicy, TSecurityPolicy::ETypeC3, 0, 0, caps2, 2);

	TCapability caps4[] = {ECapabilityReadUserData};
	::GetTablePolicyTest(KTableName3, RDbs::EReadPolicy, TSecurityPolicy::ETypeC3, 0, 0, caps1, 3);
	::GetTablePolicyTest(KTableName3, RDbs::EWritePolicy, TSecurityPolicy::ETypeC3, 0, 0, caps4, 1);

	TCapability caps5[] = {ECapabilityWriteUserData};
	::GetTablePolicyTest(KTableName4, RDbs::EReadPolicy, TSecurityPolicy::ETypeC3, 0, 0, caps5, 1);
	::GetTablePolicyTest(KTableName4, RDbs::EWritePolicy, TSecurityPolicy::ETypeC3, 0, 0, caps2, 2);
	}

/**
@SYMTestCaseID SYSLIB-DBMS-CT-0004
@SYMTestCaseDesc Test RDbs::DatabaseNamesL(TDriveNumber, const TDesC&, TUid) -
                 getting a list of secure shared databases.
@SYMTestPriority High
@SYMTestActions  A call with valid arguments and result check
				 A call with an invalid drive number.
				 A call with a valid, but not presented in the system, drive number.
				 A call with an invalid UID.
@SYMTestExpectedResults The test must not fail.
@SYMREQ          REQ2429
                 DBMS shall provide an API to apply security policies to database tables.
*/
static void GetDatabaseListTestL()
	{
	TDBSCUtils::DeleteDatabase(TheDbs, KSecureDbUid, KDb1Name);
	TDBSCUtils::DeleteDatabase(TheDbs, KSecureDbUid, KDb2Name);

	TheDb1 = TDBSCUtils::CreateDatabase(TheDbs, KSecureDbUid, KDb1Name);
	TheDb1.Close();

	TheDb2 = TDBSCUtils::CreateDatabase(TheDbs, KSecureDbUid, KDb2Name);
	TheDb2.Close();

	TBuf<32> dbName;
	dbName.Copy(KDb1Name().Mid(KDatabaseNamePos));
	TEST(TDBSCUtils::IsDatabaseThereL(TheDbs, KSecureDbUid, EDriveC, dbName));
	dbName.Copy(KDb2Name().Mid(KDatabaseNamePos));
	TEST(TDBSCUtils::IsDatabaseThereL(TheDbs, KSecureDbUid, EDriveC, dbName));

	TheTest.Printf(_L("--An attempt to retrieve the list of database names for an UID, which security policy cannot be satisfied by the current test application\n"));
	CDbDatabaseNames* dbNames = NULL;
	TRAPD(err, dbNames = TheDbs.DatabaseNamesL(EDriveZ, KSecureDbUid2));
	delete dbNames;
	TEST2(err, KErrNone);

	::InvalidPolicyArgTests2L();
	}

/**
@SYMTestCaseID SYSLIB-DBMS-CT-0005
@SYMTestCaseDesc Test RDbs::CopyDatabase(const TDesC&, const TDesC&, TUid) -
                 copying secure shared database .
@SYMTestPriority High
@SYMTestActions  A call with valid arguments and result check
                 A call with an invalid source database name.
				 A call with an invalid target database name.
				 A call with an invalid UID.
				 An attempt to copy an opened database.
				 An attempt to copy a database, which security policy cannot be satisfied.
				 An attempt to copy a database with path specified in the database name.
				 An attempt to copy non-existing database.
@SYMTestExpectedResults The test must not fail.
@SYMREQ          REQ2429
                 DBMS shall provide an API to apply security policies to database tables.
*/
static void CopyDatabaseTestL()
	{
	TDBSCUtils::DeleteDatabase(TheDbs, KSecureDbUid, KDb1Name);
	TheDb1 = TDBSCUtils::CreateDatabase(TheDbs, KSecureDbUid, KDb1Name);
	TheDb1.Close();

	TheTest.Printf(_L("--Simple copy test\n"));
	TDBSCUtils::DeleteDatabase(TheDbs, KSecureDbUid, KDb3Name);
	TInt err = TheDbs.CopyDatabase(KDb1Name, KDb3Name, KSecureDbUid);
	TEST2(err, KErrNone);
	TBuf<32> dbName;
	dbName.Copy(KDb3Name().Mid(KDatabaseNamePos));
	TEST(TDBSCUtils::IsDatabaseThereL(TheDbs, KSecureDbUid, EDriveC, dbName));
	TDBSCUtils::DeleteDatabase(TheDbs, KSecureDbUid, KDb3Name);

	TheTest.Printf(_L("--An attempt to copy a source database, which security policy cannot be satisfied by the current test application\n"));
	err = TheDbs.CopyDatabase(KProtDb1ZName, KDb3Name, KSecureDbUid2);
	TEST2(err, KErrPermissionDenied);

	TheTest.Printf(_L("--Null source database name\n"));
	err = TheDbs.CopyDatabase(KNullDesC, KDb3Name, KSecureDbUid);
	TEST2(err, KErrArgument);
	TDBSCUtils::DeleteDatabase(TheDbs, KSecureDbUid, KDb3Name);

	TheTest.Printf(_L("--Source database name with not presented drive number\n"));
	TDriveNumber driveNumber = ::FirstNotPresentedDriveNumber();
	TChar driveChar;
	TEST2(RFs::DriveToChar(driveNumber, driveChar), KErrNone);
	TBuf<KDatabaseNamePos> drvBuf;
	drvBuf.Append(driveChar);
	drvBuf.Append(':');
	dbName.Copy(drvBuf);
	dbName.Append(KDb1Name().Mid(KDatabaseNamePos));
	err = TheDbs.CopyDatabase(dbName, KDb3Name, KSecureDbUid);
	TEST2(err, KErrNotReady);

	TheTest.Printf(_L("--Source database name - only with a drive letter and \":\"\n"));
	err = TheDbs.CopyDatabase(drvBuf, KDb3Name, KSecureDbUid);
	TEST2(err, KErrNotReady);

	TheTest.Printf(_L("--Source database name - only with a drive letter\n"));
	drvBuf.SetLength(1);
	err = TheDbs.CopyDatabase(drvBuf, KDb3Name, KSecureDbUid);
	TEST2(err, KErrArgument);

	TheTest.Printf(_L("--Null destination database name\n"));
	err = TheDbs.CopyDatabase(KDb1Name, KNullDesC, KSecureDbUid);
	TEST2(err, KErrArgument);

	TheTest.Printf(_L("--Destination database name with not presented drive number\n"));
	drvBuf.Append(':');
	dbName.Copy(drvBuf);
	dbName.Append(KDb3Name().Mid(KDatabaseNamePos));
	err = TheDbs.CopyDatabase(KDb1Name, dbName, KSecureDbUid);
	TEST2(err, KErrNotReady);

	TheTest.Printf(_L("--Destination database - ROM drive\n"));
	TDriveNumber romDriveNumber = ::FirstROMDriveNumber();
	TEST2(RFs::DriveToChar(romDriveNumber, driveChar), KErrNone);
	drvBuf[0] = static_cast <TText> (driveChar);
	dbName.Copy(drvBuf);
	dbName.Append(KDb3Name().Mid(KDatabaseNamePos));
	err = TheDbs.CopyDatabase(KDb1Name, dbName, KSecureDbUid);
	TEST2(err, KErrAccessDenied);

	TheTest.Printf(_L("--A source database with a valid name, but not presented in the system\n"));
	_LIT(KMissingDb, "C:asdhasfsudfygusdfuierfb.db");
	err = TheDbs.CopyDatabase(KMissingDb, KDb3Name, KSecureDbUid);
	TEST2(err, KErrNotFound);

	TheTest.Printf(_L("--A test with an open source database\n"));
	TDBSCUtils::DeleteDatabase(TheDbs, KSecureDbUid, KDb1Name);
	TheDb2 = TDBSCUtils::CreateDatabase(TheDbs, KSecureDbUid, KDb1Name);
	err = TheDbs.CopyDatabase(KDb1Name, KDb3Name, KSecureDbUid);
	TheDb2.Close();
	TEST2(err, KErrInUse);

	TheTest.Printf(_L("--An atempt to copy over an existing database\n"));
	TDBSCUtils::DeleteDatabase(TheDbs, KSecureDbUid, KDb2Name);
	TheDb1 = TDBSCUtils::CreateDatabase(TheDbs, KSecureDbUid, KDb2Name);
	TheDb1.Close();
	err = TheDbs.CopyDatabase(KDb1Name, KDb2Name, KSecureDbUid);
	TEST2(err, KErrAlreadyExists);

	TheTest.Printf(_L("--Too long destination database name > KDbMaxName symbols\n"));
	_LIT(KTooLongDbName, "A_0123456789_0123456789_0123456789_0123456789_0123456789_0123456789_0123456789.db");
	err = TheDbs.CopyDatabase(KDb1Name, KTooLongDbName, KSecureDbUid);
	TEST2(err, KErrArgument);

	TheTest.Printf(_L("--Null UID\n"));
	err = TheDbs.CopyDatabase(KDb1Name, KDb3Name, KNullUid);
	TEST2(err, KErrArgument);

	TheTest.Printf(_L("--Invalid UID\n"));
	const TUid KInvalidUid = {0x490122};
	err = TheDbs.CopyDatabase(KDb1Name, KDb3Name, KInvalidUid);
	TEST2(err, KErrArgument);

	TheTest.Printf(_L("--An attempt to copy a database with the path specified in the name\n"));
	TDBSCUtils::DeleteDatabase(TheDbs, KSecureDbUid, KDb2Name);
	_LIT(KSrcDbName2, "C:\\a.Db");
	_LIT(KDestDbName2, "C:\\b.dB");
	err = TheDbs.CopyDatabase(KSrcDbName2, KDestDbName2, KSecureDbUid);
	TEST2(err, KErrArgument);
	}

/**
@SYMTestCaseID SYSLIB-DBMS-CT-0006
@SYMTestCaseDesc Test RDbs::Delete(const TDesC&, TUid) - deleting secure shared database .
@SYMTestPriority High
@SYMTestActions  A call with valid arguments and result check
                 A call with an invalid UID.
				 A call with an invalid database name.
				 An attempt to delete an opened database.
				 An attempt to delete a database, which security policy cannot be satisfied.
				 An attempt to delete a database with path specified in the database name.
				 An attempt to delete non-existing database.
@SYMTestExpectedResults The test must not fail.
@SYMREQ          REQ2429
                 DBMS shall provide an API to apply security policies to database tables.
*/
static void DeleteDatabaseTestL()
	{
	TDBSCUtils::DeleteDatabase(TheDbs, KSecureDbUid, KDb1Name);
	TheDb1 = TDBSCUtils::CreateDatabase(TheDbs, KSecureDbUid, KDb1Name);
	TheDb1.Close();

	TheTest.Printf(_L("--Simple delete database test\n"));
	TInt err = TheDbs.DeleteDatabase(KDb1Name, KSecureDbUid);
	TEST2(err, KErrNone);

	TheTest.Printf(_L("--Null UID\n"));
	err = TheDbs.DeleteDatabase(KDb1Name, KNullUid);
	TEST2(err, KErrArgument);

	TheTest.Printf(_L("--Invalid UID\n"));
	const TUid KInvalidUid = {0x490122};
	err = TheDbs.DeleteDatabase(KDb1Name, KInvalidUid);
	TEST2(err, KErrArgument);

	TheTest.Printf(_L("--Null database name\n"));
	err = TheDbs.DeleteDatabase(KNullDesC, KSecureDbUid);
	TEST2(err, KErrArgument);

	TheTest.Printf(_L("--Non-existing database - no drive letter (the name is invalid)\n"));
	_LIT(KTooLongDbName, "A_0123456789_0123456789_0123456789_0123456789_0123456789_0123456789_0123456789.db");
	err = TheDbs.DeleteDatabase(KTooLongDbName, KSecureDbUid);
	TEST2(err, KErrArgument);
	_LIT(KInvDbName1, "UI789.db");
	err = TheDbs.DeleteDatabase(KInvDbName1, KSecureDbUid);
	TEST2(err, KErrArgument);

	TheTest.Printf(_L("--Non-existing database - valid db name\n"));
	_LIT(KMissingDbName1, "C:A0192345.db");
	err = TheDbs.DeleteDatabase(KMissingDbName1, KSecureDbUid);
	TEST2(err, KErrNotFound);

	TheTest.Printf(_L("--An attempt to delete a database with the path specified in the name\n"));
	_LIT(KMissingDbName2, "C:\\TEMP\\A0192345.db");
	err = TheDbs.DeleteDatabase(KMissingDbName2, KSecureDbUid);
	TEST2(err, KErrArgument);

	TheTest.Printf(_L("--An attempt to delete a database, which security policy cannot be satisfied by the current test application\n"));
	err = TheDbs.DeleteDatabase(KProtDb1CName, KSecureDbUid2);
	TEST2(err, KErrPermissionDenied);

	TheTest.Printf(_L("--An attempt to delete an open database\n"));
	TDBSCUtils::DeleteDatabase(TheDbs, KSecureDbUid, KDb1Name);
	TheDb1 = TDBSCUtils::CreateDatabase(TheDbs, KSecureDbUid, KDb1Name);
	err = TheDbs.DeleteDatabase(KDb1Name, KSecureDbUid);
	TEST2(err, KErrInUse);
	TheDb1.Close();
	}

/**
@SYMTestCaseID SYSLIB-DBMS-CT-0007
@SYMTestCaseDesc Test RDbNamedDatabase::Create(RDbs&, const TDesC&, const TDesC&) -
                 secure shared database creation.
@SYMTestPriority High
@SYMTestActions  A call with valid arguments and result check
                 An attempt to create already existing database.
				 An attempt to create already existing and opened database.
				 RDbNamedDatabase::Create() called with an invalid format string.
				 RDbNamedDatabase::Create() called with a database name containing '\' symbols.
				 An attempt to create a database, which security policy cannot be satisfied.
@SYMTestExpectedResults The test must not fail.
@SYMREQ          REQ2429
                 DBMS shall provide an API to apply security policies to database tables.
*/
static void CreateDatabaseL()
	{
	TheTest.Printf(_L("--Simple create database test\n"));
	TDBSCUtils::DeleteDatabase(TheDbs, KSecureDbUid, KDb1Name);
	TBuf<32> format;
	format.Copy(KSecure);
	format.Append(KSecureDbUid.Name());
	TInt err = TheDb1.Create(TheDbs, KDb1Name, format);
	TEST2(err, KErrNone);

	TheTest.Printf(_L("--An atempt to recreate an open database\n"));
	err = TheDb1.Create(TheDbs, KDb1Name, format);
	TEST2(err, KErrAlreadyExists);
	TheDb1.Close();

	TheTest.Printf(_L("--An atempt to recreate an existing database\n"));
	err = TheDb1.Create(TheDbs, KDb1Name, format);
	TEST2(err, KErrAlreadyExists);

	TheTest.Printf(_L("--Format string with \"SECURE\" keyword and some other words too\n"));
	TDBSCUtils::DeleteDatabase(TheDbs, KSecureDbUid, KDb1Name);
	format.Copy(_L("A "));
	format.Append(KSecure);
	format.Append(_L(" B"));
	format.Append(KSecureDbUid.Name());
	err = TheDb1.Create(TheDbs, KDb1Name, format);
	TheDb1.Close();
	TEST2(err, KErrNotSupported);

	TheTest.Printf(_L("--Format string with \"SeCuRe\" keyword\n"));
	TDBSCUtils::DeleteDatabase(TheDbs, KSecureDbUid, KDb1Name);
	format.Copy(_L("SeCuRe"));
	format.Append(KSecureDbUid.Name());
	err = TheDb1.Create(TheDbs, KDb1Name, format);
	TheDb1.Close();
	TEST2(err, KErrNone);

	TheTest.Printf(_L("--Format string with \"SECURED\" keyword\n"));
	TDBSCUtils::DeleteDatabase(TheDbs, KSecureDbUid, KDb1Name);
	format.Copy(_L("SECURED"));
	format.Append(KSecureDbUid.Name());
	err = TheDb1.Create(TheDbs, KDb1Name, format);
	TheDb1.Close();
	TEST2(err, KErrNotSupported);

	TheTest.Printf(_L("--Format string with a null uid\n"));
	TDBSCUtils::DeleteDatabase(TheDbs, KSecureDbUid, KDb1Name);
	format.Copy(_L("SECURE"));
	format.Append(KNullUid.Name());
	err = TheDb1.Create(TheDbs, KDb1Name, format);
	TheDb1.Close();
	TEST2(err, KErrArgument);

	TheTest.Printf(_L("--Format string with an invalid uid\n"));
	TDBSCUtils::DeleteDatabase(TheDbs, KSecureDbUid, KDb1Name);
	format.Copy(_L("SECURE"));
	const TUid KInvalidUid = {0x490122};
	format.Append(KInvalidUid.Name());
	err = TheDb1.Create(TheDbs, KDb1Name, format);
	TheDb1.Close();
	TEST2(err, KErrArgument);

	TheTest.Printf(_L("--Format string with a uid only\n"));
	TDBSCUtils::DeleteDatabase(TheDbs, KSecureDbUid, KDb1Name);
	format.Copy(KSecureDbUid.Name());
	err = TheDb1.Create(TheDbs, KDb1Name, format);
	TheDb1.Close();
	TEST2(err, KErrNotSupported);

	TheTest.Printf(_L("--Blank format string\n"));
	TDBSCUtils::DeleteDatabase(TheDbs, KSecureDbUid, KDb1Name);
	format.Zero();
	err = TheDb1.Create(TheDbs, KDb1Name, format);
	TheDb1.Close();
	TEST2(err, KErrNotSupported);

	TheTest.Printf(_L("--Valid format string + database name with a path\n"));
	TDBSCUtils::DeleteDatabase(TheDbs, KSecureDbUid, KDb1Name);
	format.Copy(KSecure);
	format.Append(KSecureDbUid.Name());
	_LIT(KSrcDbName2, "C:\\a.Db");
	err = TheDb1.Create(TheDbs, KSrcDbName2, format);
	TheDb1.Close();
	TEST2(err, KErrArgument);

	TheTest.Printf(_L("--An attempt to create a database, which security policy cannot be satisfied by the current test application\n"));
	format.Copy(KSecure);
	format.Append(KSecureDbUid2.Name());
	err = TheDb1.Create(TheDbs, KProtDb2CName, format);
	TheDb1.Close();
	TEST2(err, KErrPermissionDenied);
	}

static void INC057327L()
    {
     TRAPD(err,delete TheDbs.DatabaseNamesL(EDriveZ, TUid::Uid(0x12344321)));
     TheTest(err == KErrNone);
    }

static void DoRunL()
	{
	TheTest.Start(_L(" @SYMTestCaseID:SYSLIB-DBMS-CT-0003 Get database/table policies "));
	::GetPolicyTestL();

	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-DBMS-CT-0004 Get databases list "));
	::GetDatabaseListTestL();

	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-DBMS-CT-0005 Copy database "));
	::CopyDatabaseTestL();

	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-DBMS-CT-0006 Delete database "));
	::DeleteDatabaseTestL();

	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-DBMS-CT-0007 Create database "));
	::CreateDatabaseL();

	TheTest.Next(_L("Database security"));
	::DoDbTestL();

	TheTest.Next(_L("Table security"));
	::DoTblTestL();

	TheTest.Next(_L("Test for INC057327"));
	::INC057327L();
	}

TInt E32Main()
    {
	__UHEAP_MARK;
	CTrapCleanup* tc = CTrapCleanup::New();
	TEST(tc != NULL);

	TInt err = TheDbs.Connect();
	TEST2(err, KErrNone);

	TRAP(err, ::DoRunL());
	TEST2(err, KErrNone);

	::DeleteDatabases();
	TheDbs.Close();

	TheTest.End();
	TheTest.Close();

	delete tc;

	__UHEAP_MARKEND;
	User::Heap().Check();
	return KErrNone;
    }
