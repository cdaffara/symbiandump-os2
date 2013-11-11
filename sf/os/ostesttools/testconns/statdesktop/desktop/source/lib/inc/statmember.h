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
* Description:     STAT member class header file
*
*/




#ifndef STATMEMBER30_H
#define STATMEMBER30_H

#include <creporter.h>
#include <statengine.h>
#include "ini.h"

#define MAX_ID_PREFIX				256

class STATDLLMember
{
public:
	STATDLLMember(const STATCONNECTTYPE iConnectType, 
				  const char *pszPlatformType,
				  Reporter *theRep = NULL);
	virtual ~STATDLLMember();
	bool Prepare(int iMillisecondDelay, LPTSTR pszLogPath, bool bAppend, MessageReporter *const aMessageReporter, LPTSTR pszRefDir, const bool bRemoveOldImages, const int iFactor, bool bState);
	int SetCommandDelay(int iMillisecondDelay);		// set the command delay
	int SetLogging(LPTSTR pszLogPath, bool bAppend, MessageReporter *const aMessageReporter);// set logging of the back end operations
	int SetImageVerification(LPTSTR pszRefDir, const bool bRemoveOldImages, const int iFactor);
	void SetResponseTimeout(const unsigned long lMilliseconds) { pEngine->iMaxTimeLimit = lMilliseconds; }
	void SetMultithreaded(bool bState = true) { pEngine->SetMultithreaded(bState); }	// set multithreaded check

	int OpenScriptFile(LPCTSTR pszText, bool bIsFile);
	int RunScript(ScriptProgressMonitor *const monitor);
	int SendCommandFile(LPCTSTR pszFile, ScriptProgressMonitor *const monitor);			// execute a command file
	int SendRawCommand(LPCTSTR pszText, ScriptProgressMonitor *const monitor);			// execute a command script
	int StopProcessing(void);
	int GetCommandCount(LPCTSTR pszFile, int* commandCount);
	int GetCurrentCommandNumber( int* commandNumber);
	const char * GetReceivedData(void);
	const char * GetErrorText( int errorCode );

	int GetSnapshot(TBitmapFileHeader **ppFile, TBitmapInfoHeader **ppBitmap, char **ppData, unsigned long *pSize);
	const char * GetDeviceInfo() { return pEngine->pDeviceInfo; }
	const char * GetTEFSharedData() { return pEngine->iTEFSharedData; }
	const char * GetErrorText() {	return szErrorText; }	// returns complete error message
	int GetErrorCode() { return iErrorCode; }		// return just the code

	static LPTSTR ToUnicode(const char *string);	// convert ANSI to Unicode
	static const char * ToAnsi(LPCTSTR string);			// convert Unicode to ANSI
	static void ParseConnection(char *pConnection, STATCONNECTTYPE *pType, char **ppAddress);

	STATCONNECTTYPE iConnectionType;				// type of connection
	char *pszAddress;								// our selected address

	STATDLLMember* lPrevConnection;					// previous connection in list
	STATDLLMember* lNextConnection;					// next connection in list

private:
	void Message(const char * message, ...);
	void ErrorMessage(const char * message, ...);

	CSTATEngine *pEngine;							// pointer to back end
	int iErrorCode;									// error code
	char szErrorText[MAX_ERROR_MSG_LEN * 2];		// error message
	char szPrefix[MAX_ID_PREFIX];					// id prefix
	Reporter *pRep;									// pointer to logging
	CStatIniFile statIniFile;							// ini file reader
};

#endif // STATMEMBER30_H
