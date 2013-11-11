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
// Name          : SdpOriginFieldPtrs.cpp
// Part of       : Local SDP Codec
// Version       : 1.0
//



#include "sdporiginfieldptrs.h"

// -----------------------------------------------------------------------------
// CSdpOriginFieldPtrs::NewL
// -----------------------------------------------------------------------------
//
CSdpOriginFieldPtrs* CSdpOriginFieldPtrs::NewL( TInt64 aSessionId,
        								  		TInt64 aSessionVersion )
	{
	CSdpOriginFieldPtrs* self = new ( ELeave ) CSdpOriginFieldPtrs;
	CleanupStack::PushL( self );
	self->ConstructL( aSessionId, aSessionVersion );
	CleanupStack::Pop( self );
	return self;
	}

// ----------------------------------------------------------------------------
// CSdpOriginFieldPtrs::CSdpOriginFieldPtrs
// ----------------------------------------------------------------------------
//
CSdpOriginFieldPtrs::CSdpOriginFieldPtrs()
	{
	}	

// ----------------------------------------------------------------------------
// CSdpOriginFieldPtrs::ConstructL
// ----------------------------------------------------------------------------
//
void CSdpOriginFieldPtrs::ConstructL( TInt64 aSessionId,
        							  TInt64 aSessionVersion )
	{
	iUserName = KNullDesC8().AllocL();
	SetSessionIdL( aSessionId );
	SetSessionVersionL( aSessionVersion );	
	}

// ----------------------------------------------------------------------------
// CSdpOriginFieldPtrs::~CSdpOriginFieldPtrs
// ----------------------------------------------------------------------------
//
CSdpOriginFieldPtrs::~CSdpOriginFieldPtrs()
	{
	delete iUserName;
	delete iSessionId;
	delete iSessionVersion;
	}

// -----------------------------------------------------------------------------
// CSdpOriginFieldPtrs::SetUserNameL
// -----------------------------------------------------------------------------
// 
void CSdpOriginFieldPtrs::SetUserNameL( const TDesC8& aUserName )
    {
    HBufC8* tmp = aUserName.AllocL();
    delete iUserName;
    iUserName = tmp;
    }
        
// -----------------------------------------------------------------------------
// CSdpOriginFieldPtrs::UserName
// -----------------------------------------------------------------------------
//
const TDesC8& CSdpOriginFieldPtrs::UserName() const
    {
    return *iUserName;
    }

// -----------------------------------------------------------------------------
// CSdpOriginFieldPtrs::SetSessionIdL
// -----------------------------------------------------------------------------
// 
void CSdpOriginFieldPtrs::SetSessionIdL( const TDesC8& aSessionId )
    {
    HBufC8* tmp = aSessionId.AllocL();
    delete iSessionId;
    iSessionId = tmp;
    }

// -----------------------------------------------------------------------------
// CSdpOriginFieldPtrs::SetSessionIdL
// -----------------------------------------------------------------------------
// 
void CSdpOriginFieldPtrs::SetSessionIdL( TInt64 aSessionId )
	{
	TBuf8< KMaxTInt64Digits > number;
    number.AppendNum( aSessionId );
    HBufC8* tmp = number.AllocL();
	delete iSessionId;
    iSessionId = tmp;
	}

// -----------------------------------------------------------------------------
// CSdpOriginFieldPtrs::SessionId
// -----------------------------------------------------------------------------
//
const TDesC8& CSdpOriginFieldPtrs::SessionId() const
    {
    return *iSessionId;
    }

// -----------------------------------------------------------------------------
// CSdpOriginFieldPtrs::SetSessionVersionL
// -----------------------------------------------------------------------------
// 
void CSdpOriginFieldPtrs::SetSessionVersionL( const TDesC8& aSessionVersion )
    {
    HBufC8* tmp = aSessionVersion.AllocL();
    delete iSessionVersion;
    iSessionVersion = tmp;
    }

// -----------------------------------------------------------------------------
// CSdpOriginFieldPtrs::SetSessionVersionL
// -----------------------------------------------------------------------------
// 
void CSdpOriginFieldPtrs::SetSessionVersionL( TInt64 aSessionVersion )
	{
	TBuf8< KMaxTInt64Digits > number;
    number.AppendNum( aSessionVersion );
    HBufC8* tmp = number.AllocL();
	delete iSessionVersion;
    iSessionVersion = tmp;
	}
   
// -----------------------------------------------------------------------------
// CSdpOriginFieldPtrs::SessionVersion
// -----------------------------------------------------------------------------
//
const TDesC8& CSdpOriginFieldPtrs::SessionVersion() const
    {
    return *iSessionVersion;
    }
