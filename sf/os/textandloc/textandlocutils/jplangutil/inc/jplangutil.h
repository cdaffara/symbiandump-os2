/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*     A utility for providing Japanese Language-specific (UNICODE) functions.
*
*
*/


#ifndef __JPLANGUTIL_H__
#define __JPLANGUTIL_H__

// INCLUDES
#include <e32std.h>

// CONSTANTS

// CLASS DECLARATION

/**
 *  This is the only class provided buy the JPLangUtil Library,
 *  and is intended for encapsulation only.
 *  As such, it should not be instantiated.
 *
 *  @lib JPLangUtil.lib
 *  @since 2.6
 */
class JPLangUtil
    {
    public: // Conversion API

        /**
        * Converts all Half-width conformant text (including ASCII, Special
        * Characters and Katakana) found in aUnicodeSource to their Full-width
        * counterparts and places the resulting text into aUnicodeTarget.
        *
        * @param aUnicodeSource Descriptor that contains the text to be
        *        converted.
        * @param aUnicodeTarget The target descriptor for the converted text.
        *
        * @return Either a positive integer signifying the count of converted
        *         characters, or the negative error code KErrToBig when the
        *         target descriptor cannot hold all the converted text.
        */
        IMPORT_C static TInt ConvertHalfToFullWidth
            (const TDesC16& aUnicodeSource, TDes16& aUnicodeTarget );

        /**
        * Converts all Full-width conformant text found in aUnicodeSource to
        * their Half-width counterparts and places the resulting text into
        * aUnicodeTarget. Only those characters with existing Half-width
        * variants are converted. There will be a 2-for-1 conversion for each
        * Full-width Voiced and Semi-voiced Katakana character.
        *
        * @param aUnicodeSource Descriptor that contains the text to be
        *        converted.
        * @param aUnicodeTarget The target descriptor for the converted text.
        *
        * @return Either a positive integer signifying the count of converted
        *         characters, or the negative error code KErrToBig when the
        *         target descriptor cannot hold all the converted text.
        */
        IMPORT_C static TInt ConvertFullToHalfWidth
            ( const TDesC16& aUnicodeSource, TDes16& aUnicodeTarget );

        /**
        * Converts Half-width Katakana and Special Character text found in
        * aUnicodeSource to their Full-width counterparts and places the
        * resulting text into aUnicodeTarget.
        *
        * @param aUnicodeSource Descriptor that contains the text to be
        *        converted.
        * @param aUnicodeTarget The target descriptor for the converted text.
        *
        * @return Either a positive integer signifying the count of converted
        *         characters, or the negative error code KErrToBig when the
        *         target descriptor cannot hold all the converted text.
        */
        IMPORT_C static TInt ConvertHalfToFullWidthKatakana
            ( const TDesC16& aUnicodeSource, TDes16& aUnicodeTarget );

        /**
        * Converts Full-width Katakana and Special Character text found in
        * aUnicodeSource to their Half-width counterparts and places the
        * resulting text into aUnicodeTarget. There will be a 2-for-1 conversion
        * for each Full-width Voiced and Semi-voiced Katakana character.
        *
        * @param aUnicodeSource Descriptor that contains the text to be
        *        converted.
        * @param aUnicodeTarget The target descriptor for the converted text.
        *
        * @return Either a positive integer signifying the count of converted
        *         characters, or the negative error code KErrToBig when the
        *         target descriptor cannot hold all the converted text.
        */
        IMPORT_C static TInt ConvertFullToHalfWidthKatakana
            ( const TDesC16& aUnicodeSource, TDes16& aUnicodeTarget );

        /**
        * Converts Full-width Hiragana and Special Character text found in
        * aUnicodeSource to their Full-width counterparts and places the
        * resulting text into aUnicodeTarget.
        *
        * @param aUnicodeSource Descriptor that contains the text to be
        *        converted.
        * @param aUnicodeTarget The target descriptor for the converted text.
        *
        * @return Either a positive integer signifying the count of converted
        *         characters, or the negative error code KErrToBig when the
        *         target descriptor cannot hold all the converted text.
        */
        IMPORT_C static TInt ConvertFullHiragnaToFullKatakana
            ( const TDesC16& aUnicodeSource, TDes16& aUnicodeTarget );

    public: // Character Category Query API

        /**
        * Returns ETrue if aUnicodeChar is Katakana.
        * This includes both Full and Half-width Katakana.
        *
        * @param aUnicodeChar The unicode character to test.
        *
        * @return Etrue : aUnicodeChar is Katakana
        */
        IMPORT_C static TBool IsKatakana( const TText aUnicodeChar );

        /**
        * Returns ETrue if aUnicodeChar is Hiragana.
        * There are no Half-width Hiragana variants.
        *
        * @param aUnicodeChar The unicode character to test.
        *
        * @return Etrue : aUnicodeChar is Hiragana
        */
        IMPORT_C static TBool IsHiragana( const TText aUnicodeChar );

        /**
        * Returns ETrue if aUnicodeChar is Kanji, or CJK ideographic
        * All characters in the CJK ideographic range are Full-width.
        *
        * @param aUnicodeChar The unicode character to test.
        *
        * @return Etrue : aUnicodeChar is Kanji
        */
        IMPORT_C static TBool IsKanji( const TText aUnicodeChar );

        /**
        * Returns ETrue if aUnicodeChar is Half-width according to
        * the UNICODE definition of the term.
        *
        * @param aUnicodeChar The unicode character to test.
        *
        * @return Etrue : aUnicodeChar is Half-width
        */
        IMPORT_C static TBool IsHalfWidth( const TText aUnicodeChar );

        /**
        * Returns ETrue if aUnicodeChar is Full-width.
        * Essentially, all text that is not Half-width.
        *
        * @param aUnicodeChar The unicode character to test.
        *
        * @return Etrue : aUnicodeChar is Full-width
        */
        IMPORT_C static TBool IsFullWidth( const TText aUnicodeChar );

    };

#endif //  __JPLANGUTIL_H__

// End of file
