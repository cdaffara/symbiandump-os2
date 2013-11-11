/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
* testutil - server classes
*
*/


/**
 @file 
 @test
 @internalComponent
*/

#include <e32base.h>
#include "testutilfilewatcher.h"

class CTestUtilServer;

class MTimeoutClient
	{
public:
	/** Client's implementation gets called on timer expiry */
	virtual void HandleTimeout() = 0;
	};

/** Simple timer class which calls a callback on timer expiry */
class CGenericTimer : public CTimer
	{
public:
	static CGenericTimer* NewL(MTimeoutClient& aClient);
private:
	CGenericTimer(MTimeoutClient& aClient);
	void RunL();
private:
	MTimeoutClient& iClient;
	};

class CTestFileDetector : public CActive, public MTimeoutClient
	{
public:
	static CTestFileDetector* NewL(const RMessage2& iMessage, RFs& aFs);
	CTestFileDetector(const RMessage2& iMessage, RFs& aFs);
	~CTestFileDetector();
	void DetectFile();
private:
	void ConstructL();
	virtual void HandleTimeout();
	void RunL();
	void DoCancel();
	void CheckAndComplete();
private:
	RFs& iFs;
	CGenericTimer* iTimer;
	HBufC* iFileName;
	TInt iTimeInterval;
	const RMessage2& iMessage;
	};

class CTestUtilSession : public CSession2
	{
public:
	CTestUtilSession();
	void CreateL();
private:
	~CTestUtilSession();
	inline CTestUtilServer& Server();
	void ServiceL(const RMessage2& aMessage);
	void ServiceError(const RMessage2& aMessage,TInt aError);
private:
	RArray<RFile> iLockedFileHandles;
	CFileWatcher* iFileWatcher;
	CTestFileDetector* iDetector;
	};


inline CTestUtilServer& CTestUtilSession::Server()
	{return *static_cast<CTestUtilServer*>(const_cast<CServer2*>(CSession2::Server()));}
