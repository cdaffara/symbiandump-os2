// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

#include <f32file.h>
#include <e32debug.h>

//This function is used in the test code to kill SCHSVR or MinimalTaskHandler
//processes (or some other) when they leftover and may cause OOM condinitons.
static void KillProcessL(const TDesC& aProcessName)
	{
	TFullName name;

	TInt err = KErrNotFound;
	
	TBuf<64> pattern(aProcessName);
	TInt length = pattern.Length();
	pattern += _L("*");
	TFindProcess procFinder(pattern);

	while(procFinder.Next(name) == KErrNone)
		{
		if(name.Length() > length) 
			{//If found name is a string containing aProcessName string.
			TChar c(name[length]);
			if(c.IsAlphaDigit() || c == TChar('_') || c == TChar('-'))
				{//If the found name is other valid application name starting with aProcessName string.
				RDebug::Print(_L(":: Process name: \"%S\".\n"), &name);
				continue;
				}
			}
		RProcess proc;
		
		err = proc.Open(name);
		if(err == KErrNone)
			{
			
			//If the process is alive, terminate it
			if(proc.ExitType() == EExitPending)
			{
			proc.Kill(0);
			RDebug::Print(_L("\"%S\" process killed.\n"), &name);	
			}
			
			//If the process has already died record the error
			else err = KErrDied;

			}
		proc.Close();
		}
	User::Leave(err);
	}

void DoTestsL()
	{
	TFileName name;
	User::CommandLine(name);
	KillProcessL(name);
	}

GLDEF_C TInt E32Main()
	{
	CTrapCleanup* cleanup=CTrapCleanup::New();
	if(cleanup == NULL)
	{
		return KErrNoMemory;
	}
	
	TRAPD(err,DoTestsL());

	delete cleanup;
	return err;
	}

