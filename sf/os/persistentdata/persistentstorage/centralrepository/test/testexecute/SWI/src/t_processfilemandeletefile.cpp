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
// Helper executable with high capability to perform deletion of files for
// test harnesses. 
// 
//

#include <f32file.h>
#include <e32debug.h>
#include <bautils.h>


TInt DoDeleteFileL(const TDesC& aFile)
	{
	RFs fs;
	fs.Connect();
	CFileMan* fileMan = CFileMan::NewL(fs);
	CleanupStack::PushL(fileMan);

    // Make the file writeable 
    TInt err = fileMan->Attribs(aFile, 0, KEntryAttReadOnly, TTime(0), 0);
    // Delete file using CFileMan
    err = fileMan->Delete(aFile);
    RDebug::Print(_L("CFileMan Delete file %S - err = %d\n"), &aFile, err);
    
	CleanupStack::PopAndDestroy(fileMan); 
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
	name.TrimAll();
	TInt err = DeleteFile(name);	
	delete cleanup;
	return err;
	}
