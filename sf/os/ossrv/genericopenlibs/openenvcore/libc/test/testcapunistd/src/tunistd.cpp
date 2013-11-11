// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name        : tunistd.cpp
// 
//


#include "tunistd.h"

CTestunistd::~CTestunistd() 
	{ 
	}  

CTestunistd::CTestunistd(const TDesC& aStepName)
	{
	// MANDATORY Call to base class method to set up the human readable name for logging.
	SetTestStepName(aStepName);		
	}

TVerdict CTestunistd::doTestStepPreambleL()
	{
	__UHEAP_MARK;	
	SetTestStepResult(EPass);
	return TestStepResult();
	}



TVerdict CTestunistd::doTestStepPostambleL()
	{
	__UHEAP_MARKEND;	
	return TestStepResult();
	}


TVerdict CTestunistd::doTestStepL()
	{
	int err;

	
	//captest steps
 if(TestStepName() == Kaccesssys)
   			{
   	   		INFO_PRINTF1(_L("accesssys():"));
   	   		err = accesssys();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == Kaccessprivate)
   			{
   	   		INFO_PRINTF1(_L("accessprivate():"));
   	   		err = accessprivate();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == Krmdirsys)
   			{
   	   		INFO_PRINTF1(_L("rmdirsys():"));
   	   		err = rmdirsys();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	
	else if(TestStepName() == Krmdirprivate)
   			{
   	   		INFO_PRINTF1(_L("rmdirprivate():"));
   	   		err = rmdirprivate();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == Kunlinksys)
   			{
   	   		INFO_PRINTF1(_L("unlinksys():"));
   	   		err = unlinksys();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	
	
	else if(TestStepName() == Kunlinkprivate)
   			{
   	   		INFO_PRINTF1(_L("unlinkprivate():"));
   	   		err = unlinkprivate();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
		else if(TestStepName() == Kreadlinksys)
   			{
   	   		INFO_PRINTF1(_L("readlinksys():"));
   	   		err = readlinksys();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
		else if(TestStepName() == Kreadlinkprivate)
   			{
   	   		INFO_PRINTF1(_L("readlinkprivate():"));
   	   		err = readlinkprivate();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	
		else if(TestStepName() == Ksymlinksys)
   			{
   	   		INFO_PRINTF1(_L("symlinksys():"));
   	   		err = symlinksys();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	
		else if(TestStepName() == Ksymlinkprivate)
   			{
   	   		INFO_PRINTF1(_L("symlinkprivate():"));
   	   		err = symlinkprivate();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	   else if(TestStepName() == Klinksys)
   			{
   	   		INFO_PRINTF1(_L("linksys():"));
   	   		err = linksys();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	
		else if(TestStepName() == Klinkprivate)
   			{
   	   		INFO_PRINTF1(_L("linkprivate():"));
   	   		err = linkprivate();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	
	else if(TestStepName() == Kchdirprivate)
   			{
   	   		INFO_PRINTF1(_L("chdirprivate():"));
   	   		err = chdirprivate();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	
		else if(TestStepName() == Kchdirsys)
   			{
   	   		INFO_PRINTF1(_L("chdirsys():"));
   	   		err = chdirsys();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	
	
	
	
	return TestStepResult(); 

	}
 
