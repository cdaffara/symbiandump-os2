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
// t_processdeletefile.cpp
// Helper process with high capability to perform file operation for
// test harnesses. 
// 
//

#include <f32file.h>
#include <e32debug.h>


TInt DoMakeFileWriteableL(const TDesC& aFile)
	{
	// Make sure the file is not read-only
	RDebug::Print(_L("Open for writing file %S\n"), &aFile);				
	TInt r = KErrNone;
	RFs fs;
	r = fs.Connect();
	if (r != KErrNone)
		{
			User::Leave(r);
		}
	CFileMan* fileMan = CFileMan::NewL(fs);
	CleanupStack::PushL(fileMan);
	TInt err = fileMan->Attribs(aFile, 0, KEntryAttReadOnly, TTime(0), 0);
	CleanupStack::PopAndDestroy(fileMan);
	fs.Close();
	return err;
	}
	
static TInt MakeFileWriteable(const TDesC& aFile)
	{	
	TRAPD(err,DoMakeFileWriteableL(aFile));
	return err;
	}

GLDEF_C TInt E32Main()
	{
	CTrapCleanup* cleanup =	CTrapCleanup::New(); 
	
	TFileName fileName;
	User::CommandLine(fileName);
	TInt err = MakeFileWriteable(fileName);	
	delete cleanup;
	return err;
	}
