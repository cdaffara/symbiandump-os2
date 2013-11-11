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


// INCLUDES FILES
#include "jplangutil.h"
#include "jplangutil.tables.h"

// LOCAL DEFINES

// LOCAL CONSTANTS AND ENUMS

#if defined(_DEBUG)
_LIT( KJPLangUtilPanic, "JPLangUtil" );
enum TJPLangUtilPanicCodes
    {
    EErrNotKatakana = 0x777,
    EErrNotVoicedKatakana
    };
#endif

enum TConvDirection
    {
    EHalfToFullWidth,
    EFullToHalfWidth
    };

const TInt KHalfWidthKatakanaRange = 59;
const TInt KHalfWidthIndex = 0;
const TInt KFullWidthIndex = 1;
const TInt KFullWidthVoicableKatakanaRange = 23;
const TInt KFullWidthSemiVoicableKatakanaRange = 5;
const TInt KHalfWidthSpecialCharRange = 21;

const TText KHalfWidthKatakanaVoicedSoundMark     = 0xff9e;
const TText KHalfWidthKatakanaSemiVoicedSoundMark = 0xff9f;

// The following Full-width (semi-)voiced markers are
// *also* considered Hiragana, per Unicode spec.
const TText KFullWidthKatakanaVoicedSoundMark     = 0x309b;
const TText KFullWidthKatakanaSemiVoicedSoundMark = 0x309c;

const TText KHalfWidthKatakanaLowerBound = 0xff65;
const TText KHalfWidthKatakanaUpperBound = 0xff9f;
const TText KFullWidthKatakanaLowerBound = 0x30a0;
const TText KFullWidthKatakanaUpperBound = 0x30ff;

const TText KCJKUnifiedIdiographLowerBound = 0x4e00;
const TText KCJKUnifiedIdiographUpperBound = 0x9fa5;
const TText KCJKUnifiedIdeographExtALowerBound = 0x3400;
const TText KCJKUnifiedIdeographExtAUpperBound = 0x4dbf;

const TText KHalfToFullWidthASCIIOffset = 0xfee0;
const TText KHalfWidthASCIILowerBound = 0x0021; // Leaves out space (0x0020)
const TText KHalfWidthASCIIUpperBound = 0x007e;
const TText KFullWidthASCIILowerBound = 0xff01;
const TText KFullWidthASCIIUpperBound = 0xff5e;

const TUint KFullWidthHiraganaSmallA = 0x3041;
const TUint KFullWidthHiraganaVU = 0x3094;
const TUint KFullWidthHiraganaU = 0x3046;
const TUint KFullWidthHiraganaVoicedSound = 0x309B;
const TUint KFullWidthKatakanaSmallA = 0x30a1;
const TUint KFullWidthKatakanaSmallVU = 0x30F4;


// ============================== LOCAL CLASSES ================================

/**
 *  This is an internal utility class used by the JPLangUtil class
 *  and is intended for encapsulation only.
 *
 *  @lib JPLangUtil.lib
 *  @since 2.6
 */
class UnicodeTextUtil
    {
    public: // Query Functions

        /**
        * Determines if aUnicodeChar is a base Katakana that is capable
        * of being converted into either a Vioced or Semi-voiced Full-width
        * Katakana character.
        * If aSemiVoiced is true, then the check is only performed
        * for Semi-voiced conversion capability.
        *
        * @param aUnicodeChar The character to be tested.
        * @param aSemiVoiced  Whether or not the character is to be tested
        *                     as convertable to a Semi-voiced Full-width
        *                     Katakana, rather than just voiced.
        *
        * @return ETrue is the character is in fact convertable to either
        *         a voiced or semi-voiced fullwidth katakana character,
        *         depending on the aSemiVoiced flag.
        */
        static TBool IsFullWidthVoicedConvertableHalfWidthBaseKatakana
                             ( TText aUnicodeChar, TBool aSemiVoiced = EFalse );

        /**
        * Determines if aUnicodeChar is a  Vioced or Semi-voiced Full-width
        * Katakana character.
        * If aSemiVoiced is true, then the check is only performed
        * for Semi-voiced.
        *
        * @param aUnicodeChar The character to be tested.
        * @param aSemiVoiced  Whether or not the character is a Semi-voiced
        *                     Full-width Katakana, rather than just voiced.
        *
        * @return ETrue is the character is in fact a voiced or semi-voiced
        *         fullwidth katakana character, depending on the aSemiVoiced
        *         flag.
        */
        static TBool IsFullWidthVoicedKatakana( TText aUnicodeChar,
                                                TBool aSemiVoiced = EFalse);

    public: // Conversion Functions

        /**
        * Converts one Katakana character from either Half to Full-width
        * or vice versa, depending on the direction specified by aDirection
        *
        * @param aDirection Direction to convert.
        * @param aKatakanaChar The character to convert.
        *
        * @return The opposite width Katakana character.
        */
        static TChar ConvertKatakanaChar( TConvDirection aDirection,
                                          TText aKatakanaChar );

        /**
        * Converts one voiced Katakana character from either Half to Full-width
        * or vice versa, depending on the direction specified by aDirection,
        * and appends the converted character to the descriptor aTarget.
        * In the case that it is from Full to Half-width, it is necessary to
        * convert it into a base-katakana + (semi-)voiced mark pair, thus
        * the necessity for passing in the TDes reference return type.
        * In the case of converting from Half to Full-width, it is only
        * necessary to pass in a base-katakana that has a (semi-)voiced
        * Full-width counterpart.
        *
        * @param aDirection Direction to convert.
        * @param aVoicedKatakana The character to convert.
        * @param aTarget The descriptor to append the converted character(s) to.
        * @param aSemiVoiced ETrue if performing a Semi-voiced conversion.
        *                    Default is EFalse.
        */
        static void ConvertVoicedKatakanaCharAndAppendToTarget(
                                                   TConvDirection aDirection,
                                                   TText aVoicedKatakana,
                                                   TDes& aTarget,
                                                   TBool aSemiVoiced = EFalse );

        /**
        * Converts all special characters *in-place* from either Half to
        * Full-width or vice versa, depending on the direction specified
        * by aDirection
        *
        * @param aDirection Direction to convert.
        * @param aUnicodeText The character to convert.
        *
        * @return The number of converted special characters.
        */
        static TInt ConvertSpecialCharactersInPlace( TConvDirection aDirection,
                                                     TDes16& aUnicodeText );

        /**
        * Converts all ASCII *in-place* from either Half to
        * Full-width or vice versa, depending on the direction specified
        * by aDirection
        *
        * @param aDirection Direction to convert.
        * @param aUnicodeText The character to convert.
        *
        * @return The number of converted ASCII-range characters.
        */
        static TInt ConvertASCIIInPlace( TConvDirection aDirection,
                                         TDes16& aUnicodeText );
    };

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// UnicodeTextUtil::IsFullWidthVoicedConvertableHalfWidthBaseKatakana
// Determines if aUnicodeChar is a base Katakana that is capable
// of being converted into either a Vioced or Semi-voiced
// Full-width character.
// If aSemiVoiced is true, then the check is only performed
// for Semi-voiced conversion capability.
// Returns: ETrue if it is convertable.
// -----------------------------------------------------------------------------
//
TBool UnicodeTextUtil::IsFullWidthVoicedConvertableHalfWidthBaseKatakana
                                       ( TText aUnicodeChar, TBool aSemiVoiced )
    {
    if( aSemiVoiced )
        {
        return ( aUnicodeChar >= 0xff8a && aUnicodeChar <= 0xff8e );
        }
    else
        {
        return ( ( aUnicodeChar == 0xff73 ) ||
                 ( aUnicodeChar >= 0xff76 && aUnicodeChar <= 0xff7f ) ||
                 ( aUnicodeChar >= 0xff80 && aUnicodeChar <= 0xff84 ) ||
                 ( aUnicodeChar >= 0xff8a && aUnicodeChar <= 0xff8e ) ||
                 ( aUnicodeChar == 0xff9c ) ||
                 ( aUnicodeChar == 0xff66 ) );
        }
    }

// -----------------------------------------------------------------------------
// UnicodeTextUtil::IsFullWidthVoicedKatakana
// Determines if aUnicodeChar is Full-width Katakana.
// Returns: ETrue if it is Full-width Katakana.
// -----------------------------------------------------------------------------
//
TBool UnicodeTextUtil::IsFullWidthVoicedKatakana( TText aUnicodeChar,
                                                  TBool aSemiVoiced )
    {
    const TInt voicableRange( aSemiVoiced ?
                              KFullWidthSemiVoicableKatakanaRange :
                              KFullWidthVoicableKatakanaRange );
    const TText* const (*voicedTable) = aSemiVoiced ?
                              KHalfWidthBaseToFullWidthSemiVoicedKatakanaTable :
                              KHalfWidthBaseToFullWidthVoicedKatakanaTable;
    for( TInt i( 0 ); i < voicableRange; ++i )
        {
        if( aUnicodeChar == voicedTable[i][KFullWidthIndex] )
            {
            return ETrue;
            }
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// UnicodeTextUtil::ConvertKatakanaChar
// Converts one Katakana Char in the direction specified by aDirection.
// Assumes aKatakanaChar is within range.
// Returns: A TText containing either the varient character or the character
//          itself, in the case there is no conversion.
// -----------------------------------------------------------------------------
//
TChar UnicodeTextUtil::ConvertKatakanaChar( TConvDirection aDirection,
                                            TText aKatakanaChar )
    {
    __ASSERT_DEBUG(
        JPLangUtil::IsKatakana( aKatakanaChar ) &&
        !IsFullWidthVoicedKatakana( aKatakanaChar ),
        User::Panic( KJPLangUtilPanic, EErrNotKatakana )
    );
    TChar convChar( aKatakanaChar );
    const TInt directionIndex( ( aDirection == EHalfToFullWidth ) ?
                                   KHalfWidthIndex : KFullWidthIndex );
    for( TInt i( 0 ); i < KHalfWidthKatakanaRange; ++i )
        {
        if( aKatakanaChar ==
                KHalfToFullWidthKatakanaCharTable[i][directionIndex] )
            {
            convChar = KHalfToFullWidthKatakanaCharTable[i][!directionIndex];
            break;
            }
        }
    return convChar;
    }

// -----------------------------------------------------------------------------
// UnicodeTextUtil::ConvertVoicedKatakanaChar
// Converts one Voiced Katakana Char in the direction specified by aDirection,
// and appends to the descriptor, aTarget.
// If aSemiVoiced is ETrue, then the code point is converted to the Semi-voiced
// variant.
// Assumes aVoicedKatakana is within range.
// -----------------------------------------------------------------------------
//
void UnicodeTextUtil::ConvertVoicedKatakanaCharAndAppendToTarget(
                                                      TConvDirection aDirection,
                                                      TText aVoicedKatakana,
                                                      TDes& aTarget,
                                                      TBool aSemiVoiced )
    {
    __ASSERT_DEBUG(
        IsFullWidthVoicedConvertableHalfWidthBaseKatakana( aVoicedKatakana ) ||
        IsFullWidthVoicedKatakana( aVoicedKatakana, aSemiVoiced ),
        User::Panic( KJPLangUtilPanic, EErrNotVoicedKatakana )
    );

    const TInt  voicableRange( aSemiVoiced ?
                               KFullWidthSemiVoicableKatakanaRange :
                               KFullWidthVoicableKatakanaRange );
    const TText* const (*voicedTable) = aSemiVoiced ?
                              KHalfWidthBaseToFullWidthSemiVoicedKatakanaTable :
                              KHalfWidthBaseToFullWidthVoicedKatakanaTable;
    if( aDirection == EHalfToFullWidth )
        {
        // In the case of Half Width, only the base character is needed for
        // the conversion
        for( TInt i( 0 ); i < voicableRange; ++i )
            {
            if( aVoicedKatakana == voicedTable[i][KHalfWidthIndex] )
                {
                const TChar uniChar( voicedTable[i][KFullWidthIndex] );
                aTarget.Append( uniChar );
                break;
                }
            }
        }
    else
        {
        for( TInt i( 0 ); i < voicableRange; ++i )
            {
            if( aVoicedKatakana == voicedTable[i][KFullWidthIndex] )
                {
                const TChar uniChar( voicedTable[i][KHalfWidthIndex] );
                aTarget.Append( uniChar );
                break;
                }
            }
        const TChar voicedSoundMark( aSemiVoiced ?
                                     KHalfWidthKatakanaSemiVoicedSoundMark :
                                     KHalfWidthKatakanaVoicedSoundMark );
        aTarget.Append( voicedSoundMark );
        }
    }

// -----------------------------------------------------------------------------
// UnicodeTextUtil::ConvertSpecialCharactersInPlace
// Converts all special characters in the aUnicodeText descriptor in-place
// that have both Full and Half-width variants.
// Conversion occurs in the direction specified by aDirection.
// Returns: The total number of characters converted.
// -----------------------------------------------------------------------------
//
TInt UnicodeTextUtil::ConvertSpecialCharactersInPlace
                             ( TConvDirection aDirection, TDes16& aUnicodeText )
    {
    TInt totalConverted( 0 );
    const TInt directionIndex( ( aDirection == EHalfToFullWidth ) ?
                                 KHalfWidthIndex : KFullWidthIndex );
    for( TInt i( 0 ); i < aUnicodeText.Length(); ++i )
        {
        const TText uniChar( aUnicodeText[i] );
        for( TInt j( 0 ); j < KHalfWidthSpecialCharRange; ++j )
            {
            if( uniChar == KHalfToFullWidthSpecialCharTable[j][directionIndex] )
                {
                aUnicodeText[i] =
                    KHalfToFullWidthSpecialCharTable[j][!directionIndex];
                totalConverted++;
                break;
                }
            }
        }
    return totalConverted;
    }

// -----------------------------------------------------------------------------
// UnicodeTextUtil::ConvertASCIIInPlace
// Converts all ASCII *in-place* from either Half to Full-width or vice versa,
// depending on the direction specified by aDirection.
// Returns: The total number of characters converted.
// -----------------------------------------------------------------------------
//
TInt UnicodeTextUtil::ConvertASCIIInPlace( TConvDirection aDirection,
                                           TDes16& aUnicodeText )
    {
    TInt totalConverted( 0 );
    TText lowerBound( ( aDirection == EHalfToFullWidth ) ?
                        KHalfWidthASCIILowerBound :
                        KFullWidthASCIILowerBound );
    TText upperBound( ( aDirection == EHalfToFullWidth ) ?
                        KHalfWidthASCIIUpperBound :
                        KFullWidthASCIIUpperBound );
    TInt offset( ( aDirection == EHalfToFullWidth ) ?
                   KHalfToFullWidthASCIIOffset :
                   -KHalfToFullWidthASCIIOffset );
    for( TInt i( 0 ); i < aUnicodeText.Length(); ++i )
        {
        const TText uniChar( aUnicodeText[i] );
        if( uniChar >= lowerBound && uniChar <= upperBound )
            {
            TText convertedChar( static_cast<TText>( uniChar + offset ) );
            aUnicodeText[i] = convertedChar;
            totalConverted++;
            }
        }
    return totalConverted;
    }

// EXPORTED FUNCTIONS

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// JPLangUtil::ConvertHalfToFullWidth
// Converts all Half-width conformant text (including ASCII, Special Characters
// and Katakana) found in aUnicodeSource to their Full-width counterparts and
// places the resulting text into aUnicodeTarget.
// (detailed information about the parameters and return values can be found
//  in the header file)
// -----------------------------------------------------------------------------
//
EXPORT_C TInt JPLangUtil::ConvertHalfToFullWidth( const TDesC16& aUnicodeSource,
                                                  TDes16& aUnicodeTarget )
    {
    TInt totalConverted( 0 );
    // Special Characters are handled in the Katakana conversion...
    totalConverted =  ConvertHalfToFullWidthKatakana( aUnicodeSource,
                                                      aUnicodeTarget );
    totalConverted += UnicodeTextUtil::ConvertASCIIInPlace( EHalfToFullWidth,
                                                            aUnicodeTarget );
    return totalConverted;
    }

// -----------------------------------------------------------------------------
// JPLangUtil::ConvertFullToHalfWidth
// Converts all Full-width conformant text found in aUnicodeSource to their
// Full-width counterparts and places the resulting text into aUnicodeTarget.
// Only those characters with existing Half-width variants are converted. There
// will be a 2-for-1 conversion for each Full-width Voiced and Semi-voiced
// Katakana character.
// (detailed information about the parameters and return values can be found in
// the header file)
// -----------------------------------------------------------------------------
//
EXPORT_C TInt JPLangUtil::ConvertFullToHalfWidth( const TDesC16& aUnicodeSource,
                                                  TDes16& aUnicodeTarget )
    {
    TInt totalConverted( 0 );
    // Special Characters are handled in the Katakana conversion...
    totalConverted =  ConvertFullToHalfWidthKatakana( aUnicodeSource,
                                                      aUnicodeTarget );
    totalConverted += UnicodeTextUtil::ConvertASCIIInPlace( EFullToHalfWidth,
                                                            aUnicodeTarget );
    return totalConverted;
    }

// -----------------------------------------------------------------------------
// JPLangUtil::ConvertHalfToFullWidthKatakana
// Converts Half-width Katakana and Special Character text found in
// aUnicodeSource to their Full-width counterparts and places the resulting text
// into aUnicodeTarget.
// (detailed information about the parameters and return values can be found in
// the header file)
// -----------------------------------------------------------------------------
//
EXPORT_C TInt JPLangUtil::ConvertHalfToFullWidthKatakana
                       ( const TDesC16& aUnicodeSource, TDes16& aUnicodeTarget )
    {
    TInt totalConverted( 0 );
    const TInt length( aUnicodeSource.Length() );
    if( length > aUnicodeTarget.MaxLength() )
        {
        return KErrTooBig;
        }
    aUnicodeTarget.Zero();
    for( TInt i( 0 ); i < length; ++i )
        {
        const TText uniChar( aUnicodeSource[i] );
        // Check if the next character is a Half Width Katakana (Semi-)Voiced
        // Sound Mark and if the current character + the voiced sound mark have
        // a Full Width counterpart
        if( i + 1 < length )
            {
            const TBool isVoiced(
            ( UnicodeTextUtil::IsFullWidthVoicedConvertableHalfWidthBaseKatakana
                  ( uniChar ) &&
              ( aUnicodeSource[i + 1] == KHalfWidthKatakanaVoicedSoundMark )
            ) );

            const TBool isSemiVoiced(
            ( UnicodeTextUtil::IsFullWidthVoicedConvertableHalfWidthBaseKatakana
                  ( uniChar, ETrue ) &&
              ( aUnicodeSource[i + 1] == KHalfWidthKatakanaSemiVoicedSoundMark )
            ) );

            if( isVoiced || isSemiVoiced )
                {
                UnicodeTextUtil::ConvertVoicedKatakanaCharAndAppendToTarget(
                    EHalfToFullWidth,
                    uniChar,
                    aUnicodeTarget,
                    isSemiVoiced
                );
                i++; // Skip the (semi-)voice marker
                totalConverted++;
                continue;
                }
            }
        // If not, then just convert directly if in range
        if( uniChar >= KHalfWidthKatakanaLowerBound &&
            uniChar <= KHalfWidthKatakanaUpperBound )
            {
            aUnicodeTarget.Append(
                UnicodeTextUtil::ConvertKatakanaChar( EHalfToFullWidth,
                                                      uniChar )
            );
            totalConverted++;
            }
        // Else this is not Half Width Katakana, so copy directly...
        else
            {
            const TChar uniCharacter( uniChar );
            aUnicodeTarget.Append( uniCharacter );
            }
        }

    // Now handle special characters
    // This logic may be moved into this function to avoid another
    // loop over the text.
    totalConverted +=
        UnicodeTextUtil::ConvertSpecialCharactersInPlace( EHalfToFullWidth,
                                                          aUnicodeTarget );

    return totalConverted;
    }

// -----------------------------------------------------------------------------
// JPLangUtil::ConvertFullToHalfWidthKatakana
// Converts Full-width Katakana and Special Character text found in
// aUnicodeSource to their Half-width counterparts and places the resulting text
// into aUnicodeTarget. There will be a 2-for-1 conversion for each Full-width
// Voiced and Semi-voiced Katakana character.
// (detailed information about the parameters and return values can be found in
// the header file)
// -----------------------------------------------------------------------------
//
EXPORT_C TInt JPLangUtil::ConvertFullToHalfWidthKatakana
                       ( const TDesC16& aUnicodeSource, TDes16& aUnicodeTarget )
    {
    TInt totalConverted( 0 );
    const TInt length( aUnicodeSource.Length() );
    const TInt maxLength( aUnicodeTarget.MaxLength() );
    if( length > maxLength )
        {
        return KErrTooBig;
        }
    aUnicodeTarget.Zero();
    for( TInt i( 0 ); i < length; ++i )
        {
        const TText uniChar( aUnicodeSource[i] );
        // First check if this is this Full Width Katakana
        if( ( uniChar >= KFullWidthKatakanaLowerBound &&
              uniChar <= KFullWidthKatakanaUpperBound ) ||
            ( uniChar == KFullWidthKatakanaVoicedSoundMark ||
              uniChar == KFullWidthKatakanaSemiVoicedSoundMark ) )
            {
            // Then check if it is (Semi-)Voiced and convert it properly
            const TBool isVoiced(
                UnicodeTextUtil::IsFullWidthVoicedKatakana( uniChar )
            );
            const TBool isSemiVoiced(
                UnicodeTextUtil::IsFullWidthVoicedKatakana( uniChar, ETrue )
            );
            if( isVoiced || isSemiVoiced )
                {
                if( aUnicodeTarget.Length() + 2 > maxLength )
                    {
                    // This descriptor can't hold the new data
                    aUnicodeTarget.Zero();
                    return KErrTooBig;
                    }
                UnicodeTextUtil::ConvertVoicedKatakanaCharAndAppendToTarget(
                    EFullToHalfWidth,
                    uniChar,
                    aUnicodeTarget,
                    isSemiVoiced
                );
                totalConverted++;
                }
            else
                {
                if( aUnicodeTarget.Length() + 1 > maxLength )
                    {
                    // This descriptor can't hold the new data
                    aUnicodeTarget.Zero();
                    return KErrTooBig;
                    }
                aUnicodeTarget.Append(
                    UnicodeTextUtil::ConvertKatakanaChar(
                        EFullToHalfWidth,
                        uniChar
                    )
                );
                totalConverted++;
                }
            }
        // This is not Full Width Katakana, so copy directly...
        else
            {
            if( aUnicodeTarget.Length() + 1 > maxLength )
                {
                // This descriptor can't hold the new data
                aUnicodeTarget.Zero();
                return KErrTooBig;
                }
            const TChar uniCharacter( uniChar );
            aUnicodeTarget.Append( uniCharacter );
            }
        }

    // Now handle special characters
    // This logic may be moved into this function to avoid another loop over
    // the text
    totalConverted +=
        UnicodeTextUtil::ConvertSpecialCharactersInPlace( EFullToHalfWidth,
                                                          aUnicodeTarget );

    return totalConverted;
    }

// -----------------------------------------------------------------------------
// JPLangUtil::ConvertFullHiragnaToFullKatakana
// Converts Full-width Hiragana and Special Character text found in
// aUnicodeSource to their Full-width counterparts and places the
// resulting text into aUnicodeTarget.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt JPLangUtil::ConvertFullHiragnaToFullKatakana
                       ( const TDesC16& aUnicodeSource, TDes16& aUnicodeTarget )
    {
    TInt totalConverted( 0 );
    const TInt length( aUnicodeSource.Length() );
    const TInt maxLength( aUnicodeTarget.MaxLength() );
    if( length > maxLength )
        {
        return KErrTooBig;
        }

    const TUint comp = KFullWidthKatakanaSmallA - KFullWidthHiraganaSmallA;

    aUnicodeTarget.Zero();
    for( TInt i( 0 ); i < length; ++i )
        {
        const TText uniChar( aUnicodeSource[i] );
        TText uniChar2(0);
        if (i + 1 < length)
            {
            uniChar2 = aUnicodeSource[i+1];
            }
        // First check if this is this Full Width Katakana
        if (KFullWidthHiraganaSmallA <= uniChar && uniChar <= KFullWidthHiraganaVU)
            {
            if (uniChar == KFullWidthHiraganaU
             && uniChar2 == KFullWidthHiraganaVoicedSound)
                {
                aUnicodeTarget.Append(KFullWidthKatakanaSmallVU);
                totalConverted++;
                i++;
                }
            else
                {
                TUint katakana = uniChar + comp;
                if (IsKatakana(katakana))
                    {
                    aUnicodeTarget.Append(katakana);
                    totalConverted++;
                    }
                }
            }
        else
            {
            aUnicodeTarget.Append(uniChar);
            totalConverted++;
            }
        }

    // Now handle special characters
    // This logic may be moved into this function to avoid another loop over
    // the text
    totalConverted +=
        UnicodeTextUtil::ConvertSpecialCharactersInPlace( EFullToHalfWidth,
                                                          aUnicodeTarget );

    return totalConverted;
    }

// -----------------------------------------------------------------------------
// JPLangUtil::IsKatakana
// Determines whether or not the character is Katakana
// (detailed information about the parameters and return values can be found in
// the header file)
// -----------------------------------------------------------------------------
//
EXPORT_C TBool JPLangUtil::IsKatakana( const TText aUnicodeChar )
    {
    return ( ( aUnicodeChar >= KFullWidthKatakanaLowerBound &&
               aUnicodeChar <= KFullWidthKatakanaUpperBound ) ||
             ( aUnicodeChar >= KHalfWidthKatakanaLowerBound &&
               aUnicodeChar <= KHalfWidthKatakanaUpperBound ) ||
             ( aUnicodeChar == KFullWidthKatakanaVoicedSoundMark ||
               aUnicodeChar == KFullWidthKatakanaSemiVoicedSoundMark ) );
    }

// -----------------------------------------------------------------------------
// JPLangUtil::IsHiragana
// Determines whether or not the character is Hiragana
// (detailed information about the parameters and return values can be found in
// the header file)
// -----------------------------------------------------------------------------
//
EXPORT_C TBool JPLangUtil::IsHiragana( const TText aUnicodeChar )
    {
    return ( ( aUnicodeChar >= 0x3041 && aUnicodeChar <= 0x3096 ) ||
             ( aUnicodeChar >= 0x3099 && aUnicodeChar <= 0x309f ) );
    }

// -----------------------------------------------------------------------------
// JPLangUtil::IsKanji
// Determines whether or not the character is a CJK Unified Ideograph (ExtA)
// (detailed information about the parameters and return values can be found in
// the header file)
// -----------------------------------------------------------------------------
//
EXPORT_C TBool JPLangUtil::IsKanji( const TText aUnicodeChar )
    {
    return( ( aUnicodeChar >= KCJKUnifiedIdiographLowerBound &&
              aUnicodeChar <= KCJKUnifiedIdiographUpperBound ) ||
            ( aUnicodeChar >= KCJKUnifiedIdeographExtALowerBound &&
              aUnicodeChar <= KCJKUnifiedIdeographExtAUpperBound ) );
    }

// -----------------------------------------------------------------------------
// JPLangUtil::IsHalfWidth
// Determines whether or not the character is UNICODE defined Half-width.
// (detailed information about the parameters and return values can be found in
// the header file)
// -----------------------------------------------------------------------------
//
EXPORT_C TBool JPLangUtil::IsHalfWidth( const TText aUnicodeChar )
    {
    return // HW Katakana
           ( ( aUnicodeChar >= 0xff61 && aUnicodeChar <= 0xff9f ) ||
           // ASCII
             ( aUnicodeChar >= 0x0020 && aUnicodeChar <= 0x007e ) ||
           // Special Characters
             ( aUnicodeChar == 0x00a2 || aUnicodeChar == 0x00a3 ||
               aUnicodeChar == 0x00a5 || aUnicodeChar == 0x00a6 ||
               aUnicodeChar == 0x00a9 || aUnicodeChar == 0x00ac ||
               aUnicodeChar == 0x00af || aUnicodeChar == 0x2985 ||
               aUnicodeChar == 0x2986 ) ||
           // HW Symbol Variants
             ( aUnicodeChar >= 0xffe8 && aUnicodeChar <= 0xffee ) );
    }

// -----------------------------------------------------------------------------
// JPLangUtil::IsFullWidth
// Determines whether or not the character is UNICODE defined Full-width.
// Essentially, all code values that are not Half-width.
// (detailed information about the parameters and return values can be found in
// the header file)
// -----------------------------------------------------------------------------
//
EXPORT_C TBool JPLangUtil::IsFullWidth( const TText aUnicodeChar )
    {
        return !IsHalfWidth( aUnicodeChar );
    }

// End of File
