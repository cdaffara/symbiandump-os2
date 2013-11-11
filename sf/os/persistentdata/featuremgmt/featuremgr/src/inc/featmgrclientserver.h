/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef FEATMGRCLIENTSERVER_H
#define FEATMGRCLIENTSERVER_H

//  INCLUDES
#include <e32cmn.h>
#include <featmgr/featurecmn.h>
#include "featurepanics.h"

// DATA TYPES

// Opcodes used in message passing between client and server
// Important:   Make sure IsWriteOperation function of CFeatMgrSession
//              is updated if a new enum for function that will modify
//              feature (write operation) is added. 
enum TFeatMgrServRequest
    {
	/**
	* Fetches information whether a certain feature is supported.
	*
	* 0: TFeatureEntry - Feature entry
  	* 1: TInt - ret - Returns feature support value (0,1) or error code.
  	*/
	EFeatMgrFeatureSupported = 0,

	/**
	* Fetches information of subset of features support-status.
	*
	* 0: TInt - Count of features, notification will be requested
	* 1: TPtr8 - Feature UID array (RFeatureUidArray)
  	* 2: TInt - Count of features in response.
  	*/
	EFeatMgrFeaturesSupported,

   	/**
	* Lists all supported features.
	*
	* 0: In return as a parameter received reference to the client owned 
    *    RFeatureUidArray array contains Ids of all supported features.
  	*/
	EFeatMgrListSupportedFeatures,

   	/**
	* Number of all supported features.
	*
	* 0: TInt - err - Returns the number of all supported features.
  	*/
	EFeatMgrNumberOfSupportedFeatures,

	/**
	* Request for single or subset of features notification.
	*
	* 0: TPtr8 - On change notification, server sets changed UID (TUid)
  	* 1: TRequestStatus - Request to be signaled.
  	*/
	EFeatMgrReqNotify,
	
	/**
	* Uids associated with request for single or subset of features notification.
	*
	* 0: TInt - Count of features, notification will be requested
	* 1: TPtr8 - Feature UID array (RFeatureUidArray)
  	* 2: TInt - err - Operation error code.
  	*/
	EFeatMgrReqNotifyUids,
	
	/**
	* Request cancellation of single feature notification.
	*
	* 0: TUid - Feature UID
  	* 1: TRequestStatus - Request to be canceled
  	* 2: TInt - err - Operation error code.
  	*/
	EFeatMgrReqNotifyCancel,
	
	/**
	* Request cancellation of all features notification.
	*
  	* 0: TInt - err - Operation error code.
  	*/
	EFeatMgrReqNotifyCancelAll,

#ifdef EXTENDED_FEATURE_MANAGER_TEST
   	/**
  	* Number of notify features.
  	*
   	*/
    EFeatMgrNumberOfNotifyFeatures,

   	/**
   	* Number of allocated cells.
   	*
   	*/
    EFeatMgrCountAllocCells,
#endif
    
    /**
	* Enables a certain feature.
	*
	* 0: TUid - Feature UID
  	* 1: TInt - err - Operation error code.
  	*/
	EFeatMgrEnableFeature,
	
	/**
	* Disables a certain feature.
	*
	* 0: TUid - Feature UID
  	* 1: TInt - err - Operation error code.
  	*/
	EFeatMgrDisableFeature,

	/**
	* Adds a feature entry.
	*
	* 0: TFeatureEntry - Feature entry
  	* 1: TInt - err - Operation error code.
  	*/
    EFeatMgrAddFeature,

	/**
	* Sets a certain feature and associated data.
	*
	* 0: TUid - Feature UID
  	* 1: TBool - Feature enable or disable.
  	* 2: TInt - Feature data.
  	* 3: TInt - err - Operation error code.
  	*/
	EFeatMgrSetFeatureAndData,
	
	/**
	* Sets certain feature's data.
	*
	* 0: TUid - Feature UID
  	* 1: TInt - Feature data.
  	* 2: TInt - err - Operation error code.
  	*/
	EFeatMgrSetFeatureData,

	/**
	* Deletes a feature entry.
	*
	* 0: TUid - Feature UID
  	* 1: TInt - err - Operation error code.
  	*/
    EFeatMgrDeleteFeature,
    
    /**
    * Software Installation started
    * 
    */ 
    EFeatMgrSWIStart,
    
    /**
    * Software Installation ended
    * 
    */ 
    EFeatMgrSWIEnd,
    
    EFeatMgrResourceMark,
    EFeatMgrResourceCheck,
    EFeatMgrResourceCount,
    EFeatMgrSetHeapFailure
    
    };



#endif  // FEATMGRCLIENTSERVER_H
            
// End of File
