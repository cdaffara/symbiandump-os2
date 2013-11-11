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

#ifdef _DEBUG
#undef _MSG_NO_LOGGING
#endif

#include "t_XMLParserLOG.H"


CImLog::CImLog()
	{
	__DECLARE_NAME(_S("CImLog"));
	}

//
// NewL - user describes full file path and file name for the log file
//
CImLog  *CImLog::NewL(const TDesC& aFullFilePath, TImLogOpenMode aImLogOpenMode)
	{
	CImLog* self = new (ELeave) CImLog();
	CleanupStack::PushL(self);
	self->ConstructL(aFullFilePath,aImLogOpenMode);
	CleanupStack::Pop();
	return self;
	}

//
// 2nd phase of construction
//
#ifndef _MSG_NO_LOGGING
void CImLog::ConstructL(const TDesC& aFullFilePath, TImLogOpenMode aImLogOpenMode)
#else
void CImLog::ConstructL(const TDesC& /*aFullFilePath*/, TImLogOpenMode /*aImLogOpenMode*/)
#endif
	{
#ifndef _MSG_NO_LOGGING

	User::LeaveIfError(iFs.Connect());
	TFileName logFile; 
	logFile.Append(aFullFilePath);

	if (aImLogOpenMode==EAppend)
		{
		TInt err=iFile.Open(iFs,logFile,EFileWrite|EFileShareAny);
		if (err==KErrNotFound)
			{
			iFile.Create(iFs,logFile,EFileWrite|EFileShareAny);
			}
		else if (err != KErrNone)
			{	
			User::Leave(err);
			}
		TInt position=1;
		User::LeaveIfError(iFile.Seek(ESeekEnd,position));
		}
	else
		{
		// If the log directory has not been created, no log will be created. OK
		User::LeaveIfError(iFile.Replace(iFs,logFile,EFileWrite|EFileShareAny));
		}

#endif // _MSG_NO_LOGGING
	}

CImLog::~CImLog()
	{
#ifndef _MSG_NO_LOGGING

	iFile.Close();
	iFs.Close();

#endif // _MSG_NO_LOGGING
	}

//
// Log an error message
//
#ifndef _MSG_NO_LOGGING
void CImLog::AppendError(const TDesC8& aErrorMessage, TInt aErrorCode)
#else
void CImLog::AppendError(const TDesC8& /*aErrorMessage*/, TInt /*aErrorCode*/)
#endif
	{
#ifndef _MSG_NO_LOGGING
	WriteToLog(KLogErrorString, &aErrorMessage, aErrorCode);
#endif
	}

//
// Log a comment
//
#ifndef _MSG_NO_LOGGING
void CImLog::AppendComment(const TDesC8& aMessage)
#else
void CImLog::AppendComment(const TDesC8& /*aMessage*/)
#endif
	{
#ifndef _MSG_NO_LOGGING
	WriteToLog(KLogCommentString, &aMessage);
#endif
	}

//
// Construct a string with date, a message type indicator (out, in, error) sort out formatting
//
#ifndef _MSG_NO_LOGGING
void CImLog::BuildLogString(const TDesC8& aFormat,const TDesC8& aOutPut, TBool aWriteCrLf)
#else
void CImLog::BuildLogString(const TDesC8& /*aFormat*/,const TDesC8& /*aOutPut*/, TBool /*aWriteCrLf*/)
#endif
	{
#ifndef _MSG_NO_LOGGING

	TPtrC8 logString(aOutPut);

	for(;;)
		{
		TInt crLfPos=logString.Find(KLogCR);
		if(crLfPos==KErrNotFound)
			{
			break;
			}
		else
			{
			TPtrC8 line;
			if (aWriteCrLf && (crLfPos == logString.Size() - 2))
				{
				line.Set(logString);
				}
			else
				{
				line.Set(logString.Left(crLfPos));
				}
			WriteToLog(TRefByValue<const TDesC8>(aFormat),&line);
			logString.Set(logString.Mid(crLfPos+2));
			}
		}

	if(logString.Length())
		{
		WriteToLog(TRefByValue<const TDesC8>(aFormat),&logString);
		}

#endif // _MSG_NO_LOGGING
	}

//
// Write information to the log file
//
#ifndef _MSG_NO_LOGGING
void CImLog::WriteToLog(TRefByValue<const TDesC8> aFmt,...)
#else
void CImLog::WriteToLog(TRefByValue<const TDesC8> /*aFmt*/,...)
#endif
	{
#ifndef _MSG_NO_LOGGING

	TBuf8<1024> buf;
	TTime date;
	date.HomeTime();
	TBuf<18> dateString;
	TRAPD(error, date.FormatL(dateString,(KLogDateFormatString)));
	if(error)
	{
		dateString.Copy(_L("Invalid Date"));
	}
	buf.Copy(dateString);
	
	VA_LIST list;
	VA_START(list,aFmt);

	buf.AppendFormatList(aFmt,list);
	buf.Append(KLogCR);
	iFile.Write(buf);

#endif // _MSG_NO_LOGGING
	}
