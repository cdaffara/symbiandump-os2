/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
* Test passes everytime, without doing anything.  Used to allow arbitrary batch files to be run as pre-commands without being assigned to specific tests in the ONB.
*
*/


/**
 @file 
*/
#include "nooperationteststep.h"

CNoOperationTestStep::CNoOperationTestStep()
/**
 * Constructor
 */
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KNoOperationTestStep);
	}

TVerdict CNoOperationTestStep::doTestStepL()
/**
 * @return - EPass
 * A test which passes everytime.
 */
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}
