// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// TTaskFilesChecker.cpp
// 
//

#include <f32file.h>
#include <e32debug.h>

// This function is used in the test code to check if there's any task files left
// after scheduled tasks completed

_LIT(KTaskFileScanDir, "_:\\Private\\10005399\\");

_LIT(KTaskFileWildname, "*.tmp");

static TInt CheckTaskFilesL()
	{
	RFs fs;
	User::LeaveIfError(fs.Connect());
	
	//Get the correct system drive
	TBuf<32> taskFilePath(KTaskFileScanDir);
	taskFilePath[0] = RFs::GetSystemDriveChar();
	
	// Search for task files using wildcard file name
	TFindFile fileFinder(fs);
	CDir* fileList = NULL;
	TInt rel = fileFinder.FindWildByDir(KTaskFileWildname, taskFilePath, fileList);
	
	// delete file list, we won't use it
	if (fileList != NULL)
		delete fileList;
						
	// When any task file found left, leave with error code KErrGeneral
	if (rel == KErrNone)
		rel = KErrGeneral;
		
	// When there's no task file left, return KErrNone
	if (rel == KErrNotFound)
		rel = KErrNone;
		
	// Leave with KErrGeneral or any other error	
	User::LeaveIfError(rel);
	return rel;
	}
	

GLDEF_C TInt E32Main()
	{
	TInt err = KErrNoMemory;
	CTrapCleanup* cleanup=CTrapCleanup::New();	//can fail
	if (cleanup)
		{
		err = KErrNone;
		TRAP(err, CheckTaskFilesL())
		delete cleanup;
		}
	return err;
	}
