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
// Defines the ECom Performance Test for the SSA discovery.
// 
//

/**
 @file
 @internalComponent
*/

 
#ifndef	TE_ECOMSTARTUPSTATEPERFTESTSTEP_H
#define	TE_ECOMSTARTUPSTATEPERFTESTSTEP_H

#include <e32cmn.h>
#include "Te_EComPerfTestStep.h"
#include "EComSessionAux.h"

_LIT(KEComStartupStatePerfTest, "EComStartupStatePerfTest");
        
class CEComStartupStatePerfTest : public CEComPerfTestBase
	{
public:
	CEComStartupStatePerfTest();
	~CEComStartupStatePerfTest();
	virtual TVerdict doTestStepL(void);
	// doTestStepPreamble and doTestStepPostamble are implemented in CEComPerfTestBase, 
	// which performs initialization common to all ECom performance tests	
private:
#ifdef __ECOM_SERVER_PERFORMANCE__
	void RetrieveRegistryCountL(RegistryCounts::TRegistryCounts::TRegistryCountDriveType aDriveType,
		RegistryCounts::TRegistryCounts& aCounts, TPtrC aMessage);
	TVerdict ProcessState(RStartupStateTimerResults& aTimerResults, 
		TInt aTimerState, 
		TReal& aStateStartTime, 
		TReal& aStateComplTime);
	TVerdict ProcessState(REComHeapUsageRecords& aHeapResults, TInt aState,TInt& aHeapUsage);		
#endif //__ECOM_SERVER_PERFORMANCE__

	};
	
#endif //TE_ECOMSTARTUPSTATEPERFTESTSTEP_H
