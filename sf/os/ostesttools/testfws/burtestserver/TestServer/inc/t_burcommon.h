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
*
*/



/**
 @file
 @released
*/

#ifndef __T_BURCOMMON_H__
#define __T_BURCOMMON_H__

#include <connect/sbtypes.h>
#include <s32file.h>

using namespace conn;

namespace bur_ts
	{
	
	// Directory names defining the Backup Archive naming scheme:
	_LIT(KRegFilesDir, "RegistrationData\\");
	_LIT(KPassiveDataDir, "Passive\\");
	_LIT(KActiveDataDir, "Active\\");
	_LIT(KBaseDataDir, "Base\\");
	_LIT(KIncrementalDataDir, "Incremental\\");
	_LIT(KSnapshotDataDir, "Snapshot\\");
	_LIT(KDataOwnerDir, "DataOwners\\");
	
	_LIT(KData, "data");
	_LIT(KSnapshot, "snapshot");
	_LIT(KIncrement, "increment");
	
	_LIT(KMidlet, "midlet");
	
	_LIT(KSlash, "\\");
	_LIT(KBackupExtn, ".bkp");
	
	_LIT(KSidDir, "IdData\\");
	
	// forward declaration
	class CBURTDataOwner;
	
	// Some usefull shorthands:
	typedef RPointerArray<CDataOwnerInfo> RDataOwnerArray;
	typedef RPointerArray<CSBSecureId> RSBSecureIdArray;

	typedef RPointerArray<CSBGenericTransferType> RTransferTypeArray;
	
	typedef RArray<CSBGenericDataType> RGenericDataTypeArray;
	
	// Indentation levels for logging:
	const TUint LOG_LEVEL1 = 1;
	const TUint LOG_LEVEL2 = 2;
	const TUint LOG_LEVEL3 = 3;
	const TUint LOG_LEVEL4 = 4;
	const TUint LOG_LEVEL5 = 5;
	const TUint LOG_LEVEL6 = 6;
	const TUint LOG_LEVEL7 = 7;
	const TUint LOG_LEVEL8 = 8;
	
	// Indentation marker for logging:
	_LIT(KIndentationMarker, "--");
	
	enum TBackupMode
		/**
		@released
		@internalComponent
		*/
		{
		EBackupMode = 0,
		ERestoreMode = 1
		};
		
	enum TCurrentState
		/**
		@released
		@internalComponent
		*/
		{
		ERegistration =0,
		ESnapshots = 1,
		EBase = 2,
		EIncrements
		};
		
	// ERROR MESSAGES:
	_LIT(KBURTestPanicText,"BURTestServer: ");
	
	// ERROR CODES:
	/** 
	Panic A null pointer has been encountered 
	@internalComponent
	*/
	const TInt KErrBURTestNullPointer 			= 1;
	
	/** 
	Panic An invalid file name
	@internalComponent
	*/
	const TInt KErrBURTestInvalidFileName 		= 2;
	
	}

#endif // __T_BURCOMMON_H__


