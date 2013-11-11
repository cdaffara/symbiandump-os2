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
* Header for the CWordTest class; testing functions added to WORD after it ceased to be a standard EPOC application.
*
*/




#ifndef __WPTEST_H__
#define __WPTEST_H__ 1

#include <e32std.h>
#include <txtfrmat.h> 

class CWordTest: public CBase, public MFormCustomDraw
/**
@internalComponent
*/
	{
public:
	static CWordTest* NewL(CTextView* aTextView);
	~CWordTest();
	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aEventCode,TChar& aCharToBeInserted);
	void Highlight(MUnifiedEditor& aEditor,TFontPresentation::TFontHighlightStyle aStyle);
	void SetPictureAlignmentL(CEikRichTextEditor* aEditor,TFontPresentation::TAlignment aAlignment);
	void SetWrapL(CEikRichTextEditor* aEditor,TBool aWrap);

	enum TKeyboardCode
		{
		EStandardKeyboard,
		EArabicKeyboard,
		EGreekKeyboard,
		ERussianKeyboard
		};

	TKeyboardCode Keyboard() const { return iKeyboard; }
	void SetKeyboardL(TKeyboardCode aKeyboard);
	void TurnOnCustomDrawing(CTextLayout* aLayout);
	TBool RuledPaper() const { return iRuledPaper; }
	void SetRuledPaperL(CEikRichTextEditor* aEditor,TBool aOn);
	TBool BackgroundBitmap() const { return iBackgroundBitmap != NULL; }
	void SetBackgroundBitmapL(CEikRichTextEditor* aEditor,TBool aOn);
	void ToggleCaseL(CEikRichTextEditor* aEditor);
	void BenchmarkL();
	void SetTruncateWithEllipsisL(CEikRichTextEditor* aEditor,TBool aOn);
	TBool TruncateWithEllipsis() const { return iTruncateWithEllipsis; }
	TBool CursorFlashing() const { return iCursorFlashing; }
	TTmCursorPlacement CursorPlacement() const { return iCursorPlacement; }
	TInt CursorWeight() const { return iCursorWeight; }
	TRgb CursorXorColor() const { return iCursorXorColor; }
	void ToggleCursorFlash();
	void SetCursorPlacement(TTmCursorPlacement aPlacement);
	void SetCursorWeight(TInt aWeight);
	void SetCursorXorColor(TRgb aColor);
	void SetSelectionHighlightStyle(TInt aStyleIndex);
	TInt SelectionHighlightStyle() const;
	void SetCursorPositioningHintL(TCursorPosition::TPosHint aHint);
	TCursorPosition::TPosHint CursorPositioningHint() const;
	void InsertPictureL(CEikRichTextEditor* aEditor);
private:
	CWordTest(CTextView* aTextView);

	// virtual functions from MFormCustomDraw
	void DrawLineGraphics(const TParam& aParam,const TLineInfo& aLineInfo) const;
	void DrawText(const TParam& aParam,const TLineInfo& aLineInfo,const TCharFormat& aFormat,const TDesC& aText,
				  const TPoint& aTextOrigin,TInt aExtraPixels) const;
	void DrawBackground(const TParam& aParam,const TRgb& aBackground,TRect& aDrawn) const;
	TRgb SystemColor(TUint aColorIndex, TRgb aDefaultColor) const;

	CTextView* iTextView;
	TKeyboardCode iKeyboard;
	TInt iKeyboardState;
	TBool iRuledPaper;
	CFbsBitmap* iBackgroundBitmap;
	TBool iTruncateWithEllipsis;
	TBool iCursorFlashing;
	TTmCursorPlacement iCursorPlacement;
	TInt iCursorWeight;
	TRgb iCursorXorColor;
	TInt iStyleIndex;
	TCursorPosition::TPosHint iPositioningHint;
	};

#endif // __WPTEST_H__
