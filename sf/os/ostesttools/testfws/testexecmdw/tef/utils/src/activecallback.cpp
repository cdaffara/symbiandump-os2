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
* @file
* This contains CActiveCallback
*
*/



/**
 @prototype
 @test
*/

#include "activecallback.h"

EXPORT_C CActiveCallback::~CActiveCallback()
/**
 * Destructor
 */
	{
	Cancel();
	}

EXPORT_C CActiveCallback* CActiveCallback::NewL(MActiveCallback& aCallback, TInt aPriority)
/**
 * Two phase constructor that allocates and constructs
 * a new Active object whos actions are performed by a callback
 *
 * @param	aCallback - object to inform on RunL.
 * @param	aPriority - priority of active object.
 *
 * @see		MActiveCallback
 *
 * @leave	system wide error
 *
 * @return	New Callback active object.
 */
	{
	CActiveCallback*	self=NewLC(aCallback, aPriority);
	CleanupStack::Pop();
	return self;
	}

EXPORT_C CActiveCallback* CActiveCallback::NewLC(MActiveCallback& aCallback, TInt aPriority)
/**
 * Two phase constructor that allocates and constructs
 * a new Active object whos actions are performed by a callback
 *
 * @param	aCallback - object to inform on RunL.
 * @param	aPriority - priority of active object.
 *
 * @see		MActiveCallback
 *
 * @leave	system wide error
 *
 * @return	New Callback active object.
 */
	{
	CActiveCallback*	self=new(ELeave) CActiveCallback(aCallback, aPriority);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

EXPORT_C void CActiveCallback::Activate( TInt aIndex )
/**
 * Activate the object
 *
 * @param	aIndex - command index
 */
	{
	iIndex = aIndex;
	SetActive();
	}

EXPORT_C void CActiveCallback::KickState()
/**
 * Kick Start the object
 */
	{
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrNone);
	SetActive();
	}

EXPORT_C CActiveCallback::CActiveCallback(MActiveCallback& aCallback, TInt aPriority)
/**
 * Protected constructor with timer completion callback and priority.
 *
 * Called by two phase constructor.
 *
 * @param	aTestTimerCallback - object to inform on timer completion.
 * @param	aPriority - priority of active object.
 *
 * @see		MActiveCallback
 */
:	CActive(aPriority)
,	iCallback(aCallback)
,	iIndex(0)
	{
	}

EXPORT_C void CActiveCallback::ConstructL()
/**
 * This is internal and not intended for use.
 *
 * Second phase of two phase constructor.
 *
 * @leave	system wide error
 */
	{
	CActiveScheduler::Add(this);
	}

EXPORT_C void CActiveCallback::RunL()
/**
 * Active object RunL implementation.
 *
 * Calls the MActiveCallback::RunL to inform user that the RunL has been reached.
 *
 * @see		MActiveCallback::RunL
 * @leave	system wide error
 */
	{
	iCallback.RunL(this,iIndex);
	}

EXPORT_C void CActiveCallback::DoCancel()
/**
 * Active object DoCancel implementation.
 *
 * Calls the MActiveCallback::DoCancel to inform user that the DoCancel has been reached.
 *
 * @see		MActiveCallback::DoCancel
 */
	{
	iCallback.DoCancel(this,iIndex);
	}
