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




#ifndef STATTASK_H
#define STATTASK_H

// pointer to our thread procedure
typedef DWORD (WINAPI *ThreadProc) (LPVOID);

class STATTask
{
public:
	STATTask(const ThreadProc pFunc, void *pMember, char *pConection = NULL);
	~STATTask();
	bool Start();
	bool StillActive(DWORD timeout = 5);
	bool Kill();
	void * Member() { return theMember; }
	char * Connection() { return szConection; }

private:
	HANDLE hThreadHandle;				// handle to this thread
	ThreadProc pfThreadProc;
	void *theMember;					// pointer to associated object
	char *szConection;					// connection string
};

#endif
