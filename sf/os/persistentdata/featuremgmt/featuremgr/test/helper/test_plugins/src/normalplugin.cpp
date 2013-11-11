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



#include "normalplugin.h"
#include <featmgr/featureinfoplugin.h>
#include "featmgrdebug.h"
#include "efm_test_consts.h"
#include <ecom/implementationproxy.h>
#include <e32property.h>

// Map the interface implementation UIDs to implementation factory functions
const TImplementationProxy ImplementationTable[] =
    {
    IMPLEMENTATION_PROXY_ENTRY(0x102836F6,	CFeatMgrNormalPlugin::NewL),
    };

// Exported proxy for instantiation method resolution.
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    return ImplementationTable;
    }

CFeatMgrNormalPlugin* CFeatMgrNormalPlugin::NewL()
    {
    CFeatMgrNormalPlugin* self = new(ELeave) CFeatMgrNormalPlugin();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

CFeatMgrNormalPlugin::~CFeatMgrNormalPlugin()
    {
    }

CFeatMgrNormalPlugin::CFeatMgrNormalPlugin()
    {
    }

void CFeatMgrNormalPlugin::ConstructL()
    {
    }
    
void CFeatMgrNormalPlugin::GenericTimerFiredL(
	    MFeatureInfoPluginCallback& aService,
	    FeatureInfoCommand::TFeatureInfoCmd aCommandId,
	    TUint8 aTransId,
	    TInt aRetVal )
	    {
	    INFO_LOG2( "FeatMgr Plugin: GenericTimerFiredL (0x%x, 0x%x)", 
	                         aCommandId, aTransId );
	    TInt err( KErrNone );
	    
	    switch ( aCommandId )
	        {
	        case FeatureInfoCommand::ELoadFeatureInfoCmdId:
	            {
	            // Testing the capability access privelige for a plugin
	            TInt propVal = KErrNotFound;
	        	
	        	err = RProperty::Get(KPropertyCat2, KPropertyKey, propVal);
	            // If the property is defined then run the capability-ralated code. Otherwise run the normal code for the plugin
	        	if( KErrNone == err )
	            	{
	            	if( propVal == ENonReadDeviceDataCap )
	            		{
	            		const TInt KSomeVal = -55;
	            		err = RProperty::Set(KAccessTestPropertyCat, KPropertyKey, KSomeVal);
	            		ERROR_LOG1( "FeatMgr Plugin: Error value returned from RProperty::Set() when no ReadDeviceData privalige is obtained: %d", err );
	    	            // Resetting the property value with the error result from calling an API that requires capabilities other than ReadDeviceData.
	            		// This value is used by the test case to check for passing and failing.
	            		err = RProperty::Set(KPropertyCat2, KPropertyKey, err);
	            		ERROR_LOG1( "FeatMgr Plugin: Error value returned from RProperty::Set() after resetting KPropertyCat2: %d", err );
	    	            }
	            	else if( propVal == EReadDeviceDataCap )
	            		{
	            		TInt KSomeVal; 	// Dummy variable to hold the value read from the property. Not needed in the plugin but required by RProperty::Get()
	            		err = RProperty::Get(KAccessTestPropertyCat, KPropertyKey, KSomeVal);
	            		ERROR_LOG1( "FeatMgr Plugin: Error value returned from RProperty::Get() when ReadDeviceData privalige is obtained: %d", err );
	    	            err = RProperty::Set(KPropertyCat2, KPropertyKey, err);
	            		ERROR_LOG1( "FeatMgr Plugin: Error value returned from RProperty::Set() after resetting KPropertyCat2: %d", err );
	    	            }
	            	}
	            else 
	            	{
		            // Return some features and KErrNone.
		            FeatureInfoCommand::TFeatureInfo featureList;
		            TInt err( KErrNone );
		            RArray<FeatureInfoCommand::TFeature> list;
		            CleanupClosePushL( list );
		            FeatureInfoCommand::TFeature feature1; 

		            feature1.iFeatureID = KDefaultSupportedUid.iUid;
		            feature1.iValue = ETrue;
		            list.AppendL(feature1);
		            feature1.iFeatureID = KBlacklistedUid.iUid;
		            feature1.iValue = ETrue;
		            list.AppendL(feature1);
		            feature1.iFeatureID = KDSR1Uid.iUid;
		            feature1.iValue = EFalse;
		            list.AppendL(feature1);
		            feature1.iFeatureID = KPluginIgnoringUid.iUid;
		            feature1.iValue = ETrue;
		            list.AppendL(feature1);
		            
		            featureList.iErrorCode = KErrNone;
		            featureList.iList = list;
		                
		            FeatureInfoCommand::TFeatureInfoRespPckg resPackage(featureList);
		            
		            TRAP(err, aService.ProcessResponseL(aCommandId, aTransId, resPackage));    
		            
		            CleanupStack::PopAndDestroy( &list );
	            	}      	
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
	    CFeatMgrBasePlugin::GenericTimerFiredL(aService,aCommandId,aTransId,aRetVal);
	    	    
	    }
