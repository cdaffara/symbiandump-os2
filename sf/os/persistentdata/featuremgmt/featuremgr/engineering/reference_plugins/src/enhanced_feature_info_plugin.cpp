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



#include "enhanced_feature_info_plugin.h"

//header file declaring ECOM structures needed to resolve interface implementations
#include <implementationproxy.h>

// Map the interface implementation UIDs to implementation factory functions
const TImplementationProxy ImplementationTable[] =
    {
    IMPLEMENTATION_PROXY_ENTRY(0x10283704,	CEnhancedFeatureInfoPlugin::NewL),
    };

// Exported proxy for instantiation method resolution.
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    return ImplementationTable;
    }

//factory function for our plugin
CEnhancedFeatureInfoPlugin* CEnhancedFeatureInfoPlugin::NewL()
    {
    CEnhancedFeatureInfoPlugin* self = new(ELeave)CEnhancedFeatureInfoPlugin();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

CEnhancedFeatureInfoPlugin::CEnhancedFeatureInfoPlugin()
    {
    //perform any basic initialisation here
    }

void CEnhancedFeatureInfoPlugin::ConstructL()
    {
    //perform any initialisation that may cause our code to leave
	}

CEnhancedFeatureInfoPlugin::~CEnhancedFeatureInfoPlugin()
{
	//make sure that our helper object that we used to implement asynchronous response is deleted
	if ( iTimer )
		{
		delete iTimer;
		iTimer = NULL;
		}
}

//ProcessCommandL function is invoked by the server in order to pass acommand to the plugin.
//the plugin is expected to initiate asynchronous command handling and return imediately
void CEnhancedFeatureInfoPlugin::ProcessCommandL( const FeatureInfoCommand::TFeatureInfoCmd aCommandId,
        const TUint8 aTransId,
        TDesC8& /*aData*/ )
{
	if (aCommandId != FeatureInfoCommand::ELoadEnhancedFeatureInfoCmdId) //we only support FeatureInfoCommand::ELoadEnhancedFeatureInfoCmdId
		{
		User::Leave(KErrNotSupported);
		}
		
	TInt timeout( 50 ); // microseconds

	// create new timer
	if ( iTimer && !iTimer->IsActive() )
		{
		delete iTimer;
		iTimer = NULL;
		}
	
	//store command ID and transaction ID - we will need them in order to identify the current command when the timer expires 
	iCommandId = aCommandId;
	iTransId = aTransId;
	//create the timer object
	iTimer = CPluginTimer::NewL(timeout, this);
}

//method called by the timer object when the timeout expires
void CEnhancedFeatureInfoPlugin::GenericTimerFiredL()
	    {
	    // Return some features and set error code to KErrNone.
	    FeatureInfoCommand::TEnhancedFeatureInfo featureList;
        RFeatureArray features; 
        CleanupClosePushL( features );
        TBitFlags32 flags( 0 );
        flags.Set( EFeatureSupported );
        flags.Set( EFeatureModifiable );
        const TInt KDefaultData( 0x00ff00ff );

        const TInt KImaginaryId( 0x18273645 );
        //create a feature with UID, state information and additional data word
        TFeatureEntry entry( TUid::Uid(KImaginaryId), flags, KDefaultData );
        //append the feature to the feature list
        features.AppendL( entry );
        
        featureList.iErrorCode = KErrNone;
        featureList.iList = features;
            
        FeatureInfoCommand::TEnhancedFeatureInfoRespPckg resPackage(featureList);
        
        TRAPD(err, iResponseCallback->ProcessResponseL(iCommandId, iTransId, resPackage));    
        
        CleanupStack::PopAndDestroy( &features );
	 	    
	    if ( iTimer && !iTimer->IsActive() )
	        {
	        delete iTimer;
	        iTimer = NULL;            
	        }
	    
	    if (err!=KErrNone)
	    	{
	    	//perform any error handling here
	    	}
	    }
