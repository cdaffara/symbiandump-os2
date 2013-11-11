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
* Implements an interface to perform asynchronous database operations.
*
*/


/**
 @file
 @internalTechnology
 @released
*/
 
#include "upsdbw.h"

using namespace UserPromptService;


CDecisionDbCompactor::CDecisionDbCompactor():CActive(EPriorityStandard)
/**
	Constructor. 
 */
	{
	CActiveScheduler::Add(this);
	}
	
CDecisionDbCompactor::~CDecisionDbCompactor()
/**
	Destructor
 */
 	{
 	iDbIncremental.Close();
 	Deque();
 	}
 	
 CDecisionDbCompactor* CDecisionDbCompactor::NewLC()
 /**
	
	
	@return A pointer to the newly allocated compactor object, if creation is successful.
	        The pointer is also put onto the cleanup stack.
 */
 	{
 	CDecisionDbCompactor *self = new(ELeave) CDecisionDbCompactor();
 	CleanupStack::PushL(self);
 	return self;
 	}
 	
 
 EXPORT_C void CDecisionDbCompactor::Compact(TRequestStatus& aStatus)
 /**
 	Performs the asynchronous compaction of the decision database, returning immediately 
 	and signalling the request status when the operation is fully complete.
 	
 	@param aStatus The request status used to contain completion information for the function.
	               On completion, the status value should be interpreted as follows: 
	               0, compaction is complete.< 0, an error code.
 	
  */
 	{
	__ASSERT_ALWAYS(!IsActive(), User::Panic(KDecisionViewPanic,KErrInUse));	
	
	aStatus = KRequestPending;
	iClientStatus = &aStatus;
		
	if(iStep() > 0)
		{	
	    iDbIncremental.Next(iStep, iStatus);
		SetActive();
		}
	else
		{
		iDbIncremental.Close();
		User::RequestComplete(iClientStatus,KErrNone);
		}
	
	}
	

void CDecisionDbCompactor::DoCancel()
//From CActive
	{
	iDbIncremental.Close();
	if (iClientStatus)
		{
		User::RequestComplete(iClientStatus, KErrCancel);
		}
	}


TInt CDecisionDbCompactor::RunError(TInt aError)
//From CActive
	{
	if (iClientStatus)
		{
		User::RequestComplete(iClientStatus, aError);
		}
	return KErrNone;
	}


void CDecisionDbCompactor::RunL() 
//From CActive
	{
	TInt status = iStatus.Int();
	User::LeaveIfError(status);
	
	if(iStep() > 0)
		{
		iDbIncremental.Next(iStep, iStatus);
		SetActive();
		}
	else
		{
		iDbIncremental.Close();
		User::RequestComplete(iClientStatus,status);
		}
	}
