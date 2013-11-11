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
#include <e32def.h>           // First to avoid NULL redefine warning (no #ifndef NULL).
#include <e32std.h>
#include <string.h>
#include <ezdecompressor.h>   // Ezlib.lib, GZip decoders
#include <uri8.h>
#include <uri16.h>
#include <uricommon.h>

#include <bafl/multipartparser.h>
#include "gzipbufmgr.h"
#include "tinternetdate.h"

#include <bafl/qpcodec.h>
#include <bsul/clientmessage.h>

// CONSTANTS
const char Multipart_Mixed[] = {"multipart/mixed"};
const char Multipart_Related[] = {"multipart/related"};
const char Multipart_Boundary_Text[] = {"boundary="};
const char Multipart_Content_Base_Text[] = {"Content-Base:"};
const char Multipart_Content_Location_Text[] = {"Content-Location:"};
const char Multipart_Content_Type_Text[] = {"Content-Type:"};
const char Multipart_Content_Transfer_Encoding_Text[] = {"Content-Transfer-Encoding:"};
// violate RFC2045; but upon customer request
const char Multipart_Content_Encoding_Text[] = {"Content-Encoding:"};
const char Multipart_Content_ID_Text[] = {"Content-ID:"};
const char Multipart_Hypens_Text[] = {"--"};
const char Multipart_CRLF_Text[] = {"\r\n"};
const char Multipart_LF_Text[] = {"\n"};
const char Multipart_DoubleCRLF_Text[] = {"\r\n\r\n"};
const char Multipart_DoubleLF_Text[] = {"\n\n"};
const char Multipart_Charset_Text[] = {"charset="};
const char Multipart_ContentTypeString_Delimiter_Text[] = {";"};
const char Multipart_ContentTypeString_Quotes_Text[] = {"\""};
const char Multipart_Content_Transfer_Encoding_Base64[] = {"base64"};
const char Multipart_Content_Transfer_Encoding_QuotedPrintable[] = {"quoted-printable"};
const char Multipart_Content_Transfer_Encoding_7bit[] = {"7bit"};
const char Multipart_Content_Transfer_Encoding_8bit[] = {"8bit"};
const char Multipart_Content_Transfer_Encoding_binary[] = {"binary"};
const char Multipart_Content_Encoding_GZip[] = {"gzip"};
const char Multipart_Content_Type_GZip[] = {"application/x-gzip"};

// MACROS
#define MULTIPART_CONTENT_BASE_LENGTH               13
#define MULTIPART_CONTENT_LOCATION_LENGTH           17
#define MULTIPART_CONTENT_TYPE_LENGTH               13
#define MULTIPART_CONTENT_TRANSFER_ENCODING_LENGTH  26
// violates RFC2045; but upon vodafone request
#define MULTIPART_CONTENT_ENCODING_LENGTH           17
#define MULTIPART_CONTENT_LENGTH_LENGTH             15
#define MULTIPART_LAST_MODIFIED_LENGTH              14
#define MULTIPART_CONTENT_ID_LENGTH                 11

#define MULTIPART_CONTENT_BASE               1
#define MULTIPART_CONTENT_LOCATION           2
#define MULTIPART_CONTENT_TRANSFER_ENCODING  3
#define MULTIPART_CONTENT_TYPE               4
#define MULTIPART_CONTENT_ID                 5

#define MULTIPART_INTERNET_DATE_STRING_LENGTH       29

#define SLASH_CHAR    '/'
#define DOT_CHAR      '.'
#define AT_CHAR       '@'
#define COLON_CHAR    ':'
// <scheme>://
#define SCHEME_SEPARATOR_LENGTH               3


// ============================= LOCAL FUNCTIONS ===============================


// ============================ MEMBER FUNCTIONS ===============================


// ------------------------------------------------------------------------- 
// Parse and put each body part to the body part array
// ------------------------------------------------------------------------- 
EXPORT_C void MultipartParser::ParseL( const TDesC8& aMultipartBody, 
                                       const TDesC8& aContentType,
                                       const TDesC8& aBoundary,
                                       const TDesC16& aUrl,
                                       RPointerArray <CBodyPart>& aBodyPartsArray,
                                       TInt aMaxToParse )
    {
    // check on required parameters
    __ASSERT_ALWAYS( aMultipartBody.Ptr() != NULL,
                     User::Panic(_L("MultipartParser Panic"), KErrArgument) );
    __ASSERT_ALWAYS( aContentType.Ptr() != NULL,
                     User::Panic(_L("MultipartParser Panic"), KErrArgument) );
    __ASSERT_ALWAYS( aBoundary.Ptr() != NULL,
                     User::Panic(_L("MultipartParser Panic"), KErrArgument) );
    __ASSERT_ALWAYS( aUrl.Ptr() != NULL,
                     User::Panic(_L("MultipartParser Panic"), KErrArgument) );

    const TUint8* multipartBuffer = aMultipartBody.Ptr();
    TUint32 multipartLen = aMultipartBody.Length();
    __ASSERT_ALWAYS( multipartLen != 0,
                     User::Panic(_L("MultipartParser Panic"), KErrArgument) );
    TUint8* bodyPartBuffer = NULL;
    TUint32 bodyPartBufferLength = 0;
    TUint32 startPosition = 0;
    char* singleEolChar = NULL;
    char* doubleEolChar = NULL;
    CBodyPart* bodyPart = NULL;

    // currently only support mixed and related.
    char* contentType = (char*)aContentType.Ptr();
    if( strncasecmp( contentType, Multipart_Mixed, strlen(Multipart_Mixed) ) != 0 && 
        strncasecmp( contentType, Multipart_Related, strlen(Multipart_Related) ) != 0 )
        {
        User::Leave( KErrNotSupported );
        }

    // get singleEol and doubleEol
    MultipartParser::SetEolCharacters( multipartBuffer,
                                       multipartLen,
                                       aBoundary,
                                       &singleEolChar,
                                       &doubleEolChar );

    // get body parts one by one
    // null bodyPartBuffer indicates the end of the multipart body
    int counter = 0;
    do
        {
        // stop when we get required number of parse done
        if( aMaxToParse != -1 && counter >= aMaxToParse )
            {
            break;
            }
        // update counter
        counter++;

        // get the next body part
        bodyPartBufferLength = MultipartParser::GetNextBodyPartBuffer( startPosition, 
                                                                       multipartBuffer,
                                                                       multipartLen,
                                                                       aBoundary,
                                                                       singleEolChar,
                                                                       &bodyPartBuffer );
        // break if we are at end
        if( bodyPartBuffer == NULL )
            {
            break;
            }
        // update start position
        startPosition += bodyPartBufferLength;

        // create new body part
        bodyPart = CBodyPart::NewL();
        // parse each body part buffer to fill in body part
        MultipartParser::ParseBodyPartL( bodyPartBuffer, 
                                         bodyPartBufferLength,
                                         singleEolChar,
                                         doubleEolChar,
                                         aUrl, 
                                         bodyPart );  
        // add the body part to the array
        aBodyPartsArray.Append( bodyPart );
        }
    while( bodyPartBuffer != NULL );
    }

// ------------------------------------------------------------------------- 
// Composes RFC1521 compliant multipart document with given bodyparts
// Actual task of creating the document is delegated to specialized composer
// for each of the subtypes
// ------------------------------------------------------------------------- 
EXPORT_C HBufC8* MultipartParser::ComposeL( RPointerArray<CBodyPart>& aBodyPartsArray,
                                            const TDesC8& aBoundary,
                                            TMultipartSubtype aSubtype,
                                            TInt aHeaderMask )
    {
    // check on required parameters
    if ( !aBoundary.Ptr() || !aBoundary.Length() )
        {
        User::Leave( KErrArgument );
        }
    
    HBufC8* multipartDoc = NULL;
    switch(aSubtype)
        {
        case EMultipartSubtypeMixed:
            {
            multipartDoc = ComposeMixedL( aBodyPartsArray,
                                          aBoundary,
                                          aHeaderMask );
            }
        break;
        default:
            {
            User::Leave( KErrArgument );
            }
        }
    return multipartDoc;
    }

// ------------------------------------------------------------------------- 
// Default constructor
// ------------------------------------------------------------------------- 
MultipartParser::MultipartParser()
	{
	}

// ------------------------------------------------------------------------- 
// Returns with the next body part buffer from start position (offset)
// ------------------------------------------------------------------------- 
TUint32
MultipartParser::GetNextBodyPartBuffer( TUint32 aStartPosition, 
                                        const TUint8* aMultipartBody,
                                        TUint32 aMultipartLen,
                                        const TDesC8& aBoundary,
                                        char* aSingleEolChar,
                                        TUint8** aBodyPartBuffer )
    {
    // check on required parameters
    __ASSERT_ALWAYS( aMultipartBody != NULL,
                     User::Panic(_L("MultipartParser Panic"), KErrArgument) );
    __ASSERT_ALWAYS( aBoundary.Ptr() != NULL,
                     User::Panic(_L("MultipartParser Panic"), KErrArgument) );
    __ASSERT_ALWAYS( aSingleEolChar != NULL,
                     User::Panic(_L("MultipartParser Panic"), KErrArgument) );

    long startOffset = -1;
    long endOffset = -1;
    TUint32 length = 0;
    TBool badContent = EFalse;
    TUint32 offset = 0;
    const char* boundaryStr = (const char*) aBoundary.Ptr();
    int boundaryLength = aBoundary.Length();
    int hypensLength = strlen( Multipart_Hypens_Text );
    int singleEolLength = strlen( aSingleEolChar );
    TUint32 i = aStartPosition;

    // from RFC 1341 7.2
    // Overall, the body of a multipart entity may be specified as follows:
    // multipart-body := preamble 1*encapsulation close-delimiter epilogue
    // encapsulation := delimiter CRLF body-part
    // delimiter := CRLF "--" boundary   ; taken from Content-Type field.
    // when content-type is multipart.
    // There must be no space between "--" and boundary.
    // close-delimiter := delimiter "--" ; Again, no  space  before
    // "--"

    // boundary = 12xy
    // body: here comes some text that we ignore
    //       --12xy
    //       first body
    //       --12xy
    //       second body
    //       --12xy--
    //       closing boundary. we ignore this text here
    while( i < (aMultipartLen - hypensLength + 1 ) )
        {
        // get the first two hypens
        // using char comparison to compare "--"
        // hopefully this is faster
        if( (char)aMultipartBody[ i ] == '-' && 
            (char)aMultipartBody[ i+1 ] == '-' )
          {
          char* boundary = (char*)&aMultipartBody[ i + hypensLength ];
          // check if the body is long enough first and then check if boundary matches
          if( aMultipartLen >= i + hypensLength + boundaryLength )
            {
            if( strncasecmp( boundary, boundaryStr, boundaryLength ) == 0 )
              {
              // we've got the boundary
              offset = i + hypensLength + boundaryLength;
              // Next two chars must be either two hypens (closing boundary - 2 bytes),
              // or single Eol characters (new body).
              // Eol = CRLF (2 bytes - windows) or LF (1 byte - unix/mac).
              char* eolBuf = (char*)&aMultipartBody[ offset ];
              // Check if buffer is long enough for hypens [2 bytes], or eol [1 or 2 bytes]
              if( aMultipartLen >= offset + hypensLength )
                {
                 if( strncmp( eolBuf, aSingleEolChar, singleEolLength ) == 0|| 
                     eolBuf[0] == Multipart_LF_Text[0])
                   
                  {
                  // We found Eol, so this is a new multipart body (header and content)
                  if( startOffset == -1 )
                    {
                    // this is the beginning.
                    startOffset = offset;
                    // let's looking for the end of this body part which either could
                    // be closing boundary or an opening boundary for the next body part

                    // jump over the boundary information
                    i = startOffset + singleEolLength;
                    }
                  else
                    {
                    // We found the next boundary marker, so this is the
                    // beginning of the next body part
                    endOffset = offset - boundaryLength - hypensLength;
                    // we've got both start and end offset
                    break;
                    }
                  }
                 else if( strncmp( eolBuf, Multipart_Hypens_Text, hypensLength ) == 0 )
                   {
                    // We found the closing boundary marker
                    endOffset = offset - boundaryLength - hypensLength;
                    break;
                   }
                 else
                   {
                   // it's neither Eol nor two hypens "--"
                   badContent = ETrue;
                   break;
                   }
                }
              else
                {
                // the buffer is too short and not closed properly
                endOffset = i;
                break;
                }
              }
            }
          else
            {
            // the buffer is far too short
            endOffset = i;
            break;
            }
          }
        i++;
        } // end of while loop

    // missing closing boundary check
    if( endOffset == -1 )
        {
        // take the end of the body as closing boundary
        endOffset = i - 1;
        }

    if( badContent )
        {
        endOffset = -1;
        startOffset = -1;
        }

    if( startOffset != -1 && endOffset != -1 )
        {
        *aBodyPartBuffer = (TUint8*)&aMultipartBody[ startOffset ];
        length = endOffset - startOffset;
        }
    else
        {
        *aBodyPartBuffer = NULL;
        length = 0;
        }

    return length;
    }


// ------------------------------------------------------------------------- 
// Set End-Of-Line characters.  Look at the eol character after the boundary to
// determine if it is a CRLF (2 bytes used by windows), or LF (1 byte used by
// unix/mac).

// NOTE: CR = 0x0D = '\r', LF = 0x0A = '\n'

// Multipart entity is specified as follows:
// --boundary123[eol]
// Content-type: text/html[eol]
// Content-location: http:\\www.example.com\index.html[eol]
// [eol]
// <html><body>...example...</html>[eol]
// --boundary123--[eol]
// ------------------------------------------------------------------------- 
void
MultipartParser::SetEolCharacters( const TUint8* aMultipartBody,
                                   TUint32 aMultipartLen,
                                   const TDesC8& aBoundary,
                                   char** aSingleEolChar,
                                   char** aDoubleEolChar )
    {
    // check on required parameters
    __ASSERT_ALWAYS( aMultipartBody != NULL,
                     User::Panic(_L("MultipartParser Panic"), KErrArgument) );
    __ASSERT_ALWAYS( aBoundary.Ptr() != NULL,
                     User::Panic(_L("MultipartParser Panic"), KErrArgument) );
    __ASSERT_ALWAYS( aSingleEolChar != NULL,
                     User::Panic(_L("MultipartParser Panic"), KErrArgument) );
    __ASSERT_ALWAYS( aDoubleEolChar != NULL,
                     User::Panic(_L("MultipartParser Panic"), KErrArgument) );

    TUint32 i = 0;
    const char* boundaryStr = (const char*) aBoundary.Ptr();
    int boundaryLength = aBoundary.Length();
    int hypensLength = strlen( Multipart_Hypens_Text );
    int lfLength = strlen( Multipart_LF_Text );

    // Set the default eol (CRLF)
    *aSingleEolChar = (char *)Multipart_CRLF_Text;
    *aDoubleEolChar = (char *)Multipart_DoubleCRLF_Text;

    while (i < (aMultipartLen - hypensLength + 1))
        {
        // Get the first two hypens
        char* bodyPart = (char*)&aMultipartBody[ i ];
        if (strncmp(bodyPart, Multipart_Hypens_Text, hypensLength) == 0)
            {
            char* boundary = (char*)&aMultipartBody[ i + hypensLength ];
            // Check if the body is long enough first and then check if boundary matches
            if (aMultipartLen >= i + hypensLength + boundaryLength )
                {
                if (strncasecmp(boundary, boundaryStr, boundaryLength) == 0)
                    {
                    // We've got the boundary
                    i = i + hypensLength + boundaryLength;
                    // Next two chars should be the single Eol characters.
                    // Eol = CRLF (2 bytes - windows), or LF (1 byte - unix/mac).
                    char* eolBuf = (char*)&aMultipartBody[ i ];
                    // Check if buffer is long enough for eol [1 byte LF]
                    if (aMultipartLen >= i + lfLength)
                        {
                        if (strncmp(eolBuf, Multipart_LF_Text, lfLength) == 0)
                            {
                            // We found LF Eol (unix/mac)
                            *aSingleEolChar = (char *)Multipart_LF_Text;
                            *aDoubleEolChar = (char *)Multipart_DoubleLF_Text;
                            } // end of if compare eol to LF
                        } // end of if buffer size ok

                    // Break in all cases, we will use the default CRLF if we don't
                    // find eol=LF, or the remaining buffer is too small
                    break;
                    } // end of compare/found boundary
                }
            } // end of looking for first two hypens

        ++i;
        } // end of while
    }


// ------------------------------------------------------------------------- 
// parse body
// The bodyPart parameter can contain the optional headers and response, or
// just the response.  In the case of both the (optional) header(s) and the
// response, let's cut off header(s) and return the response body.  The
// header is seperated from the response by two End-of-line (Eol) characters,
// i.e. two CRLF's (windows) or two LF's (unix/mac).
// --boundary123 (omitted from bodyPart parameter, starts next line)
// Content-type: text/html[eol]
// Content-location: http:\\www.example.com\index.html[eol]
// [eol]
// <html><body>example</body></html>
//
// In the case of no headers, there may be only one (or more) Eol characters.
// --boundary123 (omitted from bodyPart parameter, starts on next line)
// [eol]
// <html><body>example</body></html>
// ------------------------------------------------------------------------- 
void
MultipartParser::ParseBodyPartL( TUint8* aBodyPartBuffer,
                                 TUint32 aBodyPartBufferLength,
                                 char* aSingleEolChar,
                                 char* aDoubleEolChar,
                                 const TDesC16& aResponseUrl,
                                 CBodyPart* aBodyPart )
    {
    // check on required parameters
    __ASSERT_ALWAYS( aBodyPartBuffer != NULL,
                     User::Panic(_L("MultipartParser Panic"), KErrArgument) );
    __ASSERT_ALWAYS( aSingleEolChar != NULL,
                     User::Panic(_L("MultipartParser Panic"), KErrArgument) );
    __ASSERT_ALWAYS( aDoubleEolChar != NULL,
                     User::Panic(_L("MultipartParser Panic"), KErrArgument) );
    __ASSERT_ALWAYS( aBodyPart != NULL,
                     User::Panic(_L("MultipartParser Panic"), KErrArgument) );
    __ASSERT_ALWAYS( aResponseUrl.Ptr() != NULL,
                     User::Panic(_L("MultipartParser Panic"), KErrArgument) );

    // headers look something like this
    // we need to return "text/html" if the requested header is "Content-type"
    // --boundary123
    // Content-type: text/html
    // Content-location: http:\\www.example.com\index.html
    //
    // <html>
    // <body>
    // example
    int contentHeaderValueCharLen = 0;
    TPtrC8 contentHeaderValuePtr( NULL, 0 );
    int contentHeaderNameLength = 0;

    /*lint -e{668} Possibly passing a null pointer to function */
    int singleEolLength = strlen( aSingleEolChar );
    int doubleEolLength = strlen( aDoubleEolChar );
    // start looking for the header name
    for( TUint32 i = 0; i < aBodyPartBufferLength ; i++ )
        {
        int found = 0;
        const char* tempBodyPartBuffer = (char*)&aBodyPartBuffer[ i ];

        // Did we find the Content Header Value
	    if (strncasecmp( tempBodyPartBuffer, Multipart_Content_Base_Text, MULTIPART_CONTENT_BASE_LENGTH ) == 0)
            {
            contentHeaderNameLength = MULTIPART_CONTENT_BASE_LENGTH;
            found = MULTIPART_CONTENT_BASE;
            }
	    else if (strncasecmp( tempBodyPartBuffer, Multipart_Content_Location_Text, MULTIPART_CONTENT_LOCATION_LENGTH ) == 0)
            {
            contentHeaderNameLength = MULTIPART_CONTENT_LOCATION_LENGTH;
            found = MULTIPART_CONTENT_LOCATION;
            }
	    else if (strncasecmp( tempBodyPartBuffer, Multipart_Content_Transfer_Encoding_Text, MULTIPART_CONTENT_TRANSFER_ENCODING_LENGTH ) == 0)
            {
            contentHeaderNameLength = MULTIPART_CONTENT_TRANSFER_ENCODING_LENGTH;
            found = MULTIPART_CONTENT_TRANSFER_ENCODING;
            }     
	    else if (strncasecmp( tempBodyPartBuffer, Multipart_Content_Encoding_Text, MULTIPART_CONTENT_ENCODING_LENGTH ) == 0)
            {
            contentHeaderNameLength = MULTIPART_CONTENT_ENCODING_LENGTH;
            found = MULTIPART_CONTENT_TRANSFER_ENCODING;
            }
	    else if (strncasecmp( tempBodyPartBuffer, Multipart_Content_Type_Text, MULTIPART_CONTENT_TYPE_LENGTH ) == 0)
            {
            contentHeaderNameLength = MULTIPART_CONTENT_TYPE_LENGTH;
            found = MULTIPART_CONTENT_TYPE;
            }
	    else if (strncasecmp( tempBodyPartBuffer, Multipart_Content_ID_Text, MULTIPART_CONTENT_ID_LENGTH ) == 0)
            {
            contentHeaderNameLength = MULTIPART_CONTENT_ID_LENGTH;
            found = MULTIPART_CONTENT_ID;
            }

        if (found)
            {        
            // skip spaces
            int startPos = i + contentHeaderNameLength;
            while ( (char)aBodyPartBuffer[ startPos ] == ' ' )
                {
                startPos++;
                }

            // used for finding '<' in Content-ID field
            char charFirst = aBodyPartBuffer[ startPos ];
            // content headers are closed with End-Of-Line (Eol) character
            for( TUint32 j = startPos; j < aBodyPartBufferLength - singleEolLength + 1; j++ )
                {
                char* tmpContentHeaderValue = (char*)&aBodyPartBuffer[ j ];
                if( strncmp( tmpContentHeaderValue, aSingleEolChar, singleEolLength ) == 0 
                   || tmpContentHeaderValue[0] == Multipart_LF_Text[0])
                    {
                    if( found == MULTIPART_CONTENT_ID )
                        {
                        if( charFirst == '<' )
                            {
                            // length of the value excluding beginging '<' and ending '>'
                            contentHeaderValueCharLen = j - startPos - 2;
                            contentHeaderValuePtr.Set( (TUint8*)&aBodyPartBuffer[ startPos+1 ], contentHeaderValueCharLen );
                            }
                        }
                    else
                        {
                        // length of the value
                        contentHeaderValueCharLen = j - startPos;
                        contentHeaderValuePtr.Set( (TUint8*)&aBodyPartBuffer[ startPos ], contentHeaderValueCharLen );
                        }

                    // rewind so the double EOL will be checked against later
                    i = j - 1;
                    // break the inner loop
                    break;
                    }              
                } // end of inner for loop

            switch( found )
                {
                case MULTIPART_CONTENT_BASE:
                    aBodyPart->SetContentBase( contentHeaderValuePtr );
                    break;
                case MULTIPART_CONTENT_LOCATION:
                    aBodyPart->SetContentLocation( contentHeaderValuePtr );
                    break;
                case MULTIPART_CONTENT_TRANSFER_ENCODING:
                    aBodyPart->SetContentTransferEncoding( contentHeaderValuePtr );
                    break;
                case MULTIPART_CONTENT_TYPE:
                    aBodyPart->SetContentType( contentHeaderValuePtr );
                    break;
                case MULTIPART_CONTENT_ID:
                    aBodyPart->SetContentID( contentHeaderValuePtr );
                    break;
                default:
                    break;
                } 
            } // end of if (found) 

        // Did we get to the end of the Content Header. Many of the Content Header Values
	    // are optional, so we could get to the end of the Content Header (double Eol) and
	    // not find the Content Header Value we were searching for.
        // get the response body
	    int aEolLength = strlen(Multipart_DoubleLF_Text);
	    if (strncmp( tempBodyPartBuffer, aDoubleEolChar, doubleEolLength ) == 0 ||
	        strncmp( tempBodyPartBuffer, Multipart_DoubleLF_Text,aEolLength)== 0)
            {
            if(strncmp( tempBodyPartBuffer, aDoubleEolChar, doubleEolLength)== 0)
               aEolLength = doubleEolLength;
            // set body
            TUint8* responseBody = (TUint8*) &tempBodyPartBuffer[aEolLength];
            int lengthBody = aBodyPartBufferLength - ( i + aEolLength );
            TPtrC8 body( responseBody, lengthBody );
            aBodyPart->SetBody( body );

            // set headers when we have headers
            if( i != 0 )
                {
                // jump over the starting single EOL
                TUint8* responseHeaders = (TUint8*) &aBodyPartBuffer[ singleEolLength ];
                // // jump over the starting single EOL and the ending double EOL
                int lengthHeaders = aBodyPartBufferLength - lengthBody - singleEolLength - aEolLength;
                TPtrC8 headers( responseHeaders, lengthHeaders );
                aBodyPart->SetHeaders( headers );
                }

            break;
            }
       } // end of outter for loop

    // prepare more on body part

    // Check to see if we have a Content-Transfer-Encoding.
    TUint8* contentTransferEncodingValue = (TUint8*) aBodyPart->ContentTransferEncoding().Ptr();
    // If we have Content-Transfer-Encoding, prepare to decode
    if( MultipartParser::IsEncoded(contentTransferEncodingValue) )
        {
        // Initialize the encoded body, input
        TPtrC8 encodedBody( aBodyPart->Body() );

        // This will contain the "decoded" data.
        // The memory (decodedBody.Ptr) is owned by this method, but allocated by
        // the DecodeContentTransferEncoding method.
        TPtr8 decodedBody( NULL, 0, 0 );

        // The decoded data will return in decodedBody.Ptr.
        // The memory allocated is owned by this method.
        TInt err = MultipartParser::DecodeContentTransferEncoding( contentTransferEncodingValue,
                                                                   encodedBody,
                                                                   decodedBody );                    
        User::LeaveIfError(err);

        // The responseBody pointer is an offset into the response
        // buffer, do not delete. Substitute the decodedBody pointer.
        aBodyPart->SetBody( decodedBody );
        aBodyPart->SetIsDecodedBody( ETrue );
        }   // end of if (contentTransferEncodingValue)

    // Check to see if we have a Content-Type.
    TUint8* contentTypeValue = (TUint8*) aBodyPart->ContentType().Ptr();
    // parse contentType to get new contentType, charset, boundary info
    if( contentTypeValue )
        {
        MultipartParser::CutOffContentTypeAttributes( aBodyPart );
        // updated content type
        contentTypeValue = (TUint8*) aBodyPart->ContentType().Ptr();
        }

    // If we have zipped Content-Type, prepare to unzip
    if( MultipartParser::IsZipped(contentTypeValue) )
        {
        // Initialize the zipped body, input
        TPtrC8 zippedBody( aBodyPart->Body() );

        // This will contain the "unzipped" data.
        // The memory (unzippedBody.Ptr) is owned by this method, but allocated by
        // the Unzip method.
        TPtr8 unzippedBody( NULL, 0, 0 );

        // The unzipped data will return in unzippedBody.Ptr.
        // The memory allocated is owned by this method.
        TInt err = MultipartParser::Unzip( contentTypeValue,
                                           zippedBody,
                                           unzippedBody );                    
        User::LeaveIfError(err);

        if( aBodyPart->IsDecodedBody() )
            {
            // old body is not the original buffer, delete it
            delete (TUint8*) aBodyPart->Body().Ptr();
            }
        // unzip happend, use unzippedBody; delete decodedBody
        else
            {
            // The responseBody pointer is an offset into the response
            // buffer, do not delete. Substitute the decodedBody pointer.

            aBodyPart->SetIsDecodedBody( ETrue );
            }

        aBodyPart->SetBody( unzippedBody );
        }

    // Get the url of the current body part
    HBufC16* responseUrl = MultipartParser::GetBodyPartUrlL( aBodyPart->ContentBase(),
                                                             aBodyPart->ContentLocation(),
                                                             aResponseUrl );
    aBodyPart->SetUrl( responseUrl );
    }


// ------------------------------------------------------------------------- 
// From RFC 2045 Multipurpose Internet Mail Extensions (MIME) Part One: Format of Internet Message Bodies
//   Section 6.2.  Content-Transfer-Encodings Semantics
//
//   The Content-Transfer-Encoding values "7bit", "8bit", and "binary" all
//   mean that the identity (i.e. NO) encoding transformation has been
//   performed.  As such, they serve simply as indicators of the domain of
//   the body data, and provide useful information about the sort of
//   encoding that might be needed for transmission in a given transport
//   system.  The terms "7bit data", "8bit data", and "binary data" are
//   all defined in Section 2.

//  Returns true if contentTransferEncodingValue is neither NULL nor a domain.
// ------------------------------------------------------------------------- 
TBool MultipartParser::IsEncoded( TUint8* aContentTransferEncodingValue )
    {        
    if( !aContentTransferEncodingValue )
        {
        return EFalse;
        }

    char* encoding = (char*)aContentTransferEncodingValue;

    if ( strncasecmp( encoding,
                      Multipart_Content_Transfer_Encoding_7bit,
                      strlen(Multipart_Content_Transfer_Encoding_7bit) ) == 0 )
        {
        return EFalse;
        }

    if ( strncasecmp( encoding,
                      Multipart_Content_Transfer_Encoding_8bit,
                      strlen(Multipart_Content_Transfer_Encoding_8bit) ) == 0 )
        {
        return EFalse;
        }

    if ( strncasecmp( encoding,
                      Multipart_Content_Transfer_Encoding_binary,
                      strlen(Multipart_Content_Transfer_Encoding_binary) ) == 0 )
        {
        return EFalse;
        }      

    return ETrue;
    }


// ----------------------------------------------------------------------------
// DecodeContentTransferEncoding
//
// Decodes the Content-Transfer-Encoding.  The returned length of decodedBody
// is zero if decoding failed.
// NOTES:
// 1. This method should be called with a non-null string, i.e.
// aContentTransferEncodingValue.
// 2. Memory is allocated in this method, but ownership is with the calling method.
// ----------------------------------------------------------------------------
TInt
MultipartParser::DecodeContentTransferEncoding( TUint8* aContentTransferEncodingValue,
                                                const TDesC8& aEncodedBody,
                                                TPtr8& aDecodedBody )
    {
    // check on required parameters
    __ASSERT_ALWAYS( aContentTransferEncodingValue != NULL,
                     User::Panic(_L("MultipartParser Panic"), KErrArgument) );
    __ASSERT_ALWAYS( aEncodedBody.Ptr() != NULL,
                     User::Panic(_L("MultipartParser Panic"), KErrArgument) );

    TInt status = KErrNone;
    char* contentTransferEncodingString = (char*)aContentTransferEncodingValue;

    // Set the decodedBody.Length to zero, length > 0 if successful decode
    aDecodedBody.SetLength(0);

    // Is the Content-Transfer-Encoding = "base64"
    if( strncasecmp( contentTransferEncodingString,
                     Multipart_Content_Transfer_Encoding_Base64, 
                     strlen(Multipart_Content_Transfer_Encoding_Base64) ) == 0 )
        {
        // The decoded length of base64 is about half (use same) encoded length
        TUint maxBodyLength = aEncodedBody.Length();
        TUint8* decodedDataPtr = new TUint8[maxBodyLength];
        if( decodedDataPtr )
            {
            aDecodedBody.Set(decodedDataPtr, 0, maxBodyLength);

			using namespace BSUL;
            // Decode the base64 Content-Transfer-Encoding
            Base64Codec::Decode(aEncodedBody, aDecodedBody);

            if (aDecodedBody.Length() == 0)
                {
                status = KErrGeneral;
                }
            }
        else // decodedDataPtr is NULL
            {
            status = KErrNoMemory;
            }
        }   // end of base64 decoding

    // Is the Content-Transfer-Encoding = "quoted-printable"
    else if( strncasecmp( contentTransferEncodingString,
                          Multipart_Content_Transfer_Encoding_QuotedPrintable, 
                          strlen(Multipart_Content_Transfer_Encoding_QuotedPrintable) ) == 0 )
        {
        // The decoded length of QP is the same as the encoded length
        TUint maxBodyLength = aEncodedBody.Length();
        TUint8* decodedDataPtr = new TUint8[maxBodyLength];
        if( decodedDataPtr )
            {
            aDecodedBody.Set(decodedDataPtr, 0, maxBodyLength);

            // Decode the quoted-printable Content-Transfer-Encoding
            QuotedPrintableCodec::Decode(aEncodedBody, aDecodedBody);

            if (aDecodedBody.Length() == 0)
                {
                status = KErrGeneral;
                }
            }
        else // decodedDataPtr is NULL
            {
            status = KErrNoMemory;
            }
        }   // end of quoted-printed decoding

    // Is the Content-Encoding = "gzip"
    else if( strncasecmp( contentTransferEncodingString,
                          Multipart_Content_Encoding_GZip, 
                          strlen(Multipart_Content_Encoding_GZip) ) == 0 )
        {
        // Our GZip decoder parts
        GZipBufMgr* gZipBufMgr = NULL;
        CEZDecompressor* ezDecompressor = NULL;

        // We have gzip, lets decompress the encoded data.
        // Set up the encoded data into a GZip buffer manager.
        TInt err = 0;
        TRAP(err, gZipBufMgr = GZipBufMgr::NewL( aEncodedBody ));

        // Get the GZip decompressor
        if( gZipBufMgr )
            {
            TRAP(err, ezDecompressor = CEZDecompressor::NewL(*gZipBufMgr, -CEZDecompressor::EMaxWBits));

            // Inflate the GZip data
            if( ezDecompressor )
                {
                TRAP(err, ezDecompressor->InflateL());
                // Set the finalize flag
                if (err == KErrNone)
                    {
                    TRAP(err, gZipBufMgr->FinalizeL(*ezDecompressor));
                    // Get the inflated data, it is much larger then the encoded data
                    if (err == KErrNone)
                        {
                        TPtrC8 output = ezDecompressor->OutputDescriptor();
                        if (output.Length() != 0)
                            {
                            TInt size = output.Size();
                            TUint8* outBuf = new TUint8[size];
                            if( outBuf )
                                {
                                memcpy(outBuf, output.Ptr(), size);

                                aDecodedBody.Set((TUint8*)outBuf, size, size);
                                }
                            else // outBuf is NULL
                                {
                                status = KErrNoMemory;
                                }
                            }
                        else
                            {
                            status = KErrGeneral;
                            }
                        }
                    else
                        {
                        status = KErrGeneral;
                        }
                    }
                else
                    {
                    status = KErrGeneral;
                    }
                }
            else // ezDecompressor is NULL
                {
                status = KErrNoMemory;
                }
            }
        else // gZipBufMgr is NULL
            {
            status = KErrNoMemory;
            }

        // Clean up our memory
        delete gZipBufMgr;
        delete ezDecompressor;
        }   // end of gzip 

    // We can add additional decodings here.
    // When adding additional decoding be aware of the decodedBody.Ptr()
    // max size. Do the realloc here, AND allow the decodedBody.Ptr()
    // ownership to be passed back to calling method.

    return status;
    }

    
// ------------------------------------------------------------------------- 
// only support application/x-gzip
// ------------------------------------------------------------------------- 
TBool MultipartParser::IsZipped( TUint8* aContentTypeValue )
    {        
    if( !aContentTypeValue )
        {
        return EFalse;
        }

    char* contentType = (char*)aContentTypeValue;

    if ( strncasecmp( contentType,
                      Multipart_Content_Type_GZip,
                      strlen(Multipart_Content_Type_GZip) ) == 0 )
        {
        return ETrue;
        }

    return EFalse;
    }


// ----------------------------------------------------------------------------
// Unzip
//
// Unzip the .gz.  The returned length of unzippedBody
// is zero if unzip failed.
// NOTES:
// 1. This method should be called with a non-null string, i.e.
// aContentType.
// 2. Memory is allocated in this method, but ownership is with the calling method.
// ----------------------------------------------------------------------------
TInt
MultipartParser::Unzip( TUint8* aContentType,
                        const TDesC8& aZippedBody,
                        TPtr8& aUnzippedBody )
    {
    // check on required parameters
    __ASSERT_ALWAYS( aContentType != NULL,
                     User::Panic(_L("MultipartParser Panic"), KErrArgument) );
    __ASSERT_ALWAYS( aZippedBody.Ptr() != NULL,
                     User::Panic(_L("MultipartParser Panic"), KErrArgument) );

    TInt status = KErrNone;
    char* contentTypeStr = (char*)aContentType;

    // Set the aUnzippedBody.Length to zero, length > 0 if successful decode
    aUnzippedBody.SetLength(0);

    // Our GZip decoder parts
    GZipBufMgr* gZipBufMgr = NULL;
    CEZDecompressor* ezDecompressor = NULL;

    // Is the Content-Type = "application/x-gzip"
    if( strncasecmp( contentTypeStr,
                     Multipart_Content_Type_GZip, 
                     strlen(Multipart_Content_Type_GZip) ) == 0 )
        {
        // We have gzip, lets decompress the encoded data.
        // Set up the encoded data into a GZip buffer manager.
        TInt err = 0;
        TRAP(err, gZipBufMgr = GZipBufMgr::NewL(aZippedBody));

        // Get the GZip decompressor
        if( gZipBufMgr )
            {
            TRAP(err, ezDecompressor = CEZDecompressor::NewL(*gZipBufMgr, -CEZDecompressor::EMaxWBits));

            // Inflate the GZip data
            if( ezDecompressor )
                {
                TRAP(err, ezDecompressor->InflateL());
                // Set the finalize flag
                if (err == KErrNone)
                    {
                    TRAP(err, gZipBufMgr->FinalizeL(*ezDecompressor));
                    // Get the inflated data, it is much larger then the encoded data
                    if (err == KErrNone)
                        {
                        TPtrC8 output = ezDecompressor->OutputDescriptor();
                        if (output.Length() != 0)
                            {
                            TInt size = output.Size();
                            TUint8* outBuf = new TUint8[size];
                            if( outBuf )
                                {
                                memcpy(outBuf, output.Ptr(), size);

                                aUnzippedBody.Set((TUint8*)outBuf, size, size);
                                }
                            else // outBuf is NULL
                                {
                                status = KErrNoMemory;
                                }
                            }
                        else
                            {
                            status = KErrGeneral;
                            }
                        }
                    else
                        {
                        status = KErrGeneral;
                        }
                    }
                else
                    {
                    status = KErrGeneral;
                    }
                }
            else // ezDecompressor is NULL
                {
                status = KErrNoMemory;
                }
            }
        else // gZipBufMgr is NULL
            {
            status = KErrNoMemory;
            }
        }   // end of gzip 

    // Clean up our memory
    delete gZipBufMgr;
    delete ezDecompressor;

    return status;
    }


// ----------------------------------------------------------------------------
// It cuts off the charset value from the content type header
// content type string looks like as follows:
// text/plain; charset=us-ascii; boundary="abc"
// ----------------------------------------------------------------------------
void
MultipartParser::CutOffContentTypeAttributes( CBodyPart* aBodyPart ) 
    {
    // check on required parameters
    __ASSERT_ALWAYS( aBodyPart != NULL,
                     User::Panic(_L("MultipartParser Panic"), KErrArgument) );

    TPtrC8 aContentType( aBodyPart->ContentType() );

    // check if there is a delimiter ';'
    TInt lenCT = aContentType.Length();
    TInt offset = aContentType.Find( (TUint8*)Multipart_ContentTypeString_Delimiter_Text, 
                                     strlen(Multipart_ContentTypeString_Delimiter_Text) );
    if (offset != KErrNotFound)
        {
        // ; is meant to be the end of the content type value.
        // cut off content type unrelated part
        aBodyPart->SetContentType( aContentType.Left( offset ) );

        // extract boundary and charset info
        if( lenCT > offset )
            {
            TPtrC8 unrelated = aContentType.Right( lenCT - offset );
            TInt lenU = unrelated.Length();
            
            // check the boundary information
            TInt offsetB = unrelated.Find( (TUint8*)Multipart_Boundary_Text, 
                                           strlen(Multipart_Boundary_Text) );
            if (offsetB != KErrNotFound)
                {
                // now, we are at the beginning of "boundary="abc" string.
                // move to the "abc" part
                TPtrC8 boundary = unrelated.Right( lenU - 
                                                   offsetB - 
                                                   strlen( Multipart_Boundary_Text ) );
                TInt lenB = boundary.Length();

                // look for where to end
                TInt offsetQ = boundary.Find( (TUint8*)Multipart_ContentTypeString_Quotes_Text, 
                                              strlen(Multipart_ContentTypeString_Quotes_Text) );
                if (offsetQ != KErrNotFound)
                    {
                    // skip the quote (") char
                    boundary.Set( boundary.Right( lenB - offsetQ ) );
                    }

                // look for where to end
                // check "
                TInt offsetE = boundary.Find( (TUint8*)Multipart_ContentTypeString_Quotes_Text, 
                                              strlen(Multipart_ContentTypeString_Quotes_Text) );
                if (offsetE == KErrNotFound)
                    {
                    // check ;
                    offsetE = boundary.Find( (TUint8*)Multipart_ContentTypeString_Delimiter_Text, 
                                             strlen(Multipart_ContentTypeString_Delimiter_Text) );
                    }
                if (offsetE != KErrNotFound)
                    {
                    boundary.Set( boundary.Left( offsetE ) );
                    }

                // set it on to the input parameter
                aBodyPart->SetBoundary( boundary );
                } // end of if (offsetB != KErrNotFound)

            // check the charset information
            TInt offsetCh = unrelated.Find( (TUint8*)Multipart_Charset_Text, 
                                            strlen(Multipart_Charset_Text) );
            if (offsetCh != KErrNotFound)
                {
                // now, we are at the beginning of "charset=us-ascii" string.
                // move to the us-ascii part
                TPtrC8 charset = unrelated.Right( lenU - 
                                                  offsetCh - 
                                                  strlen( Multipart_Charset_Text ) );
                TInt lenCh = charset.Length();

                // look for where to end
                TInt offsetQ = charset.Find( (TUint8*)Multipart_ContentTypeString_Quotes_Text, 
                                             strlen(Multipart_ContentTypeString_Quotes_Text) );
                if (offsetQ != KErrNotFound)
                    {
                    // skip the quote (") char
                    charset.Set( charset.Right( lenCh - offsetQ ) );
                    }

                // look for where to end
                // check "
                TInt offsetE = charset.Find( (TUint8*)Multipart_ContentTypeString_Quotes_Text, 
                                             strlen(Multipart_ContentTypeString_Quotes_Text) );
                if (offsetE == KErrNotFound)
                    {
                    // check ;
                    offsetE = charset.Find( (TUint8*)Multipart_ContentTypeString_Delimiter_Text, 
                                            strlen(Multipart_ContentTypeString_Delimiter_Text) );
                    }
                if (offsetE != KErrNotFound)
                    {
                    charset.Set( charset.Left( offsetE ) );
                    }

                // set it on to the input parameter
                aBodyPart->SetCharset( charset );
                } // end of if (offsetCh != KErrNotFound)

            } // end of if( lenCT > offset )
        } // end of if (offset != KErrNotFound)
    }


// ----------------------------------------------------------------------------
// MultipartParser::GetBodyPartUrl
//
// Builds up the URL which refers to this particular body part
// ----------------------------------------------------------------------------
HBufC16*
MultipartParser::GetBodyPartUrlL( const TDesC8& aContentBase,
                                  const TDesC8& aContentLocation,
                                  const TDesC16& aResponseUrl )
    {
    // check on required parameters
    __ASSERT_ALWAYS( aResponseUrl.Ptr() != NULL,
                     User::Panic(_L("MultipartParser Panic"), KErrArgument) );

    // Body url builds up as follows:
    // Take global (respond header) Content Base header first.
    // If local (in the body part) Content Base header is present, then
    // take it as default Content Base header (by overruling global Content
    // Base header).  Check if the Content Location header is either an
    // absolute or relative URL.  If it is an absolute, then ignore the
    // Content Base, otherwise concatenate them.
    // Error cases:
    //   No Content Base header + Content Location is relative URL
    //   No Content Location header
    //   Content Base header is relative URL
    TBool contentBaseInvalid = EFalse;
    HBufC16* url = NULL;
    
    if (aContentBase.Ptr())
        {
        // Check if it is a relative url
        if ( MultipartParser::IsUrlRelativeL( aContentBase ) )
            {
            // Relative URL is not valid here as base location.
            contentBaseInvalid = ETrue;
            }
        }
    else
        {
        // no content base header
        contentBaseInvalid = ETrue;
        } // end of if (aContentBase)

    if (contentBaseInvalid)
        {
        if( aResponseUrl.Ptr() )
            {
            // Copy response url
            TInt lenU = aResponseUrl.Length();
            url = HBufC::NewLC( lenU + 1 );
            url->Des().Copy( aResponseUrl );
            url->Des().ZeroTerminate();
            }
        }
    else
        {
        // Copy global content "base" location 
        TInt lenCB = aContentBase.Length();
        url = HBufC::NewLC( lenCB + 1 );
        url->Des().Copy( aContentBase );
        url->Des().ZeroTerminate();
        } // end of if (contentBaseInvalid)

    // Check if Content Localtion is valid
    if( aContentLocation.Ptr() )
        {
        TInt lenCL = aContentLocation.Length();

        // If the Content Location is an absolute URL, then Content Base value is ignored,
        // otherwise the absolute path is going to be built unless Content Base is missing
        if ( !MultipartParser::IsUrlRelativeL( aContentLocation ) )
            {
            // clean up memory
            if( url )
                {
                CleanupStack::PopAndDestroy();  // url
                }

            // fill url with content location
            url = HBufC::NewL( lenCL + 1 );
            url->Des().Copy( aContentLocation );
            url->Des().ZeroTerminate();
            }
        else
            {
            if( url )
                {
                HBufC16* urlN = MultipartParser::UrlRelToAbsL( *url, aContentLocation );

                CleanupStack::PopAndDestroy(); // url

                url = urlN;
                }
            }
        } // end of if( aContentLocation
    else
        {
        if( url )
            {
            CleanupStack::Pop(); // url
            }
        }

    return url;
    }


// ----------------------------------------------------------------------------
// MultipartParser::IsUrlRelativeL
//
// ----------------------------------------------------------------------------
TBool
MultipartParser::IsUrlRelativeL( const TDesC8& aUrl )
    {
    // check on required parameters
    __ASSERT_ALWAYS( aUrl.Ptr() != NULL,
                     User::Panic(_L("MultipartParser Panic"), KErrArgument) );

    TUriParser8 uriParser;

    User::LeaveIfError(uriParser.Parse(aUrl));

    if( uriParser.Extract(EUriScheme).Ptr() )
        {
        return EFalse;
        }
    else
        {
        return ETrue;
        }
    }


// ----------------------------------------------------------------------------
// MultipartParser::UrlRelToAbsL
//
// Absolute path is built as : Base + Relative
// ----------------------------------------------------------------------------
HBufC16*
MultipartParser::UrlRelToAbsL( TDesC16& aBase, 
                               const TDesC8& aRelativeUrl )
    {
    // check on required parameters
    __ASSERT_ALWAYS( aBase.Ptr() != NULL,
                     User::Panic(_L("MultipartParser Panic"), KErrArgument) );
    __ASSERT_ALWAYS( aRelativeUrl.Ptr() != NULL,
                     User::Panic(_L("MultipartParser Panic"), KErrArgument) );

    // length of absolute url
    TInt len = 0;
    // length of relative url
    TInt lenR = 0;
    TBool appendSlash = EFalse;
    // path of absolute url
    TPtrC16 path( NULL, 0 );

    // must to have aRelativeUrl
    User::LeaveIfNull( (TUint8*)aRelativeUrl.Ptr() );

    TUriParser16 uriParser;
    User::LeaveIfError( uriParser.Parse( aBase ) );

    // <scheme>://
    TPtrC16 scheme( uriParser.Extract( EUriScheme ) );
    // must to have scheme
    User::LeaveIfNull( (TUint16*)scheme.Ptr() );
    len += scheme.Length() + SCHEME_SEPARATOR_LENGTH;

    // <user>:<password>@
    TPtrC16 user( uriParser.Extract( EUriUserinfo ) );
    if( user.Ptr() )
        {
        len += user.Length() + 1;
        }

    // <host>
    TPtrC16 host( uriParser.Extract( EUriHost ) );
    // must to have host
    User::LeaveIfNull( (TUint16*)host.Ptr() );
    len += host.Length();

    // :<port>
    TPtrC16 port( uriParser.Extract( EUriPort ) );
    if( port.Ptr() )
        {
        len += port.Length();
        }

    // If the relative url begins with "./", remove it
    TPtrC8 relativeUrl( NULL, 0 );
    TInt indexD = aRelativeUrl.Locate( DOT_CHAR );
    TInt indexS = aRelativeUrl.Locate( SLASH_CHAR );
    if ( indexD == 0 && indexS == 1)
        {
        // Found a dot-slash at beginning of relative url
        relativeUrl.Set( aRelativeUrl.Mid( 2 ) );
        }
    else
        {
        relativeUrl.Set( aRelativeUrl );
        }

    lenR = relativeUrl.Length();
    len += lenR;
    // If the relative url begins with a slash, then it is an absolute path
    // Does relative url start with slash?
    indexS = relativeUrl.Locate( SLASH_CHAR );
    // no, need to extract path from base url
    if( indexS != 0 )
        {
      // <path>
        path.Set( uriParser.Extract( EUriPath ) );
        if( path.Ptr() )
            {
            // cut off the file path
            if ( path.LocateReverse( DOT_CHAR ) )
                {
                // case: dir/index.html
                if ( TInt indexS2 = path.LocateReverse( SLASH_CHAR ) )
                    {
                    // to keep the slash
                    path.Set( path.Left( indexS2 + 1 ) );
                    }
                // case: index.html
                else
                    {
                    path.Set( NULL, 0 );
                    }
                }

            // figure out the end slash
            if( path.Ptr() )
                {
                if( path.LocateReverse( SLASH_CHAR ) != (path.Length() - 1) )
                    {
                    appendSlash = ETrue;
                    }
    
                len += path.Length();
                }
            else
                {
                appendSlash = ETrue;
                }
            }
        }
    // yes, no need to extract path from base url
    if( appendSlash )
        {
        ++len;
        }

    // NULL terminator
	// In certain operator cases, need to have an extra space(size of a 2-byte NULL terminator) 
	// for proper String Termination
    len += 2;

    // new absolute url
    HBufC16* urlAbs = HBufC16::NewL( len );
    TPtr16 urlAbsPtr = urlAbs->Des();

    // copy base into absolute url

    // scheme
    urlAbsPtr.Copy( scheme );
    urlAbsPtr.Append( COLON_CHAR );
    urlAbsPtr.Append( SLASH_CHAR );
    urlAbsPtr.Append( SLASH_CHAR );

    // user
    if( user.Ptr() )
        {
        urlAbsPtr.Append( user );
        urlAbsPtr.Append( AT_CHAR );
        }

    // host
    urlAbsPtr.Append( host );

    // port
    if( port.Ptr() )
        {
        urlAbsPtr.Append( COLON_CHAR );
        urlAbsPtr.Append( port );
        }

    // path
    if( path.Ptr() )
        {
        urlAbsPtr.Append( path );
        }

    // slash between path and relative url
    if( appendSlash )
        {
        urlAbsPtr.Append( SLASH_CHAR );
        }

    // relative url
    TUint16* relUrlInt = new TUint16[ lenR ];
    TPtr16 relUrl16( relUrlInt, lenR );
    relUrl16.Copy( relativeUrl );
    urlAbsPtr.Append( relUrl16 );
    delete[] relUrlInt;

    // null terminate
    urlAbsPtr.ZeroTerminate();

    return urlAbs;
    }
    
// ------------------------------------------------------------------------- 
// Composes multipart/mixed document
// ------------------------------------------------------------------------- 
HBufC8*
MultipartParser::ComposeMixedL( RPointerArray<CBodyPart>& aBodyArray,
                                const TDesC8& aBoundary,
                                TInt aHeaderMask )
    {
    // --(aBoundary)
    _LIT8(KBoundary, "--%S\r\n");
    HBufC8* boundary = HBufC8::NewLC( aBoundary.Length() + 4 );
    boundary->Des().Format( KBoundary, &aBoundary );
    
    // Calculate the size of this document.
    TInt bodySize = 0;
    //    a. for each CBodyPart
    //       boundaries + CRLF between headers and body (constant addition)
    bodySize += (boundary->Length() + strlen(Multipart_CRLF_Text)) * aBodyArray.Count() ;
    for (TInt i = 0; i < aBodyArray.Count(); i++)
        {
        if (!(aBodyArray[i]->Headers().Length() +
            aBodyArray[i]->Body().Length()))
            {
            // one less boundary
            bodySize -= boundary->Length() + strlen(Multipart_CRLF_Text);
            // skip empty bodypart
            continue;
            }
        bodySize += aBodyArray[i]->Headers().Length();
    //       ensure there are only 2 CRLFs between header and body
        if (aBodyArray[i]->Headers().Length() > 0)
            {
            TPtrC8 bodyHeaders(aBodyArray[i]->Headers().Ptr(), aBodyArray[i]->Headers().Length());
            TUint newEnd = bodyHeaders.Length() - 1;
            while( bodyHeaders[ newEnd ] == '\r' || bodyHeaders[ newEnd ] == '\n' )
                {
                --newEnd;
                --bodySize;
                }
            // two CRLFs
            bodySize += strlen(Multipart_CRLF_Text);
            }
        bodySize += aBodyArray[i]->Body().Length();
    //       CRLF (end of body, add one only if there is body AND does not end with CRLF)
        TPtrC8 bodyBody(aBodyArray[i]->Body().Ptr(), aBodyArray[i]->Body().Length());
        if (bodyBody.Length() > 0 
            && bodyBody.Right(2) != TPtrC8((TUint8*)Multipart_CRLF_Text, strlen(Multipart_CRLF_Text)))
            {
            bodySize += strlen(Multipart_CRLF_Text);
            }
        }
    //     end boundary (boundary - '\r\n' + "--")
    bodySize += boundary->Length();
    TInt docSize = bodySize; 
    //     calculate the size of Headers
    _LIT8(KContentType, "Content-Type: multipart/mixed; boundary=\"%S\"\r\n");
    if ( aHeaderMask & EMultipartTopLevelHeaderContentType )
        {
        docSize += MULTIPART_CONTENT_TYPE_LENGTH + 1; // Content-Type: + empty space
        docSize += KContentType().Length() - 2 + aBoundary.Length(); // multipart/mixed; boundary="{aBoundary}"
        docSize += strlen(Multipart_CRLF_Text); // eol
        }
    if ( aHeaderMask & EMultipartTopLevelHeaderContentLength )
        {
        docSize += MULTIPART_CONTENT_LENGTH_LENGTH + 1; // Content-Length: + empty space
        // calculate number of chars needed to represent bodySize
        HBufC8* bodySizeSize = HBufC8::NewLC( 16 );
        bodySizeSize->Des().Num( bodySize );
        docSize += bodySizeSize->Length(); // content length (bodySize)
        docSize += strlen(Multipart_CRLF_Text); // eol
        CleanupStack::PopAndDestroy( bodySizeSize );
        }
    if ( aHeaderMask & EMultipartTopLevelHeaderLastModified )
        {
        docSize += MULTIPART_LAST_MODIFIED_LENGTH + 1;
        docSize += MULTIPART_INTERNET_DATE_STRING_LENGTH; // timestamp (fixed length)
        docSize += strlen(Multipart_CRLF_Text); // eol
        }
    //    extra CRLF for separating header and body
    docSize += strlen(Multipart_CRLF_Text);
    //  CALCULATION COMPLETE
    //  at this point, bodySize contains the size of bodyparts, i.e. Content-Length:
    //  and docSize contains the size of the entire document (use it to create HBufC8*
    //  of appropriate size)
  
    //  construct multipart document
    HBufC8* document = HBufC8::NewLC(docSize);
    TPtr8 docAppend(document->Des());
    if ( aHeaderMask & EMultipartTopLevelHeaderContentType )
        {
        docAppend.Format( KContentType, &aBoundary );
        }
    if ( aHeaderMask & EMultipartTopLevelHeaderContentLength )
        {
        _LIT8( KContentLength, "Content-Length: %d\r\n" );
        docAppend.AppendFormat( KContentLength, bodySize );
        }
    if ( aHeaderMask & EMultipartTopLevelHeaderLastModified )
        {
        _LIT8( KLastModified, "Last-Modified: %S\r\n" );
        TTime current;
        current.UniversalTime();
        TInternetDate modDate(current.DateTime());
        HBufC8* dateString = modDate.InternetDateTimeL( TInternetDate::ERfc1123Format );
        docAppend.AppendFormat( KLastModified, dateString );
        delete dateString;
        }
    // required CRLF
    docAppend.Append((TUint8*)Multipart_CRLF_Text, strlen(Multipart_CRLF_Text));
    //  BodyParts
    for (TInt i = 0; i < aBodyArray.Count(); i++)
        {
        if (!(aBodyArray[i]->Headers().Length() +
            aBodyArray[i]->Body().Length()))
            {
            // skip empty bodypart
            continue;
            }
        docAppend.Append( *boundary );
        TInt headerLength = aBodyArray[i]->Headers().Length() - 1;
        while ( headerLength > 0 &&
                (aBodyArray[i]->Headers()[headerLength] == '\r'
                || aBodyArray[i]->Headers()[headerLength] == '\n' ))
            {
            --headerLength;
            }
        docAppend.Append( aBodyArray[i]->Headers().Ptr(), headerLength + 1 );

        if ( headerLength > 0 )
            {
            docAppend.Append((TUint8*)Multipart_DoubleCRLF_Text, strlen(Multipart_DoubleCRLF_Text));
            }
        else
            {
            docAppend.Append((TUint8*)Multipart_CRLF_Text, strlen(Multipart_CRLF_Text));
            }
    //  body
        docAppend.Append(aBodyArray[i]->Body());
    //  CRLF only if body exists and doesn't end with CRLF
        TPtrC8 bodyBody(aBodyArray[i]->Body().Ptr(), aBodyArray[i]->Body().Length());
        if (bodyBody.Length() > 0
            && bodyBody.Right(2) != TPtrC8((TUint8*)Multipart_CRLF_Text, strlen(Multipart_CRLF_Text)))
            {
            docAppend.Append((TUint8*)Multipart_CRLF_Text, strlen(Multipart_CRLF_Text));
            }
        }
    //  end boundary
    _LIT8(KEndBoundary, "--%S--");
    docAppend.AppendFormat(KEndBoundary, &aBoundary);
    CleanupStack::Pop( document );
    CleanupStack::PopAndDestroy( boundary );
    return document;
    }
