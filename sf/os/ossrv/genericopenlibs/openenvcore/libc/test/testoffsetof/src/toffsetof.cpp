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
// Name        : offsetof.cpp
// 
//


#include "toffsetof.h"
#include "toffsetofserver.h"

	
	CTestOffsetof::~CTestOffsetof() 
	{ 
	}  

CTestOffsetof::CTestOffsetof(const TDesC& aStepName)
	{
	// MANDATORY Call to base class method to set up the human readable name for logging.
	SetTestStepName(aStepName);		
	}

TVerdict CTestOffsetof::doTestStepPreambleL()
	{
	__UHEAP_MARK;	
	SetTestStepResult(EPass);
	return TestStepResult();
	}



TVerdict CTestOffsetof::doTestStepPostambleL()
	{
	__UHEAP_MARKEND;	
	return TestStepResult();
	}

TVerdict CTestOffsetof::doTestStepL()
	{
		int err;
   		if(TestStepName() == Koffsetof)
   			{
   	   		INFO_PRINTF1(_L("offsetof_test():"));
   	   		err = offsetof_test();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	return TestStepResult(); 

	}	
   	   		
	
	
	
struct S
	{
	int member1;
	char member2;
	int member3;
	};

/*Test case Name:offsetof_test
 *API Tested 	:offsetof
 *Description   :The value of this expression is the offset in bytes to  the  structure
 *member  (designated by member-designator) from the beginning
 *of its structure (designated by type).
 *Return Value	:offsetof() returns the offset of the given element within the given type, in units of bytes.   
 */

 	
TInt CTestOffsetof::offsetof_test()
	{
	TInt ret =0 ;
	int off = offsetof(S,member3);
	if (off >= 0)
		{
		ret = KErrNone;
		}
	else
		{
		ret = KErrGeneral;
		}
	return ret;	
	}
