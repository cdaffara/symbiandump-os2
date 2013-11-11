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

#include <e32test.h>
#include <e32std.h>
#include "t_testhandler.h"
#include "tscriptandhardcoded.h"
#include "t_certapps_defs.h"

#include "t_certapps_actions.h"


LOCAL_D void DoTests()
	{	
	START_SCRIPT_LIST
	SCRIPT_ITEM(CTestWrapper<CInitManager>, KInitManager),
	SCRIPT_ITEM(CTestWrapper<CDestroyManager>, KDestroyManager),
	SCRIPT_ITEM(CTestWrapper<CClearAllApps>, KClearAllApps),
	SCRIPT_ITEM(CTestWrapper<CAddApp>, KAddApp),
	SCRIPT_ITEM(CTestWrapper<CRemoveApp>, KRemoveApp),
	SCRIPT_ITEM(CTestWrapper<CAppCount>, KAppCount),
	SCRIPT_ITEM(CTestWrapper<CGetApplications>, KGetApplications),
	SCRIPT_ITEM(CTestWrapper<CGetApp>, KGetApp)
	END_SCRIPT_LIST

	
	TDriveUnit sysDrive (RFs::GetSystemDrive());
	TBuf<64> scriptFile (sysDrive.Name());
	scriptFile.Append(_L("\\tcertapps\\scripts\\script1.txt"));
	
	_LIT(KLog, "tcertapps.log");
	CScriptSetup::CreateAndRunTestsL(theTestTypes, scriptFile, KLog);	
	}

GLDEF_C TInt E32Main() // main function called by E32
    {
	RProcess().DataCaging(RProcess::EDataCagingOn);
	__UHEAP_MARK;
	CTrapCleanup* cleanup=CTrapCleanup::New(); // get clean-up stack
	TRAP_IGNORE(DoTests());
	delete cleanup; // destroy clean-up stack
	__UHEAP_MARKEND;
	return 0; // and return
    }
