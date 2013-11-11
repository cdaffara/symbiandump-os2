/**
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Contains defines shared between the Active Backup client and Server.
* 
*
*/



/**
 @file
*/

#ifndef __ABCLIENTSERVER_H__
#define __ABCLIENTSERVER_H__

#include <connect/tserverstart.h>

namespace conn
	{
	/** The name of the AB server.
	@ingroup Client
	@internalComponent
	*/
	_LIT(KABServerName,"!ABServer");

	/** Number of times to attempt connection to server.
	@ingroup Client
	@internalComponent
	*/
	const TInt KABRetryCount = 4;

	/** Number of async message slots.
	@ingroup Client
	@internalComponent
	*/
	const TInt KABASyncMessageSlots = 1;


	/** The AB major version number.
	@ingroup Client
	@internalComponent
	*/
	const TUint KABMajorVersionNumber = 1;

    /** The AB minor version number.
	@ingroup Client
	@internalComponent
	*/
	const TUint KABMinorVersionNumber = 0;

    /** The AB build number
	@ingroup Client
	@internalComponent
	*/
	const TUint KABBuildVersionNumber = 0;
	
	/** Size of data blocks to send back to the PC from the ABClient
	@ingroup Client
	@internalComponent
	*/
	const TInt KABMaxSendDataBlock = 65536;

	enum TABMessages
	/** AB Client/Server Messages
	@internalComponent
	*/
		{
		EABMsgBURModeInfo,					/*!< Get info about the backup/restore mode */
		EABMsgDoesPartialAffectMe,			/*!< Query whether or not this client is being operated on */
		EABMsgConfirmReadyForBUR,			/*!< Confirm to the SBEngine that all data has now been prepared */
		EABMsgPrimeForCallback,				/*!< This is the first msg that primes server for callbacks */
		EABMsgPrimeForCallbackAndResponse,	/*!< Re-prime the server, sending a response from the last call */
		EABMsgPrimeForCallbackAndResponseDes, /*!< As for PrimeForCallbackAndResponse but for sending a descriptor */
		EABMsgPropagateLeave,				/*!< Propagate a leave from the callback, back to the engine */
		EABMsgGetDataSync,					/*!< Synchronous call to get data from the server */
		EABMsgSendDataLength,				/*!< Synchronous call to send the length of data to the server */
		EABMsgClosingDownCallback,			/*!< Inform the server that we're closing down the callback interface */
		EABMsgGetDriveNumForSuppliedSnapshot /*!< Called from within the ReceiveSnapshot callback to return the drive num */
		};

	enum TABCallbackCommands
	/**
	List of commands that the server requests the client to call on it's callback interface
	@internalComponent
	*/
		{
		EABCallbackAllSnapshotsSupplied,
		EABCallbackReceiveSnapshotData,
		EABCallbackGetExpectedDataSize,
		EABCallbackGetSnapshotData,
		EABCallbackInitialiseGetBackupData,
		EABCallbackGetBackupDataSection,
		EABCallbackInitialiseRestoreBaseDataSection,
		EABCallbackRestoreBaseDataSection,
		EABCallbackInitialiseRestoreIncrementData,
		EABCallbackRestoreIncrementDataSection,
		EABCallbackRestoreComplete,
		EABCallbackInitialiseGetProxyBackupData,
		EABCallbackInitialiseRestoreProxyBaseData,
		EABCallbackTerminateMultiStageOperation,
		EABCallbackGetDataChecksum
		};
		
	/** The maximum size of descriptor data passed with each IPC call 
	@internalComponent
	*/
	const TInt KIPCMessageSize = 4096;

	}

#endif //__ABCLIENTSERVER_H__
