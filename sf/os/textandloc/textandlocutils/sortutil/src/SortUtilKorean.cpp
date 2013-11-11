/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:     Korean SortUtil implementation
*
*/


// INCLUDE FILES
#include "SortUtilKorean.h"
#include "SortUtilImpl.h"
#include "SortUtilImplExport.h"

// Unnamed namespace for local definitions
namespace
    {
    // Address range for hangul syllables.
    const TUint KSyllablesBegin = 0xac00;
    const TUint KSyllablesEnd = 0xd7a3;

    // Address range for hangul compatibility jamos.
    const TUint KCompatibilityJamosBegin = 0x3130;
    const TUint KCompatibilityJamosEnd = 0x318f;
    
    // Address range for hangul jamos.
    const TUint KJamosBegin = 0x1100;
    const TUint KJamosEnd = 0x11FF;
    
    /**
     * Character classes for Korean sorting, in sorting order.
     */
    enum TKoreanCharClass
        {
        ECharClassSpecial = 1,
        ECharClassDigit,
        ECharClassLatin,
        ECharClassCompatibilityJamo,
        ECharClassJamo,
        ECharClassHangulSyllable
        };

    /**
     * Returns the Korean character class of aChar.
     */
    TKoreanCharClass CharClass( TUint aChar )
        {
        const TChar ch( aChar );
        
        // Hangul syllable
        if ( aChar >= KSyllablesBegin && aChar <= KSyllablesEnd )
            {
            return ECharClassHangulSyllable;
            }
        // Compatibility jamo
        else if ( aChar >= KCompatibilityJamosBegin && 
            aChar <= KCompatibilityJamosEnd )
            {
            return ECharClassCompatibilityJamo;
            }
        // Jamo    
        else if ( aChar >= KJamosBegin && aChar <= KJamosEnd ) 
            {
            return ECharClassJamo;
            }
        // Latin character    
        else if ( ( aChar >= 'A' && aChar <= 'Z' ) ||
            ( aChar >= 'a' && aChar <= 'z' ) )
            {
            return ECharClassLatin;
            }
        // Digit
        else if ( ( aChar >= '0' && aChar <= '9' ) ||
            ( aChar >= 0xff10 && aChar <= 0xff19 ) )
            {
            return ECharClassDigit;
            }
        // Special characters
        else
            {
            return ECharClassSpecial;
            }
        }

    /**
     * Compares two characters with Korean comparison rules.
     */
    TInt KoreanCompareC( TUint aLhs, TUint aRhs )
        {
        // First compare character classes
        const TInt lhsCls = CharClass( aLhs );
        const TInt rhsCls = CharClass( aRhs );
        return lhsCls - rhsCls;
        }

    /**
     * Compares two strings with Korean comparison rules.
     */
    TInt KoreanCompareS( const TDesC& aLhs, const TDesC& aRhs )
        {
        TInt i;
        TInt result( 0 );
        TInt prevCompPos( 0 );
        const TInt minLen = Min( aLhs.Length(), aRhs.Length() );
        
        for ( i = 0; i < minLen && result == 0; ++i )
            {
            result = KoreanCompareC( aLhs[i], aRhs[i] );
            if ( 0 == result )
                {
                TPtrC lPart;
                TPtrC rPart;
                TInt orgClass = CharClass( aLhs[ i ] );
                
                if ( ECharClassHangulSyllable == orgClass ||
                    ECharClassCompatibilityJamo == orgClass ||
                    ECharClassJamo == orgClass )
                    {
                    int j = i;
                    for ( ; j < aLhs.Length(); ++j )
                        {
                        if ( orgClass != CharClass( aLhs[ j ] ) )
                            {
                            break;
                            }
                        }
                    lPart.Set( aLhs.Mid( prevCompPos, j - prevCompPos ) );
                    for ( j = i; j < aRhs.Length(); ++j )
                        {
                        if ( orgClass != CharClass( aRhs[ j ] ) )
                            {
                            break;
                            }
                        }
                    rPart.Set( aRhs.Mid( prevCompPos, j - prevCompPos ) );
                    }
                else
                    {
                    lPart.Set( aLhs.Mid( i, 1 ) );
                    rPart.Set( aRhs.Mid( i, 1 ) );
                    }
                result = lPart.CompareC( rPart );
                }
            else
                {
                prevCompPos = i;
                }
            }
            
        if ( result == 0 )
            {
            if ( i < aLhs.Length() )
                {
                // aLhs is longer than aRhs
                result += User::Collate( aLhs[ i ] );
                }
            if ( i < aRhs.Length() )
                {
                // aRhs is longer that aLhs
                result -= User::Collate( aRhs[ i ] );
                }
            }
            
        return result;
        }
    }  // namespace

// Only export in DLL
EXPORT_C MSortUtil* SortUtilFactoryFunctionL()
    {
    MSortUtil* util = new (ELeave) TSortUtilKorean;
    return util;
    }

inline TSortUtilKorean::TSortUtilKorean()
    {
    }

TSortUtilKorean::~TSortUtilKorean()
    {
    }

TInt TSortUtilKorean::CompareItems( 
    const MSortKeyArray& aLhs, 
    const MSortKeyArray& aRhs ) const
    {    
    TInt result = 0;
    TInt lhsIndex = 0;
    TInt rhsIndex = 0;
    
    do  
        {
        const TDesC& lhsText = 
            SortUtilImpl::FindNextNonEmptyKey( aLhs, ESortKeyBasic, lhsIndex );
        const TDesC& rhsText = 
            SortUtilImpl::FindNextNonEmptyKey( aRhs, ESortKeyBasic, rhsIndex );
        result = KoreanCompareS( lhsText, rhsText );
        if ( result != 0 )
            {
            return result;
            }
        }
    while ( lhsIndex <= aLhs.SortKeyCount() && rhsIndex < aRhs.SortKeyCount() );
    return result;
    }

// End of file
