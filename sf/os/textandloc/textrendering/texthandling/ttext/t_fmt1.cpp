/*
* Copyright (c) 1997-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* T_FMT.CPP
*
*/


#include <txtfmlyr.h>
#include <txtrich.h>
#include <txtfrmat.h>
#include <gdi.h>
#include "../stext/TXTSTD.H"
#include "T_FMT1.h"

#define test(cond)											\
	{														\
	TBool __bb = (cond);									\
	pTestStep->TEST(__bb);									\
	if (!__bb)												\
		{													\
		pTestStep->ERR_PRINTF1(_L("ERROR: Test Failed"));	\
		User::Leave(1);										\
		}													\
	}

void CT_FMT1::TestDEF047316L()
// Test CParaFormat stack definition leave protection
//
	{
    INFO_PRINTF1(_L(" @SYMTestCaseID:SYSLIB-ETEXT-LEGACY-t_fmt1-0001 CParaFormat - DEF047316 - stack definition leave protection "));
	CParaFormat paraFormat;
	
	// Allocate resources for paraFormat
	TTabStop tabStop;
	paraFormat.StoreTabL(tabStop);
	
	TParaBorder border;
	paraFormat.SetParaBorderL(CParaFormat::EParaBorderTop,border);

	paraFormat.iBullet=new(ELeave)TBullet;
	
	// Push cleanup method for paraFormat
	ResetOnCleanupL( &paraFormat);
	
	// Force cleanup method to be called
	CleanupStack::PopAndDestroy();

	// Note that test end is when paraFormat's destructor is called when 
	// this function terminates. The destructor should not cause any problems
	// even though we have already called the cleanup method
	}

CT_FMT1::CT_FMT1()
    {
    SetTestStepName(KTestStep_T_FMT1);
    }

TVerdict CT_FMT1::doTestStepL()
    {
    SetTestStepResult(EFail);

    CTrapCleanup* cleanup=CTrapCleanup::New();
    INFO_PRINTF1(_L("TFormat Test Code for DEF047316"));
    
    __UHEAP_MARK;
    TRAPD(error1, TestDEF047316L());

    __UHEAP_MARKEND;
    delete cleanup;

    if(error1 == KErrNone)
        {
        SetTestStepResult(EPass);
        }

    return TestStepResult();
    }
