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
* Description: This module contains implementation of 
* CStifSectionParser class member functions.
*
*/

// INCLUDE FILES
#include <e32std.h>
#include "StifSectionParser.h"
#include "ParserTracing.h"

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

// ================= MEMBER FUNCTIONS =========================================

/*
-------------------------------------------------------------------------------

    Class: CStifSectionParser

    Method: CStifSectionParser

    Description: Default constructor.

    C++ default constructor can NOT contain any code, that
    might leave.

    Parameters: const TInt aLength: in: Parsed section length

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
CStifSectionParser::CStifSectionParser( const TInt aLength ) :
    iSection( 0, 0 ),
    iLength( aLength ),
    iSkipAndMarkPos( 0 ),
    iLineIndicator( EFalse )
    {

    }

/*
-------------------------------------------------------------------------------

    Class: CStifSectionParser

    Method: ConstructL

    Description: Symbian OS second phase constructor.

    Symbian OS default constructor can leave.

    Parameters: None

    Return Values: None

    Errors/Exceptions:  Leaves if iLength is negative
                        Leaves if called NewL method fails

    Status: Approved

-------------------------------------------------------------------------------
*/
void CStifSectionParser::ConstructL()
    {
    // Construct modifiable heap-based descriptor
    iHBufferSection = HBufC::NewL( iLength );
    iSection.Set(iHBufferSection->Des());

    iSubOffset = 0;

    }

/*
-------------------------------------------------------------------------------

    Class: CStifSectionParser

    Method: NewL

    Description: Two-phased constructor.

    Parameters: const TInt aLength: in: Parsed section length

    Return Values: CStifSectionParser* : pointer to CStifSectionParser object

    Errors/Exceptions:  Leaves if section length is negative
                        Leaves if construction fails

    Status: Approved

-------------------------------------------------------------------------------
*/
CStifSectionParser* CStifSectionParser::NewL( const TInt aLength )
    {
    __TRACE( KInfo, ( _L( "STIFPARSER: Create a section" ) ) );
    __ASSERT_ALWAYS( aLength > 0, User::Leave( KErrArgument ) );

    CStifSectionParser* item = new (ELeave) CStifSectionParser( aLength );

    CleanupStack::PushL( item );
    item->ConstructL();
    CleanupStack::Pop( item );

    return item;

    }

/*
-------------------------------------------------------------------------------

    Class: CStifSectionParser

    Method: ~CStifSectionParser

    Description: Destructor

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/    
CStifSectionParser::~CStifSectionParser()
    {
    __TRACE( KInfo, ( _L( "STIFPARSER: Call destructor '~CStifSectionParser'" ) ) );

    delete iHBufferSection;

    }

/*
-------------------------------------------------------------------------------

    Class: CStifSectionParser

    Method: ParseStartAndEndPos

    Description: Start and end position parser.

    Parameters: TPtrC aSection: in: Parsed section
                const TDesC& aStartTag: in: Start tag of parsing
                TTagToReturnValue aTagIndicator: in: Will aStartTag included to
                the returned values
                TInt& aStartPos: inout: Start point of parsing
                TInt& aEndPos: inout: End point of parsing
                TInt& aLength: inout: Length of parsed section

    Return Values: TInt: Error code

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
TInt CStifSectionParser::ParseStartAndEndPos( TPtrC aSection,
                                            const TDesC& aStartTag,
                                            TTagToReturnValue aTagIndicator,
                                            TInt& aStartPos,
                                            TInt& aEndPos,
                                            TInt& aLength )
    {
    TLex lex( aSection );
    lex.SkipAndMark( iSkipAndMarkPos );

    // Check is aStartTag given
    if ( aStartTag.Length() == 0 )
        {
        // Skip line break, tabs, spaces etc.
        lex.SkipSpace();
        aStartPos = lex.Offset();
        }
    else
        {
        // While end of section and aStartTag is given
        while ( !lex.Eos() )
            {
            lex.SkipSpace();
            TPtrC line = SubstractLine( lex.Remainder() );
            TInt tagStartPos = 0;
            TInt tagEndPos = 0;
            if ( FindTag( line, aStartTag, tagStartPos, tagEndPos ) == KErrNone )
            	{
            	if ( aTagIndicator == ETag )
            		{
            		aStartPos = lex.Offset();
            		}
            	else
            		{
            		aStartPos = lex.Offset() + tagEndPos;
            		if ( line.Length() - tagEndPos == 0 )
            			{
            			return KErrNotFound;
            			}
            		}
            	break;
            	}
                
            GotoEndOfLine( lex );
            }
        }

    // End tag parsing starts and if we are end of the section
    if( lex.Eos() )
        {
        return KErrNotFound;
        }

    // "Delete" white spaces(includes line break) 
    aEndPos = GotoEndOfLine( lex );
    // Position where start next parsing.(End position, includes white spaces)
    iSkipAndMarkPos = lex.Offset();
    // The length includes spaces and end of lines
    aLength = ( aEndPos - aStartPos );

    return KErrNone;

    }

/*
-------------------------------------------------------------------------------

    Class: CStifSectionParser

    Method: SubstractLine

    Description: Substracts line from selected text

    Parameters: TPtrC& aText: in: text.

    Return Values: TPtrC: Substracted line.

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
TPtrC CStifSectionParser::SubstractLine( const TPtrC& aText )
	{
	TLex lex( aText );
	
	while( !lex.Eos() )
		{
		if ( lex.Get() == 0x0A ) // "\n" character. Unix style
			{
			break;
			}		
		}
	
	return aText.Left( lex.Offset() );
	}

/*
-------------------------------------------------------------------------------

    Class: CStifSectionParser

    Method: FindTag

    Description: Searches for selected tag in text

    Parameters: const TPtrC& aText: in: text,
				const TPtrC& aTag: in: tag,
				TInt& aStartPos: out: tag start position,
				TInt& aEndPos: out: tag end position.

    Return Values: TInt: KErrNone, if tag was found. KErrNotFound if it was not found. 

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
TInt CStifSectionParser::FindTag( const TDesC& aText, const TDesC& aTag, TInt& aStartPos, TInt& aEndPos ) {
	TBool tagWithEqualChar = EFalse;

	HBufC* tagBuf = NULL;
	TRAPD( err, tagBuf = aTag.AllocL() );
	if ( err != KErrNone )
		{
		return err;
		}

	RBuf tag( tagBuf );
	tag.Trim();
	if ( tag.Right( 1 ) == _L("=") )
		{
		tagWithEqualChar = ETrue;
		tag.SetLength( tag.Length() - 1 );
		}
	
	TLex lex(aText);
	lex.SkipSpaceAndMark();
	TInt startPos = lex.Offset();
	TPtrC token = lex.NextToken(); 
	
	
	if ( !tagWithEqualChar )
		{
		if ( token == tag )
			{
			aStartPos = startPos;
			lex.SkipSpace();
			aEndPos = lex.Offset();
			tag.Close();
			return KErrNone;
			}
		}
	else
		{
		lex.UnGetToMark();
		TPtrC remText = lex.Remainder();
		if ( remText.Find( tag ) == 0 )
			{
			lex.SkipAndMark( tag.Length() );
			lex.SkipSpaceAndMark();
			if ( !lex.Eos() )
				{
				if ( lex.Get() == '=' )
					{
					aStartPos = startPos;
					lex.SkipSpace();					
					aEndPos = lex.Offset();
					tag.Close();
					return KErrNone;
					}
				}
			}
		}
	
	tag.Close();
	return KErrNotFound;
}

/*
-------------------------------------------------------------------------------

    Class: CStifSectionParser

    Method: GotoEndOfLine

    Description: Goes end of the line.

    Parameters: TLex& lex: inout: Parsed line.

    Return Values: TInt: Last item's end position.

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
TInt CStifSectionParser::GotoEndOfLine( TLex& lex )
    {
    // End position of the last token(Initialized with current position)
    TInt lastItemPosition( lex.Offset() );

    // LINE BREAK NOTE:
    // Line break in SOS, WIN:  '\r\n'
    // Line break in UNIX:      '\n'

    do
        {
        // Peek next character(10 or '\n' in UNIX style )
        if( lex.Peek() == 0x0A )
            {
            lex.Inc();
            break;
            }

        // Peek next character(13 or '\r' in Symbian OS)
        if ( lex.Peek() == 0x0D )
            {
            // Increment the lex position
            lex.Inc();
            // Peek next character(10 or '\n' in Symbian OS)
            if ( lex.Peek() == 0x0A )
                {
                // End of the section is found and increment the lex position
                lex.Inc();
                break;
                }
            // 0x0A not found, decrement position
            lex.UnGet();
            }
        // Peek for tabulator(0x09) and space(0x20)
        else if ( lex.Peek() == 0x09 || lex.Peek() == 0x20 )
            {
            // Increment the lex position
            lex.Inc();
            continue;
            }
        
        // If white spaces not found take next token
        lex.NextToken();
        lastItemPosition = lex.Offset();    
        
        } while ( !lex.Eos() );
        
    return lastItemPosition;

    }

/*
-------------------------------------------------------------------------------

    Class: CStifSectionParser

    Method: GetItemLineL

    Description: Parses a line for items parsing.

    If start tag is empty the parsing starts beging of the section.

    Parameters: const TDesC& aTag: in: Indicates parsing start point.
                TTagToReturnValue aTagIndicator: in: Will aTag included to the
                returned object(For default the tag will be added)

    Return Values:  CStifItemParser* : pointer to CStifItemParser object
                    NULL will return if ParseStartAndEndPos() method returns -1
                    NULL will return if length is 0 or negative
                    NULL will return if iStartPos is 0

    Errors/Exceptions: Leaves if called CStifItemParser::NewL method fails

    Status: Approved

-------------------------------------------------------------------------------
*/
EXPORT_C CStifItemParser* CStifSectionParser::GetItemLineL( const TDesC& aTag,
                                            TTagToReturnValue aTagIndicator )
    {
    TInt startPos( 0 );
    TInt endPos( 0 );
    TInt length( 0 );

    // Indicator that GetItemLineL has been used
    iLineIndicator = ETrue;

    iSkipAndMarkPos = 0;

    TInt ret = ParseStartAndEndPos( iSection, aTag, aTagIndicator,
                                    startPos, endPos, length );

    // No parsing found
    if ( KErrNone != ret || length <= 0 || startPos < 0 )
        {
        __TRACE( 
            KInfo, ( _L( "STIFPARSER: GetItemLineL method returns a NULL" ) ) );
        return NULL;
        }

    CStifItemParser* line = CStifItemParser::NewL(
                                            iSection, startPos, length );

    return line;

    }

/*
-------------------------------------------------------------------------------

    Class: CStifSectionParser

    Method: GetNextItemLineL

    Description: Parses a next line for items parsing.

    Parameters: None

    Return Values:  CStifItemParser* : pointer to CStifItemParser object
                    NULL will return if iLineIndicator is false
                    NULL will return if ParseStartAndEndPos() method returns -1
                    NULL will return if length is 0 or negative
                    NULL will return if iStartPos is 0

    Errors/Exceptions: Leaves if called CStifItemParser::NewL method fails

    Status: Approved

-------------------------------------------------------------------------------
*/
EXPORT_C CStifItemParser* CStifSectionParser::GetNextItemLineL()
    {
    // GetLine() or GetItemLineL() method is not called
    if ( !iLineIndicator )
        {
        __TRACE( KInfo, 
            ( _L( "STIFPARSER: GetNextItemLineL method returns a NULL" ) ) );
        return NULL;
        }

    TInt startPos( 0 );
    TInt endPos( 0 );
    TInt length( 0 );

    // tagIndicator has no meaning in this method
    TTagToReturnValue tagIndicator( ETag );

    TInt ret = ParseStartAndEndPos( iSection, KNullDesC, tagIndicator,
                                    startPos, endPos, length );

    // No parsing found
    if ( KErrNone != ret || length <= 0 || startPos < 0 )
        {
        __TRACE( KInfo, 
            ( _L( "STIFPARSER: GetNextItemLineL method returns a NULL" ) ) );
        return NULL;
        }

    CStifItemParser* line = CStifItemParser::NewL(
                                            iSection, startPos, length );

    return line;

    }

/*
-------------------------------------------------------------------------------

    Class: CStifSectionParser

    Method: GetNextItemLineL

    Description: Parses a next line for items parsing with a tag.

    If start tag is empty the parsing starts beging of the section.

    Parameters: const TDesC& aTag: in: Indicates parsing start point
                TTagToReturnValue aTagIndicator: in: Will aTag included to the
                returned object(For default the tag will be added)

    Return Values:  CStifItemParser* : pointer to CStifItemParser object
                    NULL will return if iLineIndicator is false
                    NULL will return if ParseStartAndEndPos() method returns -1
                    NULL will return if length is 0 or negative
                    NULL will return if iStartPos is 0

    Errors/Exceptions: Leaves if called CStifItemParser::NewL method fails

    Status: Approved

-------------------------------------------------------------------------------
*/
EXPORT_C CStifItemParser* CStifSectionParser::GetNextItemLineL(
                                            const TDesC& aTag,
                                            TTagToReturnValue aTagIndicator )
    {
    // GetLine() or GetItemLineL() method is not called
    if ( !iLineIndicator )
        {
        __TRACE( KInfo, 
            ( _L( "STIFPARSER: GetNextItemLineL method returns a NULL" ) ) );
        return NULL;
        }

    TInt startPos( 0 );
    TInt endPos( 0 );
    TInt length( 0 );

    TInt ret = ParseStartAndEndPos( iSection, aTag, aTagIndicator,
                                    startPos, endPos, length );
    // No parsing found
    if ( KErrNone != ret || length <= 0 || startPos < 0 )
        {
        __TRACE( KInfo, 
            ( _L( "STIFPARSER: GetNextItemLineL method returns a NULL" )  ) );
        return NULL;
        }

    CStifItemParser* line = CStifItemParser::NewL(
                                            iSection, startPos, length );

    return line;

    }

/*
-------------------------------------------------------------------------------

    Class: CStifSectionParser

    Method: SubSectionL

    Description: Parses sub sections from the main section.

    If start tag is empty the parsing starts begin of the section.
    If end tag is empty the parsing goes end of section.
    This method starts always from beginning of parsed section and parses
    first subsection if aSeeked parameters is not given.
    If parsed section includes several subsections with both start and end
    tags so aSeeked parameter seeks the required subsection. The aSeeked
    parameters indicates subsection that will be parsed.

    Parameters: const TDesC& aStartTag: in: Indicates parsing start point
                const TDesC& aEndTag: in: Indicates parsing end point
                TInt aSeeked: in: a seeked subsection which will be parsed

    Return Values:  CStifItemParser* : pointer to CStifItemParser object
                    NULL will return if end tag is not found
                    NULL will return if length is 0 or negative
                    NULL will return if lengthStart is 0

    Errors/Exceptions: Leaves if called CStifSectionParser::NewL method fails

    Status: Proposal

-------------------------------------------------------------------------------
*/
EXPORT_C CStifSectionParser* CStifSectionParser::SubSectionL( 
                                                        const TDesC& aStartTag,
                                                        const TDesC& aEndTag,
                                                        TInt aSeeked )
    {
    
    iSubOffset = 0;
    return NextSubSectionL( aStartTag, aEndTag, aSeeked );

    }

/*
-------------------------------------------------------------------------------

    Class: CStifSectionParser

    Method: SubSectionL

    Description: Parses subsections from the main section.

    If start tag is empty the parsing starts begin of the section.
    If end tag is empty the parsing goes end of section.
    This method will parse next subsection after the earlier subsection if
    aSeeked parameter is not given.
    If parser section includes several subsections with both start and end
    tags so aSeeked parameter seeks the required subsection. The aSeeked
    parameter indicates subsection that will be parsed.

    Parameters: const TDesC& aStartTag: in: Indicates parsing start point
                const TDesC& aEndTag: in: Indicates parsing end point
                TInt aSeeked: in: a seeked subsection which will be parsed

    Return Values:  CStifItemParser* : pointer to CStifItemParser object
                    NULL will return if end tag is not found
                    NULL will return if length is 0 or negative
                    NULL will return if lengthStart is 0

    Errors/Exceptions: Leaves if called CStifSectionParser::NewL method fails

    Status: Proposal

-------------------------------------------------------------------------------
*/
EXPORT_C CStifSectionParser* CStifSectionParser::NextSubSectionL( 
                                                    const TDesC& aStartTag,
                                                    const TDesC& aEndTag,
                                                    TInt aSeeked )
    {
    
    TLex lex( iSection );

    lex.SkipAndMark( iSubOffset );

    // Get the required sub section length
    TInt length( 0 );
    TInt lengthStartPos( 0 );
    TInt lengthEndPos( 0 );
    TBool eos( EFalse );
    TInt tagCount( 1 );

    // Check is aStartTag given
    if ( aStartTag.Length() == 0 )
        {
        // Skip line break, tabs, spaces etc.
        lex.SkipSpace();
        lengthStartPos = lex.Offset();
        }
    else
        {
        // While end of section and aStartTag is given
        while ( !lex.Eos() )
            {
            TPtrC ptr = lex.NextToken();
            // Start of the section is found and correct section
            if ( ptr == aStartTag && tagCount == aSeeked )
                {
                // Start position
                lengthStartPos = lex.Offset();
                break;
                }
            // Start tag is found but not correct section
            else if ( ptr == aStartTag )
                {
                tagCount++;
                }
            }
        }

    // If we are end of section lex.Eos() and eos will be ETrue
    eos = lex.Eos();

    // Seeked section is not found
    if ( tagCount != aSeeked )
        {
        __TRACE( KInfo, ( _L( "STIFPARSER: NextSubSectionL method: Seeked subsection is not found" ) ) );
        User::Leave( KErrNotFound );
        }

    // Check is aEndTag given
    if ( aEndTag.Length() == 0 )
        {
        lengthEndPos = iSection.MaxLength();
        }
    else
        {
        // While end of section and aEndTag is given
        while ( !lex.Eos() )
            {
            TPtrC ptr = lex.NextToken();
            // End tag of the section is found
            if ( ptr == aEndTag )
                {
                // End position
                lengthEndPos = lex.Offset();
                // Because Offset() position is after the aEndTag
                lengthEndPos -= aEndTag.Length();
                break;
                }
            }
        }

    // If we are end of section and lengthEndPos is 0
    if ( lengthEndPos == 0 )
        {
        // lex.Eos() and eos will be ETrue
        eos = lex.Eos();
        }

    // The length includes spaces and end of lines
    length = ( lengthEndPos - lengthStartPos );

    CStifSectionParser* section = NULL;

    // If eos is true or length is negative
    if ( eos || length <= 0  ) 
        {
        __TRACE( KInfo, 
            ( _L( "STIFPARSER: SubSectionL method returns a NULL" ) ) );
        }    
    else
        {
        // Position where start next parsing.(End position,
        // includes white spaces)
        iSubOffset = lex.Offset();
        
        // Make CStifSectionParser object and alloc required length
        section = CStifSectionParser::NewL( length );
        CleanupStack::PushL( section );

        // Copy required data to the section object
        section->SetData( iSection, lengthStartPos, length );

        CleanupStack::Pop( section );
        }

    return section;

    }

/*
-------------------------------------------------------------------------------

    Class: CStifSectionParser

    Method: GetLine

    Description: Get a line.

    Search an item from the section and return rest of the line. If start tag 
    is empty the parsing starts beging of the section.

    Parameters: const TDesC& aTag: in: Indicates parsing start point
                TPtr& aLine: inout: Parsed line
                TTagToReturnValue aTagIndicator: in: Will aTag included to the
                returned value(For default the tag will be added)

    Return Values: TInt: Error code

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
EXPORT_C TInt CStifSectionParser::GetLine( const TDesC& aTag,
                                            TPtrC& aLine,
                                            TTagToReturnValue aTagIndicator )
    {
    TInt ret( KErrNone );

    if ( 0 == iSection.Length() )
        {
        return KErrNotFound;
        }

    // Indicator that GetLine has been used
    iLineIndicator = ETrue;

    TInt startPos( 0 );
    TInt endPos( 0 );
    TInt length( 0 );

    iSkipAndMarkPos = 0;

    ret = ParseStartAndEndPos( iSection, aTag, aTagIndicator,
                                startPos, endPos, length );

    if ( KErrNone != ret )
        {
        // Nothing to parse
        return ret;
        }

    aLine.Set( &iSection[startPos], length );

    return KErrNone;

    }

/*
-------------------------------------------------------------------------------

    Class: CStifSectionParser

    Method: GetNextLine

    Description: Get a line

    Search an item from the section and return rest of the line.

    Parameters: TPtr& aLine: inout: Parsed line

    Return Values: TInt: Error code

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
EXPORT_C TInt CStifSectionParser::GetNextLine( TPtrC& aLine )
    {
    TInt ret( KErrNone );

    // GetLine() or GetItemLineL() method is not called
    if ( !iLineIndicator )
        {
        return KErrNotReady;
        }

    TInt startPos( 0 );
    TInt endPos( 0 );
    TInt length( 0 );

    // tagIndicator has no meaning in this method
    TTagToReturnValue tagIndicator( ETag );

    ret = ParseStartAndEndPos( iSection, KNullDesC, tagIndicator,
                                startPos, endPos, length );
    if ( KErrNone != ret )
        {
        // Nothing to parse
        return ret;
        }

    aLine.Set( &iSection[startPos], length );

    return KErrNone;

    }

/*
-------------------------------------------------------------------------------

    Class: CStifSectionParser

    Method: GetNextLine

    Description: Get a line with tag

    Search a next line with the required tag from the section. If start tag
    is empty the parsing starts beging of the section.

    Parameters: const TDesC& aTag: in: Indicates parsing start point
                TPtr& aLine: inout: Parsed line
                TTagToReturnValue aTagIndicator: in: Will aTag included to the
                returned value(For default the tag will be added)

    Return Values: TInt: Error code

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
EXPORT_C TInt CStifSectionParser::GetNextLine( const TDesC& aTag, TPtrC& aLine,
                                            TTagToReturnValue aTagIndicator )
    {
    TInt ret( KErrNone );

    // GetLine() or GetItemLineL() method is not called
    if ( !iLineIndicator )
        {
        return KErrNotReady;
        }

    TInt startPos( 0 );
    TInt endPos( 0 );
    TInt length( 0 );

    ret = ParseStartAndEndPos( iSection, aTag, aTagIndicator,
                                startPos, endPos, length );
    if ( KErrNone != ret )
        {
        // Nothing to parse
        return ret;
        }

    aLine.Set( &iSection[startPos], length );

    return KErrNone;

    }

 /*
-------------------------------------------------------------------------------

    Class: CStifSectionParser

    Method: GetPosition

    Description: Get current position.

    Returns current parsing position, which
    can be used as parameter for SetPosition afterwards to go back
    to old parsing position.

    Parameters: None
    
    Return Values: TInt: Current parsing position.

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
EXPORT_C TInt CStifSectionParser::GetPosition()
    {
    
    return iSkipAndMarkPos;
    
    }

/*
-------------------------------------------------------------------------------

    Class: CStifSectionParser

    Method: SetPosition

    Description: Set current position.
                
    SetPosition can be used to set parsing position, e.g. to rewind 
    back to some old position retrieved with GetPosition.
    
    Parameters: TInt aPos: in: new parsing position.
    
    Return Values: TInt: Error code

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
EXPORT_C TInt CStifSectionParser::SetPosition( TInt aPos )  
    {
    
    if( aPos < 0 || aPos >= iSection.Length() )
        {
        return KErrArgument;
        }
        
    iSkipAndMarkPos = aPos;    
    
    return KErrNone;
    
    }        
        
/*
-------------------------------------------------------------------------------

    Class: CStifSectionParser

    Method: SetData

    Description: Create a section.

    Parameters: const TPtr aData: in: Data to be parsed
                TInt aStartPos: in: Indicates parsing start position
                TInt aLength: in: Indicates length of parsed section
    
    Return Values: None

    Errors/Exceptions: None

    Status: Approved
    
-------------------------------------------------------------------------------
*/
void CStifSectionParser::SetData( TPtr aData,
                                    TInt aStartPos,
                                    TInt aLength )
    {
    iSection.Copy( aData.Mid( aStartPos, aLength ) );

    }

/*
-------------------------------------------------------------------------------

    Class: CStifSectionParser

    Method: Des

    Description: Returns a section.

    Parameters: None

    Return Values: const TPtrC: Returns a current section

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
const TPtrC CStifSectionParser::Des()
    {
    return (TPtrC)iSection;

    }

// ================= OTHER EXPORTED FUNCTIONS =================================

// End of File
