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
*    This file contains headers of SafeQueue template class.
*/

#ifndef SAFEQUEUE_H
#define SAFEQUEUE_H

#include "sync.h"

//STL
#pragma warning (push, 3)
#pragma warning( disable : 4702 ) // Unreachable code warning
#pragma warning( disable : 4290 ) // C++ exception specification ignored except to indicate a function is not __declspec(nothrow)
#include <queue>




using namespace std;
class Data;

//**********************************************************************************
// Template Class SafeQueue
//
// This template class implements a thread safe queue with generic types
//**********************************************************************************

template <class T>
class SafeQueue : private queue<T>,
				  private Mutex,
				  private Semaphore
{
public:
    typedef typename queue<T>::size_type size_type;
    SafeQueue();
	~SafeQueue();
	/*
	 * Adds a new element to the end of the queue
	 */	
	void push(const T& val);
	/*
	 * Removes the next element in the queue
	 */	
	void pop(void);
	/*
	 * Returns whether the queue is empty or not
	 */	
	bool empty(void) const;
	/*
	 * Returns a reference to the last element in the queue
	 */	
	T& back(DWORD timeout = INFINITE) throw (TimeoutException);
	/*
	 * Returns a constant reference to the last element in the queue
	 */	
	const T& back(DWORD timeout = INFINITE) const throw (TimeoutException);
	/*
	 * Returns a reference to the next element in the queue
	 */	
	T& front(DWORD timeout = INFINITE) throw (TimeoutException);
	/*
	 * Returns a constant reference to the next element in the queue
	 */	
	const T& front(DWORD timeout = INFINITE) const throw (TimeoutException);
	/*
	 * Returns the number of elements in the queue
	 */	
	typename SafeQueue<T>::size_type size(void) const;
	/*
	 * This method is used to pop all of the elements in queue and return them in single Data object
	 */
	T& popAll(DWORD timeout = INFINITE) throw (TimeoutException);
};

template <class T>
SafeQueue<T>::SafeQueue()
{
}

template <class T>
SafeQueue<T>::~SafeQueue()
{
}

/*
 * Adds a new element to the end of the queue
 */
template <class T>
void SafeQueue<T>::push(const T& val)
{
	Mutex::Lock();
	queue<T>::push(val);
	Mutex::Unlock();
	Semaphore::Notify();
}

/*
 * Removes the next element in the queue
 */
template <class T>
void SafeQueue<T>::pop(void)
{
	Mutex::Lock();
	queue<T>::pop();
	Mutex::Unlock();
}

/*
 * Returns whether the queue is empty or not
 */
template <class T>
bool SafeQueue<T>::empty(void) const
{
	const_cast<SafeQueue<T>*>(this)->Lock();
	bool value = queue<T>::empty();
	const_cast<SafeQueue<T>*>(this)->Unlock();
	return value;
}

/*
 * Returns a reference to the last element in the queue
 */
template <class T>
T& SafeQueue<T>::back(DWORD timeout)
	throw (TimeoutException)
{
	if (Semaphore::Wait(timeout) == WAIT_TIMEOUT)
	{
		throw TimeoutException("queue back timed out");
	}
	Mutex::Lock();
	T& value = queue<T>::back();
	Mutex::Unlock();
	return value;
}

/*
 * Returns a constant reference to the last element in the queue
 */
template <class T>
const T& SafeQueue<T>::back(DWORD timeout) const
	throw (TimeoutException)
{
	if (const_cast<SafeQueue<T>*>(this)->Wait(timeout) == WAIT_TIMEOUT)
	{
		throw TimeoutException("queue back timed out");;
	}
	const_cast<SafeQueue<T>*>(this)->Lock();
	const T& value = queue<T>::back();
	const_cast<SafeQueue<T>*>(this)->Unlock();
	return value;
}

/*
 * Returns a reference to the next element in the queue
 */
template <class T>
T& SafeQueue<T>::front(DWORD timeout)
	throw (TimeoutException)
{
	if (Semaphore::Wait(timeout) == WAIT_TIMEOUT)
	{
		throw TimeoutException("queue front timed out");
	}
	Mutex::Lock();
	T& value = queue<T>::front();
	Mutex::Unlock();
	return value;
}

/*
 * Returns a constant reference to the next element in the queue
 */
template <class T>
const T& SafeQueue<T>::front(DWORD timeout) const
	throw (TimeoutException)
{
	if (const_cast<SafeQueue<T>*>(this)->Wait(timeout) == WAIT_TIMEOUT)
	{
		throw TimeoutException("queue front timed out");
	}
	const_cast<SafeQueue<T>*>(this)->Lock();
	const T& value = queue<T>::front();
	const_cast<SafeQueue<T>*>(this)->Unlock();
	return value;
}

/*
 * Returns the number of elements in the queue
 */
template <class T>
typename SafeQueue<T>::size_type SafeQueue<T>::size(void) const
{
	const_cast<SafeQueue<T>*>(this)->Lock();
	queue<T>::size_type value = queue<T>::size();
	const_cast<SafeQueue<T>*>(this)->Unlock();
	return value;
}


#pragma warning (pop)

#endif

// End of the file