/*
* Copyright (c) 1995-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/


#include "T_InvalidFontFile.h"
#include <e32std.h>
#include <f32file.h>
#include <s32file.h>


/**
Simple application to create an invalid font file for use in the Font Store eclipsing tests
This app is seperate from the test code so that it may use the Tcb capability in order to
write to the \resource\* directory.
*/
LOCAL_C void MainL()
	{
	RFs fs;
	CleanupClosePushL(fs);
	User::LeaveIfError(fs.Connect());

	RFileWriteStream fw;
	CleanupClosePushL(fw);
	
	//Ensure the directory exists or the file writing will fail
	TRAPD(err, fs.MkDirAll(KEclipsingFolder));
	if (err != KErrAlreadyExists)
		User::LeaveIfError(err);
	
	//Create the invalid file
	User::LeaveIfError(fw.Create(fs, KEclipsingFile, EFileWrite));
	fw.WriteL(KInvalidFontFileText);
	fw.CommitL();

	CleanupStack::PopAndDestroy(2, &fs);
	}

GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	TRAPD(mainError, MainL());
	if (mainError != KErrNone)
		{
		RDebug::Printf("T_WriteInvalidFontFile.exe failed with error %i", mainError);
		}
	delete cleanup;
	__UHEAP_MARKEND;
	return KErrNone;
	}

