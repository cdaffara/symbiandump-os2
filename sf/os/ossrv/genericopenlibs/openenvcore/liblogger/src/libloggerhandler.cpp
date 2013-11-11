/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Contained libc logger class implementation.
 *
*/


// INCLUDE FILES
#include <f32file.h>

#ifdef SYMBIAN_FILE_LOGGER
#include <flogger.h>
#endif

#include "libloggerhandler.h"

// CONSTANTS
#define LOG_MESSAGE_TYPE_INFO "INFO"
#define LOG_MESSAGE_TYPE_MINOR "MINOR"
#define LOG_MESSAGE_TYPE_MAJOR "MAJOR"
#define LOG_MESSAGE_TYPE_CRITICAL "CRITICAL"


#define MAX_LOG_STR_LEN 512 /* Overall size of any logging string size, including timestamp and others */
#define MAX_DATE_TIME_LEN 32

#define DUMP_STR_LENGTH 16

#ifdef SYMBIAN_FILE_LOGGER
_LIT(KLogFileLocation,"libc"); // c:\logs\libc
#else
_LIT(KLogFileLocation,"c:\\logs\\libc\\");
_LIT8(KDumpSpace,"                   %04x ");
#endif

const char KMsgType[4][16] =
	{
		LOG_MESSAGE_TYPE_INFO,
		LOG_MESSAGE_TYPE_MINOR,
		LOG_MESSAGE_TYPE_MAJOR,
		LOG_MESSAGE_TYPE_CRITICAL
	};

// ============================ LOCAL FUNCTIONS ===============================

// strlen is implemented over here, just to remove the dependancy from libc/estdlib

// -----------------------------------------------------------------------------
// _lstrlen
// find the string length.
// -----------------------------------------------------------------------------
//
LOCAL_C TInt _lstrlen(const char *str)

    {
	const char *s;
	for (s = str; *s; ++s)
	    {
	    }
	return(s - str);
    }
// -----------------------------------------------------------------------------
// BitPosition
// Find the bit position within the number
// -----------------------------------------------------------------------------
//
LOCAL_C TInt BitPosition(TInt aNumber)
    {
    TInt pos = 0;
    while(!(aNumber & 1))
        {
        aNumber = (aNumber >> 1);
        pos++;
        }
    // only first 4 bits are used.
    if ( pos < 0 || pos >= 4)
        {
        // crosses the boundary.
        pos = 0;
        }
    return pos;
    }


// -----------------------------------------------------------------------------
// CLibLogger::GetLogFilename
// Get the log file name.
// -----------------------------------------------------------------------------
//
void CLibLogger::GetLogFilename(TDes& aFileName)
{
	_LIT(KFileNameFormat, "Mrt_%S.txt");
    RProcess currentProcess;
    TName processName = currentProcess.Name();
#ifdef SYMBIAN_FILE_LOGGER
    aFileName.Format(KFileNameFormat, &processName);
#else
    aFileName.Copy(KLogFileLocation);
    aFileName.AppendFormat(KFileNameFormat, &processName);
#endif
    currentProcess.Close();
}

// -----------------------------------------------------------------------------
// CLibLogger::WriteMessage
// Write message to the log file
// -----------------------------------------------------------------------------
//
TInt CLibLogger::WriteMessage(const TDesC8& aMessage)
{
	TFileName logFileName;
	GetLogFilename(logFileName);
	TInt err = KErrNone;
#ifdef SYMBIAN_FILE_LOGGER
    RFileLogger::Write(KLogFileLocation, logFileName, EFileLoggingModeAppend, aMessage);
#else
	RFs fsSession; /* file server */
	RFile iFile;
	err = CLibLogger::CreateFileSession(fsSession, iFile);
	if(KErrNone != err)
	{
		/* Error : Unable to initialize the file session */
		return err;
	}

	/* write the buffer to the file */
	err = iFile.Write(aMessage);
	if(KErrNone != err)
	{
		/* Error : Unable to initialize the file session */
		iFile.Close();
		fsSession.Close();
		return err;
	}

	/* flush the buffer */
	iFile.Flush(); /* commit the write. */

	/* close the session and resources */
	iFile.Close();
	fsSession.Close();
#endif
    return err;
}

// -----------------------------------------------------------------------------
// CLibLogger::LogMessage
// Logging the internal messages.
// -----------------------------------------------------------------------------
//
int CLibLogger::LogMessage(TLibTraceMessageType aLogMessageType,
							char *aFileName,
							int aLine,
							char *aFormat,
							VA_LIST& aMarkerList)
{
	/* checking for error inputs */
	if(NULL == aFormat)
	{
		/* Error : Check the input parameter. */
		return -1;
	}

	/* trying to fetch the local time */
	TBuf8<MAX_LOG_STR_LEN> lBuf;
	lBuf.FillZ();
	lBuf.SetLength(0);

#ifdef SYMBIAN_FILE_LOGGER
	/* formulate the time stamp with log type */
	_LIT8(KFormat, "(%s : %d) [%s] - ");
	lBuf.Format(KFormat,
		aFileName,
		aLine,
		KMsgType[BitPosition(aLogMessageType)]
		);
#else
	TTime lCurTime;
	lCurTime.HomeTime();
	TDateTime lTimeStamp = lCurTime.DateTime();

	/* formulate the time stamp with log type */
	_LIT8(KFormat, "%2d/%2d/%d-%2d:%2d:%2d.%3d - (%s : %d) [%s] - ");
	lBuf.Format(KFormat,
		lTimeStamp.Day(),
		(int)(lTimeStamp.Month() + 1),
		lTimeStamp.Year(),
		lTimeStamp.Hour(),
		lTimeStamp.Minute(),
		lTimeStamp.Second(),
		(int)((lTimeStamp.MicroSecond())/1000),
		aFileName,
		aLine,
		KMsgType[BitPosition(aLogMessageType)]
		);
#endif // SYMBIAN_FILE_LOGGER

	TPtrC8 lPtr((TUint8*)aFormat, _lstrlen(aFormat));
	lBuf.AppendFormatList(lPtr, aMarkerList);

	/* new line */
	_LIT8(KNewLine, "\r\n");
	lBuf.Append(KNewLine);
    WriteMessage(lBuf);
	return lBuf.Length();
}

// -----------------------------------------------------------------------------
// CLibLogger::LogMessage
// Logging the internal messages.
// -----------------------------------------------------------------------------
//
int CLibLogger::LogMessage(char *aFileName, int aLine)
{
	/* trying to fetch the local time */
	TBuf8<MAX_LOG_STR_LEN> lBuf;
	lBuf.FillZ();
	lBuf.SetLength(0);

#ifdef SYMBIAN_FILE_LOGGER
	/* formulate the time stamp with log type */
	_LIT8(KFormat, "(%s : %d) ");
	lBuf.Format(KFormat,
		aFileName,
		aLine);
#else
	TTime lCurTime;
	lCurTime.HomeTime();
	TDateTime lTimeStamp = lCurTime.DateTime();

	/* formulate the time stamp with log type */
	_LIT8(KFormat, "%2d/%2d/%d-%2d:%2d:%2d.%3d - (%s : %d) ");
	lBuf.Format(KFormat,
		lTimeStamp.Day(),
		(int)(lTimeStamp.Month() + 1),
		lTimeStamp.Year(),
		lTimeStamp.Hour(),
		lTimeStamp.Minute(),
		lTimeStamp.Second(),
		(int)((lTimeStamp.MicroSecond())/1000),
		aFileName,
		aLine);
#endif // SYMBIAN_FILE_LOGGER
    WriteMessage(lBuf);
	return lBuf.Length();
}


// -----------------------------------------------------------------------------
// CLibLogger::LogMessage
// Logging the internal messages.
// -----------------------------------------------------------------------------
//
int CLibLogger::LogMessage(TLibTraceMessageType aLogMessageType,
							char *aFormat, 
							VA_LIST& aMarkerList)
{
	/* checking for error inputs */
	if(NULL == aFormat)
	{
		/* Error : Check the input parameter. */
		return -1;
	}

	/* trying to fetch the local time */
	TBuf8<MAX_LOG_STR_LEN> lBuf;
	lBuf.FillZ();
	lBuf.SetLength(0);

	/* formulate the time stamp with log type */
	_LIT8(KFormat, "[%s] - ");
	lBuf.Format(KFormat, 
		KMsgType[BitPosition(aLogMessageType)]
		);

	TPtrC8 lPtr((TUint8*)aFormat, _lstrlen(aFormat));
	lBuf.AppendFormatList(lPtr, aMarkerList);

	/* new line */
	_LIT8(KNewLine, "\r\n");
	lBuf.Append(KNewLine);
    WriteMessage(lBuf);
	return lBuf.Length();
}


// -----------------------------------------------------------------------------
// CLibLogger::DumpFormatMessage
// Dump the internal message in hex format.
// -----------------------------------------------------------------------------
//
int CLibLogger::DumpFormatMessage(TLibTraceMessageType aLogMessageType,
                            char *aFileName,
							int aLine,
							char *aMessage,
							char *aFormat,
							VA_LIST& aMarkerList)
{
	if(NULL == aFormat)
	{
		/* Error : Check your input value. */
		return 0;
	}
	TBuf8<MAX_LOG_STR_LEN> lBuf;
	lBuf.FillZ();
	lBuf.SetLength(0);
	TPtrC8 lPtr((TUint8*)aFormat, _lstrlen(aFormat));
	lBuf.AppendFormatList(lPtr, aMarkerList);

	return DumpMessage(aLogMessageType, aFileName, aLine, aMessage, (char *)lBuf.PtrZ(), lBuf.Length());
}

// -----------------------------------------------------------------------------
// CLibLogger::DumpMessage
// Dump the internal message in hex format.
// -----------------------------------------------------------------------------
//
int CLibLogger::DumpMessage(TLibTraceMessageType aLogMessageType,
                            char *aFileName,
							int aLine,
							char *aMessage,
							char *aStr,
							int aStrLen)
{
	int istrLen = (aStrLen == -1) ? _lstrlen(aStr) : aStrLen;
	if((NULL == aStr)
		|| (istrLen <= 0))
	{
		/* Error : Check your input value. */
		return 0;
	}
	/* log the timestamp */

	return LibTracer(aLogMessageType, aFileName, aLine, aMessage) + DumpMessage(aStr, istrLen);
}

// -----------------------------------------------------------------------------
// CLibLogger::DumpFormatMessage
// Dump the internal message in hex format.
// -----------------------------------------------------------------------------
//
int CLibLogger::DumpFormatMessage(TLibTraceMessageType aLogMessageType,
							char *aMessage,
							char *aFormat,
							VA_LIST& aMarkerList)
{
	if(NULL == aFormat)
	{
		/* Error : Check your input value. */
		return 0;
	}


	/* trying to fetch the local time */
	TBuf8<MAX_LOG_STR_LEN> lBuf;
	lBuf.FillZ();
	lBuf.SetLength(0);

	/* formulate the time stamp with log type */
	_LIT8(KFormat, "[%s] - %s");
	lBuf.Format(KFormat,
		KMsgType[BitPosition(aLogMessageType)], aMessage);


	_LIT8(KNewLine, "\r\n");
	lBuf.Append(KNewLine);
    WriteMessage(lBuf);
    int len = lBuf.Length();
	lBuf.FillZ();
	lBuf.SetLength(0);
	TPtrC8 lPtr((TUint8*)aFormat, _lstrlen(aFormat));
	lBuf.AppendFormatList(lPtr, aMarkerList);

	return len + DumpMessage((char *)lBuf.PtrZ(), lBuf.Length());
}



#ifdef SYMBIAN_FILE_LOGGER
// -----------------------------------------------------------------------------
// CLibLogger::DumpMessage
// Dump the internal message in hex format.
// -----------------------------------------------------------------------------
//
int CLibLogger::DumpMessage(char *aStr, int aStrLen)
{
	/* check the inputs */
	if((NULL == aStr)
		|| (aStrLen <= 0))
	{
		/* Error : Check your input value. */
		return 0;
	}

	/* Input parameter is okey */

	TFileName logFileName;
	GetLogFilename(logFileName);
    RFileLogger::HexDump(KLogFileLocation, logFileName, EFileLoggingModeAppend, NULL, NULL, (TUint8*)aStr, aStrLen);
    return aStrLen;
}


#else

// -----------------------------------------------------------------------------
// CLibLogger::CreateFileSession
// Create a file session for the logger.
// -----------------------------------------------------------------------------
//
int CLibLogger::CreateFileSession(RFs &aFsSession, RFile &aFile)
{
	int lErr;
	/* register with the file service session. */

	lErr = aFsSession.Connect();
	if(lErr != KErrNone)
	{
		/* Error : unable to create a session with file server */
		return lErr;
	}

	/* Success : application is connected with the file server */

	TFileName logFileName;
	GetLogFilename(logFileName);

	aFsSession.MkDirAll(logFileName);
	CDir* fileList;
	CDir* dirList;
	
	//coverity[alloc_fn]
	lErr = aFsSession.GetDir(logFileName,KEntryAttNormal,
		ESortByName,fileList,dirList);
	if(lErr != KErrNone)
	{
		/* Error : unable to retrieve the directory list */
		aFsSession.Close(); /* close the session with file server */
		return lErr;
	}

	if(0 >= fileList->Count())
	{
		/* File doesn't exist. create a new file. */
		lErr = aFile.Replace(aFsSession,
								logFileName,
								EFileWrite|EFileStreamText);
		if(lErr != KErrNone)
		{
			/* Error : unable to create the new file. */
			delete fileList;
			delete dirList;
			aFsSession.Close(); /* close the session with file server */
			return lErr;
		}
	}
	else
	{
		/* Shared access for reading and writing. */
		lErr = aFile.Open(aFsSession,
								logFileName,
								EFileWrite | EFileStreamText);
		if(lErr != KErrNone)
		{
			/* Error : unable to open the file. */
			delete fileList;
			delete dirList;
			aFsSession.Close(); /* close the session with file server */
			return lErr;
		}
		int pos = 0; /* since we have pass the 2nd parameter as reference. */
		lErr = aFile.Seek(ESeekEnd, pos);
		if(lErr != KErrNone)
		{
			/* Error : unable to move the file pointer. */
			//coverity[leave_without_push]
			aFile.Close(); /* close the file */
			//coverity[leave_without_push]
			aFsSession.Close(); /* close the session with file server */
			delete fileList;
			delete dirList;
			return lErr;
		}
	}
	/* delete the resources */

	delete fileList;
	delete dirList;
	return lErr;
}


// -----------------------------------------------------------------------------
// CLibLogger::DumpMessage
// Dump the internal message in hex format.
// -----------------------------------------------------------------------------
//
int CLibLogger::DumpMessage(char *aStr, int aStrLen)
{
	/* check the inputs */
	if((NULL == aStr)
		|| (aStrLen <= 0))
	{
		/* Error : Check your input value. */
		return 0;
	}

	/* Input parameter is okey */

	/* local variables */
	int counter = 0;

	/* store the input string to a local pointer */
	TPtrC8 istrMesg((TUint8* )aStr, aStrLen);

	/* local log buffer */
	TBuf8<MAX_LOG_STR_LEN> lstrLogMesgString;
	TBuf8<MAX_LOG_STR_LEN> ptempStr;

	/* few literals */
	_LIT8(KSpace, " ");
	_LIT8(KDblSpace, "   ");
	_LIT8(KFormat, "%2x "); /* hex format */
    _LIT8(KNewLine, "\r\n");
	RFs fsSession; /* file server */
	RFile file;
	int lErr = CLibLogger::CreateFileSession(fsSession, file);
	if(KErrNone != lErr)
	{
		/* Error : Unable to initialize the file session */
		return lErr;
	}
	// since these method can be called in from thread context and
	// thread may not call this method within TRAPD
    //CleanupClosePushL(fsSession);
    //CleanupClosePushL(file);

	for(counter = 0; counter < aStrLen; counter++)
	{
		if((counter != 0) && ((counter % DUMP_STR_LENGTH) == 0))
		{
			/* sring reaches with the multiple to 16 (DUMP_STR_LENGTH) */
			lstrLogMesgString.UpperCase();
			lstrLogMesgString.Append(KSpace);
			/* append 16 (DUMP_STR_LENGTH) characters */
			lstrLogMesgString.Append(istrMesg.Mid(((counter / DUMP_STR_LENGTH) - 1) * DUMP_STR_LENGTH, DUMP_STR_LENGTH));

			/* dump the buffer */
			ptempStr.Format(KDumpSpace, counter - DUMP_STR_LENGTH);
			lstrLogMesgString.Insert(0, ptempStr);
            file.Write(lstrLogMesgString);
            file.Write(KNewLine);
            file.Flush();
			/* free the buffer, set the length to zero */
			lstrLogMesgString.FillZ();
			lstrLogMesgString.SetLength(0);
		} /* end of if */
		ptempStr.Format(KFormat,(int)istrMesg[counter]);
		lstrLogMesgString.Append(ptempStr);
	} /*  end of for */

	if(0 < lstrLogMesgString.Length())
	{
		/* few character remains */
		/* it's not multiple of 16 (DUMP_STR_LENGTH) */
		/* adjust the dump and the character portion */
		if (aStrLen % DUMP_STR_LENGTH != 0)
		{
    		for(int lliCounter = 0; lliCounter < DUMP_STR_LENGTH - (aStrLen % DUMP_STR_LENGTH); lliCounter++)
    		{
    			lstrLogMesgString.Append(KDblSpace); /* filling the blanks */
    		} /* end of for */
        }
		lstrLogMesgString.UpperCase();
		/* fill the string */
		lstrLogMesgString.Append(KSpace);
		/* append the string */
		if (aStrLen % DUMP_STR_LENGTH != 0)
		{
		    lstrLogMesgString.Append(istrMesg.Mid(((counter / DUMP_STR_LENGTH)) * DUMP_STR_LENGTH, (aStrLen - ((counter / DUMP_STR_LENGTH)) * DUMP_STR_LENGTH)));
		}
		else
		{
		    lstrLogMesgString.Append(istrMesg.Mid(((counter / DUMP_STR_LENGTH) - 1) * DUMP_STR_LENGTH, (aStrLen - ((counter / DUMP_STR_LENGTH) -1) * DUMP_STR_LENGTH)));
		}
		ptempStr.Format(KDumpSpace, ((aStrLen / DUMP_STR_LENGTH) * DUMP_STR_LENGTH));
		lstrLogMesgString.Insert(0, ptempStr);
        file.Write(lstrLogMesgString);
        file.Write(KNewLine);
        file.Flush();
	} /* end of if */
	//CleanupStack::PopAndDestroy(2); // file, fsFession
	file.Close();
	fsSession.Close();
	return aStrLen; /* return the total dump message length */
}

#endif // SYMBIAN_FILE_LOGGER

#ifndef EKA2

GLDEF_C TInt E32Dll(TDllReason aReason)
	{
	switch (aReason)
		{
	default:
		break;
		}
	return KErrNone;
	}

#endif

// End of file
