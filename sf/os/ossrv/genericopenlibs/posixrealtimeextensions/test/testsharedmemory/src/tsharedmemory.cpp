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
// Name        : tsharedmemory.cpp
// 
//

#include "tsharedmemory.h"
#include <unistd.h> 
#include <errno.h>
#include <stdio.h>
#include <e32std.h>
#include <stdlib.h>
#include <string.h>


CTestsharedmemory::~CTestsharedmemory() 
	{ 
	}  

CTestsharedmemory::CTestsharedmemory(const TDesC& aStepName)
	{
	// MANDATORY Call to base class method to set up the human readable name for logging.
	SetTestStepName(aStepName);		
	}

TVerdict CTestsharedmemory::doTestStepPreambleL()
	{
	__UHEAP_MARK;	
	SetTestStepResult(EPass);
	return TestStepResult();
	}



TVerdict CTestsharedmemory::doTestStepPostambleL()
	{
	__UHEAP_MARKEND;	
	iParamCnt=0;
	return TestStepResult();
	}


TVerdict CTestsharedmemory::doTestStepL()
	{
	int err;
	if(TestStepName() == KTestsharedmemory1)
   		{
   	   	INFO_PRINTF1(_L("Testsharedmemory1():"));
   	   	err = Testsharedmemory1();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}   	
	if(TestStepName() == KTestsharedmemory2)
   		{
   	   	INFO_PRINTF1(_L("Testsharedmemory2():"));
   	   	err = Testsharedmemory2();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}     	
	if(TestStepName() == KTestsharedmemory3)
   		{
   	   	INFO_PRINTF1(_L("Testsharedmemory3():"));
   	   	err = Testsharedmemory3();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}     	
	if(TestStepName() == KTestsharedmemory4)
   		{
   	   	INFO_PRINTF1(_L("Testsharedmemory4():"));
   	   	err = Testsharedmemory4();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}      	
	if(TestStepName() == KTestsharedmemory5)
   		{
   	   	INFO_PRINTF1(_L("Testsharedmemory5():"));
   	   	err = Testsharedmemory5();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}      	
	if(TestStepName() == KTestsharedmemory6)
   		{
   	   	INFO_PRINTF1(_L("Testsharedmemory6():"));
   	   	err = Testsharedmemory6();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}      	
	if(TestStepName() == KTestsharedmemory7)
   		{
   	   	INFO_PRINTF1(_L("Testsharedmemory7():"));
   	   	err = Testsharedmemory7();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}      	
	if(TestStepName() == KTestsharedmemory8)
   		{
   	   	INFO_PRINTF1(_L("Testsharedmemory8():"));
   	   	err = Testsharedmemory8();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}      	
	if(TestStepName() == KTestsharedmemory9)
   		{
   	   	INFO_PRINTF1(_L("Testsharedmemory9():"));
   	   	err = Testsharedmemory9();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}      	
	if(TestStepName() == KTestsharedmemory10)
   		{
   	   	INFO_PRINTF1(_L("Testsharedmemory10():"));
   	   	err = Testsharedmemory10();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}       	
	if(TestStepName() == KTestsharedmemory11)
   		{
   	   	INFO_PRINTF1(_L("Testsharedmemory11():"));
   	   	err = Testsharedmemory11();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}       	
	if(TestStepName() == KTestsharedmemory12)
   		{
   	   	INFO_PRINTF1(_L("Testsharedmemory12():"));
   	   	err = Testsharedmemory12();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}       	
	if(TestStepName() == KTestsharedmemory13)
   		{
   	   	INFO_PRINTF1(_L("Testsharedmemory13():"));
   	   	err = Testsharedmemory13();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}        	
	if(TestStepName() == KTestsharedmemory14)
   		{
   	   	INFO_PRINTF1(_L("Testsharedmemory14():"));
   	   	err = Testsharedmemory14();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}        	
	if(TestStepName() == KTestsharedmemory15)
   		{
   	   	INFO_PRINTF1(_L("Testsharedmemory15():"));
   	   	err = Testsharedmemory15();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}         	
	if(TestStepName() == KTestsharedmemory16)
   		{
   	   	INFO_PRINTF1(_L("Testsharedmemory16():"));
   	   	err = Testsharedmemory16();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}          	
	if(TestStepName() == KTestsharedmemory17)
   		{
   	   	INFO_PRINTF1(_L("Testsharedmemory17():"));
   	   	err = Testsharedmemory17();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}         	
	if(TestStepName() == KTestsharedmemory18)
   		{
   	   	INFO_PRINTF1(_L("Testsharedmemory18():"));
   	   	err = Testsharedmemory18();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}         	
	if(TestStepName() == KTestsharedmemory19)
   		{
   	   	INFO_PRINTF1(_L("Testsharedmemory19():"));
   	   	err = Testsharedmemory19();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}        	
	if(TestStepName() == KTestsharedmemory20)
   		{
   	   	INFO_PRINTF1(_L("Testsharedmemory20():"));
   	   	err = Testsharedmemory20();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}        	
	if(TestStepName() == KTestsharedmemory21)
   		{
   	   	INFO_PRINTF1(_L("Testsharedmemory21():"));
   	   	err = Testsharedmemory21();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}       	
	if(TestStepName() == KTestsharedmemory22)
   		{
   	   	INFO_PRINTF1(_L("Testsharedmemory22():"));
   	   	err = Testsharedmemory22();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}        	
	if(TestStepName() == KTestsharedmemory23)
   		{
   	   	INFO_PRINTF1(_L("Testsharedmemory23():"));
   	   	err = Testsharedmemory23();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}       	
	if(TestStepName() == KTestsharedmemory24)
   		{
   	   	INFO_PRINTF1(_L("Testsharedmemory24():"));
   	   	err = Testsharedmemory24();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}        	
	if(TestStepName() == KTestsharedmemory25)
   		{
   	   	INFO_PRINTF1(_L("Testsharedmemory25():"));
   	   	err = Testsharedmemory25();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}       	
	if(TestStepName() == KTestsharedmemory26)
   		{
   	   	INFO_PRINTF1(_L("Testsharedmemory26():"));
   	   	err = Testsharedmemory26();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	if(TestStepName() == KTestsharedmemory27)
   		{
   	   	INFO_PRINTF1(_L("Testsharedmemory27():"));
   	   	err = Testsharedmemory27();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	if(TestStepName() == KTestsharedmemory28)
   		{
   	   	INFO_PRINTF1(_L("Testsharedmemory28():"));
   	   	err = Testsharedmemory28();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	if(TestStepName() == KTestsharedmemory29)
   		{
   	   	INFO_PRINTF1(_L("Testsharedmemory29():"));
   	   	err = Testsharedmemory29();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	if(TestStepName() == KTestsharedmemory30)
   		{
   	   	INFO_PRINTF1(_L("Testsharedmemory30():"));
   	   	err = Testsharedmemory30();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	return TestStepResult();

	}
 

