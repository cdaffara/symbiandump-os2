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



#include "ab_normalplugin.h"
#include <featmgr/featureinfoplugin.h>
#include "featmgrdebug.h"
#include "efm_test_consts.h"
#include <ecom/implementationproxy.h>

// Map the interface implementation UIDs to implementation factory functions
const TImplementationProxy ImplementationTable[] =
    {
    IMPLEMENTATION_PROXY_ENTRY(0x102836FE,	CFeatMgrABNormalPlugin::NewL),
    };

// Exported proxy for instantiation method resolution.
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    return ImplementationTable;
    }

CFeatMgrABNormalPlugin* CFeatMgrABNormalPlugin::NewL()
    {
    CFeatMgrABNormalPlugin* self = new(ELeave) CFeatMgrABNormalPlugin();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

CFeatMgrABNormalPlugin::~CFeatMgrABNormalPlugin()
    {
    }

CFeatMgrABNormalPlugin::CFeatMgrABNormalPlugin()
    {
    }

void CFeatMgrABNormalPlugin::ConstructL()
    {
    }
    
void CFeatMgrABNormalPlugin::GenericTimerFiredL(
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
        // Return some features and KErrNone.
        FeatureInfoCommand::TFeatureInfo featureList;
        TInt err( KErrNone );
        RArray<FeatureInfoCommand::TFeature> list;
        CleanupClosePushL( list );
        FeatureInfoCommand::TFeature feature; 

        // Adding support to feature A
        feature.iFeatureID = KFeatureAUid.iUid;
        feature.iValue = ETrue;
        list.AppendL(feature);
        // Adding support to feature B
        feature.iFeatureID = KFeatureBUid.iUid;
        feature.iValue = ETrue;
        list.AppendL(feature);
        
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
    CFeatMgrBasePlugin::GenericTimerFiredL(aService,aCommandId,aTransId,aRetVal);
    	    
    }
