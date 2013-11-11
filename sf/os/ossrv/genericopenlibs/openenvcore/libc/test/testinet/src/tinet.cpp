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



#include "tinet.h"


CTestInet::~CTestInet() 
	{ 
	}  

CTestInet::CTestInet(const TDesC& aStepName)
	{
	// MANDATORY Call to base class method to set up the human readable name for logging.
	SetTestStepName(aStepName);		
	}

TVerdict CTestInet::doTestStepPreambleL()
	{
	__UHEAP_MARK;	
	iParamCnt=0;
	SetTestStepResult(EPass);
	return TestStepResult();
	}



TVerdict CTestInet::doTestStepPostambleL()
	{
	__UHEAP_MARKEND;	
	return TestStepResult();
	}


TVerdict CTestInet::doTestStepL()
	{
	int err;

	if(TestStepName() == KInet_addr_with_valid_input)
		{
   		INFO_PRINTF1(_L("Inet_addr_with_valid_input():"));
   		err = Inet_addr_with_valid_input();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KInet_addr_with_invalid_input)
		{
   		INFO_PRINTF1(_L("Inet_addr_with_invalid_input():"));
   		err = Inet_addr_with_invalid_input();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KInet_ntoaTest)
		{
   		INFO_PRINTF1(_L("Inet_ntoaTest():"));
   		err = Inet_ntoaTest();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KInet_ptonTest)
		{
   		INFO_PRINTF1(_L("Inet_ptonTest():"));
   		err = Inet_ptonTest();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KInet_ntopTest)
		{
   		INFO_PRINTF1(_L("Inet_ntopTest():"));
   		err = Inet_ntopTest();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	return TestStepResult(); 

	}
 


