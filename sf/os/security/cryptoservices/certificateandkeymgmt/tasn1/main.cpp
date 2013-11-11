/*
* Copyright (c) 1998-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* TPKIXCERTMAIN.CPP
*
*/


#include <e32base.h>
#include "t_testhandler.h"
#include "tasn1normaltest.h"
#include "tScriptTests.h"
#include "tScriptSetup.h"
#include <random.h>

LOCAL_D void callExampleL() // initialize and call example code under cleanup stack
    {
    CSystemRandom *rng = CSystemRandom::NewL();
    SetThreadRandomLC(rng);

	START_SCRIPT_LIST
	SCRIPT_ITEM(CASN1NormalTest, _L8("Normal"))
	END_SCRIPT_LIST
	
	TDriveUnit sysDrive (RFs::GetSystemDrive());
	TDriveName driveName(sysDrive.Name());
	TBuf<64> scriptFile (driveName);
	scriptFile.Append(_L("\\tasn1\\scripts\\tasn1.txt"));
	CScriptSetup::CreateAndRunTestsL(theTestTypes, scriptFile, _L("tasn1.log"));
	
	DestroyThreadRandom();
	CleanupStack::Pop(); // Pop the cleanup item which would have reset/delete the threadrandom
    }

GLDEF_C TInt E32Main() // main function called by E32
    {
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New(); // get clean-up stack
	TRAP_IGNORE(callExampleL());
	// dont panic in case its a syntax error in script.
	//__ASSERT_ALWAYS(!error,User::Panic(_L("CERT"),error));
	delete cleanup; // destroy clean-up stack
	__UHEAP_MARKEND;
	return 0;
    }
