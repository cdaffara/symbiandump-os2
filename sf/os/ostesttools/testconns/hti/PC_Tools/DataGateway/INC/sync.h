/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
*   This file contains headers of synchronziation objects. These classes are Mutex, Semaphore and
*   Event classes. Implementation of TimeoutException is defined.
*/

#ifndef SYNC_H
#define SYNC_H

#include <windows.h>
#include <process.h>

#include <string>

using namespace std;

//**********************************************************************************
// Class TimeoutException
//
//**********************************************************************************

class TimeoutException
{
public:
	TimeoutException(const char* s = "") { m_Message.assign(s); }
	const string& GetMessage() const { return m_Message; }
private:	
	string m_Message;
};

//**********************************************************************************
// Class Mutex
//
// This class implements a Mutex using Windows API synchronization mechanism 
//**********************************************************************************


class Mutex
{
public:
	Mutex(void);
	~Mutex(void);
	void Lock(void);
	void Unlock(void);
private:
	CRITICAL_SECTION m_Lock;
};

//**********************************************************************************
// Class Semaphore
//
// This class encapsulates Windows API Semaphore functionality
//**********************************************************************************

class Semaphore
{
public:
	Semaphore(void);
	Semaphore(int available);
	~Semaphore(void);
	DWORD Wait(DWORD timeout = INFINITE);
	void Notify(void);
	void Notify(int how_many);
private:
	HANDLE m_Semaphore;
};

//**********************************************************************************
// Class Event
//
// This class encapsulates Windows API Event functionality
//**********************************************************************************

class Event
{
public:
	Event(bool manual_reset = false);
	~Event(void);
	void Set(void);
	void Reset(void);
	DWORD Wait(DWORD timeout = INFINITE);
	HANDLE EventHandle() const;
private:
	HANDLE m_Event;
};

#endif

// End of the file