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
* Description:     include file for worker class								*
*/




#include <stat.h>
#include <stattask.h>
#include <ini.h>
#include "STATTask.h"

#define MAX_CONNECTIONS 25
#define X_MAXENTRYLEN MAX_PATH + 1
/////////////////////////////////////////////////////////////////////////
// Task initialisation info
typedef struct STATTaskInfo
{
	int iterations;
	int delay;
	long verify;
	long removeImages;
	int fudge;
	TCHAR *pLogfile;
	TCHAR *pCommand;
	TCHAR *pSnapshot;
	TCHAR *pRefdir;
	STATCONNECTTYPE connectType;
	char szAddress[256];
} STATTASKINFO;


/////////////////////////////////////////////////////////////////////////
// Main worker class
class dlltest_worker
{
public:
	dlltest_worker();
	~dlltest_worker();
	bool Prepare();
	int WorkPending();
	bool Run();
	void Finish();

private:
	void GetTaskInfo(int index);
	void ParseConnection(char *pConnection, STATCONNECTTYPE *pType, char **ppAddress);

	STATTask *pDeviceTask[MAX_CONNECTIONS];				// device connections
	STATTASKINFO TaskInfo[MAX_CONNECTIONS];				// info associated with device connections
	char szConections[MAX_CONNECTIONS][X_MAXENTRYLEN];	// device connection configuration
	int iterations;										// number of times to run test
	int delay;											// command delay
	long lConectionCount;								// number of device connections on this PC
	long verify;										// verify images?
	long removeImages;									// remove existing images before verification?
	int fudge;											// percentage image difference
	TCHAR szLogfile[256];								// log file
	TCHAR szCommand[2048];								// command to execute
	TCHAR szSnapshot[256];								// file name of saved snapshot
	TCHAR szRefdir[256];								// location of reference images
};
