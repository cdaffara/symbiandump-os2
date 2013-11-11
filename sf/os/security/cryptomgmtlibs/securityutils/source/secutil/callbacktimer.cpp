/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Interface class for using the services of CCallbackTimer
*
*/


/**
 @file
*/

#include "callbacktimer.h"

// Implementation of class CCallbackTimer
EXPORT_C CCallbackTimer* CCallbackTimer::NewL(MTimerObserver& aCallBackIf, TBool aEnableCancelCallback/*= EFalse*/)
	{
	CCallbackTimer* self = new (ELeave) CCallbackTimer(aCallBackIf, aEnableCancelCallback);
	CleanupStack::PushL(self);
	self->ConstructL();		// calls CTimer::ConstructL()
	CleanupStack::Pop(self);
	return self;
	}

CCallbackTimer::CCallbackTimer(MTimerObserver& aCallBackIf, TBool aEnableCancelCallback)
	: CTimer(CActive::EPriorityStandard),
	iCallBackIf(aCallBackIf),
	iEnableCancelCallback(aEnableCancelCallback)
	{
	CActiveScheduler::Add(this);
	}

void CCallbackTimer::RunL()
	{
	// Notify the client
	// Don't inform about cancels if that feature is disabled
	TInt status = iStatus.Int();
	if (iEnableCancelCallback || status != KErrCancel)
		{
		iCallBackIf.TimerRun(status);
		}
	}

