/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* This is the Shared header needed for implementing the Obex Class Controller
* configuration.
*
*/

#include <e32base.h>
// server name

_LIT(KServerName,"ObexServer");

// Server location

_LIT(KServerExe,"z:\\sys\\bin\\classControllerServerSession.exe");

const TUint32 KObexCCUidTUint = 0x10281916;
const TUid KObexCCUid = {KObexCCUidTUint};


// A version must be specifyed when creating a session with the server

const TUint KMajorVersionNumber=1;
const TUint KMinorVersionNumber=0;
const TUint KBuildVersionNumber=0;
const TInt KMessageSlots=-1;

/**
 * Declaration of function
 */
IMPORT_C TInt StartThread(RThread& aServerThread);

enum TServRqst
	{
	EStartClassContServer,
	EStopClassContServer
	};
