/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __FRMTLAY_INTERNAL_H__
#define __FRMTLAY_INTERNAL_H__

#include <e32std.h>
#include <e32base.h>
#include <gdi.h>
#include <tagma.h>

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <tagma_internal.h>
#endif

class MLayDoc;
class MFormCustomInterfaceProvider; 
class MFormCustomInvisibleCharacterRemapper;
class MFormCustomWrap;
class MFormCustomDraw;

/**
@internalTechnology
Internal to Symbian
*/
NONSHARABLE_CLASS(TLayDocTextSource) : public MTmSource, public MFormLabelApi, public MTmTextDrawExt, public MTmCustomExtension
	{
	public:

	enum
		{
		EDefaultFontHeightIncreaseFactor = 7,
		EDefaultMinimumLineDescent = 3
		};

	TLayDocTextSource();

	// overrides for MTmSource pure virtual functions
	MGraphicsDeviceMap& FormatDevice() const;
	MGraphicsDeviceMap& InterpretDevice() const;
	TInt DocumentLength() const;
	void GetText(TInt aPos,TPtrC& aText,TTmCharFormat& aFormat) const;
	void GetParagraphFormatL(TInt aPos,RTmParFormat& aFormat) const;
	TInt ParagraphStart(TInt aPos) const;

	// overrides for other MTmSource virtual functions
	TRgb SystemColor(TUint aColorIndex,TRgb aDefaultColor) const;
	CPicture* PictureL(TInt aPos) const;
	TInt GetPictureSizeInTwipsL(TInt aPos,TSize& aSize) const;
	TBool LabelModeSelect(TLabelType aType, TInt aPos);
	void LabelModeCancel();
	void LabelMetrics(TLabelType aType, TSize& aLabelSize, TInt& aMarginSize) const;
	TUint Map(TUint aChar) const;
	void SetLineHeight(const TLineHeightParam& aParam,TInt& aAscent,TInt& aDescent) const;
	TBool PageBreakInRange(TInt aStartPos,TInt aEndPos) const;
	void DrawBackground(CGraphicsContext& aGc,const TPoint& aTextLayoutTopLeft,const TRect& aRect,
						const TLogicalRgb& aBackground,TRect& aRectDrawn) const;
	void DrawLineGraphics(CGraphicsContext& aGc,const TPoint& aTextLayoutTopLeft,const TRect& aRect,
						  const TTmLineInfo& aLineInfo) const;
	void DrawText(CGraphicsContext& aGc,const TPoint& aTextLayoutTopLeft,const TRect& aRect,
				  const TTmLineInfo& aLineInfo,const TTmCharFormat& aFormat,
				  const TDesC& aText,const TPoint& aTextOrigin,TInt aExtraPixels) const;
	virtual void DrawPicture(CGraphicsContext& aGc,
							 const TPoint& aTextLayoutTopLeft, const TRect& aRect,
							 MGraphicsDeviceMap& aDevice, const CPicture& aPicture) const;
	void DrawText(CGraphicsContext& aGc,const TPoint& aTextLayoutTopLeft,const TRect& aRect,
				  const TTmLineInfo& aLineInfo,const TTmCharFormat& aFormat,
				  const TDesC& aText, const TInt aStart, const TInt aEnd, const TPoint& aTextOrigin,TInt aExtraPixels) const;
	
	// overrides for the MTmSource virtual functions which implements the MFormCustomWrap
	// if iCustomWrap is set. 
	TBool LineBreakPossible(TUint aPrevClass,TUint aNextClass,TBool aHaveSpaces) const;
	TUint LineBreakClass(TUint aCode,TUint& aRangeStart,TUint& aRangeEnd) const;
	TBool GetLineBreakInContext(const TDesC& aText,TInt aMinBreakPos,TInt aMaxBreakPos,
												 TBool aForwards,TInt& aBreakPos) const;
	TBool IsHangingCharacter(TUint aChar) const;
	
	// other functions
	TBool CanMap() const;

	TAny* GetExtendedInterface(const TUid& aInterfaceId);

	//MTmTextDrawExt implementations
	virtual void DrawLine(CGraphicsContext& aGc, const TPoint& aPt1, const TPoint& aPt2) const;
	virtual void DrawText(CGraphicsContext& aGc, const TDesC& aText, const TPoint& aPt) const;
	virtual void DrawRect(CGraphicsContext& aGc, const TRect& aRc) const;

	//Set/reset opaque flag of aGc graphics context
	void SetOpaque(CGraphicsContext& aGc) const;
	void ResetOpaque(CGraphicsContext& aGc) const;

	// flags
	enum
		{
		EWrap = 1,
		ETruncateWithEllipsis = 2,
		EUseLabelsDevice = 4
		};

	MLayDoc* iLayDoc;
	TUint iFlags;								// wrap, truncate, etc.
	TInt iWidth;								// wrapping width
	TChar iEllipsis;							// ellipsis character (or none if 0xFFFF) used if truncating
	TInt iLabelsWidth;
	TInt iLabelsGutter;
	CLayoutData::TFormatMode iFormatMode;
	MGraphicsDeviceMap* iImageDevice;			// device map used for drawing text
	MGraphicsDeviceMap* iLabelsDevice;			// device map used for labels
	MGraphicsDeviceMap* iFormatDevice;			// device map used for formatting
	TInt iFontHeightIncreaseFactor;				// percentage automatically added to font heights
	TInt iMinimumLineDescent;					// minimum line descent in pixels
	TNonPrintingCharVisibility iNonPrintingCharVisibility;
	const MFormParam* iFormParam;				// if non-null, points to the object that supplies system colours
	const MFormCustomDraw* iCustomDraw;			// if non-null, points to custom drawing routines
	const MFormCustomWrap* iCustomWrap;			// if non-null, points to custom wrapping routines 
    /** Optional object able to provide a concrete customisation object for use 
	    internally by Form. */
	MFormCustomInterfaceProvider* iInterfaceProvider;
	TBool iDrawOpaque;						// Opaque drawing active if the flag is ETrue
	TInt iExcessHeightRequired;				// delta required to position the baseline so there is enough
											// space for the highset glyph in pixels.  
	MFormCustomInvisibleCharacterRemapper* iInvisibleCharacterRemapper;	// Used to customize the remapping of 
																		// invisible characters to visible characters
	};

#endif
