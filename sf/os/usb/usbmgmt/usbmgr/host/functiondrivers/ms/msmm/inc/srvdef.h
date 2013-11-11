/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/

/**
 @file
 @internalComponent
*/

#ifndef SRVDEF_H
#define SRVDEF_H

#include <e32base.h>

// Constants

// The MSMM server name
_LIT(KMsmmServerName, "!MSMM Server");

#ifdef __OVER_DUMMYCOMPONENT__
_LIT(KMsmmServerBinaryName, "msmmserver_over_dummycomponent.exe");
// server UID
const TUid KMsmmServUid = {0x1028653F};
#else
/** The MSMM server binary executable file name */
_LIT(KMsmmServerBinaryName, "msmmserver.exe");
/** The MSMM server UID */
const TUid KMsmmServUid = {0x10285c44};
#endif

#ifdef __OVER_DUMMYCOMPONENT__
// Just the FDF process can start the MSMM server
const TUint32 KFDFWSecureId = 0x10285c46; // SID for unit test project
#else
const TUint32 KFDFWSecureId = 0x10282B48;
#endif

#ifdef __OVER_DUMMYCOMPONENT__
const TUint32 KSidHbDeviceDialogAppServer = 0x20022FC4; // SID for unit test project
#else
const TUint32 KSidHbDeviceDialogAppServer = 0x20022FC5; //SID for indicator UI that will call eject usb drives
#endif

/** The Msmm Version numbers */
const TUint KMsmmServMajorVersionNumber = 1;
const TUint KMsmmServMinorVersionNumber = 0;
const TUint KMsmmServBuildVersionNumber = 1;

/** IPC messages supported by the server. */
enum TServMessage
    {
    EHostMsmmServerAddFunction = 0, // Add function request
    EHostMsmmServerRemoveDevice = 2, // Remove device request
    EHostMsmmServerDbgFailNext = 3,
    EHostMsmmServerDbgAlloc = 4,
    EHostMsmmServerEjectUsbDrives = 5, // Dismount USB drives request
    EHostMsmmServerEndMarker // Request end mark
    };

// Default number of message slots per session
const TUint KDefaultMessageSlots = 1;

// Max connection number
const TInt KMaxClientCount = 2;

#endif // #ifndef SRVDEF_H
