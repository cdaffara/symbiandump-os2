// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//


/**
   @file
   @internalComponent
*/

#ifndef PRIMSGQUEUE_H
#define PRIMSGQUEUE_H

#include <e32std.h>
#include <e32msgqueue.h>

/**
   RPriMsgQueue panic category
*/
_LIT(RPriMsgQueuePanicCategory, "RPriMsgQueue");

/**
   A priority-based message queue similar to RMsQueue .

   This templated class provides the behaviour for managing an asynchronous
   queue of messages, ordered by descending priority order, where the template
   parameter defines the message type. Note that this message queue is intended
   to be used with pointer types only. Also, message types are assumed to have
   a member with the following signature : TInt Priority()

 */
template <typename T>
class RPriMsgQueue
    {

public:

    inline TInt CreateLocal(TInt aSize, TOwnerType aType=EOwnerProcess);
	inline TInt Handle() const;
    inline TInt Send(const T& aMsg);
    inline TInt Receive(T& aMsg);
	inline void Close();
	inline void NotifyDataAvailable(TRequestStatus& aStatus);
	inline void CancelDataAvailable();

protected:

	class TFrontQueueElement
		{

	public:

		inline explicit TFrontQueueElement(const TAny* apInfo);
		inline TFrontQueueElement(const TAny* apInfo, TInt aPriority);

	public:

		const TAny* ipInfo; 
		TPriQueLink iLink;

		};

protected:

	inline TInt DrainBackQueue();

protected:

	RMsgQueueBase iBackQueue;
	TPriQue<TFrontQueueElement> iFrontQueue;

    };

#include "primsgqueue.inl"

#endif // PRIMSGQUEUE_H
