// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Name          : SdpCodecStringPool.cpp
// Part of       : Local SDP Codec
// Version       : 1.0
//



#include <stringpool.h>
#include "sdpcodecerr.h"
#include "sdpcodecstringpool.h"
#include "csdpcodecstringpool.h"
#include "sdpcodecstringconstants.h"
#include "_sdpdefs.h"


// -----------------------------------------------------------------------------
// SdpCodecStringPool::OpenL
// Opens stringpool owner, leaves if one already exists in TLS
// -----------------------------------------------------------------------------
//
EXPORT_C void SdpCodecStringPool::OpenL()
	{
	__ASSERT_ALWAYS( Dll::Tls() == NULL, User::Leave ( KErrAlreadyExists ) );

	CSdpCodecStringPool* codecStringPool = CSdpCodecStringPool::NewLC();
	User::LeaveIfError( Dll::SetTls( codecStringPool ) );
	CleanupStack::Pop();    //  codecStringPool
	}

// -----------------------------------------------------------------------------
// SdpCodecStringPool::Close
// Closes & deletes string pool
// -----------------------------------------------------------------------------
//
EXPORT_C void SdpCodecStringPool::Close()
	{
	if ( Dll::Tls() == NULL )
        {
        return;
        }

	CSdpCodecStringPool* codecStringPool =
        static_cast<CSdpCodecStringPool*>( Dll::Tls() );
    delete codecStringPool;
	Dll::SetTls( NULL );
	}

// -----------------------------------------------------------------------------
// SdpCodecStringPool::StringPoolL
// Returns string pool
// -----------------------------------------------------------------------------
//
EXPORT_C RStringPool SdpCodecStringPool::StringPoolL()
    {
    CSdpCodecStringPool* codecStringPool =
        static_cast<CSdpCodecStringPool*>( Dll::Tls() );
    __ASSERT_ALWAYS( codecStringPool != NULL,
                     User::Leave( KErrSdpCodecStringPool ) );

	return codecStringPool->StringPool();
    }

// -----------------------------------------------------------------------------
// SdpCodecStringPool::StringTableL
// Returns string table
// -----------------------------------------------------------------------------
//
EXPORT_C const TStringTable& SdpCodecStringPool::StringTableL()
    {
     CSdpCodecStringPool* codecStringPool =
        static_cast<CSdpCodecStringPool*>( Dll::Tls() );
    __ASSERT_ALWAYS( codecStringPool != NULL,
                     User::Leave( KErrSdpCodecStringPool ) );

	return codecStringPool->StringTable();
    }
