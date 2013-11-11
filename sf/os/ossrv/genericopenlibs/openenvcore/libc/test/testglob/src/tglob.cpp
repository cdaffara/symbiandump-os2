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
// Name        : tfcntl.cpp
// 
//

 

#include "tglob.h"

CTestglob::~CTestglob() 
	{ 
	iRfs.RmDir(KDIRPATH1);
	iRfs.RmDir(KDIRPATH2);
	iRfs.RmDir(KBASEDIR);
	iRfs.Close();
	}  

CTestglob::CTestglob(const TDesC& aStepName)
	{
	// MANDATORY Call to base class method to set up the human readable name for logging.
	iRfs.Connect();
	iRfs.MkDirAll(KDIRPATH1);
	iRfs.MkDirAll(KDIRPATH2);
	SetTestStepName(aStepName);		
	}

TVerdict CTestglob::doTestStepPreambleL()
	{
	__UHEAP_MARK;	
	SetTestStepResult(EPass);
	return TestStepResult();
	}



TVerdict CTestglob::doTestStepPostambleL()
	{
	__UHEAP_MARKEND;
	return TestStepResult();
	}


TVerdict CTestglob::doTestStepL()
	{
	int err;

	
	//glob steps
 	if(TestStepName() == Kglobappend)
   			{
   	   		INFO_PRINTF1(_L("Kglobappend():"));
   	   		err = globappend();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   		
	else if(TestStepName() == Kglobbrace)
   			{
   	   		INFO_PRINTF1(_L("Kglobbrace():"));
   	   		err = globbrace();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   		
	else if(TestStepName() == Kglobdooffs)
   			{
   	   		INFO_PRINTF1(_L("Kglobdooffs():"));
   	   		err = globdooffs();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	
	else if(TestStepName() == Kglobmark)
   			{
   	   		INFO_PRINTF1(_L("Kglobmark():"));
   	   		err = globmark();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}	
   	   		
 	else if(TestStepName() == Kglobnocheck)
			{
	   		INFO_PRINTF1(_L("Kglobnocheck():"));
	   		err = globnocheck();
	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
	   		}
	   		
	else if(TestStepName() == Kglobnosort)
			{
	   		INFO_PRINTF1(_L("Kglobnosort():"));
	   		err = globnosort();
	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
	   		}
	else if(TestStepName() == Kglobtilde)
			{
	   		INFO_PRINTF1(_L("Kglobtilde():"));
	   		err = globtilde();
	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
	   		}
	else
			{
			SetTestStepResult(EPass);
			}
			
	return TestStepResult(); 
	}


