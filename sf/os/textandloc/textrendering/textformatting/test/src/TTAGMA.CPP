/*
* Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <e32std.h>
#include <e32test.h>
#include <bitstd.h>
#include <bitdev.h>
#include <fntstore.h>
#include <fbs.h>
#include <txtetext.h>
#include <txtfrmat.h>
#include <tagma.h>

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "TAGMA_INTERNAL.H"
#endif

static const TInt KFontClearlyU = 268461580;

static const TText KParBreakChar = CEditableText::EParagraphDelimiter;
static const TPtrC ParBreak(&KParBreakChar,1);
static const TText KLineBreakChar = CEditableText::ELineBreak;
static const TPtrC LineBreak(&KLineBreakChar,1);
static const TText KPopDirectionalFormatChar = 0x202C;
static const TPtrC KPopDirectionalFormat(&KPopDirectionalFormatChar,1);
static const TText KLeftToRightOverrideChar = 0x202D;
static const TPtrC KLeftToRightOverride(&KLeftToRightOverrideChar,1);
static const TText KRightToLeftOverrideChar = 0x202E;
static const TPtrC KRightToLeftOverride(&KRightToLeftOverrideChar,1);

RTest test(_L("TTAGMA.CPP"));

/*
Sample texts, using #NNNN for Unicode characters outside the ASCII range.
The ordinary C++ notation \xNNNN is not used because it is not supported properly by GCC.
*/

// The bidirectional examples from the Unicode Standard, version 2.0, page 3-21.
_LIT(Bd1,"car means #202eCAR#202c.#2029");
_LIT(Bd2,"#202e#202dcar#202c MEANS CAR.#202c#2029");
_LIT(Bd3,"he said \"#202e#202dcar#202c MEANS CAR#202c.\"#2029");
_LIT(Bd4,"#202eDID YOU SAY '#202dhe said \"#202e#202dcar#202c MEANS CAR#202c\"#202c'?#202c#2029");

// Displayed versions of the bidirectional examples.
_LIT(DisplayedBd1,"car means RAC.");
_LIT(DisplayedBd2,".RAC SNAEM car");
_LIT(DisplayedBd3,"he said \"RAC SNAEM car.\"");
_LIT(DisplayedBd4,"?'he said \"RAC SNAEM car\"' YAS UOY DID");

// Arabic examples - the Arabic is not real words, just strings of Arabic letters.
_LIT(Arabic1,"abc #fe8d#fe92#fea4#fe9e 123-456 56..78 #fec3#fec6 76.5, 3, 1,000 #fee3#fee6 def#2029");
	// number parsing, etc.
_LIT(Arabic2,"abc \"#fe8d#fe92#fea4#fe9e!\" def#2029"); // ordinary embedded Arabic
_LIT(Arabic3,"abc \"#fe8d#fe92#fea4#fe9e!#200f\" def#2029"); // use RLM (U+200F) to put ! at the end of the Arabic
_LIT(Arabic4,"#200E#0643#064B#0633#0651#064B#0631#064B means 'he smashed'.#2029");
	// base forms, with vowels and shadda; LRM at start to set predominant direction to left-to-right
_LIT(Arabic5,"#067E#067F#0680 #0686#0687#0688 #06a6#06a9#06ad#06af #06b1#06b3#06bb#06be#0698 #067e #0686#2029");
	// extended Arabic letters for Persian, Urdu, Sindhi, etc.

// Displayed versions of the Arabic examples.
_LIT(DisplayedArabic1,"abc #fee6#fee3 1,000 ,3 ,76.5 #fec6#fec3 78..56 456-123 #fe9e#fea4#fe92#fe8d def");
_LIT(DisplayedArabic2,"abc \"#fe9e#fea4#fe92#fe8d!\" def");
_LIT(DisplayedArabic3,"abc \"!#fe9e#fea4#fe92#fe8d\" def");
_LIT(DisplayedArabic4,"#feae#064B#feb4#0651#064B#fedb#064B means 'he smashed'.");
_LIT(DisplayedArabic5,"#0686 #067e #fb8b#fbad#fba3#fb99#fb9c #fb93#fbd6#fb91#fb70 #fb89#fb81#fb7c #fb5b#fb65#fb58");

// A custom layout and drawing class.
class TMyCustom: public MTmCustom
	{
	TRgb SystemColor(TUint aColorIndex,TRgb aDefaultColor) const;
	void DrawBackground(CGraphicsContext& aGc,const TPoint& aTextLayoutTopLeft,const TRect& aRect,
						const TLogicalRgb& aBackground,TRect& aRectDrawn) const;
	void DrawLineGraphics(CGraphicsContext& aGc,const TPoint& aTextLayoutTopLeft,const TRect& aRect,
						  const TTmLineInfo& aLineInfo) const;
	};

class CTestHarness: public CBase
	{
	public:
	static CTestHarness* NewL();
	~CTestHarness();
	void PerformTestsL();
	void Test(TBool aCondition);

	private:
	CTestHarness();
	void ConstructL();
	void Clear();
	void Print(const TDesC& aText);
	void UseFont(CFbsFont* aFont);
	const TDesC& Translate(const TDesC& aText);

	void SimpleTest1L();
	void SimpleTest2L();
	void LineBreakTestL();
	void PlainTextLineBreakTest();
	void IllegalLineBreakTestL();
	void BidirectionalTestL();
	void CustomizeTestL();
	void TruncationWithEllipsisTestL();
	void GetNextVisualCursorPosTestL();

	RTest iTest;
	RFbsSession* iFbs;
	CFbsBitGc* iGc;
	CFbsScreenDevice* iDev;
	CFbsFont* iStandardFont;
	TPoint iPrintPos;
	TInt iBaseLine;
	TInt iLineHeight;
	TBuf<256> iTranslation;
	TTmCharFormat iCharFormat;
	};

void CTestHarness::PerformTestsL()
	{
	BidirectionalTestL();
	GetNextVisualCursorPosTestL();
	SimpleTest1L();
	SimpleTest2L();
	LineBreakTestL();
	PlainTextLineBreakTest();
	IllegalLineBreakTestL();
	CustomizeTestL();
	TruncationWithEllipsisTestL();
	}

CTestHarness::CTestHarness():
	iTest(_L("TTAGMA")),
	iCharFormat(_L("ClearlyU"), 19)
	{
	iTest.Title();
	iTest.Start(_L("The TAGMA formatting engine."));
	}

CTestHarness* CTestHarness::NewL()
	{
	CTestHarness* t = new (ELeave)CTestHarness;
	CleanupStack::PushL(t);
	t->ConstructL();
	CleanupStack::Pop();
	return t;
	}

void CTestHarness::ConstructL()
	{
	RFbsSession::Connect();
	iFbs = RFbsSession::GetSession();
	if (iFbs == NULL)
		User::Leave(KErrGeneral);
	TRAPD(error,iDev = CFbsScreenDevice::NewL(0,EColor64K));
	if (error)
		iDev = CFbsScreenDevice::NewL(_L("scdv"),EGray4);
	iDev->ChangeScreenDevice(NULL);
	iDev->SetAutoUpdate(ETrue);
	iDev->CreateContext(iGc);
	TFontSpec fs(_L("Arial"),16);
	error = iDev->GetNearestFontInPixels((CFont*&)iStandardFont,fs);
	if (error)
		User::Panic(_L("Could not create 16-pixel Arial"),error);
	CFont* font = NULL;
	TAlgStyle as;
	error = iDev->GetFontById(font, TUid::Uid(KFontClearlyU), as);
	if (error)
		User::Panic(_L("ClearlyU font not installed"),error);
	iCharFormat.iFontSpec = font->FontSpecInTwips();
	if (iCharFormat.iFontSpec.Name().CompareF(_L("ClearlyU")))
		User::Panic(_L("ClearlyU font not installed"),error);
	iDev->ReleaseFont(font);
	}

CTestHarness::~CTestHarness()
	{
	if (iStandardFont)
		iDev->ReleaseFont(iStandardFont);
	delete iGc;
	delete iDev;
	RFbsSession::Disconnect();
	iTest.End();
	iTest.Close();
	}

const TDesC& CTestHarness::Translate(const TDesC& aText)
	{
	iTranslation.SetLength(0);
	for (TInt i = 0; i < aText.Length(); i++)
		{
		if (aText[i] == '#')
			{
			TPtrC16 p(aText.Mid(i + 1,4));
			TLex16 lex(p);
			TText x;
			TInt error = lex.Val(x,EHex);
			if (error)
				User::Panic(_L("Hex number expected"),error);
			iTranslation.Append(x);
			i += 4;
			}
		else
			iTranslation.Append(aText[i]);
		}
	return iTranslation;
	}

void CTestHarness::Test(TBool aCondition)
	{
	iTest(aCondition);
	}

/* *****
void CTestHarness::TestFramework()
	{
	iTest.Next(_L("test"));
	CTmText *text = CTmText::NewL(*iDev,200,TTmFormatParam::EWrap);
	CleanupStack::PushL(text);


	CleanupStack::PopAndDestroy(); // text
	iTest.Printf(_L("press a key...\n"));
	iTest.Getch();
	}
***** */

void CTestHarness::SimpleTest1L()
	{
	iTest.Next(_L("simple test 1"));
	CTmText* text = CTmText::NewL(*iDev,0,0);
	CleanupStack::PushL(text);

	// Empty text has no lines or paragraphs, and height and width of zero.
	iTest(text->Lines() == 0);
	iTest(text->Paragraphs() == 0);
	iTest(text->LayoutHeight() == 0);
	iTest(text->LayoutWidth() == 0);

	// Text containing a single 'a' has one line, one paragraph, and a positive height and width.
	text->InsertL(0,_L("a"));
	Test(text->Lines() == 1);
	iTest(text->Paragraphs() == 1);
	Test(text->LayoutHeight() > 0);
	Test(text->LayoutWidth() > 0);

	// Line break causes a line break.
	text->InsertL(1,LineBreak);
	Test(text->Lines() == 2);

	// Paragraph break causes a line break.
	text->InsertL(2,ParBreak);
	Test(text->Lines() == 3);

	CleanupStack::PopAndDestroy();
	iTest.Printf(_L("press a key...\n"));
	iTest.Getch();
	}

void CTestHarness::SimpleTest2L()
	{
	iTest.Next(_L("simple test 2"));
	CTmText *text = CTmText::NewL(*iDev,300,TTmFormatParam::EWrap);
	CleanupStack::PushL(text);
	const TText* sample = (const TText*)
						  L"Dissatisfaction with his lot seems to be the characteristic of men in all ages and "
						  L"climates. So far, however, from being an evil, as at first might be supposed, it "
						  L"has been the great civiliser of our race; and has tended, more than any thing else, "
						  L"to raise us above the condition of the brutes. But the same discontent which has been "
						  L"the source of all improvement, has been the parent of no small progeny of follies and "
						  L"absurdities; to trace these latter is our present object.";
	text->InsertL(0,TPtrC(sample));

	// There is one paragraph.
	TInt pars = text->Paragraphs();
	Test(pars == 1);

	// There is at least one line.
	TInt lines = text->Lines();
	Test(lines >= 1);

	// First line has start-of-paragraph flag set.
	TTmLineInfo info;
	text->LineNumberToLine(0,info);
	Test(info.iFlags & TTmLineInfo::EParStart);

	// Last line has end-of-paragraph flag set.
	text->LineNumberToLine(lines - 1,info);
	Test(info.iFlags & TTmLineInfo::EParEnd);

	// Inserting a paragraph break increases the numbers of paragraphs by one.
	text->InsertL(93,ParBreak);
	pars++;
	Test(pars == text->Paragraphs());

	// Get the minimum width and height for various sizes not allowing illegal line breaks.
	TSize size;
	text->GetMinimumLayoutSizeL(0,size);
	Test(size.iWidth > 0 && size.iHeight > 0);
	text->GetMinimumLayoutSizeL(125,size);
	Test(size.iWidth > 0 && size.iHeight > 0);
	text->GetMinimumLayoutSizeL(KMaxTInt,size);
	Test(size.iWidth > 0 && size.iHeight > 0);

	// Get the minimum width and height for various sizes not allowing illegal line breaks.
	text->GetMinimumLayoutSizeL(0,ETrue,size);
	Test(size.iWidth > 0 && size.iHeight > 0);
	text->GetMinimumLayoutSizeL(125,ETrue,size);
	Test(size.iWidth > 0 && size.iHeight > 0);
	text->GetMinimumLayoutSizeL(KMaxTInt,ETrue,size);
	Test(size.iWidth > 0 && size.iHeight > 0);

	// Get the minimum width and height for various sizes allowing illegal line breaks.
	text->GetMinimumLayoutSizeL(0,EFalse,size);
	Test(size.iWidth > 0 && size.iHeight > 0);
	text->GetMinimumLayoutSizeL(125,EFalse,size);
	Test(size.iWidth > 0 && size.iHeight > 0);
	text->GetMinimumLayoutSizeL(KMaxTInt,EFalse,size);
	Test(size.iWidth > 0 && size.iHeight > 0);

	TPoint top_left(0,0);
	TRect clip(0,0,KMaxTInt,KMaxTInt);
	TLogicalRgb background = KRgbWhite;
	for (TInt i = 0; i < 100; i += 10)
		{
		text->SetWrapWidthL(200 + i);
		text->Draw(*iGc,top_left,clip,&background,TRUE);
		}

	CleanupStack::PopAndDestroy();
	iTest.Printf(_L("press a key...\n"));
	iTest.Getch();
	(iTest.Console())->ClearScreen();
	}

void CTestHarness::BidirectionalTestL()
	{
	iTest.Next(_L("bidirectional formatting"));
	CTmText *text = CTmText::NewL(*iDev,KMaxTInt,FALSE);
	CleanupStack::PushL(text);

	// Test the sample texts from the Unicode book.
	text->InsertL(0,Translate(Bd1));
	text->InsertL(KMaxTInt,Translate(Bd2));
	text->InsertL(KMaxTInt,Translate(Bd3));
	text->InsertL(KMaxTInt,Translate(Bd4));

	TBuf<256> buffer;
	TInt needed;
	text->GetDisplayedText(0,buffer,needed);
	Test(buffer == DisplayedBd1);
	text->GetDisplayedText(1,buffer,needed);
	Test(buffer == DisplayedBd2);
	text->GetDisplayedText(2,buffer,needed);
	Test(buffer == DisplayedBd3);
	text->GetDisplayedText(3,buffer,needed);
	Test(buffer == DisplayedBd4);

	// Insert some ordinary text and verify that bidirectional overrides span the whole of a paragraph but not more.
	_LIT(ordinary_text,"par1 line1#2028#202epar1 line2#2028par1 line3#2028par1 line4#2028par1 line5#2029par2 line1#2029");
	text->InsertL(0,Translate(ordinary_text));
	text->GetDisplayedText(3,buffer,needed);
	Test(buffer == _L("4enil 1rap"));
	text->GetDisplayedText(4,buffer,needed);
	Test(buffer == _L("5enil 1rap"));
	text->GetDisplayedText(5,buffer,needed);
	Test(buffer == _L("par2 line1"));

	/*
	Insert some Arabic letters and verify that they are right-to-left, and that numbers and punctuation are
	handled properly. Use contextual versions in some examples, base forms in others.
	*/
	text->Clear();
	text->InsertL(0,Translate(Arabic1),&iCharFormat);
	text->GetDisplayedText(0,buffer,needed);
	Test(buffer == Translate(DisplayedArabic1));
	text->InsertL(0,Translate(Arabic2),&iCharFormat);
	text->GetDisplayedText(0,buffer,needed);
	Test(buffer == Translate(DisplayedArabic2));
	text->InsertL(0,Translate(Arabic3),&iCharFormat);
	text->GetDisplayedText(0,buffer,needed);
	Test(buffer == Translate(DisplayedArabic3));
	text->InsertL(0,Translate(Arabic4),&iCharFormat);
	text->GetDisplayedText(0,buffer,needed);
	Test(buffer == Translate(DisplayedArabic4));
	text->InsertL(0,Translate(Arabic5),&iCharFormat);
	text->GetDisplayedText(0,buffer,needed);
	Test(buffer == Translate(DisplayedArabic5));

	TPoint top_left(0,50);
	TRect clip(0,0,KMaxTInt,KMaxTInt);
	TLogicalRgb background = KRgbWhite;
	text->Draw(*iGc,top_left,clip,&background,TRUE);

	CleanupStack::PopAndDestroy(); // text
	iTest.Printf(_L("press a key...\n"));
	iTest.Getch();
	(iTest.Console())->ClearScreen();
	}

void CTestHarness::TruncationWithEllipsisTestL()
	{
	iTest.Next(_L("truncation with ellipsis"));
	CTmText *text = CTmText::NewL(*iDev,200,TTmFormatParam::ETruncateWithEllipsis);
	CleanupStack::PushL(text);

	text->InsertL(0,_L("Truncation of sesquipedalian verbiage using non-trivial algorithms."));
	text->InsertL(text->EndChar() - 1,LineBreak);
	text->InsertL(text->EndChar() - 1,_L("Antidisestablishmentarianismically speaking this makes sense."));
	text->InsertL(text->EndChar() - 1,LineBreak);
	text->InsertL(text->EndChar() - 1,_L("Short line."));
	TPoint top_left(0,120);
	TRect clip(0,0,KMaxTInt,KMaxTInt);
	TLogicalRgb background = KRgbWhite;
	for (TInt i = 0; i <= 400; i++)
		{
		text->SetWrapWidthL(i);
		text->Draw(*iGc,top_left,clip,&background,TRUE);
		iGc->SetBrushStyle(CGraphicsContext::ENullBrush);
		iGc->SetPenStyle(CGraphicsContext::ESolidPen);
		iGc->SetPenColor(KRgbRed);
		TRect outline(0,120,i,120 + text->LayoutHeight());
		iGc->DrawRect(outline);
		}

	CleanupStack::PopAndDestroy(); // text
	iTest.Printf(_L("press a key...\n"));
	iTest.Getch();
	}

void CTestHarness::LineBreakTestL()
	{
	iTest.Next(_L("line breaking"));
	CTmText *text = CTmText::NewL(*iDev,1,TTmFormatParam::EWrap | TTmFormatParam::ELegalLineBreaksOnly);
	CleanupStack::PushL(text);
					  // line ends should occur only at the places marked by asterisks (last is 66 not 65; end of par)
					  //		  1			2		  3			4		  5			6
					  //01234567890123456789012345678901234567890123456789012345678901234567890
					  //....*...*.....*.........*....*...*.....*.....*...*......*.........*....
	text->InsertL(0,_L("one two,three ( four )% five-six/seven.eight 9.9 $10.00 eleven !!"));
	TInt lines = text->Lines();
	Test(lines == 11);

	static TInt line_end[11] = { 4, 8, 14, 24, 29, 33, 39, 45, 49, 56, 66 };
	TTmLineInfo info;
	for (TInt i = 0; i < 11; i++)
		{
		text->LineNumberToLine(i,info);
		Test(info.iEnd == line_end[i]);
		}

	CleanupStack::PopAndDestroy(); // text
	iTest.Printf(_L("press a key...\n"));
	iTest.Getch();
	}

void CTestHarness::PlainTextLineBreakTest()
	{
	iTest.Next(_L("plain text line breaking"));
	_LIT(text,"one two,three ( four )% five-six/seven.eight 9.9 $10.00 eleven !!");
	TInt break_pos;
	TInt next_line_start;
	MTmSource::GetStandardLineBreak(text,0,20,break_pos,next_line_start);
	Test(break_pos == 13 && next_line_start == 14);
	MTmSource::GetStandardLineBreak(text,20,40,break_pos,next_line_start);
	Test(break_pos == 39 && next_line_start == 39);
	MTmSource::GetStandardLineBreak(text,40,50,break_pos,next_line_start);
	Test(break_pos == 48 && next_line_start == 49);
	MTmSource::GetStandardLineBreak(text,50,64,break_pos,next_line_start);
	Test(break_pos == 55 && next_line_start == 56);
	}

void CTestHarness::IllegalLineBreakTestL()
	{
	iTest.Next(_L("line breaking at an illegal position when there are no legal ones"));
	CTmText *text = CTmText::NewL(*iDev,1,TTmFormatParam::EWrap);
	CleanupStack::PushL(text);
	text->InsertL(0,_L("test"));
	TInt lines = text->Lines();
	Test(lines == 5); // each character goes on a separate line. Include an extra end of line char from INC087637
	CleanupStack::PopAndDestroy(); // text
	iTest.Printf(_L("press a key...\n"));
	iTest.Getch();
	}

void CTestHarness::GetNextVisualCursorPosTestL()
	{
	iTest.Next(_L("get next visual cursor position"));
	CTmText *text = CTmText::NewL(*iDev,200,TTmFormatParam::EWrap);
	CleanupStack::PushL(text);
	text->InsertL(0,Translate(Arabic4),&iCharFormat);
	TTmPosInfo caret;
	TTmLineInfo line_info;
	TPoint p(0,0);
	text->FindXyPos(p,caret,line_info);
	// Any position at 0 will do.
	Test(caret.iEdge.iX == 0);
	Test(caret.iDocPos.iPos == 0
		|| (caret.iDocPos.iPos == 1 && !caret.iDocPos.iLeadingEdge)
		|| (caret.iDocPos.iPos == 8 && !caret.iDocPos.iLeadingEdge));

	// Traverse the first few visual caret positions, including a right-to-left section and a left-to-right section.
	const TInt caret_positions = 7;

	TInt i;
	static TInt left_to_right_caret_position[caret_positions] = { 1, 6, 3, 1, 9, 10, 11 };
	for (i = 1; i < caret_positions; i++)
		{
		TTmPosInfo next_caret;
		text->GetNextVisualCursorPos(caret.iDocPos,next_caret,FALSE);
		caret = next_caret;
		Test(caret.iDocPos.iPos == left_to_right_caret_position[i]);
		}

	// Going from right to left 8 occurs twice, once as a leading edge and once as a trailing edge.
	static TInt right_to_left_caret_position[caret_positions] = { 8, 6, 3, 8, 9, 10, 11 };
	for (i = caret_positions - 2; i >= 0; i--)
		{
		TTmPosInfo next_caret;
		text->GetNextVisualCursorPos(caret.iDocPos,next_caret,TRUE);
		caret = next_caret;
		Test(caret.iDocPos.iPos == right_to_left_caret_position[i]);
		}

	TPoint top_left(100,150);
	TRect clip(0,0,KMaxTInt,KMaxTInt);
	TLogicalRgb background = KRgbWhite;
	text->Draw(*iGc,top_left,clip,&background,TRUE);
	CleanupStack::PopAndDestroy(); // text
	iTest.Printf(_L("press a key...\n"));
	iTest.Getch();
	(iTest.Console())->ClearScreen();
	}

TRgb TMyCustom::SystemColor(TUint aColorIndex,TRgb aDefaultColor) const
	{
	if (aColorIndex == TLogicalRgb::ESystemForegroundIndex)
		return KRgbBlue;
	if (aColorIndex == TLogicalRgb::ESystemBackgroundIndex)
		return KRgbYellow;
	return aDefaultColor;
	}

void TMyCustom::DrawBackground(CGraphicsContext& aGc,const TPoint& /*aTextLayoutTopLeft*/,const TRect& aRect,
							   const TLogicalRgb& aBackground,TRect& aRectDrawn) const
	{
	SetBrushColor(aGc,aBackground);
	aGc.SetBrushStyle(CGraphicsContext::ESolidBrush);
	aGc.SetPenStyle(CGraphicsContext::ENullPen);
	aGc.DrawRect(aRect);
	aRectDrawn = aRect;
	}

void TMyCustom::DrawLineGraphics(CGraphicsContext& aGc,const TPoint& /*aTextLayoutTopLeft*/,const TRect& /*aRect*/,
								 const TTmLineInfo& aLineInfo) const
	{
	aGc.SetPenColor(KRgbRed);
	aGc.SetPenStyle(CGraphicsContext::ESolidPen);
	TPoint p;
	p.iX = aLineInfo.iOuterRect.iTl.iX;
	p.iY = aLineInfo.iInnerRect.iBr.iY - 1;
	TPoint q = p;
	q.iX = aLineInfo.iOuterRect.iBr.iX - 1;
	aGc.DrawLine(p,q);
	}

void CTestHarness::CustomizeTestL()
	{
	iTest.Next(_L("test custom layout and drawing"));
	CTmText *text = CTmText::NewL(*iDev,250,TTmFormatParam::EWrap);
	CleanupStack::PushL(text);
	TMyCustom custom;
	text->CustomizeL(&custom);
	const TText* sample = (const TText*)
						  L"This is a test of the customization of drawing and layout. The text should appear "
						  L"in blue on a yellow background, underlined in red.";
	text->InsertL(0,TPtrC(sample));
	TPoint top_left(100,150);
	TRect clip(0,0,KMaxTInt,KMaxTInt);
	TLogicalRgb background = KRgbWhite;
	text->Draw(*iGc,top_left,clip,&background,TRUE);

	CleanupStack::PopAndDestroy(); // text
	iTest.Printf(_L("press a key...\n"));
	iTest.Getch();
	(iTest.Console())->ClearScreen();
	}

void CTestHarness::Clear()
	{
	TRect r(iDev->SizeInPixels());
	iGc->Clear(r);
	iPrintPos = TPoint(0,0);
	}

void CTestHarness::Print(const TDesC& aText)
	{
	TPoint p = iPrintPos;
	p.iY += iBaseLine;
	iGc->DrawText(aText,p);
	iPrintPos.iX = 0;
	iPrintPos.iY += iLineHeight;
	}

void CTestHarness::UseFont(CFbsFont* aFont)
	{
	iGc->UseFont(aFont);
	TOpenFontMetrics metrics;
	aFont->GetFontMetrics(metrics);
	iBaseLine = metrics.MaxHeight();
	iLineHeight = metrics.MaxHeight() + metrics.MaxDepth();
	}

void RunTestsL()
	{
	test.Title();
	test.Start(_L(" @SYMTestCaseID:SYSLIB-FORM-LEGACY-TTAGMA-0001 "));

	CTestHarness* test_harness = CTestHarness::NewL();
	test_harness->PerformTestsL();
	delete test_harness;

	test.End();
	test.Close();
	}

TInt E32Main()
    {
	FbsStartup();
	CTrapCleanup* theCleanup =CTrapCleanup::New();
	TRAPD(err,RunTestsL());

	delete theCleanup;
	return err;
    }
