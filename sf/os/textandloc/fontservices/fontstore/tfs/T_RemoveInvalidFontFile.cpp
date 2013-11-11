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
Simple application to remove the invalid font file created by T_WriteInvalidFontFile.exe.
These applications are seperate from the test code and each other so they can have the 
capability Tcb in order to write to the \resource\* directory.
*/
LOCAL_C void MainL()
	{
	RFs fs;
	CleanupClosePushL(fs);
	User::LeaveIfError(fs.Connect());

	User::LeaveIfError(fs.Delete(KEclipsingFile));
	
	CleanupStack::PopAndDestroy(&fs);
	}

GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	TRAPD(mainError, MainL());
	if (mainError != KErrNone)
		{
		RDebug::Printf("T_RemoveInvalidFontFile.exe failed with error %i", mainError);
		}
	delete cleanup;
	__UHEAP_MARKEND;
	return KErrNone;
	}

