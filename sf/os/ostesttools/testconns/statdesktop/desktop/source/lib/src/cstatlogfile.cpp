/*
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




#include "stdafx.h"
#include "CSTATLogFile.h"
#include <statcommon.h>

//----------------------------------------------------------------------------
// our thread-safe mechanism - this must be defined, initialised and destroyed by
// the code using CSTATEngine.  See STATExp.cpp for an example
extern CRITICAL_SECTION CriticalSection;
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
//standard constructor
CSTATLogFile::CSTATLogFile()
: bMessageBox(false), bMessage(false), bWriteToScreen(false), bWriteToFile(false), bScreenshot(false),
	iMessageReporter(NULL)
{
	message = _T("");
	text = _T("");
	*(szLogPrefix) = (char)0;
}

//----------------------------------------------------------------------------
//destructor
CSTATLogFile::~CSTATLogFile()
{
	CloseLogFile();
}


//----------------------------------------------------------------------------
// opens (creates) logfile
int CSTATLogFile::CreateLogFile(const CString& newlogfilename,const CString& defaultPath, const char* prefix, bool append, bool bMessages, bool bFile)
{
	// set flags to write to dialog screen/file
	bWriteToScreen = bMessages;
	bWriteToFile = bFile;

	// construct our log file
	if (bWriteToFile)
	{
		EnterCriticalSection(&CriticalSection);

		// in case it's already open
		CloseLogFile();

		int position;
		UINT openFlags = CFile::modeCreate | CFile::modeWrite | CFile::shareDenyNone;

		// if newlogfilename contains a path, use it otherwise get the default log directory
		position = newlogfilename.ReverseFind('\\');
		if (position != -1)
		{
			Logfilename = newlogfilename;
			CreateDirectory(Logfilename.Left(position), NULL); // try to create directory just in case
		}
		else
		{
			Logfilename=defaultPath;
			if (Logfilename[Logfilename.GetLength() - 1] != _T('\\'))
				Logfilename += _T("\\");

			Logfilename += newlogfilename;
			CreateDirectory(Logfilename, NULL); // try to create directory just in case
		}

		// just path supplied, add a filename
		if (Logfilename[Logfilename.GetLength() - 1] == _T('\\'))
		{
			char FormattedDate[12] = {0};
			Logfilename += _T("STAT");
			Logfilename += _strdate(FormattedDate);		// ...\\filename is now 'STATmm/dd/yy'
			Logfilename.Remove('/');					// ...\\filename is now 'STATmmddyy'
		}

		// add extension if not supplied
		if(Logfilename.ReverseFind('.') == -1)
			Logfilename += _T(".log");

		// if not append, create a new file every time, renaming with (1),(2)...(n) as needed
		CFileFind finder;
		if (!append)
		{
			CString tempLogfile, tempLogstart, tempLogend;
			char szCopycount[15] = {0};
			int iCopycount = 1;

			// save various bits
			tempLogfile = Logfilename;
			position = tempLogfile.ReverseFind('.');
			tempLogstart = tempLogfile.Left(position);
			tempLogend = Logfilename.Right(Logfilename.GetLength() - position);

			while (finder.FindFile(tempLogfile, 0))
			{
				if (position > 0)
				{
					tempLogfile = tempLogstart;
					tempLogfile += "(";
					tempLogfile += itoa(iCopycount++, szCopycount, 10);
					tempLogfile += ")";
					tempLogfile += tempLogend;
				}
				finder.Close();
			}					

			// assign new filename
			Logfilename = tempLogfile;
		}
		else
		{
			// if it already exists, open simple in case other handles are open on it
			if (finder.FindFile(Logfilename, 0))
			{
				openFlags = CFile::modeWrite | CFile::shareDenyNone;
				finder.Close();
			}
		}

		// open the file
		int valid = logfile.Open(Logfilename, openFlags);
		if (valid)
		{
			logfile.SeekToEnd();
			if (prefix && *prefix)
			{
				strcpy(szLogPrefix, "  ");
				strcat(szLogPrefix, prefix);
			}
		}

		LeaveCriticalSection(&CriticalSection);

		if (!valid)
			return LOG_FILE_FAILURE;
	}

	return LOG_FILE_OK;
}

//----------------------------------------------------------------------------
// Specifies the message handler.
void CSTATLogFile::SetMessageReporter(MessageReporter *const messageReporter)
{
	iMessageReporter =	messageReporter;
}

//----------------------------------------------------------------------------
// sets a log message
void
CSTATLogFile::WriteTimeToLog()
{
	if (logfile.m_hFile != CFile::hFileNull)
	{
		EnterCriticalSection(&CriticalSection);

		// get the time
		time_t aclock;
		time(&aclock);
		CString cBuf = asctime(localtime(&aclock));
		int position = cBuf.Find(_T('\n')); // remove the carriage return from the end
		if (position != -1)
			cBuf.SetAt(position, 0);

		Write("--------------------------------------------------\r\n");
		Write(FormatText(cBuf));

		logfile.Flush();

		LeaveCriticalSection(&CriticalSection);
	}
}


//----------------------------------------------------------------------------
// writes CString directly to the log file
void
CSTATLogFile::Write(CString cBuf)
{
	Write(FormatText(cBuf));
}


//----------------------------------------------------------------------------
//writes basic text directly to the log file
void CSTATLogFile::Write(char *szText, ...)
{
	if (logfile.m_hFile != CFile::hFileNull)
	{
		EnterCriticalSection(&CriticalSection);

		static char szMessage[MAX_LOG_MSG_LEN + 1];
		static time_t curTime;

		logfile.SeekToEnd();
		logfile.Write(szLogPrefix, strlen(szLogPrefix));

		// write the date/time
		time ( &curTime );
		strftime ( szMessage, 
					sizeof ( szMessage ), 
					"%d/%m %H:%M:%S ",
					localtime ( &curTime ) );
		logfile.Write(szMessage, strlen(szMessage));

		// write the message
		memset(&szMessage, 0, sizeof(szMessage));
		va_list pCurrent = (va_list)0;
		va_start (pCurrent, szText);
		vsprintf (szMessage, szText, pCurrent);
		va_end (pCurrent);
		logfile.Write(szMessage, strlen(szMessage));

		// add a CRLF if there isn't one already
		if (strlen(szMessage) > 2)
			if (strcmp(szMessage + strlen(szMessage) - 2, "\r\n") != 0)
				logfile.Write("\r\n", 2);

		logfile.Flush();

		LeaveCriticalSection(&CriticalSection);
	}
}


//----------------------------------------------------------------------------
// sets a log message (same as Write() but subject to waiting for previous
// message to be processed before returning (if multi-threaded write to screen
// is enabled)
int
CSTATLogFile::Set(const char* newtext)
{
	Set(-1, newtext, false, false);
	return ITS_OK;
}

//----------------------------------------------------------------------------
// sets a log message (same as Write() but subject to waiting for previous
// message to be processed before returning (if multi-threaded write to screen
// is enabled)
int
CSTATLogFile::Set(int iMsgCode, const char* newtext, bool bMsgBox, bool bScrshot)
{
	// write to screen
	if (bWriteToScreen && (NULL != iMessageReporter))
	{
		// set contents
		if (iMsgCode != -1)
		{
			message = ReturnCodes.GetRetMsg(iMsgCode);
		}
		else
		{
			message.Empty( );
		}

		if ((newtext != NULL) && (*newtext != '\0'))
		{
			text = newtext;
		}

		iMessageReporter->OnMessage( message.operator LPCTSTR(), text.operator LPCTSTR(),
										bMsgBox, bScrshot );
	}

	// write to file
	if (bWriteToFile)
	{
		EnterCriticalSection(&CriticalSection);

		if (iMsgCode != -1)
			Write(FormatText(ReturnCodes.GetRetMsg(iMsgCode)));

		if ((newtext != NULL) && (*newtext != '\0'))
			Write(FormatText(newtext));

		LeaveCriticalSection(&CriticalSection);
	}

	return iMsgCode;
}

//----------------------------------------------------------------------------
//closes the active logfile
void CSTATLogFile::CloseLogFile()
{
	EnterCriticalSection(&CriticalSection);

	if (logfile.m_hFile != CFile::hFileNull)
		logfile.Abort();

	LeaveCriticalSection(&CriticalSection);
}


//----------------------------------------------------------------------------
// PRIVATE METHODS
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
//removes spacing between data when writing to file
char* CSTATLogFile::FormatText(const char* message)
{
	int newlen = 0;
	static char szBuffer[MAX_LOG_MSG_LEN + 3];
	int maxlen = strlen(message);

	if (maxlen > MAX_LOG_MSG_LEN)
		maxlen = MAX_LOG_MSG_LEN;

	(*szBuffer) = (char)0;
	newlen = ToAnsi(message, szBuffer, maxlen);

	// add CR-LF and null-terminate, even if an empty string
	*(szBuffer + newlen) = (char)0;
	strcat(szBuffer, "\r\n");

	return szBuffer;
}


//----------------------------------------------------------------------------
//function to convert unicode to ansi - for logging
int CSTATLogFile::ToAnsi(LPCTSTR szUnicode, LPSTR szBuffer, int nBufLen)
{
#ifdef UNICODE
	return (WideCharToMultiByte(CP_ACP,						// conversion type
								0,							// flags
								szUnicode,					// source
								nBufLen,					// length
								szBuffer,					// dest
								nBufLen,					// length
								NULL,
								NULL));
#else
	strncpy(szBuffer, szUnicode, nBufLen);
	*(szBuffer + nBufLen) = (char)0;
	return strlen(szBuffer);
#endif
}


//////////////////////////////////////////////////////////////////////////////////////////
// Converts a char * to it's Unicode equivalent
//
LPCTSTR
CSTATLogFile::ToUnicode(const char *string)
{
#ifdef UNICODE
	static TCHAR szBuffer[MAX_LOG_MSG_LEN + 1] = {0};
	szBuffer[0] = (TCHAR)0;

    // Convert to UNICODE.
    if (!MultiByteToWideChar(CP_ACP,					// conversion type
							 0,							// flags
							 string,					// source
							 -1,						// length
							 szBuffer,					// dest
							 MAX_LOG_MSG_LEN))			// length
    {
        return _T("Unable to convert ansi to unicode");
    }

    return szBuffer;
#else
	return string;
#endif
}
