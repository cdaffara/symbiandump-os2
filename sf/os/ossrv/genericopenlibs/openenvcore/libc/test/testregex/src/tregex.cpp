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



#include "tregex.h"
#include <unistd.h> 



CTestRegex::~CTestRegex() 
	{ 
	}  

CTestRegex::CTestRegex(const TDesC& aStepName)
	{
	// MANDATORY Call to base class method to set up the human readable name for logging.
	SetTestStepName(aStepName);		
	}

TVerdict CTestRegex::doTestStepPreambleL()
	{
	__UHEAP_MARK;	
	iParamCnt=0;	
	SetTestStepResult(EPass);
	return TestStepResult();
	}



TVerdict CTestRegex::doTestStepPostambleL()
	{
	__UHEAP_MARKEND;	
	return TestStepResult();
	}


TVerdict CTestRegex::doTestStepL()
	{
	int err;
   	if(TestStepName() == Kregcomp_escape)
   		{
   		INFO_PRINTF1(_L("regcomp_escape():"));
   		err = regcomp_escape();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else
   		if(TestStepName() == Kregcomp_badrpt)
   			{
   	   		INFO_PRINTF1(_L("regcomp_badrpt():"));
   	   		err = regcomp_badrpt();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	else
   		if(TestStepName() == Kregcomp_basic)
   			{
   	   		INFO_PRINTF1(_L("regcomp_basic():"));
   	   		err = regcomp_basic();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	else
   		if(TestStepName() == Kregcomp_complex)
   			{
   	   		INFO_PRINTF1(_L("regcomp_complex():"));
   	   		err = regcomp_complex();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	else
   	   	if(TestStepName() == Kregcomp_eparen)
   			{
   	   		INFO_PRINTF1(_L("regcomp_eparen():"));
   	   		err = regcomp_eparen();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	else
   	   	if(TestStepName() == Kregcomp_ebrack)
   			{
   	   		INFO_PRINTF1(_L("regcomp_ebrack():"));
   	   		err = regcomp_ebrack();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
   	   	if(TestStepName() == Kregexec_basic)
   			{
   	   		INFO_PRINTF1(_L("regexec_basic():"));
   	   		err = regexec_basic();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
   	   	if(TestStepName() == Kregexec_icase)
   			{
   	   		INFO_PRINTF1(_L("regexec_icase():"));
   	   		err = regexec_icase();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
   	   	if(TestStepName() == Kregexec_notbol)
   			{
   	   		INFO_PRINTF1(_L("regexec_notbol():"));
   	   		err = regexec_notbol();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
   	   	if(TestStepName() == Kregexec_noteol)
   			{
   	   		INFO_PRINTF1(_L("regexec_noteol():"));
   	   		err = regexec_noteol();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
   	   	if(TestStepName() == Kregexec_range)
   			{
   	   		INFO_PRINTF1(_L("regexec_range():"));
   	   		err = regexec_range();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
   		if(TestStepName() == Kregfree_basic)
   			{
   	   		INFO_PRINTF1(_L("regfree_basic():"));
   	   		err = regfree_basic();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == Kregfree_multiple)
   			{
   	   		INFO_PRINTF1(_L("regfree_multiple():"));
   	   		err = regfree_multiple();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == Kregerror_basic)
   			{
   	   		INFO_PRINTF1(_L("regerror_basic():"));
   	   		err = regerror_basic();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	else
		if(TestStepName() == Kregcomp1)
   			{
   	   		INFO_PRINTF1(_L("regcomp1():"));
   	   		err = regcomp1();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	else
		if(TestStepName() == Kregcomp2)
   			{
   	   		INFO_PRINTF1(_L("regcomp2():"));
   	   		err = regcomp2();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	else
		if(TestStepName() == Kregcomp3)
   			{
   	   		INFO_PRINTF1(_L("regcomp3():"));
   	   		err = regcomp3();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   			}
   	else
   		if(TestStepName() == Kregcomp_nospec)
   			{
   	   		INFO_PRINTF1(_L("regcomp_nospec():"));
   	   		err = regcomp_nospec();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == Kregcomp_icase2)
   			{
   	   		INFO_PRINTF1(_L("regcomp_icase2():"));
   	   		err = regcomp_icase2();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == Kregcomp_newline)
   			{
   	   		INFO_PRINTF1(_L("regcomp_newline():"));
   	   		err = regcomp_newline();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	else
		if(TestStepName() == Kregfree_multiple1)
   			{
   	   		INFO_PRINTF1(_L("regfree_multiple1():"));
   	   		err = regfree_multiple1();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == Kregerror_basic1)
   			{
   	   		INFO_PRINTF1(_L("regerror_basic():"));
   	   		err = regerror_basic1();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	return TestStepResult(); 

	}
 


