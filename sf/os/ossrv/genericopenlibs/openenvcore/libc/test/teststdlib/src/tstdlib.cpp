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



#include "tstdlib.h"
#include <unistd.h> 
#include <errno.h>
#include <stdio.h>
#include <e32std.h>
#include <stdlib.h>
#include <string.h>


CTestStdlib::~CTestStdlib() 
	{ 
	}  

CTestStdlib::CTestStdlib(const TDesC& aStepName)
	{
	// MANDATORY Call to base class method to set up the human readable name for logging.
	SetTestStepName(aStepName);		
	}

TVerdict CTestStdlib::doTestStepPreambleL()
	{
	__UHEAP_MARK;		
	SetTestStepResult(EPass);
	return TestStepResult();
	}



TVerdict CTestStdlib::doTestStepPostambleL()
	{
	__UHEAP_MARKEND;		
	return TestStepResult();
	}



TVerdict CTestStdlib::doTestStepL()
	{
	int err;

   	if(TestStepName() == Kmalloc_Test0)
   		{
   		INFO_PRINTF1(_L("malloc_Test0():"));
   		err = malloc_Test0();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else
   		if(TestStepName() == Kmalloc_Test1)
   			{
   	   		INFO_PRINTF1(_L("malloc_Test1():"));
   	   		err = malloc_Test1();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
   	   	if(TestStepName() == Kmalloc_Test2)
   			{
   	   		INFO_PRINTF1(_L("malloc_Test2():"));
   	   		err = malloc_Test2();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	if(TestStepName() == Kcalloc_Test0)
		{
		INFO_PRINTF1(_L("calloc_Test0():"));
		err = calloc_Test0();
		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
		}
	else
   	   	if(TestStepName() == Kcalloc_Test1)
   			{
   	   		INFO_PRINTF1(_L("calloc_Test1():"));
   	   		err = calloc_Test1();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == Kcalloc_Test2)
   			{
   	   		INFO_PRINTF1(_L("calloc_Test2():"));
   	   		err = calloc_Test2();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
   	   	if(TestStepName() == Krealloc_Test0)
   			{
   	   		INFO_PRINTF1(_L("realloc_Test0():"));
   	   		err = realloc_Test0();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == Krealloc_Test1)
   			{
   	   		INFO_PRINTF1(_L("realloc_Test1():"));
   	   		err = realloc_Test1();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
   	   	if(TestStepName() == Krealloc_Test2)
   			{
   	   		INFO_PRINTF1(_L("realloc_Test2():"));
   	   		err = realloc_Test2();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
   	   	if(TestStepName() == Krealloc_Test3)
   			{
   	   		INFO_PRINTF1(_L("realloc_Test3():"));
   	   		err = realloc_Test3();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
   	   	if(TestStepName() == Krealloc_Test4)
   			{
   	   		INFO_PRINTF1(_L("realloc_Test4():"));
   	   		err = realloc_Test4();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == Kfree_Test0)
   			{
   	   		INFO_PRINTF1(_L("free_Test0():"));
   	   		err = free_Test0();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   		
	else
		if(TestStepName() == Kgetenv_Test0)
   			{
   	   		INFO_PRINTF1(_L("getenv_Test0():"));
   	   		err = getenv_Test0();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == Kgetenv_Test1)
   			{
   	   		INFO_PRINTF1(_L("getenv_Test1():"));
   	   		err = getenv_Test1();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == Ksetenv_Test0)
   			{
   	   		INFO_PRINTF1(_L("setenv_Test0():"));
   	   		err = setenv_Test0();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == Ksetenv_Test1)
   			{
   	   		INFO_PRINTF1(_L("setenv_Test1():"));
   	   		err = setenv_Test1();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == Ksetenv_Test2)
   			{
   	   		INFO_PRINTF1(_L("setenv_Test2():"));
   	   		err = setenv_Test2();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == Ksetenv_Test3)
   			{
   	   		INFO_PRINTF1(_L("setenv_Test3():"));
   	   		err = setenv_Test3();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == Ksetenv_Test4)
   			{
   	   		INFO_PRINTF1(_L("setenv_Test4():"));
   	   		err = setenv_Test4();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == Kputenv_Test0)
   			{
   	   		INFO_PRINTF1(_L("putenv_Test0():"));
   	   		err = putenv_Test0();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == Kputenv_Test1)
   			{
   	   		INFO_PRINTF1(_L("putenv_Test1():"));
   	   		err = putenv_Test1();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == Kputenv_Test2)
   			{
   	   		INFO_PRINTF1(_L("putenv_Test2():"));
   	   		err = putenv_Test2();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == Kputenv_Test3)
   			{
   	   		INFO_PRINTF1(_L("putenv_Test3():"));
   	   		err = putenv_Test3();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == Kunsetenv_Test0)
   			{
   	   		INFO_PRINTF1(_L("unsetenv_Test0():"));
   	   		err = unsetenv_Test0();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == Ksystem_Test0)
   			{
   	   		INFO_PRINTF1(_L("system_Test0():"));
   	   		err = system_Test0();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == Ksystem_Test1)
   			{
   	   		INFO_PRINTF1(_L("system_Test1():"));
   	   		err = system_Test1();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == Ksystem_Test2)
   			{
   	   		INFO_PRINTF1(_L("system_Test2():"));
   	   		err = system_Test2();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == Ksystem_Test3)
   			{
   	   		INFO_PRINTF1(_L("system_Test3():"));
   	   		err = system_Test3();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == Ksystem_Test4)
   			{
   	   		INFO_PRINTF1(_L("system_Test4():"));
   	   		err = system_Test4();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == Ksystem_Test5)
   			{
   	   		INFO_PRINTF1(_L("system_Test5():"));
   	   		err = system_Test5();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == Ksystem_Test6)
   			{
   	   		INFO_PRINTF1(_L("system_Test6():"));
   	   		err = system_Test6();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == Kabort_Test0)
   			{
   	   		INFO_PRINTF1(_L("abort_Test0():"));
   	   		err = abort_Test0();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}	
	else
		if(TestStepName() == Kfopen_Test)
			{
	   		INFO_PRINTF1(_L("fopen_Test():"));
	   		err = fopen_Test();
	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
	   		}	
	else
		if(TestStepName() == Kexit_Test0)
   			{
   	   		INFO_PRINTF1(_L("exit_Test0():"));
   	   		err = exit_Test0();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	else
   	   	 if(TestStepName() == Kexit_Test1)
   			{
   	   		INFO_PRINTF1(_L("exit_Test1():"));
   	   		err = exit_Test1();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == K_exitE_Test0)
   			{
   	   		INFO_PRINTF1(_L("_exitE_Test0():"));
   	   		err = _exitE_Test0();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == K_exit_Test0)
   			{
   	   		INFO_PRINTF1(_L("_exit_Test0():"));
   	   		err = _exit_Test0();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == Katexit_Test0)
   			{
   	   		INFO_PRINTF1(_L("atexit_Test0():"));
   	   		err = atexit_Test0();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == Katexit_Test1)
   			{
   	   		INFO_PRINTF1(_L("atexit_Test1():"));
   	   		err = atexit_Test1();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	else
		if(TestStepName() == Kabs_good_param)
   			{
   	   		INFO_PRINTF1(_L("abs_good_param():"));
   	   		err = abs_good_param();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == Kabs_pos_max)
   			{
   	   		INFO_PRINTF1(_L("abs_pos_max():"));
   	   		err = abs_pos_max();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == Kabs_neg_max)
   			{
   	   		INFO_PRINTF1(_L("abs_neg_max():"));
   	   		err = abs_neg_max();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == Klabs_good_param)
   			{
   	   		INFO_PRINTF1(_L("labs_good_param():"));
   	   		err = labs_good_param();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == Klabs_pos_max)
   			{
   	   		INFO_PRINTF1(_L("labs_pos_max():"));
   	   		err = labs_pos_max();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == Klabs_neg_max)
   			{
   	   		INFO_PRINTF1(_L("labs_neg_max():"));
   	   		err = labs_neg_max();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == Kllabs_good_param)
   			{
   	   		INFO_PRINTF1(_L("llabs_good_param():"));
   	   		err = llabs_good_param();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == Kllabs_pos_max)
   			{
   	   		INFO_PRINTF1(_L("llabs_pos_max():"));
   	   		err = llabs_pos_max();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == Kllabs_neg_max)
   			{
   	   		INFO_PRINTF1(_L("llabs_neg_max():"));
   	   		err = llabs_neg_max();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == Katoi_null_string)
   			{
   	   		INFO_PRINTF1(_L("atoi_null_string():"));
   	   		err = atoi_null_string();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == Katoi_good_param)
   			{
   	   		INFO_PRINTF1(_L("atoi_good_param():"));
   	   		err = atoi_good_param();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == Katof_null_string)
   			{
   	   		INFO_PRINTF1(_L("atof_null_string():"));
   	   		err = atof_null_string();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == Katof_pos_floatnum)
   			{
   	   		INFO_PRINTF1(_L("atof_pos_floatnum():"));
   	   		err = atof_pos_floatnum();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
		if(TestStepName() == Katof_neg_floatnum)
   			{
   	   		INFO_PRINTF1(_L("atof_neg_floatnum():"));
   	   		err = atof_neg_floatnum();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
   		if(TestStepName() == Katof_alpha)
   			{
   	   		INFO_PRINTF1(_L("atof_alpha():"));
   	   		err = atof_alpha();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
   	   	if(TestStepName() == Katof_alpha_floatnum)
   			{
   	   		INFO_PRINTF1(_L("atof_alpha_floatnum():"));
   	   		err = atof_alpha_floatnum();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
   	   	if(TestStepName() == Katof_pos_floatnum_alpha)
   			{
   	   		INFO_PRINTF1(_L("atof_pos_floatnum_alpha():"));
   	   		err = atof_pos_floatnum_alpha();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
   	   	if(TestStepName() == Katof_neg_floatnum_alpha)
   			{
   	   		INFO_PRINTF1(_L("atof_neg_floatnum_alpha():"));
   	   		err = atof_neg_floatnum_alpha();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
   	   	if(TestStepName() == Katof_leading_zero)
   			{
   	   		INFO_PRINTF1(_L("atof_leading_zero():"));
   	   		err = atof_leading_zero();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
   	   	if(TestStepName() == Katof_neg_leading_zero)
   			{
   	   		INFO_PRINTF1(_L("atof_neg_leading_zero():"));
   	   		err = atof_neg_leading_zero();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
   	   	if(TestStepName() == Katof_pos_floatnum_pos_expo)
   			{
   	   		INFO_PRINTF1(_L("atof_pos_floatnum_pos_expo():"));
   	   		err = atof_pos_floatnum_pos_expo();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
   	   	if(TestStepName() == Katof_neg_floatnum_pos_expo)
   			{
   	   		INFO_PRINTF1(_L("atof_neg_floatnum_pos_expo():"));
   	   		err = atof_neg_floatnum_pos_expo();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
   	   	if(TestStepName() == Katof_pos_floatnum_neg_expo)
   			{
   	   		INFO_PRINTF1(_L("atof_pos_floatnum_neg_expo():"));
   	   		err = atof_pos_floatnum_neg_expo();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
   	   	if(TestStepName() == Katof_neg_floatnum_neg_expo)
   			{
   	   		INFO_PRINTF1(_L("atof_neg_floatnum_neg_expo():"));
   	   		err = atof_neg_floatnum_neg_expo();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	else
   	   	if(TestStepName() == Katol_null_string)
   			{
   	   		INFO_PRINTF1(_L("atol_null_string():"));
   	   		err = atol_null_string();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
   	   	if(TestStepName() == Katol_good_param)
   			{
   	   		INFO_PRINTF1(_L("atol_good_param():"));
   	   		err = atol_good_param();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
   	   	if(TestStepName() == Katollbasic)
   			{
   	   		INFO_PRINTF1(_L("atollbasic():"));
   	   		err = atollbasic();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
   	   	if(TestStepName() == Katollspacecheck)
   			{
   	   		INFO_PRINTF1(_L("atollspacecheck():"));
   	   		err = atollspacecheck();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
   	   	if(TestStepName() == Katollsignedsrc)
   			{
   	   		INFO_PRINTF1(_L("atollsignedsrc():"));
   	   		err = atollsignedsrc();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
   	   	if(TestStepName() == Katolloctalcheck)
   			{
   	   		INFO_PRINTF1(_L("atolloctalcheck():"));
   	   		err = atolloctalcheck();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
   	   	if(TestStepName() == Katollhexcheck)
   			{
   	   		INFO_PRINTF1(_L("atollhexcheck():"));
   	   		err = atollhexcheck();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
   	   	if(TestStepName() == Katolldigitfirstcheck)
   			{
   	   		INFO_PRINTF1(_L("atolldigitfirstcheck():"));
   	   		err = atolldigitfirstcheck();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
   	   	if(TestStepName() == Katollalphafirstcheck)
   			{
   	   		INFO_PRINTF1(_L("atollalphafirstcheck():"));
   	   		err = atollalphafirstcheck();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
   	   	if(TestStepName() == Katollalphacheck)
   			{
   	   		INFO_PRINTF1(_L("atollalphacheck():"));
   	   		err = atollalphacheck();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
   	   	if(TestStepName() == Katollleadingzeroscheck)
   			{
   	   		INFO_PRINTF1(_L("atollleadingzeroscheck():"));
   	   		err = atollleadingzeroscheck();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
   	   	if(TestStepName() == Kdiv_good_param)
   			{
   	   		INFO_PRINTF1(_L("div_good_param():"));
   	   		err = div_good_param();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
   	   	if(TestStepName() == Kldiv_good_param)
   			{
   	   		INFO_PRINTF1(_L("ldiv_good_param():"));
   	   		err = ldiv_good_param();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
   	   	if(TestStepName() == Klldiv_good_param0)
   			{
   	   		INFO_PRINTF1(_L("lldiv_good_param0():"));
   	   		err = lldiv_good_param0();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
   	   	if(TestStepName() == Klldiv_good_param1)
   			{
   	   		INFO_PRINTF1(_L("lldiv_good_param1():"));
   	   		err = lldiv_good_param1();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
   	   	if(TestStepName() == Klldiv_good_param2)
   			{
   	   		INFO_PRINTF1(_L("lldiv_good_param2():"));
   	   		err = lldiv_good_param2();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
   	   	if(TestStepName() == Klldiv_good_param3)
   			{
   	   		INFO_PRINTF1(_L("lldiv_good_param3():"));
   	   		err = lldiv_good_param3();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
   	   	if(TestStepName() == Klldiv_good_param4)
   			{
   	   		INFO_PRINTF1(_L("lldiv_good_param4():"));
   	   		err = lldiv_good_param4();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
   	   	if(TestStepName() == Krand)
   			{
   	   		INFO_PRINTF1(_L("rand():"));
   	   		err = randL();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
   	   	if(TestStepName() == Ksrand)
   			{
   	   		INFO_PRINTF1(_L("srand():"));
   	   		err = srandL();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
   	   	if(TestStepName() == Kstrtol_good_param)
   			{
   	   		INFO_PRINTF1(_L("strtol_good_param():"));
   	   		err = strtol_good_param();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
   	   	if(TestStepName() == Kstrtol_null_string)
   			{
   	   		INFO_PRINTF1(_L("strtol_null_string():"));
   	   		err = strtol_null_string();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
   	   	if(TestStepName() == Kstrtod_null_string)
   			{
   	   		INFO_PRINTF1(_L("strtod_null_string():"));
   	   		err = strtod_null_string();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
   	   	if(TestStepName() == Kstrtod_good_param)
   			{
   	   		INFO_PRINTF1(_L("strtod_good_param():"));
   	   		err = strtod_good_param();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
   	   	if(TestStepName() == Kstrtod_nan)
   			{
   	   		INFO_PRINTF1(_L("strtod_nan():"));
   	   		err = strtod_nan();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
   	   	if(TestStepName() == Kstrtod_inf)
   			{
   	   		INFO_PRINTF1(_L("strtod_inf():"));
   	   		err = strtod_inf();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
   	   	if(TestStepName() == Kstrtod_neg_cases)
   			{
   	   		INFO_PRINTF1(_L("strtod_neg_cases():"));
   	   		err = strtod_neg_cases();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
   	   	if(TestStepName() == Kstrtod_misc_cases)
   			{
   	   		INFO_PRINTF1(_L("strtod_misc_cases():"));
   	   		err = strtod_misc_cases();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
   	   	if(TestStepName() == Kstrtoul_null_string)
   			{
   	   		INFO_PRINTF1(_L("strtoul_null_string():"));
   	   		err = strtoul_null_string();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
   	   	if(TestStepName() == Kstrtoul_good_param)
   			{
   	   		INFO_PRINTF1(_L("strtoul_good_param():"));
   	   		err = strtoul_good_param();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
   	   	if(TestStepName() == Kstrtoul_neg_leading_zeroes)
   			{
   	   		INFO_PRINTF1(_L("strtoul_neg_leading_zeroes():"));
   	   		err = strtoul_neg_leading_zeroes();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
   	   	if(TestStepName() == Kstrtoul_neg_num_alpha)
   			{
   	   		INFO_PRINTF1(_L("strtoul_neg_num_alpha():"));
   	   		err = strtoul_neg_num_alpha();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
   	   	if(TestStepName() == Kqsort_integers)
   			{
   	   		INFO_PRINTF1(_L("qsort_integers():"));
   	   		err = qsort_integers();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
   	   	if(TestStepName() == Kqsort_strings)
   			{
   	   		INFO_PRINTF1(_L("qsort_strings():"));
   	   		err = qsort_strings();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
   	   	if(TestStepName() == Kbinsearch_integers)
   			{
   	   		INFO_PRINTF1(_L("binsearch_integers():"));
   	   		err = binsearch_integers();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
   	   	if(TestStepName() == Kbinsearch_strings)
   			{
   	   		INFO_PRINTF1(_L("binsearch_strings():"));
   	   		err = binsearch_strings();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
   	   	if(TestStepName() == Kisatty_Test0)
   			{
   	   		INFO_PRINTF1(_L("isatty_Test0():"));
   	   		err = isatty_Test0();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
   	   	if(TestStepName() == Kisatty_Test1)
   			{
   	   		INFO_PRINTF1(_L("isatty_Test1():"));
   	   		err = isatty_Test1();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
   	   	if(TestStepName() == Kusleep_Test0)
   			{
   	   		INFO_PRINTF1(_L("usleep_Test0():"));
   	   		err = usleep_Test0();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
   	   	if(TestStepName() == Kusleep_Test1)
   			{
   	   		INFO_PRINTF1(_L("usleep_Test1():"));
   	   		err = usleep_Test1();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
   	   	if(TestStepName() == Kusleep_Test2)
   			{
   	   		INFO_PRINTF1(_L("usleep_Test2():"));
   	   		err = usleep_Test2();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
   	   	if(TestStepName() == Kgetcwd_Test0)
   			{
   	   		INFO_PRINTF1(_L("getcwd_Test0():"));
   	   		err = getcwd_Test0();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
   	   	if(TestStepName() == Kgetcwd_Test1)
   			{
   	   		INFO_PRINTF1(_L("getcwd_Test1():"));
   	   		err = getcwd_Test1();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
   	   	if(TestStepName() == Kgetcwd_Test2)
   			{
   	   		INFO_PRINTF1(_L("getcwd_Test2():"));
   	   		err = getcwd_Test2();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
   	   	if(TestStepName() == Kgetcwd_Test3)
   			{
   	   		INFO_PRINTF1(_L("getcwd_Test3():"));
   	   		err = getcwd_Test3();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
   	   	if(TestStepName() == Ktmpfile_Test0)
   			{
   	   		INFO_PRINTF1(_L("tmpfile_Test0():"));
   	   		err = tmpfile_Test0();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
   	   	if(TestStepName() == Ktmpfile_Test1)
   			{
   	   		INFO_PRINTF1(_L("tmpfile_Test1():"));
   	   		err = tmpfile_Test1();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
   	   	if(TestStepName() == Ktmpnam_Test0)
   			{
   	   		INFO_PRINTF1(_L("tmpnam_Test0():"));
   	   		err = tmpnam_Test0();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
   	   	if(TestStepName() == Ktmpnam_Test1)
   			{
   	   		INFO_PRINTF1(_L("tmpnam_Test1():"));
   	   		err = tmpnam_Test1();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
   	   	if(TestStepName() == Krealpath_Test0)
   			{
   	   		INFO_PRINTF1(_L("realpath_Test0():"));
   	   		err = realpath_Test0();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
   	   	if(TestStepName() == Krealpath_Test1)
   			{
   	   		INFO_PRINTF1(_L("realpath_Test1():"));
   	   		err = realpath_Test1();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
   	   	if(TestStepName() == Krealpath_Test2)
   			{
   	   		INFO_PRINTF1(_L("realpath_Test2():"));
   	   		err = realpath_Test2();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
   	   	if(TestStepName() == Krealpath_Test3)
   			{
   	   		INFO_PRINTF1(_L("realpath_Test3():"));
   	   		err = realpath_Test3();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
   	   	if(TestStepName() == Krealpath_Test4)
   			{
   	   		INFO_PRINTF1(_L("realpath_Test4():"));
   	   		err = realpath_Test4();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
   	   	if(TestStepName() == Kperror_Test0)
   			{
   	   		INFO_PRINTF1(_L("perror_Test0():"));
   	   		err = perror_Test0();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
   	   	if(TestStepName() == Kmkstemp_Test0)
   			{
   	   		INFO_PRINTF1(_L("mkstemp_Test0():"));
   	   		err = mkstemp_Test0();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
   	   	if(TestStepName() == Kmkstemp_Test1)
   			{
   	   		INFO_PRINTF1(_L("mkstemp_Test1():"));
   	   		err = mkstemp_Test1();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
   	   	if(TestStepName() == Kconfstr_Test0)
   			{
   	   		INFO_PRINTF1(_L("confstr_Test0():"));
   	   		err = confstr_Test0();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
   	   	if(TestStepName() == Kconfstr_Test1)
   			{
   	   		INFO_PRINTF1(_L("confstr_Test1():"));
   	   		err = confstr_Test1();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
   	   	if(TestStepName() == Kfpathconf_Test0)
   			{
   	   		INFO_PRINTF1(_L("fpathconf_Test0():"));
   	   		err = fpathconf_Test0();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
   	   	if(TestStepName() == Kfpathconf_Test1)
   			{
   	   		INFO_PRINTF1(_L("fpathconf_Test1():"));
   	   		err = fpathconf_Test1();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
   	   	if(TestStepName() == Kfilelock_Test0)
   			{
   	   		INFO_PRINTF1(_L("filelock_Test0():"));
   	   		err = filelock_Test0();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
   	   	if(TestStepName() == Kfilelock_Test1)
   			{
   	   		INFO_PRINTF1(_L("filelock_Test1():"));
   	   		err = filelock_Test1();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
   	   	if(TestStepName() == Kfilelock_Test2)
   			{
   	   		INFO_PRINTF1(_L("filelock_Test2():"));
   	   		err = filelock_Test2();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
   	   	if(TestStepName() == Kfilelock_Test3)
   			{
   	   		INFO_PRINTF1(_L("filelock_Test3():"));
   	   		err = filelock_Test3();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
   	   	if(TestStepName() == KgetoptTest1)
   			{
   	   		INFO_PRINTF1(_L("getoptTest1():"));
   	   		err = getoptTest1();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
   	   	if(TestStepName() == KgetoptTest2)
   			{
   	   		INFO_PRINTF1(_L("getoptTest2():"));
   	   		err = getoptTest2();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
   	   	if(TestStepName() == KgetoptTest3)
   			{
   	   		INFO_PRINTF1(_L("getoptTest3():"));
   	   		err = getoptTest3();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
   	   	if(TestStepName() == KgetoptTest4)
   			{
   	   		INFO_PRINTF1(_L("getoptTest4():"));
   	   		err = getoptTest4();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
   	   	if(TestStepName() == KgetoptTest5)
   			{
   	   		INFO_PRINTF1(_L("getoptTest5():"));
   	   		err = getoptTest5();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
   	   	if(TestStepName() == KgetoptTest6)
   			{
   	   		INFO_PRINTF1(_L("getoptTest6():"));
   	   		err = getoptTest6();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
   	   	if(TestStepName() == KgetoptTest_long1)
   			{
   	   		INFO_PRINTF1(_L("getoptTest_long1():"));
   	   		err = getoptTest_long1();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
   	   	if(TestStepName() == KgetoptTest_long2)
   			{
   	   		INFO_PRINTF1(_L("getoptTest_long2():"));
   	   		err = getoptTest_long2();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
   	   	if(TestStepName() == KgetoptTest_long3)
   			{
   	   		INFO_PRINTF1(_L("getoptTest_long3():"));
   	   		err = getoptTest_long3();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KgetoptTest_long4)
   			{
   	   		INFO_PRINTF1(_L("getoptTest_long4():"));
   	   		err = getoptTest_long4();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	
   	else if(TestStepName() == KTestlseek)
   			{
   	   		INFO_PRINTF1(_L("TestlSeek():"));
   	   		err = Testlseek();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		} 
   	else if(TestStepName() == KTestsigemptyset)
   			{
   	   		INFO_PRINTF1(_L("Testsigemptyset():"));
   	   		err = Testsigemptyset();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}	
	else if(TestStepName() == Kstrfmon1)
   			{
   	   		INFO_PRINTF1(_L("strfmon1():"));
   	   		err = strfmon1();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		} 	
   	else if(TestStepName() == KTestRealPath5)
	   		{
	   	   	INFO_PRINTF1(_L("TestRealPath5:"));
	   	   	err = TestRealPath5();
	   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
	   	   	}
	else if(TestStepName() == KTestlseek1)
   			{
   	   		INFO_PRINTF1(_L("TestlSeek1():"));
   	   		err = Testlseek1();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		} 
   	else if(TestStepName() == KTestlseek2)
   			{
   	   		INFO_PRINTF1(_L("TestlSeek2():"));
   	   		err = Testlseek2();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == KTestlseek3)
   			{
   	   		INFO_PRINTF1(_L("TestlSeek3():"));
   	   		err = Testlseek3();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	else if(TestStepName() == KgetoptTest_long5)
   			{
   	   		INFO_PRINTF1(_L("getoptTest_long5():"));
   	   		err = getoptTest_long5();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}   
   	else if(TestStepName() == KsetjmpTest)
   			{
   	   		INFO_PRINTF1(_L("setjmpTest:"));
   	   		err = setjmpTest();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}	
   	else if(TestStepName() == KabortTest)
   			{
   	   		INFO_PRINTF1(_L("abortTest:"));
   	   		err = abortTest();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		} 
   	else if(TestStepName() == KchownTest)
   			{
   	   		INFO_PRINTF1(_L("chownTest:"));
   	   		err = chownTest();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	 else if(TestStepName() == KiconvTest)
   			{
   	   		INFO_PRINTF1(_L("iconvTest:"));
   	   		err = iconvTest();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	 else if(TestStepName() == KsetgrentTest)
   			{
   	   		INFO_PRINTF1(_L("setgrentTest:"));
   	   		err = setgrentTest();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	 else if(TestStepName() == KwcreatTest)
   			{
   	   		INFO_PRINTF1(_L("wcreatTest:"));
   	   		err = wcreatTest();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}	
   	 else if(TestStepName() == Katof_locale)
   			{
   	   		INFO_PRINTF1(_L("atof_locale:"));
   	   		err = atof_locale();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	 else if(TestStepName() == Kstrtod_locale)
   			{
   	   		INFO_PRINTF1(_L("strtod_locale:"));
   	   		err = strtod_locale();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}	
   	else
   	   	if(TestStepName() == Kbinsearch_integers1)
   			{
   	   		INFO_PRINTF1(_L("binsearch_integers1():"));
   	   		err = binsearch_integers1();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}	
   	else
		if(TestStepName() == Kcalloc_Test3)
   			{
   	   		INFO_PRINTF1(_L("calloc_Test3():"));
   	   		err = calloc_Test3();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	else
		if(TestStepName() == Kcalloc_Test4)
   			{
   	   		INFO_PRINTF1(_L("calloc_Test4():"));
   	   		err = calloc_Test4();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	else
		if(TestStepName() == Kgetenv_Test2)
   			{
   	   		INFO_PRINTF1(_L("getenv_Test2():"));
   	   		err = getenv_Test2();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	else
   	   	if(TestStepName() == Kreallocf_Test0)
   			{
   	   		INFO_PRINTF1(_L("reallocf_Test0():"));
   	   		err = reallocf_Test0();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	else
		if(TestStepName() == Katexit_Test2)
   			{
   	   		INFO_PRINTF1(_L("atexit_Test2():"));
   	   		err = atexit_Test2();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	else
   	   	if(TestStepName() == Kstrtol_space_string)
   			{
   	   		INFO_PRINTF1(_L("strtol_space_string():"));
   	   		err = strtol_space_string();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	else
   	   	if(TestStepName() == Kstrtol_extreme_string)
   			{
   	   		INFO_PRINTF1(_L("strtol_extreme_string():"));
   	   		err = strtol_extreme_string();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	else
   	   	if(TestStepName() == Kstrtol_extreme_string1)
   			{
   	   		INFO_PRINTF1(_L("strtol_extreme_string1():"));
   	   		err = strtol_extreme_string1();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	else
   	   	if(TestStepName() == Kstrtoll_extreme_string)
   			{
   	   		INFO_PRINTF1(_L("strtoll_extreme_string():"));
   	   		err = strtoll_extreme_string();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	else
   	   	if(TestStepName() == Kstrtoll_extreme_string1)
   			{
   	   		INFO_PRINTF1(_L("strtoll_extreme_string1():"));
   	   		err = strtoll_extreme_string1();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	else
   	   	if(TestStepName() == Kstrtoll_good_param)
   			{
   	   		INFO_PRINTF1(_L("strtoll_good_param():"));
   	   		err = strtoll_good_param();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	else
   	   	if(TestStepName() == Kstrtoul_extreme_string)
   			{
   	   		INFO_PRINTF1(_L("strtoul_extreme_string():"));
   	   		err = strtoul_extreme_string();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	else
   	   	if(TestStepName() == Kstrtoul_extreme_string1)
   			{
   	   		INFO_PRINTF1(_L("strtoul_extreme_string1():"));
   	   		err = strtoul_extreme_string1();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	else
   	   	if(TestStepName() == Kqsort_integers1)
   			{
   	   		INFO_PRINTF1(_L("qsort_integers1():"));
   	   		err = qsort_integers1();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	else

   	   	if(TestStepName() == Kqsort_integers2)
   			{
   	   		INFO_PRINTF1(_L("qsort_integers2():"));
   	   		err = qsort_integers2();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	else
   	   	if(TestStepName() == Kqsort_integers3)
   			{
   	   		INFO_PRINTF1(_L("qsort_integers3():"));
   	   		err = qsort_integers3();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	else if(TestStepName() == Kstrtoullbasic)
		{
   		INFO_PRINTF1(_L("strtoullbasic():"));
   		err = strtoullbasic();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
	else if(TestStepName() == Kstrtoullneg)
		{
   		INFO_PRINTF1(_L("strtoullneg():"));
   		err = strtoullneg();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}  
   	else if(TestStepName() == Kstrtoullpos)
		{
   		INFO_PRINTF1(_L("strtoullpos():"));
   		err = strtoullpos();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}   
   	else if(TestStepName() == Kstrtoullbase)
		{
   		INFO_PRINTF1(_L("strtoullbase():"));
   		err = strtoullbase();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Kstrtoulpos)
		{
   		INFO_PRINTF1(_L("strtoulpos():"));
   		err = strtoulpos();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		} 
   	else if(TestStepName() == Kstrtoimaxbasic)
		{
   		INFO_PRINTF1(_L("strtoimaxbasic():"));
   		err = strtoimaxbasic();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Kstrtoimaxbasic1)
		{
   		INFO_PRINTF1(_L("strtoimaxbasic1():"));
   		err = strtoimaxbasic1();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Kstrtoimaxbasic2)
		{
   		INFO_PRINTF1(_L("strtoimaxbasic2():"));
   		err = strtoimaxbasic2();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Kstrtoumaxbasic)
		{
   		INFO_PRINTF1(_L("strtoumaxbasic():"));
   		err = strtoumaxbasic();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Kstrtoumaxbasic1)
		{
   		INFO_PRINTF1(_L("strtoumaxbasic1():"));
   		err = strtoumaxbasic1();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Kstrtoumaxbasic2)
		{
   		INFO_PRINTF1(_L("strtoumaxbasic2():"));
   		err = strtoumaxbasic2();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Kconfstr_Test2)
   			{
   	   		INFO_PRINTF1(_L("confstr_Test2():"));
   	   		err = confstr_Test2();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	else if(TestStepName() == Ktmpfile_fseek)
   			{
   	   		INFO_PRINTF1(_L("tmpfile_fseek():"));
   	   		err = tmpfile_fseek();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	
    else if(TestStepName() == KtestSymLink)
             {
             INFO_PRINTF1(_L("testSymLink():"));
             err = testSymLink();
             SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
             }
   	return TestStepResult(); 
	}

 

