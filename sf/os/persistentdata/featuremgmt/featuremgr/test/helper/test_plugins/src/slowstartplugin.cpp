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



#include "slowstartplugin.h"
#include <featmgr/featureinfoplugin.h>
#include "featmgrdebug.h"
#include "efm_test_consts.h"     
#include <ecom/implementationproxy.h>
#include "featmgrconfiguration.h"


// Map the interface implementation UIDs to implementation factory functions
const TImplementationProxy ImplementationTable[] =
    {
    IMPLEMENTATION_PROXY_ENTRY(0x102836FC,	CFeatMgrSlowStartPlugin::NewL),
    };

// Exported proxy for instantiation method resolution.
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    return ImplementationTable;
    }

CFeatMgrSlowStartPlugin* CFeatMgrSlowStartPlugin::NewL()
    {
    CFeatMgrSlowStartPlugin* self = new(ELeave) CFeatMgrSlowStartPlugin();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

CFeatMgrSlowStartPlugin::~CFeatMgrSlowStartPlugin()
    {
    }

CFeatMgrSlowStartPlugin::CFeatMgrSlowStartPlugin()
    {
    }

void CFeatMgrSlowStartPlugin::ConstructL()
    {
    }

void CFeatMgrSlowStartPlugin::ProcessCommandL( const FeatureInfoCommand::TFeatureInfoCmd aCommandId,
                                      const TUint8 aTransId,
                                      TDesC8& /*aData*/ )
    {
    INFO_LOG2( "FeatMgr SlowStart Plugin: Processing command: 0x%x, TransId: 0x%x", 
                         aCommandId, aTransId );

    // Slow things down with a 1/2 second delay.
    User::After(200000);
           
    switch ( aCommandId )
        {
        case FeatureInfoCommand::ELoadFeatureInfoCmdId:
            {
            INFO_LOG( "FeatMgr SlowStart Plugin: Processed ELoadFeatureInfoCmdId" );
            // Load feature info
            }
            break;
        case FeatureInfoCommand::ELoadEnhancedFeatureInfoCmdId:
            {
			INFO_LOG( "FeatMgr SlowStart Plugin: extended command interface not supported");
			User::Leave(KErrNotSupported);
            }
        default :
            {
            INFO_LOG1( "FeatMgr SlowStart Plugin: Unknown Command: 0x%x", aCommandId );
            User::Leave(KErrNotSupported);
            }
            break;
        }
        
    TInt retval(KErrNone);
    TInt timeout( 0.5*KDefaultFeatMgrTimer ); // We don't actually want to timeout, but add an extra delay.

    // create new timer
    if ( iTimer && !iTimer->IsActive() )
        {
        delete iTimer;
        iTimer = NULL;
        }
        
    iTimer = CPluginTimer::NewL(timeout, *iResponseCallback, aCommandId, aTransId, retval,this);
    }


void CFeatMgrSlowStartPlugin::GenericTimerFiredL(
	    MFeatureInfoPluginCallback& aService,
	    FeatureInfoCommand::TFeatureInfoCmd aCommandId,
	    TUint8 aTransId,
	    TInt aRetVal )
	    {
	    CFeatMgrBasePlugin::GenericTimerFiredL(aService,aCommandId,aTransId,aRetVal);
	    INFO_LOG2( "FeatMgr SlowStart Plugin: GenericTimerFiredL (0x%x, 0x%x)", 
	                         aCommandId, aTransId );
	    
	    TInt err( KErrNone );
	    
	    // Delay things a little.
	    User::After(600000);

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

	            feature1.iFeatureID = KSlowStartPluginUid.iUid;
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

