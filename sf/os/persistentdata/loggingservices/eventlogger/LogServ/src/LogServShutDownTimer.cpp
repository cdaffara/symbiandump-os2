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

#include "LogServShutdownTimer.h"


/////////////////////////////////////////////////////////////////////////////////////////
// -----> CLogServShutdownTimer (source)
/////////////////////////////////////////////////////////////////////////////////////////

CLogServShutdownTimer::CLogServShutdownTimer(TInt aPriority)
:	CTimer(aPriority)
	{
	CActiveScheduler::Add(this);
	}

void CLogServShutdownTimer::ConstructL()
	{
	CTimer::ConstructL();
	}

CLogServShutdownTimer* CLogServShutdownTimer::NewL(TInt aPriority)
	{
	CLogServShutdownTimer* self = new(ELeave) CLogServShutdownTimer(aPriority);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

void CLogServShutdownTimer::Start()
	{
	After(KLogShutdownDelay);
	}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

void CLogServShutdownTimer::RunL()
	{
	// Initiate server exit when the timer expires
	CActiveScheduler::Stop();
	}
