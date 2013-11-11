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




// Utils.h : main header file for the basic utilities class
//

#ifndef UTILS_H
#define UTILS_H

#include <windows.h>
#include <stdio.h>
#include <winsvc.h>

// service requests
#define REQ_START 1
#define REQ_STOP  2

// file requests
#define DELETEALL 1
#define DELETEFILESONLY 2
#define REMOVEALLATTRIBUTES 4


class CUtils
{
	SC_HANDLE hSCM;
	HANDLE m_hMutex;
	WIN32_FIND_DATA ffd;
	unsigned int iFileCommand;
	unsigned int iFileCount;
	TCHAR szError[2048];
	unsigned long retCode;

	bool Validate(SC_HANDLE hSCM, const char *szServiceName, DWORD dwState, bool bDisplayOnly = false);
	void Recurse(const char * rootDir);
	void DealWithDependentServices(const char *szServiceName, const unsigned int iRequest);
	void _GetWindowsError();

public:
	CUtils();
	~CUtils();

	bool AlreadyRunning(LPCTSTR szAppName);
	bool CallProcessAndWait(LPCTSTR szApplication, LPTSTR szCommandLine, LPCTSTR szDirectory, bool bRunMinimised = false);
	bool CallProcess(LPCTSTR szApplication, LPTSTR szCommandLine, LPCTSTR szDirectory);
	bool ServiceRequest(const char *szServiceName, const unsigned int iRequest);
	bool ListServices(DWORD dwState);
	unsigned int RecurseDir(const char *szDirectory, const unsigned int iCommand);
	void AddSlash(char *szPath);
	int TrimBufferToFirstEOL(HANDLE mmphndl, char *szBuffer, bool bIncludeCR = true);
	void TrimBufferToLastEOL(HANDLE mmphndl, char *szBuffer);
	void TrimTabsCRAndWhitespace(char *szBuffer);
	char *GetWindowsError();
	char *GetWindowsErrorNow();
	unsigned long GetReturnCode() {return retCode;}
	unsigned long GetReturnCodeNow() {return GetLastError();}
};

#endif
