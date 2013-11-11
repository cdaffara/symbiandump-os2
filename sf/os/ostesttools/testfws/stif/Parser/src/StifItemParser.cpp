/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: This module contains implementation of CStifItemParser 
* class member functions.
*
*/

// INCLUDE FILES
#include <e32std.h>
#include "StifItemParser.h"

// EXTERNAL DATA STRUCTURES
// None

// EXTERNAL FUNCTION PROTOTYPES
// None

// CONSTANTS
// None

// MACROS
// None

// LOCAL CONSTANTS AND MACROS
// None

// MODULE DATA STRUCTURES
// None

// LOCAL FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
// None

// ==================== LOCAL FUNCTIONS =======================================
// None

/*
-------------------------------------------------------------------------------

    DESCRIPTION

    This module contains implementation of CStifItemParser class member
    functions.

-------------------------------------------------------------------------------
*/

// ================= MEMBER FUNCTIONS =========================================

/*
-------------------------------------------------------------------------------

    Class: CStifItemParser

    Method: CStifItemParser

    Description: Default constructor

    C++ default constructor can NOT contain any code, that
    might leave.
    
    Parameters: TPtrC aSection: in: Parsed section
                TInt aStartPos: in: Start point of parsing
                TInt aLength: in: Length of parsed section
    
    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
CStifItemParser::CStifItemParser( TPtrC aSection,
                                    TInt aStartPos,
                                    TInt aLength ) :
    iItemLineSection( aSection.Mid( aStartPos, aLength ) ),
    iItemSkipAndMarkPos( 0 ),
    iGetMethodsIndicator( EFalse ),
    iParsingType( CStifItemParser::ENormalParsing ) // Mode ON
    {

    }

/*
-------------------------------------------------------------------------------

    Class: CStifItemParser

    Method: ConstructL

    Description: Symbian OS second phase constructor

    Symbian OS default constructor can leave.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
void CStifItemParser::ConstructL()
    {

    }

/*
-------------------------------------------------------------------------------

    Class: CStifItemParser

    Method: NewL

    Description: Two-phased constructor.

    Parameters: TPtrC aSection: in: Parsed section
                TInt aStartPos: in: Start point of parsing
                TInt aLength: in: Length of parsed section

    Return Values: CStifItemParser*: pointer to CStifItemParser object

    Errors/Exceptions:  Leaves if memory allocation fails
                        Leaves if ConstructL leaves

    Status: Approved

-------------------------------------------------------------------------------
*/
EXPORT_C CStifItemParser* CStifItemParser::NewL( TPtrC aSection,
                                                    TInt aStartPos,
                                                    TInt aLength )
    {
    CStifItemParser* item = new (ELeave) CStifItemParser( aSection, aStartPos,
                                                            aLength );

    CleanupStack::PushL( item );
    item->ConstructL();
    CleanupStack::Pop( item );

    return item;

    }

/*
-------------------------------------------------------------------------------

    Class: CStifItemParser

    Method: ~CStifItemParser

    Description: Destructor

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/    
CStifItemParser::~CStifItemParser()
    {

    }

/*
-------------------------------------------------------------------------------

    Class: CStifItemParser

    Method: ParseStartAndEndPos

    Description: Start and end position parser.
                 Handles also quote feature parsing. EQuoteStyleParsing
                 option removes correctly setted quotes(" and ").

    Parameters: TPtrC aSection: in: Parsed section
                const TDesC& aStartTag: in: Start tag of parsing
                TInt& aStartPos: inout: Start point of parsing
                TInt& aEndPos: inout: End point of parsing
                TInt& aLength: inout: Length of parsed section
                TInt& aExtraEndPos: inout: Used if end position needs to
                                           set, e.g. used in quote cases.

    Return Values: TInt: Error code

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/

TInt CStifItemParser::ParseStartAndEndPos( TPtrC aSection,
                                        const TDesC& aStartTag,
                                        TInt& aStartPos,
                                        TInt& aEndPos,
                                        TInt& aLength,
                                        TInt& aExtraEndPos )
    {
    aStartPos = 0;
    aEndPos = 0;
    aExtraEndPos = 0; // Normally this is 0    
    
    RBuf tag( aStartTag.AllocL() );
    // Remove whitespaces at the begining and end of the tag
    tag.Trim();
    
    // Create parser
    TLex lex( aSection );
    // Move parsing pointer to position from which parsing will start 
    lex.SkipAndMark( iItemSkipAndMarkPos );
    
    TBool tagWithEqualMark = EFalse;
    
    // Check if tag is defined
    if ( tag.Length() != 0 )
    	{
    	// Check if tag contains '=' character at the end.
    	// If tag is "=" then we treat it as normal tag (not as tag with '=' 
    	// character).
        if ( ( tag.Length() > 1 ) && ( tag.Right( 1 ) == _L("=") ) )
        	{
        	// Tag contains '=' character. Remove '=' so only clean tag remains. 
        	tagWithEqualMark = ETrue;
        	tag.SetLength( tag.Length() - 1 );
        	tag.Trim();
        	}

		// Search for specified tag in parsed text
		while( !lex.Eos() )
			{
			lex.SkipSpaceAndMark();
			TPtrC token = lex.NextToken();
			if ( tagWithEqualMark ) 
				{
				if ( ( token == tag ) && ( !lex.Eos() ) ) // "tag = val" or "tag =val" 
					{
					lex.SkipSpaceAndMark();
					TPtrC val = lex.NextToken();
					lex.UnGetToMark();
					if ( val == _L("=") ) // "tag = val"
						{
						lex.Inc();
						break;
						}
					else	// "tag =val"
						{
						if ( val.Left( 1 ) == _L("=") )
							{
							// Skip "=" character.
							lex.Inc();
							break;
							}
						else // We didn't find any variation of "tag="
							{
							lex.UnGetToMark();
							}
						}
					}
				else // "tag=val" or "tag= val"
					{
					// It handles both situations
					if ( ( token.Find( tag ) == 0 ) && ( token.Find( _L("=") ) == tag.Length() ) )
						{
						lex.UnGetToMark();
						lex.SkipAndMark( tag.Length() + 1 );
						lex.SkipSpace();
						break;
						}
					}
				}
			else
				{
				// Tag without '=' character. Just compare.
    			if ( token == tag )
    				{
    				break;
    				}
				}
			}
		
		// Check if we are at the end of parsed text
		if ( lex.Eos() )
			{
			// We are at the end of parsed text. Eaven if we found tag its value
			// is not defined.
			tag.Close();
			return KErrNotFound;
			}
    		    	
    	}
    
    tag.Close();       

	if( iParsingType == CStifItemParser::ENormalParsing )
		{
        TPtrC ptr = lex.NextToken();
        aEndPos = lex.Offset();             // End position
        aStartPos = aEndPos - ptr.Length(); // Start position    		
		}
	else
		{
		lex.SkipSpaceAndMark();		
		TPtrC token = lex.NextToken();
		lex.UnGetToMark();
		
		if ( token.Length() == 0 )
			{
			return KErrNotFound;
			}
		
		if ( token.Left( 1 ) == _L("\"") )
			{
			lex.Inc();
			aStartPos = lex.Offset();
			
			TBool foundEndingQuoteMark = false;
			while( !lex.Eos() )
				{
				if ( lex.Get() == '"' ) 
					{
					foundEndingQuoteMark = ETrue;
					break;;
					}
				}

			if ( foundEndingQuoteMark )
				{
				aEndPos = lex.Offset() - 1;
				if ( aEndPos > aStartPos )
					{
					aExtraEndPos = 1;
					}
				else
					{
					aStartPos--;
					aEndPos++;
					}
				}
			else
				{
				lex.UnGetToMark();
				aStartPos = lex.Offset();
	            TPtrC ptr = lex.NextToken();
	            aEndPos = lex.Offset();             // End position	            
				}			
			}
		else
			{
            TPtrC ptr = lex.NextToken();
            aEndPos = lex.Offset();             // End position
            aStartPos = aEndPos - ptr.Length(); // Start position    		
			}
		}

    aLength = ( aEndPos - aStartPos );      
    
    // aStartPos is negative (0 is allowed) or length is negative
    if( ( aLength <= 0 ) && ( iParsingType == CStifItemParser::ENormalParsing ) )
        {
        return KErrNotFound;
        }
    
    return KErrNone;    
    }

/*
-------------------------------------------------------------------------------

    Class: CStifItemParser

    Method: GetString

    Description: Get a string.

    Search an item from the section and return everything after the item until
    separator cut the parsing.
    If start tag is empty the first string will be parsed and returned.

    Parameters: const TDesC& aTag: in: Indicates parsing start point
                TPtrC& aString: inout: Parsed string

    Return Values: TInt: Error code

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
EXPORT_C TInt CStifItemParser::GetString( const TDesC& aTag, TPtrC& aString )
    {
    if ( 0 == iItemLineSection.Length() )
        {
        // Probably parsering is failed
        return KErrNotFound;
        }

    TInt startPos( 0 );
    TInt endPos( 0 );
    TInt length( 0 );
    TInt extraEndPos( 0 );

    iItemSkipAndMarkPos = 0;
    TInt ret = ParseStartAndEndPos( iItemLineSection, aTag, startPos,
                                    endPos, length, extraEndPos );
    if ( KErrNone != ret)
        {
        // Nothing to parse
        return ret;
        }

    // Current position for forward parsing operations
    iItemSkipAndMarkPos = endPos + extraEndPos;

    // Take string
    aString.Set( (TUint16*)&iItemLineSection[startPos], length );

    // Indicator that GetString has been used
    iGetMethodsIndicator = ETrue;

    return KErrNone;

    }

/*
-------------------------------------------------------------------------------

    Class: CStifItemParser

    Method: GetNextString

    Description: Get a next string.

    Get next string until separator cut the parsing. GetString or GetInt or
    GetChar must be called before call GetNextString method.

    Parameters: TPtrC& aString: inout: Parsed string
    
    Return Values: TInt: Error code

    Errors/Exceptions: None

    Status: Approved
    
-------------------------------------------------------------------------------
*/
EXPORT_C TInt CStifItemParser::GetNextString( TPtrC& aString )
    {
    // GetString(), GetInt() or GetChar() is not called
    if ( !iGetMethodsIndicator )
        {
        return KErrNotReady;
        }

    if ( 0 == iItemLineSection.Length() )
        {
        // Probably parsering is failed
        return KErrNotFound;
        }

    TInt startPos( 0 );
    TInt endPos( 0 );
    TInt length( 0 );
    TInt extraEndPos( 0 );
    TInt ret = ParseStartAndEndPos( iItemLineSection, KNullDesC, startPos,
                                    endPos, length, extraEndPos );
    if ( KErrNone != ret)
        {
        // Nothing to parse
        return ret;
        }

    // Current position for forward parsing operations
    iItemSkipAndMarkPos = endPos + extraEndPos;

    // Take string
    aString.Set( (TUint16*)&iItemLineSection[startPos], length );

    return KErrNone;

    }

/*
-------------------------------------------------------------------------------

    Class: CStifItemParser

    Method: GetNextString

    Description: Get a next string with tag.

    Get next string with tag until separator cut the parsing. If start tag is
    empty the next string will be parsed and returned.
    GetString or GetInt or GetChar must be called before call GetNextString
    method.

    Parameters: const TDesC& aTag: in: Indicates parsing start point
                TPtrC& aString: inout: Parsed string

    Return Values: TInt: Error code

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
EXPORT_C TInt CStifItemParser::GetNextString( const TDesC& aTag,
                                                TPtrC& aString )
    {
    // GetString(), GetInt() or GetChar() is not called
    if ( !iGetMethodsIndicator )
        {
        return KErrNotReady;
        }

    if ( 0 == iItemLineSection.Length() )
        {
        // Probably parsering is failed
        return KErrNotFound;
        }

    TInt startPos( 0 );
    TInt endPos( 0 );
    TInt length( 0 );
    TInt extraEndPos( 0 );
    TInt ret = ParseStartAndEndPos( iItemLineSection, aTag, startPos,
                                    endPos, length, extraEndPos );
    if ( KErrNone != ret)
        {
        // Nothing to parse
        return ret;
        }

    // Current position for forward parsing operations
    iItemSkipAndMarkPos = endPos + extraEndPos;

    // Take string
    aString.Set( (TUint16*)&iItemLineSection[startPos], length );

    return KErrNone;

    }

/*
-------------------------------------------------------------------------------

    Class: CStifItemParser

    Method: GetInt

    Description: Get a integer.

    Search an item from the section and return integer after the item until 
    separator cut the parsing.
    If start tag is empty the first integer will be parsed and returned.

    Parameters: const TDesC& aTag: in: Indicates parsing start point
                TInt& aInteger: inout: Parsed integer

    Return Values: TInt: Error code

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
EXPORT_C TInt CStifItemParser::GetInt( const TDesC& aTag, TInt& aInteger )
    {
    if ( 0 == iItemLineSection.Length() )
        {
        // Probably parsering is failed
        return KErrNotFound;
        }

    TInt startPos( 0 );
    TInt endPos( 0 );
    TInt length( 0 );
    TInt extraEndPos( 0 );

    iItemSkipAndMarkPos = 0;
    TInt ret = ParseStartAndEndPos( iItemLineSection, aTag, startPos,
                                    endPos, length, extraEndPos );
    if ( KErrNone != ret )
        {
        // Nothing to parse
        return ret;
        }

    // Current position for forward parsing operations
    iItemSkipAndMarkPos = endPos;

    // Indicator that GetInt has been used
    iGetMethodsIndicator = ETrue;

    // Take integer
    TLex lex( iItemLineSection );
    // Mark to current position of parsing
    lex.SkipAndMark( startPos );
    lex.SkipSpace();
    return lex.Val( aInteger );

    }

/*
-------------------------------------------------------------------------------

    Class: CStifItemParser

    Method: GetNextInt

    Description: Get a next integer.

    Get next integer until separator cut the parsing and returns it.
    GetString or GetInt or GetChar must be called before call GetNextInt
    method.

    Parameters: TInt& aInteger: inout: Parsed integer

    Return Values: TInt: Error code

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
EXPORT_C TInt CStifItemParser::GetNextInt( TInt& aInteger )
    {
    // GetString(), GetInt() or GetChar() is not called
    if ( !iGetMethodsIndicator )
        {
        return KErrNotReady;
        }
    if ( 0 == iItemLineSection.Length() )
        {
        // Probably parsering is failed
        return KErrNotFound;
        }

    TInt startPos( 0 );
    TInt endPos( 0 );
    TInt length( 0 );
    TInt extraEndPos( 0 );
    TInt ret = ParseStartAndEndPos( iItemLineSection, KNullDesC, startPos,
                                    endPos, length, extraEndPos );
    if ( KErrNone != ret )
        {
        // Nothing to parse
        return ret;
        }

    // Current position for forward parsing operations
    iItemSkipAndMarkPos = endPos;

    // Take integer
    TLex lex( iItemLineSection );
    // Mark to current position of parsing
    lex.SkipAndMark( startPos );
    lex.SkipSpace();
    return lex.Val( aInteger );

    }

/*
-------------------------------------------------------------------------------

    Class: CStifItemParser

    Method: GetNextInt

    Description: Get a next integer with tag.

    Get next integer with tag until separator cut the parsing. If start tag is
    empty the next integer will be parsed and returned.
    GetString or GetInt or GetChar must be called before call GetNextInt
    method.

    Parameters: const TDesC& aTag: in: Indicates parsing start point
                TInt& aInteger: inout: Parsed integer

    Return Values: TInt: Error code

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
EXPORT_C TInt CStifItemParser::GetNextInt( const TDesC& aTag, TInt& aInteger )
    {
    // GetString(), GetInt() or GetChar() is not called
    if ( !iGetMethodsIndicator )
        {
        return KErrNotReady;
        }
    if ( 0 == iItemLineSection.Length() )
        {
        // Probably parsering is failed
        return KErrNotFound;
        }

    TInt startPos( 0 );
    TInt endPos( 0 );
    TInt length( 0 );
    TInt extraEndPos( 0 );
    TInt ret = ParseStartAndEndPos( iItemLineSection, aTag, startPos,
                                    endPos, length, extraEndPos );
    if ( KErrNone != ret )
        {
        // Nothing to parse
        return ret;
        }

    // Current position for forward parsing operations
    iItemSkipAndMarkPos = endPos;

    // Take integer
    TLex lex( iItemLineSection );
    // Mark to current position of parsing
    lex.SkipAndMark( startPos );
    lex.SkipSpace();
    return lex.Val( aInteger );

    }

/*
-------------------------------------------------------------------------------

    Class: CStifItemParser

    Method: GetInt

    Description: Get a integer.

    Search an item from the section and return integer after the item until 
    separator cut the parsing.
    If start tag is empty the first integer will be parsed and returned.

    Parameters: const TDesC& aTag: in: Indicates parsing start point
                TUint& aInteger: inout: Parsed integer
                TRadix aRadix: in: Convert a number into different
                presentation. For default: EDecimal.

    Return Values: TInt: Error code

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
EXPORT_C TInt CStifItemParser::GetInt( const TDesC& aTag,
                                        TUint& aInteger,
                                        TRadix aRadix )
    {
    if ( 0 == iItemLineSection.Length() )
        {
        // Probably parsering is failed
        return KErrNotFound;
        }

    TInt startPos( 0 );
    TInt endPos( 0 );
    TInt length( 0 );
    TInt extraEndPos( 0 );

    iItemSkipAndMarkPos = 0;
    TInt ret = ParseStartAndEndPos( iItemLineSection, aTag, startPos,
                                    endPos, length, extraEndPos );
    if ( KErrNone != ret )
        {
        // Nothing to parse
        return ret;
        }

    // Current position for forward parsing operations
    iItemSkipAndMarkPos = endPos;

    // Indicator that GetInt has been used
    iGetMethodsIndicator = ETrue;

    // Take integer
    TLex lex( iItemLineSection );
    // Mark to current position of parsing
    lex.SkipAndMark( startPos );
    lex.SkipSpace();
    // NOTE: If parsed integer is e.g. '-1' and aRadix is EDesimal the lex will
    // return error code -2(KErrGeneral). Main reason is for this new TUint
    // method that cast operation does not work in above case and -2 returned.
    // In this case use GetInt method that reference type is TInt.
    return lex.Val( aInteger, aRadix );

    }

/*
-------------------------------------------------------------------------------

    Class: CStifItemParser

    Method: GetNextInt

    Description: Get a next integer.

    Get next integer until separator cut the parsing and returns it.
    GetString or GetInt or GetChar must be called before call GetNextInt
    method.

    Parameters: TUint& aInteger: inout: Parsed integer
                TRadix aRadix: in: Convert a number into different
                presentation. For default: EDecimal.

    Return Values: TInt: Error code

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
EXPORT_C TInt CStifItemParser::GetNextInt( TUint& aInteger,
                                            TRadix aRadix )
    {
    // GetString(), GetInt() or GetChar() is not called
    if ( !iGetMethodsIndicator )
        {
        return KErrNotReady;
        }
    if ( 0 == iItemLineSection.Length() )
        {
        // Probably parsering is failed
        return KErrNotFound;
        }

    TInt startPos( 0 );
    TInt endPos( 0 );
    TInt length( 0 );
    TInt extraEndPos( 0 );
    TInt ret = ParseStartAndEndPos( iItemLineSection, KNullDesC, startPos,
                                    endPos, length, extraEndPos );
    if ( KErrNone != ret )
        {
        // Nothing to parse
        return ret;
        }

    // Current position for forward parsing operations
    iItemSkipAndMarkPos = endPos;

    // Take integer
    TLex lex( iItemLineSection );
    // Mark to current position of parsing
    lex.SkipAndMark( startPos );
    lex.SkipSpace();
    // NOTE: If parsed integer is e.g. '-1' and aRadix is EDesimal the lex will
    // return error code -2(KErrGeneral). Main reason is for this new TUint
    // method that cast operation does not work in above case and -2 returned.
    // In this case use GetNextInt method that reference type is TInt.
    return lex.Val( aInteger, aRadix );

    }

/*
-------------------------------------------------------------------------------

    Class: CStifItemParser

    Method: GetNextInt

    Description: Get a next integer with tag.

    Get next integer with tag until separator cut the parsing. If start tag is
    empty the next integer will be parsed and returned.
    GetString or GetInt or GetChar must be called before call GetNextInt
    method.

    Parameters: const TDesC& aTag: in: Indicates parsing start point
                TUint& aInteger: inout: Parsed integer
                TRadix aRadix: in: Convert a number into different
                presentation. For default: EDecimal.

    Return Values: TInt: Error code

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
EXPORT_C TInt CStifItemParser::GetNextInt( const TDesC& aTag,
                                            TUint& aInteger,
                                            TRadix aRadix )
    {
    // GetString(), GetInt() or GetChar() is not called
    if ( !iGetMethodsIndicator )
        {
        return KErrNotReady;
        }
    if ( 0 == iItemLineSection.Length() )
        {
        // Probably parsering is failed
        return KErrNotFound;
        }

    TInt startPos( 0 );
    TInt endPos( 0 );
    TInt length( 0 );
    TInt extraEndPos( 0 );
    TInt ret = ParseStartAndEndPos( iItemLineSection, aTag, startPos,
                                    endPos, length, extraEndPos );
    if ( KErrNone != ret )
        {
        // Nothing to parse
        return ret;
        }

    // Current position for forward parsing operations
    iItemSkipAndMarkPos = endPos;

    // Take integer
    TLex lex( iItemLineSection );
    // Mark to current position of parsing
    lex.SkipAndMark( startPos );
    lex.SkipSpace();
    // NOTE: If parsed integer is e.g. '-1' and aRadix is EDesimal the lex will
    // return error code -2(KErrGeneral). Main reason is for this new TUint
    // method that cast operation does not work in above case and -2 returned.
    // In this case use GetNextInt method that reference type is TInt.
    return lex.Val( aInteger, aRadix );

    }

/*
-------------------------------------------------------------------------------

    Class: CStifItemParser

    Method: GetChar

    Description: Get a character.

    Search an item from the section and return character after the item.
    If start tag is empty the first character will be parsed and returned.

    Parameters: const TDesC& aTag: in: Indicates parsing start point
                TChar& aCharacter: inout: Parsed character

    Return Values: TInt: Error code

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
EXPORT_C TInt CStifItemParser::GetChar( const TDesC& aTag, TChar& aCharacter )
    {
    if ( 0 == iItemLineSection.Length() )
        {
        // Probably parsering is failed
        return KErrNotFound;
        }

    TInt startPos( 0 );
    TInt endPos( 0 );
    TInt length( 0 );
    TInt extraEndPos( 0 );

    iItemSkipAndMarkPos = 0;
    TInt ret = ParseStartAndEndPos( iItemLineSection, aTag, startPos,
                                    endPos, length, extraEndPos );
    if ( KErrNone != ret )
        {
        // Nothing to parse
        return ret;
        }

    // Current position for forward parsing operations
    iItemSkipAndMarkPos = startPos + 1;

    // Take character
    aCharacter = iItemLineSection[startPos];

    // Indicator that GetChar has been used
    iGetMethodsIndicator = ETrue;

    return KErrNone;

    }

/*
-------------------------------------------------------------------------------

    Class: CStifItemParser

    Method: GetNextChar

    Description: Get a next character.

    Get next character and returns it.
    GetString or GetInt or GetChar must be called before call GetNextChar
    method.

    Parameters: TChar& aCharacter: inout: Parsed character

    Return Values: TInt: Error code

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
EXPORT_C TInt CStifItemParser::GetNextChar( TChar& aCharacter )
    {
    // GetString(), GetInt() or GetChar() is not called
    if ( !iGetMethodsIndicator )
        {
        return KErrNotReady;
        }
    if ( 0 == iItemLineSection.Length() )
        {
        // Probably parsering is failed
        return KErrNotFound;
        }

    TInt startPos( 0 );
    TInt endPos( 0 );
    TInt length( 0 );
    TInt extraEndPos( 0 );
    TInt ret = ParseStartAndEndPos( iItemLineSection, KNullDesC, startPos,
                                    endPos, length, extraEndPos );
    if ( KErrNone != ret )
        {
        // Nothing to parse
        return ret;
        }

    // Current position for forward parsing operations
    iItemSkipAndMarkPos = startPos + 1;

    // Take character
    aCharacter = iItemLineSection[startPos];

    return KErrNone;

    }

/*
-------------------------------------------------------------------------------

    Class: CStifItemParser

    Method: GetNextChar

    Description: Get a next character with tag.

    Get next character with tag until separator cut the parsing. If start tag
    is empty the next character will be parsed and returned.
    GetString or GetInt or GetChar must be called before call GetNextChar
    method.

    Parameters: const TDesC& aTag: in: Indicates parsing start point
                TChar& aCharacter: inout: Parsed character

    Return Values: TInt: Error code

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
EXPORT_C TInt CStifItemParser::GetNextChar( const TDesC& aTag,
                                            TChar& aCharacter )
    {
    // GetString(), GetInt() or GetChar() is not called
    if ( !iGetMethodsIndicator )
        {
        return KErrNotReady;
        }
    if ( 0 == iItemLineSection.Length() )
        {
        // Probably parsering is failed
        return KErrNotFound;
        }

    TInt startPos( 0 );
    TInt endPos( 0 );
    TInt length( 0 );
    TInt extraEndPos( 0 );
    TInt ret = ParseStartAndEndPos( iItemLineSection, aTag, startPos,
                                    endPos, length, extraEndPos );
    if ( KErrNone != ret )
        {
        // Nothing to parse
        return ret;
        }

    // Current position for forward parsing operations
    iItemSkipAndMarkPos = startPos + 1;

    // Take character
    aCharacter = iItemLineSection[startPos];

    return KErrNone;

    }

/*
-------------------------------------------------------------------------------

    Class: CStifItemParser

    Method: Remainder

    Description: Get remaining strings in parsed line.
                 
                 Note: This method does not support the quote(TParsingType)
                 feature, only GetString and GetNextString methods include
                 support.

    Parameters: TPtrC& aString: inout: Parsed string

    Return Values: TInt: Error code

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
EXPORT_C TInt CStifItemParser::Remainder( TPtrC& aString )
    {
    
    if ( 0 == iItemLineSection.Length() )
        {
        // Probably parsering is failed
        return KErrNotFound;
        }

    if( iItemLineSection.Length() <= iItemSkipAndMarkPos )
        {
        // Nothing to parse, check here to avoid lex's crash
        return KErrNotFound;
        }

    // Skip space
    TLex lex( iItemLineSection );
    lex.SkipAndMark( iItemSkipAndMarkPos );
    lex.SkipSpace();
    
    if( lex.Eos() )
        {
        // Remaining are only spaces. To avoid Set()'s panic
        return KErrNotFound;
        }

    // Take string
    aString.Set( (TUint16*)&iItemLineSection[ lex.Offset() ], 
                 iItemLineSection.Length() - lex.Offset() );

    return KErrNone;

    }

/*
-------------------------------------------------------------------------------

    Class: CStifItemParser

    Method: SetParsingType

    Description: Parsing type can be given when parsing string(GetString and
                 GetNextString). Please see TParsingType enumeration for more
                 inforamtion.

    Parameters: CStifItemParser::TParsingType aType: in: Sets a parsing type.

    Return Values: TInt: Symbian error code

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
EXPORT_C TInt CStifItemParser::SetParsingType( 
                                    CStifItemParser::TParsingType aType )
    {
    iParsingType = aType;

    return KErrNone;

    }

/*
-------------------------------------------------------------------------------

    Class: CStifItemParser

    Method: ParsingType

    Description: Get current parsing type. Please see TParsingType enumeration
                 for more inforamtion.

    Parameters: None

    Return Values: CStifItemParser::TParsingType: Current parsing type.

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
EXPORT_C CStifItemParser::TParsingType CStifItemParser::ParsingType()
    {
    return iParsingType;

    }

// ================= OTHER EXPORTED FUNCTIONS =================================

// End of File
