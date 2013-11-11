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
//

/**
 @file te_uloggermclsuitestepbase.cpp
 @internalTechnology
*/

#include "te_uloggermclsuitestepbase.h"
#include "te_uloggermclsuitedefs.h"

// Device driver constants

TVerdict CTe_ULoggerMCLSuiteStepBase::doTestStepPreambleL()
/**
 * @return - TVerdict
 */
	{

	// process some common pre setting to test steps then set SetTestStepResult to EFail or Epass.
	INFO_PRINTF1(_L("Please delete this line or modify me!! I am in doTestStepPreambleL() of the class CTe_ULoggerMCLSuiteStepBase!"));
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CTe_ULoggerMCLSuiteStepBase::doTestStepPostambleL()
/**
 * @return - TVerdict
 */
	{
	// process some common post setting to test steps then set SetTestStepResult to EFail or Epass.
	INFO_PRINTF1(_L("Please delete this line or modify me!! I am in doTestStepPostambleL() of the class CTe_ULoggerMCLSuiteStepBase!"));
	//SetTestStepResult(EPass);  // or EFail
	return TestStepResult();
	}

CTe_ULoggerMCLSuiteStepBase::~CTe_ULoggerMCLSuiteStepBase()
	{
	}

CTe_ULoggerMCLSuiteStepBase::CTe_ULoggerMCLSuiteStepBase()
	{
	}
