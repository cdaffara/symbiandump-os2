/*
* Copyright (c) 2002-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Test code for MTmSource functionality
*
*/


#include "TAGMA.H"
#include <e32test.h>

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "TAGMA_INTERNAL.H"
#endif

#include "ttmsource.h"


#define UNUSED_VAR(a) a = a

namespace LocalToTTmSource
{
CTTmSourceStep* TestStep = NULL;
#define TESTPOINT(p) TestStep->testpoint(p,(TText8*)__FILE__,__LINE__)
#define TESTPRINT(p) TestStep->print(p,(TText8*)__FILE__,__LINE__)
}
using namespace LocalToTTmSource;

class TTestGraphicsDeviceMap : public MGraphicsDeviceMap
	{
public:
	TInt HorizontalTwipsToPixels(TInt a) const { return a; }
	TInt VerticalTwipsToPixels(TInt a) const { return a; }
	TInt HorizontalPixelsToTwips(TInt a) const { return a; }
	TInt VerticalPixelsToTwips(TInt a) const { return a; }
	TInt GetNearestFontInTwips(CFont*&,const TFontSpec&)
		{
		return KErrGeneral;
		}
	void ReleaseFont(CFont*) {}
	};

class CTestPicture : public CPicture
	{
public:
	CTestPicture() {}
	virtual void Draw(CGraphicsContext&, const TPoint&, const TRect&, MGraphicsDeviceMap*) const {}
	virtual void ExternalizeL(RWriteStream&) const {}
	virtual void GetOriginalSizeInTwips(TSize&) const {}
	virtual TBool LineBreakPossible(TUint aClass,TBool aBeforePicture,TBool aHaveSpaces) const
		{
		++iRequestCount;
		if (aBeforePicture)
			{
            TESTPOINT(aClass == iClassBefore);
            TESTPOINT(aHaveSpaces == iSpacesBefore);
			return iResultBefore;
			}
		TESTPOINT(aClass == iClassAfter);
		TESTPOINT(aHaveSpaces == iSpacesAfter);
		return iResultAfter;
		}
	// expected parameters for LineBreakPossible
	TUint iClassBefore;
	TUint iClassAfter;
	TBool iSpacesBefore;
	TBool iSpacesAfter;
	// Results for breaking before/breaking after
	TBool iResultBefore;
	TBool iResultAfter;
	mutable TInt iRequestCount;
	};

class TTestSource : public MTmSource
	{
public:
	TTestSource() : iPicturePos(-1) {}
	virtual ~TTestSource() {}
	MGraphicsDeviceMap& FormatDevice() const { return iGDM; }
	MGraphicsDeviceMap& InterpretDevice() const { return iGDM; }
	TInt DocumentLength() const
		{
		return iText->Length();
		}
	void GetText(TInt aPos,TPtrC& aText, TTmCharFormat& aFormat) const
		{
		TTmCharFormat f;
		aFormat = f;
		aText.Set(iText->Mid(aPos));
		}
	void GetParagraphFormatL(TInt, RTmParFormat& aFormat) const
		{
		RTmParFormat p;
		aFormat.CopyL(p);
		}
	CPicture* PictureL(TInt aPos) const
		{
		return aPos == iPicturePos? iPicture : 0;
		}
	TInt ParagraphStart(TInt) const { return 0; }

	virtual TUint LineBreakClass(TUint aCode, TUint& aRangeStart,
		TUint& aRangeEnd) const
		{
		if ('@' == aCode)
			{
			aRangeStart = aRangeEnd = aCode;
			return ESaLineBreakClass;
			}
		if ('0' <= aCode && aCode <= '9')
			{
			aRangeStart = aRangeEnd = aCode;
			return ELineBreakClasses + aCode - '0';
			}
		return MTmSource::LineBreakClass(aCode, aRangeStart, aRangeEnd);
		}

	virtual TBool LineBreakPossible(TUint aPrevClass, TUint aNextClass,
		TBool aHaveSpaces) const
		{
		TInt first = static_cast<TInt>(aPrevClass);
		TInt second = static_cast<TInt>(aNextClass);
		TInt customCount = 0;
		if (iDirection < 0)
			{
			first = aNextClass;
			second = aPrevClass;
			}
		if (ELineBreakClasses <= first && first < ELineBreakClasses + 10)
			{
			++customCount;
			TESTPOINT(first - ELineBreakClasses + '0' == FindNextCustomClass());
			TInt countSpaces = CountSpaces();
			TESTPOINT(!aHaveSpaces == !countSpaces);
			}
		if (ELineBreakClasses <= second && second < ELineBreakClasses + 10)
			{
			++customCount;
			TInt c = FindNextCustomClass();
			TESTPOINT(second - ELineBreakClasses + '0' == c);
			}
		if (0 == customCount)
			return MTmSource::LineBreakPossible(aPrevClass, aNextClass, aHaveSpaces);
		// Between custom and non-custom classes, allow a break only with spaces
		// or between @ and 5
		if (1 == customCount)
			return aHaveSpaces
				|| (first == ESaLineBreakClass && second == ELineBreakClasses + 5)
				|| (second == ESaLineBreakClass && first == ELineBreakClasses + 5);
		// Allow a break with spaces except after '0' or before '9'
		if (aHaveSpaces)
			return aPrevClass != ELineBreakClasses && aNextClass != ELineBreakClasses + 9;
		// Allow a break only between a class and the class one more than it.
		return aPrevClass + 1 == aNextClass;
		}

	virtual TBool GetLineBreakInContext(
		const TDesC& aText, TInt aMinBreakPos, TInt aMaxBreakPos,
		TBool aForwards,TInt& aBreakPos) const
		{
	    TESTPOINT (iDirection == (aForwards? 1 : -1));
		// The allowable break-points should not include the first
		// and last characters of the run.
	    TESTPOINT (aMinBreakPos != 0);
		for (TInt i = aMinBreakPos - 1; i <= aMaxBreakPos; ++i)
		    TESTPOINT('@' == aText[i]);
		++iSaRequestCount;
		aBreakPos = iText->Ptr() + iSaBreakpoint - aText.Ptr();
		return aMinBreakPos <= aBreakPos && aBreakPos <= aMaxBreakPos;
		}

	virtual TBool IsHangingCharacter(TUint aChar) const
		{
		++iHangingCharRequestCount;
		TESTPOINT(aChar == (*iText)[iMaxBreakPos]);
		if (!iHangingChar)
			return EFalse;
		if (iDirection < 0)
			++iCurrentPos;
		return ETrue;
		}

	// non-virtual
	TBool GetLineBreakL(const TDesC& aText, TInt aDocPos,
		TInt aMinBreakPos, TInt aMaxBreakPos, TBool aForwards,
		TInt& aBreakPos, TInt& aHangingChars, TInt& aBreakPosAfterSpaces) const
		{
		iText = &aText;
		iMaxBreakPos = aMaxBreakPos;
		iMinBreakPos = aMinBreakPos;
		iHangingCharRequestCount = 0;
		iSaRequestCount = 0;
		iDirection = aForwards? 1 : -1;
		iCurrentPos = aForwards? aMinBreakPos : aMaxBreakPos - 1;
		TBool r = MTmSource::GetLineBreakL(aText, aDocPos,
			aMinBreakPos, aMaxBreakPos, aForwards,
			aBreakPos, aHangingChars, aBreakPosAfterSpaces);
		if (r)
			{
            TESTPOINT(aMinBreakPos <= aBreakPos);
            TESTPOINT(0 < aBreakPos);
            TESTPOINT(aBreakPos <= aHangingChars);
            TESTPOINT(aHangingChars <= aBreakPosAfterSpaces);
            TESTPOINT(aBreakPos <= aMaxBreakPos);
            TESTPOINT(aHangingChars == aBreakPos || iHangingChar);
			// If the direction was backwards, the algorithm should have
			// checked if a hanging character was allowed.
			// This condition could be relaxed to allow it not to be checked
			// if there is no break allowed between the possible hanging
			// character and the previous character.
            TESTPOINT(!aForwards || aText.Length() == aMaxBreakPos
				|| 0 < iHangingCharRequestCount);
			// If the maximum break point was chosen or exceeded, the algorithm
			// should have checked to find out whether a hanging character is
			// allowed.
            TESTPOINT(aHangingChars < aMaxBreakPos
				|| 0 < iHangingCharRequestCount);
			// Check that only spaces exist between aHangingChars and
			// aMaxBreakPos
			for (TInt i = aHangingChars; i != aBreakPosAfterSpaces; ++i)
				{
				TUint n;
				TESTPOINT(ESpLineBreakClass == LineBreakClass(aText[i], n, n));
				}
			// Check that all the spaces were counted
			TESTPOINT(aBreakPosAfterSpaces == aText.Length()
				|| aText[aBreakPosAfterSpaces] != ' ');
			}
		// Find out how many runs of two or more Sa there are, and check that
		// this matches the number of times that it was requested.
		TInt minChecked = aMinBreakPos - 1;
		TInt maxChecked = aMaxBreakPos + 2;
		if (r)
			{
			if (aForwards)
				maxChecked = aBreakPos + 1;
			else
				minChecked = aBreakPos - 1;
			}
		if (minChecked < 0)
			minChecked = 0;
		if (aText.Length() < maxChecked)
			maxChecked = aText.Length();
		TInt runs = 0;
		TInt sasSoFar = 0;
		TESTPOINT (maxChecked - minChecked < 2
			|| aText[minChecked] != '@'
			|| aText[minChecked + 1] != '@'
			|| !aForwards
			|| aHangingChars == iSaBreakpoint);
		for (; minChecked != maxChecked; ++minChecked)
			{
			if (aText[minChecked] == '@')
				++sasSoFar;
			else
				{
				if (1 < sasSoFar)
					++runs;
				sasSoFar = 0;
				}
			}
		if (1 < sasSoFar)
			++runs;
		TESTPOINT(sasSoFar < 2 || aForwards || aHangingChars == iSaBreakpoint);
		TESTPOINT(runs == iSaRequestCount);
		return r;
		}

	TInt FindNextCustomClass() const
		{
		TInt end = iDirection < 0? -1 : iText->Length();
		for (; iCurrentPos != end; iCurrentPos += iDirection)
			{
			TInt c = (*iText)[iCurrentPos];
			if ('0' <= c && c <= '9')
				return c;
			}
		return -1;
		}
	TInt CountSpaces() const
		{
		TInt end = iDirection < 0? -1 : iText->Length();
		TInt count = 0;
		if (iCurrentPos == end)
			return 0;
		iCurrentPos += iDirection;
		for (; iCurrentPos != end; iCurrentPos += iDirection, ++count)
			{
			TInt c = (*iText)[iCurrentPos];
			if (' ' != c)
				return count;
			}
		return count;
		}

private:
	mutable TTestGraphicsDeviceMap iGDM;
	mutable const TDesC* iText;
	mutable TInt iMaxBreakPos;
	mutable TInt iMinBreakPos;

	mutable TInt iDirection;
	mutable TInt iCurrentPos;
	mutable TInt iHangingCharRequestCount;
	mutable TInt iSaRequestCount;

public:
	TInt iPicturePos;
	CPicture* iPicture;

	TBool iHangingChar;

	TInt iSaBreakpoint;
	};

TInt TestLineBreak(const TDesC& aText, TInt aSaBreak, TBool aHangingChar,
	TInt aMin, TInt aMax, TBool aForwards)
	{
	if (aMax == 0)
		aMax = aText.Length();
	TTestSource t;
	t.iHangingChar = aHangingChar;
	t.iSaBreakpoint = aSaBreak;
	TInt b0, b1, b2;
	b0 = KMaxTInt;
	b1 = KMaxTInt;
	b2 = KMaxTInt;
	return t.GetLineBreakL(aText, 0, aMin, aMax, aForwards, b0, b1, b2)?
		b1 : -1;
	}

CTTmSourceStep::CTTmSourceStep()
    {
    
    }


TVerdict CTTmSourceStep::doTestStepL()
	{
    SetTestStepResult(EPass);
    TestStep = this;
    TESTPRINT(_L("TTmSource - MTmSource tests"));
	TESTPRINT(_L(" @SYMTestCaseID:SYSLIB-FORM-LEGACY-TTMSOURCE-0001 Line-Break Tests: "));

	TEST(-1 == TestLineBreak(_L(""), 0, 0, 0, 0, 0));
	TEST(-1 == TestLineBreak(_L("5"), 0, 0, 0, 0, 0));
	TEST(-1 == TestLineBreak(_L("5"), 0, 0, 0, 0, 1));
	TEST(-1 == TestLineBreak(_L("@"), 1, 0, 0, 0, 0));
	TEST(1 == TestLineBreak(_L("a   b"), 0, 0, 0, 0, 0));
	TEST(-1 == TestLineBreak(_L("0 0 0 9    9"), 0, 0, 0, 0, 0));
	TEST(-1 == TestLineBreak(_L("0 0 0 9    9"), 0, 0, 0, 0, 1));
	TEST(9 == TestLineBreak(_L("4242454445"), 0, 0, 0, 0, 0));
	TEST(5 == TestLineBreak(_L("4242454445"), 0, 0, 0, 0, 1));
	TEST(5 == TestLineBreak(_L("hello there"), 0, 0, 0, 0, 0));
	TEST(5 == TestLineBreak(_L("hello there"), 0, 0, 0, 0, 1));
	TEST(-1 == TestLineBreak(_L("hel  the re"), 0, 0, 5, 7, 0));
	TEST(-1 == TestLineBreak(_L("hel  the re"), 0, 0, 5, 7, 1));
	TEST(8 == TestLineBreak(_L("hel  the re"), 0, 1, 5, 7, 0));
	TEST(8 == TestLineBreak(_L("hel  the re"), 0, 1, 6, 7, 1));
	TEST(3 == TestLineBreak(_L("@@@@@"), 3, 0, 0, 0, 0));
	TEST(3 == TestLineBreak(_L("@@@@@"), 3, 0, 0, 0, 1));
	TEST(5 == TestLineBreak(_L("9999@@@@@00099@@@@gfra"), 5, 0, 5, 0, 0));
	TEST(5 == TestLineBreak(_L("9999@@@@@00099@@@@gfra"), 5, 0, 5, 0, 1));
	TEST(16 == TestLineBreak(_L("9999@@@@@00099@@@@gfra"), 16, 0, 0, 0, 0));
	TEST(16 == TestLineBreak(_L("9999@@@@@00099@@@@gfra"), 16, 0, 0, 0, 1));
	TEST(5 == TestLineBreak(_L("55@@@55"), 0, 0, 0, 0, 0));
	TEST(2 == TestLineBreak(_L("55@@@55"), 0, 0, 0, 0, 1));
	TEST(3 == TestLineBreak(_L("55@55"), 0, 0, 0, 0, 0));
	TEST(2 == TestLineBreak(_L("55@55"), 0, 0, 0, 0, 1));

	// Test for DEF046468, which was caused by the TLineBreakIterator constructor accessing past the end of a string
	TESTPRINT(_L("Line-Break DEF046468 Test:"));
	// Create a string of 16 chars with a picture code at the 17th position
	_LIT(KLarsString, "dolor sit amet, \xFFFC");
	// Create a TPtrC for the 16 character string ( with the picture code after the string in memory )
	TBufC<20> KTestBuffer(KLarsString);
	TPtrC KTestString( reinterpret_cast<const TUint16*>(KTestBuffer.Ptr()), 16);
	// Test the iterator overrun. If iterator accesses past the end of the array, it'll get picture code and crash
	TEST(9 == TestLineBreak(KTestString,0,0,1,15,0));
	
	return TestStepResult();
	}
