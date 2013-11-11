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
// Name          : SdpUtil.cpp
// Part of       : Local SDP Codec
// Version       : 1.0
//



#include <s32strm.h>
#include <in_sock.h>
#include "sdputil.h"
#include "sdpcodecstringconstants.h"
#include "sdpcodecstringpool.h"
#include "sdpcodecconstants.h"
#include "_sdpdefs.h"

// LOCAL CONSTANTS
const TInt KMaxAddressLength = 256;
_LIT8(KInvalidTokenCharacters, "\0\r\n\t ()<>@,;:");
_LIT8(KInvalidByteStringCharacters, "\0\r\n");
_LIT8(KWhitespaceCharacters, "\0\r\n \t");
_LIT16(KWildAddr, "0.0.0.0");
    
// -----------------------------------------------------------------------------
// SdpUtil::IsValidCharSet
// Generic checker to validate that string has only valid characters
// -----------------------------------------------------------------------------
//
TBool SdpUtil::IsValidCharSet(
    const TDesC8& aIllegalChars,
    const TDesC8& aValue,
    TInt aStart,
    TInt aEnd)
	{
	TBool result = EFalse;
	
	if ( aIllegalChars.Length() > 0 && aStart >= 0 && 
         aEnd <= aValue.Length() && aStart < aEnd )
		{
		TInt comp = KErrNotFound;
		while (comp == KErrNotFound && aStart < aEnd)
			{
			comp = aIllegalChars.Locate( aValue[aStart] );
			++aStart;
			}
		result = ( comp == KErrNotFound );
		}
	else 
		{
		result = ( ( aStart < aEnd ) && 
                   ( aIllegalChars.Length() == 0 || aValue.Length() == 0 ) );
		}

	return result;
	}

// -----------------------------------------------------------------------------
// SdpUtil::IsTokenList
// Checks that token list is valid
// -----------------------------------------------------------------------------
//
TBool SdpUtil::IsTokenList(
    const TDesC8& aValue )
	{	
	TBool atError = ( aValue.Length() == 0 );
    
    // Check that we are starting with valid character, not e.g. with space
    if ( !atError && 
         KInvalidTokenCharacters().Locate( aValue[0] ) != KErrNotFound )
        {
        atError = ETrue;
        }
    
    TLex8 lexer( aValue );
    // Go through the tokens to see that they are valid
    while ( !atError && !lexer.Eos() )
        {
        if ( !IsToken( lexer.NextToken() ) )
            {
            atError = ETrue;
            }   
        }

	return !atError;
	}

// -----------------------------------------------------------------------------
// SdpUtil::IsToken
// Checks if aValue is valid token
// -----------------------------------------------------------------------------
//
TBool SdpUtil::IsToken( 
    const TDesC8& aValue, 
    TInt aStart, 
    TInt aEnd)
	{
	return IsValidCharSet( KInvalidTokenCharacters, aValue, aStart, aEnd );
	}

// -----------------------------------------------------------------------------
// SdpUtil::IsToken
// Checks if aValue is valid token
// -----------------------------------------------------------------------------
//
TBool SdpUtil::IsToken(
    const TDesC8& aValue )
	{
	return IsValidCharSet( KInvalidTokenCharacters, aValue, 0, aValue.Length() );
	}

// -----------------------------------------------------------------------------
// SdpUtil::IsByteString
// Checks if aValue is valid byte string
// -----------------------------------------------------------------------------
//
TBool SdpUtil::IsByteString(
    const TDesC8& aValue, 
    TInt aStart, 
    TInt aEnd )
	{
	return IsValidCharSet( KInvalidByteStringCharacters, aValue, aStart, aEnd );
	}

// -----------------------------------------------------------------------------
// SdpUtil::IsByteString
// Checks if aValue is valid byte string
// -----------------------------------------------------------------------------
//
TBool SdpUtil::IsByteString(
    const TDesC8& aValue )
	{
	return IsValidCharSet( KInvalidByteStringCharacters, aValue, 0, 
                           aValue.Length() );
	}

// -----------------------------------------------------------------------------
// SdpUtil::IsTokenTextPair
// Checks if aValue is valid pair ("valid token":"valid byte-string")
// -----------------------------------------------------------------------------
//
TBool SdpUtil::IsTokenTextPair(
    const TDesC8& aValue, 
    TInt aStart,
    TInt aEnd )
	{
	// token ":" text
	TBool result = EFalse;
	if ( aStart >= 0 && aEnd <= aValue.Length() && aStart < aEnd )
		{
		TPtrC8 subVal = aValue.Mid( aStart, aEnd - aStart );
		TInt colonPos = subVal.Locate( KColonChar );
		result = colonPos != KErrNone &&
                 IsToken( subVal, 0, colonPos ) &&
                 IsByteString( subVal, colonPos + 1, subVal.Length() );
		}
	return result;
	}

// -----------------------------------------------------------------------------
// SdpUtil::IsTokenTextPair
// Checks if aValue is valid pair ("valid token":"valid byte-string")
// -----------------------------------------------------------------------------
//
TBool SdpUtil::IsTokenTextPair(
    const TDesC8& aValue )
	{
	return IsTokenTextPair( aValue, 0, aValue.Length() );
	}

// -----------------------------------------------------------------------------
// SdpUtil::IsNonWhitepce
// Checks if aValue contains only non-whitespace characters
// -----------------------------------------------------------------------------
//
TBool SdpUtil::IsNonWhitespace(
    const TDesC8& aValue )
	{
	return IsValidCharSet( KWhitespaceCharacters, aValue, 0, aValue.Length() );
	}

// -----------------------------------------------------------------------------
// SdpUtil::IsValidNtpTime
// Checks if aValue contains only non-whitespace characters
// -----------------------------------------------------------------------------
//
TBool SdpUtil::IsValidNtpTime(
    const TDesC8& aValue )
	{
	// POS-DIGIT 9*DIGIT
	TUint len = aValue.Length();
	TBool result = len >= 10 && TChar( aValue[0] ) != TChar('0');
	if ( result )
		{
		for ( TUint i( 0 ); result && i < len; i++)
			{
			TInt tmp = aValue[i] - '0';
			result = ( tmp >= 0 && tmp <= 9 );
			}
		}
	return result;
	}

// -----------------------------------------------------------------------------
// SdpUtil::DivideToLinesL
// Divides descriptor into a number of lines that are separated by
// CRLF or LF marks
// -----------------------------------------------------------------------------
//
RArray<TPtrC8> SdpUtil::DivideToLinesL( 
    const TDesC8& aLines,
    TInt aErrCode )
    {
    RArray<TPtrC8> lineArray;
    CleanupClosePushL( lineArray );
    
    TInt lineStartPos( 0 );
    TInt lineEndPos( aLines.Length() );
    TInt pos( 0 );
    
    TPtrC8 parsedPart( aLines.Right( lineEndPos - lineStartPos ) );
    // Splits aLines to number of TPtrC8s, each ending to CRLF/LF
    while ( lineStartPos < lineEndPos && 
           ( pos = parsedPart.Find( KLFStr ) ) != KErrNotFound )
        {  
        if ( ! ( pos == 0 || 
                ( pos == ( KCRLFStr().Length() - 1 ) && 
                  parsedPart.Find( KCRLFStr ) == 0 ) ) )
            {
            // Lines that contain only CRLF or LF are ignored
            // Put the valid lines to the array
            User::LeaveIfError( 
                lineArray.Append( 
                    TPtrC8( parsedPart.Left( pos + KLFStr().Length() ) ) ) );
            }
        // Advance the start of the line after LF mark
        lineStartPos += pos + KLFStr().Length();
        parsedPart.Set( aLines.Right( lineEndPos - lineStartPos ) );
        }    
    
    // This method expects that last line of aLines ends _always_ 
    // to LF not to '\0'
    if ( aLines.Length() == 0 ||
        ( aLines.Length() > 0 && aLines[aLines.Length() - 1] != '\n' ) )
        {
        User::Leave( aErrCode );
        }

    CleanupStack::Pop();    // lineArray
    return lineArray;    
    }

// -----------------------------------------------------------------------------
// SdpUtil::GetElementsFromLineL
// Gets all the elements from a single line
// -----------------------------------------------------------------------------
//
RArray<TPtrC8> SdpUtil::GetElementsFromLineL(
    const TDesC8& aLine, 
    TInt aErrCode )
    {
    TLex8 lexer( aLine );
    RArray<TPtrC8> lineArray = 
        SdpUtil::GetElementsFromLineL( lexer, KSPChar, aErrCode );    
    return lineArray;
    }

// -----------------------------------------------------------------------------
// SdpUtil::GetElementsFromLineL
// Gets all the elements from a single line
// -----------------------------------------------------------------------------
//
RArray<TPtrC8> SdpUtil::GetElementsFromLineL( 
    const TDesC8& aLine, 
    TChar aDelimiter,
    TInt aErrCode )
    {
    TLex8 lexer( aLine );
    RArray<TPtrC8> lineArray = 
        SdpUtil::GetElementsFromLineL( lexer, aDelimiter, aErrCode );
    return lineArray;
    }

// -----------------------------------------------------------------------------
// SdpUtil::GetElementsFromLineL
// Gets all the elements from a single line
// -----------------------------------------------------------------------------
//
RArray<TPtrC8> SdpUtil::GetElementsFromLineL( 
    TLex8& aLexer, 
    TChar aDelimiter,
    TInt aErrCode )
    {
    RArray<TPtrC8> lineArray;    
    CleanupClosePushL( lineArray );
   
    // Header is special case, because it is joined _without_
    // space character with the first attribute, so it must be
    // parsed separately        
            
    aLexer.Mark();
    // curChar must be != KEqualChar at first comparison
    for ( TChar curChar( KSPChar ); curChar != KEqualChar; )
        {
        curChar = aLexer.Peek();
        // Syntax check      
        if ( curChar == KEofChar || curChar == KLFChar ||
             curChar == KCRChar )
            {
            User::Leave( aErrCode );
            }        
        aLexer.Inc();       
        }
    // Append header to array
    User::LeaveIfError( lineArray.Append( aLexer.MarkedToken() ) );
    
    // Whitespace MUST NOT be used after the "=" sign
    if (aLexer.Peek() == KSPChar) 
	    {
		User::Leave( aErrCode );	    	 
	    }

    // Get the other elements from the string to the array
    ChopElementsFromLineL( lineArray, aLexer, aDelimiter, aErrCode );

    CleanupStack::Pop();    // lineArray
    return lineArray;
    }

// -----------------------------------------------------------------------------
// SdpUtil::IsPosDigit
// Checks if all the charcaters on the descriptor are POS-DIGITs
// -----------------------------------------------------------------------------
//
TBool SdpUtil::IsPosDigit( 
    const TDesC8& aDes )
    {
    // POS-DIGIT = %x31-39 ; 1 - 9
    _LIT8( KValidPosDigits, "123456789" );
    return SdpUtil::IsValidChars( KValidPosDigits, aDes );
    }

// -----------------------------------------------------------------------------
// SdpUtil::IsDigit
// Checks if all the charcaters on the descriptor are DIGITs
// -----------------------------------------------------------------------------
//
TBool SdpUtil::IsDigit( 
    const TDesC8& aDes )
    {
    // DIGIT = "0" / "1" / "2" / "3" / "4" / "5" / "6" /
    //         "7" / "8" / "9"
    _LIT8( KValidDigits, "0123456789" );

    return SdpUtil::IsValidChars( KValidDigits, aDes );
    }

// -----------------------------------------------------------------------------
// SdpUtil::IsValidChars
// Checks if all the charcaters on the descriptor are from valid charset
// -----------------------------------------------------------------------------
//
TBool SdpUtil::IsValidChars( 
    const TDesC8& aValidChars, 
    const TDesC8& aDes )
    {
    TBool valid( aDes.Length() > 0 );
    for ( TInt i( 0 ); i < aDes.Length() && valid; i++ )
        {
        if ( aValidChars.Locate( aDes[i] ) == KErrNotFound )
            {
            valid = EFalse;
            }
        }

    return valid;
    }
        
// -----------------------------------------------------------------------------
// SdpUtil::IsTokenCharWithSpacesL
// Checks if all the elements on the string are valid tokens
// -----------------------------------------------------------------------------
//
TBool SdpUtil::IsTokenCharWithSpacesL(
    const TDesC8& aValue )
	{
	TLex8 lex( aValue );
	lex.SkipSpace();

	if( lex.Remainder().Length() == 0 )
		{
		User::Leave( KErrArgument );
		}

	while ( !lex.Eos() )
		{
		if ( !IsTokenChar( lex.NextToken() ) )
			{
			return EFalse;
			}
		lex.SkipSpace();
		}
	return ETrue;
	}

// -----------------------------------------------------------------------------
// SdpUtil::IsTokenCharWithOptionalSlash
// Checks if all the possible two elements divided by slash are valid tokens
// -----------------------------------------------------------------------------
//
TBool SdpUtil::IsTokenCharWithOptionalSlash(const TDesC8& aValue)
	{
	TInt lineEndPosition = aValue.Locate('/');

	if ( lineEndPosition != KErrNotFound )
		{
		TPtrC8 firstToken( aValue.Left( lineEndPosition ) );
		if( !IsTokenChar( firstToken ) ||
		    !IsTokenChar( aValue.Mid( lineEndPosition + 1 ) ) )
			{
			return EFalse;
			}
		}
	else
		{
		return IsTokenChar( aValue );
		}
	return ETrue;
	}

// -----------------------------------------------------------------------------
// SdpUtil::IsTokenChar
// Checks if all the elements on the string are valid tokens
// -----------------------------------------------------------------------------
//
TBool SdpUtil::IsTokenChar(
    const TDesC8& aValue )
	{	
    TLex8 lex( aValue );
	while( !lex.Eos() )
		{
		TChar ch = lex.Get();
		lex.Inc();

		if( ch == '\r' ||ch == '\n' || ch == '\0' || !IsTokenChar( ch ) )
			{
			return EFalse;
			}
		}
	return ETrue;
	}

// -----------------------------------------------------------------------------
// SdpUtil::IsTokenChar
// Checks if the character is a valid token character
// -----------------------------------------------------------------------------
//
TBool SdpUtil::IsTokenChar( 
    TChar aChar )
	{
    return ( ( aChar == '!')||
	         ( aChar >= '#' && aChar <= '\'' ) ||
	         ( aChar == '*' || aChar == '+' ) ||
	         ( aChar == '-' || aChar == '.' ) ||
	         ( aChar.IsDigit() ) ||
	         ( aChar >= 'a' && aChar <= 'z' ) ||
	         ( aChar >= 'A' && aChar <= 'Z' ) ||
	         ( aChar >= '^' && aChar <= '~' ) );	
	}

// ---------------------------------------------------------------------------
// SdpUtil::EncodeBufferL
// Encodes headername and a value to the stream
// ---------------------------------------------------------------------------
//
void SdpUtil::EncodeBufferL(
    const TDesC8& aValue, 
    TInt aIndex,
    RWriteStream& aStream )
	{
	if( aValue.Length() != 0 )
		{
        RStringPool pool = SdpCodecStringPool::StringPoolL();
		RStringF headername = pool.StringF( aIndex, 
                                        SdpCodecStringPool::StringTableL() );
		aStream.WriteL( headername.DesC() );
		aStream.WriteL( aValue );
		aStream.WriteL( KCRLFStr );
		}
	}

// ---------------------------------------------------------------------------
// SdpUtil::SetDefaultNetTypeAndAddrType
// Sets network type and address type to their "default" values
// ---------------------------------------------------------------------------
//
void SdpUtil::SetDefaultNetTypeAndAddrType( 
    RStringPool aPool, 
    const TInetAddr& aAddress,
    RStringF& aNetType, 
    RStringF& aAddressType )
    {
     // Sets network type to IN
    aNetType.Close();
    aNetType = aPool.StringF( SdpCodecStringConstants::ENetType,
                              SdpCodecStringConstants::Table ).Copy();
    // Address type       
    aAddressType.Close();

    TBuf16 <KMaxAddressLength> output;
    aAddress.Output(output);


    //addresstype for IPv4    
	if((aAddress.Address() &&  !aAddress.IsV4Mapped()) ||
       (!aAddress.Address() && aAddress.IsWildAddr() && 
        output.Match(KWildAddr) == 0))
        {
        aAddressType = aPool.StringF( SdpCodecStringConstants::EAddressTypeIP4,
                                      SdpCodecStringConstants::Table ).Copy();
        }
    else
        {
		//addresstype for IPv4-Mapped IPv6 && IPv6
        aAddressType = aPool.StringF( SdpCodecStringConstants::EAddressType,
                                      SdpCodecStringConstants::Table ).Copy();
        }
    }

// ---------------------------------------------------------------------------
// SdpUtil::SkipSpacesUntilNextLineBreak
// Skips spaces until next line break, if the line break can be found.
// Examples: "   \r\n" -> "\r\n", "abc def\r\n" -> "abc def\r\n"
// ---------------------------------------------------------------------------
//       
TBool SdpUtil::SkipSpacesUntilNextLineBreak( TLex8& aLexer )
    {
    TBool found = EFalse;
    if ( aLexer.Peek() == KSPChar )
        {
        TInt spaceCount = 0;
        while ( aLexer.Peek() == KSPChar )
            {
            spaceCount++;
            aLexer.Get();
            }
        if ( aLexer.Peek() != KCRChar && aLexer.Peek() != KLFChar )
            {
            for ( TInt i=0; i < spaceCount; i++ )
                {
                aLexer.UnGet();
                }
            }
        else
            {
            found = ETrue;
            }
        }
    return found;
    }

// ---------------------------------------------------------------------------
// SdpUtil::ChopElementsFromLineL
// Chops all the elements that are divided by delimiter from the string
// ---------------------------------------------------------------------------
//
void SdpUtil::ChopElementsFromLineL( 
    RArray<TPtrC8>& aArray, 
    TLex8& aLexer,
    TChar aDelimiter,
    TInt aErrCode )
    {
    // Chop the elements to the array from lexer
    TBool eofcFound = EFalse;
    while (!eofcFound)
        {
        aLexer.Mark();        
        // Parse single token, leave other special characters
        // to token except aDelimiter | '\r' | '\n' | '\0'
        while ( aLexer.Peek() != aDelimiter && 
                aLexer.Peek() != KCRChar &&
                aLexer.Peek() != KLFChar && 
                aLexer.Peek() != KEofChar )
            {
            aLexer.Inc();
            }
		
        if ( aLexer.MarkedToken().Length() > 0 )
            {            
            aArray.AppendL( aLexer.MarkedToken() );
            }

        if ( aDelimiter == KSPChar &&
             aLexer.Peek() == aDelimiter )
            {
            SkipSpacesUntilNextLineBreak( aLexer );
            }

        // Check if its end-of-line
        if ( aLexer.Peek() == KCRChar )
            {
            aLexer.Inc();
            }

        if ( aLexer.Peek() == KLFChar )
            {
            aLexer.Inc();
            if ( aLexer.Peek() != KEofChar )
                {
                User::Leave( aErrCode );
                }
            eofcFound = ETrue;
            }
        else if ( aLexer.Peek() == KEofChar )
            {
            // EoF character not tolerated at the middle of the string
            User::Leave( aErrCode );
            }
        else
            {
            aLexer.Inc();
            }
        }
    }
