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
//

/**
 @file te_createconfigemptystep.cpp
*/
 
#include "te_createconfigemptystep.h"
#include "te_clearconfig.h"

using namespace Ulogger;

CCreateConfigEmptyTestStep::~CCreateConfigEmptyTestStep()
/**
 * Destructor
 */
	{
	}

CCreateConfigEmptyTestStep::CCreateConfigEmptyTestStep()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KCreateConfigEmptyStep);
	}
 
 
/**
 * Override of base class virtual
 * @return - TVerdict code
 * 
 */
TVerdict CCreateConfigEmptyTestStep::doTestStepPreambleL()
	{
	return TestStepResult();
	}

/**
 * Override of base class virtual
 * @return - TVerdict code
 */
TVerdict CCreateConfigEmptyTestStep::doTestStepPostambleL()
	{
	return TestStepResult();	
	}
	
/**
 * Test 
 * @return - TVerdict code
 */
	
TVerdict CCreateConfigEmptyTestStep::doTestStepL()
	{	
	
	
	RULogger logger;
	  
	logger.Connect();
	CClearConfig configIni;
	
	configIni.ClearL(logger);

	logger.SetSecondaryFilteringEnabled(EFalse);	

	SetTestStepResult(EPass);
	
	return TestStepResult();	
	
	}
