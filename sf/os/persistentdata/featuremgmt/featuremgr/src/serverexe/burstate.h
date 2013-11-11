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

#ifndef BURSTATE_H
#define BURSTATE_H

// INCLUDES
#include <e32svr.h>
#include <featmgr/featurecmn.h>
#include <featmgr/featureinfoplugin.h>
#include <babackup.h>
#include "featmgrclientserver.h"
#include "featmgrtimer.h"
#include "featmgrfeatureregistry.h"

// CLASS DECLARATION
class CFeatMgrServer;
class CBurState;

/**
 * States of the backup and restore state-machine.
 */ 
typedef enum 
	{
	/** Normal state : Normal operation, not in a BUR state */
	EFeatMgrBURState_None = 0,          
	/** Error state */
	EFeatMgrBURState_Error,             
	/** Backup commands */
	EFeatMgrBURState_BackupStarted,     
	EFeatMgrBURState_BackupEnded,
	/** Restore commands */
	EFeatMgrBURState_RestoreStarted,    
	EFeatMgrBURState_RestoreEnded,
	} BURStatus;

const TInt KBURArrayLength = 7;
const TInt KBURErrorArrayLength = 5;


/** 
 * State machine for handling the BUR operations for Feature Manager.
 */ 
class CBurState
	{
	public: 

	~CBurState();
	CBurState(CFeatMgrServer* aServer);

	static CBurState* NewL();
	static CBurState* NewLC();
	
   	/**
     * Function from MBackupOperationObserver in response to a change in the state of the backup and
     * restore operation(s). This function is called to notify the Feature Manager that a backup and 
     * restore operation is in progress.
     * @param aBackupOperationAttributes The current state of the backup operation. Made up of 
     *        MBackupObserver::TFileLockFlags and TOperationType types.
     * @see	MBackupObserver::TFileLockFlags 
     * @see	TOperationType.
     */
	 void BUROperationL(const TBackupOperationAttributes& aBackupOperationAttributes);

	private:

	CBurState();
	void ConstructL();
	
   /** 
     * Used to check the TBackupOperationAttributes values that have arrived in Feature Manager 
     * as part of HandleBackupOperationEventL. This function makes sure the arguments from the Backup and Restore
     * server can be properly understood by the Feature Manager.
     * @param  TBackupOperationAttributes
     * @return Error code. Can either be KErrNone or KErrNotFound if an argument is not understood.
     * @see    CFeatMgrFeatureRegistry::HandleBackupOperationEventL
     */
    TInt CheckBURArguments(  const TBackupOperationAttributes& aBackupOperationAttributes );
    
    /** 
     * Conversion function will change TBackupOperationAttributes into a form more easily managed.
     * @param TBackupOperationAttributes
     * @return BURStatus
     */
    BURStatus ConvertToBURState(  const TBackupOperationAttributes& aBackupOperationAttributes );        

    /**
     * Increment the state machine.
     * @param aCurrent : current state (location)
     * @param aGoto : state to go to next (direction)
     * @return next state
     */
    BURStatus BUR_StateMachine( BURStatus aCurrent, BURStatus aGoto );
    
    void SetUpBurStruct();
    
    // Backup Notification
    TUint32 iBackupFlag;
    CBaBackupSessionWrapper *iBackupNotification;
    BURStatus iCurrentBURStatus;
    CFeatMgrServer *iServer;
    
    /** The file lock status of the file we are interested in. */ 
    MBackupObserver::TFileLockFlags iBURLockFlag;
    /** The current BUR operation, e.g. Start/End. */ 
    MBackupOperationObserver::TOperationType iBUROpType;
        
    /**
     * A structure for storing the information on the possible state elements
     * that are used for backup and restore. Each element has a current state, and
     * a direction to goto. e.g. (s1)-> (s2)-> (s3)-> (s4)-> (s5)-> etc..
     * The state machine is formed from a series of these interconnecting elements, 
     * and transition between states is handled by an associated function.
     */
    struct BURStruct
        {
        BURStatus iCurrent;
        BURStatus iGoto;
        
        /** A function pointer that is called to switch between states */
        BURStatus  (CFeatMgrServer::*iFunc)( BURStatus );
        };
    
    /** 
     * State machine architecture, written as BURStruct elements. This array defines the 
     * normal operation paths of the state machine.
     */
    BURStruct iBURStructArray[ KBURArrayLength ];
    /** 
     * Error handling is taken care of in the same manner as normal operation 
     * in the state machine architecture. Also written as written as BURStruct elements.
     * The error array defines a set of paths to take upon discovering a path that is 
     * not supported by the normal operation paths.
     */
    BURStruct iBURStructErrorArray[ KBURErrorArrayLength ];

	};

#endif // BURSTATE_H

