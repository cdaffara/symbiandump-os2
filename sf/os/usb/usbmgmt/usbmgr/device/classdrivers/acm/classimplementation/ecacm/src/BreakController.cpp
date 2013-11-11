/*
* Copyright (c) 1997-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "BreakController.h"
#include "CdcAcmClass.h"
#include "AcmUtils.h"
#include "HostPushedChangeObserver.h"
#include "AcmPanic.h"
#include "BreakObserver.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "BreakControllerTraces.h"
#endif


CBreakController::CBreakController(CCdcAcmClass& aParentAcm)
/**
 * Constructor.
 * 
 * @param aParentAcm Observer.
 */
 :	CActive(CActive::EPriorityStandard),
	iBreakState(EInactive),
	iParentAcm(aParentAcm)
	{
	OstTraceFunctionEntry0( CBREAKCONTROLLER_CBREAKCONTROLLER_CONS_ENTRY );
	CActiveScheduler::Add(this);

	// now populate the state machine that manages the transfers between
	// the declared state values of TBreakState.
	TInt oldBS;
	TInt newBS;

	for ( oldBS = 0 ; oldBS < ENumStates ; oldBS++ )
		{
		for ( newBS = 0 ; newBS < ENumStates ; newBS++ )
			{
			StateDispatcher[oldBS][newBS] = ScInvalid;
			}
		}

	// Note that these state transitions are the simple states of the machine. 
	// Checking which entity is currently in control of the break (if any) is 
	// done elsewhere.
	//				Old State -> New State
	//				|			 |
	StateDispatcher[EInactive  ][ETiming   ] = &ScSetTimer;
	StateDispatcher[EInactive  ][ELocked   ] = &ScLocked;

	StateDispatcher[ETiming    ][EInactive ] = &ScInactive;
	StateDispatcher[ETiming    ][ETiming   ] = &ScSetTimer;
	StateDispatcher[ETiming    ][ELocked   ] = &ScLocked;

	StateDispatcher[ELocked    ][EInactive ] = &ScInactive;
	StateDispatcher[ELocked    ][ETiming   ] = &ScSetTimer; 
	OstTraceFunctionExit0( CBREAKCONTROLLER_CBREAKCONTROLLER_CONS_EXIT );
	}

CBreakController* CBreakController::NewL(CCdcAcmClass& aParentAcm)
/**
 * Factory function.
 *
 * @param aParentAcm Parent.
 * @return Ownership of a new CBreakController object.
 */ 
	{
	OstTraceFunctionEntry0( CBREAKCONTROLLER_NEWL_ENTRY );
	CBreakController* self = new(ELeave) CBreakController(aParentAcm);
	CleanupStack::PushL(self);
	self->ConstructL();
	CLEANUPSTACK_POP(self);
	OstTraceFunctionExit0( CBREAKCONTROLLER_NEWL_EXIT );
	return self;
	}

void CBreakController::ConstructL()
/**
 * 2nd-phase constructor.
 */
	{
	TInt	err;
	err = iTimer.CreateLocal();
	if (err < 0)
		{
		OstTrace1( TRACE_NORMAL, CBREAKCONTROLLER_CONSTRUCTL, "CBreakController::ConstructL;err=%d", err );
		User::Leave(err);
		}
	}

CBreakController::~CBreakController()
/**
 * Destructor.
 */
	{
	OstTraceFunctionEntry0( CBREAKCONTROLLER_CBREAKCONTROLLER_DES_ENTRY );
	Cancel();
	iTimer.Close();
	OstTraceFunctionExit0( CBREAKCONTROLLER_CBREAKCONTROLLER_DES_EXIT );
	}

void CBreakController::RunL()
/**
 * Called by the active scheduler; handles timer completion.
 */
	{
	OstTraceFunctionEntry0( CBREAKCONTROLLER_RUNL_ENTRY );
	
	// check the status to see if the timer has matured, if so go straight 
	// to INACTIVE state (and publish new state)
	if ( iStatus == KErrNone )
		{
		// Use iRequester to turn the break off. This should not fail.
		TInt err = BreakRequest(iRequester, EInactive);
		static_cast<void>(err);
		if (err)
			{		
			OstTrace1( TRACE_FATAL, CBREAKCONTROLLER_RUNL, "CBreakController::RunL;err=%d", err );
			__ASSERT_DEBUG( EFalse, User::Panic(KAcmPanicCat, EPanicInternalError) );
			}
		}
	OstTraceFunctionExit0( CBREAKCONTROLLER_RUNL_EXIT );
	}

void CBreakController::DoCancel()
/**
 * Called by the framework; handles cancelling the outstanding timer request.
 */
	{
	OstTraceFunctionEntry0( CBREAKCONTROLLER_DOCANCEL_ENTRY );
	iTimer.Cancel();
	OstTraceFunctionExit0( CBREAKCONTROLLER_DOCANCEL_EXIT );
	}

TInt CBreakController::BreakRequest(TRequester aRequester, 
									TState aState, 
									TTimeIntervalMicroSeconds32 aDelay)
/**
 * Make a break-related request.
 *
 * @param aRequester The entity requesting the break.
 * @param aState The request- either a locked break, a timed break, or to 
 * make the break inactive.
 * @param aDelay The time delay, only used for a timed break.
 * @return Error, for instance if a different entity already owns the break.
 */
 	{
	OstTraceFunctionEntry0( CBREAKCONTROLLER_BREAKREQUEST_ENTRY );
	OstTraceExt3( TRACE_NORMAL, CBREAKCONTROLLER_BREAKREQUEST, 
			"CBreakController::BreakRequest;aRequester=%d;aState=%d;aDelay=%d", (TInt)aRequester, (TInt)aState, aDelay.Int() );

	// Check the validity of the request.
	if ( aRequester != iRequester && iRequester != ENone )
		{
		OstTraceExt2( TRACE_FLOW, CBREAKCONTROLLER_BREAKREQUEST_DUP1, 
				"CBreakController::BreakRequest;%d is in charge- cannot service request from %d- returning KErrInUse", 
				(TInt)iRequester, (TInt)aRequester );
		
		OstTraceFunctionExit0( CBREAKCONTROLLER_BREAKREQUEST_EXIT );
		return KErrInUse;
		}

	iRequester = aRequester;
	StateMachine(aState, aDelay);
	// Reset the owner member if relevant.
	if ( aState == EInactive )
		{
		iRequester = ENone;
		}

	OstTraceFunctionExit0( CBREAKCONTROLLER_BREAKREQUEST_EXIT_DUP1 );
	return KErrNone;
	}

void CBreakController::StateMachine(TState aBreakState, 
									TTimeIntervalMicroSeconds32 aDelay)
/**
 * The generic BREAK state machine.
 *
 * @param aBreakState The state to go to now.
 * @param aDelay Only used if going to a breaking state, the delay.
 */
	{
	OstTraceFunctionEntry0( CBREAKCONTROLLER_STATEMACHINE_ENTRY );
	TBool resultOK = EFalse;

	// Invoke the desired function.
	PBFNT pfsDispatch = StateDispatcher[iBreakState][aBreakState];
	if (!pfsDispatch)	
		{
		OstTraceExt1( TRACE_FATAL, CBREAKCONTROLLER_STATEMACHINE, "CBreakController::StateMachine;pfsDispatch=%p", (TAny*)pfsDispatch );
		__ASSERT_DEBUG( EFalse, User::Panic(KAcmPanicCat, EPanicInternalError) );
		}
	resultOK = ( *pfsDispatch )(this, aDelay);

	if ( resultOK )
		{
		OstTrace0( TRACE_NORMAL, CBREAKCONTROLLER_STATEMACHINE_DUP1, 
				"CBreakController::StateMachine;\tbreak state dispatcher returned *SUCCESS*" );
		
		// check to see if the state change will need to result
		// in a modification to the public state of BREAK which is
		// either NO-BREAK == EBreakInactive
		//	   or BREAK-ON == (anything else)
		if(    ( iBreakState != aBreakState )
			&& (
				   ( iBreakState == EInactive )
				|| ( aBreakState == EInactive )
			   )
		  )
			{
			Publish(aBreakState);
			}

		// accept the state change ready for next time
		iBreakState = aBreakState;
		}
	else
		{
		OstTrace0( TRACE_NORMAL, CBREAKCONTROLLER_STATEMACHINE_DUP2, 
				"CBreakController::StateMachine;\tbreak state dispatcher returned *FAILURE*" );
		}
	OstTraceFunctionExit0( CBREAKCONTROLLER_STATEMACHINE_EXIT );
	}

void CBreakController::Publish(TState aNewState)
/**
 * Pointer-safe method to inform the (USB) Host and the Client of BREAK 
 * changes.
 *
 * @param aNewState The next state we're about to go to.
 */
	{
	OstTraceFunctionEntry0( CBREAKCONTROLLER_PUBLISH_ENTRY );

	if (aNewState == iBreakState)
		{
		OstTrace1( TRACE_FATAL, CBREAKCONTROLLER_PUBLISH, "CBreakController::Publish;aNewState=%d", (TInt)aNewState );
		__ASSERT_DEBUG( EFalse, User::Panic(KAcmPanicCat, EPanicInternalError) );
		}

	// send the new BREAK state off to the USB Host
	// this function is normally used so that ACMCSY can send client 
	// changes to RING, DSR and DCD to the USB Host, however we use
	// it here to force it to refresh all states together with the
	// new BREAK state.
	// TODO: check return value
	iParentAcm.SendSerialState(
		iParentAcm.RingState(),
		iParentAcm.DsrState(),
		iParentAcm.DcdState());

	// inform the ACM Class client that the BREAK signal has just changed,
	// this should cause it to be toggled there.
	if( iParentAcm.BreakCallback() )
		{
		OstTrace0( TRACE_NORMAL, CBREAKCONTROLLER_PUBLISH_DUP1, "CBreakController::Publish;\tabout to call back break state change" );
		iParentAcm.BreakCallback()->BreakStateChange();
		}

	// If we're going to the inactive state, and if the device is interested, 
	// we tell the MBreakObserver (ACM port) that the break has completed. 
	if ( aNewState == EInactive )
		{
		OstTrace0( TRACE_NORMAL, CBREAKCONTROLLER_PUBLISH_DUP2, "CBreakController::Publish;\tnew state is break-inactive" );
		if ( iRequester == EDevice )
			{
			OstTrace0( TRACE_NORMAL, CBREAKCONTROLLER_PUBLISH_DUP3, "CBreakController::Publish;\tdevice is interested" );
			if( iParentAcm.BreakCallback() )
				{
				OstTrace0( TRACE_NORMAL, CBREAKCONTROLLER_PUBLISH_DUP4, "CBreakController::Publish;\tabout to call back break completion" );
				iParentAcm.BreakCallback()->BreakRequestCompleted(); 
				}
			}

		// We just got to break-inactive state. Blank the requester record.
		iRequester = ENone;
		}
	OstTraceFunctionExit0( CBREAKCONTROLLER_PUBLISH_EXIT );
	}

/**
 * +----------------------------------------------+
 * | Set of state-machine functions to be used in |
 * | a two-dimensional dispatcher matrix		  |
 * +----------------------------------------------+
 */

TBool CBreakController::ScInvalid(CBreakController *aThis, 
								  TTimeIntervalMicroSeconds32 aDelay)
	{
	OstTraceFunctionEntry0( CBREAKCONTROLLER_SCINVALID_ENTRY );
	static_cast<void>(aThis); // remove warning 
	static_cast<void>(aDelay); // remove warning
	OstTraceFunctionExit0( CBREAKCONTROLLER_SCINVALID_EXIT );
	return( EFalse );
	}

TBool CBreakController::ScInactive(CBreakController *aThis, 
								   TTimeIntervalMicroSeconds32 aDelay)
	{
	OstTraceFunctionEntry0( CBREAKCONTROLLER_SCINACTIVE_ENTRY );
	
	static_cast<void>(aDelay); // remove warning

	// this may have been called while a BREAK is already current, cancel the 
	// timer.
	aThis->Cancel();

	aThis->iParentAcm.SetBreakActive(EFalse);

	OstTraceFunctionExit0( CBREAKCONTROLLER_SCINACTIVE_EXIT );
	return( ETrue );
	}

TBool CBreakController::ScSetTimer(CBreakController *aThis, 
								   TTimeIntervalMicroSeconds32 aDelay)
	{
	OstTraceFunctionEntry0( CBREAKCONTROLLER_SCSETTIMER_ENTRY );

	// don't try to set any delay if the caller wants something impossible
	if ( aDelay.Int() <= 0 )
		{
		OstTraceFunctionExit0( CBREAKCONTROLLER_SCSETTIMER_EXIT );
		return( EFalse );
		}

	aThis->Cancel(); // in case we're already active.

	aThis->iTimer.After(aThis->iStatus, aDelay);
	aThis->SetActive();

	aThis->iParentAcm.SetBreakActive(ETrue);

	OstTraceFunctionExit0( CBREAKCONTROLLER_SCSETTIMER_EXIT_DUP1 );
	return( ETrue );
	}

TBool CBreakController::ScLocked(CBreakController *aThis, 
								 TTimeIntervalMicroSeconds32 aDelay)
	{
	OstTraceFunctionEntry0( CBREAKCONTROLLER_SCLOCKED_ENTRY );
	static_cast<void>(aDelay); // remove warning

	// this may have been called while a BREAK is already current, so cancel 
	// the timer.
	aThis->Cancel();

	aThis->iParentAcm.SetBreakActive(ETrue);

	OstTraceFunctionExit0( CBREAKCONTROLLER_SCLOCKED_EXIT );
	return( ETrue );
	}

//
// End of file
