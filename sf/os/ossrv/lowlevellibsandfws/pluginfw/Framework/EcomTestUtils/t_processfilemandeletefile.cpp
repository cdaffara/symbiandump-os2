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
// Hepler process with high capability to perform deletion of files for
// test harnesses. 
// 
//

#include <f32file.h>
#include <e32debug.h>
#include <bautils.h>


void DeleteFileL(const TDesC& aFile)
	{
	TInt r = KErrNone;	
	RFs fs;
	r = fs.Connect();
	if (r != KErrNone)
		{
			User::Leave(r);
		}
	CleanupClosePushL(fs);
	
	CFileMan* fileMan = CFileMan::NewL(fs);
	CleanupStack::PushL(fileMan);

    // Make the file writeable and delete it.
    TInt err = fileMan->Attribs(aFile, 0, KEntryAttReadOnly, TTime(0), 0);
	if ((err != KErrNone) && (err != KErrNotFound) && (err != KErrPathNotFound))
		User::Leave(err);
  
    err = fileMan->Delete(aFile);
    if ((err != KErrNone) && (err != KErrPathNotFound))
        User::Leave(err);
        
	CleanupStack::PopAndDestroy(2); 
	}
	
GLDEF_C TInt E32Main()
	{
	CTrapCleanup* cleanup =	CTrapCleanup::New(); 
	
	TFileName name;
	User::CommandLine(name);

	TInt err = KErrNone;	
	TRAP(err,DeleteFileL(name));
	if (err == KErrNone)
   		RDebug::Print(_L("CFileMan Delete file '%S' succeeded.\n"), &name);
	else
    	RDebug::Print(_L("CFileMan Delete file '%S' failed with error = %d.\n"), &name, err);
	
	delete cleanup;
	return err;
	}
