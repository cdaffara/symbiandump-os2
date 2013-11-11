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
*   This file contains the header file of template class Thread and ThreadData.
*	Interface Runnable is defined.
*/

#ifndef THREAD_H
#define THREAD_H

#include "sync.h"
#include <process.h>


typedef unsigned (__stdcall *THREAD_START_FUNC) (void *);


//**********************************************************************************
// Template Class ThreadData
//
//**********************************************************************************

template <class T>
struct ThreadData
{
	public:
		typedef void (T::*TFunc)();
		HANDLE m_hEvent;
		T* m_pThreadObject;
		TFunc m_pThreadFunc;
		static DWORD _ThreadFunc(ThreadData<T>* pThis)
		{
			ThreadData<T> td = *pThis;
			SetEvent(td.m_hEvent);
			((*(td.m_pThreadObject)).*(td.m_pThreadFunc))();
			return 0;
		}
};

//**********************************************************************************
// Template Class Runnable
// 
//**********************************************************************************

class Runnable
{
	public:
		virtual void Run() = 0;
		virtual void Stop() = 0;
};

//**********************************************************************************
// Template Class Thread
//
// This is the template for thread class, implemented by all threads in DataGateway
// The template encapsulates Windows API tread functionality
//**********************************************************************************

template <class T>
class Thread : public Runnable
{
	public:
		Thread();
		Thread(Runnable* runnable);
		~Thread();
		void Run();
		void Start();
		void Stop();
		void Suspend();
		void Resume();
		HANDLE ThreadHandle();
	private:
		HANDLE CreateMemberThread(Runnable* p,
			                      void (Runnable::*func)());
	private:
		HANDLE m_hMemberThread;
		Runnable* m_Runnable;
};

template <class T>
Thread<T>::Thread()
{
	m_hMemberThread = NULL;
	m_Runnable = this;
}

template <class T>
Thread<T>::Thread(Runnable* runnable)
{
	m_hMemberThread = NULL;
	if (runnable != NULL)
	{
		m_Runnable = runnable;
	}
	else
	{
		m_Runnable = this;
	}
}

template <class T>
Thread<T>::~Thread()
{
	if (m_hMemberThread != NULL)
	{
		CloseHandle(m_hMemberThread);
	}
	m_Runnable = NULL;
}

template <class T>
void Thread<T>::Run()
{
}

template <class T>
void Thread<T>::Suspend()
{
	if (m_hMemberThread != NULL)
	{
		SuspendThread(m_hMemberThread);
	}
}

template <class T>
void Thread<T>::Resume()
{
	if (m_hMemberThread != NULL)
	{
		ResumeThread(m_hMemberThread);
	}
}

template <class T>
void Thread<T>::Start()
{
	if (m_Runnable != NULL)
	{
		m_hMemberThread = CreateMemberThread(m_Runnable,
											 &Runnable::Run);
		if (m_hMemberThread == NULL)
		{
			cerr << "Thread: could not create thread" << endl;
		}		
	}
	else
	{
		m_hMemberThread = NULL;
		cerr << "Thread: threadfunc NULL" << endl;
	}
}

template <class T>
void Thread<T>::Stop()
{
	m_Runnable->Stop();
}

template <class T>
HANDLE Thread<T>::ThreadHandle()
{
	return m_hMemberThread;
}

template <class T>
HANDLE Thread<T>::CreateMemberThread(Runnable* p,
									 void (Runnable::*func)())
{
	ThreadData<Runnable> td;
	td.m_pThreadObject = p;
	td.m_pThreadFunc = func;
	td.m_hEvent = CreateEvent(NULL, 0, 0, NULL);

	unsigned int Dummy;
	HANDLE ThreadHandle = (HANDLE)_beginthreadex(NULL,
		                               NULL,
									   (THREAD_START_FUNC)ThreadData<Runnable>::_ThreadFunc,
									   &td,
									   NULL,
									   &Dummy);

	WaitForSingleObject(td.m_hEvent,INFINITE);
	CloseHandle(td.m_hEvent);
	return ThreadHandle;
}

#endif

// End of the file