/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/



#include "tsemaphore.h"
#include <unistd.h> 
#include <errno.h>
#include <stdio.h>
#include <e32std.h>
#include <stdlib.h>
#include <string.h>


CTestSemaphore::~CTestSemaphore() 
	{ 
	}  

CTestSemaphore::CTestSemaphore(const TDesC& aStepName) :
		iExitVal(0),
		iSemopStartFlag(EFalse),
		iReadSemIdFlag (EFalse), 
        iParamCnt(0)
	{
	// MANDATORY Call to base class method to set up the human readable name for logging.
	
	SetTestStepName(aStepName);		
	}

TVerdict CTestSemaphore::doTestStepPreambleL()
	{
	__UHEAP_MARK;	
	iNumberOfCreateSem = new int[MAX_IPC_OBJ];
    for (TInt i = 0; i < MAX_IPC_OBJ; i++)
		{
        iNumberOfCreateSem[i] = -1;
		}
	SetTestStepResult(EPass);
	
	return TestStepResult();
	}



TVerdict CTestSemaphore::doTestStepPostambleL()
	{
	delete []iNumberOfCreateSem;
	__UHEAP_MARKEND;	
	return TestStepResult();
	}


TVerdict CTestSemaphore::doTestStepL()
	{
	int err;

   	if(TestStepName() == KIntgTest1)
   		{
		INFO_PRINTF1(_L("IntgTest1():"));
		err = IntgTest1();
		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
		}
	else
		if(TestStepName() == KIntgTest2)
   			{
   	   		INFO_PRINTF1(_L("IntgTest2():"));
   	   		err = IntgTest2();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == KIntgTest3)
   			{
   	   		INFO_PRINTF1(_L("IntgTest3():"));
   	   		err = IntgTest3();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == KIntgTest4)
   			{
   	   		INFO_PRINTF1(_L("IntgTest4():"));
   	   		err = IntgTest4();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == KIntgTest5)
   			{
   	   		INFO_PRINTF1(_L("IntgTest5():"));
   	   		err = IntgTest5();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == KIntgTest6)
   			{
   	   		INFO_PRINTF1(_L("IntgTest6():"));
   	   		err = IntgTest6();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == KIntgTest7)
   			{
   	   		INFO_PRINTF1(_L("IntgTest7():"));
   	   		err = IntgTest7();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == KIntgTest8)
   			{
   	   		INFO_PRINTF1(_L("IntgTest8():"));
   	   		err = IntgTest8();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == KIntgTest9)
   			{
   	   		INFO_PRINTF1(_L("IntgTest9():"));
   	   		err = IntgTest9();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == KIntgTest10)
   			{
   	   		INFO_PRINTF1(_L("IntgTest10():"));
   	   		err = IntgTest10();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == KIntgTest11)
   			{
   	   		INFO_PRINTF1(_L("IntgTest11():"));
   	   		err = IntgTest11();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == KIntgTest12)
   			{
   	   		INFO_PRINTF1(_L("IntgTest12():"));
   	   		err = IntgTest12();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == KIntgTest13)
   			{
   	   		INFO_PRINTF1(_L("IntgTest13():"));
   	   		err = IntgTest13();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == KSemCreate )
   			{
   	   		INFO_PRINTF1(_L("SemCreate():"));
   	   		err = SemCreate();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	else
		if(TestStepName() == KIntgTest13 )
   			{
   	   		INFO_PRINTF1(_L("IntgTest13():"));
   	   		err = IntgTest13();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	else
		if(TestStepName() == KIntgTest14 )
   			{
   	   		INFO_PRINTF1(_L("IntgTest14():"));
   	   		err = IntgTest14();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	
   	else if(TestStepName() == KSemgetCreateKey )
   			{
   	   		INFO_PRINTF1(_L("SemgetCreateKey():"));
   	   		err = SemgetCreateKey();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	else if(TestStepName() == KSemgetCreatePrivate )
   			{
   	   		INFO_PRINTF1(_L("SemgetCreatePrivate():"));
   	   		err = SemgetCreatePrivate();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	else if(TestStepName() == KSemgetGetPrivate )
   			{
   	   		INFO_PRINTF1(_L("SemgetGetPrivate():"));
   	   		err = SemgetGetPrivate();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KSemgetCreateKeyMax )
   			{
   	   		INFO_PRINTF1(_L("SemgetCreateKeyMax():"));
   	   		err = SemgetCreateKeyMax();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	else if(TestStepName() == KSemgetCreatePvtMax )
   			{
   	   		INFO_PRINTF1(_L("SemgetCreatePvtMax():"));
   	   		err = SemgetCreatePvtMax();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}   
   	else if(TestStepName() == KSemgetGetNonExisting )
   			{
   	   		INFO_PRINTF1(_L("SemgetGetNonExisting():"));
   	   		err = SemgetGetNonExisting();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}   
   	else if(TestStepName() == KSemgetCreateInvalidSizeK )
   			{
   	   		INFO_PRINTF1(_L("SemgetCreateInvalidSizeK():"));
   	   		err = SemgetCreateInvalidSizeK();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}   
   	else if(TestStepName() == KSemgetCreateInvalidSizeP )
   			{
   	   		INFO_PRINTF1(_L("SemgetCreateInvalidSizeP():"));
   	   		err = SemgetCreateInvalidSizeP();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}   
   	else if(TestStepName() == KSemgetGetMore )
   			{
   	   		INFO_PRINTF1(_L("SemgetGetMore():"));
   	   		err = SemgetGetMore();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	else if(TestStepName() == KSemgetCreateExisting )
   			{
   	   		INFO_PRINTF1(_L("SemgetCreateExisting():"));
   	   		err = SemgetCreateExisting();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	else if(TestStepName() == KSemgetGetExisting )
   			{
   	   		INFO_PRINTF1(_L("SemgetGetExisting():"));
   	   		err = SemgetGetExisting();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}			
   	else if(TestStepName() == KSemctlInvalidId )
   			{
   	   		INFO_PRINTF1(_L("SemctlInvalidId():"));
   	   		err = SemctlInvalidId();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	else if(TestStepName() == KSemctlInvalidArgsK )
   			{
   	   		INFO_PRINTF1(_L("SemctlInvalidArgsK():"));
   	   		err = SemctlInvalidArgsK();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	else if(TestStepName() == KSemctlInvalidArgsP )
   			{
   	   		INFO_PRINTF1(_L("SemctlInvalidArgsP():"));
   	   		err = SemctlInvalidArgsP();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	else if(TestStepName() == KSemctlStatK )
   			{
   	   		INFO_PRINTF1(_L("SemctlStatK():"));
   	   		err = SemctlStatK();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	else if(TestStepName() == KSemctlStatP )
   			{
   	   		INFO_PRINTF1(_L("SemctlStatP():"));
   	   		err = SemctlStatP();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KSemctlSetK )
   			{
   	   		INFO_PRINTF1(_L("SemctlSetK():"));
   	   		err = SemctlSetK();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KSemctlSetP )
   			{
   	   		INFO_PRINTF1(_L("SemctlSetP():"));
   	   		err = SemctlSetP();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}			
   	else if(TestStepName() == KSemctlGetValK )
   			{
   	   		INFO_PRINTF1(_L("SemctlGetValK():"));
   	   		err = SemctlGetValK();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	else if(TestStepName() == KSemctlGetValP )
   			{
   	   		INFO_PRINTF1(_L("SemctlGetValP():"));
   	   		err = SemctlGetValP();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}			
   	else if(TestStepName() == Ksemctltest8 )
   			{
   	   		INFO_PRINTF1(_L("semctltest8():"));
   	   		err = semctltest8();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	else if(TestStepName() == Ksemctltest9 )
   			{
   	   		INFO_PRINTF1(_L("semctltest9():"));
   	   		err = semctltest9();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	else if(TestStepName() == Ksemctltest10 )
   			{
   	   		INFO_PRINTF1(_L("semctltest10():"));
   	   		err = semctltest10();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	else if(TestStepName() == Ksemctltest11 )
   			{
   	   		INFO_PRINTF1(_L("semctltest11():"));
   	   		err = semctltest11();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	else if(TestStepName() == Ksemctltest12 )
   			{
   	   		INFO_PRINTF1(_L("semctltest12():"));
   	   		err = semctltest12();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	else if(TestStepName() == Ksemctltest13 )
   			{
   	   		INFO_PRINTF1(_L("semctltest13():"));
   	   		err = semctltest13();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	else if(TestStepName() == Ksemctltest14 )
   			{
   	   		INFO_PRINTF1(_L("semctltest14():"));
   	   		err = semctltest14();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	else if(TestStepName() == Ksemoptest1 )
   			{
   	   		INFO_PRINTF1(_L("semoptest1():"));
   	   		err = semoptest1();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	else if(TestStepName() == Ksemoptest2 )
   			{
   	   		INFO_PRINTF1(_L("semoptest2():"));
   	   		err = semoptest2();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
    else if(TestStepName() == Ksemoptest3 )
   			{
   	   		INFO_PRINTF1(_L("semoptest3():"));
   	   		err = semoptest3();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	else if(TestStepName() == Ksemoptest4 )
   			{
   	   		INFO_PRINTF1(_L("semoptest4():"));
   	   		err = semoptest4();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
    else if(TestStepName() == Ksemoptest5 )
   			{
   	   		INFO_PRINTF1(_L("semoptest5():"));
   	   		err = semoptest5();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	else if(TestStepName() == Ksemoptest6 )
   			{
   	   		INFO_PRINTF1(_L("semoptest6():"));
   	   		err = semoptest6();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	else if(TestStepName() == Ksemoptest7 )
   			{
   	   		INFO_PRINTF1(_L("semoptest7():"));
   	   		err = semoptest7();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	else if(TestStepName() == Ksemoptest8 )
   			{
   	   		INFO_PRINTF1(_L("semoptest8():"));
   	   		err = semoptest8();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	else if(TestStepName() == Ksemoptest9 )
   			{
   	   		INFO_PRINTF1(_L("semoptest9():"));
   	   		err = semoptest9();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	else if(TestStepName() == Ksemoptest10 )
   			{
   	   		INFO_PRINTF1(_L("semoptest10():"));
   	   		err = semoptest10();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	else if(TestStepName() == Ksemoptest11 )
   			{
   	   		INFO_PRINTF1(_L("semoptest11():"));
   	   		err = semoptest11();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	else if(TestStepName() == Ksemoptest12 )
   			{
   	   		INFO_PRINTF1(_L("semoptest12():"));
   	   		err = semoptest12();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	else if(TestStepName() == Ksemoptest13 )
   			{
   	   		INFO_PRINTF1(_L("semoptest13():"));
   	   		err = semoptest13();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	else if(TestStepName() == Ksemoptest14 )
   			{
   	   		INFO_PRINTF1(_L("semoptest14():"));
   	   		err = semoptest14();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	else if(TestStepName() == Ksemoptest15 )
   			{
   	   		INFO_PRINTF1(_L("semoptest15():"));
   	   		err = semoptest15();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	return TestStepResult(); 

	}
 

