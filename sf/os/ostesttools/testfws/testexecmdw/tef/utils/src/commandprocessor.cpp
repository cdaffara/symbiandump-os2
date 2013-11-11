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
* This contains CCommandProcessor
*
*/



/**
 @prototype
 @test
*/

#include "commandprocessor.h"

EXPORT_C CCommandProcessor::~CCommandProcessor()
/**
 * Destructor
 */
	{
	Cancel();
	}

EXPORT_C CCommandProcessor* CCommandProcessor::NewL(MCommandProcessorCallback& aCallback, TInt aPriority)
/**
 * Two phase constructor that allocates and constructs
 * a new Active object whos actions are performed by a callback
 *
 * @param	aCallback - object to inform on RunL.
 * @param	aPriority - priority of active object.
 *
 * @leave	system wide error
 *
 * @see		MCommandProcessorCallback
 *
 * @return	New Callback active object.
 */
	{
	CCommandProcessor*	self=NewLC(aCallback, aPriority);
	CleanupStack::Pop();
	return self;
	}

EXPORT_C CCommandProcessor* CCommandProcessor::NewLC(MCommandProcessorCallback& aCallback, TInt aPriority)
/**
 * Two phase constructor that allocates and constructs
 * a new Active object whos actions are performed by a callback
 *
 * @param	aCallback - object to inform on RunL.
 * @param	aPriority - priority of active object.
 *
 * @leave	system wide error
 *
 * @see		MCommandProcessorCallback
 *
 * @return	New Callback active object.
 */
	{
	CCommandProcessor*	self=new(ELeave) CCommandProcessor(aCallback, aPriority);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

EXPORT_C void CCommandProcessor::Activate()
/**
 * Activate the object
 */
	{
	SetActive();
	}

EXPORT_C void CCommandProcessor::KickState()
/**
 * Kick Start the object
 */
	{
	TRequestStatus*	status = &iStatus;
	User::RequestComplete(status, KErrNone);
	SetActive();
	}

CCommandProcessor::CCommandProcessor(MCommandProcessorCallback& aCallback, TInt aPriority)
/**
 * Protected constructor with timer completion callback and priority.
 *
 * Called by two phase constructor.
 *
 * @param	aTestTimerCallback - object to inform on timer completion.
 * @param	aPriority - priority of active object.
 *
 * @see		MCommandProcessorCallback
 */
:	CActive(aPriority)
,	iCallback(aCallback)
	{
	}

void CCommandProcessor::ConstructL()
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

void CCommandProcessor::RunL()
/**
 * Active object RunL implementation.
 *
 * Calls the MCommandProcessorCallback::NextCommandL to inform user that the RunL has been reached.
 *
 * @leave	system wide error
 */
	{
	iCallback.NextCommandL();
	}

void CCommandProcessor::DoCancel()
/**
 * Active object DoCancel implementation.
 *
 * Calls the MCommandProcessorCallback::DoCancel to inform user that the DoCancel has been reached.
 */
	{
	}
