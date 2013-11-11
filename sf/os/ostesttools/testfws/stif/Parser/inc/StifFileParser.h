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
* Description: This file contains the header file of the CStifFileParser.
*
*/

#ifndef STIF_FILE_PARSER_H
#define STIF_FILE_PARSER_H

//  INCLUDES
#include <f32file.h>
#include <e32std.h>
#include <e32base.h>
#include "cstackdeprecated.h"
#include <StifTestModule.h>
#include "StifParser.h"

// CONSTANTS
_LIT(KIncludeKeyword, "INCLUDE");

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
// CStifFileParser is a STIF Test Framework StifParser class.
// Class contains a configuration file parsing operations without loading it into memory.


NONSHARABLE_CLASS (CStifFileParser) : public CBase
{
    public:     // Enumerations
		// Indication what kind of char sequence is to be found
        enum TWhatToFind
      	{
        	EStart,             // '//' or '/*' or '"' signs
        	EQuota,             // '"' sign
        	EEndOfComment       // '*/' sequence
    	};

		// Indication if beginning or end of section is to be found
        enum TSectionFind
        {
        	ESectionStart,
        	ESectionEnd
        };

    private:    // Enumerations

    public:     // Constructors and destructor

        /**
        * Two-phased constructor.
        * Creating parser with given handle to file. For default all
        * information will be parsed(ENoComments).
        */
        static CStifFileParser* NewL(RFs& aFs,
                                     RFile &aFile,
                                     TBool aIsUnicode,
                                     CStifParser::TCommentType aCommentType = CStifParser::ENoComments);

        /**
        * Destructor.
        */
        ~CStifFileParser();

    public:     // New functions

        /**
        * Open and read configuration source and parses a required section.
        * This method will parse next section after the earlier section if
        * aSeeked parameter is equal 1.
        * If configuration file includes several sections with both start and
        * end tags so aSeeked parameter seeks the required section. The aSeeked
        * parameters indicates section that will be parsed.
        * If start tag is not given, returned section begins from the beginning of
        * the file. If end tag is not given, returned sections ends at the end of
        * the file.
        * If section is not found and first occurence of section is seeked, function
        * will return NULL, but if second (or further) occurence is seeked, function
        * will leave with KErrNotFound (for compability with CStifParser::NextSectionL
		* reason).
        */
		HBufC* NextSectionL(const TDesC& aStartTag,
                            const TDesC& aEndTag,
                            TInt& aOffset,
                            TInt aSeeked = 1);

    public:     // Functions from base classes

    protected:  // New functions

    protected:  // Functions from base classes

    private:

        /**
        * C++ default constructor.
        */
        CStifFileParser(CStifParser::TCommentType aCommentType);

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL(RFs& aFs,
                        RFile &aFile,
                        TBool aIsUnicode);

        /**
        * Reads configuration source and returns a complete line.
        * This method will return also and end of line sequence.
        */
		TBool ReadLineL(TPtr& aLineBuffer,
		                TPtr& aEndOfLineBuffer);

        /**
        * Parses a given line and removes all c-style comments from it (and #-style comments).
        * Result is passed in destination buffer.
        */
		void ReplaceCommentsLineL(TPtr& aSrcBufPtr,
		                          TPtr& aDstBufPtr,
								  TWhatToFind& aFind);

        /**
        * Parses a given line and removes all #-style comments.
        * Result is passed in destination buffer.
        */
	 	void ReplaceHashCommentsLineL(TPtr& aSrcBufPtr,
 	                                  TPtr& aDstBufPtr);

        /**
        * Closes all files on file stack and clears the stack.
        */
 	    void ClearFileStack(void);

        /**
        * Pops RFile handle from file stack and sets correct current file handle.
        */
        void PopFromFileStack(void);

        /**
        * Opens file and pushes it to stack.
        */
        void PushFileToStackL(const TDesC& aFileName);

        /**
        * Deletes all descriptors assigned to array and empties array.
        */
        void ClearSectionLinesArray(void);

    public:     // Data

    protected:  // Data

    private:    // Data

        /**
        * For file opening.
        */
        RFs         iFileServer;

        /**
        * For file opening. This is the first (base) file. Later other files may be opened if
        * they are included to base one.
        */
        RFile       iBaseFile;

        /**
        * Comment type's indication.
        */
        CStifParser::TCommentType iCommentType;

        /**
        * Flag indicating if file is unicode.
        */
        TBool       iIsUnicode;
        
        /**
        * How many bytes per char in file (2 for unicode, 1 for non-unicode).
        */
        TInt 		iBytesPerChar;
        
        /**
        * Handle to file which is currently read.
        */
        RFile*      iCurrentFile;

        /**
        * Stack of opened files (it does not contain base file).
        */
        CStackDeprecated<RFile, EFalse>* iFileStack;

        /**
        * Array of lines belonging to seeked section.
        */
		RPointerArray<HBufC> iSectionLines;

		/**
		* Array of already included files (to avoid loop in includes).
		*/
		RPointerArray<HBufC> iFileNames;

		/**
		* Buffer with eol sequence. Filled when include found and used after last line of inlcuded file.
		*/
		TBuf<2>     iEolBuf;

    public:     // Friend classes

    protected:  // Friend classes

    private:    // Friend classes
};

#endif      // STIF_FILE_PARSER_H

// End of File
