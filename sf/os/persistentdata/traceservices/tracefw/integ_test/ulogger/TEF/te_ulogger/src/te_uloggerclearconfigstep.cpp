// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Example CTestStep derived implementation
// 
//

/**
 @file te_uloggerclearconfigstep.cpp
 @internalTechnology
*/
#include "te_uloggerclearconfigstep.h"
#include "te_uloggermclsuitedefs.h"

using namespace Ulogger;

CULoggerClearConfigStep::~CULoggerClearConfigStep()
/**
 * Destructor
 */
	{
	}

CULoggerClearConfigStep::CULoggerClearConfigStep()
/**
 * Constructor
 */
	{
		SetTestStepName(KULoggerClearConfigStep);
	}

TVerdict CULoggerClearConfigStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
		INFO_PRINTF1(_L("Please delete this line or modify it. I am in Test Step Preamble in Class CULoggerClearConfigStep"));
		SetTestStepResult(EPass);
		return TestStepResult();
	}


TVerdict CULoggerClearConfigStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
		RULogger logger;
	    logger.Connect();
	    CClearConfig configIni;
	    configIni.ClearL(logger);		
     	SetTestStepResult(EPass);
		return TestStepResult();
	}



TVerdict CULoggerClearConfigStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
		INFO_PRINTF1(_L("Please delete this line or modify it. I am in Test Step Postamble in Class CULoggerClearConfigStep"));
		return TestStepResult();
	}
