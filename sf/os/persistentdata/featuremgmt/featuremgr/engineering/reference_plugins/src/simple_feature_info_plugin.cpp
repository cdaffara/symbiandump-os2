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



#include "simple_feature_info_plugin.h"

//header file declaring ECOM structures needed to resolve interface implementations
#include <implementationproxy.h>

// Map the interface implementation UIDs to implementation factory functions
const TImplementationProxy ImplementationTable[] =
    {
    IMPLEMENTATION_PROXY_ENTRY(0x10283702,	CSimpleFeatureInfoPlugin::NewL),
    };

// Exported proxy for instantiation method resolution.
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    return ImplementationTable;
    }

//factory function for our plugin
CSimpleFeatureInfoPlugin* CSimpleFeatureInfoPlugin::NewL()
    {
    CSimpleFeatureInfoPlugin* self = new(ELeave)CSimpleFeatureInfoPlugin();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

CSimpleFeatureInfoPlugin::CSimpleFeatureInfoPlugin()
    {
    //perform any basic initialisation here
    }

void CSimpleFeatureInfoPlugin::ConstructL()
    {
    //perform any initialisation that may cause our code to leave
	}

CSimpleFeatureInfoPlugin::~CSimpleFeatureInfoPlugin()
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
void CSimpleFeatureInfoPlugin::ProcessCommandL( const FeatureInfoCommand::TFeatureInfoCmd aCommandId,
        const TUint8 aTransId,
        TDesC8& /*aData*/ )
{
	if (aCommandId != FeatureInfoCommand::ELoadFeatureInfoCmdId) //we only support FeatureInfoCommand::ELoadFeatureInfoCmdId command
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
void CSimpleFeatureInfoPlugin::GenericTimerFiredL()
	    {
	    // Return some features and set error code to KErrNone.
	    FeatureInfoCommand::TFeatureInfo featureList;

	    RArray<FeatureInfoCommand::TFeature> list;
	    CleanupClosePushL( list );
	    FeatureInfoCommand::TFeature feature1; 

	    const TInt KImaginaryId( 0x18273644 );
	    feature1.iFeatureID = KImaginaryId;
	    feature1.iValue = ETrue;
	    list.AppendL(feature1);
	            
	    featureList.iErrorCode = KErrNone;
	    featureList.iList = list;
	    
	    //pack the response into package descriptor
	    FeatureInfoCommand::TFeatureInfoRespPckg resPackage(featureList);
	    
	    //send response to the server via client-server framework
	    TRAPD(err, iResponseCallback->ProcessResponseL(iCommandId, iTransId, resPackage));    
	    
	    CleanupStack::PopAndDestroy( &list );
	 	    
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
