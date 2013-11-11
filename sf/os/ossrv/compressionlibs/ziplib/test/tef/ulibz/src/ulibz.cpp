// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name        : ulibz.cpp
// 
//

 

#include "ulibz.h"

CTestlibz::~CTestlibz() 
	{ 

	}  

CTestlibz::CTestlibz(const TDesC& aStepName)
	{
	// MANDATORY Call to base class method to set up the human readable name for logging.
	SetTestStepName(aStepName);		
	}

TVerdict CTestlibz::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}



TVerdict CTestlibz::doTestStepPostambleL()
	{
	return TestStepResult();
	}


TVerdict CTestlibz::doTestStepL()
	{
	int err;

	
	//testlibz steps
 	if(TestStepName() == KLibzcomp_decomp)
   			{
   	   		INFO_PRINTF1(_L("KLibzcomp_decomp():"));
   	   		err = libzcomp_decomp();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   		
	else if(TestStepName() == KLibzdefl_Infl)
   			{
   	   		INFO_PRINTF1(_L("KLibzdefl_Infl():"));
   	   		err = libzdefl_Infl();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   		
	else if(TestStepName() == KLibzgzio)
   			{
   	   		INFO_PRINTF1(_L("Kglobmark():"));
   	   		iRfs.Connect();
   	   		iRfs.MkDirAll(KDIRPATH);
 
   	   		err = libzgzio();
   	   		
   	   		iRfile.Close();
   	   		iRfs.Delete(KGZFILE);
   	   		iRfs.RmDir(KDIRPATH);
   	   		iRfs.Close();
   	   		
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else
			{
			SetTestStepResult(EPass);
			}
			
	return TestStepResult(); 
	}


