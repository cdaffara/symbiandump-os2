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
* Description: This file contains the header file of the 
* CStifParser.
*
*/

#ifndef STIF_PARSER_H
#define STIF_PARSER_H

//  INCLUDES
#include <f32file.h>
#include <e32std.h>
#include <e32base.h>
#include "StifSectionParser.h"
#include "StifItemParser.h"

// CONSTANTS
// None

// MACROS
// None

// DATA TYPES
// None

// FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
class CStifSectionParser;
class CStifFileParser;

// CLASS DECLARATION

// DESCRIPTION
// CStifParser is a STIF Test Framework StifParser class.
// Class contains a configuration file parsing operations.

class CStifParser
        :public CBase
    {
    public:     // Enumerations

        // Indication which type of comments are in use
        enum TCommentType
            {
            ENoComments,        // No comments type given, parse all
            ECStyleComments,    // C-style comments( // and /*...*/ ) are
                                // removed
            };

    private:    // Enumerations

        // Indication which mode of parsing are in use
        enum TParsingMode
            {
            EFileParsing,       // Indication for file parsing.
            EBufferParsing,     // Indication for buffer parsing.
            };

    public:     // Constructors and destructor

        /**
        * Two-phased constructor.
        * Creating parser with path and file information. For default all
        * information will be parsed(ENoComments).
        */
        IMPORT_C static CStifParser* NewL( const TDesC& aPath, 
                                            const TDesC& aConfig,
                                            TCommentType = ENoComments);

        /**
        * Two-phased constructor.
        * Creating parser with buffer information. For default all
        * information will be parsed(ENoComments).
        */
        IMPORT_C static CStifParser* NewL( const TDesC& aBuffer, 
                                            TCommentType = ENoComments );

        /**
        * Destructor.
        */
        IMPORT_C ~CStifParser();

    public:     // New functions

        /**
        * Open and read configuration source and parses a required section.
        * If start tag is empty the parsing starts beging of the configuration
        * file.
        * If end tag is empty the parsing goes end of configuration file.
        * This method starts always from beginning of configuration file and
        * parses first section if aSeeked parameters is not given.
        * If configuration file includes several sections with both start and
        * end tags so aSeeked parameter seeks the required section. The aSeeked
        * parameters indicates section that will be parsed.
        */
        IMPORT_C CStifSectionParser* SectionL( const TDesC& aStartTag,
                                                const TDesC& aEndTag,
                                                TInt aSeeked = 1 );

        /**
        * Open and read configuration source and parses a required section.
        * If start tag is empty the parsing starts beging of the configuration
        * file.
        * If end tag is empty the parsing goes end of configuration file.
        * This method will parse next section after the earlier section if
        * aSeeked parameter is not given.
        * If configuration file includes several sections with both start and
        * end tags so aSeeked parameter seeks the required section. The aSeeked
        * parameters indicates section that will be parsed.
        */
        IMPORT_C CStifSectionParser* NextSectionL( const TDesC& aStartTag,
                                                    const TDesC& aEndTag,
                                                    TInt aSeeked = 1 );

    public:     // Functions from base classes

    protected:  // New functions

    protected:  // Functions from base classes

    private:

        /**
        * C++ default constructor.
        */
        CStifParser( TCommentType aCommentType );

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL( const TDesC& aPath,
                         const TDesC& aConfig,
                         const TDesC& aContent = KNullDesC );

        /**
        * Convert a section without comments.
        */
        void ParseCommentsOff( TPtr& aBuf );

        //
        // Handles special marks.( '\/\/', '\/\*' and '*/\/' ). This
        // is used when ECStyleComments comment type is used. 
        //
        void HandleSpecialMarks( TPtr& aBuf );

        /**
        * Open and read configuration source and parses a required section.
        * If start tag is empty the parsing starts beging of the configuration
        * file.
        * If end tag is empty the parsing goes end of configuration file.
        * This method will parse next section after the earlier section if
        * aSeeked parameter is not given.
        * If configuration file includes several sections with both start and
        * end tags so aSeeked parameter seeks the required section. The aSeeked
        * parameters indicates section that will be parsed.
        */
		CStifSectionParser *NextSectionMemoryL( const TDesC& aStartTag,
                                              	const TDesC& aEndTag,
                                                TInt aSeeked );

        /**
        * Open and read configuration source and parses a required section.
        * If start tag is empty the parsing starts beging of the configuration
        * file.
        * If end tag is empty the parsing goes end of configuration file.
        * This method will parse next section after the earlier section if
        * aSeeked parameter is not given.
        * If configuration file includes several sections with both start and
        * end tags so aSeeked parameter seeks the required section. The aSeeked
        * parameters indicates section that will be parsed.
        */
		CStifSectionParser *NextSectionFileL( const TDesC& aStartTag,
                                              const TDesC& aEndTag,
                                              TInt aSeeked );
    public:     // Data

    protected:  // Data

    private:    // Data

        /**
        * For file opening.
        */
        RFs         iFileServer;

        /**
        * For file opening.
        */
        RFile       iFile;

        /**
        * Indicates position where we are currently.
        */
        TInt        iOffset;

        /**
        * Comment type's indication.
        */
        TCommentType iCommentType;

        /**
        * Indication which mode of parsing are in use.
        */
        TParsingMode iParsingMode;

        /**
        * For buffer parsing.
        */
        HBufC*       iBufferTmp;
        TPtr         iBuffer;

        /**
        * Flag indicating if file is unicode.
        */
        TBool        iIsUnicode;

        /**
        * Pointer to file parser.
        */
		CStifFileParser* iFileParser;

    public:     // Friend classes

    protected:  // Friend classes

    private:    // Friend classes

    };

#endif      // STIF_PARSER_H

// End of File
