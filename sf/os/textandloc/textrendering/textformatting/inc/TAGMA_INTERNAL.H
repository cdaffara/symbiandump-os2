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

#ifndef TAGMA_INTERNAL_H_
#define TAGMA_INTERNAL_H_

#include <e32base.h>
#include <openfont.h>
#include <txtfrmat.h>
#include <txtstyle.h>
#include <tagmalayoutandsource.h>

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <tagma.h>
#endif

/** 
A tabulation position, measured from the start of the line, which will be
the left end for paragraphs with a left-to-right base direction, and vice
versa. 
@internalComponent
*/
class TTmTab

	{
public:
	/** Tab type */
	enum TType
		{
		EStandardTab,	// text is placed after the tab, in the current paragraph direction
		ECenterTab,		// text is centred around the tab
		EReverseTab		// text is placed before the tab, in the current paragraph direction
		};

	inline TTmTab();
	inline TTmTab(const TTabStop& aTab);
	IMPORT_C void operator=(const TTabStop& aTab);
	IMPORT_C void GetTTabStop(TTabStop& aTab) const;
	IMPORT_C TBool operator==(const TTmTab& aTab) const;
	inline TBool operator!=(const TTmTab& aTab) const;

	TInt iPosition;
	TType iType;
	};

/** 
A bullet or other label inserted automatically at the start of a paragraph.
As well as an actual bullet, it can be a number or a letter. 
@internalComponent
*/
class TTmBullet

	{
public:
	/** Bullet style */
	enum TStyle
		{
		EBulletStyle,
		EArabicNumberStyle,
		ESmallRomanNumberStyle,
		ECapitalRomanNumberStyle,
		ESmallLetterStyle,
		ECapitalLetterStyle
		};

	/** The alignment within the margin of bullets or other text used in TTmBullet objects */
 	enum TAlignment
		{
		/** Bullet forward aligned. */
		ELeftAlign,
		/** Bullet centre aligned. */
		ECenterAlign,
		/** Bullet reverse aligned. */
		ERightAlign
		};

	IMPORT_C TTmBullet();
	IMPORT_C void operator=(const TBullet& aBullet);
	inline TTmBullet(const TBullet& aBullet);
	IMPORT_C void GetTBullet(TBullet& aBullet) const;
	IMPORT_C TBool operator==(const TTmBullet& aBullet) const;
	inline TBool operator!=(const TTmBullet& aBullet) const;

	TChar iCharacterCode;		// the bullet or other symbol used if iStyle is EBulletStyle
	TOpenFontSpec iFontSpec;
	TBool iHangingIndent;
	TLogicalRgb iColor;
	TStyle iStyle;				// is this a bullet or a number or a letter?
	TInt iStartNumber;			// the number of the first paragraph in a run of paragraphs in this style
	TAlignment iAlignment;		// alignment of the bullet or number within the margin
	};

/** 
A specification of a paragraph border rule: that is, a line or lines drawn
above, below, to the left, or to the right of a paragraph. 
@internalComponent
*/
class TTmParBorder

	{
public:
	/** Paragraph border style */
	enum TStyle
		{
		ESolidStyle,
		EDoubleStyle,
		EDotStyle,
		EDashStyle,
		EDotDashStyle,
		EDotDotDashStyle
		};

	IMPORT_C TTmParBorder();
	IMPORT_C void operator=(const TParaBorder& aBorder);
	inline TTmParBorder(const TParaBorder& aBorder);
	IMPORT_C void GetTParaBorder(TParaBorder& aBorder) const;
	IMPORT_C TBool operator==(const TTmParBorder& aBorder) const;
	inline TBool operator!=(const TTmParBorder& aBorder) const;

	TStyle iStyle;
	TInt iWeight;
	TLogicalRgb iColor;
	TBool iAutoColor;
	};

/**
MTmTextDrawExt interface UID.
@internalComponent
*/
const TUid KTmTextDrawExtId = {0x10203665};

/**
Text drawing - an extension interface. It offfers
DrawLine(), DrawText() and DrawRect() methods.
The idea is to move out from RTm<Name>Interpreter implementations all 
CGraphicsContext::Draw<Item> calls and replace them with the corresponding 
MTmTextDrawExt::Draw<Item> calls. The implementations of MTmTextDrawExt interface can customize
drawing methods implementations - for example - including opaque drawing support.
The MTmTextDrawExt interface UID is KTmTextDrawExtId. The interface implementation can be queried
through a MTmSource::GetExtendedInterface() call.
@internalComponent
*/
class MTmTextDrawExt
	{
public:
	virtual void DrawLine(CGraphicsContext& aGc, const TPoint& aPt1, const TPoint& aPt2) const = 0;
	virtual void DrawText(CGraphicsContext& aGc, const TDesC& aText, const TPoint& aPt) const = 0;
	virtual void DrawRect(CGraphicsContext& aGc, const TRect& aRc) const = 0;
	};

/**
@internalComponent
*/
const TUid KFormLabelApiExtensionUid = { 0x101FD03C};

/**
Class used to provide label functionality within Form
as an extended interface (via the GetExtendedInterface mechanism
supplied in MTmSource). This class is entirely internal to Form.
@internalComponent
@see MTmSource::GetExtendedInterface
@since Sirocco
*/
class MFormLabelApi
	{
public:
	IMPORT_C virtual TBool LabelModeSelect(MTmSource::TLabelType aType, TInt aPos);
	IMPORT_C virtual void LabelModeCancel();
	IMPORT_C virtual void LabelMetrics(MTmSource::TLabelType aType, TSize& aLabelSize, TInt& aMarginSize) const;
	};

/**
@internalTechnology
*/
class CTmBufSeg: public CBufSeg
	{
public:
	inline CTmBufSeg(TInt aExpandSize);
	TInt MemoryUsed() const;
	};

/** 
Additional parameters used when reformatting.
@internalComponent
*/
class TTmReformatParam

	{
public:
	IMPORT_C TTmReformatParam();

	TInt iStartChar;					// start of changed text
	TInt iOldLength;					// length of text before the change
	TInt iNewLength;					// length of text after the change
	TInt iMaxExtraLines;				// maximum lines in the partial paragraph after the end of the changed text
										// to format in one go; the remainder can be handled by background formatting
	TBool iParFormatChanged;			// TRUE if the paragraph format of the changed text has changed
	TBool iParInvalid;					// format to the end of the paragraph
	};

/** 
Information returned when reformatting. 
@internalComponent
*/
class TTmReformatResult

	{
public:
	IMPORT_C TTmReformatResult();

	TRect iRedrawRect;					// rectangle to be redrawn
	TInt iHeightChange;					// change in height of the reformatted text
	TInt iUnchangedTop;					// y coordinate, before formatting, of the top of text needing no change,
										// including text in any section not yet formatted by this call
	TInt iUnformattedStart;				// start of any unformatted section; KMaxTInt if none
	};

/**
@internalComponent
*/
class TTmHighlightExtensions

	{
public:
	inline TTmHighlightExtensions();
	inline void SetAll(TInt aExtension);
	inline void SetLeftExtension(TInt aExtension);
	inline void SetRightExtension(TInt aExtension);
	inline void SetTopExtension(TInt aExtension);
	inline void SetBottomExtension(TInt aExtension);
	inline TInt LeftExtension() const;
	inline TInt RightExtension() const;
	inline TInt TopExtension() const;
	inline TInt BottomExtension() const;
	inline TBool Extends() const;
	inline TBool Shrinks() const;
	inline void ExtendRect(TRect& aRect) const;
	inline void AdjustRect(TRect& aRect) const;
	inline TBool IsNull() const;
	inline void AbsExtendRect(TRect& aRect) const;
public:
	TInt iLeftExtension;
	TInt iRightExtension;
	TInt iTopExtension;
	TInt iBottomExtension;
	};

/** 
A bit mask for selecting one or more attributes of a TTmCharFormat object. 
@internalComponent
*/
class TTmCharFormatMask

	{
public:
	/** Constants, that are also bit assignments, representing character format attributes */
	enum TAttrib
		{
		EFontName = 0x1,
		EFontCoverage = 0x2,
		EBold = 0x4,
		EItalic = 0x8,
		ESerif = 0x10,
		EMonoWidth = 0x20,
		EHeight = 0x40,
		EPrintPosition = 0x80,
		ETextColor = 0x100,
		EBackgroundColor = 0x200,
		EBackground = 0x400,
		EUnderline = 0x800,
		EStrikethrough = 0x1000,
		EShadow = 0x2000,
		EUserDefinedEffects = 0x4000,
		ELanguage = 0x8000,
		EPictureAlignment = 0x10000
		};

	inline TTmCharFormatMask();
	IMPORT_C TTmCharFormatMask(const TCharFormatMask& aMask);
	IMPORT_C void GetTCharFormatMask(TCharFormatMask& aMask) const;
	inline void Clear(TAttrib aAttrib);
	inline void Set(TAttrib aAttrib);
	inline TBool IsSet(TAttrib aAttrib); 

	TUint iFlags;
	};

/** 
A bit mask for selecting one or more attributes of an RTmParFormat. 
@internalComponent
*/	
class TTmParFormatMask

	{
public:
	/** Constants, that are also bit assignments, representing paragraph format attributes */
	enum TAttrib
		{
		EAlignment = 0x1,
		EDirection = 0x2,
		EKeepTogether = 0x4,
		EKeepWithNext = 0x8,
		EStartNewPage = 0x10,
		EWidowOrphan = 0x20,
		ENoWrap = 0x40,
		EExactLineSpacing = 0x80,
		EPixelLineSpacing = 0x100,
		ELeadingMargin = 0x200,
		ETrailingMargin = 0x400,
		EFirstLineIndent = 0x800,
		ELineSpacing = 0x1000,
		ESpaceAbove = 0x2000,
		ESpaceBelow = 0x4000,
		ETabSize = 0x8000,
		EBorderMargin = 0x10000,
		ETabList = 0x20000,
		EBullet = 0x40000,
		EBorder = 0x80000
		};

	inline TTmParFormatMask();
	IMPORT_C TTmParFormatMask(const TParaFormatMask& aMask);
	IMPORT_C void GetTParaFormatMask(TParaFormatMask& aMask) const;
	inline void Clear(TAttrib aAttrib);
	inline void Set(TAttrib aAttrib);
	inline TBool IsSet(TAttrib aAttrib);

	TUint iFlags;
	};

/** 
A character format layer. Unmasked attributes are transparent. 
@internalComponent
*/	
class TTmCharFormatLayer

	{
public:
	inline TTmCharFormatLayer();
	IMPORT_C TTmCharFormatLayer(const CCharFormatLayer& aFormat);

	TTmCharFormat iFormat;
	TTmCharFormatMask iMask;
	};

/** 
A paragraph format layer. Unmasked attributes are transparent. 
@internalComponent
*/
class RTmParFormatLayer

	{
public:
	inline void Close();
	IMPORT_C void CopyL(const CParaFormatLayer& aFormat);

	RTmParFormat iFormat;
	TTmParFormatMask iMask;
	};

/**
 A style. 
@internalComponent
*/
class RTmStyle

	{
public:
	enum
		{
		EMaxName = KMaxParagraphStyleName
		};

	inline void Close();
	IMPORT_C void CopyL(const RParagraphStyleInfo& aStyle);
	IMPORT_C void GetRParagraphStyleInfoL(RParagraphStyleInfo& aStyle,
		const CParaFormatLayer& aGlobalParaFormatLayer,const CCharFormatLayer& aGlobalCharFormatLayer,
		const CStyleList& aList) const;

	TBuf<KMaxName> iName;
	TBuf<KMaxName> iNextStyleName;
	TTmCharFormatLayer iCharFormat;
	RTmParFormatLayer iParFormat;
	TInt iOutlineLevel;
	};

/** 
A self-contained text object. It owns both layout and content. It is
intended for labels and the general display of small-to-medium-sized amounts of
text. 
@internalComponent
*/
class CTmText: public CBase, public MTmTextLayoutForwarder

	{
public:
	IMPORT_C static CTmText* NewL(MGraphicsDeviceMap* aDevice = NULL,const TTmFormatParamBase* aFormatParam = NULL);
	IMPORT_C static CTmText* NewL(MGraphicsDeviceMap& aDevice,const TTmFormatParamBase& aFormatParam);
	IMPORT_C static CTmText* NewL(MGraphicsDeviceMap& aDevice,TInt aWrapWidth,TInt aFlags);
	IMPORT_C ~CTmText();
	IMPORT_C void InsertL(TInt aPos,const TDesC& aText,
						  const TTmCharFormat* aCharFormat = NULL,const RTmParFormat* aParFormat = NULL,
						  TRect* aRedrawRect = NULL,TInt* aScroll = NULL);
	IMPORT_C void GetFormat(TTmFormatParamBase& aFormatParam) const;
	IMPORT_C void SetWrapWidthL(TInt aWrapWidth);
	IMPORT_C void ChangeFormatL(const TTmFormatParamBase& aFormatParam);
	IMPORT_C void Clear();
	IMPORT_C void CustomizeL(const MTmCustom* aCustom);
	IMPORT_C TInt MemoryUsed() const;
	inline void Draw(CGraphicsContext& aGc,const TPoint& aTopLeft,const TRect& aClipRect,
					 const TLogicalRgb* aDocBackground,TBool aDrawParBackground);

private:
	CTmText();
	IMPORT_C void Spare1();

	// implementations of MTmTextLayoutForwarder virtual functions
	const CTmTextLayout& TextLayout() const;

	CTmTextImp* iImp;		// the implementation
	};

// inline functions
/** Constructs a TTmCharFormatMask. Turns all the flags on. */
TTmCharFormatMask::TTmCharFormatMask(): iFlags(0xFFFFFFFF)
	{
	}

/** Clears the selected flag.*/
void TTmCharFormatMask::Clear(TAttrib aAttrib)
	{
	iFlags &= ~aAttrib;
	}

/** Sets the selected flag. */
void TTmCharFormatMask::Set(TAttrib aAttrib)
	{
	iFlags |= aAttrib;
	}

/** Returns true if the selected attribute is set. */
TBool TTmCharFormatMask::IsSet(TAttrib aAttrib)
	{
	return iFlags & aAttrib;
	}


/** Constructs a TTmParFormatMask with all the flags on. */
TTmParFormatMask::TTmParFormatMask():
	iFlags(0xFFFFFFFF)
	{
	}

/** Clears the selected flag. */
void TTmParFormatMask::Clear(TAttrib aAttrib)
	{
	iFlags &= ~aAttrib;
	}

/** Sets the selected flag. */
void TTmParFormatMask::Set(TAttrib aAttrib)
	{
	iFlags |= aAttrib;
	}

/**
Returns true if the selected flag is Set.
*/
TBool TTmParFormatMask::IsSet(TAttrib aAttrib)
	{
	return iFlags & aAttrib;
	}

/** Constructs a TTmCharFormatLayer object containing default TTmCharFormat and
TTmCharFormatMask objects. */
TTmCharFormatLayer::TTmCharFormatLayer()
	{
	}

/** Closes an RTmParFormatLayer object by freeing any objects allocated on the
heap. */
void RTmParFormatLayer::Close()
	{
	iFormat.Close();
	}

/** Closes an RTmStyle object by freeing any objects allocated on the heap.
*/
void RTmStyle::Close()
	{
	iParFormat.Close();
	}

/** Draws the formatted text. This inline function just calls
CTmText::DrawLayout and is identical to it. It is provided for coding
convenience because of the expectation that a drawing function called Draw
exists. */
void CTmText::Draw(CGraphicsContext& aGc,const TPoint& aTopLeft,const TRect& aClipRect,
				   const TLogicalRgb* aDocBackground,TBool aDrawParBackground)
	{
	DrawLayout(aGc,aTopLeft,aClipRect,aDocBackground,aDrawParBackground);
	}

/** Constructs a TTmTab and set it to a standard tab stop with a position of 0. */
TTmTab::TTmTab():
	iPosition(0),
	iType(EStandardTab)
	{
	}

/** Constructs a TTmTab by converting the information in a TTabStop. */
TTmTab::TTmTab(const TTabStop& aTab)
	{
	*this = aTab;
	}

/** The inequality operator. Returns TRUE if this object and aTab differ in any
way. */
TBool TTmTab::operator!=(const TTmTab& aTab) const
	{
	return !operator==(aTab);
	}

/** Constructs a TTmBullet by converting the information in a TBullet. */
TTmBullet::TTmBullet(const TBullet& aBullet)
	{
	*this = aBullet;
	}

/** The inequality operator. Returns true if this object and aBullet differ in
any way. */
TBool TTmBullet::operator!=(const TTmBullet& aBullet) const
	{
	return !operator==(aBullet);
	}

/** Constructs a TTmParBorder by converting the information in a TParaBorder. */
TTmParBorder::TTmParBorder(const TParaBorder& aBorder)
	{
	*this = aBorder;
	}

/** The inequality operator. Returns TRUE if this object and aBorder differ in
any way. */
TBool TTmParBorder::operator!=(const TTmParBorder& aBorder) const
	{
	return !operator==(aBorder);
	}

CTmBufSeg::CTmBufSeg(TInt aExpandSize):
	CBufSeg(aExpandSize)
	{
	}

CTmCode::~CTmCode()
	{
	delete iBuffer;
	}

TPtr8 CTmCode::Ptr(TInt aPos)
	{
	return iBuffer->Ptr(aPos);
	}

CBufBase* CTmCode::Buffer()
	{
	return iBuffer;
	}

const CBufBase* CTmCode::Buffer() const
	{
	return iBuffer;
	}

/** Contains the extensions to character rectangle used when highlighting 
the text in reverse video
*/
TTmHighlightExtensions::TTmHighlightExtensions()
	{
	}

void TTmHighlightExtensions::SetAll(TInt aExtension)
	{
	iLeftExtension=iRightExtension=iTopExtension=iBottomExtension=aExtension;
	}

void TTmHighlightExtensions::SetLeftExtension(TInt aExtension)
	{
	iLeftExtension=aExtension;
	}

void TTmHighlightExtensions::SetRightExtension(TInt aExtension)
	{
	iRightExtension=aExtension;
	}

void TTmHighlightExtensions::SetTopExtension(TInt aExtension)
	{
	iTopExtension=aExtension;
	}

void TTmHighlightExtensions::SetBottomExtension(TInt aExtension)
	{
	iBottomExtension=aExtension;
	}

TInt TTmHighlightExtensions::LeftExtension() const
	{
	return iLeftExtension;
	}

TInt TTmHighlightExtensions::RightExtension() const
	{
	return iRightExtension;
	}

TInt TTmHighlightExtensions::TopExtension() const
	{
	return iTopExtension;
	}

TInt TTmHighlightExtensions::BottomExtension() const
	{
	return iBottomExtension;
	}

inline TBool TTmHighlightExtensions::Extends() const
	{
	return (iTopExtension > 0 || iBottomExtension > 0 || iLeftExtension > 0 || iRightExtension > 0);
	}

inline TBool TTmHighlightExtensions::Shrinks() const
	{
	return (iTopExtension < 0 || iBottomExtension < 0 || iLeftExtension < 0 || iRightExtension < 0);
	}

inline void TTmHighlightExtensions::ExtendRect(TRect& aRect) const
	{
	if (iLeftExtension>0)
		aRect.iTl.iX-=iLeftExtension;
	if (iRightExtension>0)
		aRect.iBr.iX+=iRightExtension;
	if (iTopExtension>0)
		aRect.iTl.iY-=iTopExtension;
	if (iBottomExtension>0)
		aRect.iBr.iY+=iBottomExtension;
	}

inline void TTmHighlightExtensions::AdjustRect(TRect& aRect) const
	{
	aRect.iTl.iX-=iLeftExtension;
	aRect.iTl.iY-=iTopExtension;
	aRect.iBr.iX+=iRightExtension;
	aRect.iBr.iY+=iBottomExtension;
	}

inline TBool TTmHighlightExtensions::IsNull() const
	{
	return 0 == iLeftExtension && 0 == iTopExtension && 0 == iRightExtension && 0 == iBottomExtension;
	}

inline void TTmHighlightExtensions::AbsExtendRect(TRect& aRect) const
	{
	if (iLeftExtension>0)
		aRect.iTl.iX-=iLeftExtension;
	else
		aRect.iTl.iX+=iLeftExtension;

	if (iRightExtension>0)
		aRect.iBr.iX+=iRightExtension;
	else
		aRect.iBr.iX-=iRightExtension;

	if (iTopExtension>0)
		aRect.iTl.iY-=iTopExtension;
	else
		aRect.iTl.iY+=iTopExtension;

	if (iBottomExtension>0)
		aRect.iBr.iY+=iBottomExtension;
	else
		aRect.iBr.iY-=iBottomExtension;
	}

#endif
