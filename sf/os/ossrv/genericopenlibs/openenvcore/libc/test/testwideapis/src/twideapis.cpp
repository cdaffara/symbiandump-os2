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
// Name        : twideapis.cpp
// This material, including documentation and any related 
// computer programs, is protected by copyright controlled by 
// Nokia Corporation. All rights are reserved. Copying, 
// including reproducing, storing,  adapting or translating, any 
// or all of this material requires the prior written consent of 
// Nokia Corporation. This material also contains confidential 
// information which may not be disclosed to others without the 
// prior written consent of Nokia Corporation.
// INCLUDE FILES
//



#include "twideapis.h"

CTestWideApi::~CTestWideApi() 
	{ 
	}  

CTestWideApi::CTestWideApi(const TDesC& aStepName)
	{
	// MANDATORY Call to base class method to set up the human readable name for logging.
	SetTestStepName(aStepName);		
	}

TVerdict CTestWideApi::doTestStepPreambleL()
	{
	__UHEAP_MARK;	
	SetTestStepResult(EPass);
	return TestStepResult();
	}



TVerdict CTestWideApi::doTestStepPostambleL()
	{
	__UHEAP_MARKEND;	
	return TestStepResult();
	}

TVerdict CTestWideApi::doTestStepL()
	{
		int err;

   		if(TestStepName() == Kwfreopen_val)
   			{
   	   		INFO_PRINTF1(_L("wfreopen_val():"));
   	   		err = wfreopen_val();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
   	   		if(TestStepName() == Kwfreopen_valm)
   			{
   	   		INFO_PRINTF1(_L("wfreopen_valm():"));
   	   		err = wfreopen_valm();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
   	   		if(TestStepName() == Kwfreopen_valinv)
   			{
   	   		INFO_PRINTF1(_L("wfreopen_valinv():"));
   	   		err = wfreopen_valinv();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}   	   		
   	   	else
   	   		if(TestStepName() == Kwfdopen_val)
   			{
   	   		INFO_PRINTF1(_L("wfdopen_val():"));
   	   		err = wfdopen_val();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}   	   		
   	   	else
   	   		if(TestStepName() == Kwfdopen_ivalm)
   			{
   	   		INFO_PRINTF1(_L("wfdopen_ivalm():"));
   	   		err = wfdopen_ivalm();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}   	   		
   	   	else
   	   		if(TestStepName() == Kgetws_val)
   			{
   	   		INFO_PRINTF1(_L("getws_val():"));
   	   		err = getws_val();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}   	   		
   	   	else
   	   		if(TestStepName() == Kgetws_null)
   			{
   	   		INFO_PRINTF1(_L("getws_null():"));
   	   		err = getws_null();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}   	   		
   	   	else
   	   		if(TestStepName() == Kputws_val1)
   			{
   	   		INFO_PRINTF1(_L("putws_val1():"));
   	   		err = putws_val1();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}   	   		
   	   	else
   	   		if(TestStepName() == Kputws_val2)
   			{
   	   		INFO_PRINTF1(_L("putws_val2():"));
   	   		err = putws_val2();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}   	   		
   	   	else
   	   		if(TestStepName() == Kputws_null)
   			{
   	   		INFO_PRINTF1(_L("putws_null():"));
   	   		err = putws_null();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}   	   		
   	   	else
   	   		if(TestStepName() == Kwremove_val1)
   			{
   	   		INFO_PRINTF1(_L("wremove_val1():"));
   	   		err = wremove_val1();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}   	   		
   	   	else
   	   		if(TestStepName() == Kwremove_val2)
   			{
   	   		INFO_PRINTF1(_L("wremove_val2():"));
   	   		err = wremove_val2();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}   	   		
   	   	else
   	   		if(TestStepName() == Kwremove_val3)
   			{
   	   		INFO_PRINTF1(_L("wremove_val3():"));
   	   		err = wremove_val3();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}   	   		
   	   	else
   	   		if(TestStepName() == Kwremove_null)
   			{
   	   		INFO_PRINTF1(_L("wremove_null():"));
   	   		err = wremove_null();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}   	   		
   	   	else
   	   		if(TestStepName() == Kwfdopen_ivalm1)
   			{
   	   		INFO_PRINTF1(_L("wfdopen_ivalm1():"));
   	   		err = wfdopen_ivalm1();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}    
   	   	else
   	   		if(TestStepName() == Kwpopen_1)
   			{
   	   		INFO_PRINTF1(_L("wpopen_1():"));
   	   		err = wpopen_1();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		} 
 	  else
   	   		if(TestStepName() == Kwfreopen1)
   			{
   	   		INFO_PRINTF1(_L("wfreopen1():"));
   	   		err = wfreopen1();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		} 
   	   else
   	   		if(TestStepName() == Kwfreopen2)
   			{
   	   		INFO_PRINTF1(_L("wfreopen2():"));
   	   		err = wfreopen2();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		} 
   	   else
   	   		if(TestStepName() == Kwfreopen3)
   			{
   	   		INFO_PRINTF1(_L("wfreopen3():"));
   	   		err = wfreopen3();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		} 
   	   else
   	   		if(TestStepName() == Kwfreopen4)
   			{
   	   		INFO_PRINTF1(_L("wfreopen4():"));
   	   		err = wfreopen4();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		} 		   	   	
		return TestStepResult(); 

	}


	
