// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <f32file.h>
#include <e32debug.h>

// This function is used in the test code to create dummy test files to verify
//that these files are deleted by the task scheduler on startup 
_LIT(KTaskFile1Name, "_:\\Private\\10005399\\12345678901234567890.tmp");
_LIT(KTaskFile2Name, "_:\\Private\\10005399\\98765432100123456789.tmp");
_LIT(KTaskFile3Name, "_:\\Private\\10005399\\98765432109876543210.tmp");

static TInt CreateFileL(const TDesC& aFileName)
	{
	RFs fs;
	RFile taskFile;
	
	User::LeaveIfError(fs.Connect());
	
	//Get the correct system drive
	TBuf<64> filePath(aFileName);
	filePath[0] = RFs::GetSystemDriveChar();
	
	//Create the temp file
	TInt res = taskFile.Create(fs,filePath,EFileRead);
	taskFile.Close();
	
	fs.Close();
	
	return res;	
	}
	
static TInt CreateTaskFilesL()
	{

	//Create the temp file
	TInt res = CreateFileL(KTaskFile1Name);
		
	if(res ==KErrNone)
	{
	res = CreateFileL(KTaskFile2Name);	
	}
	
	if(res ==KErrNone)
	{
	res = CreateFileL(KTaskFile3Name);	
	}
	
	return res;
	}
		

GLDEF_C TInt E32Main()
	{
	TInt err = KErrNoMemory;
	CTrapCleanup* cleanup=CTrapCleanup::New();	//can fail
	if (cleanup)
		{
		err = KErrNone;
		TRAP(err, CreateTaskFilesL())
		delete cleanup;
		}
	return err;
	}
