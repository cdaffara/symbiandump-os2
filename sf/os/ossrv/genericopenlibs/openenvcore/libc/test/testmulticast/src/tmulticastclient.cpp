// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Name        : tmulticastclient.cpp
// 
//


#include "tmulticastclient.h"



CTestMulticastClient::~CTestMulticastClient() 
	{ 
	}  

CTestMulticastClient::CTestMulticastClient(const TDesC& aStepName)
	{
	// MANDATORY Call to base class method to set up the human readable name for logging.
	SetTestStepName(aStepName);		
	}

TVerdict CTestMulticastClient::doTestStepPreambleL()
	{
	__UHEAP_MARK;	
	iParamCnt=0;
	SetTestStepResult(EPass);
	return TestStepResult();
	}



TVerdict CTestMulticastClient::doTestStepPostambleL()
	{
	__UHEAP_MARKEND;	
	return TestStepResult();
	}


TVerdict CTestMulticastClient::doTestStepL()
	{
	int err;

   	if(TestStepName() == KTestClient)
   		{
   		INFO_PRINTF1(_L("TestClient():"));
   		err = TestClient();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	return TestStepResult(); 
   	}
 

