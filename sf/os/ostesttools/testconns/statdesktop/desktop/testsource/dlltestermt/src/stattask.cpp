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
#include "STATTask.h"

//////////////////////////////////////////////////////////////////////////
// Constructor
// Takes 3 parameters: 
// 1) Pointer to function to start the thread in
// 2) Pointer to class object to use within the thread
// 3) Optional string identifier
//////////////////////////////////////////////////////////////////////////
STATTask::STATTask(const ThreadProc pFunc, void *pMember, char *pConnection)
{
	pfThreadProc = pFunc;
	theMember = pMember;
	szConection = NULL;

	if (pConnection)
	{
		szConection = new char[strlen(pConnection) + 1];
		if (szConection)
			strcpy(szConection, pConnection);
	}
}


//////////////////////////////////////////////////////////////////////////
// Destructor
STATTask::~STATTask()
{
	Kill();

	if (szConection)
		delete [] szConection;
}


//////////////////////////////////////////////////////////////////////////
// Create the thread
bool STATTask::Start()
{
	bool valid = true;

	// event attributes for the child process
	SECURITY_ATTRIBUTES eventAttr;
	eventAttr.nLength = sizeof(eventAttr);
	eventAttr.lpSecurityDescriptor = NULL;
	eventAttr.bInheritHandle = TRUE;

	// spawn a thread to do the processing
	DWORD dwThreadID;
	if (!(hThreadHandle = CreateThread( NULL,			// security attributes
										0,				// stack size
										pfThreadProc,	// proc to call
										theMember,		// proc parameter
										0,				// creation flags
										&dwThreadID)))	// thread identifier
	{
		valid = false;
	}

	return valid;
}


//////////////////////////////////////////////////////////////////////////
// Check to see if thread has finished processing
bool STATTask::StillActive(DWORD timeout)
{
	bool valid = false;

	if (hThreadHandle)
	{
		if (WAIT_TIMEOUT == WaitForSingleObject(hThreadHandle, timeout))
		{
			valid = true;
		}
	}

	return valid;
}


//////////////////////////////////////////////////////////////////////////
// Kill the thread
bool STATTask::Kill()
{
	bool valid = true;

	if (hThreadHandle)
	{
		if (!TerminateThread(hThreadHandle, 0))
		{
			valid = false;
		}

		hThreadHandle = 0;
	}

	return valid;
}

