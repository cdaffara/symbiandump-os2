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



#include "corruptplugin.h"
#include <featmgr/featureinfoplugin.h>
#include "featmgrdebug.h"
#include "efm_test_consts.h"
#include <ecom/implementationproxy.h>
#include <e32property.h>

// Map the interface implementation UIDs to implementation factory functions
const TImplementationProxy ImplementationTable[] =
    {
    IMPLEMENTATION_PROXY_ENTRY(0x102836F8,	CFeatMgrCorruptPlugin::NewL),
    };

// Exported proxy for instantiation method resolution.
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    return ImplementationTable;
    }

CFeatMgrCorruptPlugin* CFeatMgrCorruptPlugin::NewL()
    {
    CFeatMgrCorruptPlugin* self = new(ELeave) CFeatMgrCorruptPlugin();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

CFeatMgrCorruptPlugin::~CFeatMgrCorruptPlugin()
    {
    }

CFeatMgrCorruptPlugin::CFeatMgrCorruptPlugin()
    {
    }

void CFeatMgrCorruptPlugin::ConstructL()
    {
    }

void CFeatMgrCorruptPlugin::ProcessCommandL( const FeatureInfoCommand::TFeatureInfoCmd aCommandId,
        const TUint8 aTransId,
        TDesC8& /*aData*/ )
	{
	INFO_LOG2( "FeatMgrPlugin: Processing command: 0x%x, TransId: 0x%x", 
				aCommandId, aTransId );
	
	switch ( aCommandId )
		{
		case FeatureInfoCommand::ELoadFeatureInfoCmdId:
			{
			INFO_LOG( "FeatMgr Corrupt Plugin: Processed ELoadFeatureInfoCmdId" );
			// Load feature info
			}
			break;
		case FeatureInfoCommand::ELoadEnhancedFeatureInfoCmdId:
			{
			INFO_LOG( "FeatMgr Corrupt Plugin: Processed ELoadEnhancedFeatureInfoCmdId" );
			// Load feature info
			}
			break;
		default :
			{
			INFO_LOG1( "FeatMgr Corrupt Plugin: Unknown Command: 0x%x", aCommandId );
			User::Leave(KErrNotSupported);
			}
			break;
		}

	TInt retval(KErrNone);
	TInt timeout( 500000 ); // microseconds

	// create new timer
	if ( iTimer && !iTimer->IsActive() )
		{
		delete iTimer;
		iTimer = NULL;
		}

	iTimer = CPluginTimer::NewL(timeout, *iResponseCallback, aCommandId, aTransId, retval,this);
	}

void CFeatMgrCorruptPlugin::GenericTimerFiredL(
	    MFeatureInfoPluginCallback& aService,
	    FeatureInfoCommand::TFeatureInfoCmd aCommandId,
	    TUint8 aTransId,
	    TInt aRetVal )
	    {
	    INFO_LOG2( "FeatMgr Corrupt Plugin: GenericTimerFiredL (0x%x, 0x%x)", 
	                         aCommandId, aTransId );
	    TInt err( KErrNone );
	    
	    switch ( aCommandId )
        {
        case FeatureInfoCommand::ELoadFeatureInfoCmdId:
            {  
            FeatureInfoCommand::TFeatureInfo featureList;
            TInt err( KErrNone );
            RArray<FeatureInfoCommand::TFeature> list;
            featureList.iErrorCode = KErrNone;
            featureList.iList = list;
                
            FeatureInfoCommand::TFeatureInfoRespPckg resPackage(featureList);
            TRAP(err, aService.ProcessResponseL(aCommandId, aTransId, resPackage));    

            break;            
            }          
        case FeatureInfoCommand::ELoadEnhancedFeatureInfoCmdId:
            {
            // Used for the InvalidFlags test case
            const TUid KPropertyCat = {0x1001FAD1};
        	const int KPropertyKey = 1;
        	TInt invalidFlag;
            err = RProperty::Get(KPropertyCat, KPropertyKey, invalidFlag);
            
            if( KErrNotFound == err )
            	{
                _LIT8( KResponse, "somestring" );
                TBufC8<10> Buf ( KResponse );
                TRAP(err, aService.ProcessResponseL(aCommandId, aTransId, Buf));    
            	}
            else if( KErrNone == err )
            	{
            	FeatureInfoCommand::TEnhancedFeatureInfo featureList;
	            RFeatureArray features; 
	            TBitFlags32 flags( invalidFlag );
	            const TInt KDefaultData( 0x00ff00ff );
	            
	            CleanupClosePushL( features );
	            
	            TFeatureEntry entry( TUid::Uid(KDefaultSupportedUid.iUid), flags, KDefaultData );
	            features.AppendL( entry );
	            featureList.iErrorCode = KErrNone;
	            featureList.iList = features;
	
	            FeatureInfoCommand::TEnhancedFeatureInfoRespPckg resPackage(featureList);
	            TRAP(err, aService.ProcessResponseL(aCommandId, aTransId, resPackage));
	            
	            CleanupStack::PopAndDestroy( &features );
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
