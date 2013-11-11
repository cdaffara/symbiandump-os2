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
// Flogger (File and Serial logger) server side header
// 
//

/**
 @file
 @internalComponent
*/

#ifndef __COMSDBGSVR_H__
#define __COMSDBGSVR_H__

#include <f32file.h>
#include <comms-infras/commsdebugutility.h>

class CLogMessageBase;
class CIniFileWatcher;
class CIniFileParser;
class CTimeManager;
class CLogStringMessage;
class CLogCommentMessage;
class CSecondaryThread;
class CShutDownMessage;

_LIT(KFloggerIniInROM, "Z:\\resource\\commsdbg.ini");
_LIT(KFloggerIniFile, "commsdbg.ini");
_LIT(KFloggerIniOldFile, "comsdbg.ini");
_LIT(KFloggerIniOldFilePartialPath, ":\\logs\\comsdbg.ini");
_LIT(KFloggerIniDir, "\\logs\\");
_LIT(KFloggerIniRscDir, "\\resource\\");
_LIT(KFloggerDfltIniPartialPath, ":\\logs\\commsdbg.ini");

_LIT8(KFileMedia, "File");
_LIT8(KSerial1Media, "Serial::1");
_LIT8(KSerial2Media, "Serial::2");
_LIT8(KRDebugMedia, "RDebug");
_LIT8(KOSTv2Media, "OSTv2");
_LIT8(KDefaultMedia, "Default");		// This is not supplied in the ini file, but is used to set the media
										// before the media is read from the ini file.
const TInt KMaxMediaStringLength = 9;	//This ties in with the above three strings

const TInt KMaxLoggingPathStringLength = 100;	// Given that "c:\logs\log.txt" = 15, 100 should be enough.
                                                // The whole purpose is just to allow someone to output to a different drive
typedef TBuf8<KMaxLoggingPathStringLength> TFilePath;

_LIT(KFloggerServerPanic, "Comsdbg internal");

typedef TBuf8<KMaxTagLength> TNameTag;

enum TFloggerServerPanics
	{
	ENullMessageInArray,
	ENoLoggingMediaSetUp,
	EBadMessageFunction
	};

enum TFlushStates 
	{
	ENoValue,
	EFlushOn, 
	EFlushOff
	}; ///< Stops us switching between flushing and nonflushing: We can only move from ENoValue to one of the others

/**
 * MLogArrayAccess - Provides an interface for other classes to interact with the log queue in CFileLoggerServer.
 *
 * Overridden by CFileLoggerServer.
 */
class MLogArrayAccess
	{
public:
	virtual TInt AppendAndGiveOwnership(CLogMessageBase* aMessage) = 0;
	virtual void GetFirstMessageAndTakeOwnership(CLogMessageBase*& aMessage) = 0;
	virtual void PutOOMErrorInLog() = 0;
	virtual void SignalCompletionSemaphore() = 0;
	};

#if defined (__WINS__)
class CDebugPortProtocol;
#endif

/**
 * CFileLoggerServer - maintain server state
 *
 * This class is responsible for maintaining the server state. It provides
 * control of the second thread, the ini file change notifier, the current
 * list of valid logs and other ini file settings, and
 * the log queue.
 */
class CFileLoggerServer : public CServer2, public MLogArrayAccess
	{
public:
	static CFileLoggerServer* NewL();
	~CFileLoggerServer();
// CServer
	virtual CSession2* NewSessionL(const TVersion& aVersion,const RMessage2& aMessage) const;
// MLogArrayAccess
	virtual TInt AppendAndGiveOwnership(CLogMessageBase* aMessage);
	virtual void GetFirstMessageAndTakeOwnership(CLogMessageBase*& aMessage);
	virtual void PutOOMErrorInLog();
	virtual void SignalCompletionSemaphore();

	void IniFileChanged(TDesC& aIniFile);
#ifdef _DEBUG
	void __DbgKillTimeManager();
#endif
protected:
	virtual TInt RunError(TInt aError);
private:
	CFileLoggerServer();
	void ConstructL();
	void UpdateMediaL();
	void RePrepareForOOML();
private:
	RPointerArray<CLogMessageBase> iLogMessageArray;
	TBool iArrayHasSpaceForWrite;
	RSemaphore iCompletionSemaphore;     ///< used to signal completion of write when flushing
	RCriticalSection iCriticalSection;         ///< manages atomic access to array of log messages.
	CIniFileWatcher* iIniFileWatcher;
	CIniFileWatcher* iIniOldFileWatcher;
	CIniFileParser* iIniFileParser;
	CTimeManager* iTimeManager;
	CLogCommentMessage* iPreAllocatedErrorMessage;	///< For error mesg to be allocated before all memory is consumed.
	CSecondaryThread* iSecondaryThread;
	RFs iFs;

#if defined (__WINS__)
	CDebugPortProtocol* iDebugWriter;   ///< Win32 debug port support provider
#endif
	};	

class MIniFlushModeAndLogValidQuery;
class MIniLoggingMediaQuery;

/**
 * CFileLogSession -  
 *
 * This class is responsible for servicing the client requests sent. It forms the back-end server side of
 * each client connection to flogger server and implements the ServiceL() to service all client requests.
 */
class CFileLogSession : public CSession2
	{
public:
	static CFileLogSession* NewL(MLogArrayAccess& aArrayAccess, const MIniFlushModeAndLogValidQuery& aLogValidQuery);
	~CFileLogSession();
	// CSession
	virtual void ServiceL(const RMessage2& aMessage);
	//
	void IniFileChanged();
	void SetLoggingOnOffInClient();
private:
	CFileLogSession(MLogArrayAccess& aArrayAccess, const MIniFlushModeAndLogValidQuery& aLogValidQuery);
	void CheckClientHasSetTagsL(const RMessage2& aMessage);
private:
	TNameTag iSubsystem;
	TNameTag iComponent;
	MLogArrayAccess& iArrayAccess;
	const MIniFlushModeAndLogValidQuery& iFlushModeLogValidQuery;
	TBool iLogValid;
	RMessage2 iSetLogMessage;
	TThreadId iThreadId;
	};

/**
 * CSecondaryThread - An active object who's sole purpose is to start the second thread and keep it alive.
 *
 * .
 */
class CSecondaryThread : public CActive
	{
public:
	static CSecondaryThread* NewL(MLogArrayAccess& aArrayAccess, TBool aFlushOn);
	virtual void RunL();
	virtual void DoCancel();
	TInt RunError(TInt aError);
	void SignalRequestSemaphore();
	~CSecondaryThread();
private:
	CSecondaryThread(MLogArrayAccess& aArrayAccess, TBool aFlushOn);
	void ConstructL();
	void StartSecondaryThreadL(TBool aRestarting);
private:
	MLogArrayAccess& iArrayAccess;
	CShutDownMessage* iShutDownMessage;
	RThread iSecondaryThread;
	TBool iFlushingOn;
	};



#endif // __COMSDBGSVR_H__

