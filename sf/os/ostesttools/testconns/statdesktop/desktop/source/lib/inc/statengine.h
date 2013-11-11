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




#ifndef STATENGINE_H
#define STATENGINE_H

#include <statcommon.h>
#include <CSTATLogfile.h>
#include <CSTATDataFormatConverter.h>
#include <CSTATImageVerify.h>
#include <statcomms.h>
#include <statscriptdecoder.h>
#include <ini.h>

#include "ScriptProgressMonitor.h"
#include <winsock2.h>

#define DOTTED_LINE "--------------------------------------------------------------------------------------------------------------"

class CSTATEngine
{
public:
	CSTATEngine();
	~CSTATEngine();
	int Initialise(const STATCONNECTTYPE eConnect, const char *pAddress = NULL);	// start everything
	void SetCommandDelay(int iMillisecondDelay);					// set delay between commands
	int SetLogging(const CString& logfilename, const char* prefix, 
			bool append, bool bMessages, bool bFile, MessageReporter *const aMessageReporter);	// set a log message
	int GetCommandCount(CString file, int *pCount);					// get amount of commands in script
	int OpenScriptFile(CString file, bool bIsFile);					// load the script
	int RunScript(ScriptProgressMonitor *const monitor);			// run the script
	int Release(void);												// release all resources
	const char* GetErrorText(int iCode) { return pLog->Text(iCode); }	// convert an error code into a text message
	const char* GetReceivedData(void);
	int GetDeviceReturnCode() { return iDeviceCode; }				// get device return code
	void SetMultithreaded(bool bState = true) { bMultithreaded = bState; }	// set multithreaded check

	static LPTSTR ToUnicode(const char *string);					// convert ANSI to Unicode
	static char * ToAnsi(LPCTSTR string);							// convert Unicode to ANSI
	static void CreateAllDirectories(CString &fullpath);			// create sub folders
	
	CSTATDataFormatConverter *pConverter;							// bitmap conversion
	CSTATImageVerify *pImageVerify;									// image verification
	STAT_STATE eStopProcessing;										// flag to pause/stop processing
	int iCurrentCommand;											// index of the current command being processed
	char *pDeviceInfo;												// pointer to device information
	char *iTEFSharedData;											// pointer to TEF shared data information
	int iMaxTimeLimit;												// max time to wait for a command

private:

	

	void SetScreenshotDirectory(const char *szScreenshotDir)	{ cScreenshotDirectory = szScreenshotDir; }
	int SetScreenshotDefaultDirectory(void);
	int SendCommand(CSTATScriptCommand *pCommand, CSTATScriptCommand **ppRecvCommand);
	int SendSingleCommand(CSTATScriptCommand *pCommand, CSTATScriptCommand **ppRecvCommand);
	int ReadTransferFile(const char *pFile, CSTATScriptCommand *pCommand);
	int SaveTransferFile(const char *pFile, char *pContents, unsigned long ulLength);
	void LogDetails(const char *prefix, CSTATScriptCommand *pCommand);
	int ConvertAndSaveScreeenshot(CString &file, const char *pContents, const unsigned long ulLength);
	int SaveTheFile(CString path, const char *pContents, const unsigned long ulLength);
	void Message(const char *pMsg, ...);
	bool StopProcessing();
	bool RemoveLeftoverMBMFiles();
	void AppendCommandToSTATLog(char *heading, char *log, unsigned long length);
	void StoreData( char* aData, unsigned long aLength, char*& aOutputPtr );
	CString GetConnection(STATCONNECTTYPE eConnectType);

	CSTATScriptCommand oRecvCommand;		// last received command
	STATComms *pComms;						// transport
	CSTATScriptDecoder *pDecoder;			// script decoder
	CSTATLogFile *pLog;						// log file
	CString cScreenshotDirectory;			// screenshot location
	int iDelay;								// command delay
	const int minimumdelay;
	const int maximumdelay;
	char szAddress[256];					// the name of the port we are using
	STATCONNECTTYPE eConnectType;			// current connection type
	int iDeviceCode;						// device return code
	bool bMultithreaded;					// flag if operating in multithreaded environment
	CStatIniFile statIniFile;					// ini file reader
	CString receivedData;

	void WriteToSocket(const char *data, int *length);
	void ReadFromSocket(char *data, int *length);
	int SetSocket(const char *ip, const int port);
	int ReleaseSocket();

	bool settingSocket;
	SOCKET dataSocket;
	WSADATA wsaData;

};

inline const char* CSTATEngine::GetReceivedData(void)
{
	return ( receivedData.operator LPCTSTR( ) );
}



//----------------------------------------------------------------------------
// our thread-safe mechanism - this must be defined, initialised and destroyed by
// the code using CSTATEngine.  See STATMember.cpp for an example
extern CRITICAL_SECTION CriticalSection;
//----------------------------------------------------------------------------

#endif
