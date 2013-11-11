/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Launches a second process to test functions which take a process handle.
*
*/


/**
 @file
*/

#include "kmslddtest.h"


static RProcess CreateProcess(const TDesC& aExeName, const TDesC& aCmdLineArgs)
/**
	Create a process with the supplied command line, and return a handle to it.
	This function does not resume the process.

	@param	aExeName		Executable image which is used to generate process.
	@param	aCmdLineArgs	Arguments to supply to executable.
	@return					Open handle to newly-created process.
 */
	{
	test.Printf(_L("creating process \"%S\" \"%S\"\n"), &aExeName, &aCmdLineArgs);
	RProcess pr;
	TInt r = pr.Create(aExeName, aCmdLineArgs, EOwnerThread);
	test(r == KErrNone);
	return pr;
	}

static void WaitForProcessD(RProcess aOpenProc)
/**
	Run the supplied process and wait for it to finish.  When the process has
	finished, this function closes the supplied handle.

	@param	aOpenProc		Open but suspended thread.
 */
	{
	TRequestStatus rs;
	aOpenProc.Rendezvous(rs);
	test(rs == KRequestPending);
	aOpenProc.Resume();
	User::WaitForRequest(rs);
	
	test(aOpenProc.ExitType() == EExitKill);
	aOpenProc.Close();
	}

void TestSecondProcess(
	TUint32 aCapabilityMask, TSecureId aSecureId,
	TInt (*aTestFunc)(RProcess), TInt aExpectedResult)
/**
	Construct a process with the supplied capabilities and SID,
	and use it to perform a channel operation.  Compare the result
	from that operation with the expected value.
	
	The second process does not perform any useful actions itself.
	It only exists so the main process can take a handle to another
	process with the required platsec settings.

	This function leaks an instance of the executable in c:\\sys\\bin,
	which it cannot delete because the current test process does not
	have TCB capability.

	@param	aCapabilityMask	Capabilities which should be assigned to the
							newly-created process.
	@param	aSecureId		The newly-created process' secure ID.
	@param	aTestFunc		This function should be called with a handle
							to the newly-created process.
	@param	aExpectedResult	The KMS LDD function, called via aTestFunc, should
							complete its request with this reason.
 */
	{
	TBuf<128> cmdLineArgs;	
	cmdLineArgs.Format(_L("z:\\sys\\bin\\kmslddclient.exe %x"), aCapabilityMask);
	if (aSecureId != 0)
		cmdLineArgs.AppendFormat(_L(" -SID %x"), aSecureId.iId);	// SID must be in caps
	RProcess prSetCap = CreateProcess(_L("setcap.exe"), cmdLineArgs);
	WaitForProcessD(prSetCap);
	
	RProcess pr = CreateProcess(_L("c:\\sys\\bin\\kmslddclient.exe"), KNullDesC);
	TInt r = aTestFunc(pr);
	WaitForProcessD(pr);
	test(r == aExpectedResult);
	}

