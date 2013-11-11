// Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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



// INCLUDE FILES
#include <featmgr/featurecontrol.h>
#include "featmgrtlsdata.h"
#include "featmgrdebug.h"
#include <e32std.h>

// LOCAL CONSTANTS AND MACROS
_LIT( KPanicCategory, "RFeatureControl" );

// ============================= LOCAL FUNCTIONS ===============================

CFeatMgrTlsData* TlsData( )
    {
    CFeatMgrTlsData* tlsData = STATIC_CAST( CFeatMgrTlsData*, Dll::Tls() );

    __ASSERT_ALWAYS( tlsData, User::Panic( KPanicCategory, EPanicBadHandle ) );

    return tlsData;
    }
    

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// RFeatureControl::RFeatureControl
// -----------------------------------------------------------------------------
//
EXPORT_C RFeatureControl::RFeatureControl() :
    iInitialized( EFalse ), iReserved1( 0 ), iReserved2 ( 0 )
    {
    FUNC_LOG
    }

// -----------------------------------------------------------------------------
// RFeatureControl::Connect()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RFeatureControl::Connect()
    {
	FUNC_LOG

    return Open();
	}

// -----------------------------------------------------------------------------
// RFeatureControl::Open()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RFeatureControl::Open()
    {
	FUNC_LOG

    if ( !Dll::Tls() )
        {
        CFeatMgrTlsData* tlsData = NULL;

        TRAPD( err,  tlsData = CFeatMgrTlsData::NewL() );
        
        if ( err == KErrNone )
            {
            err = Dll::SetTls( tlsData );
            if (err != KErrNone)
                {
                delete tlsData;
                ERROR_LOG1( "RFeatureControl::Open SetTls error %d ", err );
				return err;
                }
            }
        else
            {
            ERROR_LOG1( "RFeatureControl::Open error %d ", err );
            return err;
            }
        }

	// Increase the client count (self)
	iInitialized = ETrue;
	TlsData()->IncreaseClientCount();
    return KErrNone;
	}

// -----------------------------------------------------------------------------
// RFeatureControl::Close()
// -----------------------------------------------------------------------------
//
EXPORT_C void RFeatureControl::Close()
    {
	FUNC_LOG

    if ( Dll::Tls() && iInitialized )
		{
		CFeatMgrTlsData::DeleteClient();
        }
    }

// -----------------------------------------------------------------------------
// RFeatureControl::FeatureSupported(TUid)
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RFeatureControl::FeatureSupported( TUid aFeature )
    {
    TFeatureEntry feature( aFeature );
    TInt retVal = TlsData()->FeatureSupported( feature );

    return retVal;
    }

// -----------------------------------------------------------------------------
// RFeatureControl::FeatureSupported(TFeatureEntry&)
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RFeatureControl::FeatureSupported( TFeatureEntry& aFeature )
    {
    return TlsData()->FeatureSupported( aFeature );
    }

// -----------------------------------------------------------------------------
// RFeatureControl::FeaturesSupported(RFeatureArray&)
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RFeatureControl::FeaturesSupported( RFeatureArray& aFeatures )
    {
    TInt err( KErrNone );
    
    if( !aFeatures.Count() )
        {
        err = KErrArgument;
        }
    else
        {
        err = TlsData()->FeaturesSupported( aFeatures );
        }
        
    return err;
    }

// -----------------------------------------------------------------------------
// RFeatureControl::EnableFeature(TUid)
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RFeatureControl::EnableFeature( TUid aFeature )
    {
    return TlsData()->EnableFeature( aFeature );
    }
    
// -----------------------------------------------------------------------------
// RFeatureControl::DisableFeature(TUid)
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RFeatureControl::DisableFeature( TUid aFeature )
    {
    return TlsData()->DisableFeature( aFeature );
    }
    
// -----------------------------------------------------------------------------
// RFeatureControl::SetFeature(TUid, TBool, TInt)
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RFeatureControl::SetFeature( TUid aFeature, TBool aEnabled, TUint32 aData )
    {
    return TlsData()->SetFeature( aFeature, aEnabled, aData );
    }
    
// -----------------------------------------------------------------------------
// RFeatureControl::SetFeature()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RFeatureControl::SetFeature( TUid aFeature, TUint32 aData )
    {
    return TlsData()->SetFeature( aFeature, aData );
    }
    
// -----------------------------------------------------------------------------
// RFeatureControl::AddFeature()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RFeatureControl::AddFeature( TFeatureEntry& aFeature )
    {
    return TlsData()->AddFeature( aFeature );
    }
    
// -----------------------------------------------------------------------------
// RFeatureControl::DeleteFeature()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RFeatureControl::DeleteFeature( TUid aFeature )
    {
    return TlsData()->DeleteFeature( aFeature );
    }
    
// -----------------------------------------------------------------------------
// RFeatureControl::ListSupportedFeaturesL()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RFeatureControl::ListSupportedFeatures( RFeatureUidArray& aSupportedFeatures )
    {
    TRAPD( err, TlsData()->ListSupportedFeaturesL( aSupportedFeatures ) );
    return err;
    }

// -----------------------------------------------------------------------------
// RFeatureControl::SWIStart()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RFeatureControl::SWIStart()
    {
    return TlsData()->SWIStart( );
    }

// -----------------------------------------------------------------------------
// RFeatureControl::SWIEnd()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RFeatureControl::SWIEnd()
    {
    return TlsData()->SWIEnd( ); 
    }


//  End of File  
