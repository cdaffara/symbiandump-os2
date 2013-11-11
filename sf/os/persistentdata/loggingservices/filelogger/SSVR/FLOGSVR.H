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
//

/**
 @file
 @internalTechnology
*/

#ifndef __FLOGSVR_H__
#define __FLOGSVR_H__

#include <f32file.h>
#include <flogger.h>

class CFileLoggerManager;
class CFileLogSessionCounter;

NONSHARABLE_CLASS(CFileLoggerServer) : public CServer2
/**
@internalComponent
*/
	{
public:
	static CFileLoggerServer* NewL();
	~CFileLoggerServer();
// CServer
	virtual CSession2* NewSessionL(const TVersion& aVersion, const RMessage2& aMessage) const;
//
	CFileLogSessionCounter* SessionCounter() const;
	void Error(TInt aError);
private:
	CFileLoggerServer();
	void ConstructL();
public:
	enum {EPriority=950};
private:
	CFileLoggerManager* iLoggerManager;
	CFileLogSessionCounter* iSessionCounter;
	};	

NONSHARABLE_CLASS(CFileLogSession) : public CSession2
/**
@internalComponent
*/
	{
public:
	static CFileLogSession* NewL(CFileLoggerServer* aServer, CFileLoggerManager* aManager);
	~CFileLogSession();
	// CSession
	virtual void ServiceL(const RMessage2& aMessage);
	//
private:
	CFileLogSession(CFileLoggerServer* aServer, CFileLoggerManager* aManager);
	void ConstructL();
	void DispatchMessageL(const RMessage2& aMessage);
	void OpenLogL(TLogFile& aLogFile);
	void CloseLog(TLogFile& aLogFile);
private:
	CFileLoggerServer* iLoggerServer;
	CFileLoggerManager* iLoggerManager;
	CArrayFixFlat<TLogFile>* iOpenLogFiles;
	};

class CShutdownTimer;

NONSHARABLE_CLASS(CFileLogSessionCounter) : public CBase
/**
@internalComponent
*/
	{
public:
	static CFileLogSessionCounter* NewL();
	~CFileLogSessionCounter();
	void CancelTimer();
	void DecrementSessionCount();
	void IncrementSessionCount();
private:
	CFileLogSessionCounter();
	void ConstructL();
private:
	CShutdownTimer* iShutdownTimer;
	TInt iSessionCount;
	};

NONSHARABLE_CLASS(CShutdownTimer) : public CTimer
/**
@internalComponent
*/
	{
public:
	static CShutdownTimer* NewL();
	~CShutdownTimer();
protected:
	CShutdownTimer();
	void RunL();
	};

#endif // __FLOGSVR_H__

