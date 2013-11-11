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



#include "mixed_feature_info_plugin.h"

//header file declaring ECOM structures needed to resolve interface implementations
#include <implementationproxy.h>

// Map the interface implementation UIDs to implementation factory functions
const TImplementationProxy ImplementationTable[] =
    {
    IMPLEMENTATION_PROXY_ENTRY(0x10283706,	CMixedFeatureInfoPlugin::NewL),
    };

// Exported proxy for instantiation method resolution.
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    return ImplementationTable;
    }

//factory function for our plugin
CMixedFeatureInfoPlugin* CMixedFeatureInfoPlugin::NewL()
    {
    CMixedFeatureInfoPlugin* self = new(ELeave)CMixedFeatureInfoPlugin();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

CMixedFeatureInfoPlugin::CMixedFeatureInfoPlugin()
    {
    //perform any basic initialisation here
    }

void CMixedFeatureInfoPlugin::ConstructL()
    {
    //perform any initialisation that may cause our code to leave
	}

CMixedFeatureInfoPlugin::~CMixedFeatureInfoPlugin()
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
void CMixedFeatureInfoPlugin::ProcessCommandL( const FeatureInfoCommand::TFeatureInfoCmd aCommandId,
        const TUint8 aTransId,
        TDesC8& /*aData*/ )
{
	if (aCommandId != FeatureInfoCommand::ELoadFeatureInfoCmdId && aCommandId != FeatureInfoCommand::ELoadEnhancedFeatureInfoCmdId) //we support FeatureInfoCommand::ELoadFeatureInfoCmdId and FeatureInfoCommand::ELoadEnhancedFeatureInfoCmdId commands only
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
	//store command ID and transaction ID - we will need them in order to identify current command when the timer expires 
	iCommandId = aCommandId;
	iTransId = aTransId;
	//create the timer object	
	iTimer = CPluginTimer::NewL(timeout, this);
}

//method called by the timer object when the timeout expires
void CMixedFeatureInfoPlugin::GenericTimerFiredL()
	    {
	    TInt err( KErrNone );
        
	    switch ( iCommandId )
	        {
	        case FeatureInfoCommand::ELoadFeatureInfoCmdId:
	            {
	            // Return some features and KErrNone.
	            FeatureInfoCommand::TFeatureInfo featureList;
	            RArray<FeatureInfoCommand::TFeature> list;
	            CleanupClosePushL( list );
	            FeatureInfoCommand::TFeature feature1; 

	            const TInt KImaginaryId1( 0x18273644 );
	            feature1.iFeatureID = KImaginaryId1;
	            feature1.iValue = ETrue;
	            list.AppendL(feature1);
	            
	            featureList.iErrorCode = KErrNone;
	            featureList.iList = list;
	                
	            FeatureInfoCommand::TFeatureInfoRespPckg resPackage(featureList);
	            
	            TRAP(err, iResponseCallback->ProcessResponseL(iCommandId, iTransId, resPackage));    
	            
	            CleanupStack::PopAndDestroy( &list );
	            break;
	            }
	            
	        case FeatureInfoCommand::ELoadEnhancedFeatureInfoCmdId:
	            {
	            // Return some features and KErrNone.
	            FeatureInfoCommand::TEnhancedFeatureInfo featureList;
	            RFeatureArray features; 
	            CleanupClosePushL( features );
	            TBitFlags32 flags( 0 );
	            flags.Set( EFeatureSupported );
	            flags.Set( EFeatureModifiable );
	            const TInt KDefaultData( 0x00ff00ff );

	            const TInt KImaginaryId2( 0x18273645 );
	            TFeatureEntry entry2( TUid::Uid(KImaginaryId2), flags, KDefaultData );
	            features.AppendL( entry2 );
	            
	            featureList.iErrorCode = KErrNone;
	            featureList.iList = features;
	                
	            FeatureInfoCommand::TEnhancedFeatureInfoRespPckg resPackage(featureList);
	            
	            TRAP(err, iResponseCallback->ProcessResponseL(iCommandId, iTransId, resPackage));    
	            
	            CleanupStack::PopAndDestroy( &features );
	            break;
	            }
	            
	        default :
	        	err = KErrArgument;
	            break;
	        }
	 	    
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
