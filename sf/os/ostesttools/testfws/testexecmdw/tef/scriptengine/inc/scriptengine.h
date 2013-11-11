/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/



/**
 @file ScriptEngine.h
*/

#if !(defined __SCRIPT_ENGINE_H__)
#define __SCRIPT_ENGINE_H__

#include <e32base.h>
#include <f32file.h>
#include "testexecutestepbase.h"
#include <e32cons.h>
#if !(defined TEF_LITE)
#include <w32std.h>
#endif
#include <iniparser.h>
#include <test/tefshareddata.h>
#include <test/testshareddata.h>
#include "tefconst.h"
#include "blockitems.h"

#define SUB_PROC_TYPE RProcess

/** 
 * Define flogger based class that can also be used to log to console
 */
class RConsoleLogger
	{
public:
	RConsoleLogger(CConsoleBase& aConsole) : iConsole(aConsole) {};
	inline CConsoleBase& Console() const;
private:
	CConsoleBase& iConsole;
	};

class CActiveBase;
class MChildCompletion
	{
public:
	virtual void ChildCompletion(TInt aError) = 0;
	};

class RScriptTestServ : public RTestServ
	{
friend class RScriptTestSession;
public:
	inline RScriptTestServ();
	TInt Connect(const TDesC& aServerName,TBool aSharedData = EFalse);
	inline TBool SharedData() const;
	inline TInt SessionCount() const;
private:
	inline void AddSession();
	inline void RemoveSession();
private:
	TBool iSharedData;
	TBool iSessionCount;
	};

class RScriptTestSession : public RTestSession
	{
public:
	TInt Open(RScriptTestServ& aServ, const TBool aIsTestStep, const TDesC& aStepName = KNull );
	inline void Close();
protected:
	RScriptTestServ* iServ;
private:
	};

class CActiveBase : public CActive
	{
public:
	inline void Kick();
	inline virtual ~CActiveBase();
protected:
	inline void Prime();
	inline void Complete(TInt aErr);
	inline CActiveBase();
	};

class CScriptActive : public CActiveBase, public MChildCompletion
	{
public:
	inline virtual ~CScriptActive();
	inline virtual void ChildCompletion(TInt aErr);
protected:
	inline CScriptActive();
	};


class MTaskCompletion
	{
public:
	virtual void TaskCompletion(TInt aErr, const TDesC& aPanicString, TInt aScriptLineNumber,const TDesC& aCommand,TBool aTaskCanComplete,TTEFItemArray* aItemArray=NULL) = 0;
	};

class TRange
	{
public : 
	enum EState
	{
	EStateNotUsed,
	EStateInUse,
	EStateUsed,
	};
	
	TRange(TPtrC aStartTestCase,TPtrC aEndTestCase  ):
	iStartTestCase (aStartTestCase),
	iEndTestCase (aEndTestCase ),
	iState (EStateNotUsed)
	{};
	
	//some utility methods
	static TBool CompareTRangeEnd(const TRange& aRng1, const TRange& aRng2) ;
	static TBool CompareTRangeStartCase(const TRange& aRng1, const TRange& aRng2) ;
	static TInt CompareTRangeStartOrder(const TRange& aRng1, const TRange& aRng2) ;

public : 
	TPtrC    iStartTestCase ;
	TPtrC    iEndTestCase ;
	EState	iState ;

	};

enum ESelectiveTesting 
	{
	iInclusive =0 ,
	iExclusive	
	};

class TSelectiveTestingOptions
	{
public :
	TSelectiveTestingOptions(RArray<TRange>& aSelectiveCaseRange,ESelectiveTesting& aSelectiveTestingType )
	:iSelectiveCaseRange(aSelectiveCaseRange),
	iSelectiveTestingType(aSelectiveTestingType)
	{};
	
	//some utility methods...
	static TBool CompareTPtrC(const TPtrC& aPtrC1, const TPtrC& aPtrC2);
	static TInt CompareTPtrCOrder(const TPtrC& aPtrC1, const TPtrC& aPtrC2) ;
	
	TSelectiveTestingOptions::~TSelectiveTestingOptions()
	 	{iSelectiveCaseRange.Close();};

	
public :
	RArray<TRange> iSelectiveCaseRange;
	ESelectiveTesting iSelectiveTestingType;
	};

class CScriptMaster : public CScriptActive
	{
public:
	CScriptMaster(const TDesC& aScriptFilePath, CTestExecuteLogger& aLogger, RConsoleLogger& aConsoleLogger, const TDriveName& aSysDrive, const TDriveName& aTestSysDrive, TSelectiveTestingOptions* aSelTestingOptions = NULL );
	virtual ~CScriptMaster();
	void RunL();
	inline void DoCancel();
private:
	inline CTestExecuteLogger& Logger() const;
	inline RConsoleLogger& ConsoleLogger() const;
private:
	enum TState{EInit,ERunning};
	TState iState;
	TPtrC iScriptFilePath;
	CTestExecuteLogger& iLogger;
	RConsoleLogger& iConsoleLogger;
	TBool iLoop;
	TBool iStartLooping;
	TDriveName iDefaultSysDrive;
	TDriveName iTestSysDrive;
	TSelectiveTestingOptions* iSelTestingOptions;
	};

class CSyncControl;
class CScriptControl : public CScriptActive, public MTaskCompletion
	{
public:
								CScriptControl(MChildCompletion& aCompletion,const TDesC& aScriptFilePath,CTestExecuteLogger& aLogger,RConsoleLogger& aConsoleLogger,TBool aStartLooping,TBool aLoop, const TDriveName& aSysDrive, const TDriveName& aTestSysDrive, TSelectiveTestingOptions* aSelTestingOptions = NULL);
	virtual						~CScriptControl();
	virtual void				TaskCompletion(TInt aErr, const TDesC& aPanicString, TInt aScriptLineNumber, const TDesC& aCommand, TBool aTaskCanComplete, TTEFItemArray* aItemArray = NULL);
	void						RunL();
	inline void					DoCancel();
private:
	void						PrintFromScriptLine() const;
	void						LogTestCaseMarkerL();
	TVerdict					HasTestCasePassedL(TPtrC TestCaseMarker);
	void						PrintCurrentScriptLine() const;
	void						SetPrefix();
	TBool						GetNextScriptLine(TPtrC& aScriptLine);
	TBool						GetLoopScriptLine(TPtrC& aScriptLine);
	void						CreateScriptDataFromScriptFileL();
	void						CreateServerFromScriptLineL();
	TBool						GetServerIndexFromScriptLine(TInt& aIndex);
	void						RunUtilsFromScriptLineL() const;
	void						GetScriptFileFromScriptLine();
	void						RunCedFromScriptLineL();
	void						StartTimerFromScriptLine();
	void						LogResult(TVerdict aResult, const TDesC& aPanicString, TInt aScriptLineNumber, const TDesC& aCommand);
	inline CTestExecuteLogger&	Logger() const;
	inline RConsoleLogger&		ConsoleLogger() const;
	void						PrintScriptEngineError(TInt aErr) const;
	TBool						CheckValidScriptLine() const;
	TBool						CheckCommentedCommands() const;
	void						FindValueL(CIniData* aConfigData, TPtrC aConfigSection, TPtrC aSharedData, TInt& aSharedDataNum);
	void						FindValueL(CIniData* aConfigData, TPtrC aConfigSection, TPtrC aSharedData, TPtrC& aSharedDataName);
	void						CreateSharedObjectsFromScriptLineL();
	void						ParseTestBlockL( TTEFItemArray& aItemArray );
	void						MakeAbsoluteFilePathsL(TPtrC16& aScriptLineString);
	void						ConvertRelativeToAbsoluteL(TDes& aCommandString);
	TInt 						RunError(TInt aError);
	void						ExpandVariables(TDes& aCommandString);
	TBool 						ProceedTestCase() ;
	void 						ProcessEndCase();
	// Fix defect 118337
	TBool						ParseTestBlockHeader();
	// End defect 118337
private:
	enum TState{
				EInit,
				ERunning,
				EClosing,
				ERunCedPending,
				ERunCed,
				ERunScriptPending,
				ERunScript,
				EDelayPending,
				EPausePending,
				EPause,
				EEndTestCasePending,
				ERunEndTestCase,
				ERunStartSyncTestCase,
				EPrefixPending,
				ERunPrefix,
				EDelay,
				EIdling,
				EWaitCompletions,
				ETestCaseIgnore
				};
	TPtrC								iScriptFile;
	RTimer								iTimer;
	TState								iState;
	HBufC*								iScriptData;
	TLex								iScriptLex;
	MChildCompletion&					iParent;
	TPtrC								iCurrentScriptLine;
	TBuf<KMaxTestExecuteCommandLength>	iScriptLinePrefix;	
	TBuf<KMaxTestExecuteCommandLength>	iPrefixedCurrentScriptLine;
	TBool								iConcurrent;
	TBool								iCanComplete;
	TBool								iBreakOnError;
	TInt								iAsyncTasksOutstanding;
	RPointerArray<RScriptTestServ>		iServers;
	TInt								iCurrentScriptLineNumber;
	CTestExecuteLogger&					iLogger;
	RConsoleLogger&						iConsoleLogger;
	TPtrC								iChildScriptFile;
	RFs									iFS;
	RFile								iResultFile;
	TBool								iScriptLinePrefixSet;
	TLex								iTempStoreLoop;
	TLex								iStoreLoop;
	TBool								iStartLooping;
	TInt								iLoopCounter;
	TBool								iCheckVar;
	TInt								iRepeatParam;
	TBool								iLoop;
	CTEFSharedData<CTestSharedData>*	iSharedDataArray[KTEFMaxSharedArraySize];
	TInt								iSharedDataNum;
	TBool								iIsSharedData;
	CSyncControl*						iSyncControl;
	// Fix defect 119337. Change static TBool value to TInt value
	// This value shows the nested count when call RUN_SCRIPT in loop
	static TInt							iNestedNumRunScriptInLoop;
	// End defect 119337
	TBuf<KMaxTestExecuteCommandLength>	iTestCaseID;
	TBuf<KTEFMaxScriptLineLength>		iAlteredScriptLine;
	TDriveName							iDefaultSysDrive;
	TDriveName							iTestSysDrive;
	TSelectiveTestingOptions* 			iSelTestingOptions;
	TBool 								iSelectOne;
	TInt 								iRangeRefCounter;
	TInt 								iTestCaseCounter;
	TPtrC   							iTestCaseIDToIgnore ; 
public:
	static TInt							commentedCommandsCount;
	static TInt							iRunScriptFailCount;
	};

class MTaskTimerCompletion
	{
public:
	virtual void TaskTimerCompletion() =0;
	};

class CTaskTimer : public CTimer
	{
public:
	static CTaskTimer* NewL(MTaskTimerCompletion& aCompletion);
	~CTaskTimer();
	void RunL();
	void Timeout(TInt aSeconds);
private:
	CTaskTimer(MTaskTimerCompletion& aCompletion);
	void RunTimeout();
	inline void ConstructL();
private:
	MTaskTimerCompletion&	iParent;
	TInt					iTimesAfter;
	TInt					iSecondsRemaining;
	};

const TInt KDefaultTimeoutSeconds = 120;
const TInt KDefaultDelayMilliseconds = 5000;
const TInt KTEFStatusDelay = 2000;


class CTaskControlBase : public CActiveBase , public MTaskTimerCompletion
	{
public:
	inline void SetTaskComplete(TBool aTaskCanComplete);
protected:
	CTaskControlBase(const TDesC& aCommand, MTaskCompletion& aCompletion,TInt aScriptLineNumber, CTestExecuteLogger& aLogger);
	virtual ~CTaskControlBase();
	inline const CTestExecuteLogger& Logger() const;
private:
protected:
	TBuf<KTEFMaxScriptLineLength> iCommandLine;
	MTaskCompletion&				iParent;
	TInt							iScriptLineNumber;
	CTestExecuteLogger&				iLogger;
	CTaskTimer*						iTaskTimer;
	TInt							iTaskTimeout;
	TBuf<KTEFMaxTaskExitCategory>	iTaskExitCategory;
	TBool							iTaskCanComplete;
private:
	};

class CClientControl : public CTaskControlBase
	{
public:	
	CClientControl(	RScriptTestServ& aTestServ,
					const TDesC& aStepCommand,
					MTaskCompletion& aCompletion,
					TInt aScriptLineNumber,
					CTestExecuteLogger& aLogger,
					TInt aLoopIndex,
					const TDesC& aTestCaseID,
					TPtrC& aScriptFilePath,
					const TDriveName& aSysDrive,
					const TDriveName& aTestSysDrive);
					
	static	CClientControl* NewL(	RScriptTestServ& aTestServ,
									const TDesC& aCommand,
									MTaskCompletion& aCompletion,
									TInt aScriptLineNumber,
									CTestExecuteLogger& aLogger,
									TInt aLoopIndex,
									const TDesC& aTestCaseID,
									const TDesC& aEndBlockCommand,
									TPtrC& aScriptFilePath,
									const TTEFItemArray& aBlockArray,
									const TDriveName& aSysDrive,
									const TDriveName& aTestSysDrive);

	virtual void TaskTimerCompletion();
	virtual ~CClientControl();
	void RunL();
	inline void DoCancel();
private:
	void GetStepParamsFromStepCommand();
	void ConstructL( const TDesC& aEndBlockCommand, const TTEFItemArray& aBlockArray );
	TTEFItemArray* CreateBlockArrayLC();
	TInt RunError(TInt aError);
	void TestStepComplete();
	
private:
	enum TState
		{
		EInit		= 0,
		ERunning,
		EServerRetry
		};

	TPtrC								iEndBlockCommand;		
	TState								iState;
	RScriptTestServ&					iServ;
	TBuf<KMaxTestExecuteCommandLength>	iStepArgs;
	TPtrC								iStepName;
	RScriptTestSession					iSession;
	RTimer								iTimer;
	TInt								iRetryCount;
	TPtrC								iTaskHeapSize;
	TBool								iOOMRequestSet;
	TBuf<KMaxTestExecuteCommandLength>	iSetUpParamValue;
	TInt								iLoopIndex;
	TBuf<KMaxTestExecuteCommandLength>	iTestCaseID;
	HBufC8*								iBlockArrayPkg;
	TPtr8								iBlockArrayPtr;
	TPtrC								iScriptFilePath;
	TDriveName							iDefaultSysDrive;
	TDriveName							iTestSysDrive;
	};

class CProgramControl : public CTaskControlBase
	{
public:
	virtual void TaskTimerCompletion();
	CProgramControl(const TDesC& aCommand,MTaskCompletion& aCompletion,TInt aScriptLineNumber, CTestExecuteLogger& aLogger, TBool aWSProgRun=EFalse);
	~CProgramControl();
	void RunL();
	inline void DoCancel();
private:
	void GetProgramArgsFromCommand();
	void RunProgramL();
	#if !defined TEF_LITE
	bool IsWindowServerAvailable(void) const;
	#endif
private:
	SUB_PROC_TYPE iProgram;
	TPtrC iProgramArgs;
	TBool iProgRenamed;
	enum TState{EInit,ERunning};
	TState iState;
	TBool iWSProgRun;
	#if !defined TEF_LITE
	// Delete dependence of MW apparc.lib. 
	// apparc.lib is moved from OS layer to MW layer. 
	// TEF will delete this dependece because TEF is built on OS layer.
	SUB_PROC_TYPE iProgramWS;
	RWsSession iWs;
	#endif
	};

#if !defined TEF_LITE
inline bool CProgramControl::IsWindowServerAvailable(void) const
{
	return (0 != iWs.WsHandle());
}
#endif
class CSyncControl : public CBase
	{
public:
	CSyncControl();
	~CSyncControl();
	
	static	CSyncControl* NewL();
	static	CSyncControl* NewLC();
	
	void	SetResultL( TVerdict aError );
	TBool	TestCaseContinueL();
	
private:
	void ConstructL();
	
	CTEFSharedData<CTestSharedData>* iSharedTEFStatus;
	CTEFSharedData<CTestSharedData>* iSharedTEFResult;
	};
	
#include "scriptengine.inl"
#endif
