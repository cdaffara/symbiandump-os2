// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name        : ttrapsignal.cpp
// 
//

#include "ttrapsignal.h"
#include <unistd.h> 
#include <errno.h>
#include <stdio.h>
#include <e32std.h>
#include <stdlib.h>
#include <string.h>


CTestTrapsignal::~CTestTrapsignal() 
	{ 
	}  

CTestTrapsignal::CTestTrapsignal(const TDesC& aStepName)
	{
	// MANDATORY Call to base class method to set up the human readable name for logging.
	SetTestStepName(aStepName);		
	}

TVerdict CTestTrapsignal::doTestStepPreambleL()
	{
	__UHEAP_MARK;	
	SetTestStepResult(EPass);
	return TestStepResult();
	}



TVerdict CTestTrapsignal::doTestStepPostambleL()
	{
	__UHEAP_MARKEND;	
	iParamCnt=0;
	return TestStepResult();
	}


TVerdict CTestTrapsignal::doTestStepL()
	{
	int err;
	if(TestStepName() == KTestsigactionapi)
   		{
   	   	INFO_PRINTF1(_L("Testsigactionapi():"));
   	   	err = Testsigactionapi();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KTestcustomsignalapi)
   		{
   	   	INFO_PRINTF1(_L("Testcustomsignalapi():"));
   	   	err = Testcustomsignalapi();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KTestcustomsigsetapi1)
   		{
   	   	INFO_PRINTF1(_L("Testcustomsigsetapi1():"));
   	   	err = Testcustomsigsetapi1();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KTestcustomsigsetapi2)
   		{
   	   	INFO_PRINTF1(_L("Testcustomsigsetapi2():"));
   	   	err = Testcustomsigsetapi2();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KTestcustombsd_signalapi)
   		{
   	   	INFO_PRINTF1(_L("Testcustombsd_signalapi():"));
   	   	err = Testcustombsd_signalapi();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KTestcustomsigactionapi)
   		{
   	   	INFO_PRINTF1(_L("Testcustomsigactionapi():"));
   	   	err = Testcustomsigactionapi();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == KTestnegcustomsignalapi)
   		{
   	   	INFO_PRINTF1(_L("Testnegcustomignalapi():"));
   	   	err = Testnegcustomsignalapi();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}  
   	else if(TestStepName() == KTestnegcustomsigsetapi)
   		{
   	   	INFO_PRINTF1(_L("Testnegcustomsigsetapi():"));
   	   	err = Testnegcustomsigsetapi();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}  
   	   	else if(TestStepName() == KTestnegcustombsd_signalapi)
   		{
   	   	INFO_PRINTF1(_L("Testnegcustombsd_signalapi():"));
   	   	err = Testnegcustombsd_signalapi();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}  
   	else if(TestStepName() == KTestnegcustomsigactionapi)
   		{
   	   	INFO_PRINTF1(_L("Testnegcustomsigactionapi():"));
   	   	err = Testnegcustomsigactionapi();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}    
   	else if(TestStepName() == KTestignoresignalapi)
   		{
   	   	INFO_PRINTF1(_L("Testignoresignalapi():"));
   	   	err = Testignoresignalapi();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}      
   	else if(TestStepName() == KTestignoresigsetapi1)
   		{
   	   	INFO_PRINTF1(_L("Testignoresigsetapi1():"));
   	   	err = Testignoresigsetapi1();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}      
   	else if(TestStepName() == KTestignoresigsetapi2)
   		{
   	   	INFO_PRINTF1(_L("Testignoresigsetapi2():"));
   	   	err = Testignoresigsetapi2();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}      
   	else if(TestStepName() == KTestignorebsd_signalapi)
   		{
   	   	INFO_PRINTF1(_L("Testignorebsd_signalapi():"));
   	   	err = Testignorebsd_signalapi();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}      
   	else if(TestStepName() == KTestignoresigactionapi)
   		{
   	   	INFO_PRINTF1(_L("Testignoresigactionapi():"));
   	   	err = Testignoresigactionapi();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	} 
   	else if(TestStepName() == KTestsigignoreapi)
   		{
   	   	INFO_PRINTF1(_L("Testsigignoreapi():"));
   	   	err = Testsigignoreapi();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	} 
   	
   	else if(TestStepName() == KTestnegignoresignalapi)
   		{
   	   	INFO_PRINTF1(_L("Testnegignoresignalapi():"));
   	   	err = Testnegignoresignalapi();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}  
   	else if(TestStepName() == KTestnegignoresigsetapi)
   		{
   	   	INFO_PRINTF1(_L("Testnegignoresigsetapi():"));
   	   	err = Testnegignoresigsetapi();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}  
   	else if(TestStepName() == KTestnegignorebsd_signalapi)
   		{
   	   	INFO_PRINTF1(_L("Testnegignorebsd_signalapi():"));
   	   	err = Testnegignorebsd_signalapi();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}  
   	else if(TestStepName() == KTestnegignoresigactionapi)
   		{
   	   	INFO_PRINTF1(_L("Testnegignoresigactionapi():"));
   	   	err = Testnegignoresigactionapi();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}    
   	else if(TestStepName() == KTestnegsigignoreapi)
   		{
   	   	INFO_PRINTF1(_L("Testnegsigignoreapi():"));
   	   	err = Testnegsigignoreapi();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}    

   	else if(TestStepName() == KTestnegdefaultsignalapi)
   		{
   	   	INFO_PRINTF1(_L("Testnegdefaultsignalapi():"));
   	   	err = Testnegdefaultsignalapi();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}  
   	else if(TestStepName() == KTestnegdefaultsigsetapi)
   		{
   	   	INFO_PRINTF1(_L("Testnegdefaultsigsetapi():"));
   	   	err = Testnegdefaultsigsetapi();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}  
   	else if(TestStepName() == KTestnegdefaultbsd_signalapi)
   		{
   	   	INFO_PRINTF1(_L("Testnegdefaultbsd_signalapi():"));
   	   	err = Testnegdefaultbsd_signalapi();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}  
   	else if(TestStepName() == KTestnegdefaultsigactionapi)
   		{
   	   	INFO_PRINTF1(_L("Testnegdefaultsigactionapi():"));
   	   	err = Testnegdefaultsigactionapi();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}   
   	else if(TestStepName() == KTestsigsethold1)
   		{
   	   	INFO_PRINTF1(_L("Testsigsethold1():"));
   	   	err = Testsigsethold1();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	} 
   	else if(TestStepName() == KTestsigactionapi1)
   		{
   	   	INFO_PRINTF1(_L("Testsigactionapi1():"));
   	   	err = Testsigactionapi1();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}   
   	else if(TestStepName() == KTestsigactionapi2)
   		{
   	   	INFO_PRINTF1(_L("Testsigactionapi2():"));
   	   	err = Testsigactionapi2();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}   
   	else if(TestStepName() == KTestsigactionapi3)
   		{
   	   	INFO_PRINTF1(_L("Testsigactionapi3():"));
   	   	err = Testsigactionapi3();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}    
   	else if(TestStepName() == KTestsigactionapi4)
   		{
   	   	INFO_PRINTF1(_L("Testsigactionapi4():"));
   	   	err = Testsigactionapi4();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}     
   	else if(TestStepName() == KTestsigactionapi5)
   		{
   	   	INFO_PRINTF1(_L("Testsigactionapi5():"));
   	   	err = Testsigactionapi5();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}    
   	else if(TestStepName() == KTestsigactionapi6)
   		{
   	   	INFO_PRINTF1(_L("Testsigactionapi6():"));
   	   	err = Testsigactionapi6();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}  
	return TestStepResult();

	}
 

