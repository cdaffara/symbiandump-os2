/**
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
 @file Server.h
*/

#if (!defined __ET_RFILELOGGER_SERVER_H__)
#define __ET_RFILELOGGER_SERVER_H__
#include <e32base.h>
#include <f32file.h>
#include <test/rfilelogger.h>


class CActiveBase : public CActive
	{
public:
	inline 	TRequestStatus& Status();
	inline 	void SetActive();
	inline	void Kick();
	inline	void Prime();
	inline	void Complete(TInt aCode);
	inline	virtual ~CActiveBase();
protected:
	inline	CActiveBase();
	};

class CLogBuffer : public CBase
	{
public:
	CLogBuffer(HBufC8& aBuffer);
	~CLogBuffer();
	inline const TDesC8& Buf();
	inline static TInt LinkOffset();
private:
	TSglQueLink iLink;
	HBufC8& iLogBuffer;
	};

class CLogServer;
class CLogFileControl : public CActiveBase
	{
public:
	enum TLogFormat{ETxt=10,EXml};
	
	static CLogFileControl* NewL(CLogServer& aParent, const TDesC& aLogFilePath,RFileFlogger::TLogMode aMode);
	~CLogFileControl();
	void RunL();
	inline void AddLogBuffer(CLogBuffer& aBuffer);
	inline const TDesC& LogFile();
	inline void DoCancel();
	inline void AddSession();
	inline void RemoveSession();
	inline TInt SessionCount() const;
	inline TBool QueueEmpty() const;
	void SetLogType(TLogFormat aLogFormat)
	{
		iLogFormat=aLogFormat;
	};

private:
	void ConstructL(RFileFlogger::TLogMode aMode);
	CLogFileControl(CLogServer& aParent, const TDesC& aLogFilePath);
	void WriteTxt(const TDesC8 &aDes);
	void WriteXml(const TDesC8 &aDes);
	
	struct TLogField8
	{
	TBuf8<KMaxSizOfTag> iLogTag8;
	TBuf8<KMaxSizOfString> iLogValue8;	
	};

private:
	CLogServer& iParent;
	TBuf<KMaxLoggerFilePath> iLogFileName;
	RFile iLogFile;
	TSglQue<CLogBuffer> iQueue;
	TInt iSessionCount;
	TBool iTransmitted;
	TLogFormat iLogFormat;
	};

class CLogServer : public CServer2
	{
public:
	static CLogServer* NewL();
	~CLogServer();
	void ControlComplete(CLogFileControl& aControl);
	
	inline RPointerArray<CLogFileControl>& LogControl();
	inline RFs& Fs();

	CSession2* NewSessionL(const TVersion& aVersion,const RMessage2& aMessage) const;
	void SessionClosed();
private:
	CLogServer();
	void ConstructL();
private:
	RFs iFs;
	RPointerArray<CLogFileControl> iControl;
	};

class CLogSession : public CSession2
	{
public:
	inline const CLogServer& LogServer() const;
	CLogSession();
	~CLogSession();
	void ServiceL(const RMessage2& aMessage);
private:
	CLogFileControl* iControl;
	};

#include "Server.inl"

#endif
