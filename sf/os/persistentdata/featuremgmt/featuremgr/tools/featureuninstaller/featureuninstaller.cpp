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
_LIT(KTxt,"featureuninstaller: mainL failed");


// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// mainL
// -----------------------------------------------------------------------------
//  
LOCAL_C void mainL()
	{
	// Connect Feature Control
	RFeatureControl featureControl;
    User::LeaveIfError( featureControl.Open() );
    
    // Example code disables an existing feature.
    // Comment or uncomment code when needed. 
    
    // Specify in your .pkg file:
    // @"featureuninstaller.sisx", (0x10283304)
    
    //Replace UID 0x00000000 with real value
	TUid featureUid( TUid::Uid( 0x00000000 ) ); 
	
    // Feature data is 32-bit quantity for client read and write. Feature data is
    // defined by owner of the feature and can contain for example flags, 
    // enumeratons and/or integers.
    // Set feature data. Replace <featureData> with real value!
    // Uncomment when needed!
    // TUint32 featureData( 0x00000000 );
    
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

    // Disable an existing feature.
    User::LeaveIfError( featureControl.DisableFeature( featureUid ) );
    
    // Inform feature manager that caching feature manipulations should stop and they should be 
    // committed to feature manager.
    // A call to this API should follow the call to SWIStart before any RFeatureControl API calls
    // that manipulate features to feature manager.
    User::LeaveIfError(featureControl.SWIEnd());
    
    // OR disable an exsiting feature and set feature data.
    // User::LeaveIfError( featureControl.SetFeature( featureUid, EFalse, featureData ) );
    
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
