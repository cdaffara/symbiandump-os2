// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "t_cenrep_helper.h"
#include "srvdefs.h"
#include <f32file.h> // RFs
#include <e32test.h> // RTest
#include <hal.h>
#include "cachemgr.h"
#include "clirep.h"

#if defined(__CENTREP_SERVER_PERFTEST__) || defined(__CENTREP_SERVER_MEMTEST__) || defined(__CENTREP_SERVER_CACHETEST__)
	#include "srvreqs.h"
	#define CONDITIONAL_PARAM(x) x
#else
	#define CONDITIONAL_PARAM(x)
#endif

void PatchDrive(TDes& aPath)
	{
	TDriveNumber sysdrive = RFs::GetSystemDrive();
	aPath[0] = 'a' + sysdrive-EDriveA; // Replace drive letter only.
	}

EXPORT_C void printDirL(const TDesC& aDirName)
	{
	CDir* fileList=NULL;
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);

	TInt r = fs.GetDir(aDirName,KEntryAttNormal, ESortByDate, fileList);

	if (r==KErrPathNotFound)
		{
		RDebug::Print(_L("No directory %S"), &aDirName);
		}
	else if (r==KErrNone)
		{
		TInt fileCount=fileList->Count();
		RDebug::Print( _L("%02d files in %S\n"),fileCount, &aDirName);
		for (TInt i = 0;i < fileCount; ++i)
			{
			TEntry entry=(*fileList)[i];

			RDebug::Print( _L("File[%02d] - %S \n"),
							i,
							&(entry.iName)
							);
			}
		}
	else
		{
		RDebug::Print(_L("Error getting contents of directory %S"), &aDirName);
		}
	delete fileList;
	CleanupStack::PopAndDestroy();	//fs
	}

EXPORT_C TInt KillProcess(const TDesC& aProcessName)
	{
	TFullName name;

	RDebug::Print(_L("Find and kill \"%S\" process.\n"), &aProcessName);

	TBuf<64> pattern(aProcessName);
	TInt length = pattern.Length();
	pattern += _L("*");
	TFindProcess procFinder(pattern);

	while (procFinder.Next(name) == KErrNone)
		{
		if (name.Length() > length)
			{//If found name is a string containing aProcessName string.
			TChar c(name[length]);
			if (c.IsAlphaDigit() ||
				c == TChar('_') ||
				c == TChar('-'))
				{
				// If the found name is other valid application name
				// starting with aProcessName string.
				RDebug::Print(_L(":: Process name: \"%S\".\n"), &name);
				continue;
				}
			}
		RProcess proc;
		if (proc.Open(name) == KErrNone)
			{
			proc.Kill(0);
			RDebug::Print(_L("\"%S\" process killed.\n"), &name);
			}
		proc.Close();
		}
	return KErrNone;
	}

const TInt KSmallDelay = 2*1000;

//File cleanup function
EXPORT_C void CleanupCDriveL(TBool aRemoveRomCache)
	{
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);

	CFileMan* fm = CFileMan::NewL(fs);
	CleanupStack::PushL(fm);
	HBufC* file_buf = HBufC::NewLC(KMaxFileName);
	TPtr file(file_buf->Des());
	file.Copy(KCPersistsFiles);
	PatchDrive(file);
	TInt r = fm->Delete(file);

	if (r != KErrNone &&
		r != KErrNotFound &&
		r != KErrPathNotFound)
		{
		User::Leave(r);
		}
		
	if(aRemoveRomCache)
		{
		//Delete cached rom version file
		file.Copy(KCRomVersionFiles);
		PatchDrive(file);
		fm->Attribs(file, 0, KEntryAttReadOnly, TTime(0), 0);
		r = fm->Delete(file);
		if (r != KErrNone &&
			r != KErrNotFound &&
			r != KErrPathNotFound &&
			r != KErrPermissionDenied)
			{
			User::Leave(r);
			}
		
		// Delete all install files
		file.Copy(KCInstallFiles);
		PatchDrive(file);	
		r = fm->Delete(file);
		if (r != KErrNone &&
			r != KErrNotFound &&
			r != KErrPathNotFound)
			{
			User::Leave(r);
			}

		// Give SW time to handle uninstall.
		User::After(KSmallDelay);
		}
	CleanupStack::PopAndDestroy(3);
	}

//Function to remove all repositories from repository cache
//Try not to use this function because it is time consuming (129.5 seconds)
EXPORT_C void CleanupRepositoryCache()
	{
	// So we wait here until the cache is empty to correct the behaviour.
	User::After(KDefaultEvictionTimeout+950000);
    }

//Function to clean specific repository files
EXPORT_C void CleanupFileFromCDriveL(const TUid aRepository)
	{
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);

	CFileMan* fm = CFileMan::NewL(fs);
	CleanupStack::PushL(fm);
	
	HBufC* file_buf = HBufC::NewLC(KMaxFileName);
	TPtr filename(file_buf->Des());
	filename = KCPersistsDir;
	filename.AppendNumFixedWidth(aRepository.iUid,EHex,8);
	filename.Append(KTxtFileExt);
	PatchDrive(filename);
	// Delete txt file from persists dir
	TInt r = fm->Delete(filename);

	if (r != KErrNone &&
		r != KErrNotFound &&
		r != KErrPathNotFound)
		{
		User::Leave(r);
		}

	filename = KCPersistsDir;
	filename.AppendNumFixedWidth(aRepository.iUid,EHex,8);
	filename.Append(KCreFileExt);
	PatchDrive(filename);
	// Delete cre file from persists dir
	r = fm->Delete(filename);

	if (r != KErrNone &&
		r != KErrNotFound &&
		r != KErrPathNotFound)
		{
		User::Leave(r);
		}

	filename = KCInstallDir;
	filename.AppendNumFixedWidth(aRepository.iUid,EHex,8);
	filename.Append(KTxtFileExt);	
	PatchDrive(filename);
	// Delete txt file from install dir	
	r = fm->Delete(filename);
	if (r != KErrNone &&
		r != KErrNotFound &&
		r != KErrPathNotFound)
		{
		User::Leave(r);
		}

	filename = KCInstallDir;
	filename.AppendNumFixedWidth(aRepository.iUid,EHex,8);
	filename.Append(KCreFileExt);	
	PatchDrive(filename);	
	// Delete cre file from install dir	
	r = fm->Delete(filename);
	if (r != KErrNone &&
		r != KErrNotFound &&
		r != KErrPathNotFound)
		{
		User::Leave(r);
		}
 
	// Give SW time to handle uninstall.
	User::After(KSmallDelay);

	CleanupStack::PopAndDestroy(3);
	}	
	
//This function copies files from a source folder to a target folder
//and sets the file attributes to archive and read only
EXPORT_C void CopyTestFilesL(CFileMan& aFm, const TDesC& aSrc, const TDesC& aDest)
	{
	TBuf<KMaxFileName> src, dest;	
	src.Copy(aSrc);	
	dest.Copy(aDest);
				
	PatchDrive(dest);

	//copy test files
	User::LeaveIfError(aFm.Copy(src, dest,CFileMan::ERecurse));
	aFm.Attribs(dest,
					KEntryAttArchive,
					KEntryAttReadOnly,
					TTime(0),
					CFileMan::ERecurse);		
	}

//This function prints out the recorded time in milliseconds of aTime.
EXPORT_C void RecordPerformanceTimingL(TUint32 aTime)
	{
	TInt freq = 0;
	TInt Err = HAL::Get(HAL::EFastCounterFrequency, freq);
	if(Err != KErrNone)
	{
		RDebug::Print(_L("HAL error <%d>\r\n"), Err);
	}
	const TInt KMicroSecIn1Sec = 1000000;
	const TInt KMsIn1Sec = 1000;

	double v = ((double)aTime * KMicroSecIn1Sec) / (double)freq; TInt v2 = (TInt)v;
	RDebug::Print(_L("####Execution time: %d ms\r\n"), v2 / KMsIn1Sec);
	}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
//Test macroes and functions

EXPORT_C void CheckL(RTest& aTest, TInt aValue, TInt aLine)
	{
	if (!aValue)
		{
		CleanupCDriveL();
		aTest(EFalse, aLine);
		}
	}
EXPORT_C void CheckL(RTest& aTest, TInt aValue, TInt aExpected, TInt aLine)
	{
	if (aValue != aExpected)
		{
		RDebug::Print(_L("*** Expected error: %d, got: %d\r\n"),
					  aExpected, aValue);
		CleanupCDriveL();
		aTest(EFalse, aLine);
		}
	}

/**
 *Retrieves transaction state of the session.
*/
EXPORT_C TInt TransactionState(CRepository* aRep)
	{
	return (static_cast<CClientRepository*>(aRep))->TransactionState();
	}

/**
 *Sends EGetSetParameters to server.
*/
EXPORT_C TInt SetGetParameters(const TIpcArgs& CONDITIONAL_PARAM(aArgs))
	{
#if defined(__CENTREP_SERVER_PERFTEST__) || defined(__CENTREP_SERVER_MEMTEST__) || defined(__CENTREP_SERVER_CACHETEST__)
	RRepositorySession session;
	TInt ret = session.Connect();
	if (ret == KErrNone)
		{
		ret = session.SendReceive(EGetSetParameters, aArgs);
		session.Close();
		}
	return ret;
#else
	return KErrNotSupported;
#endif
	}
