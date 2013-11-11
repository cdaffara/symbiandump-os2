// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef PANIC_H
#define PANIC_H

#include <e32std.h>
#include "clientrequest.h"

enum TPanic
	{
	ESessionNotInitialised,			// 0 PanicClient only.
	ESessionAlreadyInitialised,		// 1 PanicClient only.
	ERepositoryNotFound,			// 2 Not used.
	EStartAlreadyInTransaction,		// 3 PanicClient only.
	ECommitNotInTransaction,		// 4 PanicClient only.
	EBadSubsessionHandle,			// 5 PanicClient only.
	EBadMessageNumber,				// 6 PanicClient only.
	ECacheIniFileCorrupted			// 7 Panic server only (debug only).
#ifdef SYMBIAN_CENTREP_SUPPORT_MULTIROFS	
	,EMultiRofsPanicOldCre,
	EMultiRofsPanicGlobalOverride,
	EMultiRofsPanicTypeOveride,
	EMultiRofsPanicIllegalRofs
#endif
	};

GLREF_C void Panic(TPanic aReason);

GLREF_C void PanicClient(TPanic aReason, const TClientRequest& aMessage);

#endif // PANIC_H
