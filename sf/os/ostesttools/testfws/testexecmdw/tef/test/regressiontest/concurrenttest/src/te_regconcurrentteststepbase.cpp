/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Example CTestStep derived implementation
*
*/



/**
 @file te_regconcurrentteststep1.cpp
 @internalTechnology
*/
#include "te_regconcurrentteststepbase.h"
#include "te_regconcurrenttestsuitedefs.h"

CTe_RegConcurrentTestStepBase::~CTe_RegConcurrentTestStepBase()
/**
 * Destructor
 */
	{
	
	}

CTe_RegConcurrentTestStepBase::CTe_RegConcurrentTestStepBase()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	
	}

TVerdict CTe_RegConcurrentTestStepBase::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	SetTestStepResult(EPass);
	PrintCurrentTimeStamp();
	return TestStepResult();
	}


/*TVerdict CTe_RegConcurrentTestStepBase::doTestStepL()
*
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 
	{
	INFO_PRINTF1(_L("Running Test Step1..."));
	return TestStepResult();
	}*/



TVerdict CTe_RegConcurrentTestStepBase::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	PrintCurrentTimeStamp(EFalse);
	return TestStepResult();
	}

void CTe_RegConcurrentTestStepBase::PrintCurrentTimeStamp(TBool aStart)
	{
	TTime time;
	time.UniversalTime();
	TDateTime dateTime = time.DateTime();
	TBuf<32> timeBuf;
	timeBuf.Format(_L("%02d:%02d:%02d:%03d"), dateTime.Hour(), dateTime.Minute(), dateTime.Second(), dateTime.MicroSecond());
	TBuf<16> buf(aStart?_L("started"):_L("ended"));
	_LIT(KTimeFormat, "%S in server %S %S at %S");
	INFO_PRINTF5(KTimeFormat, &ConfigSection(), &GetServerName(), &buf, &timeBuf);
	}

