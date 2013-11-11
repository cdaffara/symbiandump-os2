/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
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




/**
 @file
*/

//This is a dummy source file to be used to create a dummy exe
#include <e32base.h>
#include <f32file.h>

#include "captestframework.h"


void MainL();

GLDEF_D TInt E32Main()
	{
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(cleanup == NULL)
		{
		return KErrNoMemory;
		}
	
	TRAPD(err,MainL());
	delete cleanup;
	return err;
	}

HBufC* GetDesParameterL(TInt aParam)
	{
	// Get Dll name	
	TInt length=User::ParameterLength(aParam);
	User::LeaveIfError(length);
	
	HBufC* des=HBufC::NewLC(length);
	TPtr desPtr=des->Des();
	User::LeaveIfError(User::GetDesParameter(aParam, desPtr));

	CleanupStack::Pop(des);
	return des;
	}


HBufC* GetDllNameL()
	{
	return GetDesParameterL(KDllNameTransferSlot);
	}

void MainL()
	{
	TInt err=KErrNone;
	
	// figure out which test we're supposed to be running
	HBufC* dllName=GetDllNameL();
	
	HBufC* logFileName=GetDesParameterL(KLogFileNameTransferSlot);
	
	TBool shouldPass;
	err = User::GetTIntParameter(KShouldPassTransferSlot, shouldPass);
	TInt testNumber;
	err = User::GetTIntParameter(KTestNumberTransferSlot, testNumber);

	// session
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
		
	//logfile
	RFile logFile;
	User::LeaveIfError(logFile.Replace(fs, *logFileName, 0));
	CleanupClosePushL(logFile);
	
	// load library
	RLibrary lib;
	User::LeaveIfError(lib.Load(*dllName));
	CleanupClosePushL(lib);
	
	//runtests
	TLibraryFunction testFactory=lib.Lookup(1);
	MCapabilityTestFactory* factory=reinterpret_cast<MCapabilityTestFactory*>(testFactory());
	factory->Test(testNumber)->SetExpectPermissionDenied(!shouldPass);
	
	TRAP(err, factory->Test(testNumber)->RunTestL(logFile));

	delete factory;
	CleanupStack::PopAndDestroy(3, &fs); // lib, logFile, fs
	
	User::LeaveIfError(err);
	}
