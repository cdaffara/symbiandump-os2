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


#include "tselect.h"




CTestSelect::~CTestSelect() 
	{ 
	}  

CTestSelect::CTestSelect(const TDesC& aStepName)
	{
	// MANDATORY Call to base class method to set up the human readable name for logging.
	SetTestStepName(aStepName);		
	}

TVerdict CTestSelect::doTestStepPreambleL()
	{
	__UHEAP_MARK;	
	SetTestStepResult(EPass);
	return TestStepResult();
	}



TVerdict CTestSelect::doTestStepPostambleL()
	{
	__UHEAP_MARKEND;	
	return TestStepResult();
	}


TVerdict CTestSelect::doTestStepL()
	{
		int err;
   		if(TestStepName() == KTestTimeout)
   			{
   	   		INFO_PRINTF1(_L("TestTimeout():"));
   	   		err = TestTimeout();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
   	   		if(TestStepName() == KTestReadFileDesc)
   			{
   	   		INFO_PRINTF1(_L("TestReadFileDesc():"));
   	   		err = TestReadFileDesc();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
   	   		if(TestStepName() == KTestWriteFileDesc)
   			{
   	   		INFO_PRINTF1(_L("TestWriteFileDesc():"));
   	   		err = TestWriteFileDesc();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
   	   		if(TestStepName() == KTestReadWriteFileDesc)
   			{
   	   		INFO_PRINTF1(_L("TestReadWriteFileDesc():"));
   	   		err = TestReadWriteFileDesc();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
   	   		if(TestStepName() == KTestPipeReadDesc)
   			{
   	   		INFO_PRINTF1(_L("TestPipeReadDesc():"));
   	   		err = TestPipeReadDesc();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
   	   		if(TestStepName() == KTestPipeReadDesc1)
   			{
   	   		INFO_PRINTF1(_L("TestPipeReadDesc1():"));
   	   		err = TestPipeReadDesc1();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
   	   		if(TestStepName() == KTestPipeWriteDesc)
   			{
   	   		INFO_PRINTF1(_L("TestPipeWriteDesc():"));
   	   		err = TestPipeWriteDesc();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
   	   		if(TestStepName() == KTestSocketDesc)
   			{
   	   		INFO_PRINTF1(_L("TestSocketDesc():"));
   	   		err = TestSocketDesc();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
   	   		if(TestStepName() == KTestSocketDesc2)
   			{
   	   		INFO_PRINTF1(_L("TestSocketDesc2():"));
   	   		err = TestSocketDesc2();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
   	   		if(TestStepName() == KTestSocketDesc3)
   			{
   	   		INFO_PRINTF1(_L("TestSocketDesc3():"));
   	   		err = TestSocketDesc3();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
   	   		if(TestStepName() == KTestConnectErrorDesc)
   			{
   	   		INFO_PRINTF1(_L("TestConnectErrorDesc():"));
   	   		err = TestConnectErrorDesc();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
   	   		if(TestStepName() == KTestMultipleDesc)
   			{
   	   		INFO_PRINTF1(_L("TestMultipleDesc():"));
   	   		err = TestMultipleDesc();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	   	if(TestStepName() == KTestSelectOnBadPipeEnd1)
   	   	    {
   	   	   	INFO_PRINTF1(_L("TestSelectOnBadPipeEnd1():"));
   	   	   	err = TestSelectOnBadPipeEnd1();
   	   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	   	}
   	   	else
   	   	   	if(TestStepName() == KTestSelectOnBadPipeEnd2)
   	   	   	{
   	   	   	INFO_PRINTF1(_L("TestSelectOnBadPipeEnd2():"));
   	   	   	err = TestSelectOnBadPipeEnd2();
   	   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	   	}
   	   	else
   	   	   	if(TestStepName() == KTestSelectLargeFd)
   	   	   	   	{
   	   	   	   	INFO_PRINTF1(_L("TestSelectLargeFd():"));
   	   	   	   	err = TestSelectLargeFd();
   	   	   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	   	   	}
   	   	else
   	   	    if(TestStepName() == KTestSelectRedirDescWrite )
   	   	        {
   	   	        INFO_PRINTF1(_L("KTestSelectRedirDescWrite():"));
   	   	        err = TestSelectRedirDescWrite();
   	   	        SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	        }
   	    else
   	        if(TestStepName() == KTestSelectConnectFailure )
   	            {
   	            INFO_PRINTF1(_L("KTestSelectConnectFailure():"));
   	            err = TestSelectConnectFailure();
   	            SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	            }
   	   	   	
   	   	
   	   	return TestStepResult(); 

	}
 


