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
* CStifSectionParser.
*
*/

#ifndef STIF_SECTION_PARSER_H
#define STIF_SECTION_PARSER_H

// INCLUDES
#include <f32file.h>
#include <e32std.h>
#include <e32base.h>
#include "StifItemParser.h"

// CONSTANTS
// Indicates is tag included in returned value.
enum TTagToReturnValue
    {
    ETag,   // Return value includes the tag
    ENoTag  // Return value include no tag
    };

// MACROS
// None

// DATA TYPES
// None

// FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
class CStifItemParser;

// CLASS DECLARATION

// DESCRIPTION
// CStifSectionParser is a STIF Test Framework StifParser class.
// Class contains a configuration file parsing operations.

class CStifSectionParser 
        :public CBase
    {
    public:     // Enumerations

    private:    // Enumerations

    public:     // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CStifSectionParser* NewL( const TInt aLength );

        /**
        * Destructor.
        */
        ~CStifSectionParser();

    public:     // New functions
        
        /**
        * Parses a line for items parsing with a tag.
        * Returns a CStifItemParser object.
        * If start tag is empty the parsing starts beging of the section.
        * TTagToReturnValue indicates will also the aTag value (if exist)
        * include to the returned object(For default the tag will be added).
        */
        IMPORT_C CStifItemParser* GetItemLineL( const TDesC& aTag,
                                TTagToReturnValue aTagIndicator = ETag );

        /**
        * Parses a next line for items parsing.
        * Returns a CStifItemParser object.
        */
        IMPORT_C CStifItemParser* GetNextItemLineL();

        /**
        * Parses a next line for items parsing with a tag.
        * Returns a CStifItemParser object.
        * If start tag is empty the parsing starts beging of the section.
        * TTagToReturnValue indicates will also the aTag value (if exist)
        * include to the returned object(For default the tag will be added).
        */
        IMPORT_C CStifItemParser* GetNextItemLineL( const TDesC& aTag,
                                TTagToReturnValue aTagIndicator = ETag );

        /**
        * Parses a sub sections from the main section with a start and with
        * a end tag.
        * Returns a CStifSectionParser object.
        * If start tag is empty the parsing starts beging of the section.
        * If end tag is empty the parsing goes end of section.
        * This method will parse next subsection after the earlier subsection
        * if aSeeked parameter is not given.
        * If configuration file includes several subsections with both start
        * and end tags so aSeeked parameter seeks the required subsection. The
        * aSeeked parameters indicates subsection that will be parsed.
        */
        IMPORT_C CStifSectionParser* SubSectionL( const TDesC& aStartTag,
                                                    const TDesC& aEndTag,
                                                    TInt aSeeked = 1 );

        /**
        * Parses a next subsections from the main section with a start and with
        * a end tag.
        * Returns a CStifSectionParser object.
        * If start tag is empty the parsing starts beging of the section.
        * If end tag is empty the parsing goes end of section.
        * This method will parse next subsection after the earlier subsection
        * if aSeeked parameter is not given.
        * If configuration file includes several subsections with both start
        * and end tags so aSeeked parameter seeks the required subsection. The
        * aSeeked parameters indicates subsection that will be parsed.
        */        
        IMPORT_C CStifSectionParser* NextSubSectionL( const TDesC& aStartTag,
                                                const TDesC& aEndTag,
                                                TInt aSeeked = 1 );

        /**
        * Get a line from section with a tag.
        * Returns an error code and a reference to the parsed line.
        * If start tag is empty the parsing starts beging of the section.
        * TTagToReturnValue indicates will also the aTag value(if exist)
        * include to the aLine reference(For default the tag will be added).
        */
        IMPORT_C TInt GetLine( const TDesC& aTag, TPtrC& aLine, 
                                TTagToReturnValue aTagIndicator = ETag );

        /**
        * Get next line.
        * Returns an error code and a reference to the parsed line.
        */
        IMPORT_C TInt GetNextLine( TPtrC& aLine );

        /**
        * Get next line with tag.
        * Returns an error code and a reference to the parsed line.
        * If start tag is empty the parsing starts beging of the section.
        * TTagToReturnValue indicates will also the aTag value(if exist)
        * include to the aLine reference(For default the tag will be added).
        */
        IMPORT_C TInt GetNextLine( const TDesC& aTag, TPtrC& aLine,
                                TTagToReturnValue aTagIndicator = ETag );

        /**
        * Get current position.
        * Returns current parsing position, which
        * can be used as parameter for SetPosition afterwards to go back
        * to old parsing position.
        */
        IMPORT_C TInt GetPosition();
        
        /**
        * Set position.
        * Returns a Symbian OS error code. aPos indicates the position to 
        * which section parser should go.  
        * SetPosition can be used to set parsing position, e.g. to rewind 
        * back to some old position retrieved with GetPosition.
        */
        IMPORT_C TInt SetPosition( TInt aPos );        
        
        /**
        * Create a section.
        */
        void SetData( TPtr aData,
                        TInt aStartPos, 
                        TInt aLength );

        /**
        * Returns a current section.
        */
        const TPtrC Des();

    public:     // Functions from base classes

    protected:  // New functions

    protected:  // Functions from base classes

    private:

        /**
        * C++ default constructor.
        */
        CStifSectionParser( const TInt aLength );

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();

        /**
        * Generic start and end position parser for given data.
        * Returns an error code.
        */
        TInt ParseStartAndEndPos(   TPtrC aSection,
                                    const TDesC& aStartTag,
                                    TTagToReturnValue aTagIndicator,
                                    TInt& aStartPos,
                                    TInt& aEndPos,
                                    TInt& aLength );
                                    
        /**
        * Generic search for end-of-line.
        * Returns position before linefeed and 
        * lex is posotioned to the beginning of next line.
        */
        TInt GotoEndOfLine( TLex& lex );                            

        /**
        * Substracts line from the begining of selected text.
        */
        TPtrC SubstractLine( const TPtrC& aText );
		
		/**
		* Searches for selected tag in a text.
		*/		
        TInt FindTag( const TDesC& aText, const TDesC& aTag, TInt& aStartPos, TInt& aEndPos );
        
    public:     // Data

    protected:  // Data

    private:    // Data

        /**
        * Parsed section with SectionL() or SubSectionL() method.
        */
        TPtr                iSection;

        /**
        * Section length definitions.
        */
        HBufC*              iHBufferSection;

        /**
        * Section length
        */
        TInt                iLength;

        /**
        * Indicates position where start the parsing.
        */
        TInt                iSkipAndMarkPos;

        /**
        * For line and item line parsing. Indicator is GetLine() or
        * GetItemLineL() method used.
        */
        TBool               iLineIndicator;

        /**
        * Indicates position where we are currently(subsection).
        */
        TInt                iSubOffset;

    public:     // Friend classes

        /**
        * For testing operations to get length information.
        */
        friend class CTestParser;

    protected:  // Friend classes

    private:    // Friend classes

    };

#endif      // STIF_SECTION_PARSER_H

// End of File
