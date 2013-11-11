// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
//  Name        : tlargefile.h

#ifndef __TESTLARGEFILE_H__
#define __TESTLARGEFILE_H__

#include <test/TestExecuteStepBase.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <spawn.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <ftw.h>
#include <wchar.h>
#include <e32std.h>
#include <sys/types.h>
#include <dirent.h>
#include <glob.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/limits.h>

_LIT(KTestLargeFileTruncate, "TestLargeFileTruncate"); 
_LIT(KTestLargeFileFtruncate, "TestLargeFileFtruncate");
_LIT(KTestLargeFileReadWrite, "TestLargeFileReadWrite"); 
_LIT(KTestLargeFileFseeknTell, "TestLargeFileFseeknTell");
_LIT(KTestLargeFileLseeknTell, "TestLargeFileLseeknTell"); 
_LIT(KTestCreatenFseeknPos, "TestCreatenFseeknPos");
_LIT(KTestCreatenLseeknPos, "TestCreatenLseeknPos"); 
_LIT(KTestChildFileTransfer, "TestChildFileTransfer"); 
_LIT(KTestLargeFileFtw, "TestLargeFileFtw"); 
_LIT(KTestLargeFileReaddir, "TestLargeFileReaddir");
_LIT(KTestLargeFileAlphasort, "TestLargeFileAlphasort");
_LIT(KTestLargeFileWreaddir, "TestLargeFileWreaddir");
_LIT(KTestNonsortScandir, "TestNonsortScandir"); 
_LIT(KTestSortScandir, "TestSortScandir"); 
_LIT(KTestFcntlwithSinglefd, "TestFcntlwithSinglefd"); 
_LIT(KTestFcntlwithDoublefds, "TestFcntlwithDoublefds");
_LIT(KTestLargeFileSeek, "TestLargeFileSeek");
_LIT(KTestLargeFileGlob, "TestLargeFileGlob");
_LIT(KTestLargeFileMmapWrite, "TestLargeFileMmapWrite");
_LIT(KTestLargeFileWriteMmap, "TestLargeFileWriteMmap");
_LIT(KTestNegSmallOffsetValues, "TestNegSmallOffsetValues");
_LIT(KTestNegLargeOffsetValues, "TestNegLargeOffsetValues");
_LIT(KTestLargeFileFreopen, "TestLargeFileFreopen");
_LIT(KTestLargeFileWfreopen, "TestLargeFileWfreopen");
_LIT(KTestFcntlWriteLockwithMaxSize, "TestFcntlWriteLockwithMaxSize");



//Enum defined to select a specific api for file creation that is used in helper function
enum TApiselection
    {
    ECreate,		//largefile creation is done using create64()
    EOpen,			//largefile creation is done using open64() without O_LARGEFILE flag
    EMkstemp,		//largefile creation is done using mkstemp64()
    EFopen,			//largefile creation is done using fopen64()
    ETmpfile,		//largefile creation is done using tmpfile64()
    EWcreate,		//largefile creation is done using wcreate64()
    EWopen,			//largefile creation is done using wopen64()
    EWfopen,		//largefile creation is done using wfopen64()
    EOpenwithO_largefile	//largefile creation is done using open64() with O_LARGEFILE flag
    };



class CTestLargefile : public CTestStep
	{
public:
	~CTestLargefile(); 
	CTestLargefile(const TDesC& aStepName);
	TVerdict doTestStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();
private:
	FILE* CreateLargeFile(char *aLargefilename, int aEnumtocreatelargefile);
	TInt GetStringAndCreateDir(char *aLargefile);
	TInt GetFilenamesAndCreateDir(char *aTestdirectory, char *aLargefile, char *aRegularfile);
	TInt CreateFilesAndTruncate(char *aLargefile, char *aRegularfile);
	TInt CleanUpSingleFile(FILE *fp, char *aUnlinkFile);
	TInt CleanUpDoubleFile(char *aLargeFile, char *aRegularFile, char *aTestdir);
	TVerdict TestLargeFileTruncate();
	TVerdict TestLargeFileFtruncate();
	TVerdict TestLargeFileReadWrite();
	TVerdict TestLargeFileFseeknTell();
	TVerdict TestLargeFileLseeknTell();
	TVerdict TestCreatenFseeknPos();
	TVerdict TestCreatenLseeknPos();
	TVerdict TestChildFileTransfer();
	TVerdict TestLargeFileFtw();
	TVerdict TestLargeFileReaddir();
	TVerdict TestLargeFileAlphasort();
	TVerdict TestLargeFileWreaddir();
	TVerdict TestNonsortScandir();
	TVerdict TestSortScandir();
	TVerdict TestFcntlwithSinglefd();
	TVerdict TestFcntlwithDoublefds();
	TVerdict TestLargeFileSeek();
	TVerdict TestLargeFileGlob();
	TVerdict TestLargeFileMmapWrite();
	TVerdict TestLargeFileWriteMmap();
	TVerdict TestNegSmallOffsetValues();
	TVerdict TestNegLargeOffsetValues();
	TVerdict TestLargeFileFreopen();
	TVerdict TestLargeFileWfreopen();
	TVerdict TestFcntlWriteLockwithMaxSize();
	};
#endif 
