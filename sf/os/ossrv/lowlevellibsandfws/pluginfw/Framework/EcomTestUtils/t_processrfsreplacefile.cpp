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
// Helper process with high capability to perform copying of using Rfs
// files for test harness. 
// 
//

#include <f32file.h>
#include <e32debug.h>


_LIT(KSeparator, "|"); // Invalid filepath char used to separate filenames

TInt DoReplaceFileL(const TDesC& anOld,const TDesC& aNew)
	{
	TInt r = KErrNone;	
	RFs fs;
	r = fs.Connect();
	if (r != KErrNone)
		{
			User::Leave(r);
		}

	// Ensure the path exists
	TInt err = fs.MkDirAll(aNew);
    // Make the new file writeable 
    err = fs.SetAtt(aNew, 0, KEntryAttReadOnly);
	// Replace
    err = fs.Replace(anOld, aNew);
    RDebug::Print(_L("RFs Replace file %S to %S - err = %d\n"), &anOld, &aNew, err);
    
    fs.Close();
	return err;
	}

	
// Copy the files specified.  Format of aFileNames is [srcFile]|[dstFile].
static TInt ReplaceFile(const TDesC& aFileNames)
	{	
	TInt pos = aFileNames.Find(KSeparator);
	
	TFileName srcFile(aFileNames.Mid(0,pos));
	TFileName dstFile(aFileNames.Mid(pos+1, aFileNames.Length()-(pos+1)));
			
	TRAPD(err,DoReplaceFileL(srcFile, dstFile));
	return err;
	}


GLDEF_C TInt E32Main()
	{
	CTrapCleanup* cleanup =	CTrapCleanup::New(); 
	
	TBuf<KMaxFileName*2> names;
	User::CommandLine(names);
	TInt err = ReplaceFile(names);
	delete cleanup;
	return err;
	}
