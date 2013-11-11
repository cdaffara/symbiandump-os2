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
// Name        : tdevnull.cpp
//




#include "tdevnull.h"
#include <stdio.h>
#include <string.h>
#include <wchar.h>





CTestDevNull::~CTestDevNull() 
	{ 
	}  

CTestDevNull::CTestDevNull(const TDesC& aStepName)
	{
	// MANDATORY Call to base class method to set up the human readable name for logging.
	SetTestStepName(aStepName);		
	}

TVerdict CTestDevNull::doTestStepPreambleL()
	{
	__UHEAP_MARK;	
	SetTestStepResult(EPass);
	return TestStepResult();
	}



TVerdict CTestDevNull::doTestStepPostambleL()
	{
	__UHEAP_MARKEND;	
	return TestStepResult();
	}


TVerdict CTestDevNull::doTestStepL()
	{
	__UHEAP_MARK;
	int err;
   	if(TestStepName() == Ktest_devnullread )
   		{
   		INFO_PRINTF1(_L("TestDevNullRead():"));
   	   	err = TestDevNullRead();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	if(TestStepName() == Ktest_devnullwrite )
   	   		{
   	   		INFO_PRINTF1(_L("TestDevNullWrite():"));
   	   	   	err = TestDevNullWrite();
   	   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	   	}
   	__UHEAP_MARKEND;  
   	
	return TestStepResult(); 
	}

TVerdict CTestDevNull::TestDevNullRead()
	{
	SetTestStepResult(EPass);
	int ret = -1 ;
	char read_buf[10];
	memset(read_buf,0,10);
	
	wscanf(L"%d",&ret);
	if( ret != -1 )
		{
		INFO_PRINTF1(_L("wscanf failed"));
		SetTestStepResult(EFail);
		}
	int character1 = getwchar();
	if( character1 != WEOF )
		{
		INFO_PRINTF1(_L("getwchar"));
		SetTestStepResult(EFail);
		}
	if(gets(&read_buf[0]) != NULL )
		{
		INFO_PRINTF1(_L("gets failed"));
		SetTestStepResult(EFail); 
		}
	if( getchar() != EOF )
		{
		INFO_PRINTF1(_L("getchar"));
		SetTestStepResult(EFail);
		}
	return TestStepResult();
	}


TVerdict CTestDevNull::TestDevNullWrite()
	{
	SetTestStepResult(EPass);
	if( wprintf(L"Using wprintf\n") < 0 )
		{
		INFO_PRINTF1(_L("wprintf failed"));						
		SetTestStepResult(EFail);
		}
	if( putwchar(L'Y') < 0 )
		{
		INFO_PRINTF1(_L("putwchar failed"));						
		SetTestStepResult(EFail);
		}
	char read_buf[10] = {"\0"};
	(void) memcpy(read_buf, "abcdefgh",8);
	if(puts(read_buf)<0)					
		{
		INFO_PRINTF1(_L("puts failed"));		
		SetTestStepResult(EFail);  		
		}
	if( !(putchar(L'a')>0) )
		{
		INFO_PRINTF1(_L("putchar failed"));						
		SetTestStepResult(EFail);  		
		}
	if ( printf("Read integer again\n") < 0 )
		{
		INFO_PRINTF1(_L("printf failed"));						
		SetTestStepResult(EFail);
		}
	return TestStepResult();  	
	}

	
