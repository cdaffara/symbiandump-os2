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
* CStifItemParser.
*
*/

#ifndef STIF_ITEM_PARSER_H
#define STIF_ITEM_PARSER_H

//  INCLUDES
#include <f32file.h>
#include <e32std.h>
#include <e32base.h>

// CONSTANTS
const TInt KMaxTag = 256; // A tag maximum length

// MACROS
// None

// DATA TYPES
// None

// FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
// None

// CLASS DECLARATION

// DESCRIPTION
// CStifItemParser is a STIF Test Framework StifParser class.
// Class contains a configuration file parsing operations.

class CStifItemParser
        :public CBase
    {
    public:     // Enumerations

        // Parsing type can be given when parsing string(GetString and
        // GetNextString). This enumeration indicates parsing type.
        // ENormalParsing:
        //      - Indicates normal parsing without any modifications to
        //        parsed information
        // EQuoteStyleParsing:
        //      - Indicates special parsing. This style of parsing gives to
        //        quote(" ") characters special meaning.
        //      - Information between quotes is handled as a one string. Quotes
        //        not included to information.
        enum TParsingType
            {
            ENormalParsing,     // Mode on(Default)
            EQuoteStyleParsing, // Mode off
            };

    private:    // Enumerations

    public:     // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        IMPORT_C static CStifItemParser* NewL( TPtrC aSection,
                                                TInt aStartPos,
                                                TInt aLength );

        /**
        * Destructor.
        */
        ~CStifItemParser();

    public:     // New functions

        /**
        * Get a string with a tag.
        * Returns an error code and a reference to the parsed string.
         *If start tag is empty the first string will be parsed and returned.
        */
        IMPORT_C TInt GetString( const TDesC& aTag, TPtrC& aString );

        /**
        * Get next string.
        * Returns an error code and a reference to the parsed string.
        * GetString or GetInt or GetChar must be called before call
        * GetNextString method.
        */
        IMPORT_C TInt GetNextString( TPtrC& aString );

        /**
        * Get next string with a tag.
        * Returns an error code and a reference to the parsed string.
        * If start tag is empty the next string will be parsed and returned.
        * GetString or GetInt or GetChar must be called before call
        * GetNextString method.
        */
        IMPORT_C TInt GetNextString( const TDesC& aTag, TPtrC& aString );

        /**
        * Get a integer(TInt) with a tag.
        * Returns an error code and a reference to the parsed integer.
        * If start tag is empty the first integer will be parsed and returned.
        */
        IMPORT_C TInt GetInt( const TDesC& aTag, TInt& aInteger );

        /**
        * Get next integer(TInt).
        * Returns an error code and a reference to the parsed integer.
        * GetString or GetInt or GetChar must be called before call
        * GetNextInt method.
        */
        IMPORT_C TInt GetNextInt( TInt& aInteger );

        /**
        * Get next integer(TInt) with a tag.
        * Returns an error code and a reference to the parsed integer.
        * If start tag is empty the next integer will be parsed and returned.
        * GetString or GetInt or GetChar must be called before call
        * GetNextInt method.
        */
        IMPORT_C TInt GetNextInt( const TDesC& aTag, TInt& aInteger );

        /**
        * Get a integer(TUint) with a tag.
        * Returns an error code and a reference to the parsed integer.
        * If start tag is empty the first integer will be parsed and returned.
        * With TRadix parameter can convert a number into different
        * presentation(EBinary, EOctal, EDecimal and EHex).
        */
        IMPORT_C TInt GetInt( const TDesC& aTag,
                                TUint& aInteger,
                                TRadix aRadix = EDecimal );

        /**
        * Get next integer(TUint).
        * Returns an error code and a reference to the parsed integer.
        * GetString or GetInt or GetChar must be called before call
        * GetNextInt method.
        * With TRadix parameter can convert a number into different
        * presentation(EBinary, EOctal, EDecimal and EHex).
        */
        IMPORT_C TInt GetNextInt( TUint& aInteger,
                                    TRadix aRadix = EDecimal );

        /**
        * Get next integer(TUint) with a tag.
        * Returns an error code and a reference to the parsed integer.
        * If start tag is empty the next integer will be parsed and returned.
        * GetString or GetInt or GetChar must be called before call
        * GetNextInt method.
        * With TRadix parameter can convert a number into different
        * presentation(EBinary, EOctal, EDecimal and EHex).
        */
        IMPORT_C TInt GetNextInt( const TDesC& aTag,
                                    TUint& aInteger,
                                    TRadix aRadix = EDecimal );

        /**
        * Get a character with a tag.
        * Returns an error code and a reference to the parsed character.
        * If start tag is empty the first character will be parsed and
        * returned.
        */
        IMPORT_C TInt GetChar( const TDesC& aTag, TChar& aCharacter );

        /**
        * Get next character.
        * Returns an error code and a reference to the parsed character.
        * GetString or GetInt or GetChar must be called before call
        * GetNextChar method.
        */
        IMPORT_C TInt GetNextChar( TChar& aCharacter );

        /**
        * Get next character with a tag.
        * Returns an error code and a reference to the parsed character.
        * If start tag is empty the next character will be parsed and returned.
        * GetString or GetInt or GetChar must be called before call
        * GetNextChar method.
        */
        IMPORT_C TInt GetNextChar( const TDesC& aTag, TChar& aCharacter );

        
        /**
        * Get remaining strings.
        * Returns an error code and a reference to the remainder of the 
        * parsed line.
        * Note: This method does not support the quote(TParsingType) feature,
        * only GetString and GetNextString methods include support.
        */
        IMPORT_C TInt Remainder( TPtrC& aString );
        
        /**
        * Parsing type can be given when parsing string(GetString and
        * GetNextString). See TParsingType enumeration for more
        * inforamtion.
        */
        IMPORT_C TInt SetParsingType( CStifItemParser::TParsingType aType );
        
        /**
        * Get current parsing type. Please see TParsingType enumeration for
        * more inforamtion.
        */
        IMPORT_C CStifItemParser::TParsingType ParsingType();

    public:     // Functions from base classes

    protected:  // New functions

    protected:  // Functions from base classes

    private:

        /**
        * C++ default constructor.
        */
        CStifItemParser( TPtrC aSection,
                            TInt aStartPos,
                            TInt aLength );

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();

        /**
        * Generig start and end position parser for given data.
        * Returns an error code.
        */
        TInt ParseStartAndEndPos( TPtrC aSection,
                                    const TDesC& aStartTag,
                                    TInt& aStartPos,
                                    TInt& aEndPos,
                                    TInt& aLength,
                                    TInt& aExtraEndPos );

    public:     // Data

    protected:  // Data

    private:    // Data

        /**
        * Parsed section with GetItemLineL() or GetNextItemLineL() method.
        */
        TPtrC                           iItemLineSection;

        /**
        * Indicates position where start the parsing.
        */
        TInt                            iItemSkipAndMarkPos;

        /**
        * Indicator is GetString(), GetInt() or GetChar() called.
        */
        TBool                           iGetMethodsIndicator;

        /**
        * Parsing type indicator for GetString and GetNextString use.
        */
        CStifItemParser::TParsingType   iParsingType;

    public:     // Friend classes

        /**
        * For testing operations to get length information.
        */
        friend class CTestParser;

    protected:  // Friend classes

    private:    // Friend classes

    };

#endif      // STIF_ITEM_PARSER_H

// End of File
