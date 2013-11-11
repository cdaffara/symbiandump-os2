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

#include "logservpanic.h"
#include "LogServSqlStrings.h"

//
// RMessage::Panic() also completes the message. This is:
// (a) important for efficient cleanup within the kernel
// (b) a problem if the message is completed a second time
//
void PanicClientL(const RMessage2& aMessage, TLogServPanic aPanic)
	{
	aMessage.Panic(KLogServ, aPanic);
	User::Leave(KLogPanicLeave);
	}

void PanicClient(const RMessage2& aMessage, TLogServPanic aPanic)
	{
	aMessage.Panic(KLogServ, aPanic);
	}

#pragma BullseyeCoverage off

void Panic(TLogServPanic aPanic)
	{
	User::Panic(KLogServ, aPanic);
	}

#pragma BullseyeCoverage on

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
			if	(entry.iSize >= KMaxLogSize)
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
