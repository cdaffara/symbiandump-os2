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
// Flogger File and Serial logger header
// 
//

/**
 @file
 @internalComponent
*/
 
#ifndef __COMSDBGWRITER_H__
#define __COMSDBGWRITER_H__

#include <e32base.h>
#include <f32file.h>
#include <s32file.h>
#include <d32comm.h>
#include <platform/opensystemtrace.h>

#include "comsdbgsvr.h"

_LIT8(KTimeFormat,"#Time = hh:mm:ss\r\n");
const TInt KHourOffset = 8;
const TInt KMinuteOffset = 11;
const TInt KSecondOffset = 14;
const TInt KTimeStringLength = 18;

class MLogMessageProtocol
	{
public:
	virtual void ClearLog(const TFullName& aName) = 0;
	virtual void SetTimeL(const TTime& aTime) = 0;
	virtual void LogString(const TDesC8& aLogString, const TDesC8& aSubSystem, const TDesC8& aComponent, const TThreadId& aThreadId) = 0;
	virtual void LogBinaryDump(const TDesC8& aBinaryString, const TDesC8& aSubSystem, const TDesC8& aComponent) = 0;
	virtual void LogComment(const TDesC8& aComment) = 0;
	virtual void MediaUpdate(const TDesC8& aMediaSetting, const TBool aForceFlush, const TDesC8& aLogPathSetting) = 0;
	virtual void ShutDown() = 0;
	};


class CLoggerMediaBase;
class MLogArrayAccess;

/**
 * CLogManager - second/consumer/slave thread
 *
 * This class is responsible for implementing the second thread which
 * removes items from the logging queue and writes them to disk.
 * Maintains a pointer to the server's log queue, and a pointer to the
 * media base in use.
 */
class CLogManager : public CBase, public MLogMessageProtocol
	{
public:
	~CLogManager();
	static CLogManager* NewL(MLogArrayAccess& aArrayAccess);
	static TInt ThreadEntryPoint(TAny* aPtr);
	static void DoRunThreadL(MLogArrayAccess& aArrayAccess);
	//from MLogMessageProtocol
	virtual void ClearLog(const TFullName& aName);
	virtual void SetTimeL(const TTime& aTime);
	virtual void LogString(const TDesC8& aLogString, const TDesC8& aSubSystem, const TDesC8& aComponent, const TThreadId& aThreadId);
	virtual void LogBinaryDump(const TDesC8& aBinaryString, const TDesC8& aSubSystem, const TDesC8& aComponent);
	virtual void LogComment(const TDesC8& aComment);
	virtual void MediaUpdate(const TDesC8& aMediaSetting,const TBool aForceFlushOn, const TDesC8& aLogPathSetting);
	virtual void ShutDown();
private:
	void WriteDateIntoLog(TBool firstTime);
	CLogManager(MLogArrayAccess& aArrayAccess);
	void ConstructL();
	void ConvertToNarrow(const TDesC& aSource, TDes8& aDestination);
	void DoMediaUpdateL(const TDesC8& aMediaSetting,const TBool aForceFlushOn, const TDesC8& aLogPathSetting);
	void DoStart();
private:
	MLogArrayAccess& iArrayAccess;
	CLoggerMediaBase* iLogger;
	TBuf8<KTimeStringLength> iTimeString;
	TBuf8<KMaxMediaStringLength> iCurrentMediaSetting;
	TUint iTicksSinceLastLog;
	TBool iShutDown;
	TTimeIntervalDays iCurrentDate;			///< store date as # days since 1st Jan 0 AD so we can check when date changes
	TBool iLoggingEnabled;
	TFilePath iLogPath;	// location of output. stored for comparison with next ini files changes
	};

class CLoggerMediaBase : public CBase
	{
public:
	virtual void LogString(const TDesC8& aString) = 0;
	virtual void ClearLogL() = 0;
	virtual void FlushLogL() = 0;
	virtual void LogBinaryDump(const TDesC8& aBinaryString, const TDesC8& aSubSystem, const TDesC8& aComponent) = 0;
	virtual void SetForceFlush(const TBool aOn) = 0;
	TBool iForceBufferFlushAlways;
	};

class CSerialWriter : public CLoggerMediaBase
	{
public:
	~CSerialWriter();
	static CSerialWriter* NewL();
	virtual void LogString(const TDesC8& aString);
	virtual void ClearLogL();
	virtual void FlushLogL();
	virtual void LogBinaryDump(const TDesC8& aBinaryString, const TDesC8& aSubSystem, const TDesC8& aComponent);
	virtual void SetForceFlush(const TBool aOn);
	TInt LoadDevices();
	TInt OpenPort(TInt aPortNumber);
	TInt SetPortConfig();
private:
	void ConstructL();
private:
	RBusDevComm iSerialPort;
	RTimer iTimeoutTimer;
	TInt iInvalidCounter;
	};

class CFileWriter : public CLoggerMediaBase
	{
public:
	~CFileWriter();
	static CFileWriter* NewL();
	virtual void LogString(const TDesC8& aString);
	virtual void ClearLogL();
	virtual void FlushLogL();
	virtual void LogBinaryDump(const TDesC8& aBinaryString, const TDesC8& aSubSystem, const TDesC8& aComponent);
	virtual void SetForceFlush(const TBool aOn);
	void SetLogPath(const TDesC8& aLogPathSetting);
private:
	void ConstructL();
	void DoFlushBufferToFileL();
	void DoLogBinaryDumpL(const TDesC8& aBinaryString, const TDesC8& aSubSystem, const TDesC8& aComponent);
private:
	RFs iFs;
	HBufC8* iHBuf;
	TBool iForceBufferFlushAlways;
	TFilePath iLogPath;	// We can specifie a path in commsdbg.ini
	TName iLogFileName;	
	};

class CRDebugWriter : public CLoggerMediaBase
	{
public:
	~CRDebugWriter();
	static CRDebugWriter* NewL();
	virtual void LogString(const TDesC8& aString);
	virtual void ClearLogL();
	virtual void FlushLogL();
	virtual void LogBinaryDump(const TDesC8& aBinaryString, const TDesC8& aSubSystem, const TDesC8& aComponent);
	virtual void SetForceFlush(const TBool aOn);
	};

class COstv2Writer : public CLoggerMediaBase
	{
public:
	static COstv2Writer* NewL();

	COstv2Writer() : iContext(0x101fe69e, TRACE_STATE) {};
	
	virtual void LogString(const TDesC8& aString);
	virtual void ClearLogL();
	virtual void FlushLogL();
	virtual void LogBinaryDump(const TDesC8& aBinaryString, const TDesC8& aSubSystem, const TDesC8& aComponent);
	virtual void SetForceFlush(const TBool aOn);
	
private:
	TTraceContext iContext;
	};

#if defined (__WINS__)

class CDebugPortWriter : public CLoggerMediaBase
	{
public:
	static CDebugPortWriter* NewL();
	virtual void LogString(const TDesC8& aString);
	virtual void ClearLogL();
	virtual void FlushLogL();
	virtual void LogBinaryDump(const TDesC8& aBinaryString, const TDesC8& aSubSystem, const TDesC8& aComponent);
	virtual void SetForceFlush(const TBool aOn);
private:
	};

class CDebugPortProtocol : public MLogMessageProtocol
	{
public:
	static CDebugPortProtocol* NewL();
	~CDebugPortProtocol();
	virtual void ClearLog(const TFullName& aName);
	virtual void SetTimeL(const TTime& aTime);
	virtual void LogString(const TDesC8& aLogString, const TDesC8& aSubSystem, const TDesC8& aComponent, const TThreadId& aThreadId);
	virtual void LogBinaryDump(const TDesC8& aBinaryString, const TDesC8& aSubSystem, const TDesC8& aComponent);
	virtual void LogComment(const TDesC8& aComment);
	virtual void MediaUpdate(const TDesC8& aMediaSetting, const TBool aForceFlushOn, const TDesC8& aLogPathSetting);
	virtual void ShutDown();
private:
	void ConstructL();
	CDebugPortWriter* iDebugWriter;
	};

#endif

#endif //__COMSDBGWRITER_H__
