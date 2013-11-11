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

#ifndef __FLOGMAN_H__
#define __FLOGMAN_H__

#include <f32file.h>
#include <flogger.h>

class CFileLogger;
NONSHARABLE_CLASS(CFileLoggerManager) : public CBase
/**
@internalComponent
*/
	{
public:
	static CFileLoggerManager* NewL();
	~CFileLoggerManager();
//
	void FindOrCreateLogL(TLogFile& aLogFile);
	void CloseLog(const TLogFile& aLogFile);
	void WriteToLogL(const TLogFile& aLogFile, const TDesC8& aBuf);
	void DeleteLogger(CFileLogger* aLogger);
private:
	CFileLoggerManager();
	void ConstructL();
	TInt FindLogger(const TLogFile& aLogFile) const;
private:
	CArrayFixFlat<CFileLogger*> iLogger;	
	RFs iFs;
	};

class CLoggerDeletionTimer;

NONSHARABLE_CLASS(CFileLogger) : public CBase
/**
@publishedAll
@deprecated becoming internal
*/
	{
public:
	static CFileLogger* NewL(CFileLoggerManager* aLoggerManager,TLogFile& aLogFile, RFs& aFs);
	~CFileLogger();
	void Close();
	void WriteLogL(const TDesC8& aBuf);
	TBool StartDeletionTimer();
	void CancelDeletionTimer();
	void DeletionTimerExpired();
	inline void IncrementAccessCount();
	inline void DecrementAccessCount();
	inline TInt AccessCount() const;
	inline TLogFile LogFile() const;	
	inline TBool DeletionTimerActive() const;
private:
	CFileLogger(CFileLoggerManager* aLoggerManager,TLogFile& aLogFile, RFs& aFs);
	void ConstructL(TLogFile& aLogFile);
	void GetFolderAndFileNameL(TFileName& aFolder,TFileName& aFilename) const;
private:
	CLoggerDeletionTimer* iTimer;
	CFileLoggerManager* iLoggerManager;
	RFs iFs;
	RFile iFile;
	TLogFile iLogFile;
	TInt iAccessCount;
	};

NONSHARABLE_CLASS(CLoggerDeletionTimer) : public CTimer
/**
@internalComponent
*/
	{
public:
	static CLoggerDeletionTimer* NewL(CFileLogger* aLogger);
	~CLoggerDeletionTimer();
protected:
	CLoggerDeletionTimer(CFileLogger* aLoggerManager);
	void RunL();
private:
	CFileLogger* iLogger;
	};	

#include "FLOGMAN.INL"

#endif

