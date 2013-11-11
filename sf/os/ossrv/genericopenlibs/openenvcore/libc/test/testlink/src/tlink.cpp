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



#include "tlink.h"
#include <unistd.h> 
#include <errno.h>
#include <stdio.h>
#include <e32std.h>
#include <stdlib.h>
#include <string.h>


CTestLink::~CTestLink() 
	{ 
	}  

CTestLink::CTestLink(const TDesC& aStepName)
	{
	// MANDATORY Call to base class method to set up the human readable name for logging.
	SetTestStepName(aStepName);		
	}

TVerdict CTestLink::doTestStepPreambleL()
	{
	__UHEAP_MARK;	
	iParamCnt=0;
	SetTestStepResult(EPass);
	return TestStepResult();
	}



TVerdict CTestLink::doTestStepPostambleL()
	{
	__UHEAP_MARKEND;	
	return TestStepResult();
	}


TVerdict CTestLink::doTestStepL()
	{
	int err;

   	if(TestStepName() == KLink)
   		{
   		INFO_PRINTF1(_L("Link():"));
   		err = Link();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else
   		if(TestStepName() == KLink1)
   		{
   		INFO_PRINTF1(_L("Link1():"));
   		err = Link1();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else
   		if(TestStepName() == KLink2)
   		{
   	   	INFO_PRINTF1(_L("Link2():"));
   	   	err = Link2();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else
   	   	if(TestStepName() == KLink3)
   		{
   	   	INFO_PRINTF1(_L("Link3():"));
   	   	err = Link3();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
    else
   	   	if(TestStepName() == KLink4)
   		{
   	   	INFO_PRINTF1(_L("Link4():"));
   	   	err = Link4();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else
   	   	if(TestStepName() == KLink5)
   		{
   	   	INFO_PRINTF1(_L("Link5():"));
   	   	err = Link5();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	    }
   	else
   	   	if(TestStepName() == KLink6)
   		{
   	   	INFO_PRINTF1(_L("Link6():"));
   	   	err = Link6();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else
   	   	if(TestStepName() == KLink7)
   		{
   	   	INFO_PRINTF1(_L("Link7():"));
   	   	err = Link7();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else
   	   	if(TestStepName() == KReadWriteLink)
   		{
   	   	INFO_PRINTF1(_L("ReadWriteLink():"));
   	   	err = ReadWriteLink();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else
   	   	if(TestStepName() == KReadLink)
   		{
   	   	INFO_PRINTF1(_L("ReadLink():"));
   	   	err = ReadLink();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else
   		if(TestStepName() == KReadLink1)
   		{
   	   	INFO_PRINTF1(_L("ReadLink1():"));
   	   	err = ReadLink1();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else
   	   	if(TestStepName() == Klinkstat)
   		{
   	   	INFO_PRINTF1(_L("linkstat():"));
   	   	err = linkstat();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else
   		if(TestStepName() == KUnlink)
   		{
   	   	INFO_PRINTF1(_L("Unlink():"));
   	   	err = Unlink();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else
   		if(TestStepName() == KUnlink1)
   		{
   	   	INFO_PRINTF1(_L("Unlink1():"));
   	   	err = Unlink1();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else
   		if(TestStepName() == KTestUnlinkFifo)
   		{
   	   	INFO_PRINTF1(_L("TestUnlinkFifo():"));
   	   	err = TestUnlinkFifo();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else
   		if(TestStepName() == KReadLinkTest)
   		{
   	   	INFO_PRINTF1(_L("ReadLinkTest():"));
   	   	err = ReadLinkTest();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	
	return TestStepResult(); 

	}
 

