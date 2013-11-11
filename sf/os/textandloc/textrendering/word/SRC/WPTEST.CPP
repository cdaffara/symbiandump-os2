/*
* Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Functions added to Word for test purposes now that Word is no longer a standard EPOC application.
*
*/


#include <e32std.h>
#include <techview/eikcfdlg.h>
#include <techview/eikon.rsg>
#include "WPAPPUI.H"
#include "WPTEXTED.H"
#include "WPTEST.H"
#include "WPTESTPICTURE.H"


struct TKeyAction
	{
	TInt Compare(TInt aKeyCode,TInt aState) const;

	TInt iKeyCode;			// key pressed by user
	TInt iState;			// state in which this action fires
	TInt iCharToInsert;		// character to insert; if 0xFFFF, none
	TInt iNewState;			// new state
	};

TInt TKeyAction::Compare(TInt aKeyCode,TInt aState) const
	{
	if (iKeyCode - aKeyCode)
		return iKeyCode - aKeyCode;
	return iState - aState;
	}

struct TKeyboardMap
	{
	const TKeyAction* Find(TInt aKeyCode,TInt aState) const;

	const TKeyAction* iKeyAction;	// action table
	TInt iKeyActions;				// number of entries in table
	};

const TKeyAction* TKeyboardMap::Find(TInt aKeyCode,TInt aState) const
	{
	const TKeyAction* base = iKeyAction;
	const TKeyAction* end = iKeyAction + iKeyActions;
	while (base < end)
		{
		int n = end - base;
		const TKeyAction* a = &base[n / 2];
		int diff = a->Compare(aKeyCode,aState);
		if (diff > 0)
			end = a;
		else if (diff < 0)
			base = a + 1;
		else
			return a;
		}
	return NULL;
	}

static const TKeyAction TheArabicKeyAction[] =
	{
	{ 'A', 0, 0x0649, 0 },
	{ 'B', 0, 0xFFFF, 0 },
	{ 'C', 0, 0xFFFF, 0 },
	{ 'D', 0, 0x0636, 0 },
	{ 'E', 0, 0xFFFF, 0 },
	{ 'F', 0, 0xFFFF, 0 },
	{ 'G', 0, 0x063A, 0 },
	{ 'H', 0, 0x062D, 0 },
	{ 'I', 0, 0xFFFF, 0 },
	{ 'J', 0, 0xFFFF, 0 },
	{ 'K', 0, 0xFFFF, 0 },
	{ 'L', 0, 0xFFFF, 0 },
	{ 'M', 0, 0xFFFF, 0 },
	{ 'N', 0, 0xFFFF, 0 },
	{ 'O', 0, 0xFFFF, 0 },
	{ 'P', 0, 0xFFFF, 0 },
	{ 'Q', 0, 0xFFFF, 0 },
	{ 'R', 0, 0xFFFF, 0 },
	{ 'S', 0, 0x0635, 0 },
	{ 'T', 0, 0x0637, 0 },
	{ 'U', 0, 0xFFFF, 0 },
	{ 'V', 0, 0xFFFF, 0 },
	{ 'W', 0, 0xFFFF, 0 },
	{ 'X', 0, 0xFFFF, 0 },
	{ 'Y', 0, 0xFFFF, 0 },
	{ 'Z', 0, 0x0638, 0 },
	{ 'a', 0, 0x0627, 0 },
	{ 'b', 0, 0x0628, 0 },
	{ 'c', 0, 0xFFFF, 0 },
	{ 'd', 0, 0x062F, 0 },
	{ 'e', 0, 0xFFFF, 0 },
	{ 'f', 0, 0x0641, 0 },
	{ 'g', 0, 0x0639, 0 },
	{ 'h', 0, 0x0647, 0 },
	{ 'i', 0, 0xFFFF, 0 },
	{ 'j', 0, 0x062C, 0 },
	{ 'k', 0, 0x0643, 0 },
	{ 'l', 0, 0x0644, 0 },
	{ 'm', 0, 0x0645, 0 },
	{ 'n', 0, 0x0646, 0 },
	{ 'o', 0, 0xFFFF, 0 },
	{ 'p', 0, 0x067E, 0 },
	{ 'q', 0, 0x0642, 0 },
	{ 'r', 0, 0x0631, 0 },
	{ 's', 0, 0x0633, 0 },
	{ 't', 0, 0x062A, 0 },
	{ 'u', 0, 0xFFFF, 0 },
	{ 'v', 0, 0xFFFF, 0 },
	{ 'w', 0, 0x0648, 0 },
	{ 'x', 0, 0x062E, 0 },
	{ 'y', 0, 0x064A, 0 },
	{ 'z', 0, 0x0632, 0 },
	};
static const TKeyboardMap TheArabicKeyboardMap =
	{
	TheArabicKeyAction,
	sizeof(TheArabicKeyAction) / sizeof(TKeyAction)
	};

static const TKeyAction TheGreekKeyAction[] =
	{
	{ 'A', 0, 0x0391, 0 },
	{ 'B', 0, 0x0392, 0 },
	{ 'C', 0, 0xFFFF, 0 },
	{ 'D', 0, 0x0394, 0 },
	{ 'E', 0, 0x0395, 0 },
	{ 'F', 0, 0x03A6, 0 },
	{ 'G', 0, 0x0393, 0 },
	{ 'H', 0, 0x03A7, 0 },
	{ 'I', 0, 0x0399, 0 },
	{ 'J', 0, 0x0397, 0 },
	{ 'K', 0, 0x039A, 0 },
	{ 'L', 0, 0x039B, 0 },
	{ 'M', 0, 0x039C, 0 },
	{ 'N', 0, 0x039D, 0 },
	{ 'O', 0, 0x039F, 0 },
	{ 'P', 0, 0x03A0, 0 },
	{ 'Q', 0, 0x0398, 0 },
	{ 'R', 0, 0x03A1, 0 },
	{ 'S', 0, 0x03A3, 0 },
	{ 'T', 0, 0x03A4, 0 },
	{ 'U', 0, 0x03A5, 0 },
	{ 'V', 0, 0x03A8, 0 },
	{ 'W', 0, 0x03A9, 0 },
	{ 'X', 0, 0x039E, 0 },
	{ 'Y', 0, 0xFFFF, 0 },
	{ 'Z', 0, 0x0396, 0 },
	{ 'a', 0, 0x03B1, 0 },
	{ 'b', 0, 0x03B2, 0 },
	{ 'c', 0, 0x03C2, 0 },
	{ 'd', 0, 0x03B4, 0 },
	{ 'e', 0, 0x03B5, 0 },
	{ 'f', 0, 0x03C6, 0 },
	{ 'g', 0, 0x03B3, 0 },
	{ 'h', 0, 0x03C7, 0 },
	{ 'i', 0, 0x03B9, 0 },
	{ 'j', 0, 0x03B7, 0 },
	{ 'k', 0, 0x03BA, 0 },
	{ 'l', 0, 0x03BB, 0 },
	{ 'm', 0, 0x03BC, 0 },
	{ 'n', 0, 0x03BD, 0 },
	{ 'o', 0, 0x03BF, 0 },
	{ 'p', 0, 0x03C0, 0 },
	{ 'q', 0, 0x03B8, 0 },
	{ 'r', 0, 0x03C1, 0 },
	{ 's', 0, 0x03C3, 0 },
	{ 't', 0, 0x03C4, 0 },
	{ 'u', 0, 0x03C5, 0 },
	{ 'v', 0, 0x03C8, 0 },
	{ 'w', 0, 0x03C9, 0 },
	{ 'x', 0, 0x03BE, 0 },
	{ 'y', 0, 0xFFFF, 0 },
	{ 'z', 0, 0x03B6, 0 },
	};
static const TKeyboardMap TheGreekKeyboardMap =
	{
	TheGreekKeyAction,
	sizeof(TheGreekKeyAction) / sizeof(TKeyAction)
	};

static const TKeyAction TheRussianKeyAction[] =
	{
	{ 'A', 0, 0x0410, 0 },
	{ 'B', 0, 0x0411, 0 },
	{ 'C', 0, 0x0426, 0 },
	{ 'D', 0, 0x0414, 0 },
	{ 'E', 0, 0x0415, 0 },
	{ 'F', 0, 0x0424, 0 },
	{ 'G', 0, 0x0413, 0 },
	{ 'H', 0, 0x0425, 0 },
	{ 'I', 0, 0x0418, 0 },
	{ 'J', 0, 0xFFFF, 0 },
	{ 'K', 0, 0x041A, 0 },
	{ 'L', 0, 0x041B, 0 },
	{ 'M', 0, 0x041C, 0 },
	{ 'N', 0, 0x041D, 0 },
	{ 'O', 0, 0x041E, 0 },
	{ 'P', 0, 0x041F, 0 },
	{ 'Q', 0, 0xFFFF, 0 },
	{ 'R', 0, 0x0420, 0 },
	{ 'S', 0, 0x0421, 0 },
	{ 'T', 0, 0x0422, 0 },
	{ 'U', 0, 0x0423, 0 },
	{ 'V', 0, 0x0412, 0 },
	{ 'W', 0, 0xFFFF, 0 },
	{ 'X', 0, 0xFFFF, 0 },
	{ 'Y', 0, 0x0419, 0 },
	{ 'Z', 0, 0x0417, 0 },
	{ 'a', 0, 0x0430, 0 },
	{ 'b', 0, 0x0431, 0 },
	{ 'c', 0, 0x0446, 0 },
	{ 'd', 0, 0x0434, 0 },
	{ 'e', 0, 0x0435, 0 },
	{ 'f', 0, 0x0444, 0 },
	{ 'g', 0, 0x0433, 0 },
	{ 'h', 0, 0x0445, 0 },
	{ 'i', 0, 0x0438, 0 },
	{ 'j', 0, 0xFFFF, 0 },
	{ 'k', 0, 0x043A, 0 },
	{ 'l', 0, 0x043B, 0 },
	{ 'm', 0, 0x043C, 0 },
	{ 'n', 0, 0x043D, 0 },
	{ 'o', 0, 0x043E, 0 },
	{ 'p', 0, 0x043F, 0 },
	{ 'q', 0, 0xFFFF, 0 },
	{ 'r', 0, 0x0440, 0 },
	{ 's', 0, 0x0441, 0 },
	{ 't', 0, 0x0442, 0 },
	{ 'u', 0, 0x0443, 0 },
	{ 'v', 0, 0x0432, 0 },
	{ 'w', 0, 0xFFFF, 0 },
	{ 'x', 0, 0xFFFF, 0 },
	{ 'y', 0, 0x0439, 0 },
	{ 'z', 0, 0x0437, 0 },
	};
static const TKeyboardMap TheRussianKeyboardMap =
	{
	TheRussianKeyAction,
	sizeof(TheRussianKeyAction) / sizeof(TKeyAction)
	};

CWordTest* CWordTest::NewL(CTextView* aTextView)
	{
	return new(ELeave) CWordTest(aTextView);
	}

CWordTest::CWordTest(CTextView* aTextView):
	iTextView(aTextView),
	iKeyboard(EStandardKeyboard),
	iKeyboardState(0),
	iRuledPaper(FALSE),
	iBackgroundBitmap(NULL),
	iTruncateWithEllipsis(FALSE),
	iCursorFlashing(TRUE),
	iCursorPlacement(ECursorVertical),
	iCursorWeight(3),
	iCursorXorColor(KRgbWhite),
	iStyleIndex(0),
	iPositioningHint(TCursorPosition::EPosHintUndefined)
	{
	}

CWordTest::~CWordTest()
	{
	delete iBackgroundBitmap;
	}

TKeyResponse CWordTest::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aEventCode,TChar& aCharToInsert)
	{
	if (iKeyboard == EStandardKeyboard)
		return EKeyWasNotConsumed;
	if (aEventCode != EEventKey)
		return EKeyWasNotConsumed;
	if (aKeyEvent.iCode >= ESpecialKeyBase && aKeyEvent.iCode < ESpecialKeyBase + ESpecialKeyCount)
		return EKeyWasNotConsumed;
	if (aKeyEvent.iCode >= ENonCharacterKeyBase && aKeyEvent.iCode < ENonCharacterKeyBase + ENonCharacterKeyCount)
		return EKeyWasNotConsumed;
	aCharToInsert = 0xFFFF;
		
	const TKeyboardMap* map = NULL;
	switch (iKeyboard)
		{
	case EArabicKeyboard:
		map = &TheArabicKeyboardMap;
		break;
	case EGreekKeyboard:
		map = &TheGreekKeyboardMap;
		break;
	case ERussianKeyboard:
		map = &TheRussianKeyboardMap;
		break;
	default:
		break;
		}
	if (map)
		{
		const TKeyAction* action = map->Find(aKeyEvent.iCode,iKeyboardState);
		if (action)
			{
			aCharToInsert = action->iCharToInsert;
			iKeyboardState = action->iNewState;
			return EKeyWasConsumed;
			}
		}
	return EKeyWasNotConsumed;
	}

void CWordTest::SetKeyboardL(TKeyboardCode aKeyboard)
	{
	switch (aKeyboard)
		{
	case EArabicKeyboard:
		SetCursorPositioningHintL(TCursorPosition::EInsertStrongR2L);
		break;
	default:
		SetCursorPositioningHintL(TCursorPosition::EInsertStrongL2R);
		break;
		}
	iKeyboard = aKeyboard;
	}

void CWordTest::SetPictureAlignmentL(CEikRichTextEditor* aEditor,TFontPresentation::TAlignment aAlignment)
	{
	TCharFormat format;
	format.iFontPresentation.iPictureAlignment = aAlignment;
	TCharFormatMask mask;
	mask.SetAttrib(EAttFontPictureAlignment);
	aEditor->ApplyCharFormatL(format,mask);
	}

void CWordTest::SetWrapL(CEikRichTextEditor* aEditor,TBool aWrap)
	{
	CParaFormat format;
	format.iWrap = aWrap;
	TParaFormatMask mask;
	mask.SetAttrib(EAttWrap);
	aEditor->ApplyParaFormatL(&format,mask);
	}
	
void CWordTest::InsertPictureL(CEikRichTextEditor* aEditor)
	{
	CWordTestPicture* pic=CWordTestPicture::NewLC();
	TPictureHeader header;
   	header.iPictureType = KUidWordTestPictureType;
   	header.iPicture=pic;
 		
 	TInt pos = aEditor->CursorPos();
 	aEditor->RichText()->InsertL(pos, header); 
//CRichText::InsertL takes ownership of the picture via TPictureHeader 	
 	CleanupStack::Pop(pic); 
 	
 	aEditor->HandleTextChangedL();
 	aEditor->SetCursorPosL(pos+1, EFalse);	
	}

void CWordTest::ToggleCaseL(CEikRichTextEditor* aEditor)
	{
	TCursorSelection sel = aEditor->Selection();
	int start = sel.LowerPos();
	int end = start;
	TBool to_upper = TRUE;
	while (end < sel.HigherPos())
		{
		TPtrC text;
		TCharFormat f;
		aEditor->RichText()->GetChars(text,f,start);
		int length = text.Length();
		if (start + length > sel.HigherPos())
			length = sel.HigherPos() - start;
		end = start + length;
		if (start == sel.LowerPos() && end > start)
			{
			if (TChar(text[0]).IsUpper())
				to_upper = FALSE;
			}
		TText* p = (TText*)text.Ptr();
		TText* q = p + length;
		while (p < q)
			{
			if (to_upper)
				*p = (TText)(TChar(*p).GetUpperCase());
			else
				*p = (TText)(TChar(*p).GetLowerCase());
			p++;
			}

		start = end;
		}
	aEditor->TextView()->HandleRangeFormatChangeL(sel);
	}

void CWordTest::BenchmarkL()
	{
	TTime start, end;
	start.HomeTime();
	const int n = 100;
	for (int i = 0; i < n; i++)
		iTextView->FormatTextL();
	end.HomeTime();
	int ms = (I64LOW(end.Int64()) - I64LOW(start.Int64())) / 1000;
	TBuf<128> message;
#ifdef _DEBUG
	_LIT(build,"debug");
#else
	_LIT(build,"release");
#endif
	message.Format(_L("form%d %S: reformatting %d times took %d milliseconds"),
		iTextView->Layout()->MajorVersion(),&build,n,ms);
	User::InfoPrint(message);
	}

void CWordTest::SetTruncateWithEllipsisL(CEikRichTextEditor* aEditor,TBool aOn)
	{
	iTruncateWithEllipsis = aOn;
	aEditor->TextLayout()->SetTruncating(aOn);
	aEditor->NotifyNewFormatL();
	}

void CWordTest::ToggleCursorFlash()
	{
	iCursorFlashing = !iCursorFlashing;
	iTextView->SetCursorFlash(iCursorFlashing);
	}

void CWordTest::SetCursorPlacement(TTmCursorPlacement aPlacement)
	{
	iCursorPlacement = aPlacement;
	iTextView->SetCursorPlacement(aPlacement);
	}

void CWordTest::SetCursorWeight(TInt aWeight)
	{
	iCursorWeight = aWeight;
	iTextView->SetCursorWeight(aWeight);
	}

void CWordTest::SetCursorXorColor(TRgb aColor)
	{
	iCursorXorColor = aColor;
	iTextView->SetCursorXorColor(aColor);
	}

void CWordTest::TurnOnCustomDrawing(CTextLayout* aLayout)
	{
	if (aLayout->MajorVersion() == 2) // custom drawing is not supported in FORM1
		aLayout->SetCustomDraw(this);
	}

void CWordTest::SetSelectionHighlightStyle(TInt aStyleIndex)
	{
	iStyleIndex = aStyleIndex;
	}

TInt CWordTest::SelectionHighlightStyle() const
	{
	return iStyleIndex;
	}

void CWordTest::DrawLineGraphics(const TParam& aParam,const TLineInfo& aLineInfo) const
	{
	if (iRuledPaper)
		{
		aParam.iGc.SetPenColor(KRgbRed);
		TPoint p(aLineInfo.iOuterRect.iTl.iX,aLineInfo.iInnerRect.iBr.iY - 1);
		TPoint q(aLineInfo.iOuterRect.iBr.iX,aLineInfo.iInnerRect.iBr.iY - 1);
		aParam.iGc.DrawLine(p,q);
		}
	}

void CWordTest::DrawText(const TParam& aParam,const TLineInfo& aLineInfo,const TCharFormat& aFormat,const TDesC& aText,
						 const TPoint& aTextOrigin,TInt aExtraPixels) const
	{
	/*
	Draw outlined text by drawing the text nine times: eight times in the background colour, moving the origin
	one pixel up, down, right, and left to make a box, and once in the foreground colour, not offset.
	*/
	if (aFormat.iFontPresentation.iHighlightStyle == TFontPresentation::EFontHighlightFirstCustomStyle)
		{
		aParam.iGc.SetPenColor(aFormat.iFontPresentation.iTextColor);
		for (int x = -1; x <= 1; x++)
			for (int y = -1; y <= 1; y++)
				if (x || y)
					{
					if (aExtraPixels)
						aParam.iGc.SetCharJustification(aExtraPixels,aText.Length());
					TPoint p(aTextOrigin);
					p.iX += x;
					p.iY += y;
					aParam.iGc.DrawText(aText,p);
					}
		aParam.iGc.SetPenColor(aFormat.iFontPresentation.iHighlightColor);
		}
	MFormCustomDraw::DrawText(aParam,aLineInfo,aFormat,aText,aTextOrigin,aExtraPixels);
	}

void CWordTest::DrawBackground(const TParam& aParam,const TRgb& aBackground,TRect& aDrawn) const
	{
	if (iBackgroundBitmap)
		{
		aParam.iGc.SetClippingRect(aParam.iDrawRect);
		aDrawn.SetRect(aParam.iTextLayoutTopLeft,iBackgroundBitmap->SizeInPixels());
		aParam.iGc.DrawBitmap(aDrawn,iBackgroundBitmap);
		}
	else
		MFormCustomDraw::DrawBackground(aParam,aBackground,aDrawn);
	}

TRgb CWordTest::SystemColor(TUint aColorIndex, TRgb aDefaultColor) const
	{
	if (aColorIndex == TLogicalRgb::ESystemSelectionForegroundIndex)
		{
		switch (iStyleIndex)
			{
		case 1:
			return KRgbBlue;
		case 2:
			return KRgbWhite;
		default:
			break;
			}
		}
	else if (aColorIndex == TLogicalRgb::ESystemSelectionBackgroundIndex)
		{
		switch (iStyleIndex)
			{
		case 1:
			return KRgbBlue;
		case 2:
			return KRgbRed;
		default:
			break;
			}
		}
	return aDefaultColor;
	}

void CWordTest::SetRuledPaperL(CEikRichTextEditor* aEditor,TBool aOn)
	{
	iRuledPaper = aOn;
	aEditor->NotifyNewFormatL();
	}

void CWordTest::SetBackgroundBitmapL(CEikRichTextEditor* aEditor,TBool aOn)
	{
	if (iBackgroundBitmap)
		delete iBackgroundBitmap;
	iBackgroundBitmap = NULL;
	if (aOn)
		{
		_LIT(KPathMask, "_:\\");
		TFileName filename(KPathMask);
		filename[0] = 'A' + static_cast<TInt>(RFs::GetSystemDrive());
		
		CEikFileOpenDialog* dialog = new(ELeave) CEikFileOpenDialog(&filename);
		if (dialog->ExecuteLD(R_EIK_DIALOG_FILE_OPEN))
			{
			iBackgroundBitmap = new(ELeave) CFbsBitmap;
			if (iBackgroundBitmap->Load(filename))
				User::InfoPrint(_L("cannot load this file as a bitmap"));
			}
		}
	aEditor->NotifyNewFormatL();
	}

void CWordTest::SetCursorPositioningHintL(
	TCursorPosition::TPosHint aHint)
	{
	iTextView->SetCursorPositioningHintL(aHint);
	iPositioningHint = aHint;
	}

TCursorPosition::TPosHint CWordTest::CursorPositioningHint() const
	{
	return iPositioningHint;
	}
