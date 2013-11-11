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



#include "invalidplugin.h"
#include <featmgr/featureinfoplugin.h>
#include "featmgrdebug.h"
#include "efm_test_consts.h"
#include <ecom/implementationproxy.h>
#include <e32property.h>

// Map the interface implementation UIDs to implementation factory functions
const TImplementationProxy ImplementationTable[] =
    {
    IMPLEMENTATION_PROXY_ENTRY(0x10283708,	CFeatMgrInvalidPlugin::NewL),
    };

// Exported proxy for instantiation method resolution.
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    return ImplementationTable;
    }

CFeatMgrInvalidPlugin* CFeatMgrInvalidPlugin::NewL()
    {
    CFeatMgrInvalidPlugin* self = new(ELeave) CFeatMgrInvalidPlugin();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

CFeatMgrInvalidPlugin::~CFeatMgrInvalidPlugin()
    {
    }

CFeatMgrInvalidPlugin::CFeatMgrInvalidPlugin()
    {
    }

void CFeatMgrInvalidPlugin::ProcessCommandL( const FeatureInfoCommand::TFeatureInfoCmd aCommandId,
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
			User::Leave(KErrNotSupported);
			}
			break;
		default:
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


void CFeatMgrInvalidPlugin::ConstructL()
    {
    }

void CFeatMgrInvalidPlugin::GenericTimerFiredL(
	    MFeatureInfoPluginCallback& /*aService*/,
	    FeatureInfoCommand::TFeatureInfoCmd /*aCommandId*/,
	    TUint8 /*aTransId*/,
	    TInt /*aRetVal*/)
	    {
	    }
