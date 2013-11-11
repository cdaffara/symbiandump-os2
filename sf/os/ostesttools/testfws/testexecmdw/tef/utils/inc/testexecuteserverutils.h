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
* Definitions of classes local to TestExecuteUtils
*
*/



/**
 @file TestExecuteServerUtils.h
*/

#if (!defined __SERVER_UTILS_H__)
#define __SERVER_UTILS_H__
#include <test/testexecuteserverbase.h>
#include <test/testexecutestepbase.h>

class CActiveBase : public CActive
/**
@internalTechnology
@test
*/
	{
public:
	inline 	TRequestStatus& Status();
	inline 	void SetActive();
	inline	void Kick();
	inline	void Prime();
	inline	void Complete(TInt aCode);
	inline	virtual ~CActiveBase();
	inline  TDes& Error();
	inline TBool& TimedOut();
protected:
	inline	CActiveBase();
	TBuf<KMaxTestExecuteNameLength> iError;
	TBool iTimeOut;
	};

class CControlBase : public CActiveBase
/**
@internalTechnology
@test
*/
	{
public:
	inline CControlBase(CTestServer& aServer);
	inline void DoCancel();
	virtual void Stop() =0;
	inline CTestServer& Server();
	inline TDes& Args();
	inline RMessage2& Message();
	inline RThread& WorkerThread();
private:
	CTestServer&						iServer;
	RMessage2							iMessage;
	RThread								iWorkerThread;
	TBuf<KMaxTestExecuteCommandLength>	iArgs;
	};
	
	
class CStepControlBase : public CControlBase
/**
@internalTechnology
@test
*/
	{
public:
	inline CStepControlBase(CTestServer& aServer);
	virtual void StartL(const RMessage2& aMessage,const TDesC& aStepArgs) = 0;
	inline TDes& StepName();
private:
	RMessage2 iMessage;
	RThread iWorkerThread;
	TBuf<KMaxTestStepNameLength> iStepName;
	};

class CBlockControlBase : public CControlBase
/**
@internalTechnology
@test
*/
	{
public:
	inline CBlockControlBase(CTestServer& aServer);
	~CBlockControlBase();
	virtual void StartL(const RMessage2& aMessage,const TDesC& aArgs, const TDesC8& aBlockArrayPtr) = 0;
	virtual TTEFItemArray*	BlockArray() const;

protected:
	virtual void			CreateBlockArrayL( const TDesC8& aBlockArrayPtr );
	virtual HBufC8*			CreateBlockArrayPckgLC();
	
private:
	TTEFItemArray*						iBlockArray;
	RMessage2							iMessage;
	RThread								iWorkerThread;
	TBuf<KMaxTestExecuteCommandLength>	iBlockArgs;
	};

class CStepControl : public CStepControlBase
/**
@internalTechnology
@test
*/
	{
public:
	CStepControl(CTestServer& aServer, const TDesC& aStepName);
	void RunL();
	void StartL(const RMessage2& aMessage,const TDesC& aStepArgs);
	void Stop();
	~CStepControl();
private:
	};

class CBlockControl : public CBlockControlBase
/**
@internalTechnology
@test
*/
	{
public:
	CBlockControl(CTestServer& aServer);
	void RunL();
	void StartL(const RMessage2& aMessage,const TDesC& aArgs, const TDesC8& aBlockArrayPtr);
	void Stop();
	~CBlockControl();
	};

class CWorkerMonitor;
class CWorkerControl;
class CPersistentStepControl : public CStepControlBase
/**
@internalTechnology
@test
*/
	{
public:
	CPersistentStepControl(CTestServer& aServer);
	virtual ~CPersistentStepControl();
	void StartL(const RMessage2& aMessage,const TDesC& aStepArgs);
	void RunL();
	void Stop();
	inline void ChildCompletion();
private:
	CWorkerControl* iWorkerControl;
	CWorkerMonitor* iWorkerMonitor;
	TBool iInitialised;
	};

class CBlockWorkerControl;
class CPersistentBlockControl : public CBlockControlBase
/**
@internalTechnology
@test
*/
	{
public:
	CPersistentBlockControl(CTestServer& aServer);
	virtual ~CPersistentBlockControl();
	void StartL(const RMessage2& aMessage,const TDesC& aArgs, const TDesC8& aBlockArrayPtr);
	void RunL();
	void Stop();
	inline void ChildCompletion();

private:
	CBlockWorkerControl* 	iWorkerControl;
	CWorkerMonitor* 		iWorkerMonitor;
	TBool					iInitialised;
	};

class CWorkerMonitor : public CActiveBase
/**
@internalTechnology
@test
*/
	{
public:
	inline CWorkerMonitor(TRequestStatus& aParentStatus);
	inline virtual ~CWorkerMonitor();
	inline void RunL();
	inline void DoCancel();
private:
	TRequestStatus& iParentStatus;
	};

class CWorkerControl : public CActiveBase
/**
@internalTechnology
@test
*/
	{
public:
	inline CWorkerControl(CTestServer& aServer,TRequestStatus& aParentStatus);
	inline ~CWorkerControl();
	inline void RunL();
	inline void DoCancel();
	inline TRequestStatus& WorkerStatus();
	inline RThread& ControllerThread();
	inline RSemaphore& Semaphore();
	inline CTestServer& Server();
	inline TInt& Result();
	inline TPtrC& Args();
	inline TPtrC& StepName();
	inline TDes& PersistentError();
	//defect 116046
	inline void SetCleanupPtr(CTrapCleanup* aCleanup);
	inline void Cleanup();
	//END defect 116046
private:
	CTestServer& iServer;
	TRequestStatus& iParentStatus;
	RSemaphore iSemaphore;
	TRequestStatus iWorkerStatus;
	RThread iControllerThread;
	TPtrC iArgs;
	TPtrC iStepName;
	TInt iResult;
	TBuf<KMaxTestExecuteNameLength> iPersistentError;
	//defect 116046
	CTrapCleanup* iCleanup;
	//END defect 116046
	};

class CBlockWorkerControl : public CWorkerControl
/**
@internalTechnology
@test
*/
	{
public:
	inline CBlockWorkerControl(CTestServer& aServer,TRequestStatus& iParentStatus );
	inline ~CBlockWorkerControl();
	inline void				SetBlockArray( TTEFItemArray* aBlockArray );
	inline TTEFItemArray*	BlockArray() const;

private:
	TTEFItemArray*			iBlockArray;
	};
	
class CTestSession : public CSession2
/**
@publishedPartner
@test
*/
	{
public:
	IMPORT_C CTestSession();
	IMPORT_C virtual ~CTestSession();
	IMPORT_C virtual void ServiceL(const RMessage2& aMessage);
private:
	CPersistentStepControl*		iPersistentStepControl;
	CPersistentBlockControl*	iPersistentBlockControl;
	TTEFItemArray*				iBlockArray;
	};

#include <test/testexecuteserverutils.inl>
#endif

