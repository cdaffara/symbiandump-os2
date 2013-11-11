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



#include "tmutex.h"



CTestMutex::~CTestMutex() 
	{ 
	}  

CTestMutex::CTestMutex(const TDesC& aStepName)
	{
	// MANDATORY Call to base class method to set up the human readable name for logging.
	SetTestStepName(aStepName);		
	}

TVerdict CTestMutex::doTestStepPreambleL()
	{
	__UHEAP_MARK;	
	SetTestStepResult(EPass);
	return TestStepResult();
	}



TVerdict CTestMutex::doTestStepPostambleL()
	{
	__UHEAP_MARKEND;	
	return TestStepResult();
	}


TVerdict CTestMutex::doTestStepL()
	{
	int err;

   	if(TestStepName() == KPThreadAttrStaticInitTryLockL)
   		{
   	  	INFO_PRINTF1(_L("PThreadAttrStaticInitTryLockL():"));
   	  	err = PThreadAttrStaticInitTryLockL();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KPThreadMutexLockRecursive)
   		{
   	   	INFO_PRINTF1(_L("PThreadMutexLockRecursive():"));
   	   	err = PThreadMutexLockRecursive();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KPThreadMutexLockErrorCheck)
   		{
   	   	INFO_PRINTF1(_L("PThreadMutexLockErrorCheck():"));
   	   	err = PThreadMutexLockErrorCheck();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KPThreadOnce)
   		{
   	   	INFO_PRINTF1(_L("PThreadOnce():"));
   	   	err = PThreadOnce();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KSemInit)
   		{
   	   	INFO_PRINTF1(_L("SemInit():"));
   	   	err = SemInit();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KSemDestroy)
   		{
   	   	INFO_PRINTF1(_L("SemDestroy():"));
   	   	err = SemDestroy();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KSemTryWait)
   		{
   	   	INFO_PRINTF1(_L("SemTryWait():"));
   	   	err = SemTryWait();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KSemWait)
   		{
   	   	INFO_PRINTF1(_L("SemWait():"));
   	   	err = SemWait();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KSemTimedWait)
   		{
   	   	INFO_PRINTF1(_L("SemTimedWait():"));
   	   	err = SemTimedWait();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KSemPost)
   		{
   	   	INFO_PRINTF1(_L("SemPost():"));
   	   	err = SemPost();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KSemClose)
   		{
   	   	INFO_PRINTF1(_L("SemClose():"));
   	   	err = SemClose();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KSemUnlink)
   		{
   	   	INFO_PRINTF1(_L("SemUnlink():"));
   	   	err = SemUnlink();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KSemGetValue)
   		{
   	   	INFO_PRINTF1(_L("SemGetValue():"));
   	   	err = SemGetValue();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KSemMicroSleepwaitNP)
   		{
   	   	INFO_PRINTF1(_L("SemMicroSleepwaitNP():"));
   	   	err = SemMicroSleepwaitNP();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KSemPostMultiple)
   		{
   	   	INFO_PRINTF1(_L("SemPostMultiple():"));
   	   	err = SemPostMultiple();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KPThreadCondAttrInit)
   		{
   	   	INFO_PRINTF1(_L("PThreadCondAttrInit():"));
   	   	err = PThreadCondAttrInit();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KPThreadCondAttrDestroy)
   		{
   	   	INFO_PRINTF1(_L("PThreadCondAttrDestroy():"));
   	   	err = PThreadCondAttrDestroy();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KPThreadCondInit)
   		{
   	   	INFO_PRINTF1(_L("PThreadCondInit():"));
   	   	err = PThreadCondInit();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KPThreadCondDestroy)
   		{
   	   	INFO_PRINTF1(_L("PThreadCondDestroy():"));
   	   	err = PThreadCondDestroy();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KPThreadCondTimedwait)
   		{
   	   	INFO_PRINTF1(_L("PThreadCondTimedwait():"));
   	   	err = PThreadCondTimedwait();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KPThreadCondWait)
   		{
   	   	INFO_PRINTF1(_L("PThreadCondWait():"));
   	   	err = PThreadCondWait();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KPThreadCondSignal)
   		{
   	   	INFO_PRINTF1(_L("PThreadCondSignal():"));
   	   	err = PThreadCondSignal();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KPThreadCondBroadcast)
   		{
   	   	INFO_PRINTF1(_L("PThreadCondBroadcast():"));
   	   	err = PThreadCondBroadcast();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KPThreadAttrStaticInitL)
   		{
   	   	INFO_PRINTF1(_L("PThreadAttrStaticInitL():"));
   	   	err = PThreadAttrStaticInitL();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KPThreadAttrStaticInitTryLockL)
   		{
   	   	INFO_PRINTF1(_L("PThreadAttrStaticInitTryLockL():"));
   	   	err = PThreadAttrStaticInitTryLockL();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KIntgTest1)
   		{
   	   	INFO_PRINTF1(_L("IntgTest1():"));
   	   	err = IntgTest1();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KIntgTest2)
   		{
   	   	INFO_PRINTF1(_L("IntgTest2():"));
   	   	err = IntgTest2();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KIntgTest3)
   		{
   	   	INFO_PRINTF1(_L("IntgTest3():"));
   	   	err = IntgTest3();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KIntgTest4)
   		{
   	   	INFO_PRINTF1(_L("IntgTest4():"));
   	   	err = IntgTest4();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KIntgTest5)
   		{
   	   	INFO_PRINTF1(_L("IntgTest5():"));
   	   	err = IntgTest5();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KIntgTest6)
   		{
   	   	INFO_PRINTF1(_L("IntgTest6():"));
   	   	err = IntgTest6();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KIntgTest7)
   		{
   	   	INFO_PRINTF1(_L("IntgTest7():"));
   	   	err = IntgTest7();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KIntgTest8)
   		{
   	   	INFO_PRINTF1(_L("IntgTest8():"));
   	   	err = IntgTest8();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KIntgTest9)
   		{
   	   	INFO_PRINTF1(_L("IntgTest9():"));
   	   	err = IntgTest9();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == KIntgTest10)
   		{
   	   	INFO_PRINTF1(_L("IntgTest10():"));
   	   	err = IntgTest10();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KIntgTest11)
   		{
   	   	INFO_PRINTF1(_L("IntgTest11():"));
   	   	err = IntgTest11();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
		}
	else if(TestStepName() == KIntgTest12)
   		{
   	   	INFO_PRINTF1(_L("IntgTest12():"));
   	   	err = IntgTest12();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KIntgTest13)
   		{
   	   	INFO_PRINTF1(_L("IntgTest13():"));
   	   	err = IntgTest13();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}		
	else if(TestStepName() == KIntgTest14)
   		{
   	   	INFO_PRINTF1(_L("IntgTest14():"));
   	   	err = IntgTest14();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KIntgTest15)
   		{
   	   	INFO_PRINTF1(_L("IntgTest15():"));
   	   	err = IntgTest15();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KIntgTest16)
   		{
   	   	INFO_PRINTF1(_L("IntgTest16():"));
   	   	err = IntgTest16();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KIntgTest17)
   		{
   	   	INFO_PRINTF1(_L("IntgTest17():"));
   	   	err = IntgTest17();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KIntgTest18)
   		{
   	   	INFO_PRINTF1(_L("IntgTest18():"));
   	   	err = IntgTest18();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KIntgTest19)
   		{
   	   	INFO_PRINTF1(_L("IntgTest19():"));
   	   	err = IntgTest19();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}	
	else if(TestStepName() == KIntgTest20)
   		{
   	   	INFO_PRINTF1(_L("IntgTest20():"));
   	   	err = IntgTest20();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}	
	else if(TestStepName() == KIntgTest21)
   		{
   	   	INFO_PRINTF1(_L("IntgTest21():"));
   	   	err = IntgTest21();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}	
	else if(TestStepName() == KIntgTest22)
   		{
   	   	INFO_PRINTF1(_L("IntgTest22():"));
   	   	err = IntgTest22();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}	
	else if(TestStepName() == KIntgTest23)
   		{
   	   	INFO_PRINTF1(_L("IntgTest23():"));
   	   	err = IntgTest23();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KIntgTest24)
   		{
   	   	INFO_PRINTF1(_L("IntgTest24():"));
   	   	err = IntgTest24();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KIntgTest25)
   		{
   	   	INFO_PRINTF1(_L("IntgTest25():"));
   	   	err = IntgTest25();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KIntgTest26)
   		{
   	   	INFO_PRINTF1(_L("IntgTest26():"));
   	   	err = IntgTest26();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
	else if(TestStepName() == KIntgTest27)
   		{
   	   	INFO_PRINTF1(_L("IntgTest27():"));
   	   	err = IntgTest27();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == KIntgTest28)
   		{
   	   	INFO_PRINTF1(_L("IntgTest28():"));
   	   	err = IntgTest28();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}	
	else if(TestStepName() == KIntgTest29)
   		{
   	   	INFO_PRINTF1(_L("IntgTest29():"));
   	   	err = IntgTest29();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KIntgTest30)
   		{
   	   	INFO_PRINTF1(_L("IntgTest30():"));
   	   	err = IntgTest30();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == KIntgTest31)
   		{
   	   	INFO_PRINTF1(_L("IntgTest31():"));
   	   	err = IntgTest31();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KIntgTest32)
   		{
   	   	INFO_PRINTF1(_L("IntgTest32():"));
   	   	err = IntgTest32();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}	
	else if(TestStepName() == KIntgTest33)
   		{
   	   	INFO_PRINTF1(_L("IntgTest33():"));
   	   	err = IntgTest33();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KIntgTest34)
   		{
   	   	INFO_PRINTF1(_L("IntgTest34():"));
   	   	err = IntgTest34();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KIntgTest35)
   		{
   	   	INFO_PRINTF1(_L("IntgTest35():"));
   	   	err = IntgTest35();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KIntgTest36)
   		{
   	   	INFO_PRINTF1(_L("IntgTest36():"));
   	   	err = IntgTest36();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KIntgTest37)
   		{
   	   	INFO_PRINTF1(_L("IntgTest37():"));
   	   	err = IntgTest37();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KIntgTest38)
   		{
   	   	INFO_PRINTF1(_L("IntgTest38():"));
   	   	err = IntgTest38();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KIntgTest39)
   		{
   	   	INFO_PRINTF1(_L("IntgTest39():"));
   	   	err = IntgTest39();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KPThreadMutexTryLockWait1)
	   		{
	   	   	INFO_PRINTF1(_L("PThreadMutexTryLockWait1():"));
	   	   	err = PThreadMutexTryLockWait1();
	   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
	   	   	}
	else if(TestStepName() == KPThreadMutexTryLockWait2)
	   		{
	   	   	INFO_PRINTF1(_L("PThreadMutexTryLockWait2():"));
	   	   	err = PThreadMutexTryLockWait2();
	   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
	   	   	}

   	return TestStepResult(); 

	}
 
	
