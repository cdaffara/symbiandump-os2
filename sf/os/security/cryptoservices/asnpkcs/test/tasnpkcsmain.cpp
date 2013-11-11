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
* tasnpkcs.cpp
*
*/


#include <e32base.h>
#include "t_testhandler.h"
#include "t_testsetup.h"
#include "tScriptTests.h"
#include "tactiondecodepkcs5.h"
#include "tactiontestpkcs8.h"

LOCAL_D void callExampleL() // initialize and call example code under cleanup stack
    {
    START_SCRIPT_LIST
	SCRIPT_ITEM(CActionDecodePKCS5,_L8("pkcs5")),
	SCRIPT_ITEM(CActionTestPKCS8,_L8("testpkcs8"))
	END_SCRIPT_LIST

	_LIT(KScript, "c\\tasnpkcs\\scripts\\tasnpkcstests.txt");
	_LIT(KLog, "tasnpkcs.log");
	CTestSetup::CreateAndRunTestsL(theTestTypes, KScript, KLog);
    }

GLDEF_C TInt E32Main() // main function called by E32
    {
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New(); // get clean-up stack
	TRAPD(error, callExampleL());
	__ASSERT_ALWAYS(!error,User::Panic(_L("tasnpkcs"),error));
	delete cleanup; // destroy clean-up stack
	__UHEAP_MARKEND;
	return 0;
    }
