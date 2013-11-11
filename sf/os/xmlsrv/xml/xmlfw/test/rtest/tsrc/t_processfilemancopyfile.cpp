// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

TInt DoCopyFileL(const TDesC& anOld,const TDesC& aNew)
	{
	RFs fs;
	fs.Connect();
	CFileMan* fileMan = CFileMan::NewL(fs);
	CleanupStack::PushL(fileMan); 
	// Ensure the path exists
	TInt err = fs.MkDirAll(aNew);	
	// Make the destination file writeable 
    err = fileMan->Attribs(aNew, 0, KEntryAttReadOnly, TTime(0), 0);
	// To process 
    err = fileMan->Copy(anOld, aNew);
    RDebug::Print(_L("CFileMan Copy file %S to %S - err = %d\n"), &anOld, &aNew, err);
    
	CleanupStack::PopAndDestroy(fileMan); 
    fs.Close();

	return err;
	}
	
// Copy the files specified.  Format of aFileNames is [srcFile]|[dstFile].
static TInt CopyFile(const TDesC& aFileNames)
	{	
	TInt pos = aFileNames.Find(KSeparator);
	TFileName srcFile(aFileNames.Mid(0,pos));
	TFileName dstFile(aFileNames.Mid(pos+1, aFileNames.Length()-(pos+1)));
			
	TRAPD(err,DoCopyFileL(srcFile, dstFile));
	return err;
	}


GLDEF_C TInt E32Main()
	{
	CTrapCleanup* cleanup =	CTrapCleanup::New(); 
	
	TBuf<KMaxFileName*2> names;
	User::CommandLine(names);
	TInt err = CopyFile(names);
	delete cleanup;
	return err;
	}
