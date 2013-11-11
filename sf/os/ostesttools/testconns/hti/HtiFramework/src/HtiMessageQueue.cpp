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
* Description:  CHtiMessageQueue implementation
*
*/


#include "HtiMessageQueue.h"

CHtiMessageQueue* CHtiMessageQueue::NewL()
    {
    return new (ELeave) CHtiMessageQueue;
    }

CHtiMessageQueue::CHtiMessageQueue():
    iQueue( CHtiMessage::iLinkOffset ),
    iMemoryAllocated( 0 ),
    iQueueIter( iQueue ),
    iIsFirst ( ETrue )
    {
    }

CHtiMessageQueue::~CHtiMessageQueue()
    {
    RemoveAll();
    }

void CHtiMessageQueue::RemoveAll()
    {
    if ( !IsEmpty() )
        {
        CHtiMessage* msg;

        iQueueIter.SetToFirst();
        while ( ( msg = iQueueIter++ ) != NULL )
            {
            iQueue.Remove( *msg );
            delete msg;
            }
        }
        iMemoryAllocated = 0;
    }

CHtiMessage* CHtiMessageQueue::Remove()
    {
    CHtiMessage* msg = NULL;
    if ( !IsEmpty() )
        {
        msg = iQueue.First();
        iQueue.Remove( *msg );
        iMemoryAllocated -= msg->Size();
        }
    return msg;
    }

CHtiMessage* CHtiMessageQueue::Remove( CHtiMessage* aMsg)
    {
    if ( aMsg && !IsEmpty() )
        {
        iQueue.Remove( *aMsg );
        iMemoryAllocated -= aMsg->Size();
        return aMsg;
        }
    return NULL;
    }

void CHtiMessageQueue::StartServiceIteration()
    {
    if ( !IsEmpty() )
        {
        iQueueIter.SetToFirst();
        iIsFirst = ETrue;
        }
    }

CHtiMessage* CHtiMessageQueue::GetNext()
    {
    CHtiMessage* msg = NULL;
    if ( !IsEmpty() )
        {
        while ( ( msg = iQueueIter++ ) != NULL )
            {
            if ( iIsFirst )
                {
                iIsFirst = EFalse;
                iLastServiceUid = msg->DestinationServiceUid();
                return msg;
                }
            else if ( msg->DestinationServiceUid() != iLastServiceUid )
                {
                iLastServiceUid = msg->DestinationServiceUid();
                return msg;
                }
            }
        }
    return msg;
    }


void CHtiMessageQueue::Add( CHtiMessage& aMessage )
    {
    iQueue.AddLast( aMessage );
    iMemoryAllocated += aMessage.Size();
    }

TBool CHtiMessageQueue::IsEmpty() const
    {
    return iQueue.IsEmpty();
    }

TInt CHtiMessageQueue::QueueSize() const
    {
    return iMemoryAllocated;
    }
