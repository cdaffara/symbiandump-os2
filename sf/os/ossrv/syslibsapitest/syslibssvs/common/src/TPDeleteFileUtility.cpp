/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#include <f32file.h>
#include <e32debug.h>


TInt DoDeleteFileL(const TDesC& aFileName)
	{
	RFs			fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs); 
	CFileMan*	fileMan = CFileMan::NewL(fs);
	CleanupStack::PushL(fileMan); 

	// Make the destination file writeable 
    TInt	err = fileMan->Attribs(aFileName, 0, KEntryAttReadOnly, TTime(0), 0);

	// To process 
    err = fileMan->Delete(aFileName);
    RDebug::Print(_L("CFileMan Delete file %S - err = %d\n"), &aFileName, err);

	CleanupStack::PopAndDestroy(2, &fs);

	return err;
	}

// Delete the file specified.
static TInt DeleteFile(const TDesC& aFileName)
	{	
	TFileName	fileName(aFileName);

	fileName.TrimLeft();
	fileName.TrimRight();

	TRAPD(err,DoDeleteFileL(fileName));
	return err;
	}


GLDEF_C TInt E32Main()
	{
	CTrapCleanup*			cleanup =	CTrapCleanup::New(); 
	
	TBuf<KMaxFileName>	name;
	User::CommandLine(name);
	TInt					err = DeleteFile(name);
	delete cleanup;
	return err;
	}
