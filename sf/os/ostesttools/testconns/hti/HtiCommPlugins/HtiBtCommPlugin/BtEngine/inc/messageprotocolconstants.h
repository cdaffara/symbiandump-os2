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
* Description:  Constants for Bluetooth.
*
*/


#ifndef __CMESSAGE_PROTOCOL_CONSTANTS__
#define __CMESSAGE_PROTOCOL_CONSTANTS__

// INCLUDES
#include <bt_sock.h>

const TInt KRfcommChannel = 1;
const TInt KServiceClass = 0x1101;    //  SerialPort

const TUid KUidBTPointToPointApp = { 0x10005B8B };

_LIT( KStrRFCOMM,             "RFCOMM" );

const TInt KListeningQueSize = 1;


#endif //__CMESSAGE_PROTOCOL_CONSTANTS__

// End of File
