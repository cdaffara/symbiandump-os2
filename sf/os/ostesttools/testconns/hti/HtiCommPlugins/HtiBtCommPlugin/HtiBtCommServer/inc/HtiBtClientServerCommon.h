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
* Description:  Global definitions for BtCommServer and client side interface.
*
*/


#ifndef __HTIBTCLIENTSERVERCOMMON_H__
#define __HTIBTCLIENTSERVERCOMMON_H__

//  INCLUDES
#include <e32base.h>

// CONSTANTS

// The server name (and server thread name)
_LIT( KBtCommServerName,"HtiBtCommServer" );

// The version of the server
const TUint KBtCommServerMajorVersionNumber = 1;
const TUint KBtCommServerMinorVersionNumber = 0;
const TUint KBtCommServerBuildVersionNumber = 0;

const TInt KClientReceiveBufferMaxSize = 0x1000; // 4096 bytes
const TInt KClientSendBufferMaxSize = 0x1000; // 4096 bytes


// DATA TYPES

// The message ID's of BtCommServer
// from Symbian side
enum TBtCommServerRqst
    {
    EBtCommServerConnect = 0,
    EBtCommServerRecv,
    EBtCommServerSend,
    ECancelBtCommServerRecv,
    ECancelBtCommServerSend,
    EGetServicePortNumber
    };


// FUNCTION PROTOTYPES
/**
* Start HtiBtCommServer thread. This is called by client interface.
*/
IMPORT_C TInt StartThread();

#endif // __HTIBTCLIENTSERVERCOMMON_H__

// End of File
