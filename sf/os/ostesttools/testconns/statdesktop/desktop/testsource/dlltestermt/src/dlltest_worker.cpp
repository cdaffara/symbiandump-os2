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
#include <statcommon.h>
#include "dlltest_worker.h"
#include <stattask.h>
#include <statexp.h>

//////////////////////////////////////////////////////////////////////////
// Write the contents of a screenshot out to file
bool WriteBitmap(char *file, TBitmapFileHeader *fileheader, TBitmapInfoHeader * bmpHeader, char *bmpBits, unsigned long lSize)
{
	bool success = false;

	// write the whole lot out to file
	HANDLE infile;
	if (INVALID_HANDLE_VALUE != (infile = CreateFile(file,
										   GENERIC_WRITE,
										   0, 
										   NULL, 
										   CREATE_ALWAYS,
										   FILE_ATTRIBUTE_NORMAL,
										   0)))
	{
		DWORD dwBytesWritten = 0;

		// write the file info
		if (WriteFile(infile, (LPVOID *)fileheader, sizeof(TBitmapFileHeader), &dwBytesWritten, NULL) &&
			dwBytesWritten == sizeof(TBitmapFileHeader))
		{
			// write the bitmap info
			if (WriteFile(infile, (LPVOID *)bmpHeader, sizeof(TBitmapInfoHeader), &dwBytesWritten, NULL) &&
				dwBytesWritten == sizeof(TBitmapInfoHeader))
			{
				// write the bitmap data
				if (WriteFile(infile, (LPVOID *)bmpBits, lSize, &dwBytesWritten, NULL) &&
					dwBytesWritten == lSize)
				{
					success = true;
				}
			}
		}

		CloseHandle(infile);
	}

	return success;
}


//////////////////////////////////////////////////////////////////////////
// This thread function handles the processing of a script by a single
// connection
DWORD WINAPI
STATDeviceThreadProcedure(LPVOID lpParameter)
{
	DWORD valid = true;

	STATTASKINFO *pWorker = (STATTASKINFO *)lpParameter;
	if (pWorker)
	{
		int h = ConnectMT(pWorker->connectType, pWorker->szAddress);
		if (h)
		{
			printf("Version %s\r\n", Version());
			valid = SetCommandLogging(h, pWorker->pLogfile, NULL);
			valid = SetCommandDelay(h, pWorker->delay);

			if (valid)
				if (pWorker->verify)
					valid = SetImageVerification(h, pWorker->pRefdir, pWorker->removeImages?true:false, pWorker->fudge);

			if (valid)
			{
				for (int i=0;i<pWorker->iterations;i++)
				{
					//valid = SendRawCommand(h, pWorker->pCommand);
					valid = SendCommandFile(h, pWorker->pCommand);
					if (!valid)
					{
						printf("[%d] bad command(%s)\r\n", h, GetError(h));
						break;
					}

					/*struct TBitmapFileHeader *pFile = NULL;
					struct TBitmapInfoHeader *pInfo = NULL;
					char *pData = NULL;
					unsigned long lSize = 0;

					valid = GetSnapshot(h, &pFile, &pInfo, &pData, &lSize);
					if (valid)
						valid = WriteBitmap(pWorker->pSnapshot, pFile, pInfo, pData, lSize);
					if (!valid)
					{
						printf("[%d] bad command(%s)\r\n", h, GetError(h));
						break;
					}*/
				}
			}

			Disconnect(h);
		}
		else
			printf("Bad connection (%s)\r\n", GetError());
	}
	else
		printf("Bad parameter\r\n");

	return (DWORD)valid;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////
// Constructor
dlltest_worker::dlltest_worker()
: iterations(1), delay(100), lConectionCount(0), verify(0), removeImages(0), fudge(0)
{
	memset(szLogfile, 0, sizeof(szLogfile));
	memset(szCommand, 0, sizeof(szCommand));
	memset(szSnapshot, 0, sizeof(szSnapshot));
	memset(szRefdir, 0, sizeof(szRefdir));
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////
// Destructor
dlltest_worker::~dlltest_worker()
{
	Finish();
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////
// Get the settings and start the worker threads
bool dlltest_worker::Prepare()
{
	CStatIniFile statIniFile;
	statIniFile.SetIniFileName(STAT_INI_NAME);
	if(statIniFile.SectionExists(ST_TEST_KEY) )
	{
		CString setting;
		TCHAR szBuffer[2048];
		long lCount = 0;
		*(szBuffer) = (TCHAR)0;

		setting.Empty();
		setting=statIniFile.GetKeyValue(ST_ITERATIONS,ST_TEST_KEY);
		if(!setting.IsEmpty())
		{
			if (_ttoi(setting)<1)
				setting=_T("1");

			iterations=_ttoi(setting);
		}

		setting=statIniFile.GetKeyValue(ST_DELAY,ST_TEST_KEY);
		if(!setting.IsEmpty())
		{
			int iPos = _ttoi(setting);
			if (iPos < 100)
				setting= _T("100");

			if (iPos > 30000)
				setting= _T("30000");

			delay=_ttoi(setting);
		}

	// logging settings
		strcpy(szLogfile,(LPCSTR)statIniFile.GetKeyValue(ST_LOGFILE,ST_TEST_KEY));

	// raw command
		strcpy(szCommand,(LPCSTR)statIniFile.GetKeyValue(ST_CMDFILE,ST_TEST_KEY));

	// snapshot settings
		strcpy(szSnapshot,(LPCSTR)statIniFile.GetKeyValue(ST_SNAPSHOT,ST_TEST_KEY));

	// image verification settings
		verify=0;
		setting=statIniFile.GetKeyValue(ST_CHKVERIF,ST_TEST_KEY);
		if(!setting.IsEmpty())
		{
			verify=_ttol(setting);

		}
		removeImages=0;
		setting=statIniFile.GetKeyValue(ST_VERIFYREMOVEIMAGES,ST_TEST_KEY);
		if(!setting.IsEmpty())
		{
			removeImages=_ttol(setting);

		}
		strcpy(szRefdir,(LPCSTR)	statIniFile.GetKeyValue(ST_REFDIR,ST_TEST_KEY));
		setting.Empty();
		setting=statIniFile.GetKeyValue(ST_FUDGE,ST_TEST_KEY);
		if(!setting.IsEmpty())
			fudge = _ttoi(setting);

	// connection settings

		lConectionCount = 0;
		setting.Empty();
		setting=statIniFile.GetKeyValue(ST_CONNECTIONIDX,ST_TEST_KEY);
		if(!setting.IsEmpty())
		{
			lConectionCount = _ttol(setting);

		}
		if(statIniFile.SectionExists(ST_CONNECTION_LIST) )
		{
			for( int i=0;i<lConectionCount;++i)
			{
				TCHAR buf[6];
				_ltot(i+1, buf, 10);
				setting.Empty();
				setting=statIniFile.GetKeyValue(buf,ST_CONNECTION_LIST);
				if(!setting.IsEmpty())
					strcpy(szConections[i],(LPCSTR)(setting));
			}
		}
	}
	// turn on high-level logging
	SetConnectionLogging(szLogfile);

	// start the threads
	int i;
	for (i=0;i<lConectionCount;i++)
	{
		GetTaskInfo(i);
		pDeviceTask[i] = new STATTask(STATDeviceThreadProcedure, &TaskInfo[i]);
		if (pDeviceTask[i])
		{
			if (!pDeviceTask[i]->Start())
			{
				delete pDeviceTask[i];
				pDeviceTask[i] = NULL;
			}
		}
	}

	// wait a bit to allow the threads to start
	Sleep(500);

	return true;
}


/////////////////////////////////////////////////////////////////////////
// Check to see if all our jobs have finished
int dlltest_worker::WorkPending()
{
	int i;
	int count = 0;
	for (i=0;i<lConectionCount;i++)
	{
		if (pDeviceTask[i] && pDeviceTask[i]->StillActive())
		{
			count++;
		}
	}

	return count;
}

/////////////////////////////////////////////////////////////////////////
// Release any resources
void dlltest_worker::Finish()
{
	// kill the threads
	int i;
	for (i=0;i<lConectionCount;i++)
	{
		if (pDeviceTask[i])
		{
			pDeviceTask[i]->Kill();
			delete pDeviceTask[i];
			pDeviceTask[i] = NULL;
		}
	}

	// turn off high-level logging
	CloseConnectionLogging();
}


/////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS
/////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////
// Set task information for a single thread
void dlltest_worker::GetTaskInfo(int index)
{
	STATCONNECTTYPE type;
	char *addr;
	ParseConnection(szConections[index], &type, &addr);

	TaskInfo[index].connectType = type;
	TaskInfo[index].delay = delay;
	TaskInfo[index].fudge = fudge;
	TaskInfo[index].iterations = iterations;
	TaskInfo[index].removeImages = removeImages;
	TaskInfo[index].pCommand = szCommand;
	TaskInfo[index].pLogfile = szLogfile;
	TaskInfo[index].pRefdir = szRefdir;
	TaskInfo[index].pSnapshot = szSnapshot;
	TaskInfo[index].verify = verify;
	strcpy(TaskInfo[index].szAddress, addr);
}


//////////////////////////////////////////////////////////////////////////////////////
// Split a connection registry entry into its respective parts
void dlltest_worker::ParseConnection(char *pConnection, STATCONNECTTYPE *pType, char **ppAddress)
{
	(*pType) = SymbianInvalid;
	(*ppAddress) = NULL;

	static char szConnection[256];
	memset(szConnection, 0, sizeof(szConnection));
	strcpy(szConnection, pConnection);

	char *p = strchr(szConnection, ':');
	if (p)
	{
		(*p) = (char)0;
		(*ppAddress) = p + 1;

		if (stricmp(szConnection, "SymbianSocket") == 0)
		{
			(*pType) = SymbianSocket;
		}
		else if (stricmp(szConnection, "SymbianSerial") == 0)
		{
			(*pType) = SymbianSerial;
		}
		else if (stricmp(szConnection, "SymbianInfrared") == 0)
		{
			(*pType) = SymbianInfrared;
		}
		else if (stricmp(szConnection, "SymbianBluetooth") == 0)
		{
			(*pType) = SymbianBluetooth;
		}
		else if (stricmp(szConnection, "SymbianUSB") == 0)
		{
			(*pType) = SymbianUSB;
		}

		(*p) = ':';
	}
}


