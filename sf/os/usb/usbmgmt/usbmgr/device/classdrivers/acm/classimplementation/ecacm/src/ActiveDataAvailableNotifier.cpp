/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32std.h>
#include "ActiveDataAvailableNotifier.h"
#include "AcmConstants.h"
#include "AcmPanic.h"
#include "AcmUtils.h"
#include "NotifyDataAvailableObserver.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "ActiveDataAvailableNotifierTraces.h"
#endif



CActiveDataAvailableNotifier::CActiveDataAvailableNotifier(
								MNotifyDataAvailableObserver& aParent, 
								RDevUsbcClient& aLdd,
								TEndpointNumber aEndpoint)
 :	CActive(KEcacmAOPriority), 
	iParent(aParent),
	iLdd(aLdd),
	iEndpoint(aEndpoint)
/**
 * Constructor.
 *
 * @param aParent The object that will be notified if a 
 * NotifyDataAvailable() request has been made and incoming data 
 * arrives at the LDD.
 * @param aLdd The LDD handle to be used for posting read requests.
 * @param aEndpoint The endpoint to read from.
 */
	{
	OstTraceFunctionEntry0( CACTIVEDATAAVAILABLENOTIFIER_CACTIVEDATAAVAILABLENOTIFIER_CONS_ENTRY );
	CActiveScheduler::Add(this);
	OstTraceFunctionExit0( CACTIVEDATAAVAILABLENOTIFIER_CACTIVEDATAAVAILABLENOTIFIER_CONS_EXIT );
	}

CActiveDataAvailableNotifier::~CActiveDataAvailableNotifier()
/**
 * Destructor.
 */
	{
	OstTraceFunctionEntry0( CACTIVEDATAAVAILABLENOTIFIER_CACTIVEDATAAVAILABLENOTIFIER_DES_ENTRY );
	Cancel();
	OstTraceFunctionExit0( CACTIVEDATAAVAILABLENOTIFIER_CACTIVEDATAAVAILABLENOTIFIER_DES_EXIT );
	}

CActiveDataAvailableNotifier* CActiveDataAvailableNotifier::NewL(
								MNotifyDataAvailableObserver& aParent, 
								RDevUsbcClient& aLdd,
								TEndpointNumber aEndpoint)
/**
 * Standard two phase constructor.
 *
 * @param aParent The object that will be notified if a 
 * NotifyDataAvailable() request has been made and incoming data 
 * arrives at the LDD.
 * @param aLdd The LDD handle to be used for posting read requests.
 * @param aEndpoint The endpoint to read from.
 * @return Ownership of a new CActiveReadOneOrMoreReader object.
 */
	{
	OstTraceFunctionEntry0( CACTIVEDATAAVAILABLENOTIFIER_NEWL_ENTRY );
	CActiveDataAvailableNotifier* self = 
		new(ELeave) CActiveDataAvailableNotifier(aParent, aLdd, aEndpoint);
	OstTraceFunctionExit0( CACTIVEDATAAVAILABLENOTIFIER_NEWL_EXIT );
	return self;
	}

void CActiveDataAvailableNotifier::NotifyDataAvailable()
/**
 * When incoming data arrives at the LDD notify the caller.
 */
	{
	OstTraceFunctionEntry0( CACTIVEDATAAVAILABLENOTIFIER_NOTIFYDATAAVAILABLE_ENTRY );
	iLdd.ReadOneOrMore(iStatus, iEndpoint, iUnusedBuf, 0);
	SetActive();
	OstTraceFunctionExit0( CACTIVEDATAAVAILABLENOTIFIER_NOTIFYDATAAVAILABLE_EXIT );
	}

void CActiveDataAvailableNotifier::DoCancel()
/**
 * Cancel an outstanding request.
 */
	{
	OstTraceFunctionEntry0( CACTIVEDATAAVAILABLENOTIFIER_DOCANCEL_ENTRY );
	iLdd.ReadCancel(iEndpoint);
	OstTraceFunctionExit0( CACTIVEDATAAVAILABLENOTIFIER_DOCANCEL_EXIT );
	}

void CActiveDataAvailableNotifier::RunL()
/**
 * This function will be called when the zero byte ReadOneOrMore() call on the LDD
 * completes. This could have been caused by the receipt of a Zero Length Packet in
 * which case there is no data available to be read. In this situation 
 * NotifyDataAvailable() is called again, otherwise the parent is notified.
 * We also have to be careful about getting into an infinite loop if the cable has 
 * been detached.
 */
	{
	OstTraceFunctionEntry0( CACTIVEDATAAVAILABLENOTIFIER_RUNL_ENTRY );
	OstTrace1( TRACE_NORMAL, CACTIVEDATAAVAILABLENOTIFIER_RUNL, "CActiveDataAvailableNotifier::RunL;iStatus=%d", iStatus.Int() );
	TBool complete = EFalse;
	TInt completeErr = KErrNone;

	TInt recBufSize;
	TInt err = iLdd.QueryReceiveBuffer(iEndpoint, recBufSize);
	if ( err == KErrNone )
		{
		if ( recBufSize != 0 )
			{
			// There is data available.
			complete = ETrue;
			completeErr = KErrNone;
			}
		else
			{
			// There is no data available. This may be because we got a ZLP, but 
			// before we simply repost the notification, check to see if the LDD 
			// is still working. If there isn't then we should complete to the 
			// client to avoid getting ourselves into an infinite loop.
			if ( iStatus.Int() == KErrNone )
				{
				NotifyDataAvailable();
				}
			else
				{
				complete = ETrue;
				// The Active Reader and Active Writer objects pass LDD-specific 
				// errors straight up the stack, so I don't see a problem with 
				// doing the same here. [As opposed to genericising them into 
				// KErrCommsLineFail for instance.]
				completeErr = iStatus.Int();
				}
			}
		}
	else // QueryReceiveBuffer failed
		{
		complete = ETrue;
		completeErr = err;
		}

	if ( complete )
		{
		iParent.NotifyDataAvailableCompleted(completeErr);
		}
	OstTraceFunctionExit0( CACTIVEDATAAVAILABLENOTIFIER_RUNL_EXIT );
	}

//
// End of file
