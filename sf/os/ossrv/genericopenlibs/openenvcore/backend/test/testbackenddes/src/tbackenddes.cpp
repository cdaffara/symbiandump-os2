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
// Name        : tbackenddes.cpp
// 
//

#include "tbackenddes.h"
#include <unistd.h> 
#include <errno.h>
#include <stdio.h>
#include <e32std.h>
#include <stdlib.h>
#include <string.h>


CTestBackendDes::~CTestBackendDes() 
	{ 
	}  

CTestBackendDes::CTestBackendDes(const TDesC& aStepName)
	{
	// MANDATORY Call to base class method to set up the human readable name for logging.
	SetTestStepName(aStepName);		
	}

TVerdict CTestBackendDes::doTestStepPreambleL()
	{
	iParamCnt=0;
	SetTestStepResult(EPass);
	return TestStepResult();
	}



TVerdict CTestBackendDes::doTestStepPostAmbleL()
	{
	return TestStepResult();
	}


TVerdict CTestBackendDes::doTestStepL()
	{
		int err;

   		if(TestStepName() == KCLSIDesTest)
   			{
   	   		INFO_PRINTF1(_L("CLSIDesTest():"));
   	   		err = CLSIDesTest();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	
		return TestStepResult(); 

	}
 



