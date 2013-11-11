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
*
*/


#include <e32base.h>
#include "t_testhandler.h"
#include "t_testsetup.h"
#include "tScriptTests.h"
#include "tactionelement.h"
#include "tactionset.h"
#include "tactionsetreadpfs.h"
#include "tactionsetwritepfs.h"
#include "texternpbeparams.h"

LOCAL_D void callExampleL() // initialize and call example code under cleanup stack
    {
    START_SCRIPT_LIST
	SCRIPT_ITEM(CActionElement,_L8("Element")),
	SCRIPT_ITEM(CActionSet,_L8("Set")),
	SCRIPT_ITEM(CActionSetReadPFS,_L8("ReadPFS")),
	SCRIPT_ITEM(CActionSetWritePFS,_L8("WritePFS")),
	SCRIPT_ITEM(CExternPbeParams, _L8("ExternPbeParams"))
    END_SCRIPT_LIST
    
    TDriveUnit sysDrive (RFs::GetSystemDrive());
    
    TBuf<24> scriptFile (sysDrive.Name());
    scriptFile.Append(_L("\\tpbe\\tpbetests_v2.txt"));
    
    TBuf<24> logFile (sysDrive.Name());
    logFile.Append(_L("\\tpbetestsv2.log"));
    
	CTestSetup::CreateAndRunTestsL(theTestTypes, scriptFile, logFile);
    }

GLDEF_C TInt E32Main() // main function called by E32
    {
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New(); // get clean-up stack
	TRAPD(error, callExampleL());
	__ASSERT_ALWAYS(!error,User::Panic(_L("tpbe"),error));
	delete cleanup; // destroy clean-up stack
	__UHEAP_MARKEND;
	return 0;
    }
