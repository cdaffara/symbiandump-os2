
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



// INCLUDE FILES
#include <featmgr/featurecontrol.h>

// LOCAL CONSTANTS AND MACROS
_LIT(KTxt,"featureinstaller: mainL failed");


// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// mainL
// -----------------------------------------------------------------------------
//  
LOCAL_C void mainL()
	{
	// Open Feature Control.
	RFeatureControl featureControl;
    User::LeaveIfError( featureControl.Open() );
    
    // Example code adds a new persisted feature to the device.
    // Comment or uncomment code when needed. 
    
    // Specify in your .pkg file:
    // @"featureinstaller.sisx", (0x10283303)

    
    // Replace UID 0x00000000 with real value
    TUid featureUid( TUid::Uid( 0x00000000 ) ); 
    
    // Set feature flags of the feature!
   
    // Set all flags to zero.
    // Comment when needed!
    TBitFlags32 featureFlags( 0 );

    // If set, feature is supported and available for use; 
    // if not, feature is not supported.
    // Comment when needed!
    featureFlags.Set( EFeatureSupported );
    
    // If set, feature is upgradeable. The feature is known to the device
    // but it must be upgraded to enable it. If a feature s blacklisted, 
    // its upgradeable flag is unset. 
    // Uncomment when needed!
    // featureFlags.Set( EFeatureUpgradeable );
    
    // If set, the feature is modifiable and may be enabled/disabled 
    // at run-time. The initial flag values for such a feature flag are
    // defined in a ROM image obey file.
    // Comment when needed!
    featureFlags.Set( EFeatureModifiable );
    
    // If set, the feature has been blacklisted, and may not be changed at 
    // run-time. This also prevents a feature from being upgraded.
    // Uncomment when needed!
    // featureFlags.Set( EFeatureBlackListed );

    // If set, only clients with WriteDeviceData capability can modify it.
    // This ensures only trusted software can set the feature Supported flag.
    // Uncomment when needed!
    // featureFlags.Set( EFeatureProtected );
    
    // If set, this flag is saved to the system drive when modified 
    // preserving its value across reboots/power downs.
    // Comment when needed!
    featureFlags.Set( EFeaturePersisted );
    
    // If set, this flag Supported state is unknown at build-time and is
    // initialised at run-time by system software. The Feature Manager will
    // ignore the Supported flag in the file. A run-time call to RFeatureControl
    // will be needed to set the feature's supported flag. Look ups of 
    // uninitialised features result in a KErrNotReady error code
    // Uncomment when needed!
    // featureFlags.Set( EFeatureUninitialized );

    // Feature data is 32-bit quantity for client read and write. Feature data is
    // defined by owner of the feature and can contain for example flags, 
    // enumeratons and/or integers.
    // Set feature data. Replace <featureData> with real value!
    // Comment when needed!
    TUint32 featureData( 0x00000000 );
    
    // Comment when needed!
    TFeatureEntry entry( featureUid, featureFlags, featureData );
    
    TInt err( KErrNone );
    
    // Inform feature manager that your executable is launched by the software installer (SWI)
    // and it wishes to set, add, delete, enable or disable features in feature manager. This
    // function must be called before any API calls by RFeatureControl that add, set, delete 
    // enable, or disable features so that changes in feature manager are cached to be rolled 
    // back if the installationhas user-aborted or failed. Otherwise, feature manipulations will
    // be committed directly into feature manager and will not be possible to undo them in the
    // case of abort.
    // A call to SWIStart must be accompanied by a call to SWIEnd after all API calls by
    // RFeatureControl functions that manipulate features.
    User::LeaveIfError(featureControl.SWIStart()); 

    // Add a new feature to the device.
    // Comment when needed!
    err = featureControl.AddFeature( entry );
    
    if ( err == KErrAlreadyExists ) // Existing feature cannot be added as a new feature.
        {
        // Enable an existing feature.
        User::LeaveIfError( featureControl.EnableFeature( featureUid ) );
        
        // OR enable an exsiting feature and set feature data.
        // Uncomment when needed!
        //User::LeaveIfError( featureControl.SetFeature( featureUid, ETrue, featureData ) );
        }
  
    // Inform feature manager that caching feature manipulations should stop and they should be 
    // committed to feature manager.
    // A call to this API should follow the call to SWIStart before any RFeatureControl API calls
    // that manipulate features to feature manager.
    User::LeaveIfError(featureControl.SWIEnd());

    featureControl.Close();
    }
// ============================ MEMBER FUNCTIONS ===============================

TInt E32Main() // main function called by E32
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup=CTrapCleanup::New(); // get clean-up stack
	TRAPD( error,mainL() ); // Run main method
	__ASSERT_ALWAYS( !error,User::Panic(KTxt,error) );
	delete cleanup; // destroy clean-up stack
	__UHEAP_MARKEND;
	return 0; // and return
	}


// End of file
