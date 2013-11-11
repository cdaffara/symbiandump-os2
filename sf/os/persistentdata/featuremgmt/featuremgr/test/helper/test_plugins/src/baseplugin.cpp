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



#include "baseplugin.h"
#include <featmgr/featureinfoplugin.h>
#include "featmgrdebug.h"
#include "efm_test_consts.h"     
#include "plugintimer.h"     


CFeatMgrBasePlugin::CFeatMgrBasePlugin()
    {
    }

CFeatMgrBasePlugin::~CFeatMgrBasePlugin()
    {
    if ( iTimer )
        {
        delete iTimer;
        iTimer = NULL;
        }
    }

void CFeatMgrBasePlugin::ConstructL()
    {
    }
    
void CFeatMgrBasePlugin::ProcessCommandL( const FeatureInfoCommand::TFeatureInfoCmd aCommandId,
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
            }
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

    
void CFeatMgrBasePlugin::GenericTimerFiredL(
    MFeatureInfoPluginCallback& /*aService*/,
    FeatureInfoCommand::TFeatureInfoCmd /*aCommandId*/,
    TUint8 /*aTransId*/,
    TInt /*aRetVal*/ )
    {
    if ( iTimer && !iTimer->IsActive() )
        {
        delete iTimer;
        iTimer = NULL;            
        INFO_LOG( "FeatMgr Plugin: GenericTimerFiredL - Removed obsolete timer" );
        }
    }
