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



// INCLUDE FILES
#include <featmgr/featmgr.h>
#include "featmgrtlsdata.h"
#include <featmgr/featurecontrol.h>
#include "featmgrdebug.h"
#include <e32std.h>

// LOCAL CONSTANTS AND MACROS
_LIT( KPanicCategory, "FeatMgr" );


// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// FeatureManager::InitializeLibL()
// -----------------------------------------------------------------------------
//
EXPORT_C void FeatureManager::InitializeLibL()
    {
	FUNC_LOG

    if ( !Dll::Tls() )
        {
        Dll::SetTls( CFeatMgrTlsData::NewL() );
        }

	// Increase the client count (self)
	TlsData()->IncreaseClientCount();
	}

// -----------------------------------------------------------------------------
// FeatureManager::UnInitializeLibL()
// -----------------------------------------------------------------------------
//
EXPORT_C void FeatureManager::UnInitializeLib()
    {
	FUNC_LOG

    if ( Dll::Tls() )
		{
		CFeatMgrTlsData::DeleteClient();
        }
    }

// -----------------------------------------------------------------------------
// FeatureManager::FeatureSupported()
// -----------------------------------------------------------------------------
//
EXPORT_C TBool FeatureManager::FeatureSupported( TInt aFeature )
    {
    TFeatureEntry feature( TUid::Uid(aFeature) );
    TInt err( TlsData()->FeatureSupported( feature ) );

    return (( err > 0 ) ? ETrue : EFalse );
    }

// -----------------------------------------------------------------------------
// FeatureManager::TlsData()
// -----------------------------------------------------------------------------
//
CFeatMgrTlsData* FeatureManager::TlsData()
    {
    CFeatMgrTlsData* tlsData = STATIC_CAST( CFeatMgrTlsData*, Dll::Tls() );

    __ASSERT_ALWAYS( tlsData, User::Panic( KPanicCategory, ELibNotInitialized ) );

    return tlsData;
    }
    
//  End of File  
