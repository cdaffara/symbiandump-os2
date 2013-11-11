// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// class CBackupFileObserver
// 
//

class CBackupFileObserver : public CActive, public MBackupObserver
	{
public:
	enum TFileLock
		{ELocked,EReadOnly,ENoAccess};
public:
	static CBackupFileObserver* NewL(TInt aFileObserverNumber);
	~CBackupFileObserver();
	void AddFileL(TInt aFileNumber);
public:
	TInt GetFileLocksChanged();
	void ZeroFileLocksChanged();
	void SetDelay(TBool aDelay);
private:
	CBackupFileObserver();
	void ConstructL(TInt aFileObserverNumber);
private: // from MBackupObserver
	void ChangeFileLockL(const TDesC& aFileName,TFileLockFlags aFlags);
public: // From CActive
	void RunL();
	void DoCancel();
public:
	void Kill(RThread* aThread);
private:
	// File observer session into the server
	CBaBackupSessionWrapper* iBackupSession;
	// Instance number for this obsever
	TInt iFileObserverNumber;
	// Number of file lock notifications received this backup
	TInt iFileLocksChanged;
	// Whether we're testing with an introduced delay or not
	TBool iDelay;
	// Local RTest for this class - needed because global RTest fails in multi-threaded environment
	RTest* iLocalRTest;
	// Name of the file we're observing
	TFileName iFileName;
	// State of the file we're observing
	TFileLock iFileLockState;
	};

//
//	class CBackupOperationObserver
//
class CBackupOperationObserver : public CActive, public MBackupOperationObserver
	{
public:
	static CBackupOperationObserver* NewL(TInt aObserverNumber);
	~CBackupOperationObserver();
public: // From CActive
	void RunL();
	void DoCancel();
public:
	void Kill(RThread* aThread);
private:
	CBackupOperationObserver();
	void ConstructL(TInt aObserverNumber);

	// from MBackupOperationObserver
	virtual void HandleBackupOperationEventL(const TBackupOperationAttributes& aBackupOperationAttributes);
private: 
	// Every backup observer has their own session
	CBaBackupSessionWrapper* iBackupSession;
	// Every backup observer has their own identifying number
	TInt iObserverNumber;
	// Local RTest for this class - needed because global RTest fails in multi-threaded environment
	RTest* iLocalRTest;
	};

//
//	class CBackupTestsStateMachine
//

class CBackupTestsStateMachine : public CActive
	{
public:
	static CBackupTestsStateMachine* NewL();
	CBackupTestsStateMachine();
	~CBackupTestsStateMachine();
public: // From CActive
	void RunL();
	void RunError();
	void DoCancel();
public:
	void Start();
	void End();
	void Complete();
	void StartBackupL();
	void EndBackup(TInt aFileLockChangesExpected, TBool oneTimeOnly);
	void SignalEndBackupL();
	void SingleFileLockTestsL();
public:
	void CreateObserversThreadsL();
	static TInt BackupObserversThreadStartL(TAny* aPtr);
	static TInt FileObserversThreadStartL(TAny* aPtr);
	void CreateBackupObserverInstanceL();
	void CreateFileObserverInstanceL();
public:
	enum StateMachineState
		{EStateMachineStart,
		 ESingleFileTests,		   ENoBackupSessionSingleFileTests,
		 ECloseAllNormalReadOnly,  ECloseAllNormalReadOnlyReturned, 
		 ECloseAllNormalNoAccess,  ECloseAllNormalNoAccessReturned, 
		 ECloseAllDelayReadOnly,   ECloseAllDelayReadOnlyReturned,  
		 ECloseAllDelayNoAccess,   ECloseAllDelayNoAccessReturned,
		 ECloseAllDropFileSession, ECloseAllDropFileSessionReturned,
		 ECloseAllDropBackupObserverSession, ECloseAllDropBackupObserverSessionReturned,
		 EStateMachineEnd};
public:
	void CloseAllStartL(StateMachineState aNextState, MBackupObserver::TFileLockFlags aFlag);
	void CloseAllEndL(StateMachineState aNextState, TInt aExpectedNotifications);
private:
	void ConstructL();
private:
	// The RTest class for this thread
	RTest* iLocalRTest;
	// The "command" session
	CBaBackupSessionWrapper* iBackupSession;	
	// Safety for RArrays
	RMutex iMutex;
	// Array of backup observers
	RPointerArray<CBackupOperationObserver> iBackupObservers;
	// Array of file lock observers
	RPointerArray<CBackupFileObserver> iBackupFileObservers;
	// Used for single file tests
	TInt iEndBackupRecursionCount;
	// Current state of the State Machine
	StateMachineState iState;
	// Used for backup observer threads
	RPointerArray<RThread> iBackupObserverThreads;
	// Used for file observer threads
	RPointerArray<RThread> iFileObserverThreads;
	};

