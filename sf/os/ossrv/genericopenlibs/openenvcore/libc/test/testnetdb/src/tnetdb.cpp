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



#include "tnetdb.h"



CTestNetdb::~CTestNetdb() 
	{ 
	}  

CTestNetdb::CTestNetdb(const TDesC& aStepName)
	{
	// MANDATORY Call to base class method to set up the human readable name for logging.
	SetTestStepName(aStepName);		
	}

TVerdict CTestNetdb::doTestStepPreambleL()
	{
	__UHEAP_MARK;	
	iParamCnt=0;
	SetTestStepResult(EPass);
	return TestStepResult();
	}



TVerdict CTestNetdb::doTestStepPostambleL()
	{
	__UHEAP_MARKEND;	
	return TestStepResult();
	}


TVerdict CTestNetdb::doTestStepL()
	{
	int err;

   	if(TestStepName() == KGethostbynameNegativeTest)
   		{
   		INFO_PRINTF1(_L("GethostbynameNegativeTest():"));
   		err = GethostbynameNegativeTest();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KGetHostByNameLocalHost)
   		{
   		INFO_PRINTF1(_L("GetHostByNameLocalHost():"));
   		err = GetHostByNameLocalHost();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	 else
   	   	if(TestStepName() == KGethostbyaddrTest)
   		{
   	   	INFO_PRINTF1(_L("GethostbyaddrTest():"));
   	   	err = GethostbyaddrTest();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else
   	   	if(TestStepName() == KGetprotobynameTest)
   		{
   	   	INFO_PRINTF1(_L("GetprotobynameTest():"));
   	   	err = GetprotobynameTest();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else
   	   	if(TestStepName() == KGetprotobynumberTest)
   		{
   	   	INFO_PRINTF1(_L("GetprotobynumberTest():"));
   	   	err = GetprotobynumberTest();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else
   	   	if(TestStepName() == KGetservbynameTest)
   		{
   	   	INFO_PRINTF1(_L("GetservbynameTest():"));
   	   	err = GetservbynameTest();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else
   	   	if(TestStepName() == KGetservbyportTest)
   		{
   	   	INFO_PRINTF1(_L("GetservbyportTest():"));
   	   	err = GetservbyportTest();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else
   	   	if(TestStepName() == KGetaddrinfoTest)
   		{
   	   	INFO_PRINTF1(_L("GetaddrinfoTest():"));
   	   	err = GetaddrinfoTest();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else
   	   	if(TestStepName() == KGetnameinfoTest)
   		{
   	   	INFO_PRINTF1(_L("GetnameinfoTest():"));
   	   	err = GetnameinfoTest();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else
   	   	if(TestStepName() == KGetnameinfoTest1)
   		{
   	   	INFO_PRINTF1(_L("GetnameinfoTest1():"));
   	   	err = GetnameinfoTest1();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else
   	   	if(TestStepName() == KSetserventTest)
   		{
   	   	INFO_PRINTF1(_L("SetserventTest():"));
   	   	err = SetserventTest();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else
   	   	if(TestStepName() == KGai_strerrorTest)
   		{
   	   	INFO_PRINTF1(_L("Gai_strerrorTest():"));
   	   	err = Gai_strerrorTest();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else
   	   	if(TestStepName() == KFreeaddrinfoTest)
   		{
   	   	INFO_PRINTF1(_L("FreeaddrinfoTest():"));
   	   	err = FreeaddrinfoTest();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else
   	   	if(TestStepName() == KGetserventTest)
   		{
   	   	INFO_PRINTF1(_L("GetserventTest():"));
   	   	err = GetserventTest();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else
   	   	if(TestStepName() == KEndserventTest)
   		{
   	   	INFO_PRINTF1(_L("EndserventTest():"));
   	   	err = EndserventTest();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	 else
	   	if(TestStepName() == KSetDefaultIfTest1)
		{
	   	INFO_PRINTF1(_L("SetDefaultIfTest1():"));
	   	err = SetDefaultIfTest1();
	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
	   	}
   	 else
	   	if(TestStepName() == KSetDefaultIfTest2)
		{
	   	INFO_PRINTF1(_L("SetDefaultIfTest2():"));
	   	err = SetDefaultIfTest2();
	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
	   	}	
   	 else
	   	if(TestStepName() == KGetAddrInfoMultipleIPTest)
		{
	   	INFO_PRINTF1(_L("GetAddrInfoMultipleIPTest():"));
	   	err = GetAddrInfoMultipleIPTest();
	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
	   	}	
	else
   	   	if(TestStepName() == KGetnameinfoTest_localhost)
   		{
   	   	INFO_PRINTF1(_L("GetnameinfoTest_localhost():"));
   	   	err = GetnameinfoTest_localhost();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else
   	   	if(TestStepName() == KGetAddrInfoNegativeTests)
   		{
   	   	INFO_PRINTF1(_L("GetAddrInfoNegativeTests():"));
   	   	err = GetAddrInfoNegativeTests();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}   		
		return TestStepResult(); 
	}
 

