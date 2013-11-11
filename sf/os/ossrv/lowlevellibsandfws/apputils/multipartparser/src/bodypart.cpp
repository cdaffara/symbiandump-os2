// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <bafl/bodypart.h>

// CONSTANTS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CBodyPart::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CBodyPart* CBodyPart::NewL()
    {
    CBodyPart* self = new (ELeave) CBodyPart();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(); //self

    return self;
    }

// -----------------------------------------------------------------------------
// C++ default constructor.
// -----------------------------------------------------------------------------
CBodyPart::CBodyPart()
    {   
    iBody.Set( NULL, 0 );
    iBoundary.Set( NULL, 0 );
    iCharset.Set( NULL, 0 );
    iContentBase.Set( NULL, 0 );
    iContentLocation.Set( NULL, 0 );
    iContentTransferEncoding.Set( NULL, 0 );
    iContentType.Set( NULL, 0 );
    iContentID.Set( NULL, 0 );
    iIsDecodedBody = EFalse;
    iUrl = NULL;
    }

// -----------------------------------------------------------------------------
// Symbian 2nd phase constructor
// -----------------------------------------------------------------------------
void CBodyPart::ConstructL() 
    {
    }

// -----------------------------------------------------------------------------
// Destructor.
// -----------------------------------------------------------------------------
EXPORT_C CBodyPart::~CBodyPart()
    {
    if( iIsDecodedBody )
        {
        delete  (TUint8*) iBody.Ptr();
        }

    delete iUrl;
    }

// -----------------------------------------------------------------------------
// return The content-base of this body part
// -----------------------------------------------------------------------------
const TDesC8& CBodyPart::ContentBase()
    {
    return iContentBase;
    }

// -----------------------------------------------------------------------------
// return The content-location of this body part
// -----------------------------------------------------------------------------
const TDesC8& CBodyPart::ContentLocation()
    {
    return iContentLocation;
    }

// -----------------------------------------------------------------------------
// return The content-transfer-encoding of this body part
// -----------------------------------------------------------------------------
const TDesC8& CBodyPart::ContentTransferEncoding()
    {
    return iContentTransferEncoding;
    }

// -----------------------------------------------------------------------------
// return The boundary of this body part
// -----------------------------------------------------------------------------
const TDesC8& CBodyPart::Boundary()
    {
    return iBoundary;
    }

// -----------------------------------------------------------------------------
// return If the body of the body part is decoded or unzipped
// -----------------------------------------------------------------------------
TBool CBodyPart::IsDecodedBody()
    {
    return iIsDecodedBody;
    }

// -----------------------------------------------------------------------------
// Set the boundary of the body part
// -----------------------------------------------------------------------------
void CBodyPart::SetBoundary( const TDesC8& aBoundary )
    {
    iBoundary.Set( aBoundary );
    }

// -----------------------------------------------------------------------------
// Set the charset of the body part
// -----------------------------------------------------------------------------
void CBodyPart::SetCharset( const TDesC8& aCharset )
    {
    iCharset.Set( aCharset );
    }

// -----------------------------------------------------------------------------
// Set the content-base of the body part
// -----------------------------------------------------------------------------
void CBodyPart::SetContentBase( const TDesC8& aContentBase )
    {
    iContentBase.Set( aContentBase );
    }

// -----------------------------------------------------------------------------
// Set the content-location of the body part
// -----------------------------------------------------------------------------
void CBodyPart::SetContentLocation( const TDesC8& aContentLocation )
    {
    iContentLocation.Set( aContentLocation );
    }

// -----------------------------------------------------------------------------
// Set the content-transfer-encoding of the body part
// -----------------------------------------------------------------------------
void CBodyPart::SetContentTransferEncoding( const TDesC8& aContentTransferEncoding )
    {
    iContentTransferEncoding.Set( aContentTransferEncoding );
    }

// -----------------------------------------------------------------------------
// Set the content-type of the body part
// -----------------------------------------------------------------------------
void CBodyPart::SetContentType( const TDesC8& aContentType )
    {
    iContentType.Set( aContentType );
    }

// -----------------------------------------------------------------------------
// Set the content-ID of the body part
// -----------------------------------------------------------------------------
void CBodyPart::SetContentID( const TDesC8& aContentID )
    {
    iContentID.Set( aContentID );
    }

// -----------------------------------------------------------------------------
// Set if the body of the body part is decoded or unzipped
// -----------------------------------------------------------------------------
void CBodyPart::SetIsDecodedBody( TBool aIsDecodedBody )
    {
    iIsDecodedBody = aIsDecodedBody;
    }

// -----------------------------------------------------------------------------
// Set the URL of the body part
// -----------------------------------------------------------------------------
void CBodyPart::SetUrl( HBufC16* aUrl )
    {
    iUrl = aUrl;
    }


