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
// Name        : tlibcbackend.cpp
//



 /*
* ==============================================================================
*  Name        : tlibcbackend.cpp
*  Part of     : backend / testlibcbackend
*
*  Description : ?Description
*  Version:   0.5
*
*  Copyright (C) 2002 Nokia Corporation.
*  This material, including documentation and any related 
*  computer programs, is protected by copyright controlled by 
*  Nokia Corporation. All rights are reserved. Copying, 
*  including reproducing, storing,  adapting or translating, any 
*  or all of this material requires the prior written consent of 
*  Nokia Corporation. This material also contains confidential 
*  information which may not be disclosed to others without the 
*  prior written consent of Nokia Corporation.
*
* ============================================================================
*/

#include "tlibcbackend.h"
#include <unistd.h> 
#include <errno.h>
#include <stdio.h>
#include <e32std.h>
#include <stdlib.h>
#include <string.h>


CTestLibcbackend::~CTestLibcbackend() 
	{ 
	}  

CTestLibcbackend::CTestLibcbackend(const TDesC& aStepName)
	{
	// MANDATORY Call to base class method to set up the human readable name for logging.
	SetTestStepName(aStepName);		
	}

TVerdict CTestLibcbackend::doTestStepPreambleL()
	{
	iParamCnt=0;
	SetTestStepResult(EPass);
	return TestStepResult();
	}



TVerdict CTestLibcbackend::doTestStepPostAmbleL()
	{
	return TestStepResult();
	}


TVerdict CTestLibcbackend::doTestStepL()
	{
		int err;

   		if(TestStepName() == KSocketIOTest)
   			{
   	   		INFO_PRINTF1(_L("SocketIOTest():"));
   	   		err = SocketIOTest();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else if(TestStepName() == KFileIOTest)
   			{
   	   		INFO_PRINTF1(_L("FileIOTest():"));
   	   		err = FileIOTest();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else if(TestStepName() == KServerLockTest)
   			{
   	   		INFO_PRINTF1(_L("ServerLockTest():"));
   	   		err = ServerLockTest();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else if(TestStepName() == KOpenDirTest)
   			{
   	   		INFO_PRINTF1(_L("OpenDirTest():"));
   	   		err = OpenDirTest();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else if(TestStepName() == KPopenPipeTest)
   			{
   	   		INFO_PRINTF1(_L("PopenPipeTest():"));
   	   		err = PopenPipeTest();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	
		return TestStepResult(); 

	}
 



