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
* Description:  Implementation of CHtiIPCommServerSession class. This class
*                represents the client session in server.
*
*/



// INCLUDE FILES
#include "HtiIPCommServerCommon.h"
#include "HtiIPCommServerSession.h"
#include "HtiIPCommLogging.h"

CHtiIPCommServerSession::CHtiIPCommServerSession( CHtiIPCommServer* aServer )
    :iServer( aServer )
    {
    HTI_LOG_TEXT( "CHtiIPCommServerSession::CHtiIPCommServerSession");
    iServer->SessionOpened();
    }

CHtiIPCommServerSession::~CHtiIPCommServerSession()
    {
    HTI_LOG_TEXT( "CHtiIPCommServerSession::~CHtiIPCommServerSession");
    if ( iServer )
        {
        iServer->SessionClosed();
        }
    }

void CHtiIPCommServerSession::ServiceL( const RMessage2& aMessage )
    {
    switch ( aMessage.Function() )
        {
        case EIPCommServerRecv:
            HandleReceiveRequestL( aMessage );
            break;

        case EIPCommServerSend:
            HandleSendRequestL( aMessage );
            break;

        case EIPCommServerCancelRecv:
            HandleCancelReceiveRequestL( aMessage );
            break;

        case EIPCommServerCancelSend:
            HandleCancelSendRequestL( aMessage );
            break;

        default:
            aMessage.Panic( _L("IPCommServer"), KErrNotFound );
            break;
        }
    }

void CHtiIPCommServerSession::HandleReceiveRequestL( const RMessage2& aMessage )
    {
    iServer->iConnectionManager->Receive( aMessage );
    }

void CHtiIPCommServerSession::HandleSendRequestL( const RMessage2& aMessage )
    {
    iServer->iConnectionManager->Send( aMessage );
    }

void CHtiIPCommServerSession::HandleCancelReceiveRequestL( const RMessage2& aMessage )
    {
    iServer->iConnectionManager->CancelReceive();
    aMessage.Complete( KErrNone );
    }

void CHtiIPCommServerSession::HandleCancelSendRequestL( const RMessage2& aMessage )
    {
    iServer->iConnectionManager->CancelSend();
    aMessage.Complete( KErrNone );
    }
