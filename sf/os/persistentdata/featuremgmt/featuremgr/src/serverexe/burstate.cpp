// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

/**
 @file
*/

#include "burstate.h"
#include <babackup.h>
#include "featmgrdebug.h"
#include "featmgrserver.h"

CBurState::CBurState()
	{
	return;
	}

/**
 * The Backup and Restore state machine.
 * This state machine has two state tables. The first table defines the correct modes of operation
 * and the second describe the pathways arrising from error conditions. By seperating the normal
 * pathways from the error pathways we can simplify the state machine into two parts. The function 
 * SetUpBurStruct() will define the pathways for both these tables.
 * Each table has three items per row: a current state, an ending state, and a pointer to a function 
 * that transforms between these two states safely. 
 * Given that we already know where we are, and where we are told to goto, the state machine
 * function BUR_StateMachine() will search the normal operation table for a match, and then apply
 * the function through the pointer. If no match is found, then the goto state is set to Error, and
 * the error table is searched and applied. Because all the state are covered within the error table
 * we can always return to a normal more of operation through the application of the error
 * functions.
 */ 
CBurState::CBurState(CFeatMgrServer* aServer) : iCurrentBURStatus( EFeatMgrBURState_None ), iServer( aServer )
	{
	SetUpBurStruct();
	return;
	}

CBurState::~CBurState()
	{
	return;
	}

CBurState* CBurState::NewLC()
	{
	CBurState* self = new (ELeave)CBurState();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CBurState* CBurState::NewL()
	{
	CBurState* self=CBurState::NewLC();
	CleanupStack::Pop(); // self;
	return self;
	}

void CBurState::ConstructL()
	{
	return;
	}

/**
 * This function checks the arguments that are passed into HandleBackupOperationEventL. 
 * The TBackupOperationAttributes are checked against known values and an error is returned if
 * the Feature Manager doesn't recognise the events.
 */ 
TInt CBurState::CheckBURArguments(  const TBackupOperationAttributes& aBackupOperationAttributes )
	{
	TInt error( KErrNone );
    iBURLockFlag = aBackupOperationAttributes.iFileFlag;
    iBUROpType   = aBackupOperationAttributes.iOperation;
    
	// determine the operation type (backup or restore)
	switch( iBURLockFlag )	
		{
		case MBackupObserver::EReleaseLockReadOnly:
			INFO_LOG( "MBackupObserver::EReleaseLockReadOnly" );
			break;
		case MBackupObserver::EReleaseLockNoAccess:
			INFO_LOG( "MBackupObserver::EReleaseLockNoAccess" );
			break;
		case MBackupObserver::ETakeLock:
			INFO_LOG( "MBackupObserver::ETakeLock" );
			break;
		default:
			INFO_LOG( "CheckBURArguments iBURLockFlag default" );
			error = KErrNotFound;
			break;
		}
		
	// determine the operation status (e.g. starting, ending)
	switch( iBUROpType )
		{
		case MBackupOperationObserver::ENone:
			INFO_LOG( "ENone" );
			break;
		case MBackupOperationObserver::EStart:
			INFO_LOG( "EStart" );
			break;
		case MBackupOperationObserver::EEnd:
			INFO_LOG( "EEnd" );
			break;
		case MBackupOperationObserver::EAbort:
			INFO_LOG( "EAbort" );	
			break;
		default:
			INFO_LOG( "CheckBURArguments iOperation default" );	
			error = KErrNotFound;
			break;
		}

	return error;
	}

/**
 * Sets up the array entries for the possible backup/restore state vectors. 
 *               from           -> to             : function
 * State vector: None           -> BackupStarted  : Goto_StartBackupState
 * State vector: BackupStarted  -> BackupEnded    : EndBackupState
 * State vector: BackupEnded    -> None           : Goto_NormalState
 * State vector: None           -> RestoreStarted : Goto_StartRestoreState
 * State vector: RestoreStarted -> RestoreEnded   : Goto_EndRestoreState
 * State vector: RestoreEnded   -> None           : Goto_NormalState
 */
void CBurState::SetUpBurStruct()
	{
	BURStruct bStruct;

	// Backup states

	// State vector: None -> BackupStarted : Goto_StartBackupState
	bStruct.iCurrent = EFeatMgrBURState_None;
	bStruct.iGoto = EFeatMgrBURState_BackupStarted;
	bStruct.iFunc = &CFeatMgrServer::Goto_StartBackupState;
	iBURStructArray[0] = bStruct;

	// State vector: BackupStarted -> BackupEnded : EndBackupState
	bStruct.iCurrent = EFeatMgrBURState_BackupStarted;
	bStruct.iGoto = EFeatMgrBURState_BackupEnded;
	bStruct.iFunc = &CFeatMgrServer::Goto_EndBackupState;
	iBURStructArray[1] = bStruct;

	// State vector: BackupEnded -> None : Goto_NormalState
	bStruct.iCurrent = EFeatMgrBURState_BackupEnded;
	bStruct.iGoto = EFeatMgrBURState_None;
	bStruct.iFunc = &CFeatMgrServer::Goto_NormalState;
	iBURStructArray[2] = bStruct;

	// Valid restore states
	
	// State vector: None -> RestoreStarted : Goto_StartRestoreState
	bStruct.iCurrent = EFeatMgrBURState_None;
	bStruct.iGoto = EFeatMgrBURState_RestoreStarted;
	bStruct.iFunc = &CFeatMgrServer::Goto_StartRestoreState;
	iBURStructArray[3] = bStruct;

	// State vector: RestoreStarted -> RestoreEnded : Goto_EndRestoreState
	bStruct.iCurrent = EFeatMgrBURState_RestoreStarted;
	bStruct.iGoto = EFeatMgrBURState_RestoreEnded;
	bStruct.iFunc = &CFeatMgrServer::Goto_EndRestoreState;
	iBURStructArray[4] = bStruct;

	// State vector: RestoreEnded -> None : Goto_NormalState
	bStruct.iCurrent = EFeatMgrBURState_RestoreEnded;
	bStruct.iGoto = EFeatMgrBURState_None;
	bStruct.iFunc = &CFeatMgrServer::Goto_NormalState;
	iBURStructArray[6] = bStruct;

	// State vector: None -> None : Goto_NormalState
	bStruct.iCurrent = EFeatMgrBURState_None;
	bStruct.iGoto = EFeatMgrBURState_None;
	bStruct.iFunc = &CFeatMgrServer::Goto_NormalState;
	iBURStructArray[5] = bStruct;

	
	//
	// Error states

	// State vector: RestoreStarted -> Error : Goto_ErrorState
	bStruct.iCurrent = EFeatMgrBURState_RestoreStarted;
	bStruct.iGoto = EFeatMgrBURState_Error;
	bStruct.iFunc = &CFeatMgrServer::Goto_ErrorState;
	iBURStructErrorArray[0] = bStruct;

	// State vector: RestoreEnded -> Error : Goto_ErrorState
	bStruct.iCurrent = EFeatMgrBURState_RestoreEnded;
	bStruct.iGoto = EFeatMgrBURState_Error;
	bStruct.iFunc = &CFeatMgrServer::Goto_ErrorState;
	iBURStructErrorArray[1] = bStruct;

	// State vector: BackupStarted -> None : Goto_ErrorState
	bStruct.iCurrent = EFeatMgrBURState_BackupStarted;
	bStruct.iGoto = EFeatMgrBURState_Error;
	bStruct.iFunc = &CFeatMgrServer::Goto_ErrorState;
	iBURStructErrorArray[2] = bStruct;
	
	// State vector: BackupEnded -> None : Goto_ErrorState
	bStruct.iCurrent = EFeatMgrBURState_BackupEnded;
	bStruct.iGoto = EFeatMgrBURState_Error;
	bStruct.iFunc = &CFeatMgrServer::Goto_ErrorState;
	iBURStructErrorArray[3] = bStruct;
	
	// State vector: Error -> None : Goto_NormalState
	bStruct.iCurrent = EFeatMgrBURState_Error;
	bStruct.iGoto = EFeatMgrBURState_None;
	bStruct.iFunc = &CFeatMgrServer::Goto_NormalState;
	iBURStructErrorArray[4] = bStruct;
	
	return;
	}

/**
 * Convert from the type provided in TBackupOperationAttributes into more sensible BUR types that
 * can be handles by the BUR_StateMachine. This step is necessary for two reasons, the first is
 * simplification and the second is a lexical check for invalid type MBackupObserver::TFileLockFlags.
 */
BURStatus CBurState::ConvertToBURState(  const TBackupOperationAttributes& aBackupOperationAttributes )
	{
	BURStatus status = EFeatMgrBURState_Error;
    iBURLockFlag = aBackupOperationAttributes.iFileFlag;
    iBUROpType   = aBackupOperationAttributes.iOperation;
	// we ignore the iOperation state
    
	if( iBURLockFlag == MBackupObserver::ETakeLock )
		{
		// ending (this doesn't define whether it was a backup
		// or a restore ending, just that file writing can now start 
		// again).
		INFO_LOG("ChangeFileLockL() BUR ending");
		switch( iCurrentBURStatus )
			{
			case( EFeatMgrBURState_BackupStarted ):
				status = EFeatMgrBURState_BackupEnded;
				break;
			case( EFeatMgrBURState_RestoreStarted ):
				status = EFeatMgrBURState_RestoreEnded;
				break;
			case( EFeatMgrBURState_None ):
				status = EFeatMgrBURState_None;
				break;
			default:
				status = EFeatMgrBURState_Error;
				break;
			}
		}
	else if( (iBURLockFlag & MBackupObserver::EReleaseLockReadOnly) &&
		(iBURLockFlag & ~MBackupObserver::EReleaseLockNoAccess) )
		{
		// starting (making the file read-only is a sign that we are
		// in "backup" mode).
		INFO_LOG("ChangeFileLockL() backup starting");
		status = EFeatMgrBURState_BackupStarted;
		}
	else if( iBURLockFlag & MBackupObserver::EReleaseLockNoAccess )
		{
		// starting (now read/write access is used to signify that a
		// restore has been started).
		INFO_LOG("ChangeFileLockL() restore starting");
		status = EFeatMgrBURState_RestoreStarted;
		}
	else
		{
		// unhandled flag states
		INFO_LOG("ChangeFileLockL() error state");
		status = EFeatMgrBURState_Error;
		}
	
	return status;
	}

/**
 * This function is called from within featmgr server when a backup event occurs
 * There are two state variables coming into Feature Manager as types MBackupObserver::TFileLockFlags 
 * and TOperationType. Only certain combinations of these two state variables are valid for Feature
 * Manager and if the combination is not valid (because there is no way of returning an error) 
 * it will just set the internal state machine into an "error" state.
 * Given our current state and our goto state (i.e. where we are at the moment and where we want to goto)
 * the state machine checks that this is a valid path in our state machine and then perform the correct 
 * operations to get us to the next valid state.
 */
void CBurState::BUROperationL( const TBackupOperationAttributes& aBackupOperationAttributes )
	{
	BURStatus gotoState    = EFeatMgrBURState_Error;  // fail safe
	BURStatus currentState = iCurrentBURStatus;       // where in the state machine we are at the moment
	
	// Check that the arguments are within valid limits
	if( KErrNone == CheckBURArguments( aBackupOperationAttributes ) )
		{
		// Now convert into states that the Feature Manager understands.
		gotoState = ConvertToBURState( aBackupOperationAttributes );
		}
	
	// Check that this is a valid path and then perform the correct operations to get to the next valid state
	iCurrentBURStatus = BUR_StateMachine( currentState, gotoState );
	
	return;
	}

/**
 * The state machine for the backup and restore of the featmgr 
 * The logic that goes into changing states in the state machine.
 */
BURStatus CBurState::BUR_StateMachine( BURStatus aCurrent, BURStatus aGoto )
	{
	TInt count = 0;
	TBool found = EFalse;

	// Fail safe default state values
	BURStatus newState			= EFeatMgrBURState_Error;
	BURStatus stateTableGoto    = EFeatMgrBURState_Error;
	BURStatus stateTableCurrent = EFeatMgrBURState_Error;
	
	// Normal operation
	//
	do
		{
		// Get the n-th state table [current,goto] state vector
		stateTableCurrent = iBURStructArray[count].iCurrent;
		stateTableGoto    = iBURStructArray[count].iGoto;
		
		// if the state table matches what we are given
		if( (aCurrent == stateTableCurrent) && 
			(aGoto    == stateTableGoto) )
			{
			// process the relevant state function
			if( NULL != iBURStructArray[count].iFunc )
				{
				newState = (iServer->*iBURStructArray[count].iFunc)( aCurrent );

				// check result: when from the state machine is not what we expected from the state table
				if( newState != stateTableGoto )
					{
					// put state machine into an error state and break "normal" loop
					aCurrent = newState;
					aGoto    = EFeatMgrBURState_Error;
					break;
					}
				else
					{
					found = ETrue;
					}
				}
			
			// self-perpetuate in certain cases
			if( (EFeatMgrBURState_BackupEnded == newState) || 
				(EFeatMgrBURState_RestoreEnded == newState) )
				{
				aCurrent = newState;
				aGoto = EFeatMgrBURState_None;
				found = EFalse;
				count = 0;
				}
			}
		count++;
		}
	while( (!found) && count < KBURArrayLength );
	

	// Error recovery
	//
	if( EFeatMgrBURState_Error == aGoto ||
		EFalse == found )
		{
		// reset 
		aGoto = EFeatMgrBURState_Error;
		count = 0;
		found = EFalse;

		do
			{
			// Get the n-th state table [current,goto] state vector in the error array
			stateTableCurrent = iBURStructErrorArray[count].iCurrent;
			stateTableGoto    = iBURStructErrorArray[count].iGoto;
			
			// if the state table matches what we are given
			if( ((aCurrent == stateTableCurrent) && (aGoto == stateTableGoto))  )
				{
				// process the relevant state function
				if( NULL != iBURStructErrorArray[count].iFunc )
					{
					newState = (iServer->*iBURStructErrorArray[count].iFunc)( aCurrent );
					// there is no error recovery from error recovery.
					found = ETrue;
					}
				
				// self-perpetuate in certain cases
				if( EFeatMgrBURState_Error == newState )
					{
					aCurrent = newState;
					aGoto = EFeatMgrBURState_None;
					found = EFalse;
					count = 0;
					}
				}
			count++;
			}
		while( (!found) && count < KBURErrorArrayLength );
		
		}
	
	return newState;
	}

