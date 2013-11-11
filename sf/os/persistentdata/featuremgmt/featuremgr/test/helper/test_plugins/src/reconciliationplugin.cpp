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



#include "reconciliationplugin.h"
#include <featmgr/featureinfoplugin.h>
#include "featmgrdebug.h"
#include "efm_test_consts.h"     
#include <ecom/implementationproxy.h>

// Map the interface implementation UIDs to implementation factory functions
const TImplementationProxy ImplementationTable[] =
    {
    IMPLEMENTATION_PROXY_ENTRY(0x102836F4,	CFeatMgrReconciliationPlugin::NewL),
    };

// Exported proxy for instantiation method resolution.
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    return ImplementationTable;
    }

CFeatMgrReconciliationPlugin* CFeatMgrReconciliationPlugin::NewL()
    {
    CFeatMgrReconciliationPlugin* self = new(ELeave) CFeatMgrReconciliationPlugin();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

CFeatMgrReconciliationPlugin::~CFeatMgrReconciliationPlugin()
    {
    if ( iTimer )
        {
        delete iTimer;
        iTimer = NULL;
        }
    }

CFeatMgrReconciliationPlugin::CFeatMgrReconciliationPlugin()
    {
    }

void CFeatMgrReconciliationPlugin::ConstructL()
    {
    }    

void CFeatMgrReconciliationPlugin::ProcessCommandL( const FeatureInfoCommand::TFeatureInfoCmd aCommandId,
        const TUint8 aTransId,
        TDesC8& /*aData*/ )
	{
	INFO_LOG2( "FeatMgrPlugin: Processing command: 0x%x, TransId: 0x%x", 
	aCommandId, aTransId );
	
	switch ( aCommandId )
	{
	case FeatureInfoCommand::ELoadFeatureInfoCmdId:
		{
		INFO_LOG( "FeatMgrPlugin: Processed ELoadFeatureInfoCmdId" );
		// Load feature info
		}
		break;
	case FeatureInfoCommand::ELoadEnhancedFeatureInfoCmdId:
		{
		INFO_LOG( "FeatMgrPlugin: Processed ELoadEnhancedFeatureInfoCmdId" );
		// Load feature info
		}
		break;
	default :
		{
		INFO_LOG1( "FeatMgrPlugin: Unknown Command: 0x%x", aCommandId );
		User::Leave(KErrNotSupported);
		}
		break;
	}
	
	TInt retval(KErrNone);
	TInt timeout( 50 ); // microseconds
	
	// create new timer
	
	if ( iTimer && !iTimer->IsActive() )
		{
		delete iTimer;
		iTimer = NULL;
		}
	
	iTimer = CPluginTimer::NewL(timeout, *iResponseCallback, aCommandId, aTransId, retval,this);
	}

void CFeatMgrReconciliationPlugin::GenericTimerFiredL(
    MFeatureInfoPluginCallback& aService,
    FeatureInfoCommand::TFeatureInfoCmd aCommandId,
    TUint8 aTransId,
    TInt /*aRetVal*/ )
    {
    INFO_LOG2( "FeatMgr Plugin: GenericTimerFiredL (0x%x, 0x%x)", 
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

            feature1.iFeatureID = KDefaultSupportedUid.iUid;
            feature1.iValue = ETrue;
            list.AppendL(feature1);
            feature1.iFeatureID = KBlacklistedUid.iUid;
            feature1.iValue = ETrue;
            list.AppendL(feature1);
			feature1.iFeatureID = KModifiableUid.iUid;
            feature1.iValue = ETrue;
            list.AppendL(feature1);

            
            feature1.iFeatureID = KSupportedUpgradeableUid.iUid;
            feature1.iValue = ETrue;
            list.AppendL(feature1);

            feature1.iFeatureID = KSupportedUpgradeableBlacklistedUid.iUid;
            feature1.iValue = ETrue;
            list.AppendL(feature1);

            feature1.iFeatureID = KSupportedUpgradeableModifiableUid.iUid;
            feature1.iValue = ETrue;
            list.AppendL(feature1);

            feature1.iFeatureID = KSupportedUpgradeableModifiableUninitialisedUid.iUid;
            feature1.iValue = ETrue;
            list.AppendL(feature1);
            
            feature1.iFeatureID = KSupportedUpgradeableModifiablePersistedUid.iUid;
            feature1.iValue = ETrue;
            list.AppendL(feature1);
            
            featureList.iErrorCode = KErrNone;
            featureList.iList = list;
                
            FeatureInfoCommand::TFeatureInfoRespPckg resPackage(featureList);
            
            TRAP(err, aService.ProcessResponseL(aCommandId, aTransId, resPackage));    
            
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
            const TInt KDefaultData( 0x00ff00ff );        
            TFeatureEntry entry( TUid::Uid(KNewUid.iUid), flags, KDefaultData );
            features.AppendL( entry );
            featureList.iErrorCode = KErrNone;
            featureList.iList = features;
                
            FeatureInfoCommand::TEnhancedFeatureInfoRespPckg resPackage(featureList);
            
            TRAP(err, aService.ProcessResponseL(aCommandId, aTransId, resPackage));    
            
            CleanupStack::PopAndDestroy( &features );
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
    
    if ( iTimer && !iTimer->IsActive() )
        {
        delete iTimer;
        iTimer = NULL;            
        INFO_LOG( "FeatMgr Plugin: GenericTimerFiredL - Removed obsolete timer" );
        }
    }

