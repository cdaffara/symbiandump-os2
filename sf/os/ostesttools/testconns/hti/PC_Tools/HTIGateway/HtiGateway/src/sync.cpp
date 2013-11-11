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
*   This file contains implementation of synchronization objects. These are Mutex, Semaphore and
*   Event classes.
*/

// INCLUDES
#include "sync.h"

//**********************************************************************************
// Class Mutex
//
// This class implements a Mutex using Windows API synchronization mechanism 
//**********************************************************************************

Mutex::Mutex(void)
{
	InitializeCriticalSection(&m_Lock);
}

Mutex::~Mutex(void)
{
}

void Mutex::Lock(void)
{
	EnterCriticalSection(&m_Lock);
}

void Mutex::Unlock(void)
{
	LeaveCriticalSection(&m_Lock);
}

//**********************************************************************************
// Class Semaphore
//
// This class encapsulates Windows API Semaphore functionality
//**********************************************************************************

Semaphore::Semaphore(void)
{
	m_Semaphore = CreateSemaphore(NULL, 0, 0x7FFFFFF, NULL);
}

Semaphore::Semaphore(int available)
{
	m_Semaphore = CreateSemaphore(NULL, available, 0x7FFFFFF, NULL);
}

Semaphore::~Semaphore(void)
{
	CloseHandle(m_Semaphore);
}

DWORD Semaphore::Wait(DWORD timeout)
{
	DWORD dwResult = WaitForSingleObject(m_Semaphore, timeout);
	switch (dwResult)
	{
	case WAIT_OBJECT_0:
		return 0;
	case WAIT_TIMEOUT:
		return WAIT_TIMEOUT;
	default:
		return 1;
	}
}

void Semaphore::Notify(void)
{
	ReleaseSemaphore(m_Semaphore, 1, NULL);
}

void Semaphore::Notify(int how_many)
{
	ReleaseSemaphore(m_Semaphore, how_many, NULL);
}

//**********************************************************************************
// Class Event
//
// This class encapsulates Windows API Event functionality
//**********************************************************************************

Event::Event(bool manual_reset)
{
	m_Event = CreateEvent(NULL, manual_reset, false, NULL);
}

Event::~Event(void)
{
	CloseHandle(m_Event);
}

void Event::Set(void)
{
	SetEvent(m_Event);
}

void Event::Reset(void)
{
	ResetEvent(m_Event);
}

DWORD Event::Wait(DWORD timeout)
{
	DWORD dwResult = WaitForSingleObject(m_Event, timeout);
	switch (dwResult)
	{
	case WAIT_OBJECT_0:
		return 0;
	case WAIT_TIMEOUT:
		return WAIT_TIMEOUT;
	default:
		return 1;
	}
}

HANDLE Event::EventHandle() const
{
	return m_Event;
}

// End of the file