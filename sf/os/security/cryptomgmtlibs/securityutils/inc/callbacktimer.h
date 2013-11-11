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
* Contains functionality for a callback timer
*
*/


/**
 @file
 @publishedPartner
 @released
*/

#ifndef __CALLBACKTIMER_H__
#define __CALLBACKTIMER_H__

#include <e32base.h>

/**
 * Interface class for using the services of CCallbackTimer
 */
class MTimerObserver
	{
public:
	virtual ~MTimerObserver() {};

	/**
	 * Call back function to handle the expiry of the timer
	 * @param aError	KErrNone if timer expired normally else any of the system-wide error codes to indicate a system error
	 */
	virtual void TimerRun(TInt aError) = 0;
	};

/**
 * A timer class that provides a call back on timer expiry
 */
class CCallbackTimer : public CTimer
	{
	public:

	/**
	 * Create and return a new instance of the CCallbackTimer. An ongoing timer operation can be cancelled by the client by calling the Cancel() method on this object.
	 * @param aCallBackIf			Callback interface that implements TimerRun() function
	 * @param aEnableCancelCallback	If ETrue cancel events (client initiated by calling Cancel() method) are notified. Default is EFalse
	 */
	IMPORT_C static CCallbackTimer* NewL(MTimerObserver& aCallBackIf, TBool aEnableCancelCallback = EFalse);

private:
	CCallbackTimer(MTimerObserver& aCallBackIf, TBool aEnableCancelCallback);

	// Methods from CTimer/CActive
	void RunL();

private:
	MTimerObserver& iCallBackIf;
	TBool iEnableCancelCallback;
	};

#endif // __CALLBACKTIMER_H__
