// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

// User includes
#include "SchLogger.h"

#ifdef __SCHLOGGING__




//
// ------> CSheduleServerLog (source)
//

CSheduleServerLog::CSheduleServerLog()
	{
	}

CSheduleServerLog::~CSheduleServerLog()
	{
	iFile.Close();
	iFs.Close();
	}

void CSheduleServerLog::ConstructL(const TDesC& aLogFileName)
	{
	// Literal constants
	_LIT(KScheduleServerLoggingDirectoryE, "E:\\Logs\\SchSvr\\");
	_LIT(KScheduleServerLoggingDirectoryD, "D:\\Logs\\SchSvr\\");
	_LIT(KScheduleServerLoggingDirectory, "_:\\Logs\\SchSvr\\");
		
	User::LeaveIfError(iFs.Connect());
	TFileName logFile;

	// Log to drive D if possible
	TInt error = iFs.MkDirAll(KScheduleServerLoggingDirectoryE);
	if	(error == KErrAlreadyExists || error == KErrNone)
		{
		logFile.Append(KScheduleServerLoggingDirectoryE);
		}
	else
		{
		error = iFs.MkDirAll(KScheduleServerLoggingDirectoryD);
		if	(error == KErrAlreadyExists || error == KErrNone)
			{
			logFile.Append(KScheduleServerLoggingDirectoryD);
			}
		else
			{
			// system drive directory
			TBuf<15> loggingDirOnSysDrive(KScheduleServerLoggingDirectory);
			loggingDirOnSysDrive[0] = 'A' + static_cast<TInt>(RFs::GetSystemDrive());
			
			error = iFs.MkDirAll(loggingDirOnSysDrive);
			if	(error != KErrAlreadyExists && error < KErrNone)
				User::Leave(error);
			logFile.Append(loggingDirOnSysDrive);
			}
		}

	// Open log file
	TParsePtrC parser(aLogFileName);
	logFile.Append(parser.Name());

#ifdef __WINS__
	logFile.Append(_L(".WINS"));
#elif __MARM__
	logFile.Append(_L(".MARM"));
#endif
#ifdef _DEBUG
	logFile.Append(_L(".UDEB"));
#else
	logFile.Append(_L(".UREL"));
#endif
	logFile.Append(parser.Ext());
	logFile.Append(_L(".TXT"));

	User::LeaveIfError(iFile.Replace(iFs, logFile, EFileStreamText | EFileShareAny));
	SeekEnd();

	NewLine();
	NewLine();
	_LIT(KNewLogEntry, "=== NEW LOG ===");
	Write(KNewLogEntry);
	NewLine();
	}

CSheduleServerLog* CSheduleServerLog::NewL(const TDesC& aLogFileName)
	{
	CSheduleServerLog* self = new(ELeave) CSheduleServerLog();
	CleanupStack::PushL(self);
	self->ConstructL(aLogFileName);
	CleanupStack::Pop();
	return self;
	}

//
//
//

void CSheduleServerLog::Log(TRefByValue<const TDesC> aFmt,...)
	{
	VA_LIST list;
	VA_START(list, aFmt);

	TBuf<1024> buf;
	buf.AppendFormatList(aFmt, list);
	WriteWithTimeStamp(buf);
	NewLine();
	}

void CSheduleServerLog::LogList(TRefByValue<const TDesC> aFmt, VA_LIST aList)
	{
	TBuf<1024> buf;
	buf.AppendFormatList(aFmt, aList);
	WriteWithTimeStamp(buf);
	NewLine();
	}

void CSheduleServerLog::SeekEnd()
	{
	TInt pos;
	iFile.Seek(ESeekEnd, pos);
	}

//
//
//

void CSheduleServerLog::Write(const TDesC& aText)
	{
	HBufC8* buf = HBufC8::New(aText.Length());
	if	(!buf)
		return;

	TPtr8 pBuf(buf->Des());
	
	const TInt KTextLength = aText.Length();
	for(TInt i=0; i<KTextLength; i++)
		{
		if	(aText[i] < 256)
			pBuf.Append(aText[i]);
		}
	
	// Ignore errors
	TInt error = iFile.Write(pBuf);
	delete buf;
	error = iFile.Flush();	
	}

void CSheduleServerLog::Write(const TDesC& aFmt, VA_LIST& aList)
	{
	TDes* buf = new TBuf<1000>;
	if	(!buf)
		return;
	buf->AppendFormatList(aFmt, aList);
	Write(*buf);
	delete buf;
	}

void CSheduleServerLog::WriteWithTimeStamp(const TDesC& aText)
	{
	TBuf<200> buf;
	TTime now;
	now.HomeTime();
	TDateTime dateTime;
	dateTime = now.DateTime();
	buf.Format(_L("%02d.%02d:%02d:%06d "), dateTime.Hour(), dateTime.Minute(), dateTime.Second(), dateTime.MicroSecond());
	Write(buf);
	Write(aText);
	}

void CSheduleServerLog::NewLine()
	{
	TBuf<2> buf;
	buf.Append(0x0D);
	buf.Append(0x0A);
	Write(buf);
	}


#endif
