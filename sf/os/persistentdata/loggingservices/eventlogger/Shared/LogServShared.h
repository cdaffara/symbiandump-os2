// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#ifndef __LOGSERVSHARED_H__
#define __LOGSERVSHARED_H__

// System includes
#include <e32std.h>


/**
@internalComponent
*/
const TUid KServerUid3 = {0x0101f401d};

/**
Literal constants
Used to find the process.exe filename to launch (Don't change!)
@internalComponent
*/
_LIT(KLogServerName, "LogServ");
_LIT(KLogServerFullName, "!LogServ");

#endif
