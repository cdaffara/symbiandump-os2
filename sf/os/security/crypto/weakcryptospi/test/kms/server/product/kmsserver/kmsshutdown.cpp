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
* Stops key management server after a period when there have been
* no new connections.
*
*/


/**
 @file
*/


#include "kmsserver.h"


CKmsShutdown* CKmsShutdown::NewL(TInt aDelayUs)
/**
	Factory function allocates a new, initialized instance of CKmsShutdown.
	
	@param	aDelayUs		Delay in microseconds.
	@return					New, initialized instance of CKmsShutdown.
 */
	{
	CKmsShutdown* self = new(ELeave) CKmsShutdown(aDelayUs);
	CleanupStack::PushL(self);
	self->ConstructL();			// calls CTimer implementation
	CleanupStack::Pop(self);
	return self;
	}

CKmsShutdown::CKmsShutdown(TInt aDelayUs)
/**
	Record the delay in microseconds and add this object to
	the active scheduler.
	
	@param	aDelayUs		The  period in microseconds.
 */
:	CTimer(CActive::EPriorityStandard),
	iDelayUs(aDelayUs)
	{
	CActiveScheduler::Add(this);
	}

void CKmsShutdown::Restart()
/**
	Restart the timer.  The timer should not be active when this
	function is called.
	
	The timer is stopped with the base class' Cancel function.
 */
	{
	After(iDelayUs);
	}

void CKmsShutdown::RunL()
/**
	Implement CActive by stopping the server.
	(This actually stops the active scheduler.)
 */
	{
	CActiveScheduler::Stop();
	}

