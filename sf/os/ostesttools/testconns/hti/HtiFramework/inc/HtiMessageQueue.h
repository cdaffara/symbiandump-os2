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
* Description:  Simple FIFO Queue class based on single-linked list
*        (TSglQue<CHtiMessage>). It alsoe provides functionality
*        to monitor memory allocated by all messages.
*
*/


#ifndef MESSAGEQUEUE_H__
#define MESSAGEQUEUE_H__

#include "HtiMessage.h"

NONSHARABLE_CLASS(CHtiMessageQueue)  : public CBase
    {
public:
    /**
    * Create a new queue
    *
    */
    static CHtiMessageQueue* NewL();

    virtual ~CHtiMessageQueue();

    /**
    * Adds a new message to the queue
    *
    * @param aMessage instance of ready HTI message
    */
    void Add(CHtiMessage& aMessage);

    /**
    * Retrives a message from the queue
    * and remove it from the queue
    *
    * @return a HTI message
    */
    CHtiMessage* Remove();

    /**
    * Removes specifeda message from the queue
    *
    * @param aMsg message to remove
    * @return a HTI message
    */
    CHtiMessage* Remove( CHtiMessage* aMsg);

    /**
    * Deletes all messages from queue
    */
    void RemoveAll();

    /**
    * Starts iteration based on service names
    * Each call to GetNext() will return next message to the service
    * different from the service name in the current message
    */
    void StartServiceIteration();

    /**
    * Returns next message with service name different
    * than in the current message.
    * This method does not remove message from the queue, so ownership
    * is not transfered. Remove( CHtiMessage* aMsg) should be used
    * to delete the message from a queue.
    *
    * @return a HTI message or NULL if there is no messages anymore
    */
    CHtiMessage* GetNext();


    /**
    * Checks that the queue is empty
    *
    * @return ETrue if queue is empty
    */
    TBool IsEmpty() const;

    /**
    * Returns memory allocated by all messages in the queue
    */
    TInt QueueSize() const;

protected:
    CHtiMessageQueue();

protected:
    /**
    * Queue
    */
    TSglQue<CHtiMessage> iQueue;

    /**
    * Current memory allocated by hti messages in the queue
    */
    TInt iMemoryAllocated;

    /**
    * queue iterator
    */
    TSglQueIter<CHtiMessage> iQueueIter;

    /**
    * last service uid
    * used during iteration by service names
    */
    TUid iLastServiceUid;

    /**
    * Used to ommit service name checking for the first GetNext()
    */
    TBool iIsFirst;
    };

#endif
