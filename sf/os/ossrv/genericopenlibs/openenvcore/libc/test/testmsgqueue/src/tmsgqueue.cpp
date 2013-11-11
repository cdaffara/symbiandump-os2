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



#include "tmsgqueue.h"
#include <unistd.h> 
#include <errno.h>
#include <stdio.h>
#include <e32std.h>
#include <stdlib.h>
#include <string.h>


CTestMsgqueue::~CTestMsgqueue() 
	{ 
	}  

CTestMsgqueue::CTestMsgqueue(const TDesC& aStepName)
	{
	// MANDATORY Call to base class method to set up the human readable name for logging.
	SetTestStepName(aStepName);		
	}

TVerdict CTestMsgqueue::doTestStepPreambleL()
	{
	__UHEAP_MARK;	
	iNoOfMsqIds = new int[MAX_IPC_OBJ];
    for (TInt i = 0; i < MAX_IPC_OBJ; i++)
        iNoOfMsqIds[i] = -1;
	SetTestStepResult(EPass);
	return TestStepResult();
	}



TVerdict CTestMsgqueue::doTestStepPostambleL()
	{
	delete []iNoOfMsqIds; 
	__UHEAP_MARKEND;	
	return TestStepResult();
	}


TVerdict CTestMsgqueue::doTestStepL()
	{
	int err;

  	if(TestStepName() == KMessageQueueCreate)
   		{
   		INFO_PRINTF1(_L("MessageQueueCreate():"));
   	 	err = MessageQueueCreate();
   	 	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else
   		if(TestStepName() == KMessageQueueControl)
   		{
   		INFO_PRINTF1(_L("MessageQueueControl():"));
   		err = MessageQueueControl();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else
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
   		if(TestStepName() == KIntgTest14)
   		{
   	   	INFO_PRINTF1(_L("IntgTest14():"));
   	   	err = IntgTest14();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else
   	   	if(TestStepName() == KIntgTest15)
   		{
   	   	INFO_PRINTF1(_L("IntgTest15():"));
   	   	err = IntgTest15();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else
   		if(TestStepName() == KIntgTest16)
   		{
   	   	INFO_PRINTF1(_L("IntgTest16():"));
   	   	err = IntgTest16();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else
   		if(TestStepName() == KIntgTest17)
   		{
   		INFO_PRINTF1(_L("IntgTest17():"));
   		err = IntgTest17();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else
   	   	if(TestStepName() == KIntgTest6_1)
   		{
   	   	INFO_PRINTF1(_L("IntgTest6_1():"));
   	   	err = IntgTest6_1();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == Kmsggettest1)
   		{
   		INFO_PRINTF1(_L("msggettest1():"));
   		err = msggettest1();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}   	
	else if(TestStepName() == Kmsggettest2)
   		{
   		INFO_PRINTF1(_L("msggettest2():"));
   		err = msggettest2();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}  
   	else if(TestStepName() == Kmsggettest3)
   		{
   		INFO_PRINTF1(_L("msggettest3():"));
   		err = msggettest3();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == Kmsggettest4)
   		{
   		INFO_PRINTF1(_L("msggettest4():"));
   		err = msggettest4();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == Kmsggettest5)
   		{
   		INFO_PRINTF1(_L("msggettest5():"));
   		err = msggettest5();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == Kmsggettest6)
   		{
   		INFO_PRINTF1(_L("msggettest6():"));
   		err = msggettest6();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KMsggetCreateKey)
   		{
   		INFO_PRINTF1(_L("MsggetCreateKey():"));
   		err = MsggetCreateKey();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == Kmsgctltest1)
   		{
   		INFO_PRINTF1(_L("msgctltest1():"));
   		err = msgctltest1();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == Kmsgctltest2)
   		{
   		INFO_PRINTF1(_L("msgctltest2():"));
   		err = msgctltest2();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == Kmsgctltest3)
   		{
   		INFO_PRINTF1(_L("msgctltest3():"));
   		err = msgctltest3();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == Kmsgctltest4)
   		{
   		INFO_PRINTF1(_L("msgctltest4():"));
   		err = msgctltest4();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == Kmsgctltest5)
   		{
   		INFO_PRINTF1(_L("msgctltest5():"));
   		err = msgctltest5();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == Kmsgsndtest1)
   		{
   		INFO_PRINTF1(_L("msgsndtest1():"));
   		err = msgsndtest1();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == Kmsgsndtest2)
   		{
   		INFO_PRINTF1(_L("msgsndtest2():"));
   		err = msgsndtest2();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == Kmsgsndtest3)
   		{
   		INFO_PRINTF1(_L("msgsndtest3():"));
   		err = msgsndtest3();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == Kmsgsndtest4)
   		{
   		INFO_PRINTF1(_L("msgsndtest4():"));
   		err = msgsndtest4();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == Kmsgsndtest5)
   		{
   		INFO_PRINTF1(_L("msgsndtest5():"));
   		err = msgsndtest5();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == Kmsgsndtest6)
   		{
   		INFO_PRINTF1(_L("msgsndtest6():"));
   		err = msgsndtest6();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == Kmsgrcvtest1)
   		{
   		INFO_PRINTF1(_L("msgrcvtest1():"));
   		err = msgrcvtest1();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == Kmsgrcvtest2)
   		{
   		INFO_PRINTF1(_L("msgrcvtest2():"));
   		err = msgrcvtest2();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Kmsgrcvtest3)
   		{
   		INFO_PRINTF1(_L("msgrcvtest3():"));
   		err = msgrcvtest3();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == Kmsgrcvtest4)
   		{
   		INFO_PRINTF1(_L("msgrcvtest4():"));
   		err = msgrcvtest4();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == Ksndrcvtest1)
   		{
   		INFO_PRINTF1(_L("sndrcvtest1():"));
   		err = sndrcvtest1();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == Ksndrcvtest2)
   		{
   		INFO_PRINTF1(_L("sndrcvtest2():"));
   		err = sndrcvtest2();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}   
   	else if(TestStepName() == Ksndrcvtest3)
   		{
   		INFO_PRINTF1(_L("sndrcvtest3():"));
   		err = sndrcvtest3();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == Ksndrcvtest4)
   		{
   		INFO_PRINTF1(_L("sndrcvtest4():"));
   		err = sndrcvtest4();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}	  
   	else if(TestStepName() == Kthsndrcvtest1)
   		{
   		INFO_PRINTF1(_L("thsndrcvtest1():"));
   		err = thsndrcvtest1();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	   	
	return TestStepResult(); 

	}
 


