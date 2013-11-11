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




// CUtils.cpp : Handy bits and pieces for apps
//

#include "stdafx.h"
#include <windows.h>
#include <tchar.h>
#include "Utils.h"

CUtils::CUtils()
: m_hMutex((HANDLE)0)
{
    // open the Service Control Manager
    hSCM = OpenSCManager(NULL,						// local machine
                         NULL,						// ServicesActive database
                         SC_MANAGER_ALL_ACCESS);	// full access
}

CUtils::~CUtils()
{
    if (hSCM)
	    CloseServiceHandle(hSCM);

	if (m_hMutex)
		CloseHandle(m_hMutex);
}


// attempts to create a mutex for this process.
// if it fails, another process of the same name is already running
bool CUtils::AlreadyRunning(LPCTSTR szAppName)
{
	m_hMutex = CreateMutex(NULL, FALSE, szAppName);
	if ((!m_hMutex) || (WaitForSingleObject( m_hMutex, 0 ) == WAIT_TIMEOUT))
    {
		return true;   
    }

	return false;
}


// invokes an application then waits for it to exit
bool CUtils::CallProcessAndWait(LPCTSTR szApplication, LPTSTR szCommandLine, LPCTSTR szDirectory, bool bRunMinimised)
{
	bool valid = false;
	STARTUPINFO startInfo = {0};
	startInfo.cb = sizeof(STARTUPINFO);
	retCode = -1;

	// run window minimised and not active
	if (bRunMinimised)
	{
		startInfo.dwFlags = STARTF_USESHOWWINDOW;
		startInfo.wShowWindow = SW_SHOWMINIMIZED | SW_SHOWMINNOACTIVE;
	}

	PROCESS_INFORMATION procInfo = {0};

	// event attributes for the child process
	SECURITY_ATTRIBUTES eventAttr;
	eventAttr.nLength = sizeof(eventAttr);
	eventAttr.lpSecurityDescriptor = NULL;
	eventAttr.bInheritHandle = TRUE;

	// NOTE: if passing a command line, include the application name in szCommandLine and pass NULL for szApplication
	if (CreateProcess(szApplication, szCommandLine, NULL, NULL, FALSE, NULL, NULL,
					  szDirectory, &startInfo, &procInfo))
	{
		if (WAIT_OBJECT_0 == WaitForSingleObject(procInfo.hProcess, INFINITE))
		{
			GetExitCodeProcess(procInfo.hProcess, &retCode);
			CloseHandle(procInfo.hThread);
			CloseHandle(procInfo.hProcess);
			valid = true;
		}
	}
	else
		_GetWindowsError();

	return valid;
}


bool CUtils::CallProcess(LPCTSTR szApplication, LPTSTR szCommandLine, LPCTSTR szDirectory)
{
	bool valid = false;
	STARTUPINFO startInfo = {0};
	startInfo.cb = sizeof(STARTUPINFO);

	PROCESS_INFORMATION procInfo = {0};

	// event attributes for the child process
	SECURITY_ATTRIBUTES eventAttr;
	eventAttr.nLength = sizeof(eventAttr);
	eventAttr.lpSecurityDescriptor = NULL;
	eventAttr.bInheritHandle = TRUE;

	if (CreateProcess(szApplication, szCommandLine, NULL, NULL, FALSE, NULL, NULL,
					  szDirectory, &startInfo, &procInfo))
	{
		CloseHandle(procInfo.hThread);
		CloseHandle(procInfo.hProcess);
		valid = true;
	}
	else
		_GetWindowsError();

	return valid;
}

// requests an NT service to perform a particular request
bool CUtils::ServiceRequest(const char *szServiceName, const unsigned int iRequest)
{
	bool valid = false;

    if (hSCM)
	{
		switch(iRequest)
		{
		case REQ_START:
			if (Validate(hSCM, szServiceName, SERVICE_INACTIVE))
			{
				SC_HANDLE hService = OpenService(hSCM, szServiceName, SERVICE_ALL_ACCESS);
				if (hService) 
				{
					DealWithDependentServices(szServiceName, iRequest);

					if (StartService(hService, 0, NULL))
						valid = true;

					CloseServiceHandle(hService);
				}
				else
					_GetWindowsError();
			}
			else
			{
				valid = true;	// already started
			}
			break;
		case REQ_STOP:
			if (Validate(hSCM, szServiceName, SERVICE_ACTIVE))
			{
				SC_HANDLE hService = OpenService(hSCM, szServiceName, SERVICE_STOP);
				if (hService) 
				{
					DealWithDependentServices(szServiceName, iRequest);

					SERVICE_STATUS status;
					if (ControlService(hService, SERVICE_CONTROL_STOP, &status))
						valid = true;

					CloseServiceHandle(hService);
				}
				else
					_GetWindowsError();
			}
			else
			{
				valid = true;	// already stopped
			}
			break;
		default:
			break;
		};

    }
	return valid;
}



// recurses through a directory structure, performing an action on the files/directories within
unsigned int CUtils::RecurseDir(const char *szDirectory, const unsigned int iCommand)
{
	iFileCommand = iCommand;
	iFileCount = 0;

	Recurse(szDirectory);

	return iFileCount;
}


// adds a backslash to a path if it doesn't already have one
void CUtils::AddSlash(char *szPath)
{
	if (*(szPath + strlen(szPath) - 1) != '\\')
		strcat(szPath, "\\");
}


bool CUtils::ListServices(DWORD dwState)
{
	return Validate(hSCM, NULL, dwState, true);
}

////////////////////////////////////////////////////////////////////////////////////////////////
//
// Private Methods
//
////////////////////////////////////////////////////////////////////////////////////////////////

bool CUtils::Validate(SC_HANDLE hSCM, const char *szServiceName, DWORD dwState, bool bDisplayOnly)
{
	bool valid = false;
	DWORD dwBytes = 0;
	DWORD dwNumServices = 0;
	DWORD dwResumeHandle = 0;
	static ENUM_SERVICE_STATUS status[200];

	// get list of services in particular state
	if (EnumServicesStatus(hSCM,							// handle
						 SERVICE_WIN32,						// service
						 dwState,							// state
						 (ENUM_SERVICE_STATUS *)status,		// returned info
						 200 * sizeof(ENUM_SERVICE_STATUS),	// length of buffer
						 &dwBytes,							// returned length
						 &dwNumServices,					// number services returned
						 &dwResumeHandle))					// point to continue
	{
		if (bDisplayOnly)
		{
			printf("\n\nServices\n========\n\n");
		}

		for (DWORD i=0;i<dwNumServices;i++)
		{
			if (bDisplayOnly)
			{
				if (status[i].ServiceStatus.dwCurrentState == SERVICE_RUNNING)
					printf("STARTED  %s  (%s)\n", status[i].lpDisplayName, status[i].lpServiceName);
				else if (status[i].ServiceStatus.dwCurrentState == SERVICE_STOPPED)
					printf("STOPPED  %s  (%s)\n", status[i].lpDisplayName, status[i].lpServiceName);
				else if (status[i].ServiceStatus.dwCurrentState == SERVICE_PAUSED)
					printf("PAUSED   %s  (%s)\n", status[i].lpDisplayName, status[i].lpServiceName);
				else
					printf("PENDING  %s  (%s)\n", status[i].lpDisplayName, status[i].lpServiceName);
			}
			else
			{
				if (stricmp(status[i].lpServiceName, szServiceName) == 0)
				{
					valid = true;
					break;
				}
			}
		}
	}

	// check the driver list, just in case it's in there...
	if (!valid)
	{
		if (EnumServicesStatus(hSCM,							// handle
							 SERVICE_DRIVER,					// driver
							 dwState,							// state
							 (ENUM_SERVICE_STATUS *)status,		// returned info
							 200 * sizeof(ENUM_SERVICE_STATUS),	// length of buffer
							 &dwBytes,							// returned length
							 &dwNumServices,					// number services returned
							 &dwResumeHandle))					// point to continue
		{
			if (bDisplayOnly)
			{
				printf("\n\nDrivers\n=======\n\n");
			}

			for (DWORD i=0;i<dwNumServices;i++)
			{
				if (bDisplayOnly)
				{
					if (status[i].ServiceStatus.dwCurrentState == SERVICE_RUNNING)
						printf("STARTED  %s  (%s)\n", status[i].lpDisplayName, status[i].lpServiceName);
					else if (status[i].ServiceStatus.dwCurrentState == SERVICE_STOPPED)
						printf("STOPPED  %s  (%s)\n", status[i].lpDisplayName, status[i].lpServiceName);
					else if (status[i].ServiceStatus.dwCurrentState == SERVICE_PAUSED)
						printf("PAUSED   %s  (%s)\n", status[i].lpDisplayName, status[i].lpServiceName);
					else
						printf("PENDING  %s  (%s)\n", status[i].lpDisplayName, status[i].lpServiceName);
				}
				else
				{
					if (stricmp(status[i].lpServiceName, szServiceName) == 0)
					{
						valid = true;
						break;
					}
				}
			}
		}
	}

	return valid;
}


void
CUtils::Recurse(const char * rootDir)
{
    char fullname[MAX_PATH + 1];
	HANDLE hFind;
	
	sprintf( fullname, "%s\\*", rootDir );
	hFind = FindFirstFile(fullname, &ffd);

	if(hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
	        sprintf( fullname, "%s\\%s", rootDir, ffd.cFileName );

			// directory so recurse into it
			if(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				// ignore current and previous dirs
				if ((stricmp (ffd.cFileName, ".") != 0) && (strcmp (ffd.cFileName, "..") != 0))
				{
					Recurse(fullname);

					// remove it
					if (iFileCommand == DELETEALL)
						if (RemoveDirectory(fullname))
							iFileCount++;
				}
			}
			else
			{ 
				// remove now empty directory
				if (iFileCommand == DELETEALL || iFileCommand == DELETEFILESONLY)
					if (DeleteFile( fullname ))
						iFileCount++;

				// clear the attributes
				if (iFileCommand == REMOVEALLATTRIBUTES)
					if (SetFileAttributes(fullname, FILE_ATTRIBUTE_NORMAL))
						iFileCount++;
			}
		}
		while(FindNextFile(hFind, &ffd));

		FindClose( hFind );
	}

	// remove this root now we are finished with it
	if (iFileCommand == DELETEALL)
		if (RemoveDirectory(rootDir))
			iFileCount++;
}


void
CUtils::DealWithDependentServices(const char *szServiceName, const unsigned int iRequest)
{
	DWORD dwBytes = 0;
	DWORD dwNumServices = 0;
	DWORD dwResumeHandle = 0;
	ENUM_SERVICE_STATUS status[100];

	// service may have dependent services so stop them first
	SC_HANDLE hService = OpenService(hSCM, szServiceName, SERVICE_ENUMERATE_DEPENDENTS);
	if (hService) 
	{
		if (EnumDependentServices(hService, 
								SERVICE_ACTIVE,
								(ENUM_SERVICE_STATUS *)status,		// returned info
								100 * sizeof(ENUM_SERVICE_STATUS),	// length of buffer
								&dwBytes,							// returned length
								&dwNumServices))					// number services returned
		{
			for (DWORD i=0;i<dwNumServices;i++)
			{
				// recurse until we stop the last dependant
				ServiceRequest(status[i].lpServiceName, iRequest);
			}
		}

		CloseServiceHandle(hService);

		// if there were dependents, allow some time register service stops
		if (dwNumServices)
			Sleep(2000);
	}
	else
		_GetWindowsError();
}

void
CUtils::_GetWindowsError()
{
	LPVOID lpSysError;
	DWORD dwRet = GetLastError();

	FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
				   NULL,
				   dwRet,
				   MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
				   ( LPTSTR ) &lpSysError,
				   0,
				   NULL );

	if (lpSysError)
		_tcscpy(szError, ( LPTSTR )lpSysError);

	LocalFree( lpSysError );
}

LPTSTR
CUtils::GetWindowsError()
{
	return szError;
}

LPTSTR
CUtils::GetWindowsErrorNow()
{
	_GetWindowsError();
	return szError;
}


///////////////////////////////////////////////////////////////////////////////////
// Trim the text of unwanted characters
//
void
CUtils::TrimTabsCRAndWhitespace(char *szBuffer)
{
	// strip any unwanted chars off the end of each value
	char *ptr = szBuffer + strlen(szBuffer) - 1;
	if (ptr && (*ptr))
	{
		while ((*ptr) == '\r' || (*ptr) == '\n' || (*ptr) == '\t' || (*ptr) == ' ')
			ptr--;

		*(ptr + 1) = (char)0;
	}
}


///////////////////////////////////////////////////////////////////////////////////
// Trim the buffered text to the first EOL and adjust the file pointer to suit
// If EOL's included, include all up to the next occurance of text or EOF
//
int
CUtils::TrimBufferToFirstEOL(HANDLE mmphndl, char *szBuffer, bool bIncludeCR)
{
	int count = 0;
	int length = strlen(szBuffer);
	int actualend = 0;

	// move to first CR
	while (*(szBuffer + count) && (*(szBuffer + count) != '\r') && (*(szBuffer + count) != '\n'))
		count++;


	// move to after CRs
	actualend = count;
	while ((*(szBuffer + actualend) == '\r') || (*(szBuffer + actualend) == '\n'))
		actualend++;

	if (count)
	{
		// include the CRs
		if (bIncludeCR)
			count = actualend;

		// terminate the string at our desired point
		*(szBuffer + count) = (char)0;

		// set file pointer accordingly
		if (mmphndl)
			SetFilePointer(mmphndl, 0 - (length - actualend), NULL, FILE_CURRENT);
	}

	return count;
}


///////////////////////////////////////////////////////////////////////////////////
// Trim the buffered text to the last EOL and adjust the file pointer to suit
//
void
CUtils::TrimBufferToLastEOL(HANDLE mmphndl, char *szBuffer)
{
	// move back to last CR
	char *end = szBuffer + strlen(szBuffer) - 1;
	while ((end != szBuffer) && (*end != '\r') && (*end != '\n'))
		end--;

	// NULL-terminate line and adjust file pointer
	if (end != (szBuffer + strlen(szBuffer) - 1))
	{
		int diff = (szBuffer + strlen(szBuffer) - 1) - end;
		(*end) = (char)0;

		// back up the file pointer to the end of the last line
		if (mmphndl)
			SetFilePointer(mmphndl, 0 - diff, NULL, FILE_CURRENT);
	}
}
