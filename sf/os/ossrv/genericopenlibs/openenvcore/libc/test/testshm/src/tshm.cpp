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



#include "tshm.h"

CTestShm::~CTestShm() 
	{ 
	}  

CTestShm::CTestShm(const TDesC& aStepName)
	{
	// MANDATORY Call to base class method to set up the human readable name for logging.
	SetTestStepName(aStepName);		
	}

TVerdict CTestShm::doTestStepPreambleL()
	{
	
	iParamCnt=0;
	iNoOfShmIds = new int[MAX_IPC_OBJ];                    
    for (TInt i = 0; i < MAX_IPC_OBJ; i++)
		{
        iNoOfShmIds[i] = -1;
		}
	SetTestStepResult(EPass);
	return TestStepResult();
	}



TVerdict CTestShm::doTestStepPostambleL()
	{
	delete []iNoOfShmIds;
	
	return TestStepResult();
	}


TVerdict CTestShm::doTestStepL()
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
		if(TestStepName() == KShmCreate)
   			{
   	   		INFO_PRINTF1(_L("ShmCreate():"));
   	   		err = ShmCreate();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == KShmAttach)
   			{
   	   		INFO_PRINTF1(_L("ShmAttach():"));
   	   		err = ShmAttach();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == KShmDetach)
   			{
   	   		INFO_PRINTF1(_L("ShmDetach():"));
   	   		err = ShmDetach();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == KShmControl)
   			{
   	   		INFO_PRINTF1(_L("ShmControl():"));
   	   		err = ShmControl();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == KCreateThreadL)
   			{
   	   		INFO_PRINTF1(_L("CreateThreadL():"));
   	   		err = CreateThreadL();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == KCreateThreadL1)
   			{
   	   		INFO_PRINTF1(_L("KCreateThreadL1():"));
   	   		err = CreateThreadL1();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	else if(TestStepName() == Kshmgettest1)
   			{
   	   		INFO_PRINTF1(_L("shmgettest1():"));
   	   		err = shmgettest1();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	else if(TestStepName() == Kshmgettest2)
   			{
   	   		INFO_PRINTF1(_L("shmgettest2():"));
   	   		err = shmgettest2();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	else if(TestStepName() == Kshmgettest3)
   			{
   	   		INFO_PRINTF1(_L("shmgettest3():"));
   	   		err = shmgettest3();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	else if(TestStepName() == Kshmgettest4)
   			{
   	   		INFO_PRINTF1(_L("shmgettest4():"));
   	   		err = shmgettest4();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	else if(TestStepName() == Kshmgettest5)
   			{
   	   		INFO_PRINTF1(_L("shmgettest5():"));
   	   		err = shmgettest5();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	else if(TestStepName() == Kshmgettest6)
   			{
   	   		INFO_PRINTF1(_L("shmgettest6():"));
   	   		err = shmgettest6();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	else if(TestStepName() == Kshmgettest7)
   			{
   	   		INFO_PRINTF1(_L("shmgettest7():"));
   	   		err = shmgettest7();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	else if(TestStepName() == Kshmgettest8)
   			{
   	   		INFO_PRINTF1(_L("shmgettest8():"));
   	   		err = shmgettest8();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KShmgetCreateKey)
   			{
   	   		INFO_PRINTF1(_L("ShmgetCreateKey():"));
   	   		err = ShmgetCreateKey();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	else if(TestStepName() == Kshmctltest1)
   			{
   	   		INFO_PRINTF1(_L("shmctltest1():"));
   	   		err = shmctltest1();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	else if(TestStepName() == Kshmctltest2)
   			{
   	   		INFO_PRINTF1(_L("shmctltest2():"));
   	   		err = shmctltest2();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	else if(TestStepName() == Kshmctltest3)
   			{
   	   		INFO_PRINTF1(_L("shmctltest3():"));
   	   		err = shmctltest3();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	else if(TestStepName() == Kshmctltest4)
   			{
   	   		INFO_PRINTF1(_L("shmctltest4():"));
   	   		err = shmctltest4();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	else if(TestStepName() == Kshmctltest5)
   			{
   	   		INFO_PRINTF1(_L("shmctltest5():"));
   	   		err = shmctltest5();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	else if(TestStepName() == Kshmattest1)
   			{
   	   		INFO_PRINTF1(_L("shmattest1():"));
   	   		err = shmattest1();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	else if(TestStepName() == Kshmattest2)
   			{
   	   		INFO_PRINTF1(_L("shmattest2():"));
   	   		err = shmattest2();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	else if(TestStepName() == Kshmattest3)
   			{
   	   		INFO_PRINTF1(_L("shmattest3():"));
   	   		err = shmattest3();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	else if(TestStepName() == Kshmattest4)
   			{
   	   		INFO_PRINTF1(_L("shmattest4():"));
   	   		err = shmattest4();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	else if(TestStepName() == Kshmattest5)
   			{
   	   		INFO_PRINTF1(_L("shmattest5():"));
   	   		err = shmattest5();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	else if(TestStepName() == Kshmdttest1)
   			{
   	   		INFO_PRINTF1(_L("shmdttest1():"));
   	   		err = shmdttest1();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	else if(TestStepName() == Kshmopsvalidatetest)
   			{
   	   		INFO_PRINTF1(_L("shmopsvalidatetest():"));
   	   		err = shmopsvalidatetest();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	
	return TestStepResult(); 

	}
 



