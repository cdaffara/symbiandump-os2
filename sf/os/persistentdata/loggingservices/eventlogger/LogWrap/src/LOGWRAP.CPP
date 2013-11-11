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

#include <e32uid.h>

#ifndef NO_LOG_ENGINE_IN_ROM
#include <logcli.h>
#endif

#include <logwrap.h> 
#include "LOGPANIC.H"

EXPORT_C CLogWrapper::~CLogWrapper()
/** Frees all resources owned by the log wrapper prior to its destruction. Specifically, 
it deletes the Log Engine (or the instance of the Log Engine base class, if 
the UI variant does not have a Log Engine installed). */
	{
	delete iBase;
	}

EXPORT_C CLogWrapper* CLogWrapper::NewL(RFs& aFs, TInt aPriority/* = CActive::EPriorityStandard*/)
	{
	CLogWrapper* self = new(ELeave)CLogWrapper();
	CleanupStack::PushL(self);
	self->ConstructL(aFs, aPriority);
	CleanupStack::Pop(); // self
	return self;
	}

CLogWrapper::CLogWrapper()
	{
	}

#ifndef NO_LOG_ENGINE_IN_ROM
void CLogWrapper::ConstructL(RFs& aFs, TInt aPriority)
	{
	iBase = CLogClient::NewL(aFs, aPriority);
	}
#else
#pragma BullseyeCoverage off
void CLogWrapper::ConstructL(RFs&, TInt aPriority)
	{
	iBase = new(ELeave)CLogBase(aPriority);
	}
#pragma BullseyeCoverage on
#endif

EXPORT_C TBool CLogWrapper::ClientAvailable() const
/** Determines whether the Log Engine is installed.

@return ETrue if there is a Log Engine, EFalse, otherwise. */
	{
#ifndef NO_LOG_ENGINE_IN_ROM
	return ETrue;
#else
	return EFalse;
#endif
	}
