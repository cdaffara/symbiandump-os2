// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This contains CTestStepFlogger which is the base class for all 
// the flogger suite test cases

#if (!defined __TESTSTEPCOMSDBG_H__)
#define __TESTSTEPCOMSDBG_H__

#include "TE_comsdbgServer.h"

#include <comms-infras/commsdebugutility.h>

class CTestSuite;
class CTE_comsdbgServer;

const TInt KTimeForDisplay = 0;   ///< Usually set to zero. Set to 1000000, or likewise when debugging to see progress output on console or when using serial output.
const TInt KTimeForDisplay1 = KTimeForDisplay;    // historially-different delay but now is unecessary

const TInt KFloggerFileFlushTime = 	1000000;	 //< Flogger flushes its file buffer every second
const TInt KFloggerIdleTimeWait = 1000000;	 //< A guess (!) at how long the system needs to be idle before flogger's lower-priority thread kicks in and writes the data.
const TInt KFloggerWriteTime = 500000;		///< Give flogger 1/2 second to write the data.
const TInt KFloggerWriteLargeDataTime = 1500000;		///< Give flogger 1 and 1/2 second to write the data when there is a lot.
const TInt KTimeToLog = KFloggerFileFlushTime + KFloggerIdleTimeWait + KFloggerWriteTime;   //< 2.5-second delay used to guarantee the logger will have written to the log file before reading the message.
const TInt KTimeToLogLargeData = KFloggerFileFlushTime + KFloggerIdleTimeWait + KFloggerWriteLargeDataTime;   //< 2.5-second delay used to guarantee the logger will have written to the log file before reading the message.

const TInt KHeapBufSize = 50000; ///< Flogger doesn't expose the file write buf size, so we redefine it here
const TInt KHeapBufFillIterations = KHeapBufSize / KLogBufferSize;  ///< number of times we need to write a maximum-sized string to fill flogger's buffer


class CTestStepFlogger : public CTestStep
{
public:
	CTestStepFlogger();
	~CTestStepFlogger();

	// pointer to suite which owns this test 
	CTE_comsdbgServer* iFloggerSuite;

	
	TInt executeStep( CTestStepFlogger& aTestStep );
	TInt executeStep(CTestStepFlogger& aTestStep, TBool aStatus);
	TInt DoTestConnect(RFileLogger& aLogger);
	TInt executeStep ( TBool bypassChecks = EFalse );
	virtual TInt executeStepL();
	virtual TInt executeStepL(TBool aStatus);
	TVerdict doTestStepPostambleL();
	TInt doTestStepWithHeapFailureL( CTestStepFlogger& aTestStep, TInt aReturnValue, TInt lowMemory, TInt highMemory, TBool bypassChecks = EFalse);
	
	protected:
	TInt constructFloggerIniL( const TDesC8& additionalConfig );
	TInt replaceFloggerIniL( const TDesC8& newConfig );
	void ForceLogFlush(RFileLogger& aLogger);
	
	};




#endif /* __TESTSTEPCOMSDBG_H__ */
