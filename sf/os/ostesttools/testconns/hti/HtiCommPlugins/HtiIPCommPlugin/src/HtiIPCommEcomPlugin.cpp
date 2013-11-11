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
* Description:  ECOM plugin for communication over IP port
*
*/


// INCLUDE FILES
#include "HtiIPCommEcomPlugin.h"
#include <HtiLogging.h>

CHtiIPCommEcomPlugin* CHtiIPCommEcomPlugin::NewL()
    {
    CHtiIPCommEcomPlugin* self = new (ELeave) CHtiIPCommEcomPlugin();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

CHtiIPCommEcomPlugin::CHtiIPCommEcomPlugin()
    {
    }

CHtiIPCommEcomPlugin::~CHtiIPCommEcomPlugin()
    {
    iHtiIPCommServer.Close();
    }

void CHtiIPCommEcomPlugin::ConstructL()
    {
    TInt err = iHtiIPCommServer.Connect();
    if ( err )
        {
        HTI_LOG_FORMAT( "error connecting to HtiIPCommServer %d", err );
        User::Leave( err );
        }
    }

void CHtiIPCommEcomPlugin::Receive( TDes8& aRawdataBuf, TRequestStatus& aStatus )
    {
    iHtiIPCommServer.Receive( aRawdataBuf, aStatus );
    }

void CHtiIPCommEcomPlugin::Send( const TDesC8& aRawdataBuf, TRequestStatus& aStatus )
    {
    iHtiIPCommServer.Send( aRawdataBuf, aStatus );
    }

void CHtiIPCommEcomPlugin::CancelReceive()
    {
    iHtiIPCommServer.CancelReceive();
    }

void CHtiIPCommEcomPlugin::CancelSend()
    {
    iHtiIPCommServer.CancelSend();
    }

TInt CHtiIPCommEcomPlugin::GetSendBufferSize()
    {
    return iHtiIPCommServer.GetSendBufferSize();
    }

TInt CHtiIPCommEcomPlugin::GetReceiveBufferSize()
    {
    return iHtiIPCommServer.GetReceiveBufferSize();
    }


