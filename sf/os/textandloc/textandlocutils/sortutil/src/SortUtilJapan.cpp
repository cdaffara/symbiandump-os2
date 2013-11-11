/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies). 
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
*
*/






// INCLUDE FILES
#include "SortUtilJapan.h"
#include "SortUtilImpl.h"
#include "SortUtilImplExport.h"
#include <jplangutil.h>

// Unnamed namespace for local definitions
namespace
    {

    /**
     * Character classes for Japanese sorting, in sorting order.
     */
    enum TJapaneseCharClass
        {
        ECharClassKana,
        ECharClassLatin,
        ECharClassDigit,
        ECharClassKanji,
        ECharClassSpecial
        };

    /**
     * Returns the Japanese character class of aChar.
     */
    TJapaneseCharClass CharClass(TUint aChar)
        {
        const TChar ch(aChar);
        // Special characters
        if (ch.IsSpace() || ch.IsPunctuation() || ch.IsControl()
            || aChar >= 0xF0000 && aChar <= 0xFFFD)
            {
            return ECharClassSpecial;
            }
        // Digit
        else if (aChar >= '0' && aChar <= '9' ||
            aChar >= 0xFF10 && aChar <= 0xFF19)
            {
            return ECharClassDigit;
            }
        // Latin
        else if (aChar >= 'A' && aChar <= 'Z' ||
            aChar >= 'a' && aChar <= 'z' ||
            aChar >= 0x00C0 && aChar <= 0x1EF3 && aChar != 0x00D7 && aChar != 0x00F7 ||
            aChar >= 0xFF21 && aChar <= 0xFF3A ||
            aChar >= 0xFF41 && aChar <= 0xFF5A)
            {
            return ECharClassLatin;
            }
        // Kana
        else if (JPLangUtil::IsKatakana(static_cast<TText>(aChar)) ||
            JPLangUtil::IsHiragana(static_cast<TText>(aChar)))
            {
            return ECharClassKana;
            }
        // Kanji
        else if (JPLangUtil::IsKanji(static_cast<TText>(aChar)))
            {
            return ECharClassKanji;
            }
        // All other charcters are "special"
        else
            {
            return ECharClassSpecial;
            }
        }

    /**
     * Compares two characters with Japanese comparison rules.
     */
    TInt JapaneseCompareC(TUint aLhs, TUint aRhs)
        {
        // First compare character classes
        const TInt lhsCls = CharClass(aLhs);
        const TInt rhsCls = CharClass(aRhs);
        return lhsCls - rhsCls;
        }

    /**
     * Compares two strings with Japanese comparison rules.
     */
    TInt JapaneseCompareS(const TDesC& aLhs, const TDesC& aRhs)
        {
        TInt result = 0;

        const TInt minLen = Min(aLhs.Length(), aRhs.Length());
        TInt i;
        TInt prevCompPos(0);
        for (i = 0; i < minLen && result == 0; ++i)
            {
            result = JapaneseCompareC(aLhs[i], aRhs[i]);
            if (result == 0)
                {
                TInt orgClass = CharClass(aLhs[i]);
                TPtrC lPart;
                TPtrC rPart;
                if (orgClass == ECharClassKana)
                    {
                    int j = i;
                    for (; j < aLhs.Length(); ++j)
                        {
                        if (orgClass != CharClass(aLhs[j]))
                            {
                            break;
                            }
                        }
                    lPart.Set(aLhs.Mid(prevCompPos, j - prevCompPos));
                    for (j = i; j < aRhs.Length(); ++j)
                        {
                        if (orgClass != CharClass(aRhs[j]))
                            {
                            break;
                            }
                        }
                    rPart.Set(aRhs.Mid(prevCompPos, j - prevCompPos));
                    result = lPart.CompareC(rPart);
                    if (result != 0)
                        {
                        const TInt minKanaLen = Min(lPart.Length(), rPart.Length());
                        TPtrC lKanaPart(lPart.Left(minKanaLen));
                        TPtrC rKanaPart(rPart.Left(minKanaLen));
                        TInt resultKana = lKanaPart.MatchC(rKanaPart);
                        if (resultKana == 0)
                            {
                            TInt maxKanaLen = i+minKanaLen;
                            if (i+minKanaLen+1 >= minLen)
                                {
                                if (maxKanaLen < aLhs.Length()
                                 || maxKanaLen < aRhs.Length())
                                    {
                                    result = 0;
                                    i += (minKanaLen-1);
                                    }
                                else
                                    {
                                    result = lKanaPart.CompareC(rKanaPart);;
                                    }
                                }
                            else
                                {
                                result = 0;
                                i += (minKanaLen-1);
                                }
                            }
                        }

                    }
                else
                    {
                    lPart.Set(aLhs.Mid(i, 1));
                    rPart.Set(aRhs.Mid(i, 1));
                    result = lPart.CompareC(rPart);
                    }
                }
            else
                {
                prevCompPos = i;
                }
            }
        if (result == 0)
            {
            if (i < aLhs.Length())
                {
                // aLhs is longer than aRhs
                result += User::Collate(aLhs[i]);
                }
            if (i < aRhs.Length())
                {
                // aRhs is longer that aLhs
                result -= User::Collate(aRhs[i]);
                }
            }
        return result;
        }
    }  // namespace

// Only export in DLL
EXPORT_C MSortUtil* SortUtilFactoryFunctionL()
    {
    MSortUtil* util = new (ELeave) TSortUtilJapan;
    return util;
    }

inline TSortUtilJapan::TSortUtilJapan()
    {
    }

TSortUtilJapan::~TSortUtilJapan()
    {
    }

TInt TSortUtilJapan::CompareItems
        (const MSortKeyArray& aLhs,
        const MSortKeyArray& aRhs) const
    {
    TInt result(0);
    TInt lhsIndex(0);
    TInt rhsIndex(0);

    // Compare only pronounciation keys in the first pass
    do
        {
        const TDesC& lhsText =
            SortUtilImpl::FindNextNonEmptyKey(aLhs, ESortKeyPronounciation, lhsIndex);
        const TDesC& rhsText =
            SortUtilImpl::FindNextNonEmptyKey(aRhs, ESortKeyPronounciation, rhsIndex);
        if (lhsText.Length() > 0)
            {
            // lhs has pronounciation key
            if (rhsText.Length() > 0)
                {
                // Both lhs and rhs have pronounciation key: compare the key texts
                result = JapaneseCompareS(lhsText, rhsText);
                }
            else
                {
                // Only lhs has pronounciation key
                result = -1;
                }
            }
        else
            {
            // lhs does not have pronounciation key
            if (rhsText.Length() > 0)
                {
                // Only rhs has pronounciation key
                result = 1;
                }
            }

        if (result != 0)
            {
            return result;
            }
        }
    while (lhsIndex <= aLhs.SortKeyCount() && rhsIndex < aRhs.SortKeyCount());

    // No difference found with pronounciation keys: compare basic keys
    lhsIndex = 0;
    rhsIndex = 0;
    do
        {
        const TDesC& lhsText =
            SortUtilImpl::FindNextNonEmptyKey(aLhs, ESortKeyBasic, lhsIndex);
        const TDesC& rhsText =
            SortUtilImpl::FindNextNonEmptyKey(aRhs, ESortKeyBasic, rhsIndex);
        result = JapaneseCompareS(lhsText, rhsText);
        if (result != 0)
            {
            return result;
            }
        }
    while (lhsIndex <= aLhs.SortKeyCount() && rhsIndex < aRhs.SortKeyCount());

    return result;
    }

// End of file
