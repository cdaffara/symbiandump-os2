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
* Description:  Global definitions for IPCommServer and client side interface.
*
*/


#ifndef __HTIIPCOMMSERVERCOMMON_H__
#define __HTIIPCOMMSERVERCOMMON_H__

//  INCLUDES
#include <e32base.h>

// CONSTANTS

// The server name (and server thread name)
_LIT( KIPCommServerName, "HtiIPCommServer" );
const TUid KIPCommServerUid3={0x200212DC};

// The version of the server
const TUint KIPCommServerMajorVersionNumber = 1;
const TUint KIPCommServerMinorVersionNumber = 0;
const TUint KIPCommServerBuildVersionNumber = 0;

// "Hihavakiot"
const TInt KIPCommServerReceiveBufferMaxSize = 0x1000; // 4096 bytes
const TInt KIPCommServerSendBufferMaxSize    = 0x1000; // 4096 bytes


// DATA TYPES

// The message ID's of IPCommServer
// from Symbian side
enum TIPCommServerRqst
    {
    EIPCommServerRecv,
    EIPCommServerSend,
    EIPCommServerCancelRecv,
    EIPCommServerCancelSend
    };


#endif // __HTIIPCOMMCLIENTSERVERCOMMON_H__

// End of File
