/*
* Copyright (c) 2004-2010 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/

//#include "TCustomWrap.h"
#include "TGraphicsContext.h"
#include <e32std.h>
#include <e32test.h>
#include <frmtlay.h>
#include <frmtview.h>
#include <txtlaydc.h>
#include <fbs.h>
#include <w32std.h>
#include <inlinetext.h>
#include "tinlinetext.h"

namespace LocalToTInlineText
{

_LIT(KTInlineText, "TInlineText");
const TInt KDisplayWidthWide = 202;
const TInt KDisplayWidthThin = 102;
const TInt KDisplayHeight = 100;
const TInt KPictureCharacter = 0xFFFC;

CTInLineTextStep* TestStep = NULL;
#define TESTPOINT(p) TestStep->testpoint(p,(TText8*)__FILE__,__LINE__)
#define TESTPRINT(p) TestStep->print(p,(TText8*)__FILE__,__LINE__)


enum TInlineTextPanic { EAccessOutsideText = 1 };
void Panic(TInlineTextPanic)
    {
    User::Panic(_L("TInlineText"), EAccessOutsideText);
    }

class CPinkSquare : public CPicture
	{
public:
	// Size of square in twips.
	// 600 is 15 pixels using the standard test graphics device at
	// its default resolution.
	enum { KWidth = 600, KHeight = 600 };
	CPinkSquare()
		{}
	void Draw(CGraphicsContext& aGc, const TPoint& aTopLeft, const TRect& aClipRect, MGraphicsDeviceMap* aMap) const
		{
		// This picture is a magenta square
		TPoint size(KWidth, KHeight);
		if (aMap)
			size = aMap->TwipsToPixels(size);
		TRect rect(aTopLeft, aTopLeft + size);
		aGc.SetClippingRect(aClipRect);
		aGc.SetDrawMode(CGraphicsContext::EDrawModePEN);
		aGc.SetPenColor(KRgbMagenta);
		aGc.SetBrushStyle(CGraphicsContext::ESolidBrush);
		aGc.SetBrushColor(KRgbMagenta);
		aGc.DrawRect(rect);
		}
	void ExternalizeL(RWriteStream&) const
		{}
	void GetOriginalSizeInTwips(TSize& a) const
		{
		a.iWidth = CPinkSquare::KWidth;
		a.iHeight = CPinkSquare::KHeight;
		}
	};

_LIT(KEnd, "\x2029");
class TDocModel : public MLayDoc
	{
public:
	TDocModel(const TDesC& aDes)
	 :	iDes(&aDes), iParagraphFormat(0)
		{}
	void SetParagraphFormat(CParaFormat* a)
		{
		iParagraphFormat = a;
		}
	// From MLayDoc
	TInt LdDocumentLength() const
		{ return iDes->Length(); }
	TInt LdToParagraphStart(TInt& a) const
		{
		TInt curr = a;
		if (a < LdDocumentLength())
			{
			a = iDes->Left(a).LocateReverse(0x2029);
			a = a < 0? 0 : a + 1;
			}
		return curr - a;
		}
	void GetParagraphFormatL(CParaFormat* aFormat, TInt) const
		{
		if (iParagraphFormat)
			{
			aFormat->CopyL(*iParagraphFormat);
			return;
			}
		aFormat->Reset();
		TTabStop tabStop;
		tabStop.iTwipsPosition = 1000;
		tabStop.iType = TTabStop::ELeftTab;
		aFormat->StoreTabL(tabStop);
		}
	void GetChars(TPtrC& aView,TCharFormat& aFormat, TInt aStartPos)const
		{
        __ASSERT_ALWAYS(aStartPos <= LdDocumentLength(), Panic(EAccessOutsideText));
        __ASSERT_ALWAYS(aStartPos >= 0, Panic(EAccessOutsideText));
        
		TCharFormat cf;
		aFormat = cf;
		if (aStartPos == LdDocumentLength())
			aView.Set(KEnd);
		else
			aView.Set(iDes->Mid(aStartPos));
		}
	TInt GetPictureSizeInTwips(TSize& aSize, TInt aPos) const
		{
		if ((*iDes)[aPos] != KPictureCharacter)
			return KErrNotFound;
		aSize.iWidth = CPinkSquare::KWidth;
		aSize.iHeight = CPinkSquare::KHeight;
		return KErrNone;
		}
	CPicture* PictureHandleL(TInt aPos, TForcePictureLoad) const
		{
		if ((*iDes)[aPos] != KPictureCharacter)
			return 0;
		return new(ELeave) CPinkSquare;
		}
	TBool EnquirePageBreak(TInt aPos, TInt aLength)const
		{
		return iDes->Mid(aPos, aLength).Locate(0x000C) < 0?
			EFalse : ETrue;
		}
	TBool SelectParagraphLabel(TInt)
		{ return EFalse; }
	void CancelSelectLabel()
		{}
private:
	const TDesC* iDes;
	CParaFormat* iParagraphFormat;
	};
}
using namespace LocalToTInlineText;

class CTestTextView	// slightly naughty
	{
public:
	static void SetContextForFlickerFreeRedraw(CTextView* aView, CBitmapContext* aContext)
		{
		aView->iOffScreenContext = aContext;
		}
	};

// Utility functions to show contents of test data using test.Printf

_LIT(KAddressMarker, "> ");
_LIT(KSpace, " ");
_LIT(KLength, ", Length of Data = %d 16-bit words\r\n");
_LIT(KSpaces, "                                                                      ");
_LIT(KPeriod, ".");
_LIT(KSingleString, "%S\r\n");
//_LIT(KDoubleString, "%s <%s>\r\n");
//_LIT(KLinefeed, "\r\n");

void PrintTestData (const TDesC& aTitle , const TDesC16& aData)
	{

	TInt i;
	TInt j;
	TInt end;

	TInt length = aData.Length();

	TBuf<80> buffer;

	buffer.Zero();
	buffer.Append(aTitle);
	buffer.Append(KLength);

	TBuf<256> buf;
	buf.AppendFormat(buffer, length);
	TESTPRINT(buf);

	for (i = 0 ; i < length ; i += 8)
		{
		buffer.Zero();
		buffer.AppendNumFixedWidth(i, EHex, 8);
		buffer += KAddressMarker;

		end = ((length-i) >= 8) ? i+8 : length;

		for (j = i ; j < end ; ++j)
			{
			buffer.AppendNumFixedWidth(aData[j], EHex, 4);
			buffer += KSpace;
			}
		buffer += TPtrC(KSpaces().Ptr(), ((8-(j-i))*5)+4);

		for (j = i ; j < end ; ++j)
			{
			if (aData[j] >= 32)
				{
				buffer.Append(aData[j]);
				}
			else
				{
				buffer += KPeriod;
				}
			}
		buffer.ZeroTerminate();
		buf.Zero();
		buf.AppendFormat(KSingleString,buffer.Ptr());
		TESTPRINT(buf);		
		}

	}

void PrintTestData(const TDesC& aTitle, const TText16* aDataBuffer, const TInt aSize)
	{
	PrintTestData(aTitle, TPtrC16(aDataBuffer, aSize));
	}

class CTestInlineTextSource : public CBase, public MTmInlineTextSource
	{
public:
	static CTestInlineTextSource* NewL(TPtrC aText, TInt aNumChar);
	~CTestInlineTextSource();

public: // From MTmInlineTextSource

	/**
	Reports the next position into which inline text should be inserted
	@param aFrom
		The document position and character edge to start from.
	@param aMaxLength
		The maximum length within which to report inline text.
		It means that inline text at position X should be reported if
		aFrom <= X && X < aFrom + aMaxLength.
		Also report trailing inline text at position aFrom + aMaxLength
		because it is really attached to the preceding character.
		Always report only the first inline text position >= aFrom.
	@param aNext
		On exit the position of the next bit of inline text to be inserted.
		N.B. The position of trailing text following position N and the
		position of leading text preceding position N+1 are both
		considered to be N+1 - and the trailing text comes first so if
		aFrom specifies a leading edge do not report trailing edge
		inline text unless its position is greater than aFrom.
		A panic EBadReturnValue will result otherwise.
	@return
		KErrNone if a position is found within the specified range,
		KErrNotFound otherwise.
	@post
		if KErrNone returned then aFrom <= aNext
		&& GetInlineText(aNext).Length() != 0
		&& (GetInlineText(X).Length() == 0 for all
		TTmDocPos X such that aFrom < X && X < aNext)
		else if KErrNotFound returned
		GetInlineText(X).Length() == 0 for all
		TTmDocPos X such that aFrom <= X && X < aFrom + aMaxLength
	*/
	virtual TInt GetNextInlineTextPosition(const TTmDocPos& aFrom, TInt aMaxLength, TTmDocPos& aNext);

	/**
	Gets a view of the text to be inserted at aAt.
	@param aAt
		Document position, including character edge, being queried.
	@return
		Any inline text that should be attached to the specified character edge at aAt.
	*/
	virtual TPtrC GetInlineText(const TTmDocPos& aAt);

private:
	CTestInlineTextSource();
	void Construct(TPtrC aText, TInt aNumChar);
	TPtrC iNullText;
	TPtrC iText;
	TInt iNumChar;
	};

CTestInlineTextSource::CTestInlineTextSource()
	{
	}

CTestInlineTextSource* CTestInlineTextSource::NewL(TPtrC aText, TInt aNumChar)
	{
	CTestInlineTextSource* self= new(ELeave) CTestInlineTextSource();
	self->Construct(aText, aNumChar);
	return self;
	}

CTestInlineTextSource::~CTestInlineTextSource()
	{
	}

void CTestInlineTextSource::Construct(TPtrC aText, TInt aNumChar)
	{
	iText.Set(aText);
	iNumChar = aNumChar;
	}

// This takes two inputs, each of which is either a real position,
// greater than or equal to 0, or KErrNotFound. If both are real
// positions then it should return the lower. If one is a real position
// then it should be returned. If neither are a real position then
// KErrNone is returned. It is all done with pointers to TInts so that
// when I get the result I can tell which one it is.
TInt* CustomMin(TInt* aPtrPos1, TInt* aPtrPos2)
	{
	if (*aPtrPos2 == KErrNotFound)
		return aPtrPos1;
	if (*aPtrPos1 == KErrNotFound)
		return aPtrPos2;
	if (*aPtrPos1 <= *aPtrPos2)
		return aPtrPos1;
	else
		return aPtrPos2;
	}

TInt CTestInlineTextSource::GetNextInlineTextPosition(const TTmDocPos& aFrom, TInt aMaxLength, TTmDocPos& aNext)
	{
	if (iNumChar == 0)
		return KErrNotFound;
	_LIT(KSS1, "A");
	_LIT(KSS2, "B");
    _LIT(KSS3, "C");
    _LIT(KSS4, "\x05c0");//Hebrew character to test right-to-left text
	TInt from = aFrom.iPos;
	TInt from2 = from;
	if ((from > 0) && (!aFrom.iLeadingEdge))
		from2--; // adjustment takes care of not returning trailing edge if at start pos
	TInt pos1 = iText.Mid(from).Find(KSS1);
	TInt pos2 = iText.Mid(from2).Find(KSS2);
	TInt pos3 = iText.Mid(from).Find(KSS3);
    TInt pos4 = iText.Mid(from2).Find(KSS3);
    TInt pos5 = iText.Mid(from2).Find(KSS4);
	if (pos1 >= 0 ) // adjustments so we compare like for like
		pos1 += from - from2;
	if (pos3 >= 0 )
		pos3 += from - from2;
	// get the smallest real position, if any
	TInt* ptrPos = CustomMin(CustomMin(&pos1, &pos3), CustomMin(CustomMin(&pos2, &pos4),&pos5));
	if (pos1 > 0 ) // now adjust back
		pos1 -= from - from2;
	if (pos3 > 0 )
		pos3 -= from - from2;
	if (*ptrPos == KErrNotFound)
		return KErrNotFound;
	if ((ptrPos == &pos1) || (ptrPos == &pos3))
		{ // it was an A or C with leading text
		aNext.iPos = *ptrPos + from;
		aNext.iLeadingEdge = ETrue;
		}
	else if ((ptrPos == &pos2) || (ptrPos == &pos4))
		{ // it was an B oD with trailing text
		aNext.iPos = *ptrPos + from2 + 1;
		aNext.iLeadingEdge = EFalse;
		}
    else if (ptrPos == &pos5)
        { // it was an Hebrew char with trailing text
        aNext.iPos = *ptrPos + from2 + 1;
        aNext.iLeadingEdge = EFalse;
        }
	else
		{
		// something has gone horribly wrong
		return KErrNotFound;
		}
	if (aNext.iPos - aFrom.iPos > aMaxLength + (aNext.iLeadingEdge ? 0 : 1))
		return KErrNotFound;
	return KErrNone;
	}

TPtrC CTestInlineTextSource::GetInlineText(const TTmDocPos& aAt)
	{
	if (iNumChar == 0)
		return iNullText;
	if ((aAt.iPos == 0) && (!aAt.iLeadingEdge))
		return iNullText;
	_LIT(KSS1, "A");
	_LIT(KSS2, "B");
	_LIT(KSS3, "C");
    _LIT(KSS4, "\x05c0");//Hebrew character to test right-to-left text
	TInt at = aAt.iPos;
	TInt at2 = at;
	if ((at2 > 0) && (!aAt.iLeadingEdge))
		at2--; // adjustment takes care of not returning trailing edge if at start pos

	TInt pos1 = iText.Mid(at).Find(KSS1);
	TInt pos2 = iText.Mid(at2).Find(KSS2);
	TInt pos3 = iText.Mid(at).Find(KSS3);
	TInt pos4 = iText.Mid(at2).Find(KSS3);
    TInt pos5 = iText.Mid(at2).Find(KSS4);
	if (pos1 == 0)
		{
		pos1 += at;
		if ((pos1 == aAt.iPos) && aAt.iLeadingEdge)
			{
			TPtrC tPtrC;
			if (iNumChar == 1)
				{
				_LIT(KIS11, "a");
				tPtrC.Set(KIS11);
				}
			else if (iNumChar == 2)
				{
				_LIT(KIS12, "aa");
				tPtrC.Set(KIS12);
				}
			return tPtrC;
			}
		}
	if (pos2 == 0)
		{
		pos2 += at2;
		if ((pos2 + 1 == aAt.iPos) && !aAt.iLeadingEdge)
			{
			TPtrC tPtrC;
			if (iNumChar == 1)
				{
				_LIT(KIS21, "b");
				tPtrC.Set(KIS21);
				}
			else if (iNumChar == 2)
				{
				_LIT(KIS22, "bb");
				tPtrC.Set(KIS22);
				}
			return tPtrC;
			}
		}
	if ((pos3 == 0) || (pos4 == 0))
		{
		if (((pos3 + at == aAt.iPos) && aAt.iLeadingEdge) || ((pos4 + at2 + 1 == aAt.iPos) && !aAt.iLeadingEdge))
			{
			TPtrC tPtrC;
			if (iNumChar == 1)
				{
				_LIT(KIS31, "c");
				tPtrC.Set(KIS31);
				}
			else if (iNumChar == 2)
				{
				_LIT(KIS32, "cc");
				tPtrC.Set(KIS32);
				}
			return tPtrC;
			}
		}
    if (pos5 == 0)
        {
        pos5 += at2;
        if ((pos5 + 1 == aAt.iPos) && !aAt.iLeadingEdge)
            {
            TPtrC tPtrC;
            if (iNumChar == 1)
                {
                _LIT(KIS41, "\05be");
                tPtrC.Set(KIS41);
                }
            else if (iNumChar == 2)
                {
                _LIT(KIS42, "\05be\05be");
                tPtrC.Set(KIS42);
                }
            return tPtrC;
            }
        }
	return iNullText;
	}

class CTestFormExtendedInterfaceProvider: public CBase, public MFormCustomInterfaceProvider
	{
public:
	static CTestFormExtendedInterfaceProvider* NewL(TPtrC aText, TInt aNumChar);
	~CTestFormExtendedInterfaceProvider();
	TAny* GetExtendedInterface(const TUid& aInterfaceId);

private:
	CTestFormExtendedInterfaceProvider();
	void ConstructL(TPtrC aText, TInt aNumChar);

private:
	CTestInlineTextSource* iTestInlineTextSource; // Owned
	};


CTestFormExtendedInterfaceProvider::CTestFormExtendedInterfaceProvider()
	{
	}

CTestFormExtendedInterfaceProvider* CTestFormExtendedInterfaceProvider::NewL(TPtrC aText, TInt aNumChar)
	{
	CTestFormExtendedInterfaceProvider* self= new(ELeave) CTestFormExtendedInterfaceProvider();
	CleanupStack::PushL(self);
	self->ConstructL(aText, aNumChar);
	CleanupStack::Pop(); // self
	return self;
	}

CTestFormExtendedInterfaceProvider::~CTestFormExtendedInterfaceProvider()
	{
	delete iTestInlineTextSource;
	}

void CTestFormExtendedInterfaceProvider::ConstructL(TPtrC aText, TInt aNumChar)
	{
	iTestInlineTextSource = CTestInlineTextSource::NewL(aText, aNumChar);
	}

TAny* CTestFormExtendedInterfaceProvider::GetExtendedInterface( const TUid& aInterfaceId )
	{
	if (aInterfaceId == KInlineTextApiExtensionUid)
		{
		return static_cast<MTmInlineTextSource*>(iTestInlineTextSource);
		}
	else
		{
		return NULL;
		}
	}

// When referring to test cases the following shorthand is used:
// P  Primary text that has inline text (leading and/or trailing) attached
//    to it
// L  Leading inline text attached to primary text
// T  Trailing inline text attached to primary text
// X  Secondary text that precedes primary text
// Y  Secondary text that follows primary text
// S  A special case of P where the primary text is a single character
//    which has both leading and trailing inline text attached to it
//
// So, for example, X L-P will indicate some normal secondary text,
// followed by a <space>, followed by some primary text which will get
// leading text added to the start of it.
//
// When defining a test string the following are significant characters:
// A  An 'A' will get zero or more 'a's of leading text attached to it
// B  A 'B' will get zero or more 'b's of trailing text attached to it
// C  A 'C' will get zero or more 'c's of both leading and trailing text
//    attached to it
//
// All other characters are used normally.
// N.B. Spaces have significance as potential line-breaking points.
//
// So the example X L-P could be implemented as "ij Axyz". The 'A'
// indicating that leading inline text should be attatched to it.
//
// All test are carried out 3 times, once with inline text set to add zero
// characters for each inline text (i.e. switched off), once with inline
// text set to add one character for each inline text and once with inline
// text set to add two characters for each inline text.
//
// All tests use a special test graphics device and test graphics context
// to enable us to catch the output and use a special "pseudo" test font
// where all characters are exactly 10 pixels wide and 10 pixels high (to
// make the calculations easier).
//
// General combination tests
//
// These are simple tests, which would take a single line of input on the
// display and produce a single line of output on the display (if we
// weren't using a special CTestGraphicsContext to catch the output).
// These tests check that, without the complications of line-breaking,
// inline text is inserted where we expect it to be. This is why a display
// that is capable of taking 20 chars on a line is used for these tests.
//
// Future extensions:
// These tests could be extended to do similar tests using Right-to-Left
// (RtoL) text. This would require the inline text mechanism to be
// extended to also use RtoL trigger characters (and inserts). The
// expected results would need to take account of the fact that the output
// would be in display order rather than buffer order.
//
// Specific tests
//
// These tests check specific cases that involve line-breaking. They
// produce more than one line of output, apart from some (not all) cases
// when inline text is switched off. To force the line-breaking these
// tests use a display that is obly capable of taking 10 chars on a line
// is used. Detailed info for each test is given in comments in the test
// data.
//
// Future extensions:
// These tests could be extended to check the behaviour of truncation and
// the insertion of an ellipsis. These tests could also be extended to
// include some RtoL examples.

static const TInt KTestCases = 18;
static const TInt KInlineTextOptions = 3;
static const TInt KVariants = 6;

static const TPtrC KTestStrings[KTestCases][KInlineTextOptions][KVariants] =
	{	// Start of General Combination tests
		{	// Test L-P
			{
			_S("Axyz"),
			_S("Axyz"),
			_S(""),
			_S(""),
			_S(""),
			_S("")
			},
			{
			_S("Axyz"),
			_S("a"),
			_S("Axyz"),
			_S(""),
			_S(""),
			_S("")
			},
			{
			_S("Axyz"),
			_S("aa"),
			_S("Axyz"),
			_S(""),
			_S(""),
			_S("")
			}
		},
		{	// Test X L-P
			{
			_S("ij Axyz"),
			_S("ij Axyz"),
			_S(""),
			_S(""),
			_S(""),
			_S("")
			},
			{
			_S("ij Axyz"),
			_S("ij "),
			_S("a"),
			_S("Axyz"),
			_S(""),
			_S("")
			},
			{
			_S("ij Axyz"),
			_S("ij "),
			_S("aa"),
			_S("Axyz"),
			_S(""),
			_S("")
			}
		},
		{	// Test P-T
			{
			_S("xyzB"),
			_S("xyzB"),
			_S(""),
			_S(""),
			_S(""),
			_S("")
			},
			{
			_S("xyzB"),
			_S("xyzB"),
			_S("b"),
			_S(""),
			_S(""),
			_S("")
			},
			{
			_S("xyzB"),
			_S("xyzB"),
			_S("bb"),
			_S(""),
			_S(""),
			_S("")
			}
		},
		{	// Test P-T Y
			{
			_S("xyzB pq"),
			_S("xyzB pq"),
			_S(""),
			_S(""),
			_S(""),
			_S("")
			},
			{
			_S("xyzB pq"),
			_S("xyzB"),
			_S("b"),
			_S(" pq"),
			_S(""),
			_S("")
			},
			{
			_S("xyzB pq"),
			_S("xyzB"),
			_S("bb"),
			_S(" pq"),
			_S(""),
			_S("")
			}
		},
		{	// Test L-P-T
			{
			_S("AxyzB"),
			_S("AxyzB"),
			_S(""),
			_S(""),
			_S(""),
			_S("")
			},
			{
			_S("AxyzB"),
			_S("a"),
			_S("AxyzB"),
			_S("b"),
			_S(""),
			_S("")
			},
			{
			_S("AxyzB"),
			_S("aa"),
			_S("AxyzB"),
			_S("bb"),
			_S(""),
			_S("")
			}
		},
		{	// Test L-C-T
			{
			_S("C"),
			_S("C"),
			_S(""),
			_S(""),
			_S(""),
			_S("")
			},
			{
			_S("C"),
			_S("c"),
			_S("C"),
			_S("c"),
			_S(""),
			_S("")
			},
			{
			_S("C"),
			_S("cc"),
			_S("C"),
			_S("cc"),
			_S(""),
			_S("")
			}
		},
		{	// Test X L-P-T
			{
			_S("ij AxyzB"),
			_S("ij AxyzB"),
			_S(""),
			_S(""),
			_S(""),
			_S("")
			},
			{
			_S("ij AxyzB"),
			_S("ij "),
			_S("a"),
			_S("AxyzB"),
			_S("b"),
			_S("")
			},
			{
			_S("ij AxyzB"),
			_S("ij "),
			_S("aa"),
			_S("AxyzB"),
			_S("bb"),
			_S("")
			}
		},
		{	// Test X L-C-T
			{
			_S("ij C"),
			_S("ij C"),
			_S(""),
			_S(""),
			_S(""),
			_S("")
			},
			{
			_S("ij C"),
			_S("ij "),
			_S("c"),
			_S("C"),
			_S("c"),
			_S("")
			},
			{
			_S("ij C"),
			_S("ij "),
			_S("cc"),
			_S("C"),
			_S("cc"),
			_S("")
			}
		},
		{	// Test L-P-T Y
			{
			_S("AxyzB pq"),
			_S("AxyzB pq"),
			_S(""),
			_S(""),
			_S(""),
			_S("")
			},
			{
			_S("AxyzB pq"),
			_S("a"),
			_S("AxyzB"),
			_S("b"),
			_S(" pq"),
			_S("")
			},
			{
			_S("AxyzB pq"),
			_S("aa"),
			_S("AxyzB"),
			_S("bb"),
			_S(" pq"),
			_S("")
			}
		},
		{	// Test L-C-T Y
			{
			_S("C pq"),
			_S("C pq"),
			_S(""),
			_S(""),
			_S(""),
			_S("")
			},
			{
			_S("C pq"),
			_S("c"),
			_S("C"),
			_S("c"),
			_S(" pq"),
			_S("")
			},
			{
			_S("C pq"),
			_S("cc"),
			_S("C"),
			_S("cc"),
			_S(" pq"),
			_S("")
			}
		},
		{	// Test X L-P-T Y
			{
			_S("ij AxyzB pq"),
			_S("ij AxyzB pq"),
			_S(""),
			_S(""),
			_S(""),
			_S("")
			},
			{
			_S("ij AxyzB pq"),
			_S("ij "),
			_S("a"),
			_S("AxyzB"),
			_S("b"),
			_S(" pq")
			},
			{
			_S("ij AxyzB pq"),
			_S("ij "),
			_S("aa"),
			_S("AxyzB"),
			_S("bb"),
			_S(" pq")
			}
		},
		{	// Test X L-C-T Y
			{
			_S("ij C pq"),
			_S("ij C pq"),
			_S(""),
			_S(""),
			_S(""),
			_S("")
			},
			{
			_S("ij C pq"),
			_S("ij "),
			_S("c"),
			_S("C"),
			_S("c"),
			_S(" pq")
			},
			{
			_S("ij C pq"),
			_S("ij "),
			_S("cc"),
			_S("C"),
			_S("cc"),
			_S(" pq")
			}
		},	// End of General Combination tests
		{	// Start of Specific tests
			{	// Test P-T Y
				// primary text with trailing inline text, secondary text secondary
				// text won't fit on line
			_S("wxyzB pqrstuv"),
			_S("wxyzB"),
			_S(" "),
			_S("pqrstuv"),
			_S(""),
			_S("")
			},
			{
			_S("wxyzB pqrstuv"),
			_S("wxyzB"),
			_S("b"),
			_S(" "),
			_S("pqrstuv"),
			_S("")
			},
			{
			_S("wxyzB pqrstuv"),
			_S("wxyzB"),
			_S("bb"),
			_S(" "),
			_S("pqrstuv"),
			_S("")
			}
		},
		{	// Test X P-T one
			// secondary text, primary text with trailing inline text trailing
			// text won't fit and there is no line breaking point inside primary
			// text and the primary text plus trailing text will fit on next line
			{
			_S("ijklm xyzB"),
			_S("ijklm xyzB"),
			_S(""),
			_S(""),
			_S(""),
			_S("")
			},
			{
			_S("ijklm xyzB"),
			_S("ijklm"),
			_S(" "),
			_S("xyzB"),
			_S("b"),
			_S("")
			},
			{
			_S("ijklm xyzB"),
			_S("ijklm"),
			_S(" "),
			_S("xyzB"),
			_S("bb"),
			_S("")
			}
		},
		{	// Test X P-T two
			// secondary text, primary text with trailing inline text primary
			// text won't fit and there is no line breaking point inside primary
			// text and the primary text plus trailing text will fit on next line
			{
			_S("ijklm uvwxyzB"),
			_S("ijklm"),
			_S(" "),
			_S("uvwxyzB"),
			_S(""),
			_S("")
			},
			{
			_S("ijklm uvwxyzB"),
			_S("ijklm"),
			_S(" "),
			_S("uvwxyzB"),
			_S("b"),
			_S("")
			},
			{
			_S("ijklm uvwxyzB"),
			_S("ijklm"),
			_S(" "),
			_S("uvwxyzB"),
			_S("bb"),
			_S("")
			}
		},
		{	// Test X L-P-T one
			// secondary text, primary text with leading and trailing inline text trailing
			// text won't fit and there is no line breaking point inside primary text and
			// the leading text plus primary text plus trailing text will fit on the next line
			{
			_S("ij AwxyzB"),
			_S("ij AwxyzB"),
			_S(""),
			_S(""),
			_S(""),
			_S("")
			},
			{
			_S("ij AwxyzB"),
			_S("ij"),
			_S(" "),
			_S("a"),
			_S("AwxyzB"),
			_S("b")
			},
			{
			_S("ij AwxyzB"),
			_S("ij"),
			_S(" "),
			_S("aa"),
			_S("AwxyzB"),
			_S("bb")
			}
		},
		{	// Test X L-P-T two
			// secondary text, primary text with leading and trailing inline text primary
			// text won't fit and there is no line breaking point inside primary text and
			// the leading text plus primary text plus trailing text will fit on the next line
			{
			_S("ijkl AxyzB"),
			_S("ijkl AxyzB"),
			_S(""),
			_S(""),
			_S(""),
			_S("")
			},
			{
			_S("ijkl AxyzB"),
			_S("ijkl"),
			_S(" "),
			_S("a"),
			_S("AxyzB"),
			_S("b")
			},
			{
			_S("ijkl AxyzB"),
			_S("ijkl"),
			_S(" "),
			_S("aa"),
			_S("AxyzB"),
			_S("bb"),
			}
		},
		{	// Test X L-P-T three
			//secondary text, primary text with leading and trailing inline text leading
			// text won't fit and the leading text plus primary text plus trailing text
			// will fit on the next line
			{
			_S("ijklmnop AxyzB"),
			_S("ijklmnop"),
			_S(" "),
			_S("AxyzB"),
			_S(""),
			_S("")
			},
			{
			_S("ijklmnop AxyzB"),
			_S("ijklmnop"),
			_S(" "),
			_S("a"),
			_S("AxyzB"),
			_S("b")
			},
			{
			_S("ijklmnop AxyzB"),
			_S("ijklmnop"),
			_S(" "),
			_S("aa"),
			_S("AxyzB"),
			_S("bb")
			}
		}	// End of Specific tests
	};

static const TInt KTestCount[KTestCases][KInlineTextOptions] =
	{
		{	// Start of General Combination tests
		1,	// Test L-P
		2,
		2
		},
		{
		1,	// Test X L-P
		3,
		3
		},
		{
		1,	// Test P-T
		2,
		2
		},
		{
		1,	// Test P-T Y
		3,
		3
		},
		{
		1,	// Test L-P-T
		3,
		3
		},
		{
		1,	// Test L-C-T
		3,
		3
		},
		{
		1,	// Test X L-P-T
		4,
		4
		},
		{
		1,	// Test X L-C-T
		4,
		4
		},
		{
		1,	// Test L-P-T Y
		4,
		4
		},
		{
		1,	// Test L-C-T Y
		4,
		4
		},
		{
		1,	// Test X L-P-T Y
		5,
		5
		},
		{
		1,	// Test X L-C-T Y
		5,
		5
		},	// End of General Combination tests
		{	// Start of Specific tests
		3,	// Test P-T B
		4,
		4
		},
		{
		1,	// Test X P-T one
		4,
		4
		},
		{
		3,	// Test X P-T two
		4,
		4
		},
		{
		1,	// Test X L-P-T one
		5,
		5
		},
		{
		1,	// Test X L-P-T two
		5,
		5
		},
		{
		3,	// Test X L-P-T three
		5,
		5
		}	// End of Specific tests
	};

void DoLineTestL(TDes& aText, CTextLayout* aLayout, CTestGraphicsDevice* aDevice, CTextView* aView, TInt aNumChar, TInt aIndex)
	{
	aText = KTestStrings[aIndex][aNumChar][0];
	CTestFormExtendedInterfaceProvider* interfaceProvider = CTestFormExtendedInterfaceProvider::NewL(aText, aNumChar); // owned here
	aLayout->SetInterfaceProvider(interfaceProvider);
	CleanupStack::PushL(interfaceProvider);
	aDevice->LineArray().ResetLineArray();
	aView->HandleGlobalChangeL();
	// now that we've redrawn the actual ooutput (what went through DrawText(), including
	// any inline text) has been stored in the LineArray. So we#'ll have a look and see
	// if it contains what we expect it to.
	// Each line may produce one or more LineArray() entries as there is one created for
	// each call to DrawText()
	const TTestGCDisplayLine* line1 = 0;
	const TTestGCDisplayLine* line2 = 0;
	// Find out how many LineArray entries are expected for the output of this test
	TInt count = KTestCount[aIndex][aNumChar];
	// And test them (in reverse order, because it's easier)
	switch (count)
		{
	case 5:
		line1 = &(aDevice->LineArray().Line(4));
		line2 = aDevice->LineArray().Find(KTestStrings[aIndex][aNumChar][5]);
		TESTPOINT(0 != line1);
		TESTPOINT(0 != line2);
		TESTPOINT(line1->iLineData.Compare(line2->iLineData) == 0);
	case 4:
		line1 = &(aDevice->LineArray().Line(3));
		line2 = aDevice->LineArray().Find(KTestStrings[aIndex][aNumChar][4]);
		TESTPOINT(0 != line1);
		TESTPOINT(0 != line2);
		TESTPOINT(line1->iLineData.Compare(line2->iLineData) == 0);
	case 3:
		line1 = &(aDevice->LineArray().Line(2));
		line2 = aDevice->LineArray().Find(KTestStrings[aIndex][aNumChar][3]);
		TESTPOINT(0 != line1);
		TESTPOINT(0 != line2);
		TESTPOINT(line1->iLineData.Compare(line2->iLineData) == 0);
	case 2:
		line1 = &(aDevice->LineArray().Line(1));
		line2 = aDevice->LineArray().Find(KTestStrings[aIndex][aNumChar][2]);
		TESTPOINT(0 != line1);
		TESTPOINT(0 != line2);
		TESTPOINT(line1->iLineData.Compare(line2->iLineData) == 0);
	case 1:
		line1 = &(aDevice->LineArray().Line(0));
		line2 = aDevice->LineArray().Find(KTestStrings[aIndex][aNumChar][1]);
		TESTPOINT(0 != line1);
		TESTPOINT(0 != line2);
		// Can't always do a direct comparison of lines because same string
		// may appear in more than one line, so compare contents
		TESTPOINT(line1->iLineData.Compare(line2->iLineData) == 0);
		}
	aLayout->SetInterfaceProvider(NULL);
	CleanupStack::PopAndDestroy(interfaceProvider);
	}

void GeneralCombinationTestsTextViewL(TDes& aText, CTextLayout* aLayout, CTestGraphicsDevice* aDevice, CTextView* aView, TInt aNumChar)
	{
	// For all tests carried out from here up to 20 chars will fit on a line
	TESTPRINT(_L("Test L-P"));
	DoLineTestL(aText, aLayout, aDevice, aView, aNumChar, 0);
	TESTPRINT(_L("Test X L-P"));
	DoLineTestL(aText, aLayout, aDevice, aView, aNumChar, 1);
	TESTPRINT(_L("Test P-T"));
	DoLineTestL(aText, aLayout, aDevice, aView, aNumChar, 2);
	TESTPRINT(_L("Test P-T Y"));
	DoLineTestL(aText, aLayout, aDevice, aView, aNumChar, 3);
	TESTPRINT(_L("Test L-P-T"));
	DoLineTestL(aText, aLayout, aDevice, aView, aNumChar, 4);
	TESTPRINT(_L("Test L-C-T"));
	DoLineTestL(aText, aLayout, aDevice, aView, aNumChar, 5);
	TESTPRINT(_L("Test X L-P-T"));
	DoLineTestL(aText, aLayout, aDevice, aView, aNumChar, 6);
	TESTPRINT(_L("Test X L-C-T"));
	DoLineTestL(aText, aLayout, aDevice, aView, aNumChar, 7);
	TESTPRINT(_L("Test L-P-T Y"));
	DoLineTestL(aText, aLayout, aDevice, aView, aNumChar, 8);
	TESTPRINT(_L("Test L-C-T Y"));
	DoLineTestL(aText, aLayout, aDevice, aView, aNumChar, 9);
	TESTPRINT(_L("Test X L-P-T Y"));
	DoLineTestL(aText, aLayout, aDevice, aView, aNumChar, 10);
	TESTPRINT(_L("Test X L-C-T Y"));
	DoLineTestL(aText, aLayout, aDevice, aView, aNumChar, 11);
	}

void SpecificTestsTextViewL(TDes& aText, CTextLayout* aLayout, CTestGraphicsDevice* aDevice, CTextView* aView, TInt aNumChar)
	{
	// For all tests carried out from here up to 10 chars will fit on a line
    TESTPRINT(_L("Test P-T B"));
	DoLineTestL(aText, aLayout, aDevice, aView, aNumChar, 12);
	TESTPRINT(_L("Test X P-T one"));
	DoLineTestL(aText, aLayout, aDevice, aView, aNumChar, 13);
	TESTPRINT(_L("Test X P-T two"));
	DoLineTestL(aText, aLayout, aDevice, aView, aNumChar, 14);
	TESTPRINT(_L("Test X L-P-T one"));
	DoLineTestL(aText, aLayout, aDevice, aView, aNumChar, 15);
	TESTPRINT(_L("Test X L-P-T two"));
	DoLineTestL(aText, aLayout, aDevice, aView, aNumChar, 16);
	TESTPRINT(_L("Test X L-P-T three"));
	DoLineTestL(aText, aLayout, aDevice, aView, aNumChar, 17);
	}

void DoLineTestForINC141914L(TDes& aText, CTextLayout* aLayout, CTestGraphicsDevice* aDevice, CTextView* aView)
    {
    /*
     * This case is to test whether or not inline text will find the correct format after changing the text content.
     * In bytecode, there is one value to record the postion(iInlineTextFormat) to which inline text chunk attaches,
     * INC141914 is raised because this value is not updated correctly after text change.
     * The text string is as the following 3 lines:
     * 111111;
     * 222222;
     * wxyBbz
     * Where the 'b' is the inline text.
     * 
     * 1st step:
     * delete 5 leading '1's in the first line,
     * 2nd step:
     * delete 5 leading '2's in the second line.
     * after the 2 steps, inline text is still able to use iInlineTextFormat to find its attached text string
    */
    aText = _S("\x31\x31\x31\x31\x31\x31\x3B\x2029\x32\x32\x32\x32\x32\x32\x3B\x2029\x77\x78\x79\x42\x7A");
    CTestFormExtendedInterfaceProvider* interfaceProvider = CTestFormExtendedInterfaceProvider::NewL(aText, 1); // owned here
    aLayout->SetInterfaceProvider(interfaceProvider);
    CleanupStack::PushL(interfaceProvider);
    aDevice->LineArray().ResetLineArray();
    aView->HandleGlobalChangeL();
    aText.Delete(0,5);
    aView->HandleInsertDeleteL(TCursorSelection(0,0),5);
    aText.Delete(3,5);
    aView->HandleInsertDeleteL(TCursorSelection(3,3),5);
 
    aLayout->SetInterfaceProvider(NULL);
    CleanupStack::PopAndDestroy(interfaceProvider);
    }

void DoLineTestForINC143086L(TDes& aText, CTextLayout* aLayout, CTestGraphicsDevice* aDevice, CTextView* aView)
    {
    /*
     * This case is to test inline text behaviour for right-to-left text 
     * The text string is as the following characters:
     * \x5e0\x5e1\x5e2\x5e3\x5e4\x5c0\x5e5;
     * Where the '\x5c0' is the inline text.
     */
    aText = _S("\x5e0\x5e1\x5e2\x5e3\x5e4\x5c0\x5e5");
    CTestFormExtendedInterfaceProvider* interfaceProvider = CTestFormExtendedInterfaceProvider::NewL(aText, 1); // owned here
    aLayout->SetInterfaceProvider(interfaceProvider);
    CleanupStack::PushL(interfaceProvider);
    aDevice->LineArray().ResetLineArray();
    aView->HandleGlobalChangeL();
 
    aLayout->SetInterfaceProvider(NULL);
    CleanupStack::PopAndDestroy(interfaceProvider);
    }

// aNumChar determines what kind of inline text is in force
// 0 means no inline text, 1 means insert a single char for
// each possible inline text and 2 means insert two chars
void RunGeneralCombinationTestsL(TInt aNumChar)
	{
	CActiveScheduler* scheduler = new(ELeave) CActiveScheduler;
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler);
	TBuf<100> text;
	TDocModel docModel(text);
	// This time make it capable of 20 chars + a couple of spare pixels
	// so all of these tests should result in a single line of output
	TRect displayRect(0, 0, KDisplayWidthWide, KDisplayHeight);
	CTextLayout* layout = CTextLayout::NewL(&docModel, displayRect.Width());
	CleanupStack::PushL(layout);
	CTestGraphicsDevice* device = CTestGraphicsDevice::NewL(displayRect.Size(), 0);
	CleanupStack::PushL(device);
	CTextView* view = CTextView::NewL(layout, displayRect, device, device, 0, 0, 0);
	CleanupStack::PushL(view);
	// This is used to force the use of CTestGraphicsContext instead of a normal one
	CWindowGc* offScreenContext;
	User::LeaveIfError(device->CreateContext(offScreenContext));
	CleanupStack::PushL(offScreenContext);
	CTestTextView::SetContextForFlickerFreeRedraw(view, offScreenContext);

	GeneralCombinationTestsTextViewL(text, layout, device, view, aNumChar);

	CleanupStack::PopAndDestroy(offScreenContext);
	CleanupStack::PopAndDestroy(view);
	CleanupStack::PopAndDestroy(device);
	CleanupStack::PopAndDestroy(layout);
	CleanupStack::PopAndDestroy(scheduler);
	}

// aNumChar determines what kind of inline text is in force
// 0 means no inline text, 1 means insert a single char for
// each possible inline text and 2 means insert two chars
void RunSpecificTestsL( TInt aNumChar)
	{
	// Note: If you need to move these heap checks any further "in" to focus
	// on a specific test then you will have to move all the setup code in as
	// well - and still preserve the two different display widths in use
	__UHEAP_MARK;
	CActiveScheduler* scheduler = new(ELeave) CActiveScheduler;
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler);
	TBuf<100> text;
	TDocModel docModel(text);
	// This time make it capable of 10 chars + a couple of spare pixels
	// so that line wrapping will occur
	TRect displayRect(0, 0, KDisplayWidthThin, KDisplayHeight);
	CTextLayout* layout = CTextLayout::NewL(&docModel, displayRect.Width());
	CleanupStack::PushL(layout);
	CTestGraphicsDevice* device = CTestGraphicsDevice::NewL(displayRect.Size(), 0);
	CleanupStack::PushL(device);
	CTextView* view = CTextView::NewL(layout, displayRect, device, device, 0, 0, 0);
	CleanupStack::PushL(view);
	// This is used to force the use of CTestGraphicsContext instead of a normal one
	CWindowGc* offScreenContext;
	User::LeaveIfError(device->CreateContext(offScreenContext));
	CleanupStack::PushL(offScreenContext);
	CTestTextView::SetContextForFlickerFreeRedraw(view, offScreenContext);

	SpecificTestsTextViewL(text, layout, device, view, aNumChar);

	CleanupStack::PopAndDestroy(offScreenContext);
	CleanupStack::PopAndDestroy(view);
	CleanupStack::PopAndDestroy(device);
	CleanupStack::PopAndDestroy(layout);
	CleanupStack::PopAndDestroy(scheduler);
	__UHEAP_MARKEND;
	}


void RunTestsForINC141914L()
    {
    __UHEAP_MARK;
    CActiveScheduler* scheduler = new(ELeave) CActiveScheduler;
    CleanupStack::PushL(scheduler);
    CActiveScheduler::Install(scheduler);
    TBuf<100> text;
    TDocModel docModel(text);
    // This time make it capable of 10 chars + a couple of spare pixels
    // so that line wrapping will occur
    TRect displayRect(0, 0, KDisplayWidthWide, KDisplayHeight);
    CTextLayout* layout = CTextLayout::NewL(&docModel, displayRect.Width());
    CleanupStack::PushL(layout);
    CTestGraphicsDevice* device = CTestGraphicsDevice::NewL(displayRect.Size(), 0);
    CleanupStack::PushL(device);
    CTextView* view = CTextView::NewL(layout, displayRect, device, device, 0, 0, 0);
    CleanupStack::PushL(view);
    // This is used to force the use of CTestGraphicsContext instead of a normal one 
    CWindowGc* offScreenContext;
    User::LeaveIfError(device->CreateContext(offScreenContext));
    CleanupStack::PushL(offScreenContext);
    CTestTextView::SetContextForFlickerFreeRedraw(view, offScreenContext);

    DoLineTestForINC141914L(text, layout, device, view);
    DoLineTestForINC143086L(text, layout, device, view);

    CleanupStack::PopAndDestroy(offScreenContext);
    CleanupStack::PopAndDestroy(view);
    CleanupStack::PopAndDestroy(device);
    CleanupStack::PopAndDestroy(layout);
    CleanupStack::PopAndDestroy(scheduler);
    __UHEAP_MARKEND;
    }

TVerdict CTInLineTextStep::doTestStepL()
	{
    SetTestStepResult(EPass);
    TestStep = this;
    
	__UHEAP_MARK;
	TESTPRINT(KTInlineText);
	TESTPRINT(_L(" @SYMTestCaseID:SYSLIB-FORM-LEGACY-INLINETEXT-0001 General combination tests - no inline text "));
	TInt error = RFbsSession::Connect();
	if (error == KErrNotFound)
		{
		FbsStartup();
		error = RFbsSession::Connect();
		}
	TEST(error == KErrNone);
	TRAP(error, RunGeneralCombinationTestsL(0));
	TEST(error == KErrNone);
	TESTPRINT(_L("General combination tests - single char inline text"));
	TRAP(error, RunGeneralCombinationTestsL(1));
	TEST(error == KErrNone);
	TESTPRINT(_L("General combination tests - multi char inline text"));
	TRAP(error, RunGeneralCombinationTestsL(2));
	TEST(error == KErrNone);
	TESTPRINT(_L("Specific tests - no inline text"));
	TRAP(error, RunSpecificTestsL(0));
	TEST(error == KErrNone);
	TESTPRINT(_L("Specific tests - single char inline text"));
	TRAP(error, RunSpecificTestsL(1));
	TEST(error == KErrNone);
	TESTPRINT(_L("Specific tests - multi char inline text"));
	TRAP(error, RunSpecificTestsL(2));
	TEST(error == KErrNone);
	
    TESTPRINT(_L("Defect tests - for INC141914"));
    TRAP(error, RunTestsForINC141914L());
    TEST(error == KErrNone);
    
	RFbsSession::Disconnect();
	__UHEAP_MARKEND;
	User::Heap().Check();
	return TestStepResult();
	}
