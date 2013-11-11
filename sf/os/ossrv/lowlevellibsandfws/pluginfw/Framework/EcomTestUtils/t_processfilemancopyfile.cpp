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
// Helper process with high capability to perform copying of files for
// test harnesses. 
// 
//

#include <f32file.h>
#include <e32debug.h>


_LIT(KSeparator, "|"); // Invalid filepath char used to separate filenames


// Copy the files specified.
void CopyFileL(const TDesC& anOld, const TDesC& aNew)
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
	
	// Ensure the path exists
	TInt err = fs.MkDirAll(aNew);	
	if ((err != KErrNone) && (err != KErrAlreadyExists))
		User::Leave(err);
	
	// Make the destination file writeable 
    err = fileMan->Attribs(aNew, 0, KEntryAttReadOnly, TTime(0), 0);
	if ((err != KErrNone) && (err != KErrNotFound))
		User::Leave(err);

	// Do the file copy	
    User::LeaveIfError(fileMan->Copy(anOld, aNew));
    
	CleanupStack::PopAndDestroy(2); 
	}


// Format of aFileNames is [srcFile]|[dstFile]
GLDEF_C TInt E32Main()
	{
	CTrapCleanup* cleanup =	CTrapCleanup::New(); 
	
	TBuf<KMaxFileName*2> names;
	User::CommandLine(names);
	TInt pos = names.Find(KSeparator);
	TFileName srcFile(names.Mid(0,pos));
	TFileName dstFile(names.Mid(pos+1, names.Length()-(pos+1)));
			
	TInt err = KErrNone;	
	TRAP(err,CopyFileL(srcFile, dstFile));
	if (err == KErrNone)
   		RDebug::Print(_L("CFileMan Copy file '%S' to '%S' succeeded.\n"), &srcFile, &dstFile);
	else
    	RDebug::Print(_L("CFileMan Copy file '%S' to '%S' failed with error = %d.\n"), &srcFile, &dstFile, err);
	
	delete cleanup;
	return err;
	}
