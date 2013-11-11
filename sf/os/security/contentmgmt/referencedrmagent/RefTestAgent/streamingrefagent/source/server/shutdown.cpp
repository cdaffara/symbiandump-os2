/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
* Implements the timer which stops the server after no sessions have been
* open for a defined period.
*
*/


#include "sraserver.h"


CShutdownTimer* CShutdownTimer::NewL(TInt aDelay)
/**
	Factory function allocates a new, initialized instance of CShutdownTimer.

	@param	aDelay	Delay in microseconds.
	@return			New, initialized instance of CShutdownTimer.
					The newly-created object is owned by the caller.
 */
	{
	CShutdownTimer* self = new(ELeave) CShutdownTimer(aDelay);
	CleanupStack::PushL(self);
	self->ConstructL();				// CTimer implementation
	CleanupStack::Pop(self);
	return self;
	}


CShutdownTimer::CShutdownTimer(TInt aDelay)
/**
	Record the shutdown period and add this object to the active scheduler.

	@param	aDelayUs		Delay in microseconds.
 */
 :	CTimer(CActive::EPriorityStandard),
	iDelay(aDelay)
	{
	CActiveScheduler::Add(this);
	}

void CShutdownTimer::Restart()
/**
	Restart this timer.  This timer should not be active when this
	function is called.

	Once started, the timer can be stopped with the base class' Cancel function.
 */
	{
	// Start idle timeout
	After(iDelay);
	}


void CShutdownTimer::RunL()
/**
	Implement CActive::RunL by stopping the active scheduler, which 
	has the effect of breaking out of the server loop.
 */
	{
	CActiveScheduler::Stop();
	}
