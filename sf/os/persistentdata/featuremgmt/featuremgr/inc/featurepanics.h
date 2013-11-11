// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalComponent
*/

#ifndef FEATUREPANICS_H
#define FEATUREPANICS_H

/**
The following enumaration defines the Feature Manager panic codes.
They are provided for debug purposes only. They are internal values
that may change in future.
*/
enum TFeatMgrPanic
{
    //EFmpUndefined = 0,
    
    // TLS has not been set up to the thread using Feature Manager.    
    ELibNotInitialized = 0,

    // Illegal function called via client-server interface
    EPanicIllegalFunction,
    
    // Bad handle in client-server interface
	EPanicBadHandle,
	
	// Bad descriptor in client-server interface
	EPanicBadDescriptor,
	
	// Invalid feature info
	EPanicInvalidFeatureInfo,
	
	// Feature change notification has already been requested.
	EPanicNotifyRequest,
	
	// Illegal argument, cannot identify the message.
	EPanicIllegalArgument,
	
	// Error when loading existing plugin.
	EPanicLoadPluginError,

	// No feature files have been found in the ROM; indicates integration error.
	EPanicNoFeatureFiles,
	
	// No DSRs have been found in the ROM; indicates integration error.
	EPanicNoDSR,
	
	/** The Feature Manager server has found an invalid feature bit flag 
    combination. ROM defined feature has broken rule 1. A blacklisted ROM 
    defined feature had one of modifiable, persisted or uninitialised 
    flags set. */
	EFmpInvalidFeatureBitFlagsRule1,
	
	/** The Feature Manager server has found an invalid feature bit flag 
    combination. ROM defined feature has broken rule 2. A ROM defined read-only
    feature had either persist or uninitialised flags set. */
	EFmpInvalidFeatureBitFlagsRule2,
	
	/** The Feature Manager server has found an invalid feature bit flag 
    combination. Feature added using the API broke rule 3. Run-time created
    feature had the black listed flag set. Only used for ROM defined features */
	EFmpInvalidFeatureBitFlagsRule3,
	
	/** The Feature Manager server has found an invalid feature bit flag 
    combination. ROM defined feature has broken rule 4. Run-time created
    feature had the uninitialised flag set but was marked as non-modifiable */
	EFmpInvalidFeatureBitFlagsRule4,
	
};


#endif  // FEATUREPANICS_H

// End of File
