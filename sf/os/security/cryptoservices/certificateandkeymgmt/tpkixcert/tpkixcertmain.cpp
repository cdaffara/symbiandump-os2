/*
* Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <e32base.h>
#include "t_testhandler.h"
#include <f32file.h>
#include "tactionvalidate.h"
#include "taction_build.h"
#include "tscriptandhardcoded.h"

LOCAL_D void callExampleL() // initialize and call example code under cleanup stack
    {
	START_SCRIPT_LIST
	SCRIPT_ITEM(CTestActionBuild,_L8("Build")),
	SCRIPT_ITEM(CActionValidateWithStore,_L8("ValidateWithStore")),
	SCRIPT_ITEM(CActionValidateWithSuppliedCerts,_L8("ValidateWithSuppliedCerts"))
	END_SCRIPT_LIST
	
	TDriveUnit sysDrive (RFs::GetSystemDrive());
	TDriveName sysDriveName (sysDrive.Name());
	TBuf<128> scriptFile (sysDriveName);
	scriptFile.Append(_L("\\tpkixcert\\scripts\\tpkixtest_part1.txt"));
	
	TBuf<64> logFile (sysDriveName);
	logFile.Append(_L("\\TPKIXCERT_log1.txt"));
	
	CTestSetup::CreateAndRunTestsL(theTestTypes, scriptFile, logFile);
    }

GLDEF_C TInt E32Main() // main function called by E32
    {
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New(); // get clean-up stack
	TRAPD(error, callExampleL());
	__ASSERT_ALWAYS(!error,User::Panic(_L("CERT"),error));
	delete cleanup; // destroy clean-up stack
	__UHEAP_MARKEND;
	return 0;
    }
