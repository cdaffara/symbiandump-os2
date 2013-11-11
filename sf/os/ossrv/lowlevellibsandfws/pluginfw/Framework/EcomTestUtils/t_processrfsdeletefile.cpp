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
// Hepler process with high capability to perform deletion of files 
// for using RFs for test harness. 
// 
//

#include <f32file.h>
#include <e32debug.h>
#include <bautils.h>
	
TInt DoDeleteFileL(const TDesC& aFile)
	{
	TInt r = KErrNone;
	RFs fs;
	r = fs.Connect();
	if (r != KErrNone)
		{
			User::Leave(r);
		}
	
    // Make the file writeable 
    fs.SetAtt(aFile, 0, KEntryAttReadOnly);
	// Delete file using RFs
	TInt err = fs.Delete(aFile);
	RDebug::Print(_L("RFs Delete file %S - err = %d\n"), &aFile, err);
	
    fs.Close();
	return err;
	}

static TInt DeleteFile(const TDesC& aFile)
	{
	TRAPD(err,DoDeleteFileL(aFile));
	return err;
	}

GLDEF_C TInt E32Main()
	{
	CTrapCleanup* cleanup =	CTrapCleanup::New(); 
	
	TFileName name;
	User::CommandLine(name);
	TInt err = DeleteFile(name);	
	delete cleanup;
	return err;
	}
