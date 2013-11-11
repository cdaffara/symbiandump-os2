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
*
*/


/**
 @file  
 @test 
*/

#include "tupstesteractive.h" 
#include <ups/upsclient.h>
#include "tups_sys_server.h"
#include "hal.h"


// Asyncronous UPS Request tester active.
CTUPSTesterActive::CTUPSTesterActive(const RMessage2& aMessage) : CActive(EPriorityStandard),
   iMessage(aMessage),iStartTime(0), iEndTime(0), iState(EIdle)
	{
	CActiveScheduler::Add( this );
	} 


CTUPSTesterActive::~CTUPSTesterActive()
	{
  	iServerSessionPointer->CTUPSTesterActiveComplete(this);
	}


TInt CTUPSTesterActive::doActiveUpsRequest(CUPSTestSession* aTestServerSession)
	{
	iServerSessionPointer = aTestServerSession;
	TBool cancelRequest = iServerSessionPointer->iRequestData.iCancelRequest;

    iSerDest.Set(iServerSessionPointer->iRequestData.iDestination);// service destination	
   	TServiceId serviceId = iServerSessionPointer->iRequestData.iServiceUid;
    iDecision = EUpsDecNo; 
	TBool platSecPass=iServerSessionPointer->iRequestData.iPlatSecPass;

   	if (iServerSessionPointer->iRequestData.iOpaquePresent)
	   	{	
	   	//request autorization- with opaque data
	   	iOpaqueData.Set(iServerSessionPointer->iRequestData.iOpaqueData ); 
	   	}

	iStartTime = User::NTickCount();
	
   	if (iServerSessionPointer->iRequestData.iOpaquePresent)
	   	{	
	   	//request autorization- with opaque data
	   	iServerSessionPointer->iUpsSubsession.Authorise(platSecPass,serviceId,iSerDest,iOpaqueData,iDecision,iStatus);	
	   	}
   	else
	   	{	
	   	//request autorization- no opaque data
	   	iServerSessionPointer->iUpsSubsession.Authorise(platSecPass,serviceId,iSerDest,iDecision, iStatus);	
	   	}

    iState = ERequestAuthorization;    	
   	SetActive();
   	
   	// checks if request need to be cancel.
   	if (cancelRequest)
	   	{
	   	iServerSessionPointer->iUpsSubsession.CancelPrompt();
	   	}
   	
  	return KErrNone;  
	}
	
	
TInt CTUPSTesterActive::RunError(TInt aError)
	{		 	
	TRAP_IGNORE(PackageReturnDataL(aError));
	
	delete this;
	
  	return KErrNone;
	}
	
	
void CTUPSTesterActive::RunL()
	{
	User::LeaveIfError(iStatus.Int());

	switch(iState)
		{
		
		case ERequestAuthorization:
			{
			iState = EIdle;
   			
   			PackageReturnDataL(KErrNone);		 	
			
			delete this;
			return;	
			}
					
		default:
			{
		  	User::Leave(KErrCorrupt);
			}
 		} 
	}


void CTUPSTesterActive::PackageReturnDataL(TInt aError)
/*
 * load return data to second argument 
 */	
 	{
 	iEndTime = User::NTickCount();

	TInt nanokernel_tick_period;
	HAL::Get(HAL::ENanoTickPeriod, nanokernel_tick_period);	// tick duration in microseconds

	iReturnData.iRequestDuration = ((iEndTime - iStartTime) * (nanokernel_tick_period / 1000));
	iReturnData.iDecision = iDecision;
	iReturnData.iError = aError;
 			 	
	TPckgC<TUpsTestServiceReturn> compoundReturn(iReturnData);
	iMessage.WriteL(1, compoundReturn);
	iMessage.Complete(KErrNone);	
	}
