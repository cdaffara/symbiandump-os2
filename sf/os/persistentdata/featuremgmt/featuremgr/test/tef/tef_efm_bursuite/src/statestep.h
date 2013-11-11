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
#if (!defined __STATE_H__)
#define __STATE_H__

#include "tef_efm_bursuitedefs.h"
#include <test/testexecutestepbase.h>
#include "tef_efm_bursuitestepbase.h"
#include "tef_efm_bursuitesimstepbase.h"
#include "efm_test_consts.h"

#include <bautils.h>

class CStateStep : public CTefEfmBURSuiteStepBase
	{
public:
	CStateStep();
	~CStateStep();

	virtual TVerdict doTestStepL();
	
	/**
	 * This function makes the call to DoThreadActionL
	 * @see DoThreadActionL is called to execute the action content.
	 * @param aPtrThis a pointer to CStateStep
	 * @return TInt returns 1 if the thread is correctly launched, and 0 otherwise
	 */
	static TInt ThreadEntryPoint( TAny* aPtrThis );
	
	/**
	 * Perform the thread's actions. 
	 * @leave KErrNone Will leave if connection to RFeatureControl returns something other than KErrNone 
	 */ 
	void DoThreadActionL();
	
private:
	TPtrC iData;
	};

_LIT(KStateStep,"StateStep");

#endif
