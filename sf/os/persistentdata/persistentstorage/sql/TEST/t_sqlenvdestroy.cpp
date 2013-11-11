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
// SQL security - deleting secure test databases from C:
// Please, ensure that t_sqlenvdestroy test is executed after the other sql security tests
// 
//

#include <e32test.h>
#include <bautils.h>

RTest TheTest(_L("t_sqlenvdestroy - deleting secure test databases from C:"));

_LIT(KDbCFileName1, "c:\\private\\10281e17\\[21212122]AADb2.db");
_LIT(KDbCFileName2, "c:\\private\\10281e17\\[21212122]BBDb2.db");
_LIT(KDbCFileName3, "c:\\private\\10281e17\\[21212125]T_AB.db");
_LIT(KDbCFileName4, "c:\\private\\10281e17\\[1111CCCC]T_AB.db");
_LIT(KDbCFileName5, "c:\\private\\10281e17\\[21212125]T_OneDefPolicy.db");

_LIT(KDb1, "c:\\private\\10281e17\\[98765432]t_nosettings.db");
_LIT(KDb2, "c:\\private\\10281e17\\[98765432]t_emptysettings.db");
_LIT(KDb3, "c:\\private\\10281e17\\[98765432]t_invversion.db");
_LIT(KDb4, "c:\\private\\10281e17\\[98765432]t_nocollation.db");
_LIT(KDb5, "c:\\private\\10281e17\\[98765432]t_longcollation.db");
_LIT(KDb6, "c:\\private\\10281e17\\[98765432]t_nodefaultpolicy.db");
_LIT(KDb7, "c:\\private\\10281e17\\[98765432]t_invobject.db");
_LIT(KDb8, "c:\\private\\10281e17\\[98765432]t_2defaultpolicies.db");

_LIT(KPrivateSubDir, "c:\\private\\10281e17\\cfg-TestDir.db\\");

TParse TheParse;

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
//Test macros and functions
static void Check(TInt aValue, TInt aExpected, TInt aLine)
	{
	if(aValue != aExpected)
		{
		RDebug::Print(_L("*** Expected error: %d, got: %d\r\n"), aExpected, aValue);
		TheTest(EFalse, aLine);
		}
	}
#define TEST2(aValue, aExpected) ::Check(aValue, aExpected, __LINE__)

///////////////////////////////////////////////////////////////////////////////////////

void DoDeleteFile(RFs& aFs, const TDesC& aFilePath)
	{
	TInt err = BaflUtils::DeleteFile(aFs, aFilePath);
	if(err != KErrNone && err != KErrNotFound)
		{
		TheTest.Printf(_L("Error %d deleting \"%S\" file.\n"), err, &aFilePath);
		}
	}

void PrintDiskUsage(RFs& aFs, const TDesC& aPath, TInt aOffset = 0)
	{
	_LIT(KSpace, " ");
	TheTest.Printf(_L("%*.*S%S\r\n"), aOffset, aOffset, &KSpace, &aPath);
	TFindFile findFile(aFs);
	CDir* fileNameCol = NULL;
	TBuf<8> fileNameMask;
	fileNameMask.Copy(_L("*.*"));
	TInt err = findFile.FindWildByDir(fileNameMask, aPath, fileNameCol);
	if(err == KErrNone)
		{
		do
			{
			const TDesC& file = findFile.File();//"file" variable contains the drive and the path. the file name in "file" is invalid in this case.
			(void)TheParse.Set(file, NULL, NULL);
			TPtrC driveName = TheParse.Drive();
			if(aPath.FindF(driveName) >= 0)
				{		
                TInt cnt = fileNameCol->Count();
                for(TInt i=0;i<cnt;++i)
                    {
                    const ::TEntry& entry = (*fileNameCol)[i];
                    if(!entry.IsDir())
                        {
                        TheTest.Printf(_L("%*.*S    %S, size=%d\r\n"), aOffset, aOffset, &KSpace, &entry.iName, entry.iSize);
                        }
                    else
                        {
                        TBuf<100> path;
                        path.Copy(aPath);
                        path.Append(entry.iName);
                        path.Append(_L("\\"));
                        PrintDiskUsage(aFs, path, aOffset + 4);
                        }
                    }
				} // if(aPath.FindF(driveName) >= 0)
			
			delete fileNameCol;
			fileNameCol = NULL;
			} while((err = findFile.FindWild(fileNameCol)) == KErrNone);//Get the next set of files
		}
	else
		{
		TheTest.Printf(_L("  FindWildByDir() failed with err=%d\r\n"), err);
		}
	}

void DoRun()
	{
    RFs fs;
	TInt err = fs.Connect();
	TEST2(err, KErrNone);

	TheTest.Start(_L("  @SYMTestCaseID:SYSLIB-SQL-LEGACY-T_SQLENVDESTROY-0001 Delete secure databases from C: "));

	DoDeleteFile(fs, KDbCFileName1);
	DoDeleteFile(fs, KDbCFileName2);
	DoDeleteFile(fs, KDbCFileName3);
	DoDeleteFile(fs, KDbCFileName4);
	DoDeleteFile(fs, KDbCFileName5);

	DoDeleteFile(fs, KDb1);
	DoDeleteFile(fs, KDb2);
	DoDeleteFile(fs, KDb3);
	DoDeleteFile(fs, KDb4);
	DoDeleteFile(fs, KDb5);
	DoDeleteFile(fs, KDb6);
	DoDeleteFile(fs, KDb7);
	DoDeleteFile(fs, KDb8);

	TheTest.Printf(_L("====================================================\r\n"));
	PrintDiskUsage(fs, _L("c:\\"));
	TheTest.Printf(_L("====================================================\r\n"));
	
	//Remove the created subdir in the private datacage. 
	err = fs.RmDir(KPrivateSubDir);
	if(err != KErrNone && err != KErrNotFound)
		{
		TheTest.Printf(_L("Error %d deleting \"%S\" directory.\n"), err, &KPrivateSubDir);
		}
	
	fs.Close();
	}

TInt E32Main()
    {
	TheTest.Title();

	CTrapCleanup* tc = CTrapCleanup::New();

	__UHEAP_MARK;

	DoRun();

	__UHEAP_MARKEND;

	TheTest.End();
	TheTest.Close();

	delete tc;

	User::Heap().Check();
	return KErrNone;
    }
