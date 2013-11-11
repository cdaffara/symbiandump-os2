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



#include "bc_enhancedplugin.h"
#include "corruptplugin.h"
#include <featmgr/featureinfoplugin.h>
#include "featmgrdebug.h"
#include "efm_test_consts.h"
#include <ecom/implementationproxy.h>

// Map the interface implementation UIDs to implementation factory functions
const TImplementationProxy ImplementationTable[] =
    {
    IMPLEMENTATION_PROXY_ENTRY(0x10283700,	CFeatMgrBCEnhancedPlugin::NewL),
    };

// Exported proxy for instantiation method resolution.
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    return ImplementationTable;
    }

CFeatMgrBCEnhancedPlugin* CFeatMgrBCEnhancedPlugin::NewL()
    {
    CFeatMgrBCEnhancedPlugin* self = new(ELeave) CFeatMgrBCEnhancedPlugin();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

CFeatMgrBCEnhancedPlugin::~CFeatMgrBCEnhancedPlugin()
    {
    }

CFeatMgrBCEnhancedPlugin::CFeatMgrBCEnhancedPlugin()
    {
    }

void CFeatMgrBCEnhancedPlugin::ConstructL()
    {
    }

void CFeatMgrBCEnhancedPlugin::ProcessCommandL( const FeatureInfoCommand::TFeatureInfoCmd aCommandId,
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
		User::Leave(KErrNotSupported);
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

void CFeatMgrBCEnhancedPlugin::GenericTimerFiredL(
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
    case FeatureInfoCommand::ELoadEnhancedFeatureInfoCmdId:
        {
        // Return some features and KErrNone.
        FeatureInfoCommand::TEnhancedFeatureInfo featureList;
        RFeatureArray features; 
        CleanupClosePushL( features );
        TBitFlags32 flags( 0 );
        flags.Set( EFeatureSupported );
        flags.Set( EFeatureModifiable );
        
        // Adding feature B
        const TInt KBDefaultData( 0x000000bb );
        TFeatureEntry b_entry( TUid::Uid(KFeatureBUid.iUid), flags, KBDefaultData );
        features.AppendL( b_entry );
        
        // Adding feature C
        const TInt KCDefaultData( 0x000000cc );
        TFeatureEntry c_entry( TUid::Uid(KFeatureCUid.iUid), flags, KCDefaultData );
        features.AppendL( c_entry );	

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
    CFeatMgrBasePlugin::GenericTimerFiredL(aService,aCommandId,aTransId,aRetVal);
    
	}
