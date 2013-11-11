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
// Helper process with high capability to perform file operation for
// test harnesses. 
// 
//

#include <f32file.h>
#include <e32debug.h>


void DoMakeFileReadOnlyL(const TDesC& aFile)
	{
	// Setting read only flag for a file
	RDebug::Print(_L("Making file read only %S\n"), &aFile);            

	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	CFileMan* fileMan = CFileMan::NewL(fs);
	TInt err = fileMan->Attribs(aFile,KEntryAttReadOnly,KEntryAttNormal,TTime(0),0);
	delete fileMan;
	CleanupStack::PopAndDestroy(); // fs.Close()
	User::LeaveIfError(err);
	}
   
static TInt MakeFileReadOnly(const TDesC& aFile)
	{   
	TRAPD(err, DoMakeFileReadOnlyL(aFile));
	return err;
	}

GLDEF_C TInt E32Main()
	{
	CTrapCleanup* cleanup =   CTrapCleanup::New(); 

	TFileName fileName;
	User::CommandLine(fileName);
	TInt err = MakeFileReadOnly(fileName);   
	delete cleanup;
	return err;
	}
