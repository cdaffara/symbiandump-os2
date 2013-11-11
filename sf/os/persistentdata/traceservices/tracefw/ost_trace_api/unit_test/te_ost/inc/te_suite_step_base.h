// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file te_suite_step_base.h
 @internalTechnology
*/

#ifndef TE_SUITE_STEP_BASE__
#define TE_SUITE_STEP_BASE__
#include <test/testexecutestepbase.h>
#include "te_tracer.h"
#include "te_logger.h"


/****************************************************************************
* The reason to have a new step base is that it is very much possible
* that the all individual test steps have project related common variables
* and members
* and this is the place to define these common variable and members.
*
****************************************************************************/
class TTraceConfigs;

class CTe_UTraceSuiteStepBase : public CTestStep
	{
public:
	virtual ~CTe_UTraceSuiteStepBase();
	CTe_UTraceSuiteStepBase();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();

public: 
	//Please add your class members which will be common to all individual test steps:
protected:
	TInt StartLoggingL(TTraceConfigs& aTraceData);
	TInt StopLogging();
	void SetLogFilename(const TDesC& aFilename);
	void DeleteLogFileL();
protected:
	TTraceTester	iTraceTester;
	TTraceConfigs	iTraceData;
	TLogger 		iLogger;
private:
	TInt ReadGeneralConfigs();	
	};

#endif //TE_SUITE_STEP_BASE__
