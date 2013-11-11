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
#include <featmgr/featurecmn.h>

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// TFeatureEntry::TFeatureEntry()
// -----------------------------------------------------------------------------
//
EXPORT_C TFeatureEntry::TFeatureEntry() : 
    iFeatureID( TUid::Uid(0) ), iFlags( 0 ), iData( 0 ), iReserved ( 0 )
    {
	}

// -----------------------------------------------------------------------------
// TFeatureEntry::TFeatureEntry()
// -----------------------------------------------------------------------------
//
EXPORT_C TFeatureEntry::TFeatureEntry( TUid aFeature ) : 
    iFeatureID( aFeature ), iFlags( 0 ), iData( 0 ), iReserved ( 0 )
    {
    }
    
// -----------------------------------------------------------------------------
// TFeatureEntry::TFeatureEntry()
// -----------------------------------------------------------------------------
//
EXPORT_C TFeatureEntry::TFeatureEntry( TUid aFeature, TBitFlags32 aFlags, TUint32 aData ) : 
    iFeatureID( aFeature ), iFlags( aFlags ), iData( aData ), iReserved ( 0 )
    {
    }
    
// -----------------------------------------------------------------------------
// TFeatureEntry::FeatureUid()
// -----------------------------------------------------------------------------
//
EXPORT_C TUid TFeatureEntry::FeatureUid() const
    { 
    return iFeatureID;
    }

// -----------------------------------------------------------------------------
// TFeatureEntry::FeatureFlags()
// -----------------------------------------------------------------------------
//
EXPORT_C TBitFlags32 TFeatureEntry::FeatureFlags() const
    {
    return iFlags;
    }
    
// -----------------------------------------------------------------------------
// TFeatureEntry::FeatureData()
// -----------------------------------------------------------------------------
//
EXPORT_C TUint32 TFeatureEntry::FeatureData() const
    {
    return iData;
    }
    
    
//  End of File  
