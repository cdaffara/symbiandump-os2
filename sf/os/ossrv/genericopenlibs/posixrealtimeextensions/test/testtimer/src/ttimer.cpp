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
// Name        : ttimer.cpp
// 
//

#include "ttimer.h"



CTesttimer::~CTesttimer() 
	{ 
	}  

CTesttimer::CTesttimer(const TDesC& aStepName)
	{
	// MANDATORY Call to base class method to set up the human readable name for logging.
	SetTestStepName(aStepName);		
	}

TVerdict CTesttimer::doTestStepPreambleL()
	{
	__UHEAP_MARK;
	SetTestStepResult(EPass);
	return TestStepResult();
	}



TVerdict CTesttimer::doTestStepPostambleL()
	{
	__UHEAP_MARKEND;	
	iParamCnt=0;
	return TestStepResult();
	}


TVerdict CTesttimer::doTestStepL()
	{
	int err;
	if(TestStepName() == KTesttimerapi1)
   		{
   	   	INFO_PRINTF1(_L("Testtimerapi1():"));
   	   	err = Testtimerapi1();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}   	
	if(TestStepName() == KTesttimerapi2)
   		{
   	   	INFO_PRINTF1(_L("Testtimerapi2():"));
   	   	err = Testtimerapi2();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}     	
	if(TestStepName() == KTesttimerapi3)
   		{
   	   	INFO_PRINTF1(_L("Testtimerapi3():"));
   	   	err = Testtimerapi3();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}     	
	if(TestStepName() == KTesttimerapi4)
   		{
   	   	INFO_PRINTF1(_L("Testtimerapi4():"));
   	   	err = Testtimerapi4();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}      	
	if(TestStepName() == KTesttimerapi5)
   		{
   	   	INFO_PRINTF1(_L("Testtimerapi5():"));
   	   	err = Testtimerapi5();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}      	
	if(TestStepName() == KTesttimerapi6)
   		{
   	   	INFO_PRINTF1(_L("Testtimerapi6():"));
   	   	err = Testtimerapi6();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}      	
	if(TestStepName() == KTesttimerapi7)
   		{
   	   	INFO_PRINTF1(_L("Testtimerapi7():"));
   	   	err = Testtimerapi7();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}       	
	if(TestStepName() == KTesttimerapi8)
   		{
   	   	INFO_PRINTF1(_L("Testtimerapi8():"));
   	   	err = Testtimerapi8();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}      	
	if(TestStepName() == KTesttimerapi9)
   		{
   	   	INFO_PRINTF1(_L("Testtimerapi9():"));
   	   	err = Testtimerapi9();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}      	
	if(TestStepName() == KTesttimerapi10)
   		{
   	   	INFO_PRINTF1(_L("Testtimerapi10():"));
   	   	err = Testtimerapi10();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}      	
	if(TestStepName() == KTesttimerapi11)
   		{
   	   	INFO_PRINTF1(_L("Testtimerapi11():"));
   	   	err = Testtimerapi11();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}      	
	if(TestStepName() == KTesttimerapi12)
   		{
   	   	INFO_PRINTF1(_L("Testtimerapi12():"));
   	   	err = Testtimerapi12();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}      	
	if(TestStepName() == KTesttimerapi13)
   		{
   	   	INFO_PRINTF1(_L("Testtimerapi13():"));
   	   	err = Testtimerapi13();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}      	
	if(TestStepName() == KTesttimerapi14)
   		{
   	   	INFO_PRINTF1(_L("Testtimerapi14():"));
   	   	err = Testtimerapi14();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}       	
	if(TestStepName() == KTesttimerapi15)
   		{
   	   	INFO_PRINTF1(_L("Testtimerapi15():"));
   	   	err = Testtimerapi15();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}       	
	if(TestStepName() == KTesttimerapi16)
   		{
   	   	INFO_PRINTF1(_L("Testtimerapi16():"));
   	   	err = Testtimerapi16();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}       	
	if(TestStepName() == KTesttimerapi17)
   		{
   	   	INFO_PRINTF1(_L("Testtimerapi17():"));
   	   	err = Testtimerapi17();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}       	
	if(TestStepName() == KTesttimerapi18)
   		{
   	   	INFO_PRINTF1(_L("Testtimerapi18():"));
   	   	err = Testtimerapi18();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}        	
	if(TestStepName() == KTesttimerapi19)
   		{
   	   	INFO_PRINTF1(_L("Testtimerapi19():"));
   	   	err = Testtimerapi19();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}        	
	if(TestStepName() == KTesttimerapi20)
   		{
   	   	INFO_PRINTF1(_L("Testtimerapi20():"));
   	   	err = Testtimerapi20();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}        	
	if(TestStepName() == KTesttimerapi21)
   		{
   	   	INFO_PRINTF1(_L("Testtimerapi21():"));
   	   	err = Testtimerapi21();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}        	
	if(TestStepName() == KTesttimerapi22)
   		{
   	   	INFO_PRINTF1(_L("Testtimerapi22():"));
   	   	err = Testtimerapi22();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}        	
	if(TestStepName() == KTesttimerapi23)
   		{
   	   	INFO_PRINTF1(_L("Testtimerapi23():"));
   	   	err = Testtimerapi23();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}         	
	if(TestStepName() == KTesttimerapi24)
   		{
   	   	INFO_PRINTF1(_L("Testtimerapi24():"));
   	   	err = Testtimerapi24();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}           	
	if(TestStepName() == KTesttimerapi25)
   		{
   	   	INFO_PRINTF1(_L("Testtimerapi25():"));
   	   	err = Testtimerapi25();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}          	
	if(TestStepName() == KTesttimerapi26)
   		{
   	   	INFO_PRINTF1(_L("Testtimerapi26():"));
   	   	err = Testtimerapi26();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}           	
	if(TestStepName() == KTesttimerapi27)
   		{
   	   	INFO_PRINTF1(_L("Testtimerapi27():"));
   	   	err = Testtimerapi27();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
    if(TestStepName() == KTesttimerapi28)
        {
        INFO_PRINTF1(_L("Testtimerapi28():"));
        err = Testtimerapi28();
        SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
        } 
	
		return TestStepResult();
	}
 

