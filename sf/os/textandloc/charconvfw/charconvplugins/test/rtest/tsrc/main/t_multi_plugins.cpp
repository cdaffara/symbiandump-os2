/*
* Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
* Test to verify that a single charconv plugin with mulitple implementations
* can be built.
*
*/


#include <e32std.h>
#include <e32base.h>
#include <charconv.h>
#include <charactersetconverter.h>
#include "tis1620.h"
#include "t_multi_plugins.h"

#define test(cond)                                  \
    {                                               \
    TBool __bb = (cond);                            \
    TEST(__bb);                                     \
    if (!__bb)                                      \
        {                                           \
        ERR_PRINTF1(_L("ERROR: Test Failed"));      \
        User::Leave(1);                             \
        }                                           \
    }


/**
@SYMTestCaseID          SYSLIB-CharConv-UT-1882
@SYMTestCaseDesc        Verify a charconv plugin with multiple implementations can be built.
@SYMTestPriority        Normal
@SYMTestActions         Load the two implementations in tis1620.dll which is
a charconv plugin that has two implementations.
@SYMTestExpectedResults Both implementations loaded successfully.
@SYMDEF                 DEF088036
*/
void CT_MULTI_PLUGINS::DoE32MainL()
	{
    INFO_PRINTF1(_L(" @SYMTestCaseID:SYSLIB-CHARCONV-UT-1882 test DEF088036 started.\r\n "));
	CActiveScheduler* scheduler = new(ELeave) CActiveScheduler;
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler);

	// load 2nd plug in

    TUid ImpId2={KTIS1620bImplUid};

    CCharacterSetConverterPluginInterface* ex =
    	CCharacterSetConverterPluginInterface::NewL(ImpId2);
	test(ex != NULL);
	delete ex;
	ex = NULL;

	// load 1st plugin
    TUid ImpId1={KTIS1620aImplUid};
    ex = CCharacterSetConverterPluginInterface::NewL(ImpId1);
	test(ex != NULL);
	delete ex;

	REComSession::FinalClose();

	CleanupStack::PopAndDestroy(scheduler);
	}

CT_MULTI_PLUGINS::CT_MULTI_PLUGINS()
    {
    SetTestStepName(KTestStep_T_MULTI_PLUGINS);
    }

TVerdict CT_MULTI_PLUGINS::doTestStepL()
    {
    SetTestStepResult(EFail);

    __UHEAP_MARK;
    TRAPD(error1, DoE32MainL());    
    __UHEAP_MARKEND;

    if(error1 == KErrNone)
        {
        SetTestStepResult(EPass);
        }

    return TestStepResult();
    }
