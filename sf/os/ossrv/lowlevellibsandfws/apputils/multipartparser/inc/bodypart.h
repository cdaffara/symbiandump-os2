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
// Implementation of the Multipart Body Part
//



/**
 @file
 @publishedPartner
 @prototype
*/

#ifndef BODYPART_H
#define BODYPART_H

#include <e32std.h>
#include <e32base.h>

/**
Implementation of the MIME Multipart Body Part
@see MultipartParser
*/
NONSHARABLE_CLASS(CBodyPart) : public CBase
    {
    public:
        
        /** Allocates and constructs a new CBodyPart object */
        IMPORT_C static CBodyPart* NewL();
        
        /** Destructor */
        IMPORT_C virtual ~CBodyPart();

        /** Gets the Body
        @return The body of this body part
        */
	    inline const TDesC8& Body() { return iBody; }

        /** Gets the Content-Type
        @return The Content-Type of this body part
        */
	    inline const TDesC8& ContentType() { return iContentType; }

        /** Gets the charset
        @return The charset of this body part
        */
	    inline const TDesC8& Charset() { return iCharset; }	

        /** Gets the Content ID
        @return The content ID of this body part
        */
	    inline const TDesC8& ContentID() { return iContentID; }

        /** Gets the URL
        @return The URL of this body part
        */
	    inline const TDesC16& Url() { return *iUrl; }

        /** Gets the headers
        @return The headers of this body part
        */
	    inline const TDesC8& Headers() { return iHeaders; }

        /** Get the Boundary
        @return The boundary of this body part
        */
        const TDesC8& Boundary();

        /** Gets the Content-Base
        @return The Content-Base of this body part
        */
        const TDesC8& ContentBase();

        /** Gets the Content-Location
        @return The Content-Location of this body part
        */
        const TDesC8& ContentLocation();

        /** Gets the Content-Transfer-Encoding
        @return The Content-Transfer-Encoding of this body part
        */
        const TDesC8& ContentTransferEncoding();

        /** Check if the body is decoded or unzipped
        @return If the body of the body part is decoded or unzipped
        */
        TBool IsDecodedBody();

        /** Sets the Body
        @param aBody   The body to set
        */
        inline void SetBody( const TDesC8& aBody ) { iBody.Set( aBody ); }

        /** Sets the Boundary
        @param aBoundary   The boundary to set
        */
        void SetBoundary( const TDesC8& aBoundary );

        /** Sets the charset
        @param aCharset   The charset to set
        */
        void SetCharset( const TDesC8& aCharset );

        /** Sets the Content-Base
        @param aContentBase   The Content-Base to set
        */
        void SetContentBase( const TDesC8& aContentBase );

        /** Sets the Content-Location
        @param aContentLocation   The Content-Location to set
        */
        void SetContentLocation( const TDesC8& aContentLocation );

        /** Sets the Content-Transfer-Encoding
        @param aContentTransferEncoding   The Content-Transfer-Encoding to set
        */
        void SetContentTransferEncoding( const TDesC8& aContentTransferEncoding );

        /** Set the Content-Type
        @param aContentType   The Content-Type to set
        */
        void SetContentType( const TDesC8& aContentType );

        /** Sets the Content ID
        @param aContentID   The content-ID to set
        */
        void SetContentID( const TDesC8& aContentID );

        /** Sets the headers
        @param aHeaders   The headers to set
        */
        inline void SetHeaders( const TDesC8& aHeaders ) { iHeaders.Set( aHeaders ); }

        /** Sets whether the body part is decoded or unzipped
        @param aIsDecodedBody   If the body of the body part is decoded or unzipped
        */
        void SetIsDecodedBody( TBool aIsDecodedBody );

        /** Sets the URL
        * @param aUrl   The URL to set
        */
        void SetUrl( HBufC16* aUrl );

    private:

        /** Default constructor */
        CBodyPart();

        /** 2nd phase constructor */
        void ConstructL();


    private:
        TPtrC8   iBody;
        TPtrC8   iBoundary;
        TPtrC8   iCharset;
        TPtrC8   iContentBase;
        TPtrC8   iContentLocation;
        TPtrC8   iContentTransferEncoding;
        TPtrC8   iContentType;
        TPtrC8   iContentID;
        TPtrC8   iHeaders;
        TBool    iIsDecodedBody;  // if ETrue, this class is responsible for freeing the memory of iBody
        HBufC16* iUrl;
    };

#endif      // BODYPART_H   
 
