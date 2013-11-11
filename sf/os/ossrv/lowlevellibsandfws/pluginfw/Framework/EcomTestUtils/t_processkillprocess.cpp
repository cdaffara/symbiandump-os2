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
//

#include <f32file.h>
#include <e32debug.h>
	
//This function is used in the test code to kill ECOMSERVER
//processes (or some other) when they leftover and may problem in ECOMSERVERTEST
static TInt KillProcess(const TDesC& aProcessName)
	{
	TFullName name;

	RDebug::Print(_L("Find and kill \"%S\" process.\n"), &aProcessName);

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
		if(proc.Open(name) == KErrNone)
			{
			proc.Kill(0);
			RDebug::Print(_L("\"%S\" process killed.\n"), &name);
			}
		proc.Close();
		}
	return KErrNone;
	}

GLDEF_C TInt E32Main()
	{
	TFileName name;
	User::CommandLine(name);
	return KillProcess(name);
	}

