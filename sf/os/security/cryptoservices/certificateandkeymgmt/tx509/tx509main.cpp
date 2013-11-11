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
* TX509 entry point code
*
*/


#include <e32std.h>
#include "tScriptSetup.h"
#include "ValidateTest.h"
#include "SyntaxTest.h"
#include "CorruptionTest.h"
#include "extensiontest.h"
#include "comparisontest.h"

LOCAL_C void callSetupAndRunL() // initialize and call example code under cleanup stack
    {
	START_SCRIPT_LIST
	SCRIPT_ITEM(CSyntaxTest, _L8("Syntax")),
	SCRIPT_ITEM(CValidateTest, _L8("Validate")),
	SCRIPT_ITEM(CCorruptionTest, _L8("Corruption")),	
	SCRIPT_ITEM(CExtensionTest, _L8("Extension")),
	SCRIPT_ITEM(CComparisonTest, _L8("Comparison"))
	END_SCRIPT_LIST
	
	TDriveUnit sysDrive (RFs::GetSystemDrive());
	TDriveName driveName(sysDrive.Name());
	TBuf<64> scriptFile(driveName);
	scriptFile.Append(_L("\\tx509\\scripts\\tx509.txt"));
	
	CTestSetup::CreateAndRunTestsL(theTestTypes, scriptFile, _L("tx509.log"));
    }

GLDEF_C TInt E32Main() // main function called by E32
    {
	__UHEAP_MARK;
	CTrapCleanup* cleanup=CTrapCleanup::New(); // get clean-up stack
	TRAP_IGNORE(callSetupAndRunL());
	delete cleanup; // destroy clean-up stack
	__UHEAP_MARKEND;
	return 0; // and return
    }

