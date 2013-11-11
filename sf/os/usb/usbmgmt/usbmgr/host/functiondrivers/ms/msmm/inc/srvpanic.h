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

#ifndef SRVPANIC_H
#define SRVPANIC_H

#include <e32std.h>
#include <e32cmn.h>

// Constant

// Category
_LIT(KMsmmPanicServer, "MSMM Server");
_LIT(KMsmmPanicClient, "MSMM Client");

// Panic number
enum TMsmmServerPanic
    {
    ENoPolicyPlugin = 0x01, // Missing policy plugin when starting server.
    ENotSupported = 0x02,
    EServerPanicMark
    };

enum TMsmmClientPanic
    {
    EBadRequest = 0x01,
    EConnectionExist = 0x02, // Currently there is a connection to server.
    ENoSupportedVersion = 0x03, // Server can not support this version.
    EClientPanicMark
    };

inline void PanicServer(TMsmmServerPanic aPanic)
    {
    User::Panic(KMsmmPanicServer, aPanic);
    }

inline void PanicClient(const RMessage2& aMsg, TMsmmClientPanic aPanic)
    {
    aMsg.Panic(KMsmmPanicClient, aPanic);
    }

#endif /*SRVPANIC_H*/
