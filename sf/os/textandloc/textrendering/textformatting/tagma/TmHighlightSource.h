/*
* Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "TAGMA.H"
#include "TmTextDrawExt.h"

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "TAGMA_INTERNAL.H"
#endif

/**
 * MTmSource decorator that can draw text in the appropriate highlight colors.
 * The text must already be on the screen before this is drawn over it.
 * @since 7.0
 * @internalComponent
 */
NONSHARABLE_CLASS(TTmHighlightSource) : public MTmSource
	{
public:
	TTmHighlightSource(MTmSource& aDelegate);
	/** returns EFalse if the selection foreground and background colors are the same */
	inline TBool HighlightVisible() {return 0xFFFFFF & (iForeground.Value() ^ iBackground.Value());}
	void DrawBackground(CGraphicsContext& aGc,
		const TPoint& aTextLayoutTopLeft, const TRect& aRect,
		const TLogicalRgb& aBackground, TRect& aRectDrawn) const;
	void DrawLineGraphics(CGraphicsContext& aGc,
		const TPoint& aTextLayoutTopLeft, const TRect& aRect,
		const TTmLineInfo& aLineInfo) const;
	void DrawText(CGraphicsContext& aGc,
		const TPoint& aTextLayoutTopLeft, const TRect& aRect,
		const TTmLineInfo& aLineInfo, const TTmCharFormat& aFormat,
		const TDesC& aText, const TInt aStart, const TInt aEnd, const TPoint& aTextOrigin, TInt aExtraPixels) const;
	void DrawPicture(CGraphicsContext& aGc,
		const TPoint& aTopLeft, const TRect& aRect,
		MGraphicsDeviceMap& aDevice, const CPicture& aPicture) const;
	// MTmSource functions: just pass through
	MGraphicsDeviceMap& FormatDevice() const;
	MGraphicsDeviceMap& InterpretDevice() const;
	TInt DocumentLength() const;
	void GetText(TInt aPos,TPtrC& aText,TTmCharFormat& aFormat) const;
	void GetParagraphFormatL(TInt aPos,RTmParFormat& aFormat) const;
	TInt ParagraphStart(TInt aPos) const;
	CPicture* PictureL(TInt aPos) const;
	TInt GetPictureSizeInTwipsL(TInt aPos,TSize& aSize) const;
	TBool PageBreakInRange(TInt aStartPos,TInt aEndPos) const;
	TAny* GetExtendedInterface(const TUid& aInterfaceId);

private:
	/**
	 * Colour to draw the text in
	 * @internalComponent
	 */
	TRgb iForeground;
	/**
	 * Colour to draw the background in
	 * @internalComponent
	 */
	TRgb iBackground;
	/**
	 * Colour to blend the pictures with
	 * @internalComponent
	 */
	TRgb iXorPictures;
	MTmSource& iDelegate;
	MTmTextDrawExt* iTmTextDrawExt;//The implementation of MTmTextDrawExt interface will be
								   //used and it will replace all CGraphicsContext::Draw<Item> calls.
	TTmTextDrawExt	iTmTextDrawExtDefault;//The default implementation of MTmTextDrawExt
	};

