// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name        : twcharapi.cpp
// INCLUDE FILES
//



#include "twcharapi.h"

CTestWCharApi::~CTestWCharApi() 
	{ 
	}  

CTestWCharApi::CTestWCharApi(const TDesC& aStepName)
	{
	// MANDATORY Call to base class method to set up the human readable name for logging.
	SetTestStepName(aStepName);		
	}

TVerdict CTestWCharApi::doTestStepPreambleL()
	{
	__UHEAP_MARK;	
	SetTestStepResult(EPass);
	return TestStepResult();
	}



TVerdict CTestWCharApi::doTestStepPostambleL()
	{
	__UHEAP_MARKEND;	
	return TestStepResult();
	}

TVerdict CTestWCharApi::doTestStepL()
	{
		int err;

   		
		if(TestStepName() == Ktwasctime)
   			{
   	   		INFO_PRINTF1(_L("twasctime():"));
   	   		err = twasctime();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
   	   		if(TestStepName() == Ktwctime)
   			{
   	   		INFO_PRINTF1(_L("twctime():"));
   	   		err = twctime();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
   	   		if(TestStepName() == Ktwcslcat)
   			{
   	   		INFO_PRINTF1(_L("twcslcat():"));
   	   		err = twcslcat();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
   	   		if(TestStepName() == Ktwcslcpy)
   			{
   	   		INFO_PRINTF1(_L("twcslcpy():"));
   	   		err = twcslcpy();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
   	   		if(TestStepName() == Ktwsetlocale)
   			{
   	   		INFO_PRINTF1(_L("twsetlocale():"));
   	   		err = twsetlocale();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
   	   		if(TestStepName() == Ktwperror)
   			{
   	   		INFO_PRINTF1(_L("twperror():"));
   	   		err = twperror();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
   	   		if(TestStepName() == Ktwcserror)
   			{
   	   		INFO_PRINTF1(_L("twcserror():"));
   	   		err = twcserror();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
   	   		if(TestStepName() == Ktwfindfirst)
   			{
   	   		INFO_PRINTF1(_L("twfindfirst():"));
   	   		err = twfindfirst();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
   	   		if(TestStepName() == Ktwfindclose)
   			{
   	   		INFO_PRINTF1(_L("twfindclose():"));
   	   		err = twfindclose();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
   	   		if(TestStepName() == Ktwtelldir)
   			{
   	   		INFO_PRINTF1(_L("twtelldir():"));
   	   		err = twtelldir();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}   	   		
   	   	else
   	   		if(TestStepName() == Ktwseekdir)
   			{
   	   		INFO_PRINTF1(_L("twseekdir():"));
   	   		err = twseekdir();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}   
   	   	else
   	   		if(TestStepName() == Ktwsetlocale1)
   			{
   	   		INFO_PRINTF1(_L("twsetlocale1():"));
   	   		err = twsetlocale1();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}   
   	   	else
   	   		if(TestStepName() == Ktwfindfirst1)
   			{
   	   		INFO_PRINTF1(_L("twfindfirst1():"));
   	   		err = twfindfirst1();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}   
   	   	else
   	   		if(TestStepName() == Ktwfindnext1)
   			{
   	   		INFO_PRINTF1(_L("twfindnext1():"));
   	   		err = twfindnext1();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}      	   	
   	   	else if(TestStepName() == Kwcreat2)
			{
			INFO_PRINTF1(_L("wcreat2():"));
			err = wcreat2();
			SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
			}	 
		else if(TestStepName() == Kwpopen3_1)
			{
			INFO_PRINTF1(_L("wpopen3_1():"));
			err = wpopen3_1();
			SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
			}  	   		   	   		   	   	
   	   	else
   	   		if(TestStepName() == Ktwcslcpy1)
   			{
   	   		INFO_PRINTF1(_L("twcslcpy1():"));
   	   		err = twcslcpy1();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}   
   	   	else if(TestStepName() == Kwopen1)
			{
			INFO_PRINTF1(_L("wopen1():"));
			err = wopen1();
			SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
			}  
		else if(TestStepName() == Kwunlink1)
			{
			INFO_PRINTF1(_L("wunlink1():"));
			err = wunlink1();
			SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
			}
		else if(TestStepName() == Kwrmdir1)
			{
			INFO_PRINTF1(_L("wrmdir1():"));
			err = wrmdir1();
			SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
			}
		else if(TestStepName() == Kwmkdir1)
			{
			INFO_PRINTF1(_L("wmkdir1():"));
			err = wmkdir1();
			SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
			}
		else if(TestStepName() == Kwchmod1)
			{
			INFO_PRINTF1(_L("wchmod1():"));
			err = wchmod1();
			SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
			}
		else if(TestStepName() == Kwchdir1)
			{
			INFO_PRINTF1(_L("wchdir1():"));
			err = wchdir1();
			SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
			}
		else if(TestStepName() == Kwstat1)
			{
			INFO_PRINTF1(_L("wstat1():"));
			err = wstat1();
			SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
			}
		else if(TestStepName() == Kwstat2)
			{
			INFO_PRINTF1(_L("wstat2():"));
			err = wstat2();
			SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
			}
		else if(TestStepName() == Kwrename1)
			{
			INFO_PRINTF1(_L("wrename1():"));
			err = wrename1();
			SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
			}
		else if(TestStepName() == Kwrename2)
			{
			INFO_PRINTF1(_L("wrename2():"));
			err = wrename2();
			SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
			}
		else if(TestStepName() == Kwpopen_2)
			{
			INFO_PRINTF1(_L("wpopen_2():"));
			err = wpopen_2();
			SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
			}	 
		else if(TestStepName() == Kwpopen_3)
			{
			INFO_PRINTF1(_L("wpopen_3():"));
			err = wpopen_3();
			SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
			}
		return TestStepResult(); 

	}
 

	
