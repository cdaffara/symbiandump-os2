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
// Tprocesskiller.cpp
// 
//

#include <f32file.h>
#include <e32debug.h>
	
//This function is used in the test code to kill SCHSVR or MinimalTaskHandler
//processes (or some other) when they leftover and may cause OOM condinitons.
static TInt DeleteScheduleFilesL()
	{
	RFs fs;
	_LIT(KFilePath, "_:\\Private\\10005399\\*.*");
	
	//Get the correct system drive
	TBuf<32> filePath(KFilePath);
	filePath[0] = RFs::GetSystemDriveChar();
	
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	CFileMan* fileManager = CFileMan::NewL(fs);
	CleanupStack::PushL(fileManager);
	TInt ret = fileManager->Delete(filePath);
	if(ret != KErrNone && ret != KErrNotFound && ret != KErrPathNotFound)
		{
		RDebug::Print(_L("DeleteScheduleFilesL - err=%d\n"), ret);
		//Resource/memory/file deallocation is not the right place for User::Leave() call!
		//Sometimes fileManager->Delete() fails with error "-14" (KErrInUse), 
		//because SCHSVR server is alive!
		//User::Leave(ret);
		}
	CleanupStack::PopAndDestroy(fileManager);
	CleanupStack::PopAndDestroy(); //fs
	return KErrNone;
	}

GLDEF_C TInt E32Main()
	{
	TInt err = KErrNoMemory;
	CTrapCleanup* cleanup=CTrapCleanup::New();	//can fail
	if (cleanup)
		{
		err = KErrNone;
		TRAP(err, DeleteScheduleFilesL())
		delete cleanup;
		}
	return err;
	}

