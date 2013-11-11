// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the License "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Tests performance of run mode debug device component   
// 
//

#ifndef RMDEBUG_PERFORMANCE_H
#define RMDEBUG_PERFORMANCE_H

#include <u32hal.h>
#include <f32file.h>
#include <rm_debug_api.h>

/* Config for t_rmdebug_app.exe: 
 * -f1: prefetch abort,
 * -d0: zero delay before crash
 * -p: priority of test target main thread
 */    
_LIT(KTargetOptions,"-f1 -d0 -p%d");

// Default number of test runs
const TInt KNumOfTestRuns = 7;

/**
 @Class TAgentEventInfo
 
 Class for gathering event data from the run-mode driver
 */
class TAgentEventInfo
{
public:
    TAgentEventInfo() : iEventInfoBuf(iEventInfo) {}
   
public:
    // This is the underlying class for event interaction with the Run Mode debug API 
    Debug::TEventInfo          iEventInfo;
    
    TPckg<Debug::TEventInfo>  iEventInfoBuf; 
};

/**
  @Class CRunModeAgent
  
  The basic run mode agent
  */
class CRunModeAgent : public CBase
	{
public:
    
    enum displayHelp {EDontDisplayHelp =0, EDisplayHelp };
    
	static CRunModeAgent* NewL();
	~CRunModeAgent();
	void ClientAppL();  
	Debug::RSecuritySvrSession&  DebugDriver() { return iServSession; };	

private:
	void ConstructL();
	void StartTest();
	void TestDriverPerformance();
	void RunBenchMarkTest();
	TInt ParseCommandLine();
	
	void Attach();
	void Detach();
	    
	void HandleEvent(Debug::TEventInfo& aEventInfo);
	TInt LaunchProcessL(RProcess& aProcess, const TDesC& aExeName, const TDesC& aCommandLine);
	void DisplayUsage();
	void GetNanoTickPeriod();
	void SetEventAction();
	void SetDefaultParamValues();
	
	void CalculatePerformance();
	void GetEvent();
	void GetStartTime();
	TInt GetTimeInMs();
	
private:
	
	/** 
	  Used for test cases interacting with the RMDBG only
	 */
	TRequestStatus iStatus;   
	
	/**
	  The nanokernel tick period in MS
	*/
	TInt iTickPeriodMs;

	/*
	 * Handle to DSS
	 */
	Debug::RSecuritySvrSession iServSession;
	
	/**
	 Array to store tick counts between an iteration of a test run
	*/
	RArray<TUint> iTimeDifs;
	
	/**
	 Object to gather event data from RMDBG
	*/
	TAgentEventInfo iSEventInfo;
		
	/**
	  Flag to indicate test target has crashed
	*/
	TBool ilaunchCompleted;
	
	/*
	 * RProperty to get counter value from test app
	 */
	RProperty iIntegerProperty;
		
	struct PerformanceTestParams
	    {
	    enum TestOptions {EAttachAll=0,EAttachExe, EBenchMarkTest };
	    TUint iTestType;
	    TUint iNumOfTestRuns;
	    TUint iTestTargetPriority;
	    TUint iEvents;
	    TInt iCountEnd;
	    TInt iCountStart;
	    TUint iDriver;
	    } iParams;
	    
	};

#endif // RMDEBUG_PERFORMANCE_H

