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
// Implementation of the Multipart Parser
//



/**
 @file
 @publishedPartner
 @prototype
*/
#ifndef MULTIPARTPARSER_H
#define MULTIPARTPARSER_H

#include <bafl/bodypart.h>

class CBodyPart;

/**
Provides a RFC2045 MIME multipart parser and composer.  Each body part of a multipart
message is represented by CBodyPart.
*/
NONSHARABLE_CLASS(MultipartParser)
    {
    public:
		/**
		Controls which top level headers are used when composing a multipart message.
		*/
        enum TMultipartTopLevelHeader
            {
			/** Content-type: */
            EMultipartTopLevelHeaderContentType = 0x0001,
			/** Content-length: */
            EMultipartTopLevelHeaderContentLength = 0x0002,
			/** Last-modified: */
            EMultipartTopLevelHeaderLastModified = 0x0004,
			/** All top level headers */
            EMultipartTopLevelHeaderAll = 0x0007	// make SURE to change this when adding new header types
            };
        
		/**
		Identifies the MIME multipart subtype
		*/
        enum TMultipartSubtype
            {
			/** multipart/mixed */
        	EMultipartSubtypeMixed = 0
            };

    public:
    
        /** 
		Parse a multipart message
        @param aMultipartBody   The multipart file to be parsed
        @param aContentType     The content type of multipart file: mixed, related etc.
        @param aBoundary        The boundary of the multipart file
        @param aUrl             The url of the multipart file
        @param aBodyPartsArray  The array contains parsed body parts of the multipart file
        @param aMaxToParse      The max number of body parts to be parsed
		@post aBodyPartsArray contains the parsed body parts
		@leave KErrNotSupported aContentType is not multipart/mixed or multipart/related
		@leave - One of the system-wide error codes
        */
        IMPORT_C static void ParseL( const TDesC8& aMultipartBody, 
                                     const TDesC8& aContentType,
                                     const TDesC8& aBoundary,
                                     const TDesC16& aUrl,
                                     RPointerArray <CBodyPart>& aBodyPartsArray,
                                     TInt aMaxToParse = -1 );

        /** 
		Composes a multipart document
        @param aBodyArray      Array of BodyPart objects to be combined into a multipart message
        @param aBoundary       A string containing the boundary to be used in construction of multipart document
        @param aSubtype        Enumerated value of multipart subtype
        @param aHeaderMask     Integer mask of TTopLevelHeaders to indicate which top-level headers should be included
        @return multipart document; the ownership of buffer is transferred to caller.
		@leave - One of the system-wide error codes
        */
        IMPORT_C static HBufC8* ComposeL( RPointerArray<CBodyPart>& aBodyPartsArray,
                                          const TDesC8& aBoundary,
                                          TMultipartSubtype aSubtype,
                                          TInt aHeaderMask );
                                 
    private:
		/** Default constructor */
		MultipartParser();

        /** 
		Get the buffer of the next body part
        @param aStartPosition   The starting position to parse
        param aMultipartBody   The full buffer of multipart file
        @param aMultipartLen    The length of the multipart file buffer
        @param aBoundary        The boundary of the multipart file
        @param aSingleEolChar   The single EOL of the multipart file
        @param aBodyPartBuffer  The buffer of this body part
        @return Length of aBodyPartBuffer
        */
        static TUint32 GetNextBodyPartBuffer( TUint32 startPosition, 
                                       const TUint8* aMultipartBody,
                                       TUint32 aMultipartLen,
                                       const TDesC8& aBoundary,
                                       char* aSingleEolChar,
                                       TUint8** aBodyPartBuffer );

        /** 
		Set the single and double EOL of the multipart file
        @param aMultipartBody   The full buffer of multipart file
        @param aMultipartLen    The length of the multipart file buffer
        @param aBoundary        The boundary of the multipart file
        @param aSingleEolChar   The single EOL of the multipart file
        @param aDoubleEolChar   The double EOL of the multipart file
        */
        static void SetEolCharacters( const TUint8* aMultipartBody,
                               TUint32 aMultipartLen,
                               const TDesC8& aBoundary,
                               char** aSingleEolChar,
                               char** aDoubleEolChar );

        /** 
		Parse the body part
        @param aBodyPartBuffer        The buffer of this body part
        @param aBodyPartBufferLength  The length of this body part buffer
        @param aSingleEolChar         The single EOL of the multipart file
        @param aDoubleEolChar         The double EOL of the multipart file
        @param aResponseUrl           The url requested for the multipart file
        @param aBodyPart              The body part parsed and returned
		@leave - One of the system-wide error codes
        */
        static void ParseBodyPartL( TUint8* aBodyPartBuffer,
                             TUint32 aBodyPartBufferLength,
                             char* aSingleEolChar,
                             char* aDoubleEolChar,
                             const TDesC16& aResponseUrl,
                             CBodyPart* aBodyPart );

        /** 
		Checks if a Content-Transfer-Encoding is specified
        @param aContentTransferEncodingValue  The transfer encoding of this body part
        @return true if contentTransferEncodingValue is neither NULL nor a domain.
        */
        static TBool IsEncoded( TUint8* aContentTransferEncodingValue );

        /** 
		Decode text given the Content-Transfer-Encoding
        @param aContentTransferEncodingValue  The transfer encoding of this body part
        @param aEncodedBody                   The encoded body of this body part
        @param aDecodedBody                   The decoded body returned
        @return KErrNone if successful, otherwise one of the system wide error codes.
        */
        static TInt DecodeContentTransferEncoding( TUint8* aContentTransferEncodingValue,
                                            const TDesC8& aEncodedBody,
                                            TPtr8& aDecodedBody );

        /** 
		Checks if the Content-Encoding-Type is application/x-gzip
        @param aContentTypeValue  The content type of this body part
        @return ETrue if the Content-Encoding-Type is application/x-gzip.
        */
        static TBool IsZipped( TUint8* aContentTypeValue );

        /** 
		Unzip the body
        @param aContentTypeValue     The content type of this body part
        @param aZippedBody           The zipped body of this body part
        @param aUnzippedBody         The unzipped body returned
        @return KErrNone if successful, otherwise one of the system wide error codes.
		@pre aZippedBody has a Content-Encoding-Type of application/x-gzip
		@pre aContentTypeValue is application/x-gzip
        */
        static TInt Unzip( TUint8* aContentType,
                           const TDesC8& aZippedBody,
                           TPtr8& aUnzippedBody );

        /** 
		Remove the charset value from the Content-Type header
        @param aBodyPart              The body part which contains the content type
        */
        static void CutOffContentTypeAttributes( CBodyPart* aBodyPart );

        /** 
		Forms a URL that refers to this body part
        @param aContentBaseValue      The content base of this body part
        @param aContentLocationValue  The content location of this body part
        @param aResponseUrl           The url requested for the multipart file
        @return URL
		@leave - One of the system-wide error codes
        */
        static HBufC16* GetBodyPartUrlL( const TDesC8& aContentBaseValue,
                                 const TDesC8& aContentLocationValue,
                                 const TDesC16& aResponseUrl );

        /** 
		Checks whether a URL is relative or not
        @param aUrl The URL to check 
        @return ETrue if the URL is relative
		@leave - One of the system-wide error codes
        */
        static TBool IsUrlRelativeL( const TDesC8& aUrl );

        /** 
		Create an absolute URL from a relative URL
        @param aBase         The base URL
        @param aRelativeUrl  The relative URL
        @return absolute URL
		@leave - One of the system-wide error codes
        */
        static HBufC16* UrlRelToAbsL( TDesC16& aBase, 
                                      const TDesC8& aRelativeUrl );

        /** 
		Composes multipart/mixed document
        @param aBodyArray      Array of CBodyPart objects to be included in the output
        @param aBoundary       A string containing boundary to be used in construction of multipart document
        @param aHeaderMask     Integer mask of TMultipartTopLevelHeader to indicate which top-level headers should be included
        @return multipart document; the ownership of buffer is transferred to caller.
		@leave - One of the system-wide error codes
        */
        static HBufC8* ComposeMixedL( RPointerArray<CBodyPart>& aBodyArray,
                                      const TDesC8& aBoundary,
                                      TInt aHeaderMask );
                                          
    };

#endif      // MULTIPARTPARSER_H   

