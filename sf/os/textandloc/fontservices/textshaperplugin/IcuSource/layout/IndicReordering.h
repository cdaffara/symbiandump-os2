/*
 *
 * (C) Copyright IBM Corp. 1998-2005 - All Rights Reserved
 *
 */

#ifndef __INDICREORDERING_H
#define __INDICREORDERING_H

/**
 * \file
 * \internal
 */

#include "LETypes.h"
#include "OpenTypeTables.h"

U_NAMESPACE_BEGIN

// Characters that get refered to by name...
#define C_SIGN_ZWNJ           0x200C
#define C_SIGN_ZWJ            0x200D

// Character class values
#define CC_RESERVED               		0U
#define CC_VOWEL_MODIFIER         		1U
#define CC_STRESS_MARK            		2U
#define CC_INDEPENDENT_VOWEL      		3U
#define CC_INDEPENDENT_VOWEL_2    		4U
#define CC_CONSONANT              		5U
#define CC_CONSONANT_WITH_NUKTA   		6U
#define CC_NUKTA                  		7U
#define CC_DEPENDENT_VOWEL        		8U
#define CC_SPLIT_VOWEL_PIECE_1    		9U
#define CC_SPLIT_VOWEL_PIECE_2   		10U
#define CC_SPLIT_VOWEL_PIECE_3   		11U
#define CC_VIRAMA                		12U
#define CC_ZERO_WIDTH_MARK       		13U
// Added by Nokia -- special case
#define CC_INDEPENDENT_VOWEL_A			14U
// Added by Nokia -- special case with independent vowel A
#define CC_DEPENDENT_VOWEL_CANDRA_E		15U
// Added by Nokia -- special case for Kannada Ra
#define CC_CONSONANT_KANNADA_BENGALI_RA	16U
// Added by Nokia -- special case for Tamil independent vowel O
#define CC_INDEPENDENT_VOWEL_TAMIL_O	17U

// Added by Nokia -- special case for Gurmukhi Bearer ARA and independent A 
#define CC_GUR_BEARER_A					18U
// Added by Nokia -- special case for Gurmukhi Bearer IRI 
#define CC_GUR_BEARER_I					19U
// Added by Nokia -- special case for Gurmukhi Bearer URA
#define CC_GUR_BEARER_U					20U

// Added by Nokia -- special case for Gurmukhi dependent Vowel which can combine with Bearer ARA
#define CC_GUR_DEPENDENT_VOWEL_A		21U
// Added by Nokia -- special case for Gurmukhi dependent Vowel which can combine with Bearer IRI
#define CC_GUR_DEPENDENT_VOWEL_I		22U
// Added by Nokia -- special case for Gurmukhi dependent Vowel which can combine with Bearer URA
#define CC_GUR_DEPENDENT_VOWEL_U		23U

#define CC_COUNT                 		24U

// Character class flags
#define CF_CLASS_MASK    0x0000FFFFU

#define CF_CONSONANT     0x80000000U

#define CF_REPH          0x40000000U
#define CF_VATTU         0x20000000U
#define CF_BELOW_BASE    0x10000000U
#define CF_POST_BASE     0x08000000U
#define CF_LENGTH_MARK   0x04000000U
// Added by Nokia -- special case for Bengali Ya
#define CF_CONSONANT_BENGALI_YA			0x00800000U

// Added by Nokia -- 1922 Malayalam Chillu -->
#define CF_CONSONANT_MLYL_CHILLU      0x00400000U
#define CC_DEPENDENT_VOWEL_SIGN_MLYL_E 0xD46U
#define CC_CONSONANT_MLYL_YA 0xD2FU
#define CC_CONSONANT_MLYL_VA 0xD35U
#define CC_CONSONANT_MLYL_RA 0xD30U
#define CC_CONSONANT_MLYL_RRA 0xD31U
#define CC_CONSONANT_MLYL_LLA 0xD33U
#define CC_CONSONANT_MLYL_LLLA 0xD34U
// <-- 1922 mlyl

#define CF_POS_BEFORE    0x00300000U
#define CF_POS_BELOW     0x00200000U
#define CF_POS_ABOVE     0x00100000U
#define CF_POS_AFTER     0x00000000U
#define CF_POS_MASK      0x00300000U

#define CF_INDEX_MASK    0x000F0000U
#define CF_INDEX_SHIFT   16

// Script flag bits
#define SF_MATRAS_AFTER_BASE     0x80000000U
#define SF_REPH_AFTER_BELOW      0x40000000U
#define SF_EYELASH_RA            0x20000000U
#define SF_MPRE_FIXUP            0x10000000U
//Added by Nokia for matra combined with final form of YA in Gurmukhi
//Flags SF_MATRAS_AFTER_BASE and SF_MATRAS_AFTER_POSTBASE cannot coexist.
//with SF_MATRAS_AFTER_BASE setting: Mbelow, Mabove and Mpost are attached to base consonant.
//With SF_MATRAS_AFTER_AFTERBASE setting: Mbelow, Mabove and Mpost are attached to postbase consonant
//without SF_MATRAS_AFTER_BASE or SF_MATRAS_AFTER_AFTERBASE setting: Mbelow, Mabove are attached to
//base (or base+below-base consonant cluster), but Mpost attached to postbase
#define SF_MATRAS_AFTER_POSTBASE		 0x08000000U

#define SF_POST_BASE_LIMIT_MASK  0x0000FFFFU
#define SF_NO_POST_BASE_LIMIT    0x00007FFFU

typedef LEUnicode SplitMatra[3];

class MPreFixups;
class LEGlyphStorage;

struct IndicClassTable
{
    typedef le_uint32 CharClass;
    typedef le_uint32 ScriptFlags;

    LEUnicode firstChar;
    LEUnicode lastChar;
    le_int32 worstCaseExpansion;
    ScriptFlags scriptFlags;
    const CharClass *classTable;
    const SplitMatra *splitMatraTable;

    inline le_int32 getWorstCaseExpansion() const;

    CharClass getCharClass(LEUnicode ch) const;

    inline const SplitMatra *getSplitMatra(CharClass charClass) const;

    inline le_bool isVowelModifier(LEUnicode ch) const;
    inline le_bool isStressMark(LEUnicode ch) const;
    inline le_bool isConsonant(LEUnicode ch) const;
    inline le_bool isReph(LEUnicode ch) const;
    inline le_bool isVirama(LEUnicode ch) const;
    inline le_bool isNukta(LEUnicode ch) const;
    inline le_bool isVattu(LEUnicode ch) const;
    inline le_bool isMatra(LEUnicode ch) const;
    inline le_bool isSplitMatra(LEUnicode ch) const;
    inline le_bool isLengthMark(LEUnicode ch) const;
    inline le_bool hasPostOrBelowBaseForm(LEUnicode ch) const;
    inline le_bool hasPostBaseForm(LEUnicode ch) const;
    inline le_bool hasBelowBaseForm(LEUnicode ch) const;
    // Added by Nokia: For special case Bengali Ya
    inline le_bool isBengaliYa(LEUnicode ch) const;
    // Added by Nokia -- 1922 mlyl -->
    inline le_bool isMlylChillu(LEUnicode ch) const;
    // <-- 1922 mlyl
    
    inline static le_bool isVowelModifier(CharClass charClass);
    inline static le_bool isStressMark(CharClass charClass);
    inline static le_bool isConsonant(CharClass charClass);
    inline static le_bool isReph(CharClass charClass);
    inline static le_bool isVirama(CharClass charClass);
    inline static le_bool isNukta(CharClass charClass);
    inline static le_bool isVattu(CharClass charClass);
    inline static le_bool isMatra(CharClass charClass);
    inline static le_bool isSplitMatra(CharClass charClass);
    inline static le_bool isLengthMark(CharClass charClass);
    inline static le_bool hasPostOrBelowBaseForm(CharClass charClass);
    inline static le_bool hasPostBaseForm(CharClass charClass);
    inline static le_bool hasBelowBaseForm(CharClass charClass);
    // Added by Nokia: For special case Bengali Ya
    inline static le_bool isBengaliYa(CharClass charClass);
    // Added by Nokia -- 1922 mlyl -->
    inline static le_bool isMlylChillu(CharClass charClass);
    // <-- 1922 mlyl
    
    static const IndicClassTable *getScriptClassTable(le_int32 scriptCode);
};

class IndicReordering /* not : public UObject because all methods are static */ {
public:
    static le_int32 getWorstCaseExpansion(le_int32 scriptCode);

    static le_int32 reorder(const LEUnicode *theChars, le_int32 charCount, le_int32 scriptCode,
        LEUnicode *outChars, LEGlyphStorage &glyphStorage,
        MPreFixups **outMPreFixups, LEErrorCode& success);

    static void adjustMPres(MPreFixups *mpreFixups, LEGlyphStorage &glyphStorage,
        LEErrorCode& success);

    static const LETag *getFeatureOrder();

private:
    // do not instantiate
    IndicReordering();

    static le_int32 findSyllable(const IndicClassTable *classTable, const LEUnicode *chars, le_int32 prev, le_int32 charCount);

};

inline le_int32 IndicClassTable::getWorstCaseExpansion() const
{
    return worstCaseExpansion;
}

inline const SplitMatra *IndicClassTable::getSplitMatra(CharClass charClass) const
{
    le_int32 index = (charClass & CF_INDEX_MASK) >> CF_INDEX_SHIFT;

    return &splitMatraTable[index - 1];
}

inline le_bool IndicClassTable::isVowelModifier(CharClass charClass)
{
    return (charClass & CF_CLASS_MASK) == CC_VOWEL_MODIFIER;
}

inline le_bool IndicClassTable::isStressMark(CharClass charClass)
{
    return (charClass & CF_CLASS_MASK) == CC_STRESS_MARK;
}

inline le_bool IndicClassTable::isConsonant(CharClass charClass)
{
    return (charClass & CF_CONSONANT) != 0;
}

inline le_bool IndicClassTable::isReph(CharClass charClass)
{
    return (charClass & CF_REPH) != 0;
}

inline le_bool IndicClassTable::isNukta(CharClass charClass)
{
    return (charClass & CF_CLASS_MASK) == CC_NUKTA;
}

inline le_bool IndicClassTable::isVirama(CharClass charClass)
{
    return (charClass & CF_CLASS_MASK) == CC_VIRAMA;
}

inline le_bool IndicClassTable::isVattu(CharClass charClass)
{
    return (charClass & CF_VATTU) != 0;
}

inline le_bool IndicClassTable::isMatra(CharClass charClass)
{
    charClass &= CF_CLASS_MASK;
	
	// Added special CANDRA E char class check to enable formation of Devanagari CANDRA A
    // Added speical CC_GUR_DEPENDENT_VOWEL_* to support decompositions of the independent
    // vowels into a sequence of a vowel bearer and a depending vowel sign
    return charClass >= CC_DEPENDENT_VOWEL && charClass <= CC_SPLIT_VOWEL_PIECE_3
    	   || charClass == CC_DEPENDENT_VOWEL_CANDRA_E
    	   || charClass == CC_GUR_DEPENDENT_VOWEL_A
    	   || charClass == CC_GUR_DEPENDENT_VOWEL_I
    	   || charClass == CC_GUR_DEPENDENT_VOWEL_U;
}

inline le_bool IndicClassTable::isSplitMatra(CharClass charClass)
{
    return (charClass & CF_INDEX_MASK) != 0;
}

inline le_bool IndicClassTable::isLengthMark(CharClass charClass)
{
    return (charClass & CF_LENGTH_MARK) != 0;
}

inline le_bool IndicClassTable::hasPostOrBelowBaseForm(CharClass charClass)
{
    return (charClass & (CF_POST_BASE | CF_BELOW_BASE)) != 0;
}

inline le_bool IndicClassTable::hasPostBaseForm(CharClass charClass)
{
    return (charClass & CF_POST_BASE) != 0;
}

inline le_bool IndicClassTable::hasBelowBaseForm(CharClass charClass)
{
    return (charClass & CF_BELOW_BASE) != 0;
}

// Added by Nokia -- For determining whether a character is a Bengali Ya
inline le_bool IndicClassTable::isBengaliYa(CharClass charClass)
	{
	    return (charClass & CF_CONSONANT_BENGALI_YA) != 0;
	}

// Added by Nokia -- 1922 mlyl -->
inline le_bool IndicClassTable::isMlylChillu(CharClass charClass)
{
    return (charClass & CF_CONSONANT_MLYL_CHILLU) != 0;
}
// <-- 1922 mlyl

inline le_bool IndicClassTable::isVowelModifier(LEUnicode ch) const
{
    return isVowelModifier(getCharClass(ch));
}

inline le_bool IndicClassTable::isStressMark(LEUnicode ch) const
{
    return isStressMark(getCharClass(ch));
}

inline le_bool IndicClassTable::isConsonant(LEUnicode ch) const
{
    return isConsonant(getCharClass(ch));
}

inline le_bool IndicClassTable::isReph(LEUnicode ch) const
{
    return isReph(getCharClass(ch));
}

inline le_bool IndicClassTable::isVirama(LEUnicode ch) const
{
    return isVirama(getCharClass(ch));
}

inline le_bool IndicClassTable::isNukta(LEUnicode ch) const
{
    return isNukta(getCharClass(ch));
}

inline le_bool IndicClassTable::isVattu(LEUnicode ch) const
{
    return isVattu(getCharClass(ch));
}

inline le_bool IndicClassTable::isMatra(LEUnicode ch) const
{
    return isMatra(getCharClass(ch));
}

inline le_bool IndicClassTable::isSplitMatra(LEUnicode ch) const
{
    return isSplitMatra(getCharClass(ch));
}

inline le_bool IndicClassTable::isLengthMark(LEUnicode ch) const
{
    return isLengthMark(getCharClass(ch));
}

inline le_bool IndicClassTable::hasPostOrBelowBaseForm(LEUnicode ch) const
{
    return hasPostOrBelowBaseForm(getCharClass(ch));
}

inline le_bool IndicClassTable::hasPostBaseForm(LEUnicode ch) const
{
    return hasPostBaseForm(getCharClass(ch));
}

inline le_bool IndicClassTable::hasBelowBaseForm(LEUnicode ch) const
{
    return hasBelowBaseForm(getCharClass(ch));
}

// Added by Nokia -- For determining whether a character is a Bengali Ya
inline le_bool IndicClassTable::isBengaliYa(LEUnicode ch) const
{
    return isBengaliYa(getCharClass(ch));
}

// Added by Nokia -- 1922 mlyl -->
inline le_bool IndicClassTable::isMlylChillu(LEUnicode ch) const
{
    return isMlylChillu(getCharClass(ch));
}
// <-- 1922 mlyl

U_NAMESPACE_END
#endif
