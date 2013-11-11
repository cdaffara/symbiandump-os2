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
// Name          : SdpAttributeFieldPtrs.h
// Part of       : Local SDP Codec
// Version       : 1.0
//



#include "sdpattributefieldptrs.h"
#include "sdpfmtattributefield.h"

// -----------------------------------------------------------------------------
// CSdpAttributeFieldPtrs::NewL
// -----------------------------------------------------------------------------
//
CSdpAttributeFieldPtrs* CSdpAttributeFieldPtrs::NewL()
	{
	CSdpAttributeFieldPtrs* self = new ( ELeave ) CSdpAttributeFieldPtrs;
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}

// ----------------------------------------------------------------------------
// CSdpAttributeFieldPtrs::CSdpAttributeFieldPtrs
// ----------------------------------------------------------------------------
//
CSdpAttributeFieldPtrs::CSdpAttributeFieldPtrs()
	{
	}	

// ----------------------------------------------------------------------------
// CSdpAttributeFieldPtrs::ConstructL
// ----------------------------------------------------------------------------
//
void CSdpAttributeFieldPtrs::ConstructL()
	{
	iValue = KNullDesC8().AllocL();
	}

// ----------------------------------------------------------------------------
// CSdpAttributeFieldPtrs::~CSdpAttributeFieldPtrs
// ----------------------------------------------------------------------------
//
CSdpAttributeFieldPtrs::~CSdpAttributeFieldPtrs()
	{
	delete iValue;
	}

// -----------------------------------------------------------------------------
// CSdpAttributeFieldPtrs::SetValueL
// -----------------------------------------------------------------------------
// 
void CSdpAttributeFieldPtrs::SetValueL( const TDesC8& aValue )
    {
    HBufC8* tmp = aValue.AllocL();
    delete iValue;
    iValue = tmp;
    }
        
// -----------------------------------------------------------------------------
// CSdpAttributeFieldPtrs::Value
// -----------------------------------------------------------------------------
//
const TDesC8& CSdpAttributeFieldPtrs::Value() const
    {
    return *iValue;
    }
	
// -----------------------------------------------------------------------------
// CSdpAttributeFieldPtrs::SetFmtAttribute
// -----------------------------------------------------------------------------
//
void CSdpAttributeFieldPtrs::SetFmtAttribute( 
    const CSdpFmtAttributeField* aFmtAttribute )
    {
    iFmtAttributeField = aFmtAttribute;
    }
	
// -----------------------------------------------------------------------------
// CSdpAttributeFieldPtrs::FmtAttribute
// -----------------------------------------------------------------------------
//
const CSdpFmtAttributeField* CSdpAttributeFieldPtrs::FmtAttribute() const
    {
    return iFmtAttributeField;
    }
