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




#ifndef CSTATLogFile_H
#define CSTATLogFile_H

#include <afxmt.h>
#include "CSTATReturnCodes.h"
#include "MessageReporter.h"

class CSTATLogFile
{
public :
	// logging
	CSTATLogFile();
	~CSTATLogFile();
	int CreateLogFile(const CString& newlogfilename,const CString& defaultPath ,const char* prefix, bool append, bool bMessages, bool bFile);
	void SetMessageReporter(MessageReporter *const messageReporter);
	void WriteTimeToLog();					// write the current date/time to log
	void Write(char *szText, ...);			// write standard text to log
	void Write(CString cBuf);				// write CString text to log
	void CloseLogFile();					// close

	// messaging
	int Set(const char* newtext);													// write CString to storage buffer and to log
	int Set(int iMsgCode, const char* newtext = NULL, bool bMsgBox = false, bool bScrshot = false);
	// bool Get(CString &msg, CString &newtext, bool &bMsgBox, bool &bScrshot);  // get log message from storage buffer
	const char* Text(int iMsgCode) { return ReturnCodes.GetRetMsg(iMsgCode); }    // get error text

	int ToAnsi(LPCTSTR szUnicode, LPSTR szBuffer, int nBufLen);
	LPCTSTR ToUnicode(const char *string);

private :
	char* FormatText(const char* message);

	CSTATReturnCodes ReturnCodes;			// return codes for use within STAT

	// logging
	CFile logfile;							// log file object
	CString Logfilename;					// the name of our log file
	char szLogPrefix[51];					// holds a prefix to all log messages

	MessageReporter	*iMessageReporter;

	// messages
	bool bWriteToScreen;					// flag for writing to dialog box
	bool bWriteToFile;						// flag for writing to log file
	bool bMessage;							// flag for waiting message
	bool bMessageBox;						// display in a MessageBox
	bool bScreenshot;						// display associated screenshot bitmap
	CString message;						// basic message
	CString text;							// associated error text
};

#endif
