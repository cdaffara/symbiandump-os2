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
//

#include <e32test.h>
#include <bautils.h>
#include <sqldb.h>
#include <e32math.h>

#include "SqlBur.h"
#include "t_sqlbur.h"

///////////////////////////////////////////////////////////////////////////////////////

RTest TheTest(_L("SQL Backup and Restore Test"));

_LIT(KPrivateDir, "\\private\\10281e17\\");

//Don't forget to update DeleteBackupFiles() implementation if a new uid is added
const TSecureId KClientUid = 0x21212122; // the data owner's UID
const TSecureId KZeroFileSizeUid = 0xFFFF4321;
const TSecureId KTestClientUid1 = 0xFFFF4322;
const TSecureId KTestClientUid2 = 0xFFFF4323;
const TSecureId KTestClientUid3 = 0xFFFF4324;

_LIT(KBackupDir, "C:\\TEST\\");
_LIT(KBackupFileTemplate, "C:\\TEST\\Backup.bak");
_LIT(KBackupCopy, "C:\\TEST\\Backup2.bak");
_LIT(KBackupFile2Z, "Z:\\TEST\\t_sqlbur_backup_ver0.bak");
_LIT(KBackupFile2, "C:\\TEST\\t_sqlbur_backup_ver0.bak");

_LIT(KZeroSizeFile, "C:\\private\\10281e17\\[FFFF4321]t_sqlbur_zero.db");
_LIT(KTestFile1, "C:\\private\\10281e17\\[FFFF4322]t_sqlbur_test1.db");
_LIT(KTestFile1NameOnly, "[FFFF4322]t_sqlbur_test1.db");
_LIT(KTestFile1Bak, "C:\\private\\10281e17\\bak[FFFF4322]t_sqlbur_test1.db.bak");
_LIT(KTestDeleteMask1, "C:\\private\\10281e17\\[FFFF4322]*");
_LIT(KTestDeleteMask2, "*bur_test1.db");
_LIT(KTestFile2, "\\private\\10281e17\\[FFFF4323]t_sqlbur_test2.db");
_LIT(KTestFile3, "c:\\private\\10281e17\\[FFFF4324]t_sqlbur_test3.db");
_LIT(KTestFile4, "c:\\private\\10281e17\\[FFFF4324]t_sqlbur_test4.db");

const TDriveNumber KTestDrive = EDriveC;

const TUint KBufferSize = 2048; // used for reading backup files for validation

static CActiveScheduler* TheScheduler = NULL;
static CSqlSrvTestBurInterface* TheSqlSrvTestBurInterface = NULL;

/////////////////////////////////////

//Test db files
_LIT(KTestDbFileName1,"C:[21212122]AADB2.db");
_LIT(KTestDbFileName2,"C:[21212122]BBDB2.db");

//Temp buffers for storing files content, to be compared with what is received after the restore operation. 10 files max.
TInt TheFileCount = 0;
const TInt KMaxDbFileSize = 10 * 1024;//The max test db file size - 10 Kb max.
static TUint8 TheDbFileData[10][KMaxDbFileSize];//An array where the original db file content will be stored
static TInt TheDbFileSizes[10];//An array where the real db file size will be stored
static TUint8 TheBuf[KMaxDbFileSize];

/////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////

void DeleteBackupFiles()
	{
	TFileName fname;
	
	fname.Copy(KBackupFileTemplate);
	fname.Append((static_cast <TUid> (KClientUid)).Name());
	(void)TheSqlSrvTestBurInterface->Fs().Delete(fname);
	
	fname.Copy(KBackupFileTemplate);
	fname.Append((static_cast <TUid> (KZeroFileSizeUid)).Name());
	(void)TheSqlSrvTestBurInterface->Fs().Delete(fname);
	
	fname.Copy(KBackupFileTemplate);
	fname.Append((static_cast <TUid> (KTestClientUid1)).Name());
	(void)TheSqlSrvTestBurInterface->Fs().Delete(fname);
	
	fname.Copy(KBackupFileTemplate);
	fname.Append((static_cast <TUid> (KTestClientUid2)).Name());
	(void)TheSqlSrvTestBurInterface->Fs().Delete(fname);
	
	fname.Copy(KBackupFileTemplate);
	fname.Append((static_cast <TUid> (KTestClientUid3)).Name());
	(void)TheSqlSrvTestBurInterface->Fs().Delete(fname);
	}

void TestEnvDestroy()
	{
	(void)TheSqlSrvTestBurInterface->Fs().Delete(KBackupCopy);
	(void)TheSqlSrvTestBurInterface->Fs().Delete(KTestFile1);
	(void)TheSqlSrvTestBurInterface->Fs().Delete(KTestFile1Bak);
	(void)TheSqlSrvTestBurInterface->Fs().Delete(KTestFile3);
	(void)TheSqlSrvTestBurInterface->Fs().Delete(KTestFile4);
	(void)TheSqlSrvTestBurInterface->Fs().Delete(KZeroSizeFile);

	DeleteBackupFiles();
	
	delete TheSqlSrvTestBurInterface;
	TheSqlSrvTestBurInterface = NULL;		
	
	delete TheScheduler;
	TheScheduler = NULL;
	}

////////////////////////////
// Test macros and functions
////////////////////////////

void Check(TInt aValue, TInt aLine)
	{
	if(!aValue)
		{
		TestEnvDestroy();
		TheTest(EFalse, aLine);
		}
	}

void Check(TInt aValue, TInt aExpected, TInt aLine)
	{
	if(aValue != aExpected)
		{
		TestEnvDestroy();
		RDebug::Print(_L("*** Expected error: %d, got: %d\r\n"), aExpected, aValue);
		TheTest(EFalse, aLine);
		}
	}
#define TEST(arg) ::Check((arg), __LINE__)
#define TEST2(aValue, aExpected) ::Check(aValue, aExpected, __LINE__)

///////////////////////////////////////////////////////////////////////////////////////

static void DestroyFileList(RArray<HBufC*>& aFileList)
	{
	for(TInt i=0;i<aFileList.Count();++i)
		{
		delete aFileList[i];
		}
	aFileList.Close();
	}

//CSqlSrvTestBurInterface - test implementation of the MSqlSrvBurInterface, implemented in the production code by the SQL server.
CSqlSrvTestBurInterface *CSqlSrvTestBurInterface::New()
	{
	CSqlSrvTestBurInterface* self = new CSqlSrvTestBurInterface;
	TEST(self != NULL);
	self->Construct();
	return self;
	}

CSqlSrvTestBurInterface::CSqlSrvTestBurInterface()
	{
	}

void CSqlSrvTestBurInterface::Construct()
	{
	TInt err = iFs.Connect();
	TEST2(err, KErrNone);
	err = iFs.MkDir(KBackupDir);
	TEST(err == KErrNone || err == KErrAlreadyExists);
	err = iFs.CreatePrivatePath(KTestDrive);
	TEST2(err, KErrNone);
	}

CSqlSrvTestBurInterface::~CSqlSrvTestBurInterface()
	{
	iFs.Close();
	}

//Called by the backup client ot get a list of database files to backup
//The array is owned by the caller
//The SQL server would have the job to get a list of databases owned by
//the given SID and to determine whether the backup flag is set
//All databases that satisfy this requirement will be added to the array.
void CSqlSrvTestBurInterface::GetBackUpListL(TSecureId aUid, TDriveNumber aDrive, RArray<HBufC*>& aFileList)
	{
	if(aUid.iId == 0)
		{//Simulates that there are no databases for backup
		DestroyFileList(aFileList);
		return;
		}
	
	TDriveUnit driveUnit(aDrive);
	TDriveName driveName = driveUnit.Name();
	TFileName path;
	path.Copy(driveName);
	path.Append(KPrivateDir);
	//Include the aUid and the "*" mask
	TUidName uidName = (static_cast <TUid> (aUid)).Name();
	_LIT(KMatchAllDbFiles, "*");
	TBuf<KMaxUidName + sizeof(KMatchAllDbFiles)> fileNameMask(uidName);
	fileNameMask.Append(KMatchAllDbFiles);
	TParse parse;
	TInt err = parse.Set(path, &fileNameMask, NULL);
	User::LeaveIfError(err);
	//Do the search
	TPtrC fullPath(parse.FullName());
	CDir* fileNameCol = NULL;
	err = TheSqlSrvTestBurInterface->Fs().GetDir(fullPath, KEntryAttNormal, ESortNone, fileNameCol);
	if(err == KErrNotFound)
		{
		return;
		}
	User::LeaveIfError(err);
	CleanupStack::PushL(fileNameCol);
	TInt fileCount = fileNameCol->Count();
	err = aFileList.Reserve(fileCount);
	User::LeaveIfError(err);
	//Append the full database file paths to the file names list.
	for(TInt i=0;i<fileCount;++i)
		{
		const ::TEntry& entry = (*fileNameCol)[i];
		err = parse.Set(path, &entry.iName, NULL);
		User::LeaveIfError(err);
		TPtrC fname(parse.FullName());
		HBufC* fnameBuf = fname.AllocL();
		err = aFileList.Append(fnameBuf);
		User::LeaveIfError(err);
		}
	CleanupStack::PopAndDestroy(fileNameCol);
	}

//Returns the file system resource handle to the caller.
RFs& CSqlSrvTestBurInterface::Fs()
	{
	return iFs;
	}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

TBool FileExists(RFs& aFs, const TDesC& aFileName)
	{
	TEntry entry;
	return aFs.Entry(aFileName, entry) == KErrNone;
	}

void TestEnvCreate()
	{
	TheScheduler = new CActiveScheduler;
	TEST(TheScheduler != NULL);
	
	CActiveScheduler::Install(TheScheduler);

	TheSqlSrvTestBurInterface = CSqlSrvTestBurInterface::New();
	TEST(TheSqlSrvTestBurInterface != NULL);
	}

void PrepareSearchPath(TDriveNumber aDrive, TDes& aPath)
	{
	TDriveUnit driveUnit(aDrive);
	TDriveName driveName = driveUnit.Name();
	aPath.Copy(driveName);
	aPath.Append(KPrivateDir);
	}

void PrepareSearchPattern(const TDesC& aPath, TSecureId aUid, TParse& aParse)
	{
	TUidName uidName = (static_cast <TUid> (aUid)).Name();
	_LIT(KMatchAllDbFiles, "*");
	TBuf<KMaxUidName + sizeof(KMatchAllDbFiles)> fileNameMask(uidName);
	fileNameMask.Append(KMatchAllDbFiles);
	TInt err = aParse.Set(aPath, &fileNameMask, NULL);
	TEST2(err, KErrNone);
	}

//Reads the content of the db files and stores the content to a global memory buffer.
//That buffer content will be used later for verification of the restore process.
void StoreDbContentToBuf(RFs& aFs, TDriveNumber aDrive, TSecureId aUid)
	{
	TFileName path;
	PrepareSearchPath(aDrive, path);
	TParse parse;
	PrepareSearchPattern(path, aUid, parse);

	TheFileCount = -1;
	Mem::FillZ(TheDbFileData, sizeof(TheDbFileData));
	Mem::FillZ(TheDbFileSizes, sizeof(TheDbFileSizes));
	
	CDir* dir = NULL;
	TInt err = aFs.GetDir(parse.FullName(), KEntryAttNormal, ESortByName, dir);
	TEST2(err, KErrNone);
	TheFileCount = dir->Count();
	for(TInt i=0;i<TheFileCount;++i)
		{
		const ::TEntry& entry = (*dir)[i];
		err = parse.Set(path, &entry.iName, NULL);
		TEST2(err, KErrNone);
		TPtrC fname(parse.FullName());
	
		RFile dbFile;
		err = dbFile.Open(aFs, fname, EFileRead);
		TEST2(err, KErrNone);
		
		TInt fileSize = 0;
		err = dbFile.Size(fileSize);
		TEST2(err, KErrNone);
		TEST(fileSize > 0);

		TPtr8 bufptr(TheDbFileData[i], 0, KMaxDbFileSize);
		err = dbFile.Read(bufptr, fileSize);
		TEST2(err, KErrNone);
		TEST(fileSize == bufptr.Length());

		TheDbFileSizes[i] = fileSize;
		
		dbFile.Close();
		}
	delete dir;
	}

//At the moment when this function is called, the db files content is already restored.
//The function will open the restored db files and compare their content against the content
//of the original db files (kept in a global memory buffer).
void CompareDbContentWithBuf(RFs& aFs, TDriveNumber aDrive, TSecureId aUid)
	{
	TFileName path;
	PrepareSearchPath(aDrive, path);
	TParse parse;
	PrepareSearchPattern(path, aUid, parse);
	
	CDir* dir = NULL;
	TInt err = aFs.GetDir(parse.FullName(), KEntryAttNormal, ESortByName, dir);
	TEST2(err, KErrNone);
	TEST2(TheFileCount, dir->Count());
	for(TInt i=0;i<TheFileCount;++i)
		{
		TEST(TheDbFileSizes[i] > 0);
		
		const ::TEntry& entry = (*dir)[i];
		err = parse.Set(path, &entry.iName, NULL);
		TEST2(err, KErrNone);
		TPtrC fname(parse.FullName());
		
		RFile dbFile;
		TInt err = dbFile.Open(aFs, fname, EFileRead);
		TEST2(err, KErrNone);
		
		TInt fileSize = 0;
		err = dbFile.Size(fileSize);
		TEST2(err, KErrNone);
		TEST(fileSize > 0);
		TEST2(TheDbFileSizes[i], fileSize);

		TPtr8 bufptr(TheBuf, 0, KMaxDbFileSize);
		err = dbFile.Read(bufptr, fileSize);
		TEST2(err, KErrNone);
		TEST2(fileSize, bufptr.Length());

		err = Mem::Compare(TheBuf, fileSize, TheDbFileData[i], TheDbFileSizes[i]);
		TEST2(err, 0);

		dbFile.Close();
		}
	delete dir;
	}

////////////////////////////////////////////////////////////////////////////////////////

//The backup client will return a series of data chunks representing
//one of more databases for the uid of the data owner.
//This data is stored in a file on the C drive for the purposes of the test
TInt TestBackupL(CSqlBurCallback &aBackupClient, RFs& aFs, TSecureId aUid, TDriveNumber aDrive, TInt aDataChunkSize = KBufferSize)
	{
	TFileName backupFileName;
	backupFileName.Copy(KBackupFileTemplate);
	backupFileName.Append((static_cast <TUid> (aUid)).Name());
	
	RFile file;
	CleanupClosePushL(file);
	TInt err = file.Replace(aFs, backupFileName, EFileWrite | EFileStream | EFileShareExclusive);
	User::LeaveIfError(err);
	aBackupClient.InitialiseGetProxyBackupDataL(aUid, aDrive);
	
	TBuf8<KBufferSize> buf;
	TPtr8 ptr((TUint8*)buf.Ptr(), aDataChunkSize);
	TBool finishedFlag = EFalse;
	TInt count = 0;
	
	do
		{
		aBackupClient.GetBackupDataSectionL(ptr, finishedFlag);
		count += ptr.Length();
		err = file.Write(ptr);
		User::LeaveIfError(err);
		ptr.SetLength(0);
		}
	while(!finishedFlag);

	CleanupStack::PopAndDestroy(&file);
	
	if(count == 0)
		{
		User::Leave(KErrEof);
		}
	if(!FileExists(aFs, backupFileName))
		{
		User::Leave(KErrNotFound);
		}
	TheTest.Printf(_L("Backup complete. %d bytes processed.\r\n"), count);
	return count;
	}

//This sends the data in chunks form back to the BUR client
//for nupacking and restoration of the original databases files
TInt TestRestoreL(CSqlBurCallback &aRestoreClient, RFs& aFs, TSecureId aUid, TDriveNumber aDrive, TInt aDataChunkSize = KBufferSize)
	{
	TFileName backupFileName;
	backupFileName.Copy(KBackupFileTemplate);
	backupFileName.Append((static_cast <TUid> (aUid)).Name());
	
	RFile file;
	CleanupClosePushL(file);
	TInt err = file.Open(aFs, backupFileName, EFileRead | EFileShareExclusive);
	User::LeaveIfError(err);
	aRestoreClient.InitialiseRestoreProxyBaseDataL(aUid, aDrive);
	
	TBuf8<KBufferSize> buf;
	TPtr8 ptr((TUint8*)buf.Ptr(), aDataChunkSize);
	TBool finishedFlag = EFalse;
	
	TInt fileSize = 0;
	err = file.Size(fileSize);
	User::LeaveIfError(err);
	TInt count = fileSize;
	
	do
		{
		err = file.Read(ptr, aDataChunkSize);
		User::LeaveIfError(err);
		fileSize -= ptr.Length();
		finishedFlag = fileSize == 0;
		aRestoreClient.RestoreBaseDataSectionL(ptr, finishedFlag);
		ptr.SetLength(0);
		} 
	while(fileSize > 0);
	
	CleanupStack::PopAndDestroy(&file);
	
	aRestoreClient.RestoreComplete(aDrive);
	
	if(!finishedFlag)
		{
		User::Leave(KErrEof);
		}
		
	TheTest.Printf(_L("Restore complete. %d bytes processed.\r\n"), count);
	return count;
	}

//Verifies the integrity of the backup file.
void TestArchiveIntegrityL(CSqlBurCallback &aBackupClient, RFs& aFs, TSecureId aUid)
	{
	RFile bkpFile;
	CleanupClosePushL(bkpFile);

	TFileName backupFileName;
	backupFileName.Copy(KBackupFileTemplate);
	backupFileName.Append((static_cast <TUid> (aUid)).Name());
	
	TInt err = bkpFile.Open(aFs, backupFileName, EFileRead | EFileShareExclusive);
	User::LeaveIfError(err);
	
	TBuf8<KBufferSize> buf;
	TPtr8 ptr((TUint8*)buf.Ptr(), buf.MaxLength());
	
	TInt bkpFileSize = 0;
	err = bkpFile.Size(bkpFileSize);
	User::LeaveIfError(err);
	
	while(bkpFileSize > 0)
		{
		// get the checksum
		err = bkpFile.Read(ptr, 16); // 8 UTF-16 characters
		User::LeaveIfError(err);
		if(ptr.Length() != 16)
			{
			User::Leave(KErrCorrupt);
			}
		TPtr ptr16((TUint16*) ptr.Ptr(), 8, 8);
		TLex lex(ptr16);
		TUint32 checksum;
		lex.SkipSpace();
		err = lex.Val(checksum, EHex);
		User::LeaveIfError(err);
		bkpFileSize -= 16;

		// get the old file size
		err = bkpFile.Read(ptr, 16); // 8 UTF-16 characters
		User::LeaveIfError(err);
		if(ptr.Length() != 16)
			{
			User::Leave(KErrCorrupt);
			}
		ptr16.Set((TUint16*) ptr.Ptr(), 8, 8);
		lex.Assign(ptr16);
		TUint32 oldFileSize;
		lex.SkipSpace();
		err = lex.Val(oldFileSize, EHex);
		User::LeaveIfError(err);
		bkpFileSize -= 16;

		// get the backup file header version
		err = bkpFile.Read(ptr, 8); // 4 UTF-16 characters
		User::LeaveIfError(err);
		ptr16.Set((TUint16*)ptr.Ptr(), 4, 4);
		lex.Assign(ptr16);
		TUint32 hdrVer;
		lex.SkipSpace();
		err = lex.Val(hdrVer, EHex);
		User::LeaveIfError(err);
		bkpFileSize -= 8;

		// get the file size
		err = bkpFile.Read(ptr, 32); // 16 UTF-16 characters
		User::LeaveIfError(err);
		if(ptr.Length() != 32)
			{
			User::Leave(KErrCorrupt);
			}
		ptr16.Set((TUint16*) ptr.Ptr(), 16, 16);
		lex.Assign(ptr16);
		TInt64 fileSize;
		lex.SkipSpace();
		err = lex.Val(fileSize, EHex);
		User::LeaveIfError(err);
		bkpFileSize -= 32;

		// get the filename size
		err = bkpFile.Read(ptr, 16); // 8 UTF-16 characters
		User::LeaveIfError(err);
		ptr16.Set((TUint16*)ptr.Ptr(), 8, 8);
		lex.Assign(ptr16);
		TUint32 fileNameSize;
		lex.SkipSpace();
		err = lex.Val(fileNameSize, EHex);
		User::LeaveIfError(err);
		bkpFileSize -= 16;

		// get the filename
		err = bkpFile.Read(ptr, fileNameSize * 2); // fileName UTF-16 characters
		User::LeaveIfError(err);
		if(ptr.Length() != (fileNameSize * 2))
			{
			User::Leave(KErrCorrupt);
			}
		ptr16.Set((TUint16*) ptr.Ptr(), fileNameSize, fileNameSize);
		lex.Assign(ptr16);
		TParse tp;
		tp.Set(ptr16, NULL, NULL);
		TPtrC dbFileName = tp.Name();
		bkpFileSize -= fileNameSize * 2;
	
		// open a local file - replaces any previous one
		RFile64 dbFile;
		CleanupClosePushL(dbFile);
		err = dbFile.Replace(aFs, dbFileName, EFileWrite | EFileShareExclusive);
		User::LeaveIfError(err);
	
		// copy all the data (file size bytes)
		TInt bytesLeftToRead = fileSize;

		while(bytesLeftToRead > 0)
			{
			TInt readSize = bytesLeftToRead > KBufferSize ? KBufferSize : bytesLeftToRead;
			err = bkpFile.Read(ptr, readSize);
			User::LeaveIfError(err);
			if(ptr.Length() != readSize)
				{
				User::Leave(KErrCorrupt);	
				}
			bytesLeftToRead -= readSize;
			err = dbFile.Write(ptr, readSize);
			User::LeaveIfError(err);
			}

		bkpFileSize -= fileSize;
		
		// checksum the file
		TUint64 checkSum64 = 0;
		User::LeaveIfError(aBackupClient.CheckSum(dbFile, checkSum64));
		TUint32 checksum32 = checkSum64 & 0xFFFFFFFF; 
		
		if(checksum != checksum32)
			{
			User::Leave(KErrCorrupt);
			}
			
		// all done with this file
		CleanupStack::PopAndDestroy(&dbFile);
		err = aFs.Delete(dbFileName);
		User::LeaveIfError(err);
		}
	
	CleanupStack::PopAndDestroy(&bkpFile);
	}

/**
@SYMTestCaseID			SYSLIB-SQL-UT-4002
@SYMTestCaseDesc		Test for DEF113598 - "SQL, t_sqlbur unit test needs refactoring"
						The test backups 2 test db files, then verifies the backup file integrity,
						then restores the test db files content from the backup file.
						At the end, the test checks that the restored test db files content is the
						same as the content of the original test db file.
@SYMTestPriority		High
@SYMTestActions			Test for DEF113598 - "SQL, t_sqlbur unit test needs refactoring"
@SYMTestExpectedResults Test must not fail
@SYMDEF					DEF113598
*/	
void FunctionalTest()
	{
	CSqlBurCallback* backupClient = NULL;
	TRAPD(err, backupClient = CSqlBurCallback::NewL(*TheSqlSrvTestBurInterface));
	TEST(backupClient != NULL);

	////////////////////////////////////////
	
	//Virtual functions - with default implementation
	
	(void)backupClient->GetExpectedDataSize(KTestDrive);

	(void)backupClient->GetDataChecksum(KTestDrive);
	
	TBool finished = EFalse;
	TPtr8 ptr(0, 0, 0);
	TRAP(err, backupClient->GetSnapshotDataL(KTestDrive, ptr, finished));
	TEST2(err, KErrNotSupported);

	TRAP(err, backupClient->InitialiseGetBackupDataL(KTestDrive));
	TEST2(err, KErrNotSupported);

	TRAP(err, backupClient->InitialiseRestoreBaseDataL(KTestDrive));
	TEST2(err, KErrNotSupported);

	TRAP(err, backupClient->InitialiseRestoreIncrementDataL(KTestDrive));
	TEST2(err, KErrNotSupported);

	TPtrC8 ptr2(KNullDesC8);
	TRAP(err, backupClient->RestoreIncrementDataSectionL(ptr2, finished));
	TEST2(err, KErrNotSupported);

	TRAP(err, backupClient->AllSnapshotsSuppliedL());
	TEST2(err, KErrNone);

	TRAP(err, backupClient->ReceiveSnapshotDataL(KTestDrive, ptr2, finished));
	TEST2(err, KErrNotSupported);

	backupClient->TerminateMultiStageOperation();

	////////////////////////////////////////

	TInt bytesStored = 0;
	TRAP(err, bytesStored = TestBackupL(*backupClient, TheSqlSrvTestBurInterface->Fs(), KClientUid, KTestDrive));
	TEST2(err, KErrNone);

	TheTest.Next(_L("Archive integrity test"));
	
	TRAP(err, TestArchiveIntegrityL(*backupClient, TheSqlSrvTestBurInterface->Fs(), KClientUid));
	TEST2(err, KErrNone);

	delete backupClient;

	TheTest.Next(_L("Restore: functional test"));

	CSqlBurCallback* restoreClient = NULL;
	TRAP(err, restoreClient = CSqlBurCallback::NewL(*TheSqlSrvTestBurInterface));
	TEST(restoreClient != NULL);

	TInt bytesRestored = 0;
	TRAP(err, bytesRestored = TestRestoreL(*restoreClient, TheSqlSrvTestBurInterface->Fs(), KClientUid, KTestDrive));
	TEST2(err, KErrNone);
	
	TEST(bytesRestored == bytesStored);

	delete restoreClient;

	CompareDbContentWithBuf(TheSqlSrvTestBurInterface->Fs(), KTestDrive, KClientUid);
	}
	
TInt DoBackupL(TDriveNumber aDrive, TSecureId aUid)
	{
	CSqlBurCallback* backupClient = CSqlBurCallback::NewL(*TheSqlSrvTestBurInterface);
	CleanupStack::PushL(backupClient);
	TInt bytesStored = TestBackupL(*backupClient, TheSqlSrvTestBurInterface->Fs(), aUid, aDrive);
	CleanupStack::PopAndDestroy(backupClient);
	return bytesStored;
	}

TInt DoRestoreL(TDriveNumber aDrive, TSecureId aUid)
	{
	CSqlBurCallback* restoreClient = CSqlBurCallback::NewL(*TheSqlSrvTestBurInterface);
	CleanupStack::PushL(restoreClient);
	TInt bytesRestored = TestRestoreL(*restoreClient, TheSqlSrvTestBurInterface->Fs(), aUid, aDrive);
	CleanupStack::PopAndDestroy(restoreClient);
	return bytesRestored;
	}

/**
@SYMTestCaseID			SYSLIB-SQL-UT-4003
@SYMTestCaseDesc		Test for DEF113598 - "SQL, t_sqlbur unit test needs refactoring"
						Under simulated OOM condition, the test backups 2 test db files, 
						then restores the test db files content from the backup file.
						At the end, the test checks that the restored test db files content is the
						same as the content of the original test db file.
@SYMTestPriority		High
@SYMTestActions			Test for DEF113598 - "SQL, t_sqlbur unit test needs refactoring"
@SYMTestExpectedResults Test must not fail
@SYMDEF					DEF113598
*/	
void OomTest()
	{
	///////////////////////////////////////////////////////////////////////////////
	TInt err = KErrNoMemory;
	TInt bytesStored = 0;
	TInt count = 0;
	
	for(count=1;err==KErrNoMemory;++count)
		{
		TInt startProcessHandleCount;
		TInt startThreadHandleCount;
		RThread().HandleCount(startProcessHandleCount, startThreadHandleCount);
		
		User::__DbgMarkStart(RHeap::EUser);
		User::__DbgSetAllocFail(RHeap::EUser,RHeap::EFailNext, count);
		TRAP(err, bytesStored = DoBackupL(KTestDrive, KClientUid));
		User::__DbgMarkEnd(RHeap::EUser, 0);
		
		TInt endProcessHandleCount;
		TInt endThreadHandleCount;
		RThread().HandleCount(endProcessHandleCount, endThreadHandleCount);
		
		TEST(startProcessHandleCount == endProcessHandleCount);
		TEST(startThreadHandleCount == endThreadHandleCount);
		}
	TEST2(err, KErrNone);
	TheTest.Printf(_L("OOM backup test succeeded at heap failure rate of %d\r\n"), count);

	///////////////////////////////////////////////////////////////////////////////
	TheTest.Next(_L("Restore: OOM test"));
	err = KErrNoMemory;
	TInt bytesRestored = 0;
	
	for(count=1;err==KErrNoMemory;++count)
		{
		TInt startProcessHandleCount;
		TInt startThreadHandleCount;
		RThread().HandleCount(startProcessHandleCount, startThreadHandleCount);
		
		User::__DbgMarkStart(RHeap::EUser);
		User::__DbgSetAllocFail(RHeap::EUser,RHeap::EFailNext, count);
		TRAP(err, bytesRestored = DoRestoreL(KTestDrive, KClientUid));
		User::__DbgMarkEnd(RHeap::EUser, 0);
		
		TInt endProcessHandleCount;
		TInt endThreadHandleCount;
		RThread().HandleCount(endProcessHandleCount, endThreadHandleCount);
		
		TEST(startProcessHandleCount == endProcessHandleCount);
		TEST(startThreadHandleCount == endThreadHandleCount);
		}
	TEST2(err, KErrNone);
	User::__DbgSetAllocFail(RHeap::EUser, RAllocator::ENone, 0);
	TheTest.Printf(_L("OOM restore test succeeded at heap failure rate of %d\r\n"), count);
	
	TEST2(bytesStored, bytesRestored);

	CompareDbContentWithBuf(TheSqlSrvTestBurInterface->Fs(), KTestDrive, KClientUid);
	}

/**
@SYMTestCaseID			PDS-SQL-UT-4143
@SYMTestCaseDesc		SQL Backup&Restore - data chunk size test.
						The test uses an integer array of 10 elements with randomly generated data chunk sizes.
						Then the test runs 10 backup iterations using each time different data chunk size.
						After each backup iteration the test performs a restore operation and checks that the 
						data has been backup&restored without errors.
@SYMTestActions			SQL Backup&Restore - data chunk size test.
@SYMTestExpectedResults Test must not fail
@SYMTestPriority		Medium
@SYMREQ					REQ12104
*/
void FunctionalTest2()
	{
	TTime now;
	now.UniversalTime();
	TInt64 seed = now.Int64();

	const TInt KArraySize = 10;
	TInt  dataChunks[10] = {2, 6, 0, 0, 0, 0, 0, 0, 0, 0};
	const TInt KMaxDataChunkSize = 50;
	
	for(TInt i=2;i<KArraySize;)
		{
		TInt dataChunkSize = Math::Rand(seed) % KMaxDataChunkSize;
		if((dataChunkSize % 2) == 0 && dataChunkSize != 0)	//The code works only with data chunks with even sizes!!!
			{
			dataChunks[i++] = dataChunkSize;
			}
		}
	
	for(TInt i=0;i<KArraySize;++i)
		{
		TheTest.Printf(_L(" === Iteration %d, chunk size %d\r\n"), i + 1, dataChunks[i]);
		CSqlBurCallback* backupClient = NULL;
		TRAPD(err, backupClient = CSqlBurCallback::NewL(*TheSqlSrvTestBurInterface));
		TEST(backupClient != NULL);

		TInt bytesStored = 0;
		TRAP(err, bytesStored = TestBackupL(*backupClient, TheSqlSrvTestBurInterface->Fs(), KClientUid, KTestDrive, dataChunks[i]));
		TEST2(err, KErrNone);

		TRAP(err, TestArchiveIntegrityL(*backupClient, TheSqlSrvTestBurInterface->Fs(), KClientUid));
		TEST2(err, KErrNone);

		delete backupClient;

		CSqlBurCallback* restoreClient = NULL;
		TRAP(err, restoreClient = CSqlBurCallback::NewL(*TheSqlSrvTestBurInterface));
		TEST(restoreClient != NULL);

		TInt bytesRestored = 0;
		TRAP(err, bytesRestored = TestRestoreL(*restoreClient, TheSqlSrvTestBurInterface->Fs(), KClientUid, KTestDrive, dataChunks[i]));
		TEST2(err, KErrNone);
		
		TEST(bytesRestored == bytesStored);

		delete restoreClient;

		CompareDbContentWithBuf(TheSqlSrvTestBurInterface->Fs(), KTestDrive, KClientUid);
		}
	}

/**
@SYMTestCaseID			PDS-SQL-UT-4144
@SYMTestCaseDesc		SQL Backup&Restore - legacy backup file format header test.
						The 64-bit file system related changes made in the SQL server required some 
						changes to be made in the format of the backup file header.
						The test checks that a backup file created with the previous format of the file header
						can be restored without errors by the updated Backup&Restore implementation.
@SYMTestActions			SQL Backup&Restore - legacy backup file format header test.
@SYMTestExpectedResults Test must not fail
@SYMTestPriority		Medium
@SYMREQ					REQ12104
*/
void LegacyFileFormatTest()
	{
	//KBackupFile2 is a database backup file with header version 0.
	(void)TheSqlSrvTestBurInterface->Fs().Delete(KBackupFile2);
	TInt rc = BaflUtils::CopyFile(TheSqlSrvTestBurInterface->Fs(), KBackupFile2Z, KBackupFile2);
	TEST2(rc, KErrNone);
	(void)TheSqlSrvTestBurInterface->Fs().SetAtt(KBackupFile2, 0, KEntryAttReadOnly);

	//Restore the databases from KBackupFile2.
	CSqlBurCallback* restoreClient = NULL;
	TRAP(rc, restoreClient = CSqlBurCallback::NewL(*TheSqlSrvTestBurInterface));
	TEST(restoreClient != NULL);

	RFile file;
	rc = file.Open(TheSqlSrvTestBurInterface->Fs(), KBackupFile2, EFileRead | EFileShareExclusive);
	TEST2(rc, KErrNone);
	
	TRAP(rc, restoreClient->InitialiseRestoreProxyBaseDataL(KClientUid, KTestDrive));
	TEST2(rc, KErrNone);
	
	TBuf8<KBufferSize> buf;
	TPtr8 ptr((TUint8*)buf.Ptr(), buf.MaxSize());
	TBool finishedFlag = EFalse;
	
	TInt fileSize = 0;
	rc = file.Size(fileSize);
	TEST2(rc, KErrNone);
	
	do
		{
		rc = file.Read(ptr);
		TEST2(rc, KErrNone);
		fileSize -= ptr.Size();
		finishedFlag = fileSize == 0;
		TRAP(rc, restoreClient->RestoreBaseDataSectionL(ptr, finishedFlag));
		ptr.SetLength(0);
		} 
	while(fileSize > 0);
	
	file.Close();	
	
	restoreClient->RestoreComplete(KTestDrive);
	
	TEST(finishedFlag);
		
	delete restoreClient;

	//At this point we have two restored databases: KTestDbFileName1 and KTestDbFileName2.
	//The content of the restored file cannot be compared directly, because t_sqlattach uses the same test databases
	//and modifies them. The original database content was stored without executing t_sqlattach.
	//Hence a simple test is made: open the restored database, check if the database content can be accessed.
	
	RSqlDatabase db;
	rc = db.Open(KTestDbFileName1);
	TEST2(rc, KErrNone);
	//The database contains this table: "TABLE C(A1 INTEGER, B2 BLOB)". 
	rc = db.Exec(_L("INSERT INTO C VALUES(100, 200)"));
	TEST2(rc, 1);
	RSqlStatement stmt;
	rc = stmt.Prepare(db, _L("SELECT * FROM C"));
	TEST2(rc, KErrNone);
	while((rc = stmt.Next()) == KSqlAtRow)
		{
		}
	stmt.Close();
	TEST2(rc, KSqlAtEnd);
	db.Close();

	rc = db.Open(KTestDbFileName2);
	TEST2(rc, KErrNone);
	//The database contains this table: "TABLE A1(F1 INTEGER , F2 INTEGER, B1 BLOB)"
	rc = db.Exec(_L("INSERT INTO A1 VALUES(100, 200, NULL)"));
	TEST2(rc, 1);
	rc = stmt.Prepare(db, _L("SELECT * FROM A1"));
	TEST2(rc, KErrNone);
	while((rc = stmt.Next()) == KSqlAtRow)
		{
		}
	stmt.Close();
	TEST2(rc, KSqlAtEnd);
	db.Close();

	(void)TheSqlSrvTestBurInterface->Fs().Delete(KBackupFile2);
	}
		
/**
@SYMTestCaseID			PDS-SQL-UT-4192
@SYMTestCaseDesc		SQL Backup&Restore - empty backup file list test.
						The test checks what will happen if the list with the files for backup is empty.
						The GetBackupDataSectionL() should immediatelly set the flag parameter to true and do nothing.
@SYMTestActions			SQL Backup&Restore - empty backup file list test.
@SYMTestExpectedResults Test must not fail
@SYMTestPriority		High
@SYMDEF					DEF145198
*/
void EmptyBackupFileListTest()
	{
	CSqlBurCallback* backupClient = NULL;
	TRAPD(err, backupClient = CSqlBurCallback::NewL(*TheSqlSrvTestBurInterface));
	TEST(backupClient != NULL);
	
	TRAP(err, backupClient->InitialiseGetProxyBackupDataL(KNullUid, KTestDrive));
	TEST2(err, KErrNone);

	TBuf8<100> buf;
	TPtr8 ptr((TUint8*)buf.Ptr(), 0, buf.MaxLength());
	TBool finishedFlag = EFalse;
	TRAP(err, backupClient->GetBackupDataSectionL(ptr, finishedFlag));
	delete backupClient;
	TEST2(err, KErrNone);
	TEST(finishedFlag);
	TEST2(buf.Length(), 0);
	}


/**
@SYMTestCaseID			PDS-SQL-UT-4193
@SYMTestCaseDesc		SQL Backup&Restore - file I/O error simulation test.
						The test executes a backup, followed by a restore operation 
						in a file I/O error simulation loop.
@SYMTestActions			SQL Backup&Restore - file I/O error simulation test.
@SYMTestExpectedResults Test must not fail
@SYMTestPriority		High
@SYMDEF					DEF145198
*/
void BackupRestoreFileIoErrTest()
	{
	//Make sure that the database content, just before the backup, will be copied to the test biffers.
	//The buffers will be used during the restore testing for verification of the database content.
	StoreDbContentToBuf(TheSqlSrvTestBurInterface->Fs(), KTestDrive, KClientUid);
	
	for(TInt fsError=KErrNotFound;fsError>=KErrBadName;--fsError)
		{
		TheTest.Printf(_L("===Backup&Restore, simulated file system error=%d\r\n"), fsError);
		
		TInt err = KErrGeneral;
		TInt bytesStored = -1;
		TInt it_cnt1 = 0;
		for(;err<KErrNone;++it_cnt1)
			{
			__UHEAP_MARK;
			(void)TheSqlSrvTestBurInterface->Fs().SetErrorCondition(fsError, it_cnt1);
			TRAP(err, bytesStored = DoBackupL(KTestDrive, KClientUid));
			(void)TheSqlSrvTestBurInterface->Fs().SetErrorCondition(KErrNone);
			__UHEAP_MARKEND;
			}
		TEST2(err, KErrNone);
	
		err = KErrGeneral;
		TInt bytesRestored = -1;
		TInt it_cnt2 = 0;
	    for(;err<KErrNone;++it_cnt2)
			{
			__UHEAP_MARK;
			(void)TheSqlSrvTestBurInterface->Fs().SetErrorCondition(fsError, it_cnt2);
			TRAP(err, bytesRestored = DoRestoreL(KTestDrive, KClientUid));
			(void)TheSqlSrvTestBurInterface->Fs().SetErrorCondition(KErrNone);
			__UHEAP_MARKEND;
			}
		TEST2(err, KErrNone);
		
		TEST2(bytesStored, bytesRestored);
		CompareDbContentWithBuf(TheSqlSrvTestBurInterface->Fs(), KTestDrive, KClientUid);
		
		TheTest.Printf(_L("Backup&Restore file I/O error simulation test succeeded at backup iteration %d and restore itreration %d\r\n"), it_cnt1 - 1, it_cnt2 - 1);
		}
	}

/**
@SYMTestCaseID			PDS-SQL-UT-4225
@SYMTestCaseDesc		SQL Backup - zero size file backup.
						The test executes a backup on a file with zero size.
@SYMTestActions			SQL Backup - zero size file backup.
@SYMTestExpectedResults Test must not fail
@SYMTestPriority		High
*/
void BackupZeroSizeFileTest()
	{
	CSqlBurCallback* backupClient = NULL;
	TRAPD(err, backupClient = CSqlBurCallback::NewL(*TheSqlSrvTestBurInterface));
	TEST2(err, KErrNone);
	TEST(backupClient != NULL);
	//
	(void)TheSqlSrvTestBurInterface->Fs().Delete(KZeroSizeFile);
	RFile file;
	err = file.Replace(TheSqlSrvTestBurInterface->Fs(), KZeroSizeFile, EFileRead | EFileWrite);
	TEST2(err, KErrNone);
	file.Close();
	//
	TRAP(err, TestBackupL(*backupClient, TheSqlSrvTestBurInterface->Fs(), KZeroFileSizeUid, KTestDrive));
	TEST2(err, KErrNone);
	//Write something to the file
	err = file.Open(TheSqlSrvTestBurInterface->Fs(), KZeroSizeFile, EFileRead | EFileWrite);
	TEST2(err, KErrNone);
	_LIT8(KData, "123456787989");
	err = file.Write(KData);
	TEST2(err, KErrNone);
	err = file.Flush();
	TEST2(err, KErrNone);
	file.Close();
	//Do the restore. After the restore the file size should be 0.
	TRAP(err, TestRestoreL(*backupClient, TheSqlSrvTestBurInterface->Fs(), KZeroFileSizeUid, KTestDrive));
	TEST2(err, KErrNone);
	err = file.Open(TheSqlSrvTestBurInterface->Fs(), KZeroSizeFile, EFileRead | EFileWrite);
	TEST2(err, KErrNone);
	TInt size;
	err = file.Size(size);
	TEST2(err, KErrNone);
	file.Close();
	TEST2(size, 0);
	//Write something to the file
	err = file.Open(TheSqlSrvTestBurInterface->Fs(), KZeroSizeFile, EFileRead | EFileWrite);
	TEST2(err, KErrNone);
	err = file.Write(KData);
	TEST2(err, KErrNone);
	err = file.Flush();
	TEST2(err, KErrNone);
	file.Close();
	//Call RestoreBaseDataSectionL() with 0 data in the input buffer and finished flag: false.
	TRAP(err, backupClient->InitialiseRestoreProxyBaseDataL(KZeroFileSizeUid, KTestDrive));
	TEST2(err, KErrNone);
	TPtrC8 zeroBuf;
	TRAP(err, backupClient->RestoreBaseDataSectionL(zeroBuf, EFalse));
	TEST2(err, KErrNone);
	//No restore in this case, 
	err = file.Open(TheSqlSrvTestBurInterface->Fs(), KZeroSizeFile, EFileRead | EFileWrite);
	TEST2(err, KErrNone);
	err = file.Size(size);
	TEST2(err, KErrNone);
	file.Close();
	TEST(size > 0);
	//Call RestoreBaseDataSectionL() with 0 data in the input buffer and finished flag: true.
	TRAP(err, backupClient->InitialiseRestoreProxyBaseDataL(KZeroFileSizeUid, KTestDrive));
	TEST2(err, KErrNone);
	TRAP(err, backupClient->RestoreBaseDataSectionL(zeroBuf, ETrue));
	TEST2(err, KErrNone);
	//No restore in this case, 
	err = file.Open(TheSqlSrvTestBurInterface->Fs(), KZeroSizeFile, EFileRead | EFileWrite);
	TEST2(err, KErrNone);
	err = file.Size(size);
	TEST2(err, KErrNone);
	file.Close();
	TEST(size > 0);
	//
	(void)TheSqlSrvTestBurInterface->Fs().Delete(KZeroSizeFile);
	delete backupClient;
	}

/**
@SYMTestCaseID			PDS-SQL-UT-4226
@SYMTestCaseDesc		SQL Restore - corrupted archive 1.
						The test does a backup of a file with a non-zero size.
						The the test modifies the archive, simulating a corruption.
						The the test performs a restore from the archive. The corruption
						should be detected and reported by SQL B&R code.
@SYMTestActions			SQL Restore - corrupted archive 1.
@SYMTestExpectedResults Test must not fail
@SYMTestPriority		High
*/
void CorruptedArchiveTest1()
	{
	CSqlBurCallback* backupClient = NULL;
	TRAPD(err, backupClient = CSqlBurCallback::NewL(*TheSqlSrvTestBurInterface));
	TEST2(err, KErrNone);
	TEST(backupClient != NULL);
	//Create the test file that will be sent for backup
	(void)TheSqlSrvTestBurInterface->Fs().Delete(KTestFile1);
	RFile file;
	err = file.Replace(TheSqlSrvTestBurInterface->Fs(), KTestFile1, EFileRead | EFileWrite);
	TEST2(err, KErrNone);
	_LIT8(KData1, "123456787989");
	err = file.Write(KData1);
	TEST2(err, KErrNone);
	err = file.Flush();
	TEST2(err, KErrNone);
	file.Close();
	//Backup the file
	TRAP(err, TestBackupL(*backupClient, TheSqlSrvTestBurInterface->Fs(), KTestClientUid1, KTestDrive));
	TEST2(err, KErrNone);
	//Modify the file, which was sent for backup
	err = file.Open(TheSqlSrvTestBurInterface->Fs(), KTestFile1, EFileRead | EFileWrite);
	TEST2(err, KErrNone);
	_LIT8(KData2, "ABCDEFGH");
	err = file.Write(KData2);
	TEST2(err, KErrNone);
	err = file.SetSize(KData2().Length());
	TEST2(err, KErrNone);
	err = file.Flush();
	TEST2(err, KErrNone);
	file.Close();
	//Corrupt the archive
	TFileName backupFileName;
	backupFileName.Copy(KBackupFileTemplate);
	backupFileName.Append((static_cast <TUid> (KTestClientUid1)).Name());
	err = file.Open(TheSqlSrvTestBurInterface->Fs(), backupFileName, EFileRead | EFileWrite);
	TEST2(err, KErrNone);
	TInt pos = -3;
	err = file.Seek(ESeekEnd, pos);
	TEST2(err, KErrNone);
	_LIT8(KData3, "ERR");
	err = file.Write(KData3);
	TEST2(err, KErrNone);
	err = file.Flush();
	TEST2(err, KErrNone);
	file.Close();
	//Try to restore the archive
	TRAP(err, TestRestoreL(*backupClient, TheSqlSrvTestBurInterface->Fs(), KTestClientUid1, KTestDrive));
	TEST2(err, KErrCorrupt);
	//Check that the file really has not been restored
	err = file.Open(TheSqlSrvTestBurInterface->Fs(), KTestFile1, EFileRead | EFileWrite);
	TEST2(err, KErrNone);
	TBuf8<50> data;
	err = file.Read(data);
	TEST2(err, KErrNone);
	file.Close();
	TEST(data == KData2);
	//
	(void)TheSqlSrvTestBurInterface->Fs().Delete(KTestFile1);
	delete backupClient;
	}

/**
@SYMTestCaseID			PDS-SQL-UT-4227
@SYMTestCaseDesc		SQL Restore - corrupted archive 2.
						The test does a backup of a file with a non-zero size.
						Then the test modifies the archive, byte after byte each step,
						simulating a corruption.
						The the test performs a restore from the archive. The corruption
						should be detected and reported by SQL B&R code.
@SYMTestActions			SQL Restore - corrupted archive 2.
@SYMTestExpectedResults Test must not fail
@SYMTestPriority		High
*/
void CorruptedArchiveTest2()
	{
	CSqlBurCallback* backupClient = NULL;
	TRAPD(err, backupClient = CSqlBurCallback::NewL(*TheSqlSrvTestBurInterface));
	TEST2(err, KErrNone);
	TEST(backupClient != NULL);
	//Create the test file that will be sent for backup
	(void)TheSqlSrvTestBurInterface->Fs().Delete(KTestFile1);
	RFile file;
	err = file.Replace(TheSqlSrvTestBurInterface->Fs(), KTestFile1, EFileRead | EFileWrite);
	TEST2(err, KErrNone);
	_LIT8(KData1, "123456787989");
	err = file.Write(KData1);
	TEST2(err, KErrNone);
	err = file.Flush();
	TEST2(err, KErrNone);
	file.Close();
	//Backup the file
	TRAP(err, TestBackupL(*backupClient, TheSqlSrvTestBurInterface->Fs(), KTestClientUid1, KTestDrive));
	TEST2(err, KErrNone);
	//Modify the file, which was sent for backup
	err = file.Open(TheSqlSrvTestBurInterface->Fs(), KTestFile1, EFileRead | EFileWrite);
	TEST2(err, KErrNone);
	_LIT8(KData2, "ABCDEFGH");
	err = file.Write(KData2);
	TEST2(err, KErrNone);
	err = file.SetSize(KData2().Length());
	TEST2(err, KErrNone);
	err = file.Flush();
	TEST2(err, KErrNone);
	file.Close();
	//Make a copy of the modified file
	CFileMan* fm = NULL;
	TRAP(err, fm = CFileMan::NewL(TheSqlSrvTestBurInterface->Fs()));
	TEST2(err, KErrNone);
	(void)TheSqlSrvTestBurInterface->Fs().Delete(KTestFile1Bak);
	err = fm->Copy(KTestFile1, KTestFile1Bak);
	TEST2(err, KErrNone);
	//Get the archive size
	TFileName backupFileName;
	backupFileName.Copy(KBackupFileTemplate);
	backupFileName.Append((static_cast <TUid> (KTestClientUid1)).Name());
	err = file.Open(TheSqlSrvTestBurInterface->Fs(), backupFileName, EFileRead | EFileWrite);
	TEST2(err, KErrNone);
	TInt size = 0;
	err = file.Size(size);
	TEST2(err, KErrNone);
	file.Close();
	//Save a copy of the archive
	(void)TheSqlSrvTestBurInterface->Fs().Delete(KBackupCopy);
	err = fm->Copy(backupFileName, KBackupCopy);
	TEST2(err, KErrNone);
	//On each iteration step: corrupt the archive and try to do a restore from it.
	for(TInt i=0;i<size;++i)
		{
		//Change 1 byte in the archive
		err = file.Open(TheSqlSrvTestBurInterface->Fs(), backupFileName, EFileRead | EFileWrite);
		TEST2(err, KErrNone);
		TInt pos = i;
		err = file.Seek(ESeekStart, pos);
		TEST2(err, KErrNone);
		TBuf8<1> byte;
		err = file.Read(byte);
		TEST2(err, KErrNone);
		++byte[0];
		err = file.Seek(ESeekStart, pos);
		TEST2(err, KErrNone);
		err = file.Write(byte);
		TEST2(err, KErrNone);
		if(i == (size - 1) && (size & 0x01) == 0)
			{//Make the file size an odd number, just to test....  
			err = file.Write(byte);
			TEST2(err, KErrNone);
			}
		err = file.Flush();
		TEST2(err, KErrNone);
		file.Close();
		//Restore
		TRAP(err, TestRestoreL(*backupClient, TheSqlSrvTestBurInterface->Fs(), KTestClientUid1, KTestDrive));
		TheTest.Printf(_L("Iteration %d, err=%d\r\n"), i, err);
		if(err == KErrNone)
			{
			//Delete the restored file. The reason that the restore didn't fail is because only the file data is 
			//protected with checksum. The restore file header - not. The restore completed, the data was restored
			//to a file with different name. Or even to a file with the same name. Delete created file(s).
			(void)fm->Delete(KTestDeleteMask1);
			(void)fm->Delete(KTestDeleteMask2);
			}
		else
			{
			//The restore completed with an error. The file content is preserved.
			//Check that the file content is the same. 
			err = file.Open(TheSqlSrvTestBurInterface->Fs(), KTestFile1, EFileRead | EFileWrite);
			TEST2(err, KErrNone);
			TBuf8<50> data;
			err = file.Read(data);
			TEST2(err, KErrNone);
			file.Close();
			TEST(data == KData2);
			}
		//Restore the file from the backup copy
		(void)TheSqlSrvTestBurInterface->Fs().Delete(KTestFile1);
		err = fm->Copy(KTestFile1Bak, KTestFile1);
		TEST2(err, KErrNone);
		//Restore the archive from the good copy.
		(void)TheSqlSrvTestBurInterface->Fs().Delete(backupFileName);
		err = fm->Copy(KBackupCopy, backupFileName);
		TEST2(err, KErrNone);
		}
	//
	delete fm;
	(void)TheSqlSrvTestBurInterface->Fs().Delete(KTestFile1);
	(void)TheSqlSrvTestBurInterface->Fs().Delete(KTestFile1Bak);
	(void)TheSqlSrvTestBurInterface->Fs().Delete(KBackupCopy);
	delete backupClient;
	}

/**
@SYMTestCaseID			PDS-SQL-UT-4228
@SYMTestCaseDesc		SQL Backup&Restore on a drive different than KTestDrive (C: by default).
						The test creates one database on KTestDrive and another database 
						with the same name on a drive different than KTestDrive.
						Then the test backups the "not KTestDrive" drive and restores
						the data after that. The test verifies that the backup&restore
						really impacts only the other drive, not KTestDrive.
@SYMTestActions			SQL Backup&Restore on a drive different than KTestDrive (C: by default).
@SYMTestExpectedResults Test must not fail
@SYMTestPriority		High
*/
void DbDriveTest()
	{
	CSqlBurCallback* backupClient = NULL;
	TRAPD(err, backupClient = CSqlBurCallback::NewL(*TheSqlSrvTestBurInterface));
	TEST2(err, KErrNone);
	TEST(backupClient != NULL);
	//
	TDriveUnit driveUnitDefault(KTestDrive);
	TDriveName driveNameDefault = driveUnitDefault.Name(); 
	//Find a r/w drive, different than KTestDrive
	TInt drive = EDriveA;
	for(;drive<=EDriveZ;++drive)
		{
		if(drive == KTestDrive)
			{
			continue;
			}
		TDriveInfo driveInfo;
		err = TheSqlSrvTestBurInterface->Fs().Drive(driveInfo, drive);
		if(err != KErrNone)
			{
			continue;
			}
		if(driveInfo.iDriveAtt & KDriveAttRom)
			{
			continue;
			}
		//
		TDriveUnit driveUnit(drive);
		TDriveName driveName = driveUnit.Name();
		//
		TVolumeInfo vinfo;
		err = TheSqlSrvTestBurInterface->Fs().Volume(vinfo, drive);
		if(err != KErrNone)
			{
			TheTest.Printf(_L("Drive %S, RFs::Volume() err=%d\r\n"), &driveName, err);
			continue;
			}
		//R/W drive found.
		TheTest.Printf(_L("Test drive: %S\r\n"), &driveName);
		TParse parse;
		err = parse.Set(KTestFile2, &driveName, 0);
		TEST2(err, KErrNone);
		//Create the test file that will be sent for backup
		TPtrC fname1(parse.FullName());
		TheTest.Printf(_L("Test file 1: %S\r\n"), &fname1);
		err = TheSqlSrvTestBurInterface->Fs().MkDirAll(parse.FullName());
		TEST(err == KErrNone || err == KErrAlreadyExists);
		(void)TheSqlSrvTestBurInterface->Fs().Delete(parse.FullName());
		RFile file;
		err = file.Replace(TheSqlSrvTestBurInterface->Fs(), parse.FullName(), EFileRead | EFileWrite);
		TEST2(err, KErrNone);
		_LIT8(KData1, "123456787989");
		err = file.Write(KData1);
		TEST2(err, KErrNone);
		err = file.Flush();
		TEST2(err, KErrNone);
		file.Close();
		//Create a test file with the same name on drive KTestDrive 
		err = parse.Set(KTestFile2, &driveNameDefault, 0);
		TEST2(err, KErrNone);
		TPtrC fname2(parse.FullName());
		TheTest.Printf(_L("Test file 2: %S\r\n"), &fname2);
		(void)TheSqlSrvTestBurInterface->Fs().Delete(parse.FullName());
		err = file.Replace(TheSqlSrvTestBurInterface->Fs(), parse.FullName(), EFileRead | EFileWrite);
		TEST2(err, KErrNone);
		_LIT8(KData2, "ABCDEFG");
		err = file.Write(KData2);
		TEST2(err, KErrNone);
		err = file.Flush();
		TEST2(err, KErrNone);
		file.Close();
		//Do the backup on "drive"
		TRAP(err, TestBackupL(*backupClient, TheSqlSrvTestBurInterface->Fs(), KTestClientUid2, (TDriveNumber)drive));
		TEST2(err, KErrNone);
		//Modify the file that was sent for backup
		err = parse.Set(KTestFile2, &driveName, 0);
		TEST2(err, KErrNone);
		err = file.Open(TheSqlSrvTestBurInterface->Fs(), parse.FullName(), EFileRead | EFileWrite);
		TEST2(err, KErrNone);
		_LIT8(KData3, "ABCDEFGHYYYYYY");
		err = file.Write(KData3);
		TEST2(err, KErrNone);
		err = file.Flush();
		TEST2(err, KErrNone);
		file.Close();
		//Do the restore on "drive"
		TRAP(err, TestRestoreL(*backupClient, TheSqlSrvTestBurInterface->Fs(), KTestClientUid2, (TDriveNumber)drive));
		TEST2(err, KErrNone);
		//Verify the content of the restored file
		err = parse.Set(KTestFile2, &driveName, 0);
		TEST2(err, KErrNone);
		err = file.Open(TheSqlSrvTestBurInterface->Fs(), parse.FullName(), EFileRead | EFileWrite);
		TEST2(err, KErrNone);
		TBuf8<50> data;
		err = file.Read(data);
		TEST2(err, KErrNone);
		file.Close();
		TEST(data == KData1);
		//Verify the content of the file on drive KTestDrive. It should be the same as before the backup&restore.
		err = parse.Set(KTestFile2, &driveNameDefault, 0);
		TEST2(err, KErrNone);
		err = file.Open(TheSqlSrvTestBurInterface->Fs(), parse.FullName(), EFileRead | EFileWrite);
		TEST2(err, KErrNone);
		err = file.Read(data);
		TEST2(err, KErrNone);
		file.Close();
		TEST(data == KData2);
		//Cleanup
		err = parse.Set(KTestFile2, &driveNameDefault, 0);
		TEST2(err, KErrNone);
		(void)TheSqlSrvTestBurInterface->Fs().Delete(parse.FullName());
		err = parse.Set(KTestFile2, &driveName, 0);
		TEST2(err, KErrNone);
		(void)TheSqlSrvTestBurInterface->Fs().Delete(parse.FullName());
		break;
		}
	delete backupClient;
	if(drive > EDriveZ)
		{
		TheTest.Printf(_L("No R/W drive has been found, different than %S\r\n"), &driveNameDefault);
		}
	}

/**
@SYMTestCaseID			PDS-SQL-UT-4229
@SYMTestCaseDesc		SQL Backup&Restore with locked file.
						The test creates two test files on the same drive and with the same uid.
						Then the test backups the databases. After the backup the test simulates that
						the first file is "in use". Then the test performs a restore.
						The expected result: the locked file is not restored but the other file is restored.
@SYMTestActions			SQL Backup&Restore with locked file.
@SYMTestExpectedResults Test must not fail
@SYMTestPriority		High
*/
void LockFileTest()
	{
	CSqlBurCallback* backupClient = NULL;
	TRAPD(err, backupClient = CSqlBurCallback::NewL(*TheSqlSrvTestBurInterface));
	TEST2(err, KErrNone);
	TEST(backupClient != NULL);
	(void)TheSqlSrvTestBurInterface->Fs().Delete(KTestFile3);
	(void)TheSqlSrvTestBurInterface->Fs().Delete(KTestFile4);
	//Create the files. File 1.
	RFile file;
	err = file.Replace(TheSqlSrvTestBurInterface->Fs(), KTestFile3, EFileRead | EFileWrite);
	TEST2(err, KErrNone);
	_LIT8(KData1, "123456787989");
	err = file.Write(KData1);
	TEST2(err, KErrNone);
	err = file.Flush();
	TEST2(err, KErrNone);
	file.Close();
	//...file 2
	err = file.Replace(TheSqlSrvTestBurInterface->Fs(), KTestFile4, EFileRead | EFileWrite);
	TEST2(err, KErrNone);
	_LIT8(KData2, "ABCDEF");
	err = file.Write(KData2);
	TEST2(err, KErrNone);
	err = file.Flush();
	TEST2(err, KErrNone);
	file.Close();
	//Backup
	TRAP(err, TestBackupL(*backupClient, TheSqlSrvTestBurInterface->Fs(), KTestClientUid3, KTestDrive));
	TEST2(err, KErrNone);
	//Modify the files. Keep the first file opened.
	RFile file1;
	err = file1.Open(TheSqlSrvTestBurInterface->Fs(), KTestFile3, EFileRead | EFileWrite);
	TEST2(err, KErrNone);
	_LIT8(KData3, "YYYYYQQQQQQQQQQQ");
	err = file1.Write(KData3);
	TEST2(err, KErrNone);
	err = file1.Flush();
	TEST2(err, KErrNone);
	//...file 2
	err = file.Open(TheSqlSrvTestBurInterface->Fs(), KTestFile4, EFileRead | EFileWrite);
	TEST2(err, KErrNone);
	_LIT8(KData4, "5545495444j32322332234223432");
	err = file.Write(KData4);
	TEST2(err, KErrNone);
	err = file.Flush();
	TEST2(err, KErrNone);
	file.Close();
	//Restore. The reported error should be KErrInUse.
	TRAP(err, TestRestoreL(*backupClient, TheSqlSrvTestBurInterface->Fs(), KTestClientUid3, KTestDrive));
	TEST2(err, KErrInUse);
	//Close file 1 and check the content. It should be the same as after the backup
	file1.Close();
	err = file.Open(TheSqlSrvTestBurInterface->Fs(), KTestFile3, EFileRead | EFileWrite);
	TEST2(err, KErrNone);
	TBuf8<50> data;
	err = file.Read(data);
	TEST2(err, KErrNone);
	file.Close();
	TEST(data == KData3);
	//File2:  check the content. It should be the same as before the backup
	err = file.Open(TheSqlSrvTestBurInterface->Fs(), KTestFile4, EFileRead | EFileWrite);
	TEST2(err, KErrNone);
	err = file.Read(data);
	TEST2(err, KErrNone);
	file.Close();
	TEST(data == KData2);
	//
	(void)TheSqlSrvTestBurInterface->Fs().Delete(KTestFile4);
	(void)TheSqlSrvTestBurInterface->Fs().Delete(KTestFile3);
	delete backupClient;
	}

/**
@SYMTestCaseID			PDS-SQL-UT-4230
@SYMTestCaseDesc		SQL Backup&Restore with locked file - test 2.
						The test creates two test files on the same drive and with different uids.
						Then the test backups the databases. After the backup the test simulates that
						the first file is "in use". Then the test performs a restore.
						The expected result: the locked file is not restored but the other file is restored.
@SYMTestActions			SQL Backup&Restore with locked file - test 2.
@SYMTestExpectedResults Test must not fail
@SYMTestPriority		High
*/
void LockFileTest2()
	{
	CSqlBurCallback* backupClient = NULL;
	TRAPD(err, backupClient = CSqlBurCallback::NewL(*TheSqlSrvTestBurInterface));
	TEST2(err, KErrNone);
	TEST(backupClient != NULL);
	(void)TheSqlSrvTestBurInterface->Fs().Delete(KTestFile1);//KTestClientUid1 used
	(void)TheSqlSrvTestBurInterface->Fs().Delete(KTestFile4);//KTestClientUid3 used
	//Create the files. File 1.
	RFile file;
	err = file.Replace(TheSqlSrvTestBurInterface->Fs(), KTestFile1, EFileRead | EFileWrite);
	TEST2(err, KErrNone);
	_LIT8(KData1, "123456787989");
	err = file.Write(KData1);
	TEST2(err, KErrNone);
	err = file.Flush();
	TEST2(err, KErrNone);
	file.Close();
	//...file 2
	err = file.Replace(TheSqlSrvTestBurInterface->Fs(), KTestFile4, EFileRead | EFileWrite);
	TEST2(err, KErrNone);
	_LIT8(KData2, "ABCDEF");
	err = file.Write(KData2);
	TEST2(err, KErrNone);
	err = file.Flush();
	TEST2(err, KErrNone);
	file.Close();
	//Backup
	TRAP(err, TestBackupL(*backupClient, TheSqlSrvTestBurInterface->Fs(), KTestClientUid1, KTestDrive));
	TEST2(err, KErrNone);
	TRAP(err, TestBackupL(*backupClient, TheSqlSrvTestBurInterface->Fs(), KTestClientUid3, KTestDrive));
	TEST2(err, KErrNone);
	//Modify the files. Keep the first file opened.
	RFile file1;
	err = file1.Open(TheSqlSrvTestBurInterface->Fs(), KTestFile1, EFileRead | EFileWrite);
	TEST2(err, KErrNone);
	_LIT8(KData3, "YYYYYQQQQQQQQQQQ");
	err = file1.Write(KData3);
	TEST2(err, KErrNone);
	err = file1.Flush();
	TEST2(err, KErrNone);
	//...file 2
	err = file.Open(TheSqlSrvTestBurInterface->Fs(), KTestFile4, EFileRead | EFileWrite);
	TEST2(err, KErrNone);
	_LIT8(KData4, "5545495444j32322332234223432");
	err = file.Write(KData4);
	TEST2(err, KErrNone);
	err = file.Flush();
	TEST2(err, KErrNone);
	file.Close();
	//Restore the first file. The reported error should be KErrInUse.
	TRAP(err, TestRestoreL(*backupClient, TheSqlSrvTestBurInterface->Fs(), KTestClientUid1, KTestDrive));
	TEST2(err, KErrInUse);
	//Restore the second file. The reported error should be KErrNone.
	TRAP(err, TestRestoreL(*backupClient, TheSqlSrvTestBurInterface->Fs(), KTestClientUid3, KTestDrive));
	TEST2(err, KErrNone);
	//Close file 1 and check the content. It should be the same as after the backup
	file1.Close();
	err = file.Open(TheSqlSrvTestBurInterface->Fs(), KTestFile1, EFileRead | EFileWrite);
	TEST2(err, KErrNone);
	TBuf8<50> data;
	err = file.Read(data);
	TEST2(err, KErrNone);
	file.Close();
	TEST(data == KData3);
	//File2:  check the content. It should be the same as before the backup
	err = file.Open(TheSqlSrvTestBurInterface->Fs(), KTestFile4, EFileRead | EFileWrite);
	TEST2(err, KErrNone);
	err = file.Read(data);
	TEST2(err, KErrNone);
	file.Close();
	TEST(data == KData2);
	//
	(void)TheSqlSrvTestBurInterface->Fs().Delete(KTestFile4);
	(void)TheSqlSrvTestBurInterface->Fs().Delete(KTestFile1);
	delete backupClient;
	}

CDir* GetPrivateDirContent(TDriveNumber aDrive)
	{
	TDriveUnit driveUnit(aDrive);
	TDriveName driveName = driveUnit.Name();
	TFileName path;
	path.Copy(driveName);
	path.Append(KPrivateDir);
	_LIT(KMatchAllDbFiles, "*");
	path.Append(KMatchAllDbFiles);
	//Do the search
	CDir* fileNameCol = NULL;
	TInt err = TheSqlSrvTestBurInterface->Fs().GetDir(path, KEntryAttNormal, ESortByName, fileNameCol);
	TEST2(err, KErrNone);
	return fileNameCol;
	}

/**
@SYMTestCaseID			PDS-SQL-UT-4231
@SYMTestCaseDesc		SQL Backup&Restore - directory content test.
						The test stores into an array information regarding all files in 
						SQL private datacage. Then the test backups one of the files and modifies
						the file after that. Then the test does a restore. Expected result - the only
						modifed file should be the file which was sent for backup.
@SYMTestActions			SQL Backup&Restore - directory content test.
@SYMTestExpectedResults Test must not fail
@SYMTestPriority		High
*/
void DirectoryContentTest()
	{
	CSqlBurCallback* backupClient = NULL;
	TRAPD(err, backupClient = CSqlBurCallback::NewL(*TheSqlSrvTestBurInterface));
	TEST2(err, KErrNone);
	TEST(backupClient != NULL);
	(void)TheSqlSrvTestBurInterface->Fs().Delete(KTestFile1);//KTestClientUid1 used
	//Create the file
	RFile file;
	err = file.Replace(TheSqlSrvTestBurInterface->Fs(), KTestFile1, EFileRead | EFileWrite);
	TEST2(err, KErrNone);
	_LIT8(KData1, "123456787989");
	err = file.Write(KData1);
	TEST2(err, KErrNone);
	err = file.Flush();
	TEST2(err, KErrNone);
	file.Close();
	//Store file entries into an array
	CDir* dirBeforeBackup = GetPrivateDirContent(KTestDrive);
	//Backup
	TRAP(err, TestBackupL(*backupClient, TheSqlSrvTestBurInterface->Fs(), KTestClientUid1, KTestDrive));
	TEST2(err, KErrNone);
	//Check dir content
	CDir* dirAfterBackup = GetPrivateDirContent(KTestDrive);
	TEST2(dirBeforeBackup->Count(), dirAfterBackup->Count());
	for(TInt i=0;i<dirBeforeBackup->Count();++i)
		{
		const TEntry& entry1 = (*dirBeforeBackup)[i];
		const TEntry& entry2 = (*dirAfterBackup)[i];
		TheTest.Printf(_L("Entry1=%S, Entry2=%S\r\n"), &entry1.iName, &entry2.iName);
		TBool rc = entry1.iAtt == entry2.iAtt;
		TEST(rc);
		rc = entry1.iSize == entry2.iSize;
		TEST(rc);
		rc = entry1.iModified == entry2.iModified;
		TEST(rc);
		rc = entry1.iType == entry2.iType;
		TEST(rc);
		rc = entry1.iName == entry2.iName;
		TEST(rc);
		}
	delete dirAfterBackup;
	//Modify the file
	err = file.Replace(TheSqlSrvTestBurInterface->Fs(), KTestFile1, EFileRead | EFileWrite);
	TEST2(err, KErrNone);
	_LIT8(KData2, "ABCDEF");
	err = file.Write(KData2);
	TEST2(err, KErrNone);
	err = file.Flush();
	TEST2(err, KErrNone);
	file.Close();
	//Restore
	User::After(2000000);//To force a change in the file time stamp (the restored file time stamp).
	TRAP(err, TestRestoreL(*backupClient, TheSqlSrvTestBurInterface->Fs(), KTestClientUid1, KTestDrive));
	TEST2(err, KErrNone);
	//Check dir content
	CDir* dirAfterRestore = GetPrivateDirContent(KTestDrive);
	TEST2(dirBeforeBackup->Count(), dirAfterRestore->Count());
	for(TInt i=0;i<dirBeforeBackup->Count();++i)
		{
		const TEntry& entry1 = (*dirBeforeBackup)[i];
		const TEntry& entry2 = (*dirAfterRestore)[i];
		TheTest.Printf(_L("Entry1=%S, Entry2=%S\r\n"), &entry1.iName, &entry2.iName);
		TBool rc = entry1.iAtt == entry2.iAtt;
		TEST(rc);
		rc = entry1.iSize == entry2.iSize;
		TEST(rc);
		if(entry1.iName.FindF(KTestFile1NameOnly) >= 0)
			{
			rc = entry1.iModified != entry2.iModified;
			}
		else
			{
			rc = entry1.iModified == entry2.iModified;
			}
		TEST(rc);
		rc = entry1.iType == entry2.iType;
		TEST(rc);
		rc = entry1.iName == entry2.iName;
		TEST(rc);
		}
	delete dirAfterRestore;
	//
	delete dirBeforeBackup;
	(void)TheSqlSrvTestBurInterface->Fs().Delete(KTestFile1);
	delete backupClient;
	}

/**
@SYMTestCaseID			PDS-SQL-UT-4232
@SYMTestCaseDesc		SQL Backup&Restore - large file test.
						Backup and restore with a file with size bigger than 1 Mb.
@SYMTestActions			SQL Backup&Restore - large file test.
@SYMTestExpectedResults Test must not fail
@SYMTestPriority		High
*/
void LargeFileTest()
	{
	CSqlBurCallback* backupClient = NULL;
	TRAPD(err, backupClient = CSqlBurCallback::NewL(*TheSqlSrvTestBurInterface));
	TEST2(err, KErrNone);
	TEST(backupClient != NULL);
	(void)TheSqlSrvTestBurInterface->Fs().Delete(KTestFile1);//KTestClientUid1 used
	(void)TheSqlSrvTestBurInterface->Fs().Delete(KTestFile2);//KTestClientUid2 used
	//Allocate buffer for the data
	const TInt KDataBufSize = 100000;
	HBufC8* dataBuf = HBufC8::New(KDataBufSize);
	TEST(dataBuf != NULL);
	TPtr8 dataPtr = dataBuf->Des();
	//Create file 1
	const TInt KFileSize1 = 1201345;
	RFile file;
	err = file.Replace(TheSqlSrvTestBurInterface->Fs(), KTestFile1, EFileRead | EFileWrite);
	TEST2(err, KErrNone);
	dataPtr.SetLength(dataPtr.MaxLength());
	const TChar KChar1(0x5A);
	dataPtr.Fill(KChar1);
	TInt len = KFileSize1;
	while(len > 0)
		{
		TInt blockSize = Min(len, dataPtr.MaxLength()); 
		err = file.Write(dataPtr, blockSize);
		TEST2(err, KErrNone);
		len -= blockSize;
		}
	err = file.Flush();
	TEST2(err, KErrNone);
	file.Close();
	//Create file 2
	const TInt KFileSize2 = 1387651;
	err = file.Replace(TheSqlSrvTestBurInterface->Fs(), KTestFile2, EFileRead | EFileWrite);
	TEST2(err, KErrNone);
	dataPtr.SetLength(dataPtr.MaxLength());
	const TChar KChar2(0xD5);
	dataPtr.Fill(KChar2);
	len = KFileSize2;
	while(len > 0)
		{
		TInt blockSize = Min(len, dataPtr.MaxLength()); 
		err = file.Write(dataPtr, blockSize);
		TEST2(err, KErrNone);
		len -= blockSize;
		}
	err = file.Flush();
	TEST2(err, KErrNone);
	file.Close();
	//Backup
	TRAP(err, TestBackupL(*backupClient, TheSqlSrvTestBurInterface->Fs(), KTestClientUid1, KTestDrive));
	TEST2(err, KErrNone);
	TRAP(err, TestBackupL(*backupClient, TheSqlSrvTestBurInterface->Fs(), KTestClientUid2, KTestDrive));
	TEST2(err, KErrNone);
	//Delete the files
	(void)TheSqlSrvTestBurInterface->Fs().Delete(KTestFile1);
	(void)TheSqlSrvTestBurInterface->Fs().Delete(KTestFile2);
	//Restore
	TRAP(err, TestRestoreL(*backupClient, TheSqlSrvTestBurInterface->Fs(), KTestClientUid1, KTestDrive));
	TEST2(err, KErrNone);
	TRAP(err, TestRestoreL(*backupClient, TheSqlSrvTestBurInterface->Fs(), KTestClientUid2, KTestDrive));
	TEST2(err, KErrNone);
	//Check restored files content
	const TPtrC KFileNames[] = {KTestFile1(), KTestFile2()};
	const TInt KFileSizes[] =  {KFileSize1, KFileSize2};
	const TChar KSymbols[] =  {KChar1, KChar2};
	for(TInt i=0;i<(sizeof(KFileNames)/sizeof(KFileNames[i]));++i)
		{
		err = file.Open(TheSqlSrvTestBurInterface->Fs(), KFileNames[i], EFileRead);
		TEST2(err, KErrNone);
		len = 0;
		err = file.Size(len);
		TEST2(err, KErrNone);
		TEST2(len, KFileSizes[i]);
		while(len > 0)
			{
			TInt blockSize = Min(len, dataPtr.MaxLength());
			err = file.Read(dataPtr, blockSize);
			TEST2(err, KErrNone);
			len -= blockSize;
			for(TInt j=0;j<dataPtr.Length();++j)
				{
				TEST(dataPtr[j] == KSymbols[i]);
				}
			}
		file.Close();
		}
	//
	delete dataBuf;
	(void)TheSqlSrvTestBurInterface->Fs().Delete(KTestFile2);
	(void)TheSqlSrvTestBurInterface->Fs().Delete(KTestFile1);
	delete backupClient;
	}

void DoMain()
	{
	TestEnvCreate();

	TheTest.Start(_L("Store db content to memory buffer"));
	StoreDbContentToBuf(TheSqlSrvTestBurInterface->Fs(), KTestDrive, KClientUid);
		
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-4002 Backup: functional test "));
	FunctionalTest();

	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-4003 Backup: OOM test "));
	OomTest();

	TheTest.Next(_L(" @SYMTestCaseID:PDS-SQL-UT-4143 Backup&Restore: functional test 2"));
	FunctionalTest2();

	TheTest.Next(_L(" @SYMTestCaseID:PDS-SQL-UT-4144 Backup&Restore: legacy file format test"));
	LegacyFileFormatTest();
	
	TheTest.Next(_L(" @SYMTestCaseID:PDS-SQL-UT-4192 Backup&Restore: empty backup file list"));
	EmptyBackupFileListTest();
	
	TheTest.Next(_L(" @SYMTestCaseID:PDS-SQL-UT-4193 Backup: File I/O error simulation test"));
	BackupRestoreFileIoErrTest();
	
	TheTest.Next(_L(" @SYMTestCaseID:PDS-SQL-UT-4225 Zero size file - backup test"));	
	BackupZeroSizeFileTest();
	
	TheTest.Next(_L(" @SYMTestCaseID:PDS-SQL-UT-4226 Restore test - corrupted archive 1"));	
	CorruptedArchiveTest1();

	TheTest.Next(_L(" @SYMTestCaseID:PDS-SQL-UT-4227 Restore test - corrupted archive 2"));	
	CorruptedArchiveTest2();

	TheTest.Next(_L(" @SYMTestCaseID:PDS-SQL-UT-4228 Backup&Restore test on a drive different than KTestDrive (C: by default)"));
	DbDriveTest();
	
	TheTest.Next(_L(" @SYMTestCaseID:PDS-SQL-UT-4229 Backup&Restore test with locked file"));
	LockFileTest();
	
	TheTest.Next(_L(" @SYMTestCaseID:PDS-SQL-UT-4230 Backup&Restore test with locked file 2"));
	LockFileTest2();

	TheTest.Next(_L(" @SYMTestCaseID:PDS-SQL-UT-4231 Backup&Restore - directory content test"));
	DirectoryContentTest();

	TheTest.Next(_L(" @SYMTestCaseID:PDS-SQL-UT-4232 Backup&Restore - large file test"));
	LargeFileTest();
	
	TestEnvDestroy();
	}

TInt E32Main()
	{
	TheTest.Title();
	
	CTrapCleanup* tc = CTrapCleanup::New();
	TheTest(tc != NULL);
	
	__UHEAP_MARK;
	
	DoMain();
	
	__UHEAP_MARKEND;
	
	TheTest.End();
	TheTest.Close();
	
	delete tc;
	
	User::Heap().Check();
	return KErrNone;
	}
