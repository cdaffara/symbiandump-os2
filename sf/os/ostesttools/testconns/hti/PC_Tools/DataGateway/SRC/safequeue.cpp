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
*    This file contains implementation of SafeQueue template class popAll method.
*/

#include "safequeue.h"
#include "common.h"
/*
 * This method is used to pop all of the elements in queue and return them in single Data object
 */
template <>
(Data*)& SafeQueue<Data*>::popAll(DWORD timeout)
	throw (TimeoutException)
{
	if (Semaphore::Wait(timeout) == WAIT_TIMEOUT)
	{
		throw TimeoutException("queue front timed out");
	}
	Mutex::Lock();
	Data*& first = queue<Data*>::front();
	queue<Data*>::pop();
	//add to the first the rest of queue
	bool s = true;
	while ( queue<Data*>::size() > 0 && s)
	{
		Data*& a = queue<Data*>::front();
		int len = first->GetLength();
		first->AppendData( a );
		//check that size has changed
		if ( first->GetLength() != len )
		{
			queue<Data*>::pop();
			delete a;
		}
		else
		{
			s = false;
		}
	}
	Mutex::Unlock();
	return first;
}