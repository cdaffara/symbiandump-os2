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
* Description:   ISO2022kr conversion plugin
*
*/


// INCLUDES
#include <e32std.h>
#include <charconv.h>
#include <convgeneratedcpp.h>
#include <ecom/implementationproxy.h>
#include "cp949table.h"
#include "charactersetconverter.h"

static const TUint KBitsForNonStandardStates = 0x03;
static const TUint KShiftedToKSCState = 0x01;

static const TUint KMaxSizeOfTmpBuffer = 1024;

static const TUint8 KMaxAscii = 0x9f;

_LIT8(KLit8EscapeSequence, "\x1b\x24\x43");

#define SHIFT_IN_BYTE  0x0F
#define SHIFT_OUT_BYTE 0x0E

typedef enum
{
    EISO2022Initialize,
    EISO2022Ascii,
    EISO2022KSC
} TISO2022FromUniState;

// New Interface class
class CISO2022KRImplementation : public CCharacterSetConverterPluginInterface
{
    public:
        virtual const TDesC8& ReplacementForUnconvertibleUnicodeCharacters();

        virtual TInt ConvertFromUnicode(
            CCnvCharacterSetConverter::TEndianness aDefaultEndiannessOfForeignCharacters, 
            const TDesC8& aReplacementForUnconvertibleUnicodeCharacters, 
            TDes8& aForeign, 
            const TDesC16& aUnicode, 
            CCnvCharacterSetConverter::TArrayOfAscendingIndices& aIndicesOfUnconvertibleCharacters );

        virtual TInt ConvertToUnicode(
            CCnvCharacterSetConverter::TEndianness aDefaultEndiannessOfForeignCharacters, 
            TDes16& aUnicode, 
            const TDesC8& aForeign, 
            TInt& aState, 
            TInt& aNumberOfUnconvertibleCharacters, 
            TInt& aIndexOfFirstByteOfFirstUnconvertibleCharacter );

        virtual TBool IsInThisCharacterSetL(
            TBool& aSetToTrue, 
            TInt& aConfidenceLevel, 
            const TDesC8& );

        static CISO2022KRImplementation* NewL();

        virtual ~CISO2022KRImplementation();
    private:
        CISO2022KRImplementation();
};

// FUNCTION DEFINITIONS
const TDesC8& CISO2022KRImplementation::ReplacementForUnconvertibleUnicodeCharacters()
	{
	return CnvCp949Table::ReplacementForUnconvertibleUnicodeCharacters();
	}

TInt CISO2022KRImplementation::ConvertFromUnicode(
    CCnvCharacterSetConverter::TEndianness aDefaultEndiannessOfForeignCharacters, 
    const TDesC8& aReplacementForUnconvertibleUnicodeCharacters, 
    TDes8& aForeign, 
    const TDesC16& aUnicode, 
    CCnvCharacterSetConverter::TArrayOfAscendingIndices& aIndicesOfUnconvertibleCharacters)
	{
    TInt ret;
    TInt currPos = 3;
    TUint outputConversionFlags = 0;
    TUint inputConversionFlags = CCnvCharacterSetConverter::EInputConversionFlagAppend;
    TISO2022FromUniState currState = EISO2022Initialize;
    TUint8 shiftByte = 0;
    TPtr8 shiftBytePtr(NULL, 0);

    aForeign.SetLength(0);

    /* Start with escape sequence */
    aForeign.Append( KLit8EscapeSequence );

    ret = CCnvCharacterSetConverter::DoConvertFromUnicode( CnvCp949Table::ConversionData(),
                                                           aDefaultEndiannessOfForeignCharacters,
                                                           aReplacementForUnconvertibleUnicodeCharacters,
                                                           aForeign,
                                                           aUnicode,
                                                           aIndicesOfUnconvertibleCharacters,
                                                           outputConversionFlags, 
                                                           inputConversionFlags );
    /* Append shift in and out bytes as needed */
    while( currPos < aForeign.Length() )
        {
        TUint8 *currChar = (TUint8 *)aForeign.Mid(currPos).Ptr();
        if( *currChar > KMaxAscii )
            { /* KSC character */
            if( currState != EISO2022KSC )
                { /* Insert shift out byte */
                shiftByte = SHIFT_OUT_BYTE;
                currState = EISO2022KSC;
                }

            /* Clear the 8th bit */
            *currChar = (*currChar & ~(0x80));
            }
        else
            { /* ASCII character */
            if( currState != EISO2022Ascii )
                { /* Insert shift in byte */
                shiftByte = SHIFT_IN_BYTE;
                currState = EISO2022Ascii;
                }
            }

        if( shiftByte )
            {
            if( (aForeign.Length() + 1) > aForeign.MaxLength() )
                { /* Make room for shift byte */
                if( aForeign[ (aForeign.Length() - 1) ] > KMaxAscii )
                    { /* Drop a dual byte KSC character */
                    aForeign.SetLength( aForeign.Length() - 2 );
                    }
                else
                    { /* Drop a single byte ASCII character */
                    aForeign.SetLength( aForeign.Length() - 1 );
                    }
                    /* Increase unconverted amount */
                    ret++;
                /* TBD, propably should try to fix aIndicesOfUnconvertibleCharacters
                        if possible */
                }
                shiftBytePtr.Set( &shiftByte, 1, 1 );
                aForeign.Insert( currPos, shiftBytePtr );
                currPos++;
                shiftByte = 0;
            }

        /* Skip current character */
        currPos++;
        }

    return ret;
    }

TInt CISO2022KRImplementation::ConvertToUnicode(
    CCnvCharacterSetConverter::TEndianness aDefaultEndiannessOfForeignCharacters, 
    TDes16& aUnicode, 
    const TDesC8& aForeign, 
    TInt& aState, 
    TInt& aNumberOfUnconvertibleCharacters, 
    TInt& aIndexOfFirstByteOfFirstUnconvertibleCharacter)
	{
    TInt err;
    TInt ret = 0;
    TInt currPos = 0;
    TInt convPos = 0;
    TInt shiftInPos = KErrNotFound;
    TInt shiftOutPos = KErrNotFound;
    TInt shiftPos = KErrNotFound;
    TInt escPos = KErrNotFound;
    TPtrC8 currSegment;
    TPtrC8 convSegment;
    TBool changeState = EFalse;

    TUint outputConversionFlags = 0;
    TUint inputConversionFlags = CCnvCharacterSetConverter::EInputConversionFlagAppend;
    TInt numberOfUnconvertibleCharacters = 0;
    TInt indexOfFirstByteOfFirstUnconvertibleCharacter = 0;
    aNumberOfUnconvertibleCharacters = 0;

    while( currPos < aForeign.Length() )
        {

        currSegment.Set( aForeign.Mid( currPos ) );

        /* First change state if needed */
        if( changeState )
            {
            changeState = EFalse;
            if( (aState & KBitsForNonStandardStates) == KShiftedToKSCState )
                { /* Switch back to default ASCII */
                aState &= ~(KShiftedToKSCState);
                }
            else
                { /* Switch to KSC */
                aState |= KShiftedToKSCState; 
                }
            }

        /* Search for escape which should be skipped */
        escPos = currSegment.Find( KLit8EscapeSequence );
        
        /* Search for shift in byte */
        shiftInPos = currSegment.Locate( SHIFT_IN_BYTE );

        /* Search for shift out byte */
        shiftOutPos = currSegment.Locate( SHIFT_OUT_BYTE );

        /* Set shift pos according to found shift bytes */
        if( shiftInPos == KErrNotFound &&
            shiftOutPos == KErrNotFound )
            { /* Neither found */
            shiftPos = KErrNotFound;
            }
        else
            {
            if( (shiftInPos != KErrNotFound) &&
                ((shiftInPos < shiftOutPos) || (shiftOutPos == KErrNotFound)) )
                { /* shift in is nearer or shift out not found */
                shiftPos = shiftInPos;
                /* Set state change if needed */
                if( (aState & KBitsForNonStandardStates) == KShiftedToKSCState )
                    {
                    changeState = ETrue;
                    }
                }
            else
                { /* shift out must be nearer or shift in not fouind */
                shiftPos = shiftOutPos;
                /* Set state change if needed */
                if( (aState & KBitsForNonStandardStates) != KShiftedToKSCState )
                    {
                    changeState = ETrue;
                    }
                }
            }

        if( shiftPos == KErrNotFound )
            { /* Shift byte not found, same coding for the rest of the data */
            if( escPos == KErrNotFound )
                { /* No escape sequence either, just convert the rest */
                convSegment.Set( currSegment );
                }
            }
        else if( ((escPos != KErrNotFound) && (shiftPos < escPos)) ||
                 (escPos == KErrNotFound) )
            { /* Shift byte found and it comes before escape sequence or no escape
                 sequence was found, convert data preceeding the shift byte if shift
                 byte isn't the first character */
                if( shiftPos == 0 )
                { /* No data to convert preceeds the shift byte, just skip it and continue */
                    currPos += 1;
                    continue;
                }
                convSegment.Set( currSegment.Left( shiftPos ) );
                /* Clear to prevent convert to escape sequence */
                escPos = KErrNotFound;
            }

        if( escPos != KErrNotFound )
            { /* Escape sequence found before any shift bytes,
                 clear possible state change and convert data
                 preceeding the escape sequence if
                 escape sequence is not at the beginning */
            changeState = EFalse;
            if( escPos == 0 )
                { /* No data to convert preceeds the escape sequence, just skip it continue */
                currPos += KLit8EscapeSequence().Length();
                continue;
                }
            convSegment.Set( currSegment.Left( escPos ) );
            }

        if( (aState & KBitsForNonStandardStates) == KShiftedToKSCState )
            { /* Convert KSC encoded */
            HBufC8 *tmpForeign = NULL;

            if( (convSegment.Length() & 0x1) )
                { /* KSC should have even amount of bytes */
                ret = CCnvCharacterSetConverter::EErrorIllFormedInput;
                }
            else
                {
                convPos = 0;
                while( convPos < convSegment.Length() )
                    {
                    TRAP( err, tmpForeign = HBufC8::NewL( KMaxSizeOfTmpBuffer ) );
                    if( err != KErrNone )
                        {
                        User::Panic( _L("ISO-2022-KR"), err );
                        }

                    if( convSegment.Length() < KMaxSizeOfTmpBuffer )
                        { /* Convert whole segment */
                        tmpForeign->Des().Copy( convSegment );
                        }
                    else
                        { /* Convert in chunks */
                        if( (convPos + KMaxSizeOfTmpBuffer) >= convSegment.Length() )
                            { /* Last chunk */
                            tmpForeign->Des().Copy( convSegment.Mid( convPos ) );
                            }
                        else
                            {
                            tmpForeign->Des().Copy( convSegment.Mid( convPos, KMaxSizeOfTmpBuffer ) );
                            }
                        }

                    TUint8 *chars = (TUint8 *)tmpForeign->Des().Ptr();
                    for( TInt i = 0 ; i < tmpForeign->Length() ; i++ )
                        { /* Set highest bit in characters */
                        chars[i] |= 0x80;
                        }

                    numberOfUnconvertibleCharacters = 0;
                    ret = CCnvCharacterSetConverter::DoConvertToUnicode( CnvCp949Table::ConversionData(),
                                                                         aDefaultEndiannessOfForeignCharacters,
                                                                         aUnicode, *tmpForeign,
                                                                         numberOfUnconvertibleCharacters,
                                                                         indexOfFirstByteOfFirstUnconvertibleCharacter,
                                                                         outputConversionFlags,
                                                                         inputConversionFlags );
                    if( numberOfUnconvertibleCharacters != 0 &&
                        aNumberOfUnconvertibleCharacters == 0 )
                        { /* First uncovertible found, set index relative to actual input buffer*/
                        aIndexOfFirstByteOfFirstUnconvertibleCharacter = (currPos + convPos + indexOfFirstByteOfFirstUnconvertibleCharacter);
                        }

                    aNumberOfUnconvertibleCharacters += numberOfUnconvertibleCharacters;

                    if( ret < 0 )
                        { /* Some error, break the loop,
                             errors are handled later */
                        delete tmpForeign;
                        break;
                        }

                    if( ret > 0 )
                        { /* Not all were converted, fix return value
                             to be relative to convSegment and break the loop */
                        ret = (convSegment.Length() - convPos - tmpForeign->Length() + ret);
                        delete tmpForeign;
                        break;
                        }

                    convPos += tmpForeign->Length();
                    delete tmpForeign;
                    }
                }
            }
        else
            { /* Convert ASCII encoded by default, KSC can be used without setting highest bit */
                numberOfUnconvertibleCharacters = 0;
                ret = CCnvCharacterSetConverter::DoConvertToUnicode( CnvCp949Table::ConversionData(),
                                                                     aDefaultEndiannessOfForeignCharacters,
                                                                     aUnicode, convSegment,
                                                                     numberOfUnconvertibleCharacters,
                                                                     indexOfFirstByteOfFirstUnconvertibleCharacter,
                                                                     outputConversionFlags,
                                                                     inputConversionFlags );
                if( numberOfUnconvertibleCharacters != 0 &&
                    aNumberOfUnconvertibleCharacters == 0 )
                    { /* First uncovertible found, set index relative to actual input buffer*/
                    aIndexOfFirstByteOfFirstUnconvertibleCharacter = currPos + indexOfFirstByteOfFirstUnconvertibleCharacter;
                    }
                aNumberOfUnconvertibleCharacters += numberOfUnconvertibleCharacters;
            }

        if( ret < 0 )
            { /* Error during conversion */
            return ret;
            }
        else if( ret > 0 )
            { /* Not all characters where converted, return
                 value indicating how many bytes in total are left unconverted */
            return (aForeign.Length() - currPos - convSegment.Length() + ret);
            }

        /* Increase to skip converted data */
        currPos += convSegment.Length();
        if( escPos != KErrNotFound )
            { /* Increase to skip escape sequence */
            currPos += KLit8EscapeSequence().Length();
            }
        else if( shiftPos != KErrNotFound )
            { /* Increase to skip shift byte */
            currPos += 1;
            }

        }

    return 0;
	}


TBool CISO2022KRImplementation::IsInThisCharacterSetL(
    TBool& aSetToTrue, 
    TInt& aConfidenceLevel, 
    const TDesC8& /*aBuf*/)
	{
/*	
    aSetToTrue=ETrue;
    aConfidenceLevel=50;
    
    TUint8 ch(0);
    for (TInt i=0;i<aBuf.Length();i++)
        {
        ch=aBuf[i];
        if (ch<0x7F)
            {
            continue;
            }
        else if (0xa1<=ch&&ch<=0xfe)
            {
            i++;
            __ASSERT_DEBUG(i<aBuf.Length(),User::Panic(_L("IS2022KR"),__LINE__));
            }
        else
            {
            aConfidenceLevel=0;
            aSetToTrue=EFalse;
            break;
            }
        }    
	return aSetToTrue;
*/
	aSetToTrue=ETrue;
	aConfidenceLevel=0;
	return EFalse;
	}

CISO2022KRImplementation* CISO2022KRImplementation::NewL()
    {
    CISO2022KRImplementation* self = new(ELeave) CISO2022KRImplementation;
    return self;
    }

CISO2022KRImplementation::CISO2022KRImplementation()
    {
    //default constructor.. do nothing
    }

CISO2022KRImplementation::~CISO2022KRImplementation()
    {
    //default destructor .. do nothing
    }

// ECOM CREATION FUNCTION
const TImplementationProxy ImplementationTable[] = 
    {
    // Note: This is the same UID as defined in old mmp-file
    // Used also in 12221212.rss ( implementation_uid )
    IMPLEMENTATION_PROXY_ENTRY( 0x20010101, CISO2022KRImplementation::NewL )
    };

EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
    {
    aTableCount = sizeof( ImplementationTable ) / sizeof(TImplementationProxy);
    return ImplementationTable;
    }

