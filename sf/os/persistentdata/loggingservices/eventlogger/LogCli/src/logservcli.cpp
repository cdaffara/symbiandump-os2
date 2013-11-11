// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "logservcli.h"

// System includes
#include <e32math.h>
#include <s32mem.h>

// User includes
#include "LogServShared.h"

static TInt StartServer()
//
// Start the server process/thread which lives in an EPOCEXE object
//
	{
	const TUidType serverUid(KNullUid,KNullUid,KServerUid3);

	//
	// EPOC is easy, we just create a new server process. Simultaneous launching
	// of two such processes should be detected when the second one attempts to
	// create the server object, failing with KErrAlreadyExists.
	//
	RProcess server;
	TInt r=server.Create(KLogServerName,KNullDesC,serverUid);
	if (r == KErrNone)
		server.SetPriority(EPriorityForeground);
		
	if (r!=KErrNone)
		return r;
	
	TRequestStatus stat;
	server.Rendezvous(stat);
	if (stat!=KRequestPending)
		server.Kill(0);		// abort startup
	else
		server.Resume();	// logon OK - start the server
	User::WaitForRequest(stat);		// wait for start or death
	// we can't use the 'exit reason' if the server panicked as this
	// is the panic 'reason' and may be '0' which cannot be distinguished
	// from KErrNone
	r=(server.ExitType()==EExitPanic) ? KErrGeneral : stat.Int();
	server.Close();
	return r;

	}

// Initialise the operation and view ids
RLogSession::RLogSession()
: iOperationId(KLogNullOperationId + 1), iViewId(KLogNullViewId + 1)
	{
	}

TInt RLogSession::Connect()
//
// Connect to the server, attempting to start it if necessary
//
	{
	TInt retry=2;
	for (;;)
		{
		TInt r=CreateSession(KLogServerFullName,TVersion(0,0,0));
		if (r!=KErrNotFound && r!=KErrServerTerminated)
			return r;
		if (--retry==0)
			return r;
		r=StartServer();
		if (r!=KErrNone && r!=KErrAlreadyExists)
			return r;
		}
	}


void RLogSession::Send(TInt aType, const TIpcArgs& aArgs, TRequestStatus& aStatus) const
	{
	SendWithRetryAsync(aType, aArgs, aStatus);
	}

TInt RLogSession::Send(TInt aType, const TIpcArgs& aArgs) const
	{
	return SendWithRetry(aType, aArgs);
	}

TInt RLogSession::SendWithRetry(TInt aType, const TIpcArgs& aParam) const
	{
	TInt ret = SendReceive(aType, aParam);
	if (ret < KErrNone)
		{
		if(ret == KErrServerTerminated || ret == KErrNotFound)
			{
			// Try to reconnect
			const_cast<RLogSession&>(*this).Close();
			ret = const_cast<RLogSession&>(*this).Connect();
			if(ret == KErrNone)
				{
				// Try to resend
				ret = SendReceive(aType, aParam);
				}
			}
		}
	return ret;
	}


void RLogSession::SendWithRetryAsync(TInt aType, const TIpcArgs& aParam, TRequestStatus& aStatus) const
	{
	SendReceive(aType, aParam, aStatus);
	}











#ifdef LOGGING_ENABLED

const TInt KLogEngLogBufferSize = 256;

void Log::New()
	{
	_LIT(KNewLogText, "===== NEW LOG =====");
	//
	RFileLogger logger;
	TInt ret=logger.Connect();
	if	(ret==KErrNone)
		{
		logger.CreateLog(KLogFolder, KLogFileName, EFileLoggingModeOverwrite);
		logger.Write(KNewLogText);
		}
	logger.Close();
	}

void Log::Write(const TDesC& aText)
	{
	PruneLogFile();

	RFileLogger logger;
	TInt ret=logger.Connect();
	if (ret==KErrNone)
		{
		logger.SetDateAndTime(EFalse,EFalse);
		logger.CreateLog(KLogFolder, KLogFileName,EFileLoggingModeAppend);
		TBuf<KLogEngLogBufferSize> buf;
		
		// The debug log uses hometime rather than UTC for its timestamps. This is
		// purely a debugging aid.
		TTime now;
		now.HomeTime();
		
		TDateTime dateTime;
		dateTime = now.DateTime();
		buf.Format(KTimeFormat,dateTime.Hour(),dateTime.Minute(),dateTime.Second(),dateTime.MicroSecond());
		buf.AppendFormat(KTextFormat,&aText);

		logger.Write(buf);
		}

	logger.Close();
	}

void Log::WriteFormat(TRefByValue<const TDesC> aFmt,...)
	{
    VA_LIST list;
    VA_START(list,aFmt);

	PruneLogFile();

	TBuf<2*KLogEngLogBufferSize> buf;
	buf.SetMax();
	buf.FillZ();
	
	// The debug log uses hometime rather than UTC for its timestamps. This is
	// purely a debugging aid.
	TTime now;
	now.HomeTime();
	
	TDateTime dateTime;
	dateTime = now.DateTime();
	buf.Format(KTimeFormat,dateTime.Hour(),dateTime.Minute(),dateTime.Second(),dateTime.MicroSecond());
	buf.AppendFormatList(aFmt, list );

	RFileLogger logger;
	TInt ret=logger.Connect();
	if (ret==KErrNone)
		{
		logger.SetDateAndTime(EFalse,EFalse);
		logger.CreateLog(KLogFolder, KLogFileName,EFileLoggingModeAppend);
		logger.Write(buf);
		}

	logger.Close();
	}

void Log::PruneLogFile()
	{
	const TInt KMaxLogSize = 1024 * 500;
	//
	_LIT(KBaseFolder, "_:\\Logs\\");
	TFileName fileName(KBaseFolder);
	fileName[0] = 'A' + static_cast<TInt>(RFs::GetSystemDrive());
	fileName.Append(KLogFolder);
	fileName.Append(KPathDelimiter);
	fileName.Append(KLogFileName);
	//
	RFs fsSession;
	if	(fsSession.Connect() == KErrNone)
		{
		TEntry entry;
		if	(fsSession.Entry(fileName, entry) == KErrNone)
			{
			// Check size and delete if its too big
			if (entry.iSize >= KMaxLogSize)
				{
				TInt fileDeleteErr=fsSession.Delete(fileName); 

				// If a debug build - record error
				#ifdef _DEBUG
					if (fileDeleteErr != KErrNone)
						{
						RDebug::Print(_L("Log::PruneLogFile - Failed to delete file. Error = %d"), fileDeleteErr);
						}
				#else
					(void)fileDeleteErr;
				#endif
				}	
			}
		}
	fsSession.Close();
	}

#endif

