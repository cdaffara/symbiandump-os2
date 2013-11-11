/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
* Name          : SdpUtil.h
* Part of       : Local SDP Codec
* Interface     : -
* Version       : 1.0
*
*/




/**
 @internalComponent
*/

#ifndef SDPUTIL_H
#define SDPUTIL_H

//  INCLUDES
#include <e32base.h>
#include <stringpool.h>
#include <e32std.h>
#include "_sdpdefs.h"

// FORWARD DECLARATIONS
class RReadStream;
class RWriteStream;
class CSdpConnectionField;
class CSSdpKeyField;
class CSdpOriginField;
class TInetAddr;

// CLASS DECLARATION
/**
 *	Common utilities used in the implementation of the Local SDP codec.
 *  Utility class, cannot be instantiated
 */
class SdpUtil
	{

    public: // New functions

        /**
         *  Generic method that checks that all aValue's chars are not on illegal charset
         *
         *  @param aIllegalChars Illegal character set
         *  @param aValue String to be checked
         *  @param aStart Start position of the string
         *  @param aEnd End position of the string
         *  @return Validity of the string
         */
		static TBool IsValidCharSet( 
            const TDesC8& aIllegalChars, const TDesC8& aValue, TInt aStart, TInt aEnd );

        /**
         *  Checks that token list is valid (tokens with space as delimiter)
         *
         *  @param aValue String containing tokens
         *  @return Validity of the string
         */
		static TBool IsTokenList( const TDesC8& aValue );

        /**
         *  Checks that the token is valid (substring inside string)
         *
         *  @param aValue String containing the token
         *  @param aStart Start position of the token inside the string
         *  @param aEnd End position of the token inside the string
         *  @return Validity of the token
         */
		static TBool IsToken( const TDesC8& aValue, TInt aStart, TInt aEnd );

        /**
         *  Checks that the token is valid
         *
         *  @param aValue Token string 
         *  @return Validity of the token
         */
		static TBool IsToken( const TDesC8& aValue );

        /**
         *  Checks that the byte string is valid (substring inside string)
         *
         *  @param aValue String 
         *  @param aStart Start position of the byte string inside the string
         *  @param aEnd End position of the byte string inside the string
         *  @return Validity of the byte string
         */
		static TBool IsByteString( const TDesC8& aValue, TInt aStart, TInt aEnd );

        /**
         *  Checks that the byte string is valid
         *
         *  @param aValue String containing the token        
         *  @return Validity of the byte string
         */
		static TBool IsByteString( const TDesC8& aValue );

        /**
         *  Checks that the substring is a token-byte string pair with colon char
         *  as delimiter of the two strings
         *
         *  @param aValue String containing the pair
         *  @param aStart Start position of the string pair
         *  @param aEnd End position of the string pair        
         *  @return Validity of the pair
         */
		static TBool IsTokenTextPair( 
            const TDesC8& aValue, TInt aStart, TInt aEnd );

        /**
         *  Checks that the string conatins valid token-byte string pair with color
         *  char as delimiter of the two strings
         *
         *  @param aValue String containing the pair        
         *  @return Validity of the byte string
         */
		static TBool IsTokenTextPair( const TDesC8& aValue );

        /**
         *  Checks that the string does not contain any whitespace characters
         *
         *  @param aValue String        
         *  @return Validity of the string
         */
		static TBool IsNonWhitespace( const TDesC8& aValue );

        /**
         *  Checks that the string contains a valid NTP-time element
         *
         *  @param aValue String
         *  @return Validity of the string
         */
		static TBool IsValidNtpTime( const TDesC8& aValue );

        /**
         *  Divides the buffer into a number of lines. Each line is marked
         *  as a TPtrC8 object and added to array that is returned to the user
         *  NOTE! Each line in the returned array ends in LF character
         *  NOTE! The aLines must end in LF character, not '\0' character
         *
         *  @param aLines Buffer that contains all the string
         *  @param aErrCode Error code that is used on leave situations, when the
         *                  aLines parameter contains invalid string set
         *  @return Array that contains the division of buffer to lines
         */
        static RArray<TPtrC8> DivideToLinesL( 
            const TDesC8& aLines, TInt aErrCode );

        /**
         *  Parses elements from each line. The first item in the returned array
         *  is the field attribute and equal character (e.g. "a=", "c="), following
         *  elements are parsed from the string using space character as delimiter
         *
         *  @param aLine String containing line that ends to LF character
         *  @param aErrCode Error code that is used on leave situations, when
         *                  the contents of aLine are illegal
         *  @return Array that contains the division of string to elements
         */
        static RArray<TPtrC8> GetElementsFromLineL( 
            const TDesC8& aLine, TInt aErrCode );
    
        /**
         *  Parses elements from each line. The first item in the returned array
         *  is the field attribute and equal character (e.g. "a=", "c="), following
         *  elements are parsed from the string using aDelimiter as delimiter
         *
         *  @param aLine String containing line that ends to LF character
         *  @param aDelimiter Delimiter character
         *  @param aErrCode Error code that is used on leave situations, when
         *                  the contents of aLine are illegal
         *  @return Array that contains the division of string to elements
         */
        static RArray<TPtrC8> GetElementsFromLineL( 
            const TDesC8& aLine, TChar aDelimiter, TInt aErrCode ); 
            
        /**
         *  Parses elements from each line. The first item in the returned array
         *  is the field attribute and equal character (e.g. "a=", "c="), following
         *  elements are parsed from the string using space character as delimiter
         *
         *  @param aLexer Lexer that has been initialized with the string
         *  @param aErrCode Error code that is used on leave situations, when
         *                  the contents of aLine are illegal
         *  @return Array that contains the division of string to elements
         */       
        static RArray<TPtrC8> GetElementsFromLineL( 
            TLex8& aLexer, TChar aDelimiter, TInt aErrCode );
        
        /**
         *  Checks if the string contains only POS-DIGIT characters
         *
         *  @param aDes String
         *  @return Validity of POS-DIGIT string
         */
        static TBool IsPosDigit( const TDesC8& aDes );

        /**
         *  Checks if the string contains only DIGIT characters
         *
         *  @param aDes String
         *  @return Validity of DIGIT string
         */
        static TBool IsDigit( const TDesC8& aDes );

        /**
         *  Checks if the string contains valid characters
         *  
         *  @param aValidChars Valid characters allowed in the string
         *  @param aDes String
         *  @return Validity of the string
         */
        static TBool IsValidChars( const TDesC8& aValidChars, const TDesC8& aDes );                

        /**
         *  Checks if the string contains only token characters
         *
         *  @param aValue String
         *  @return Validity of the string
         */
        static TBool IsTokenChar( const TDesC8& aValue );

        /**
         *  Checks if the character is a valid token character
         *
         *  @param aChar Character
         *  @return Validity of the character
         */
		static TBool IsTokenChar( TChar aChar );

        /**
         *  Checks if the string is a valid token. The string can contain slash character,
         *  which divides the line into two tokens. The two tokens are then individually
         *  verified.
         *
         *  @param aValue String
         *  @return Validity of the string
         */
		static TBool IsTokenCharWithOptionalSlash( const TDesC8& aValue );

        /**
         *  Checks if the string is a valid token. The string can contain space characters,
         *  which divides the string into a number of tokens. Each token is then individually
         *  verified.
         *
         *  @param aValue String
         *  @return Validity of the string
         */
		static TBool IsTokenCharWithSpacesL( const TDesC8& aValue );

        /**
         *  Checks if the string is a valid token. The string can contain slash character,
         *  which divides the line into two tokens. The two tokens are then individually
         *  verified.
         *
         *  @param aValue String
         *  @return Validity of the string
         */
		static void EncodeBufferL( 
            const TDesC8& aValue, TInt aIndex, RWriteStream& aStream );

        /**
         *  Sets default network type ("IN") and default address type ("IP4" or "IP6")
         *  to variables given as parameters
         *  
         *  @param aPool String pool
         *  @param aInetAddr Internet address
         *  @param aNetType Network type
         *  @param aAddressType Address type
         */
        static void SetDefaultNetTypeAndAddrType( 
            RStringPool aPool, const TInetAddr& aAddress,
            RStringF& aNetType, RStringF& aAddressType );

        /**
         * Skips spaces until next line break, if the line break can be found.
         * @param aLexer
         * @return ETrue if the line break was found, otherwise EFalse. 
         */
        static TBool SkipSpacesUntilNextLineBreak( TLex8& aLexer );
 
    private:

        /**
         *  After the field tag has been parsed out, all the field elements are
         *  parsed in this method
         *
         *  @param aArray Reference to the array that contains the chopped elements
         *  @param aLexer Used lexer
         *  @param aDelimiter Delimiter character that determines the separation of elements
         *  @param aErrCode The error code that is used on leaves, when the argument is invalid
         */
        static void ChopElementsFromLineL( 
            RArray<TPtrC8>& aArray, TLex8& aLexer, TChar aDelimiter, TInt aErrCode );
        
        /**
         *  Private constructor         
         *  This class can never be instantiated
         */
        SdpUtil();
	};

#endif
