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
//

/**
 @file
 @test
 @internalComponent
*/
#if (!defined __BUR_STEP_H__)
#define __BUR_STEP_H__

#include "tef_efm_bursuitedefs.h"
#include <test/testexecutestepbase.h>
#include "tef_efm_bursuitestepbase.h"
#include "tef_efm_bursuitesimstepbase.h"
#include "efm_test_consts.h"

#include <bautils.h>

/** 
 * The BURStep class just calls Backup and Restore in the correct expected order.
 * This class will test normal operation of the BUR functionality in feature
 * manager.
 */
class CBURStep : public CTefEfmBURSuiteStepBase
	{
public:
	CBURStep();
	~CBURStep();

	virtual TVerdict doTestStepL();
	
	/**
	 * This is automaticaly run each time a test starts. 
	 * Overwrites the function in the CTefEfmBURSuiteStepBase class
	 * @return TVerdict EFail the test has fail  ETrue the test has passed 
	 */
	virtual TVerdict doTestStepPreambleL(); 
	
	/**
	 * This is automaticaly run when the test ends. 
	 * Overwrites the function in the CTefEfmBURSuiteStepBase class
	 * @return TVerdict EFail the test has fail  ETrue the test has passed 
	 */
	virtual TVerdict doTestStepPostambleL();

private:
	TPtrC iData;
	};

_LIT(KBURStep,"BURStep");

#endif
