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
#include <s32file.h>
#include "featmgrfeatureentry.h"

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// TFeatureServerEntry::TFeatureServerEntry()
// -----------------------------------------------------------------------------
//
TFeatureServerEntry::TFeatureServerEntry() : 
    iFeatureID( TUid::Uid(0) ), iFlags( 0 ), iData( 0 )
    {
	}

// -----------------------------------------------------------------------------
// TFeatureServerEntry::TFeatureServerEntry()
// -----------------------------------------------------------------------------
//
TFeatureServerEntry::TFeatureServerEntry( TUid aFeature ) : 
    iFeatureID( aFeature ), iFlags( 0 ), iData( 0 )
    {
    }
    
// -----------------------------------------------------------------------------
// TFeatureServerEntry::TFeatureServerEntry()
// -----------------------------------------------------------------------------
//
TFeatureServerEntry::TFeatureServerEntry( TUid aFeature, 
    TBitFlags32 aFlags, TUint32 aData ) : 
    iFeatureID( aFeature ), iFlags( aFlags ), iData( aData )
    {
    }
    
// -----------------------------------------------------------------------------
// TFeatureServerEntry::TFeatureServerEntry()
// -----------------------------------------------------------------------------
//
TFeatureServerEntry::TFeatureServerEntry( TFeatureEntry& aFeature ) : 
    iFeatureID( aFeature.FeatureUid() ), 
    iFlags( aFeature.FeatureFlags() ), 
    iData( aFeature.FeatureData() )
    {
    }
    
// -----------------------------------------------------------------------------
// TFeatureServerEntry::FeatureUid()
// -----------------------------------------------------------------------------
//
TUid TFeatureServerEntry::FeatureUid() const 
    {
    return iFeatureID;
    }

// -----------------------------------------------------------------------------
// TFeatureServerEntry::FeatureFlags()
// -----------------------------------------------------------------------------
//
TBitFlags32 TFeatureServerEntry::FeatureFlags() const
    {
    return iFlags;
    }
    
// -----------------------------------------------------------------------------
// TFeatureServerEntry::FeatureData()
// -----------------------------------------------------------------------------
//
TUint32 TFeatureServerEntry::FeatureData() const
    {
    return iData;
    }
    
// -----------------------------------------------------------------------------
// TFeatureServerEntry::InternalizeL()
// -----------------------------------------------------------------------------
//
void TFeatureServerEntry::InternalizeL( RReadStream& aStream )
    {
    iFeatureID = TUid::Uid( aStream.ReadUint32L() );
    iFlags = aStream.ReadUint32L();
    iData = aStream.ReadUint32L();
    }
    
// -----------------------------------------------------------------------------
// TFeatureServerEntry::ExternalizeL()
// -----------------------------------------------------------------------------
//
void TFeatureServerEntry::ExternalizeL( RWriteStream& aStream ) const
    {
    aStream.WriteUint32L( iFeatureID.iUid );
    aStream.WriteUint32L( iFlags.iFlags );
    aStream.WriteUint32L( iData );
    }
    
//  End of File  
