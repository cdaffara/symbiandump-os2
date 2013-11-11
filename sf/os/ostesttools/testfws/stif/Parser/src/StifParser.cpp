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
* Description: This module contains implementation of CStifParser 
* class member functions.
*
*/

// INCLUDE FILES
#include <e32std.h>
#include "StifParser.h"
#include "ParserTracing.h"
#include "StifFileParser.h"

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

    Class: CStifParser

    Method: CStifParser

    Description: Default constructor

    C++ default constructor can NOT contain any code, that
    might leave.
    
    Parameters: TCommentType aCommentType: in: Comment type's indication
    
    Return Values: None

    Errors/Exceptions: None

    Status: Approved
    
-------------------------------------------------------------------------------
*/
CStifParser::CStifParser( TCommentType aCommentType ) :
    iBuffer( 0, 0 )
    {
    iCommentType = aCommentType;

    iParsingMode = EFileParsing;

    iIsUnicode = EFalse;
    
    iFileParser = NULL;
    }

/*
-------------------------------------------------------------------------------

    Class: CStifParser

    Method: ConstructL

    Description: Symbian OS second phase constructor

    Symbian OS default constructor can leave.

    Connecting and opening configuration file if path and file information is 
    given. If path and file information is not given and information is given 
    in buffer then create parser according to the buffer.
    
    Parameters: const TDesC& aPath: in: Source path definition
                const TDesC& aConfig: in: Configuration filename
                const TDesC& aBuffer: in: Buffer of the parsed information

    Return Values: None

    Errors/Exceptions:  Leaves if called Connect method fails
                        Leaves if called SetSessionPath method fails
                        Leaves if called Open method fails
                        Leaves if HBufC::NewL operation leaves

    Status: Proposal

-------------------------------------------------------------------------------
*/
void CStifParser::ConstructL( const TDesC& aPath,
                                const TDesC& aConfig,
                                const TDesC& aBuffer)
    {
    if( aPath == KNullDesC && aConfig == KNullDesC && aBuffer != KNullDesC )
        {
        // Set mode
        iParsingMode = EBufferParsing;

        // Construct modifiable heap-based descriptor.
        iBufferTmp = HBufC::NewL( aBuffer.Length() );
        //iBuffer = iBufferTmp->Des();
        iBuffer.Set( iBufferTmp->Des() );
        // Copy content
        iBuffer.Copy( aBuffer );
        }

    else
        {
        User::LeaveIfError( iFileServer.Connect() );
    
        __TRACE( KInfo, ( _L( "STIFPARSER: Open configfile '%S%S'" ),
                &aPath, &aConfig ) );
                
        User::LeaveIfError( iFileServer.SetSessionPath( aPath ) );
        User::LeaveIfError( iFile.Open( 
                            iFileServer, aConfig, EFileRead | EFileShareAny ) );

        //Check whether the file is unicoded
        __TRACE(KInfo, (_L("STIFPARSER: Check if the file is unicode")));
        _LIT(KUnicode, "#UNICODE");
        const TInt KUnicodeLength(8 * 2 + 2); //times two, because we want to read unicode string using 8bit descriptor
                                              //two characters more because on some systems FEFF is always added on the beginning of unicode file
        TInt size(0);

        User::LeaveIfError(iFile.Size(size));

        if(size >= KUnicodeLength)
            {
            TBuf8<KUnicodeLength> buf;

            User::LeaveIfError(iFile.Read(0, buf));
            TPtrC16 bufuni((TUint16 *)(buf.Ptr()), buf.Length() / 2);
            if(bufuni.Find(KUnicode) != KErrNotFound)
                {
                iIsUnicode = ETrue;
                __TRACE(KInfo, (_L("STIFPARSER: File is unicode")));
                }
            }
        
        //Create file parser object
        iFileParser = CStifFileParser::NewL(iFileServer, iFile, iIsUnicode, iCommentType);
        }

    iOffset = 0;

    }

/*
-------------------------------------------------------------------------------

    Class: CStifParser

    Method: NewL

    Description: Two-phased constructor.

    Starting creating parser with path and file information.

    Parameters: const TDesC& aPath: in: Source path definition
                const TDesC& aConfig: in: Configuration filename
                TCommentType aCommentType: in: Comment type's indication

    Return Values: CStifParser* : pointer to CStifParser object

    Errors/Exceptions: Leaves if ConstructL leaves

    Status: Proposal

-------------------------------------------------------------------------------
*/
EXPORT_C CStifParser* CStifParser::NewL( const TDesC& aPath,
                                            const TDesC& aConfig,
                                            TCommentType aCommentType )
    {
    __TRACE( KInfo, ( _L( "STIFPARSER: Debug information is used" ) ) );

    // Create CStifParser object
    CStifParser* parser = new (ELeave) CStifParser( aCommentType );

    CleanupStack::PushL( parser );
    parser->ConstructL( aPath, aConfig );
    CleanupStack::Pop( parser );

    return parser;

    }

/*
-------------------------------------------------------------------------------

    Class: CStifParser

    Method: NewL

    Description: Two-phased constructor.

    Starting creating parser with buffer information.

    Parameters: const TDesC& aBuffer: in: Buffer of the parsed informations
                TCommentType aCommentType: in: Comment type's indication

    Return Values: CStifParser* : pointer to CStifParser object

    Errors/Exceptions: Leaves if ConstructL leaves

    Status: Proposal

-------------------------------------------------------------------------------
*/
EXPORT_C CStifParser* CStifParser::NewL( const TDesC& aBuffer,
                                            TCommentType aCommentType )
    {
    __TRACE( KInfo, ( _L( "STIFPARSER: Debug information is used" ) ) );

    // Create CStifParser object
    CStifParser* parser = new (ELeave) CStifParser( aCommentType );

    CleanupStack::PushL( parser );
    // No path and file name informations. Buffer is given
    parser->ConstructL( KNullDesC, KNullDesC, aBuffer );
    CleanupStack::Pop( parser );

    return parser;

    }

/*
-------------------------------------------------------------------------------

    Class: CStifParser

    Method: ~CStifParser

    Description: Destructor

    Close file and the fileserver handles.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/    
EXPORT_C CStifParser::~CStifParser()
    {

    if( iParsingMode == EBufferParsing )
        {
        delete iBufferTmp;
        }
    else
        {
        delete iFileParser;
        iFile.Close();
        iFileServer.Close();
        }

    }

/*
-------------------------------------------------------------------------------

    Class: CStifParser

    Method: SectionL

    Description: Parses sections from configuration files.

    Open and read configuration source and parses a required section.
    If start tag is empty the parsing starts beginning of the configuration
    file. If end tag is empty the parsing goes end of configuration file.
    This method starts always from beginning of configuration file and parses
    first section if aSeeked parameters is not given.
    If configuration file includes several sections with both start and end
    tags so aSeeked parameter seeks the required section. The aSeeked
    parameters indicates section that will be parsed.

    Parameters: const TDesC& aStartTag: in: Indicates a start tag for parsing
                const TDesC& aEndTag: in: Indicates an end tag for parsing
                TInt aSeeked: in: a seeked section which will be parsed

    Return Values:  See NextSectionL() method

    Errors/Exceptions:  See NextSectionL() method

    Status: Approved

-------------------------------------------------------------------------------
*/
EXPORT_C CStifSectionParser* CStifParser::SectionL( const TDesC& aStartTag,
                                                    const TDesC& aEndTag,
                                                    TInt aSeeked )
    {
    iOffset = 0;
    return NextSectionL( aStartTag, aEndTag, aSeeked );

    }

/*
-------------------------------------------------------------------------------

    Class: CStifParser

    Method: NextSectionL

    Description: Parses sections from configuration files.

    Open and read configuration source and parses a required section.
    If start tag is empty the parsing starts beginning of the configuration
    file. If end tag is empty the parsing goes end of configuration file.
    This method will parse next section after the earlier section if aSeeked
    parameter is not given.
    If configuration file includes several sections with both start and end
    tags so aSeeked parameter seeks the required section. The aSeeked
    parameters indicates section that will be parsed.

    Parameters: const TDesC& aStartTag: in: Indicates a start tag for parsing
                const TDesC& aEndTag: in: Indicates an end tag for parsing
                TInt aSeeked: in: a seeked section which will be parsed

    Return Values:  CStifSectionParser* : pointer to CStifSectionParser object
                    NULL will return if NextSectionFileL (or NextSectionMemoryL) returns NULL

    Errors/Exceptions:  Leaves if NextSectionFileL leaves
                        Leaves if NextSectionMemoryL leaves

    Status: Proposal

-------------------------------------------------------------------------------
*/
EXPORT_C CStifSectionParser* CStifParser::NextSectionL( const TDesC& aStartTag,
                                                        const TDesC& aEndTag,
                                                        TInt aSeeked )
    {
	//If parsing mode is set to file, we parse directly in the file
	if(iParsingMode == EFileParsing)
		{
		return NextSectionFileL(aStartTag, aEndTag, aSeeked);
		}

	//If parsing mode is set to buffer, process in old way
    return NextSectionMemoryL(aStartTag, aEndTag, aSeeked);
    }

/*
-------------------------------------------------------------------------------

    Class: CStifParser

    Method: NextSectionMemoryL

    Description: Parses sections from configuration files.

    Open and read configuration source and parses a required section.
    If start tag is empty the parsing starts beginning of the configuration
    file. If end tag is empty the parsing goes end of configuration file.
    This method will parse next section after the earlier section if aSeeked
    parameter is not given.
    If configuration file includes several sections with both start and end
    tags so aSeeked parameter seeks the required section. The aSeeked
    parameters indicates section that will be parsed.

    Parameters: const TDesC& aStartTag: in: Indicates a start tag for parsing
                const TDesC& aEndTag: in: Indicates an end tag for parsing
                TInt aSeeked: in: a seeked section which will be parsed

    Return Values:  CStifSectionParser* : pointer to CStifSectionParser object
                    NULL will return if file size or aSeeked is not positive
                    NULL will return if start tag is not found
                    NULL will return if end tag is not found
                    NULL will return if parsed section length is not positive

    Errors/Exceptions:  Leaves if called Size method fails
                        Leaves if HBufC::NewLC method leaves
                        Leaves if called Read method fails
                        Leaves if CStifSectionParser::NewL methods leaves

    Status: Proposal

-------------------------------------------------------------------------------
*/
CStifSectionParser* CStifParser::NextSectionMemoryL( const TDesC& aStartTag,
                                                 	 const TDesC& aEndTag,
                                                     TInt aSeeked )
    {
    TInt size( 0 );
    // Parser is created straight with data
    if( iParsingMode == EBufferParsing )
        {
        size = iBuffer.Length();
        }
    // Parser is created with path and file informations
    else
        {
        User::LeaveIfError( iFile.Size( size ) );
        }

    // size or aSeeked cannot be 0 or negetive
    if( size <= 0 || aSeeked <= 0)
        {
        __TRACE(
            KInfo, ( _L( "STIFPARSER: NextSectionL method returns a NULL" ) ) );
        return NULL;
        }

    const TInt tmpSize = 128;//--UNICODE-- KMaxName; // 128 - set to even value, because KMaxName may change in the future
    TInt offset( 0 ); // Offset value to parts reading

    // Construct modifiable heap-based descriptor. tmp to CleanupStack
    HBufC* tmp = HBufC::NewLC( size );
    TPtr wholeSection = tmp->Des();

    // Construct modifiable heap-based descriptor. tmp2 to CleanupStack
    HBufC8* tmp2 = HBufC8::NewLC( tmpSize );    // 128
    TPtr8 buf = tmp2->Des();

    // Construct modifiable heap-based descriptor. tmp3 to CleanupStack
    HBufC* tmp3 = HBufC::NewLC( tmpSize );      // 128
    TPtr currentSection = tmp3->Des();

    // Parser is created straight with data
    if( iParsingMode == EBufferParsing )
        {
        // If 8 bit copy changes to 16
        wholeSection.Copy( iBuffer );
        }
    // Parser is created with path and file informations
    else
        {
        TPtrC16 currentSectionUnicode;
        do // Read data in parts(Maximum part size is KMaxName)
            {
            // Read data
            User::LeaveIfError( iFile.Read( offset, buf, tmpSize ) );

            // If file is unicode convert differently
            if(iIsUnicode)
                {
                // 8 bit to 16 with unicode conversion - simply point to byte array as to double-byte array
                currentSectionUnicode.Set((TUint16 *)(buf.Ptr()), buf.Length() / 2);
                // Appends current section to whole section
                wholeSection.Append( currentSectionUnicode );
                }
            else
                {
                // 8 bit to 16
                currentSection.Copy( buf );
                // Appends current section to whole section
                wholeSection.Append( currentSection );
                }

            offset += tmpSize;

            } while( offset < size );
        }

    CleanupStack::PopAndDestroy( tmp3 );
    CleanupStack::PopAndDestroy( tmp2 );

    // User wants section without c-style comments
    if( iCommentType == ECStyleComments )
        {
        ParseCommentsOff( wholeSection );
        }

    TLex lex( wholeSection );
    lex.SkipAndMark( iOffset );

    // For the required section length and positions
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
        // While end of section
        while ( !lex.Eos() )
            {
            TPtrC ptr = lex.NextToken();
            // Start of the section is found and correct section
            if ( ptr == aStartTag && tagCount == aSeeked )
                {
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
        __TRACE( KInfo, ( _L(
            "STIFPARSER: NextSectionL method: Seeked section is not found" ) ) );
        CleanupStack::PopAndDestroy( tmp );
        User::Leave( KErrNotFound );
        }

    // Check is aEndTag given
    if ( aEndTag.Length() == 0 )
        {
        lengthEndPos = wholeSection.Length();
        }
    else
        {
        // While end of section
        while ( !lex.Eos() )
            {
            TPtrC ptr = lex.NextToken();
            // End tag of the section is found
            if ( ptr == aEndTag )
                {
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
        __TRACE(
            KInfo, ( _L( "STIFPARSER: NextSectionL method returns a NULL" ) ) );
        }
    else
        {
        // Make CStifSectionParser object and alloc required length
        section = CStifSectionParser::NewL( length );
        CleanupStack::PushL( section );

        // Copy required data to the section object
        section->SetData( wholeSection, lengthStartPos, length );

        //iOffset += lengthEndPos + aEndTag.Length();
        iOffset = lex.Offset();

        CleanupStack::Pop( section );
        }
    CleanupStack::PopAndDestroy( tmp );

    return section;

    }

/*
-------------------------------------------------------------------------------

    Class: CStifParser

    Method: NextSectionFileL

    Description: Parses sections from configuration files.

    Open and read configuration source and parses a required section.
    If start tag is empty the parsing starts beginning of the configuration
    file. If end tag is empty the parsing goes end of configuration file.
    This method will parse next section after the earlier section if aSeeked
    parameter is not given.
    If configuration file includes several sections with both start and end
    tags so aSeeked parameter seeks the required section. The aSeeked
    parameters indicates section that will be parsed.

    Parameters: const TDesC& aStartTag: in: Indicates a start tag for parsing
                const TDesC& aEndTag: in: Indicates an end tag for parsing
                TInt aSeeked: in: a seeked section which will be parsed

    Return Values:  CStifSectionParser* : pointer to CStifSectionParser object
                    NULL will return if file size or aSeeked is not positive
                    NULL will return if start tag is not found
                    NULL will return if end tag is not found
                    NULL will return if parsed section length is not positive

    Errors/Exceptions:  Leaves if called Size method fails
                        Leaves if HBufC::NewLC method leaves
                        Leaves if called Read method fails
                        Leaves if CStifSectionParser::NewL methods leaves

    Status: Proposal

-------------------------------------------------------------------------------
*/
CStifSectionParser* CStifParser::NextSectionFileL( const TDesC& aStartTag,
                                                   const TDesC& aEndTag,
                                                   TInt aSeeked )
    {
	HBufC *bufSection = iFileParser->NextSectionL(aStartTag, aEndTag, iOffset, aSeeked);

	if(bufSection)
		{
		CleanupStack::PushL(bufSection);
		TPtr bufSectionPtr(bufSection->Des());

		if(iCommentType == ECStyleComments)
			{
			ParseCommentsOff(bufSectionPtr);
			}

		// Make CStifSectionParser object and alloc required length
		CStifSectionParser* section = CStifSectionParser::NewL(bufSection->Length());
		CleanupStack::PushL(section);

		// Copy required data to the section object
		section->SetData(bufSectionPtr, 0, bufSection->Length());

		// Clean
		CleanupStack::Pop(section);
		CleanupStack::PopAndDestroy(bufSection);

		return section;
		}

	return NULL;
    }

/*
-------------------------------------------------------------------------------

    Class: CStifParser

    Method: ParseCommentsOff

    Description: Convert a section without comments.

    Parameters: TPtr& aBuf: inout: section to parsed

    Return Values: None

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
void CStifParser::ParseCommentsOff( TPtr& aBuf )
    {
    TInt startPos( 0 );
    TInt endPos( 0 );
    TInt length( 0 );
    enum TSearchType
        {
        ENormalSearch,          // Search a '//' or a '/*'
        ECStyleSlashs,          // Search is '//'
        ECStyleSlashAndAsterisk,// Search is '/*'
        EDoRemove,              // Remove comment
        };

    TSearchType searchType( ENormalSearch );

    TLex lex( aBuf );

    // Remove comments
    do
        {
        switch( searchType )
            {
            case ENormalSearch:
                {
                if( lex.Get() == '/' )
                    {
                    // Peek next character( '/' )
                    if( lex.Peek() == '/' )
                        {
                        startPos = lex.Offset();
                        startPos--;
                        lex.Inc();
                        searchType = ECStyleSlashs;
                        }
                    // Peek next character( '*' )
                    else if( lex.Peek() == '*' )
                        {
                        startPos = lex.Offset();
                        startPos--;
                        lex.Inc();
                        searchType = ECStyleSlashAndAsterisk;
                        }
                    }
                break;
                }
            case ECStyleSlashs:
                {
                // Peek next character(10 or '\n' in UNIX style )
                if( lex.Peek() == 0x0A )
                    {
                    // Don't remove line break!!( Else this fails:
                    // 1st line:"this is parsed text 1"
                    // 2nd line:"this is parsed text 2 // this is comments"
                    // 1st and 2nd lines will be together and following
                    // operations may fail)
                    endPos = lex.Offset();
                    searchType = EDoRemove;
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
                        // Don't remove line break!!( Else this fails:
                        // 1st line:"this is parsed text 1"
                        // 2nd line:"this is parsed text 2 // this is comments"
                        // 1st and 2nd lines will be together and following
                        // operations may fail)
                        endPos = lex.Offset();
                        endPos = endPos - 1; // Two line break characters
                        searchType = EDoRemove;
                        break;
                        }
                    // 0x0A not found, decrement position
                    lex.UnGet();
                    }
                // Increment the lex position
                lex.Inc();
                // Take current end position
                endPos = lex.Offset();
                break;
                }

            case ECStyleSlashAndAsterisk:
                {
                // Peek next character( '*' )
                if ( lex.Peek() == '*' )
                    {
                    // Increment the lex position
                    lex.Inc();
                    // Peek next character( '/')
                    if ( lex.Peek() == '/' )
                        {
                        // End of the section is found and increment the lex position
                        lex.Inc();
                        endPos = lex.Offset();
                        searchType = EDoRemove;
                        break;
                        }
                    // '/' not found, decrement position
                    lex.UnGet();
                    }
                // Increment the lex position
                lex.Inc();

                // Take current end position
                endPos = lex.Offset();
                break;
                }
            default:
                {
                searchType = ENormalSearch;
                break;
                }

            } // End of switch

            // Remove comment
            if( searchType == EDoRemove )
                {
                length = endPos - startPos;
                aBuf.Delete( startPos, length );
                lex = aBuf;
                searchType = ENormalSearch;
                }

        } while ( !lex.Eos() );

    // If comment is started and configure file ends to eof we remove
    // comments althougt there are no end of line or '*/' characters
    if( searchType == ECStyleSlashs || searchType == ECStyleSlashs )
        {
        length = lex.Offset() - startPos;
        aBuf.Delete( startPos, length );
        }

    HandleSpecialMarks( aBuf );

    }

//
//-----------------------------------------------------------------------------
//
//    Class: CStifParser
//
//    Method: HandleSpecialMarks
//
//    Description: Handles special marks.( '\/' and '\*' ). This
//         		   is used when ECStyleComments comment type is used.
//
//    Parameters: TPtr& aBuf: inout: section to parsed
//
//    Return Values: None
//
//    Errors/Exceptions: None
//
//    Status: Proposal
//
//-----------------------------------------------------------------------------
//
void CStifParser::HandleSpecialMarks( TPtr& aBuf )
    {
    TLex lex( aBuf );
    TInt firstPos( 0 );
    
    //        Replace \/ with /
    //        Replace \* with *
    
    do
        {
        //RDebug::Print( _L("Print : %S"), &aBuf );
        firstPos = lex.Offset();
        TChar get = lex.Get();
        // Check is '\'
        if( get == '\\' ) 
            {
            firstPos = (lex.Offset()-1);
            // Peek next character( '/' or '*' )
            if( lex.Peek() == '/' || lex.Peek() == '*')
                {
                aBuf.Delete (firstPos,1);
                lex = aBuf;
                }
            }
           
        firstPos = 0;
        } while ( !lex.Eos() );

    }

// End of File
