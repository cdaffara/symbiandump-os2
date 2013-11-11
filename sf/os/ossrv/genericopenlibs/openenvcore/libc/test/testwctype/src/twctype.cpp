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



#include "twctype.h"
#include <unistd.h> 
#include <errno.h>
#include <stdio.h>
#include <e32std.h>
#include <stdlib.h>
#include <string.h>


CTestWctype::~CTestWctype() 
	{ 
	}  

CTestWctype::CTestWctype(const TDesC& aStepName)
	{
	// MANDATORY Call to base class method to set up the human readable name for logging.
	SetTestStepName(aStepName);		
	}

TVerdict CTestWctype::doTestStepPreambleL()
	{
	__UHEAP_MARK;
	iParamCnt = 0;
	SetTestStepResult(EPass);
	return TestStepResult();
	}



TVerdict CTestWctype::doTestStepPostambleL()
	{
	__UHEAP_MARKEND;	
	return TestStepResult();
	}


TVerdict CTestWctype::doTestStepL()
	{
	int err;
   	if(TestStepName() == Ktest_iswalpha)
   		{
   		INFO_PRINTF1(_L("test_iswalpha():"));
   	   	err = test_iswalpha();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == Ktest_iswalnum)
	   	{
	   	INFO_PRINTF1(_L("test_iswalnum():"));
	   	err = test_iswalnum();
	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
	   	}
	else if(TestStepName() == Ktest_iswdigit)
	   	{
	   	INFO_PRINTF1(_L("test_iswdigit():"));
	   	err = test_iswdigit();
	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
	   	}
	else if(TestStepName() == Ktest_iswcntrl)
	   	{
	   	INFO_PRINTF1(_L("test_iswcntrl():"));
	   	err = test_iswcntrl();
	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
	   	}
	else if(TestStepName() == Ktest_iswxdigit)
	   	{
	   	INFO_PRINTF1(_L("test_iswxdigit():"));
	   	err = test_iswxdigit();
	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
	   	}
	else if(TestStepName() == Ktest_iswspace)
	   	{
	   	INFO_PRINTF1(_L("test_iswspace():"));
	   	err = test_iswspace();
	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
	   	}
	else if(TestStepName() == Ktest_iswblank)
	   	{
	   	INFO_PRINTF1(_L("test_iswblank():"));
	   	err = test_iswblank();
	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
	   	}
	else if(TestStepName() == Ktest_iswpunct)
		{
	   	INFO_PRINTF1(_L("test_iswpunct():"));
	   	err = test_iswpunct();
	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
	   	}
	else if(TestStepName() == Ktest_iswlower)
	   	{
	   	INFO_PRINTF1(_L("test_iswlower():"));
	   	err = test_iswlower();
	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
	   	}
	else if(TestStepName() == Ktest_iswupper)
	   	{
	   	INFO_PRINTF1(_L("test_iswupper():"));
	   	err = test_iswupper();
	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
	   	}
	else if(TestStepName() == Ktest_iswprint)
		{
	   	INFO_PRINTF1(_L("test_iswprint():"));
	   	err = test_iswprint();
	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
	   	}
	else if(TestStepName() == Ktest_iswgraph)
		{
	   	INFO_PRINTF1(_L("test_iswgraph():"));
	   	err = test_iswgraph();
	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
	   	}
	else if(TestStepName() == Ktest_iswctype)
	   	{
	   	INFO_PRINTF1(_L("test_iswctype():"));
	   	err = test_iswctype();
	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
		}
	return TestStepResult(); 
	}
 
