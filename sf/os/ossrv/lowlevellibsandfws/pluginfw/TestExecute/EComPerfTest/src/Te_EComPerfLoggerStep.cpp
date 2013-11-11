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
// Implements the performance loggr for ECOM
// 
//

/**
 @file
 @internalComponent
*/

#include "Te_EComPerfLoggerStep.h"
#include <e32test.h>
#include <f32file.h>
#include <bautils.h>
#include <startup.hrh>
#include <ecom/ecom.h>
#include "EComSessionAux.h"

CEComPerfLogger::CEComPerfLogger() : CEComPerfTestBase(KEComPerfLogger)
	{
	}

CEComPerfLogger::~CEComPerfLogger()
/**
 * Destructor
 */
	{
	// does nothing here.
	}
	
TVerdict CEComPerfLogger::doTestStepL()
	{
#ifdef __ECOM_SERVER_PERFORMANCE__
	// get all the timer results from the server
	RStartupStateTimerResults timerResults;
	timerResults.GetAllTimerResults();

	TInt i = 0;
	TInt count = timerResults.Count();
	for(i=0; i<count; i++)
		{
		const TStartupStateTimerResult& timerResult = timerResults.At(i);
		
		INFO_PRINTF3(_L("State: %d start time: %f mSecs\n"), timerResult.iState , timerResult.iStartTime);
		INFO_PRINTF3(_L("State: %d end time: %f mSecs\n"), timerResult.iState, timerResult.iEndTime);
		INFO_PRINTF3(_L("State: %d elapsed time: %f mSecs\n\n"), timerResult.iState, timerResult.iEndTime - timerResult.iStartTime);
		}
#else
	MacroNotDefinedError();
#endif // __ECOM_SERVER_PERFORMANCE__
	return TestStepResult();
	}
