/*
* Copyright (c) 2003-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
* TGraphemeIterator.cpp unit tests for RTmGraphemeEdgeIterator
*
*/


#include "TestLayout.h"
#include "TGraphicsContext.h"
#include "TMINTERP.H"

#include "tgraphemeiterator.h"

namespace LocalToTGraphemeIterator
{
CTGraphemeIteratorStep* TestStep;
#define TESTPOINT(p) TestStep->testpoint(p,(TText8*)__FILE__,__LINE__)
#define TESTPRINT(p) TestStep->print(p,(TText8*)__FILE__,__LINE__)

struct TTransliteration
    {
    const TText* iString;
    TInt iChar;
    };
static const TTransliteration KArabicTransliteration[] =
    {
    { reinterpret_cast<const TText*>(L"?"), 0x61F },
    { reinterpret_cast<const TText*>(L"`"), 0x621 },    // in-line hamza
    { reinterpret_cast<const TText*>(L"a"), 0x627 },    // alif
    { reinterpret_cast<const TText*>(L"b"), 0x628 },
    { reinterpret_cast<const TText*>(L"A"), 0x629 },    // teh marbuta
    { reinterpret_cast<const TText*>(L"t"), 0x62A },
    { reinterpret_cast<const TText*>(L"th"), 0x62B },
    { reinterpret_cast<const TText*>(L"j"), 0x62C },
    { reinterpret_cast<const TText*>(L"H"), 0x62D },    // hah
    { reinterpret_cast<const TText*>(L"kh"), 0x62E },
    { reinterpret_cast<const TText*>(L"d"), 0x62F },
    { reinterpret_cast<const TText*>(L"dh"), 0x630 },
    { reinterpret_cast<const TText*>(L"r"), 0x631 },
    { reinterpret_cast<const TText*>(L"z"), 0x632 },
    { reinterpret_cast<const TText*>(L"s"), 0x633 },
    { reinterpret_cast<const TText*>(L"sh"), 0x634 },
    { reinterpret_cast<const TText*>(L"S"), 0x635 },
    { reinterpret_cast<const TText*>(L"D"), 0x636 },
    { reinterpret_cast<const TText*>(L"T"), 0x637 },
    { reinterpret_cast<const TText*>(L"Z"), 0x638 },    // zah
    { reinterpret_cast<const TText*>(L"'"), 0x639 },    // ain
    { reinterpret_cast<const TText*>(L"g"), 0x63A },
    { reinterpret_cast<const TText*>(L"_"), 0x640 },    // kashida
    { reinterpret_cast<const TText*>(L"f"), 0x641 },
    { reinterpret_cast<const TText*>(L"q"), 0x642 },
    { reinterpret_cast<const TText*>(L"k"), 0x643 },
    { reinterpret_cast<const TText*>(L"l"), 0x644 },    // lam
    { reinterpret_cast<const TText*>(L"m"), 0x645 },
    { reinterpret_cast<const TText*>(L"n"), 0x646 },
    { reinterpret_cast<const TText*>(L"h"), 0x647 },    // heh
    { reinterpret_cast<const TText*>(L"w"), 0x648 },
    { reinterpret_cast<const TText*>(L"y"), 0x64A },
    { reinterpret_cast<const TText*>(L"^F"), 0x64B },   // fathatan
    { reinterpret_cast<const TText*>(L"^D"), 0x64C },   // dammatan
    { reinterpret_cast<const TText*>(L"^K"), 0x64D },   // kasratan
    { reinterpret_cast<const TText*>(L"^f"), 0x64E },   // fatha
    { reinterpret_cast<const TText*>(L"^d"), 0x64F },   // damma
    { reinterpret_cast<const TText*>(L"^k"), 0x650 },   // kasra
    { reinterpret_cast<const TText*>(L"^s"), 0x651 },   // shadda
    { reinterpret_cast<const TText*>(L"^h"), 0x652 },   // sukun
    { reinterpret_cast<const TText*>(L"^~"), 0x653 },   // maddah
    { reinterpret_cast<const TText*>(L"^`"), 0x654 },   // hamza above
    { reinterpret_cast<const TText*>(L"_`"), 0x653 },   // hamza below
    { reinterpret_cast<const TText*>(L"0"), 0x660 },
    { reinterpret_cast<const TText*>(L"1"), 0x661 },
    { reinterpret_cast<const TText*>(L"2"), 0x662 },
    { reinterpret_cast<const TText*>(L"3"), 0x663 },
    { reinterpret_cast<const TText*>(L"4"), 0x664 },
    { reinterpret_cast<const TText*>(L"5"), 0x665 },
    { reinterpret_cast<const TText*>(L"6"), 0x666 },
    { reinterpret_cast<const TText*>(L"7"), 0x667 },
    { reinterpret_cast<const TText*>(L"8"), 0x668 },
    { reinterpret_cast<const TText*>(L"9"), 0x669 }
    };
}
using namespace LocalToTGraphemeIterator;

TText TransliterateSingle(const TText*& aInput, const TText* aEnd)
	{
	const TInt tableSize =
		sizeof(KArabicTransliteration)/sizeof(KArabicTransliteration[0]);
	for (TInt i = 0; i != tableSize; ++i)
		{
		const TText* p = KArabicTransliteration[i].iString;
		const TText* q = aInput;
		while (q != aEnd && *q == *p)
			{
			++q;
			++p;
			if (*p == '\0')
				{
				aInput = q;
				return static_cast<TText>(KArabicTransliteration[i].iChar);
				}
			}
		}
	TText result = *aInput;
	++aInput;
	return result;
	}

// transliteration is turned on with { and off with }.
// use }{ to split digraphs.
void Transliterate(const TDesC& aIn, TDes& aOut)
	{
	const TText KTransliterationOn = '{';
	const TText KTransliterationOff = '}';
	TBool transliterating = EFalse;
	const TText* p = &aIn[0];
	const TText* pEnd = p + aIn.Length();
	while (p != pEnd)
		{
		if (!transliterating)
			{
			if (*p == KTransliterationOn)
				{
				transliterating = ETrue;
				++p;
				}
			else
				aOut.Append(*p++);
			}
		else
			{
			if (*p == KTransliterationOff)
				{
				transliterating = EFalse;
				++p;
				}
			else
				aOut.Append(TransliterateSingle(p, pEnd));
			}
		}
	}

/**
Tests RTmGraphemeEdgeIterator::DocPosMatches for this document position and
edge.
*/
void TestDocPosMatchesCase(const TTmGraphemeEdgeInfo& aEdgeInfo,
	TTmDocPosSpec& aPosSpec,
	RTmGraphemeEdgeIterator::TGraphemeMatch aExpectedMatchType)
	{
	TInt start = aEdgeInfo.iPos.iDocPos.iPos;
	TInt end = start;
	if (aEdgeInfo.iPos.iDocPos.iLeadingEdge)
		end += aEdgeInfo.iCodePoints;
	else
		{
		start -= aEdgeInfo.iCodePoints - 1;
		++end;
		}
	aPosSpec.iPos = start - 1;
	TESTPOINT(RTmGraphemeEdgeIterator::DocPosMatches(aPosSpec, aEdgeInfo)
		== RTmGraphemeEdgeIterator::ENoMatch);
	for (TInt i = start; i != end; ++i)
		{
		aPosSpec.iPos = i;
		TESTPOINT(RTmGraphemeEdgeIterator::DocPosMatches(aPosSpec, aEdgeInfo)
			== aExpectedMatchType);
		}
	aPosSpec.iPos = end;
	TESTPOINT(RTmGraphemeEdgeIterator::DocPosMatches(aPosSpec, aEdgeInfo)
		== RTmGraphemeEdgeIterator::ENoMatch);
	}

/**
Tests RTmGraphemeEdgeIterator::DocPosMatches for this edge and all relevant
document position specifications.
*/
void TestDocPosMatchesAllSpecs(const TTmGraphemeEdgeInfo& aEdgeInfo)
	{
	TTmDocPosSpec posSpec;
	RTmGraphemeEdgeIterator::TGraphemeMatch expected;
	posSpec.iType = TTmDocPosSpec::ELeftToRight;
	expected = aEdgeInfo.iPos.iRightToLeft?
		RTmGraphemeEdgeIterator::EPositionOnly
		: RTmGraphemeEdgeIterator::ETotalMatch;
	TestDocPosMatchesCase(aEdgeInfo, posSpec, expected);
	posSpec.iType = TTmDocPosSpec::ERightToLeft;
	expected = aEdgeInfo.iPos.iRightToLeft?
		RTmGraphemeEdgeIterator::ETotalMatch
		: RTmGraphemeEdgeIterator::EPositionOnly;
	TestDocPosMatchesCase(aEdgeInfo, posSpec, expected);
	posSpec.iType = TTmDocPosSpec::ETrailing;
	expected = aEdgeInfo.iPos.iDocPos.iLeadingEdge?
		RTmGraphemeEdgeIterator::ENoMatch
		: RTmGraphemeEdgeIterator::ETotalMatch;
	TestDocPosMatchesCase(aEdgeInfo, posSpec, expected);
	posSpec.iType = TTmDocPosSpec::ELeading;
	expected = aEdgeInfo.iPos.iDocPos.iLeadingEdge?
		RTmGraphemeEdgeIterator::ETotalMatch
		: RTmGraphemeEdgeIterator::ENoMatch;
	TestDocPosMatchesCase(aEdgeInfo, posSpec, expected);
	}

/**
Tests RTmGraphemeEdgeIterator::DocPosMatches for a variety of edges and
positions.
*/
void TestDocPosMatches()
	{
	TTmGraphemeEdgeInfo edgeInfo;
	edgeInfo.iPos.iDocPos.iPos = 5;
	for (edgeInfo.iCodePoints = 1; edgeInfo.iCodePoints <= 3;
		++edgeInfo.iCodePoints)
		{
		edgeInfo.iPos.iDocPos.iLeadingEdge = ETrue;
		edgeInfo.iPos.iRightToLeft = EFalse;
		TestDocPosMatchesAllSpecs(edgeInfo);
		edgeInfo.iPos.iDocPos.iLeadingEdge = EFalse;
		TestDocPosMatchesAllSpecs(edgeInfo);
		edgeInfo.iPos.iRightToLeft = ETrue;
		TestDocPosMatchesAllSpecs(edgeInfo);
		edgeInfo.iPos.iDocPos.iLeadingEdge = ETrue;
		TestDocPosMatchesAllSpecs(edgeInfo);
		}
	}

enum TEdgeType { ETrail, ELead };
enum TEdgeRelationship { EEdgeDifferent, EEdgeSame, EEdgeNewline };
enum TAmbiguity { EUnamb = 0, EAmb = 1 };
enum TDirectionality { EL2R = 0, ER2L = 1 };
struct TEdge
	{
	TInt iPos;
	TEdgeType iLeading;
	TEdgeRelationship iNext;
	TAmbiguity iAmbiguity;
	TDirectionality iRightToLeft;
	};

_LIT(KLatin1, "Latin text\x2029Latin text over three          lines.");
static const TEdge KLatin1Edges[] =
	{
	{0, ETrail, EEdgeSame, EUnamb, EL2R}, {0, ELead, EEdgeDifferent, EUnamb, EL2R},
	{1, ETrail, EEdgeSame, EUnamb, EL2R}, {1, ELead, EEdgeDifferent, EUnamb, EL2R},
	{2, ETrail, EEdgeSame, EUnamb, EL2R}, {2, ELead, EEdgeDifferent, EUnamb, EL2R},
	{3, ETrail, EEdgeSame, EUnamb, EL2R}, {3, ELead, EEdgeDifferent, EUnamb, EL2R},
	{4, ETrail, EEdgeSame, EUnamb, EL2R}, {4, ELead, EEdgeDifferent, EUnamb, EL2R},
	{5, ETrail, EEdgeSame, EUnamb, EL2R}, {5, ELead, EEdgeDifferent, EUnamb, EL2R},
	{6, ETrail, EEdgeSame, EUnamb, EL2R}, {6, ELead, EEdgeDifferent, EUnamb, EL2R},
	{7, ETrail, EEdgeSame, EUnamb, EL2R}, {7, ELead, EEdgeDifferent, EUnamb, EL2R},
	{8, ETrail, EEdgeSame, EUnamb, EL2R}, {8, ELead, EEdgeDifferent, EUnamb, EL2R},
	{9, ETrail, EEdgeSame, EUnamb, EL2R}, {9, ELead, EEdgeDifferent, EUnamb, EL2R},
	{10, ETrail, EEdgeSame, EUnamb, EL2R}, {10, ELead, EEdgeNewline, EUnamb, EL2R},
	{11, ETrail, EEdgeSame, EUnamb, EL2R}, {11, ELead, EEdgeDifferent, EUnamb, EL2R},
	{12, ETrail, EEdgeSame, EUnamb, EL2R}, {12, ELead, EEdgeDifferent, EUnamb, EL2R},
	{13, ETrail, EEdgeSame, EUnamb, EL2R}, {13, ELead, EEdgeDifferent, EUnamb, EL2R},
	{14, ETrail, EEdgeSame, EUnamb, EL2R}, {14, ELead, EEdgeDifferent, EUnamb, EL2R},
	{15, ETrail, EEdgeSame, EUnamb, EL2R}, {15, ELead, EEdgeDifferent, EUnamb, EL2R},
	{16, ETrail, EEdgeSame, EUnamb, EL2R}, {16, ELead, EEdgeDifferent, EUnamb, EL2R},
	{17, ETrail, EEdgeSame, EUnamb, EL2R}, {17, ELead, EEdgeDifferent, EUnamb, EL2R},
	{18, ETrail, EEdgeSame, EUnamb, EL2R}, {18, ELead, EEdgeDifferent, EUnamb, EL2R},
	{19, ETrail, EEdgeSame, EUnamb, EL2R}, {19, ELead, EEdgeDifferent, EUnamb, EL2R},
	{20, ETrail, EEdgeSame, EUnamb, EL2R}, {20, ELead, EEdgeDifferent, EUnamb, EL2R},
// This change tests the change made for DEF059214 which makes
// the trailing edges of line breaks over unambiguous text move to
// the start of the next line rather than hanging onto the end of
// the breaking line.
//	{21, ETrail, EEdgeSame, EUnamb, EL2R}, {21, ELead, EEdgeDifferent, EUnamb, EL2R},
//	{22, ETrail, EEdgeNewline, EUnamb, EL2R}, {22, ELead, EEdgeDifferent, EUnamb, EL2R},
	{21, ETrail, EEdgeSame, EUnamb, EL2R}, {21, ELead, EEdgeNewline, EUnamb, EL2R},
	{22, ETrail, EEdgeSame, EUnamb, EL2R}, {22, ELead, EEdgeDifferent, EUnamb, EL2R},
	{23, ETrail, EEdgeSame, EUnamb, EL2R}, {23, ELead, EEdgeDifferent, EUnamb, EL2R},
	{24, ETrail, EEdgeSame, EUnamb, EL2R}, {24, ELead, EEdgeDifferent, EUnamb, EL2R},
	{25, ETrail, EEdgeSame, EUnamb, EL2R}, {25, ELead, EEdgeDifferent, EUnamb, EL2R},
	{26, ETrail, EEdgeSame, EUnamb, EL2R}, {26, ELead, EEdgeDifferent, EUnamb, EL2R},
	{27, ETrail, EEdgeSame, EUnamb, EL2R}, {27, ELead, EEdgeDifferent, EUnamb, EL2R},
	{28, ETrail, EEdgeSame, EUnamb, EL2R}, {28, ELead, EEdgeDifferent, EUnamb, EL2R},
	{29, ETrail, EEdgeSame, EUnamb, EL2R}, {29, ELead, EEdgeDifferent, EUnamb, EL2R},
	{30, ETrail, EEdgeSame, EUnamb, EL2R}, {30, ELead, EEdgeDifferent, EUnamb, EL2R},
	{31, ETrail, EEdgeSame, EUnamb, EL2R}, {31, ELead, EEdgeDifferent, EUnamb, EL2R},
	{32, ETrail, EEdgeSame, EUnamb, EL2R}, {32, ELead, EEdgeDifferent, EUnamb, EL2R},
	{33, ETrail, EEdgeSame, EUnamb, EL2R}, {33, ELead, EEdgeDifferent, EUnamb, EL2R},
	{34, ETrail, EEdgeSame, EUnamb, EL2R}, {34, ELead, EEdgeDifferent, EUnamb, EL2R},
	{35, ETrail, EEdgeSame, EUnamb, EL2R}, {35, ELead, EEdgeDifferent, EUnamb, EL2R},
	{36, ETrail, EEdgeSame, EUnamb, EL2R}, {36, ELead, EEdgeDifferent, EUnamb, EL2R},
	{37, ETrail, EEdgeSame, EUnamb, EL2R}, {37, ELead, EEdgeDifferent, EUnamb, EL2R},
	{38, ETrail, EEdgeSame, EUnamb, EL2R}, {38, ELead, EEdgeDifferent, EUnamb, EL2R},
	{39, ETrail, EEdgeSame, EUnamb, EL2R}, {39, ELead, EEdgeDifferent, EUnamb, EL2R},
	{40, ETrail, EEdgeSame, EUnamb, EL2R}, {40, ELead, EEdgeDifferent, EUnamb, EL2R},
// This change tests the change made for DEF059214
//	{41, ETrail, EEdgeSame, EUnamb, EL2R}, {41, ELead, EEdgeDifferent, EUnamb, EL2R},
//	{42, ETrail, EEdgeNewline, EUnamb, EL2R}, {42, ELead, EEdgeDifferent, EUnamb, EL2R},
// similar changes have been made to other tests.
	{41, ETrail, EEdgeSame, EUnamb, EL2R}, {41, ELead, EEdgeNewline, EUnamb, EL2R},
	{42, ETrail, EEdgeSame, EUnamb, EL2R}, {42, ELead, EEdgeDifferent, EUnamb, EL2R},
	{43, ETrail, EEdgeSame, EUnamb, EL2R}, {43, ELead, EEdgeDifferent, EUnamb, EL2R},
	{44, ETrail, EEdgeSame, EUnamb, EL2R}, {44, ELead, EEdgeDifferent, EUnamb, EL2R},
	{45, ETrail, EEdgeSame, EUnamb, EL2R}, {45, ELead, EEdgeDifferent, EUnamb, EL2R},
	{46, ETrail, EEdgeSame, EUnamb, EL2R}, {46, ELead, EEdgeDifferent, EUnamb, EL2R},
	{47, ETrail, EEdgeSame, EUnamb, EL2R}, {47, ELead, EEdgeDifferent, EUnamb, EL2R},
	{48, ETrail, EEdgeSame, EUnamb, EL2R}, {48, ELead, EEdgeNewline, EUnamb, EL2R},
	};

_LIT(KArabic1, "{al'rbyA}\x2029{al'rbyA kf Sayd almwstfa}\x2029{lala lala}.");
static const TEdge KArabic1Edges[] =
	{
	{7, ELead, EEdgeSame, EUnamb, ER2L}, {7, ETrail, EEdgeDifferent, EUnamb, ER2L},
	{6, ELead, EEdgeSame, EUnamb, ER2L}, {6, ETrail, EEdgeDifferent, EUnamb, ER2L},
	{5, ELead, EEdgeSame, EUnamb, ER2L}, {5, ETrail, EEdgeDifferent, EUnamb, ER2L},
	{4, ELead, EEdgeSame, EUnamb, ER2L}, {4, ETrail, EEdgeDifferent, EUnamb, ER2L},
	{3, ELead, EEdgeSame, EUnamb, ER2L}, {3, ETrail, EEdgeDifferent, EUnamb, ER2L},
	{2, ELead, EEdgeSame, EUnamb, ER2L}, {2, ETrail, EEdgeDifferent, EUnamb, ER2L},
	{1, ELead, EEdgeSame, EUnamb, ER2L}, {1, ETrail, EEdgeDifferent, EUnamb, ER2L},
	{0, ELead, EEdgeSame, EUnamb, ER2L}, {0, ETrail, EEdgeNewline, EUnamb, ER2L},
	{18, ELead, EEdgeSame, EUnamb, ER2L}, {18, ETrail, EEdgeDifferent, EUnamb, ER2L},
	{17, ELead, EEdgeSame, EUnamb, ER2L}, {17, ETrail, EEdgeDifferent, EUnamb, ER2L},
	{16, ELead, EEdgeSame, EUnamb, ER2L}, {16, ETrail, EEdgeDifferent, EUnamb, ER2L},
	{15, ELead, EEdgeSame, EUnamb, ER2L}, {15, ETrail, EEdgeDifferent, EUnamb, ER2L},
	{14, ELead, EEdgeSame, EUnamb, ER2L}, {14, ETrail, EEdgeDifferent, EUnamb, ER2L},
	{13, ELead, EEdgeSame, EUnamb, ER2L}, {13, ETrail, EEdgeDifferent, EUnamb, ER2L},
	{12, ELead, EEdgeSame, EUnamb, ER2L}, {12, ETrail, EEdgeDifferent, EUnamb, ER2L},
	{11, ELead, EEdgeSame, EUnamb, ER2L}, {11, ETrail, EEdgeDifferent, EUnamb, ER2L},
	{10, ELead, EEdgeSame, EUnamb, ER2L}, {10, ETrail, EEdgeDifferent, EUnamb, ER2L},
	{9, ELead, EEdgeSame, EUnamb, ER2L}, {9, ETrail, EEdgeDifferent, EUnamb, ER2L},
	{8, ELead, EEdgeSame, EUnamb, ER2L}, {8, ETrail, EEdgeNewline, EUnamb, ER2L},
	{23, ELead, EEdgeSame, EUnamb, ER2L}, {23, ETrail, EEdgeDifferent, EUnamb, ER2L},
	{22, ELead, EEdgeSame, EUnamb, ER2L}, {22, ETrail, EEdgeDifferent, EUnamb, ER2L},
	{21, ELead, EEdgeSame, EUnamb, ER2L}, {21, ETrail, EEdgeDifferent, EUnamb, ER2L},
	{20, ELead, EEdgeSame, EUnamb, ER2L}, {20, ETrail, EEdgeDifferent, EUnamb, ER2L},
	{19, ELead, EEdgeSame, EUnamb, ER2L}, {19, ETrail, EEdgeNewline, EUnamb, ER2L},
	{32, ELead, EEdgeSame, EUnamb, ER2L}, {32, ETrail, EEdgeDifferent, EUnamb, ER2L},
	{31, ELead, EEdgeSame, EUnamb, ER2L}, {31, ETrail, EEdgeDifferent, EUnamb, ER2L},
	{30, ELead, EEdgeSame, EUnamb, ER2L}, {30, ETrail, EEdgeDifferent, EUnamb, ER2L},
	{29, ELead, EEdgeSame, EUnamb, ER2L}, {29, ETrail, EEdgeDifferent, EUnamb, ER2L},
	{28, ELead, EEdgeSame, EUnamb, ER2L}, {28, ETrail, EEdgeDifferent, EUnamb, ER2L},
	{27, ELead, EEdgeSame, EUnamb, ER2L}, {27, ETrail, EEdgeDifferent, EUnamb, ER2L},
	{26, ELead, EEdgeSame, EUnamb, ER2L}, {26, ETrail, EEdgeDifferent, EUnamb, ER2L},
	{25, ELead, EEdgeSame, EUnamb, ER2L}, {25, ETrail, EEdgeDifferent, EUnamb, ER2L},
	{24, ELead, EEdgeSame, EUnamb, ER2L}, {24, ETrail, EEdgeNewline, EUnamb, ER2L},
	{43, ELead, EEdgeSame, EUnamb, ER2L}, {43, ETrail, EEdgeDifferent, EUnamb, ER2L},
	{42, ELead, EEdgeSame, EUnamb, ER2L}, {42, ETrail, EEdgeDifferent, EUnamb, ER2L},
	{40, ELead, EEdgeSame, EUnamb, ER2L}, {40, ETrail, EEdgeDifferent, EUnamb, ER2L},
	{38, ELead, EEdgeSame, EUnamb, ER2L}, {38, ETrail, EEdgeDifferent, EUnamb, ER2L},
	{37, ELead, EEdgeSame, EUnamb, ER2L}, {37, ETrail, EEdgeDifferent, EUnamb, ER2L},
	{35, ELead, EEdgeSame, EUnamb, ER2L}, {35, ETrail, EEdgeDifferent, EUnamb, ER2L},
	{33, ELead, EEdgeSame, EUnamb, ER2L}, {33, ETrail, EEdgeNewline, EUnamb, ER2L}
	};

// Add another example including combining marks and zero-width characters
_LIT(KCombiners1, "z\x300\x301\x302y\x300\x301\x302\x2029z\x300\x301\x302(\xFEFF)");
static const TEdge KCombiners1Edges[] =
	{
	{0, ETrail, EEdgeSame, EUnamb, EL2R}, {0, ELead, EEdgeDifferent, EUnamb, EL2R},
	{4, ETrail, EEdgeSame, EUnamb, EL2R}, {4, ELead, EEdgeDifferent, EUnamb, EL2R},
	{8, ETrail, EEdgeSame, EUnamb, EL2R}, {8, ELead, EEdgeNewline, EUnamb, EL2R},
	{9, ETrail, EEdgeSame, EUnamb, EL2R}, {9, ELead, EEdgeNewline, EUnamb, EL2R},
	{13, ETrail, EEdgeSame, EUnamb, EL2R}, {13, ELead, EEdgeDifferent, EUnamb, EL2R},
	{14, ETrail, EEdgeSame, EUnamb, EL2R}, {14, ELead, EEdgeSame, EUnamb, EL2R},
	{15, ETrail, EEdgeSame, EUnamb, EL2R}, {15, ELead, EEdgeDifferent, EUnamb, EL2R},
	{16, ETrail, EEdgeSame, EUnamb, EL2R}, {16, ELead, EEdgeNewline, EUnamb, EL2R}
	};

// Add another example including bidirectional text
_LIT(KBidi1, "A\x301{b^ft^k}12\x200FZ\x301");
static const TEdge KBidi1Edges[] =
	{
	{0, ETrail, EEdgeSame, EUnamb, EL2R}, {0, ELead, EEdgeDifferent, EUnamb, EL2R},
	{2, ETrail, EEdgeSame, EUnamb, EL2R}, {9, ETrail, EEdgeSame, EAmb, ER2L},
	{8, ELead, EEdgeSame, EUnamb, ER2L}, {6, ELead, EEdgeDifferent, EAmb, EL2R},
	{7, ETrail, EEdgeSame, EUnamb, EL2R}, {7, ELead, EEdgeDifferent, EUnamb, EL2R},
	{8, ETrail, EEdgeSame, EUnamb, EL2R}, {6, ETrail, EEdgeDifferent, EAmb, ER2L},
	{4, ELead, EEdgeSame, EUnamb, ER2L}, {4, ETrail, EEdgeDifferent, EUnamb, ER2L},
	{2, ELead, EEdgeSame, EUnamb, ER2L}, {9, ELead, EEdgeDifferent, EAmb, EL2R},
	{11, ETrail, EEdgeSame, EUnamb, EL2R}, {11, ELead, EEdgeNewline, EUnamb, EL2R}
	};

// This example contains the "amtriguous" case where numbers
// are embedded within Arabic but are next to Latin.
_LIT(KBidi2, "A\x301{b^ft^k}12Z\x301");
static const TEdge KBidi2Edges[] =
	{
	{0, ETrail, EEdgeSame, EUnamb, EL2R}, {0, ELead, EEdgeDifferent, EUnamb, EL2R},
	{2, ETrail, EEdgeSame, EUnamb, EL2R}, {6, ELead, EEdgeDifferent, EAmb, EL2R},
	{7, ETrail, EEdgeSame, EUnamb, EL2R}, {7, ELead, EEdgeDifferent, EUnamb, EL2R},
	{8, ETrail, EEdgeSame, EUnamb, EL2R}, {6, ETrail, EEdgeDifferent, EAmb, ER2L},
	{4, ELead, EEdgeSame, EUnamb, ER2L}, {4, ETrail, EEdgeDifferent, EUnamb, ER2L},
	{2, ELead, EEdgeSame, EUnamb, ER2L}, {8, ELead, EEdgeDifferent, EAmb, EL2R},
	{10, ETrail, EEdgeSame, EUnamb, EL2R}, {10, ELead, EEdgeNewline, EUnamb, EL2R}
	};

_LIT(KParagraphs1, "z\x2029{b}\x2029z");
static const TEdge KParagraphs1Edges[] =
	{
	// First line:
	// 0T 0L (Z) 1T 1L (ParagraphDelimiter)
	{0, ETrail, EEdgeSame, EUnamb, EL2R}, {0, ELead, EEdgeDifferent, EUnamb, EL2R},
	{1, ETrail, EEdgeSame, EUnamb, EL2R}, {1, ELead, EEdgeNewline, EUnamb, EL2R},
	// Second line:
	// (PD) 3L< 3T< (Beh) 2L< 2T<
	{3, ELead, EEdgeSame, EUnamb, ER2L}, {3, ETrail, EEdgeDifferent, EUnamb, ER2L},
	{2, ELead, EEdgeSame, EUnamb, ER2L}, {2, ETrail, EEdgeNewline, EUnamb, ER2L},
	// Third line:
	// 4T 4L (Z) 5T 5L (NominalPD)
	{4, ETrail, EEdgeSame, EUnamb, EL2R}, {4, ELead, EEdgeDifferent, EUnamb, EL2R},
	{5, ETrail, EEdgeSame, EUnamb, EL2R}, {5, ELead, EEdgeNewline, EUnamb, EL2R}
	};

// 2 characters per line
_LIT(KEmbedded1, "z{bb}z\x2029{b}zz{b}\x2029z{b}zz{b}z\x2029{b}z{bb}z{b}");
// T=trailing, L=leading, !=Ambiguous, <=Right-to-left
static const TEdge KEmbedded1Edges[] =
	{
	// First line:
	// 0T 0L (0Z) 1T! 2T!< (1Beh) 1L<
	{0, ETrail, EEdgeSame, EUnamb, EL2R}, {0, ELead, EEdgeDifferent, EUnamb, EL2R},
	{1, ETrail, EEdgeSame, EAmb, EL2R}, {2, ETrail, EEdgeDifferent, EAmb, ER2L},
	{1, ELead, EEdgeNewline, EUnamb, ER2L},
	// Second line:
	// 3T< (2Beh) 2L!< 3L! (3Z) 4T 4L (4ParagraphDelimiter)
	{3, ETrail, EEdgeDifferent, EUnamb, ER2L},
	{2, ELead, EEdgeSame, EAmb, ER2L}, {3, ELead, EEdgeDifferent, EAmb, EL2R},
	{4, ETrail, EEdgeSame, EUnamb, EL2R}, {4, ELead, EEdgeNewline, EUnamb, EL2R},
	// Third line:
	// 6L (6Z) 7T! 6T!< (5Beh) 5L< 5T<
	{6, ELead, EEdgeDifferent, EUnamb, EL2R},
	{7, ETrail, EEdgeSame, EAmb, EL2R}, {6, ETrail, EEdgeDifferent, EAmb, ER2L},
	{5, ELead, EEdgeSame, EUnamb, ER2L}, {5, ETrail, EEdgeNewline, EUnamb, ER2L},
	// Fourth line:
	// (9PD) 9L< 9T< (8Beh) 8L!< 7L! (7Z) 8T
	{9, ELead, EEdgeSame, EUnamb, ER2L}, {9, ETrail, EEdgeDifferent, EUnamb, ER2L},
	{8, ELead, EEdgeSame, EAmb, ER2L}, {7, ELead, EEdgeDifferent, EAmb, EL2R},
	{8, ETrail, EEdgeNewline, EUnamb, EL2R},
	// Fifth line:
	// 10T 10L (10Z) 11T! 12T!< (11Beh) 11L<
	{10, ETrail, EEdgeSame, EUnamb, EL2R}, {10, ELead, EEdgeDifferent, EUnamb, EL2R},
	{11, ETrail, EEdgeSame, EAmb, EL2R}, {12, ETrail, EEdgeDifferent, EAmb, ER2L},
	{11, ELead, EEdgeNewline, EUnamb, ER2L},
	// Sixth line:
	// 12L (12Z) 13T 13L (13Z) 14T
	{12, ELead, EEdgeDifferent, EUnamb, EL2R},
	{13, ETrail, EEdgeSame, EUnamb, EL2R}, {13, ELead, EEdgeDifferent, EUnamb, EL2R},
	{14, ETrail, EEdgeNewline, EUnamb, EL2R},
	// Seventh line:
	// 15T< (14Beh) 14L!< 15L! (15Z) 16T 16L (16PD)
	{15, ETrail, EEdgeDifferent, EUnamb, ER2L},
	{14, ELead, EEdgeSame, EAmb, ER2L}, {15, ELead, EEdgeDifferent, EAmb, EL2R},
	{16, ETrail, EEdgeSame, EUnamb, EL2R}, {16, ELead, EEdgeNewline, EUnamb, EL2R},
	// Eighth line:
	// 18L (18Z) 19T! 18T!< (17Beh) 17L< 17T<
	{18, ELead, EEdgeDifferent, EUnamb, EL2R},
	{19, ETrail, EEdgeSame, EAmb, EL2R}, {18, ETrail, EEdgeDifferent, EAmb, ER2L},
	{17, ELead, EEdgeSame, EUnamb, ER2L}, {17, ETrail, EEdgeNewline, EUnamb, ER2L},
	// Ninth line:
	// 21T< (20Beh) 20L< 20T< (19Beh) 19L<
	{21, ETrail, EEdgeDifferent, EUnamb, ER2L},
	{20, ELead, EEdgeSame, EUnamb, ER2L}, {20, ETrail, EEdgeDifferent, EUnamb, ER2L},
	{19, ELead, EEdgeNewline, EUnamb, ER2L},
	// Tenth line:
	// (23NominalPD) 23L< 23T< (22Beh) 22L!< 21L! (21Z) 22T
	{23, ELead, EEdgeSame, EUnamb, ER2L}, {23, ETrail, EEdgeDifferent, EUnamb, ER2L},
	{22, ELead, EEdgeSame, EAmb, ER2L}, {21, ELead, EEdgeDifferent, EAmb, EL2R},
	{22, ETrail, EEdgeNewline, EUnamb, EL2R},
	};

/**
Returns which portion of the text is in the specified line.
*/
void LineExtent(TInt aLine, CTestTmTextLayout& aLayout,
	TInt& aLineStart, TInt& aLineEnd)
	{
	CTmTextLayout& layout = aLayout.Layout();
	TTmInterpreterParam interpParam(layout);
	RTmGeneralInterpreter interp(aLayout.Source(), interpParam);
	interp.LineNumberToLine(aLine);
	aLineStart = interp.LineInfo().iStart;
	aLineEnd = interp.LineInfo().iEnd;
	}

/**
Finds an edge in the expected edges list that matches a document position
specification.
*/
const TEdge* FindExpectedEdge(const TTmDocPosSpec& aPos,
	const TEdge* aExpected, TInt aNumExpected)
	{
	const TEdge* nearestTrailing = 0;
	TInt distanceTrailing = KMaxTInt;
	const TEdge* nearestLeading = 0;
	TInt distanceLeading = KMinTInt;
	for (const TEdge* e = aExpected; e != aExpected + aNumExpected; ++e)
		{
		TInt distance = e->iPos - aPos.iPos;
		if (!e->iLeading && 0 <= distance && distance < distanceTrailing)
			{
			distanceTrailing = distance;
			nearestTrailing = e;
			}
		if (e->iLeading && distanceLeading < distance && distance <= 0)
			{
			distanceLeading = distance;
			nearestLeading = e;
			}
		}
	if (aPos.iType == TTmDocPosSpec::ELeading || !nearestTrailing)
		{
		return nearestLeading;
		}
	if (aPos.iType == TTmDocPosSpec::ETrailing || !nearestLeading)
		{
		return nearestTrailing;
		}
	// Differences in iPos might be because pos is within a grapheme cluster,
	// or might be that the leading or trailing edge is not on that line.
	// Grapheme cluster differences are OK, not on the line differences will mean
	// that the one that does not match the input position is wrong.
	if (nearestLeading->iPos == aPos.iPos && nearestTrailing->iPos != aPos.iPos)
		return nearestLeading;
	if (nearestTrailing->iPos == aPos.iPos && nearestLeading->iPos != aPos.iPos)
		return nearestTrailing;
	TBool directionalitiesMatch = nearestTrailing->iRightToLeft?
		nearestLeading->iRightToLeft : !nearestLeading->iRightToLeft;
	if (directionalitiesMatch)
		return nearestLeading;
	TBool leadingIsCorrect = aPos.iType == TTmDocPosSpec::ERightToLeft?
		nearestLeading->iRightToLeft : !nearestLeading->iRightToLeft;
	return leadingIsCorrect? nearestLeading : nearestTrailing;
	}

/**
Returns ETrue if and only if the two edges specified are expected to be
coincident.
*/
TBool ExpectedEdgesCoincide(const TEdge* aA, const TEdge* aB)
	{
	const TEdge* a = aA < aB? aA : aB;
	const TEdge* b = aA < aB? aB : aA;
	while (a != b)
		{
		if (a->iNext != EEdgeSame)
			return EFalse;
		++a;
		}
	return ETrue;
	}

/**
Tests that the edge information matches the expected edge.
*/
void TestExpectedEdge(const TTmPosInfo2& aEdgeInfo,
	const TEdge* aExpected)
	{
	TESTPOINT(aEdgeInfo.iRightToLeft?
		aExpected->iRightToLeft : !aExpected->iRightToLeft);
	TESTPOINT(aEdgeInfo.iDocPos.iPos == aExpected->iPos);
	TESTPOINT(aEdgeInfo.iDocPos.iLeadingEdge?
		aExpected->iLeading : !aExpected->iLeading);
	}

/**
Tests that the edge information matches one of the expected edges.
*/
void TestEdgeExists(const TTmPosInfo2& aEdgeInfo,
	const TEdge* aExpected, TInt aNumExpected)
	{
	TTmDocPos pos(aEdgeInfo.iDocPos);
	const TEdge* edge = FindExpectedEdge(pos, aExpected, aNumExpected);
	TESTPOINT(edge != 0);
	TestExpectedEdge(aEdgeInfo, edge);
	}

/**
Tests that the visual position matches one of the expected edges.
*/
void TestVisualPositionExists(const TTmVisualDocPos& aPos,
	const TEdge* aExpected, TInt aNumExpected)
	{
    TESTPOINT(aPos.Ambiguity() != TTmVisualDocPos::ENotFound);
	TTmDocPos posLeft(aPos.LeftEdge().iDocPos);
	const TEdge* left = FindExpectedEdge(posLeft, aExpected, aNumExpected);
	TestExpectedEdge(aPos.LeftEdge(), left);
	TTmDocPos posRight(aPos.RightEdge().iDocPos);
	const TEdge* right = FindExpectedEdge(posRight, aExpected, aNumExpected);
	TestExpectedEdge(aPos.RightEdge(), right);
	TESTPOINT( (aPos.Ambiguity() == TTmVisualDocPos::EAmbiguous
			&& left->iAmbiguity && right->iAmbiguity)
		|| (aPos.Ambiguity() != TTmVisualDocPos::EAmbiguous
			&& !left->iAmbiguity && !right->iAmbiguity) );
	TESTPOINT(ExpectedEdgesCoincide(left, right));
	}

/**
Tests that a RTmGraphemeEdgeIterator iterates through all the positions in a
line from left to right.
*/
void TestLayoutSimplePass(CTestTmTextLayout& aLayout,
	const TEdge* aExpected, TInt aNumExpected)
	{
	CTmTextLayout& layout = aLayout.Layout();
	TTmInterpreterParam interpParam(layout);
	RTmGeneralInterpreter interp(aLayout.Source(), interpParam);
	interp.LineNumberToLine(0);
	RTmGraphemeEdgeIterator it;
	it.Begin(interp);
	TTmPosInfo2 last = it.GetInfo();
	for (TInt i = 0; i != aNumExpected; ++i)
		{
		const TEdge& expected = aExpected[i];
		TESTPOINT(expected.iPos == last.iDocPos.iPos);
		TESTPOINT(expected.iLeading == last.iDocPos.iLeadingEdge);
		it.Next();
		if (it.AtEnd())
			{
            TESTPOINT(expected.iNext == EEdgeNewline);
			while (interp.Op() != TTmInterpreter::EOpLine && interp.Next())
				{}
			if (i + 1 != aNumExpected)
				{
				it.Begin(interp);
				last = it.GetInfo();
				}
			}
		else
			{
			TTmPosInfo2 thisOne = it.GetInfo();
			TestEdgeExists(thisOne, aExpected, aNumExpected);
			TESTPOINT(expected.iNext != EEdgeNewline);
			if (expected.iNext == EEdgeSame)
			    TESTPOINT(last.iEdge.iX == thisOne.iEdge.iX);
			else if (expected.iNext == EEdgeDifferent)
			    TESTPOINT(last.iEdge.iX != thisOne.iEdge.iX);
			last = thisOne;
			}
		}
	it.Close();
	interp.Close();
	}

/**
Tests that FindXPos returns the edge 'closest' to the input co-ordinate
where there is no ambiguity.
*/
void TestLayoutFindXPosEdges(TInt aLine,
	CTestTmTextLayout& aLayout,
	const TEdge* aExpected, TInt aNumExpected)
	{
	CTmTextLayout& layout = aLayout.Layout();
	TTmInterpreterParam interpParam(layout);
	TTmPosInfo2 posInfo;
	TTmDocPosSpec posSpec;
	TTmLineInfo lineInfo;
	TTmVisualDocPos visPos;
	for (TInt i = 0; i != aNumExpected - 1; ++i)
		{
		const TEdge& expectedL = aExpected[i];
		const TEdge& expectedR = aExpected[i + 1];
		if (expectedL.iNext == EEdgeDifferent)
			{
			// This code assumes that no character has a width of exactly 1 pixel.
			if (!expectedL.iAmbiguity)
				{
				posSpec.iPos = expectedL.iPos;
				posSpec.iType = expectedL.iLeading?
					TTmDocPosSpec::ELeading : TTmDocPosSpec::ETrailing;
				layout.FindDocPos(posSpec, posInfo, lineInfo);
				RTmGeneralInterpreter interp(aLayout.Source(), interpParam);
				interp.LineNumberToLine(aLine);
				RTmGraphemeEdgeIterator it;
				it.Begin(interp);
				it.FindXPos(posInfo.iEdge.iX, visPos);
				TESTPOINT(visPos.Ambiguity() != TTmVisualDocPos::EAmbiguous);
				TESTPOINT(visPos.Ambiguity() != TTmVisualDocPos::ENotFound);
				TESTPOINT(visPos.LeftEdge().iDocPos.iPos == expectedL.iPos);
				TESTPOINT(visPos.LeftEdge().iDocPos.iLeadingEdge?
					expectedL.iLeading : !expectedL.iLeading);
				it.Close();
				interp.Close();
				}
			if (!expectedR.iAmbiguity)
				{
				posSpec.iPos = expectedR.iPos;
				posSpec.iType = expectedR.iLeading?
					TTmDocPosSpec::ELeading : TTmDocPosSpec::ETrailing;
				layout.FindDocPos(posSpec, posInfo, lineInfo);
				RTmGeneralInterpreter interp(aLayout.Source(), interpParam);
				interp.LineNumberToLine(aLine);
				RTmGraphemeEdgeIterator it;
				it.Begin(interp);
				it.FindXPos(posInfo.iEdge.iX - 1, visPos);
				TESTPOINT(visPos.Ambiguity() != TTmVisualDocPos::EAmbiguous);
				TESTPOINT(visPos.Ambiguity() != TTmVisualDocPos::ENotFound);
				TESTPOINT(visPos.LeftEdge().iDocPos.iPos == expectedR.iPos);
				TESTPOINT(visPos.LeftEdge().iDocPos.iLeadingEdge?
					expectedR.iLeading : !expectedR.iLeading);
				it.Close();
				interp.Close();
				}
			}
		}
	}

/**
Tests that RTmGraphemeEdgeIterator::FindXPos finds document positions that
match the positions they are supposed to be in.
*/
void TestLayoutFindXPos(TInt aLine,
	CTestTmTextLayout& aLayout,
	const TEdge* aExpected, TInt aNumExpected)
	{
	TInt lastLeftX = KMinTInt;
	TInt lastRightX = KMinTInt;
	TTmVisualDocPos visPos;
	TBool finished = EFalse;
	for (TInt x = -10; !finished; ++x)
		{
		CTmTextLayout& layout = aLayout.Layout();
		TTmInterpreterParam interpParam(layout);
		RTmGeneralInterpreter interp(aLayout.Source(), interpParam);
		interp.LineNumberToLine(aLine);
		RTmGraphemeEdgeIterator it;
		it.Begin(interp);
		it.FindXPos(x, visPos);
		TestVisualPositionExists(visPos, aExpected, aNumExpected);
		TESTPOINT(visPos.Ambiguity() != TTmVisualDocPos::ENotFound);
		TESTPOINT(visPos.LeftEdge().iEdge.iX <= visPos.RightEdge().iEdge.iX);
		TESTPOINT(visPos.Ambiguity() == TTmVisualDocPos::EAmbiguous
			|| visPos.LeftEdge().iEdge.iX == visPos.RightEdge().iEdge.iX);
		TESTPOINT(lastLeftX <= visPos.LeftEdge().iEdge.iX);
		if (lastLeftX == visPos.LeftEdge().iEdge.iX)
			{
            TESTPOINT(lastRightX == visPos.RightEdge().iEdge.iX);
			while (aExpected->iPos != visPos.LeftEdge().iDocPos.iPos
				|| aExpected->iLeading != visPos.LeftEdge().iDocPos.iLeadingEdge)
				{
                TESTPOINT(aExpected->iNext == EEdgeSame);
                TESTPOINT(0 < aNumExpected);
				++aExpected;
				--aNumExpected;
				}
			}
		else
			{
            TESTPOINT(lastRightX <= visPos.LeftEdge().iEdge.iX);
			while (aExpected->iPos != visPos.LeftEdge().iDocPos.iPos
				|| aExpected->iLeading != visPos.LeftEdge().iDocPos.iLeadingEdge)
				{
                TESTPOINT(0 < aNumExpected);
				++aExpected;
				--aNumExpected;
				}
			}
		if (interp.LineInfo().iInnerRect.iBr.iX + 120 < x)
			finished = ETrue;
		it.Close();
		interp.Close();
		}
	while (aExpected->iNext != EEdgeNewline)
		{
        TESTPOINT(aExpected->iNext == EEdgeSame);
        TESTPOINT(0 < aNumExpected);
		++aExpected;
		--aNumExpected;
		}
	}

/**
Uses RTmGraphemeEdgeIterator::FindEdge to find a document position in a
CTestTmTextLayout.
*/
TBool FindEdgeFromLayout(CTestTmTextLayout& aLayout, TInt aLine,
	const TTmDocPosSpec& aDocPos, TTmPosInfo2& aInfo)
	{
	CTmTextLayout& layout = aLayout.Layout();
	TTmInterpreterParam interpParam(layout);
	RTmGeneralInterpreter interp(aLayout.Source(), interpParam);
	interp.LineNumberToLine(aLine);
	RTmGraphemeEdgeIterator it;
	it.Begin(interp);
	TBool result = it.FindEdge(aDocPos, aInfo);
	it.Close();
	interp.Close();
	return result;
	}

/**
Tests that RTmGraphemeEdgeIterator::FindEdge finds the edges in the layout with
specifications of leading or trailing edges.
*/
void TestLayoutFindEdgesInVisualOrder(TInt aLine,
	CTestTmTextLayout& aLayout,
	const TEdge* aExpected, TInt aNumExpected)
	{
	TInt lastX = KMinTInt;
	TBool sameExpected = EFalse;
	TTmPosInfo2 posInfo;
	while (aNumExpected != 0)
		{
		TTmDocPosSpec posSpec(aExpected->iPos, aExpected->iLeading?
			TTmDocPosSpec::ELeading : TTmDocPosSpec::ETrailing);
		FindEdgeFromLayout(aLayout, aLine, posSpec, posInfo);
		TestEdgeExists(posInfo, aExpected, aNumExpected);
		TESTPOINT(aExpected->iLeading?
			posInfo.iDocPos.iLeadingEdge : !posInfo.iDocPos.iLeadingEdge);
		TESTPOINT(aExpected->iPos == posInfo.iDocPos.iPos);
		TESTPOINT(sameExpected || posInfo.iEdge.iX != lastX);
		TESTPOINT(!sameExpected || posInfo.iEdge.iX == lastX);
		lastX = posInfo.iEdge.iX;
		sameExpected = aExpected->iNext == EEdgeSame? ETrue : EFalse;
		++aExpected;
		--aNumExpected;
		}
	}

/**
Tests that RTmGraphemeEdgeIterator::FindEdge finds the edges in the layout with
specifications of directionality.
*/
void TestLayoutFindEdgesByDirectionality(TInt aLine,
	CTestTmTextLayout& aLayout,
	const TEdge* aExpected, TInt aNumExpected)
	{
	TInt lineStart;
	TInt lineEnd;
	LineExtent(aLine, aLayout, lineStart, lineEnd);
	TTmPosInfo2 lToRPosInfo;
	TTmPosInfo2 rToLPosInfo;
	for (TInt pos = lineStart - 1; pos != lineEnd + 1; ++pos)
		{
		TTmDocPosSpec rToLPosSpec(pos, TTmDocPosSpec::ERightToLeft);
		TBool rToLFound = FindEdgeFromLayout(aLayout, aLine, rToLPosSpec, rToLPosInfo);
		TTmDocPosSpec lToRPosSpec(pos, TTmDocPosSpec::ELeftToRight);
		TBool lToRFound = FindEdgeFromLayout(aLayout, aLine, lToRPosSpec, lToRPosInfo);
		if (!lToRFound)
			{
            TESTPOINT(!rToLFound);
            TESTPOINT(pos < lineStart || lineEnd <= pos);
			}
		else
			{
            TESTPOINT(rToLFound);
			TestEdgeExists(rToLPosInfo, aExpected, aNumExpected);
			TestEdgeExists(lToRPosInfo, aExpected, aNumExpected);
			// Now find the nearest edges in the expected range
			TTmDocPosSpec trailingPosSpec(pos, TTmDocPosSpec::ETrailing);
			const TEdge* trailingExpected
				= FindExpectedEdge(trailingPosSpec, aExpected, aNumExpected);
			TTmDocPosSpec leadingPosSpec(pos, TTmDocPosSpec::ELeading);
			const TEdge* leadingExpected
				= FindExpectedEdge(leadingPosSpec, aExpected, aNumExpected);
			if (!trailingExpected)
				trailingExpected = leadingExpected;
			if (!leadingExpected)
				leadingExpected = trailingExpected;
			const TEdge* rToLPosEdge
				= FindExpectedEdge(rToLPosInfo.iDocPos, aExpected, aNumExpected);
			const TEdge* lToRPosEdge
				= FindExpectedEdge(lToRPosInfo.iDocPos, aExpected, aNumExpected);
			TESTPOINT(leadingExpected != 0);
			TESTPOINT(trailingExpected != 0);
			TESTPOINT(ExpectedEdgesCoincide(leadingExpected, rToLPosEdge)
				|| ExpectedEdgesCoincide(trailingExpected, rToLPosEdge));
			TESTPOINT(ExpectedEdgesCoincide(leadingExpected, lToRPosEdge)
				|| ExpectedEdgesCoincide(trailingExpected, lToRPosEdge));
			// Also check that the "found" ones are at least as good as the
			// "expected" ones.
			TESTPOINT(rToLPosInfo.iRightToLeft
				|| (!leadingExpected->iRightToLeft && !trailingExpected->iRightToLeft));
			TESTPOINT(!lToRPosInfo.iRightToLeft
				|| (leadingExpected->iRightToLeft && trailingExpected->iRightToLeft));
			}
		}
	}

/**
Tests RTmGraphemeEdgeIterator::FindEdgeRightwards or
RTmGraphemeEdgeIterator::FindEdgeLeftwards.
*/
void TestLayoutFindEdgesLeftRight(TInt aLine,
	CTestTmTextLayout& aLayout, TBool aRightwards,
	const TEdge* aExpected, TInt aNumExpected)
	{
	TInt lineStart;
	TInt lineEnd;
	LineExtent(aLine, aLayout, lineStart, lineEnd);
	TTmPosInfo2 nearest;
	TTmVisualDocPos next;
	const TTmDocPosSpec::TType types[4]
		= {TTmDocPosSpec::ETrailing,
		TTmDocPosSpec::ELeading,
		TTmDocPosSpec::ELeftToRight,
		TTmDocPosSpec::ERightToLeft};

	for (TInt pos = lineStart - 1; pos != lineEnd + 1; ++pos)
		{
		for (TInt type = 0; type != 4; ++type)
			{
			TTmDocPosSpec posSpec(pos, types[type]);

			// What do we expect the nearest to be?
			TTmDocPosSpec leadingPosSpec(pos, TTmDocPosSpec::ELeading);
			TTmDocPosSpec trailingPosSpec(pos, TTmDocPosSpec::ETrailing);

			const TEdge* leadingExpected
				= FindExpectedEdge(leadingPosSpec, aExpected, aNumExpected);
			const TEdge* trailingExpected
				= FindExpectedEdge(trailingPosSpec, aExpected, aNumExpected);

			// but should we expect anything at all?
			if (pos < lineStart || lineEnd < pos)
				leadingExpected = trailingExpected = 0;
			if (posSpec.iType == TTmDocPosSpec::ELeading
				&& (!leadingExpected || !leadingExpected->iLeading))
				leadingExpected = trailingExpected = 0;
			if (posSpec.iType == TTmDocPosSpec::ETrailing
				&& (!trailingExpected || trailingExpected->iLeading))
				leadingExpected = trailingExpected = 0;

			// <lineEnd, trailing> is the only element that may be present
			// at position lineEnd.
			if (pos == lineEnd)
				{
				// If we are looking for a leading edge, we won't find
				// the trailing even if it is there.
				if (posSpec.iType == TTmDocPosSpec::ELeading
					|| !trailingExpected
					|| trailingExpected->iPos != pos)
					leadingExpected = trailingExpected = 0;
				}
			// <lineStart, trailing> may not be in the line.
			// We must check explicitly.
			if (pos == lineStart && posSpec.iType == TTmDocPosSpec::ETrailing)
				{
				// If there is no trailing edge at the start of the line
				// and we are looking for one, we
				// do not expect to have a nearest match there.
				if (!trailingExpected || trailingExpected->iPos != pos)
					leadingExpected = trailingExpected = 0;
				}

			if (!leadingExpected)
				leadingExpected = trailingExpected;
			if (!trailingExpected)
				trailingExpected = leadingExpected;

			const TEdge* nextExpected = 0;

			CTmTextLayout& layout = aLayout.Layout();
			TTmInterpreterParam interpParam(layout);
			RTmGeneralInterpreter interp(aLayout.Source(), interpParam);
			interp.LineNumberToLine(aLine);
			RTmGraphemeEdgeIterator it;
			it.Begin(interp);
			RTmGraphemeEdgeIterator::TEdgesFound result = aRightwards?
				it.FindEdgeRightwards(posSpec, nearest, next)
				: it.FindEdgeLeftwards(posSpec, nearest, next);
			interp.Close();

			// Does what we got match what we expect?
			if (!leadingExpected)
				{
                TESTPOINT(result == RTmGraphemeEdgeIterator::ENone);
				}
			else
				{
                TESTPOINT(result == RTmGraphemeEdgeIterator::ENearestOnly
					|| result == RTmGraphemeEdgeIterator::ENearestAndNext);
				TTmDocPosSpec nearestPos(nearest.iDocPos);
				const TEdge* nearestEdge
					= FindExpectedEdge(nearestPos, aExpected, aNumExpected);
				TestExpectedEdge(nearest, nearestEdge);
				const TEdge* matchingEdge = leadingExpected;
				if (posSpec.iType == TTmDocPosSpec::ELeading)
				    TESTPOINT(ExpectedEdgesCoincide(leadingExpected, nearestEdge));
				else if (posSpec.iType == TTmDocPosSpec::ETrailing)
					{
                    TESTPOINT(ExpectedEdgesCoincide(trailingExpected, nearestEdge));
					matchingEdge = trailingExpected;
					}
				else
					{
                    TESTPOINT(ExpectedEdgesCoincide(leadingExpected, nearestEdge)
						|| ExpectedEdgesCoincide(trailingExpected, nearestEdge));
					if (ExpectedEdgesCoincide(trailingExpected, nearestEdge))
						matchingEdge = trailingExpected;
					TBool directionalitiesMatch = leadingExpected->iRightToLeft?
						trailingExpected->iRightToLeft : !trailingExpected->iRightToLeft;
					TBool foundCorrectDirectionality
						= posSpec.iType == TTmDocPosSpec::ERightToLeft?
							nearest.iRightToLeft : !nearest.iRightToLeft;
					TESTPOINT(foundCorrectDirectionality || directionalitiesMatch);
					}

				// Find next edge in expected list
				const TEdge* e = matchingEdge;
				const TEdge* end = aRightwards?
					aExpected + aNumExpected - 1
					: aExpected;
				TInt direction = aRightwards? 1 : -1;
				while (nextExpected == 0 && e != end)
					{
					e += direction;
					if (!ExpectedEdgesCoincide(e, matchingEdge))
						nextExpected = e;
					}
				}
			if (!nextExpected)
			    TESTPOINT(result == RTmGraphemeEdgeIterator::ENone
					|| result == RTmGraphemeEdgeIterator::ENearestOnly);
			else
				{
                TESTPOINT(result == RTmGraphemeEdgeIterator::ENearestAndNext);
				TestVisualPositionExists(next, aExpected, aNumExpected);
				TESTPOINT(next.Ambiguity() != TTmVisualDocPos::ENotFound);
				TTmDocPosSpec nextPosLeft(next.LeftEdge().iDocPos);
				TESTPOINT(ExpectedEdgesCoincide(nextExpected,
					FindExpectedEdge(nextPosLeft, aExpected, aNumExpected)));
				TTmDocPosSpec nextPosRight(next.RightEdge().iDocPos);
				TESTPOINT(ExpectedEdgesCoincide(nextExpected,
					FindExpectedEdge(nextPosRight, aExpected, aNumExpected)));
				}
			it.Close();
			}
		}
	}

/**
Tests RTmGraphemeEdgeIterator::FindEdgeRightwards.
*/
void TestLayoutFindEdgesRightwards(TInt aLine,
	CTestTmTextLayout& aLayout,
	const TEdge* aExpected, TInt aNumExpected)
	{
	TestLayoutFindEdgesLeftRight(aLine, aLayout, ETrue,
		aExpected, aNumExpected);
	}

/**
Tests RTmGraphemeEdgeIterator::FindEdgeLeftwards.
*/
void TestLayoutFindEdgesLeftwards(TInt aLine,
	CTestTmTextLayout& aLayout,
	const TEdge* aExpected, TInt aNumExpected)
	{
	TestLayoutFindEdgesLeftRight(aLine, aLayout, EFalse,
		aExpected, aNumExpected);
	}

/**
Tests RTmGraphemeEdgeIterator::NextPosition. Expected behaviour is to find the
smallest number 'n' that is a position in the same line greater than the input 'i',
where the positions <i, leading> and <n, trailing> are not coincident.
*/
void TestLayoutFindEdgesForwards(TInt aLine,
	CTestTmTextLayout& aLayout,
	const TEdge* aExpected, TInt aNumExpected)
	{
	TInt lineStart;
	TInt lineEnd;
	LineExtent(aLine, aLayout, lineStart, lineEnd);
	for (TInt i = lineStart - 1; i != lineEnd + 1; ++i)
		{
		CTmTextLayout& layout = aLayout.Layout();
		TTmInterpreterParam interpParam(layout);
		RTmGeneralInterpreter interp(aLayout.Source(), interpParam);
		interp.LineNumberToLine(aLine);
		RTmGraphemeEdgeIterator it;
		it.Begin(interp);
		TInt result = it.NextPosition(i);
		interp.Close();

		if (result == KErrNotFound)
			{
			// Must be at or after the line's end.
			// Try to find a later edge in the line...
			TTmDocPosSpec in(i, TTmDocPosSpec::ELeading);
			const TEdge* inEdge = FindExpectedEdge(in,
				aExpected, aNumExpected);
			TTmDocPosSpec out(result, TTmDocPosSpec::ETrailing);
			const TEdge* outEdge = FindExpectedEdge(out,
				aExpected, aNumExpected);
			// ...and test that we failed.
			TESTPOINT(!inEdge || inEdge->iPos <= i);
			TESTPOINT(!outEdge || outEdge->iPos <= i);
			}
		else
			{
            TESTPOINT(i < result);
			TTmDocPosSpec in(i, TTmDocPosSpec::ELeading);
			const TEdge* inEdge = FindExpectedEdge(in,
				aExpected, aNumExpected);
			TTmDocPosSpec out(result, TTmDocPosSpec::ETrailing);
			const TEdge* outEdge = FindExpectedEdge(out,
				aExpected, aNumExpected);
			TESTPOINT(outEdge != 0);
			if (inEdge)
				{
                TESTPOINT(lineStart <= i);
                TESTPOINT(!ExpectedEdgesCoincide(inEdge, outEdge));
				for (TInt j = i + 1; j != result; ++j)
					{
					TTmDocPosSpec between(j, TTmDocPosSpec::ETrailing);
					const TEdge* betweenEdge = FindExpectedEdge(between,
						aExpected, aNumExpected);
					TESTPOINT(betweenEdge != 0);
					// Test that, if there actually is a <j, trailing> edge, it is
					// coincident with <i, leading>. If the edge does not exist
					// it does not matter. We can find out if it exists by checking
					// whether the returned expected edge has the same position
					// we asked for.
					TESTPOINT(ExpectedEdgesCoincide(inEdge, betweenEdge)
						|| j != betweenEdge->iPos);
					}
				}
			else
				{
				// before the start means finding the first trailing edge
				TESTPOINT (i < lineStart);
				TInt leastTrailingEdge = KMaxTInt;
				for (const TEdge* e = aExpected; e != aExpected + aNumExpected;
					++e)
					{
					if (!e->iLeading && e->iPos < leastTrailingEdge)
						leastTrailingEdge = e->iPos;
					}
				TESTPOINT(leastTrailingEdge == result);
				}
			}
		it.Close();
		}
	}

/**
Tests RTmGraphemeEdgeIterator::PreviousPosition. Expected behaviour is to find the
largest number 'n' that is a position in the same line smaller than the input 'i',
where the positions <i, trailing> and <n, leading> are not coincident.
*/
void TestLayoutFindEdgesBackwards(TInt aLine,
	CTestTmTextLayout& aLayout,
	const TEdge* aExpected, TInt aNumExpected)
	{
	TInt lineStart;
	TInt lineEnd;
	LineExtent(aLine, aLayout, lineStart, lineEnd);
	for (TInt i = lineStart - 1; i != lineEnd + 1; ++i)
		{
		CTmTextLayout& layout = aLayout.Layout();
		TTmInterpreterParam interpParam(layout);
		RTmGeneralInterpreter interp(aLayout.Source(), interpParam);
		interp.LineNumberToLine(aLine);
		RTmGraphemeEdgeIterator it;
		it.Begin(interp);
		TInt result = it.PreviousPosition(i);
		interp.Close();

		if (result == KErrNotFound)
			{
			// Must be at or before the line's beginning.
			// Could possibly be that there are no leading edges in the line, but
			// we'll ignore that possibility.
			TESTPOINT(i <= lineStart);
			}
		else
			{
			TESTPOINT(result < i);
			TTmDocPosSpec out(result, TTmDocPosSpec::ELeading);
			const TEdge* outEdge = FindExpectedEdge(out,
				aExpected, aNumExpected);
			TESTPOINT(outEdge != 0);
			TTmDocPosSpec in(i, TTmDocPosSpec::ETrailing);
			const TEdge* inEdge = FindExpectedEdge(in,
				aExpected, aNumExpected);
			// if we could not find a trailing edge at this number, then we
			// were beyond the end of the line.
			if (inEdge && !inEdge->iLeading)
				{
				TESTPOINT(inEdge != 0);
				TESTPOINT(!ExpectedEdgesCoincide(inEdge, outEdge));
				for (TInt j = result + 1; j != i; ++j)
					{
					TTmDocPosSpec between(j, TTmDocPosSpec::ELeading);
					const TEdge* betweenEdge = FindExpectedEdge(between,
						aExpected, aNumExpected);
					TESTPOINT(betweenEdge != 0);
					// Test that, if there actually is a <j, trailing> edge, it is
					// coincident with <i, leading>. If the edge does not exist
					// it does not matter. We can find out if it exists by checking
					// whether the returned expected edge has the same position
					// we asked for.
					TESTPOINT(ExpectedEdgesCoincide(inEdge, betweenEdge)
						|| j != betweenEdge->iPos);
					}
				}
			else
				{
				// after the end means finding the last leading edge
				TInt greatestLeadingEdge = KMinTInt;
				for (const TEdge* e = aExpected; e != aExpected + aNumExpected;
					++e)
					{
					if (e->iLeading && greatestLeadingEdge < e->iPos)
						greatestLeadingEdge = e->iPos;
					}
				TESTPOINT(greatestLeadingEdge == result);
				}
			}
		it.Close();
		}
	}

typedef void FTestLine(TInt aLine,
	CTestTmTextLayout& aLayout,
	const TEdge* aExpected, TInt aNumExpected);

/**
Runs a particular test for each line in the input data.
*/
void TestEachLine(FTestLine* aFn,
	CTestTmTextLayout& aLayout, const TEdge* aExpected, TInt aNumExpected)
	{
	TInt line = 0;
	TInt start = 0;
	for (TInt end = 1; end != aNumExpected; ++end)
		{
		if (aExpected[end - 1].iNext == EEdgeNewline)
			{
			aFn(line, aLayout, aExpected + start, end - start);
			start = end;
			++line;
			}
		}
	}

/**
Tests TTmGraphemeIterator and supporting functionality for the specified
layout.
*/
void TestLayoutL(CTestTmTextLayout& aLayout,
	const TEdge* aExpected, TInt aNumExpected)
	{
	TESTPRINT(_L("Simple iteration"));
	TestLayoutSimplePass(aLayout, aExpected, aNumExpected);
	TESTPRINT(_L("FindXPos"));
	TestEachLine(TestLayoutFindXPos,
		aLayout, aExpected, aNumExpected);
	TESTPRINT(_L("FindXPos (unambiguous edges)"));
	TestEachLine(TestLayoutFindXPosEdges,
		aLayout, aExpected, aNumExpected);
	TESTPRINT(_L("FindEdge"));
	TestEachLine(TestLayoutFindEdgesInVisualOrder,
		aLayout, aExpected, aNumExpected);
	TestEachLine(TestLayoutFindEdgesByDirectionality,
		aLayout, aExpected, aNumExpected);
	TESTPRINT(_L("FindEdgeRightwards"));
	TestEachLine(TestLayoutFindEdgesRightwards,
		aLayout, aExpected, aNumExpected);
	TESTPRINT(_L("FindEdgeLeftwards"));
	TestEachLine(TestLayoutFindEdgesLeftwards,
		aLayout, aExpected, aNumExpected);
	TESTPRINT(_L("NextPosition"));
	TestEachLine(TestLayoutFindEdgesForwards,
		aLayout, aExpected, aNumExpected);
	TESTPRINT(_L("PreviousPosition"));
	TestEachLine(TestLayoutFindEdgesBackwards,
		aLayout, aExpected, aNumExpected);
	}

/**
Tests TTmGraphemeIterator and supporting functionality for each piece of text.
*/
TVerdict CTGraphemeIteratorStep::doTestStepL()
	{
    SetTestStepResult(EPass);
    TestStep = this;
    TESTPRINT(_L("RTmGraphemeEdgeIterator unit"));
    
    TESTPRINT(_L(" @SYMTestCaseID:SYSLIB-FORM-LEGACY-GRAPHEMEITERATOR-0001 DocPosMatches "));
	TestDocPosMatches();
	TESTPRINT(_L("Simple Latin"));
	CTestTmTextLayout* latin1 = CTestTmTextLayout::NewLC(
		KLatin1, 100, Transliterate);
	TestLayoutL( *latin1, KLatin1Edges,
		sizeof(KLatin1Edges)/sizeof(KLatin1Edges[0]));
	CleanupStack::PopAndDestroy(latin1);

	TESTPRINT(_L("Simple Arabic"));
	CTestTmTextLayout* arabic1 = CTestTmTextLayout::NewLC(
		KArabic1, 100, Transliterate);
	TestLayoutL(*arabic1, KArabic1Edges,
		sizeof(KArabic1Edges)/sizeof(KArabic1Edges[0]));
	CleanupStack::PopAndDestroy(arabic1);

	TESTPRINT(_L("Latin with combining marks and zero width characters"));
	CTestTmTextLayout* combiners1 = CTestTmTextLayout::NewLC(
		KCombiners1, 20, Transliterate);
	TestLayoutL(*combiners1, KCombiners1Edges,
		sizeof(KCombiners1Edges)/sizeof(KCombiners1Edges[0]));
	CleanupStack::PopAndDestroy(combiners1);

	TESTPRINT(_L("Bidirectional text with combining marks"));
	CTestTmTextLayout* bidi1 = CTestTmTextLayout::NewLC(
		KBidi1, 60, Transliterate);
	TestLayoutL( *bidi1, KBidi1Edges,
		sizeof(KBidi1Edges)/sizeof(KBidi1Edges[0]));
	CleanupStack::PopAndDestroy(bidi1);

	TESTPRINT(_L("Bidirectional text with combining marks and 'amtriguity'"));
	CTestTmTextLayout* bidi2 = CTestTmTextLayout::NewLC(
		KBidi2, 60, Transliterate);
	TestLayoutL(*bidi2, KBidi2Edges,
		sizeof(KBidi2Edges)/sizeof(KBidi2Edges[0]));
	CleanupStack::PopAndDestroy(bidi2);

	TESTPRINT(_L("Small paragraphs of alternating directionality"));
	CTestTmTextLayout* paragraphs1 = CTestTmTextLayout::NewLC(
		KParagraphs1, 20, Transliterate);
	TestLayoutL(*paragraphs1, KParagraphs1Edges,
		sizeof(KParagraphs1Edges)/sizeof(KParagraphs1Edges[0]));
	CleanupStack::PopAndDestroy(paragraphs1);

	TESTPRINT(_L("Lines ending over or next to embedded runs"));
	CTestTmTextLayout* embedded1 = CTestTmTextLayout::NewLC(
		KEmbedded1, 20, Transliterate);
	TestLayoutL( *embedded1, KEmbedded1Edges,
		sizeof(KEmbedded1Edges)/sizeof(KEmbedded1Edges[0]));
	CleanupStack::PopAndDestroy(embedded1);

	return TestStepResult();
	}

