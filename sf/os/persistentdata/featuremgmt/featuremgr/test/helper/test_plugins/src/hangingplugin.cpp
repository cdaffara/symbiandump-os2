// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#include "hangingplugin.h"
#include <featmgr/featureinfoplugin.h>
#include "featmgrdebug.h"
#include "efm_test_consts.h"     
#include <ecom/implementationproxy.h>
#include "featmgrconfiguration.h"


// Map the interface implementation UIDs to implementation factory functions
const TImplementationProxy ImplementationTable[] =
    {
    IMPLEMENTATION_PROXY_ENTRY(0x102836FA,	CFeatMgrHangingPlugin::NewL),
    };

// Exported proxy for instantiation method resolution.
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    return ImplementationTable;
    }

CFeatMgrHangingPlugin* CFeatMgrHangingPlugin::NewL()
    {
    CFeatMgrHangingPlugin* self = new(ELeave) CFeatMgrHangingPlugin();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

CFeatMgrHangingPlugin::~CFeatMgrHangingPlugin()
    {
    }

CFeatMgrHangingPlugin::CFeatMgrHangingPlugin()
    {
    }

void CFeatMgrHangingPlugin::ConstructL()
    {
    }
    
void CFeatMgrHangingPlugin::ProcessCommandL( const FeatureInfoCommand::TFeatureInfoCmd aCommandId,
                                      const TUint8 aTransId,
                                      TDesC8& /*aData*/ )
    {
    INFO_LOG2( "FeatMgr Hanging Plugin: Processing command: 0x%x, TransId: 0x%x", 
                         aCommandId, aTransId );
           
    switch ( aCommandId )
        {
        case FeatureInfoCommand::ELoadFeatureInfoCmdId:
            {
            INFO_LOG( "FeatMgr Hanging Plugin: Processed ELoadFeatureInfoCmdId" );
            // Load feature info
            }
            break;
        case FeatureInfoCommand::ELoadEnhancedFeatureInfoCmdId:
            {
			INFO_LOG( "FeatMgr Hanging Plugin: extended command interface not supported");
			User::Leave(KErrNotSupported);
            }
        default :
            {
            INFO_LOG1( "FeatMgr Hanging Plugin: Unknown Command: 0x%x", aCommandId );
            User::Leave(KErrNotSupported);
            }
            break;
        }
        
    TInt retval(KErrNone);
    TInt timeout( 1.5*KDefaultFeatMgrTimer ); // microseconds

    // create new timer


    if ( iTimer && !iTimer->IsActive() )
        {
        delete iTimer;
        iTimer = NULL;
        }
        
    iTimer = CPluginTimer::NewL(timeout, *iResponseCallback, aCommandId, aTransId, retval,this);
    }
    
    
void CFeatMgrHangingPlugin::GenericTimerFiredL(
	    MFeatureInfoPluginCallback& aService,
	    FeatureInfoCommand::TFeatureInfoCmd aCommandId,
	    TUint8 aTransId,
	    TInt aRetVal )
	    {
	    CFeatMgrBasePlugin::GenericTimerFiredL(aService,aCommandId,aTransId,aRetVal);
	    INFO_LOG2( "FeatMgr Hanging Plugin: GenericTimerFiredL (0x%x, 0x%x)", 
	                         aCommandId, aTransId );
	    
	    TInt err( KErrNone );
	    
	    switch ( aCommandId )
	        {
	        case FeatureInfoCommand::ELoadFeatureInfoCmdId:
	            {
	            // Return some features and KErrNone.
	            FeatureInfoCommand::TFeatureInfo featureList;
	            TInt err( KErrNone );
	            RArray<FeatureInfoCommand::TFeature> list;
	            CleanupClosePushL( list );
	            FeatureInfoCommand::TFeature feature1; 

	            feature1.iFeatureID = KHangingPluginUid.iUid;
	            feature1.iValue = ETrue;
	            list.AppendL(feature1);
	            
	            featureList.iErrorCode = KErrNone;
	            featureList.iList = list;
	                
	            FeatureInfoCommand::TFeatureInfoRespPckg resPackage(featureList);
	            
	            TRAP(err, aService.ProcessResponseL(aCommandId, aTransId, resPackage));    
	            
	            CleanupStack::PopAndDestroy( &list );
	            break;
	            }
	            
	        default :
	            break;
	        }
	    
	    // TEST CASES END
	 
	    if ( err != KErrNone )
	        {
	        ERROR_LOG1( "FeatMgr Plugin: Error in ProcessResponseL: %d", err );
	        }	    
	    }
