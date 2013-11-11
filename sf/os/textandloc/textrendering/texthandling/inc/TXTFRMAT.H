/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __TXTFRMAT_H__
#define __TXTFRMAT_H__

#include <e32std.h>
#include <e32base.h>
#include <gdi.h>


// Classes declared in this file:
class TTabStop;
class TParaBorder;
class TBullet;
class TParaBorderArray;
class CParaFormat;
class TParaFormatMask;
class TFontPresentation;
class TCharFormat;
class TCharFormatMask;

/** 
Provides support for system colours, in addition to literal colours, in 
text formatting.

The base class TRgb stores the 24-bit literal colour value using a TUint32. 
TLogicalRgb uses the MSB from iValue2 data member as an 8-bit index. The 
purpose of the  index is to allow applications to use logical colours. If the 
index is zero, the value is not a logical colour; it is treated as an ordinary 
TRgb value. If the index is non zero (1-255), the colour should be translated by the 
application into a system colour. Indices 254 and 255 are reserved for the 
system foreground and background colours, respectively and should be translated 
into them. Translation from index to RGB colour occurs in the implementation of 
MFormParam::SystemColor().

All colours in the Text and Text Attributes API are stored using TLogicalRgb 
values and are initialised to either TLogicalRgb::ESystemForegroundColor or 
TLogicalRgb::ESystemBackgroundColor.

This system allows an application to set its text colours once, perhaps using 
the system-wide colour scheme, rather than having to set the colours each 
time a text object is created. It is also compatible with legacy code which 
expects TRgb rather than TLogicalRgb values: for example, the logical 
foreground and background colours have their bottom three bytes filled with 
black and white respectively so that code which expects TRgb values can still 
use them. 
@publishedAll
@released
*/
class TLogicalRgb : public TRgb
	{
	public:
 
	/*
	Reserved colour indices for default foreground and background colours,
	and colours for the selection highlight. The top 128 indices (128-255)
	are reserved for future expansion, but the first 126 non-zero indices
	(1-127) can be used by the GUI as convenient.
	*/ 
	enum
		{
		ESystemSelectionForegroundIndex = 252,
		ESystemSelectionBackgroundIndex = 253,
		/** Index reserved for the system foreground colour (=254). */
		ESystemForegroundIndex = 254,
		/** Index reserved for the system background colour (=255). */
		ESystemBackgroundIndex = 255
		};

	
	 
	/** Used to construct TLogicalRgb objects which should use either the 
	system foreground or background colour. */
	enum TSystemColor
		{
		ESystemSelectionForegroundColor = ESystemSelectionForegroundIndex << 24,
		ESystemSelectionBackgroundColor = ESystemSelectionBackgroundIndex << 24,
		/** The system foreground colour. */
		ESystemForegroundColor = ESystemForegroundIndex << 24,
		/** The system background colour. */
		ESystemBackgroundColor = (ESystemBackgroundIndex << 24) | 0xFFFFFF
		};
		 
	/** Constructs a new TLogicalRgb object. */
	TLogicalRgb() : 
		iValue2(0) 
		{ 
		}
	
	/** Constructs the object with a 32-bit integer. The index is stored in the 
	MSB of iValue2 data member. A TRgb value may be stored in the base TRgb class. 
	@param aValue Integer holding the logical colour index. */
	TLogicalRgb(TUint32 aValue): 
		iValue2(aValue & 0xFF000000)
		{ 
		SetInternal((TUint32)aValue | 0xFF000000);
		}
		
	/** Constructs the object with a TSystemColor value.
	@param aValue Identifies whether the colour is the system foreground or 
	system background colour. */
	TLogicalRgb(TSystemColor aValue) : 
		iValue2((TUint32)aValue & 0xFF000000)
		{ 
		SetInternal((TUint32)aValue | 0xFF000000);
		}

	/** Constructs a new TLogicalRgb object from an existing one. */
	TLogicalRgb(const TRgb& aRgb) : 
		TRgb(aRgb),
		iValue2(0)
		{ 
		}
	 
	/** Returns the logical colour's index value. Zero indicates that the value 
	is not a logical colour; it is an ordinary TRgb value. 254 and 255 indicate 
	the system foreground and background colours, respectively.
	@return The index: between zero and 255 inclusive. */
	TUint SystemColorIndex() const 
		{ 
		return iValue2 >> 24; 
		}
		
	/** Sets the logical colour's index value.
	@param aIndex The new index value (between 1 and 253 inclusive). */
	void SetSystemColorIndex(TUint aIndex) 
		{ 
		iValue2 = aIndex << 24; 
		}
	
private:	
	TUint32 iValue2;
	
	};

/** 
Indicates which format attributes are relevant when setting or sensing text 
formatting. 
@publishedAll
@released
*/
enum TTextFormatAttribute
	{
	// Paragraph format attributes.
	/** Language of the paragraph for proofing. */
	EAttParaLanguage,
	/** Background colour of the paragraph. */
	EAttFillColor,
	/** Leading text margin. */
	EAttLeftMargin,
	/** Trailing text margin. */
	EAttRightMargin,
	/** First line leading indent. */
	EAttIndent,
	/** Horizontal alignment of paragraph. */
	EAttAlignment,
	/** Vertical paragraph alignment. */
	EAttVerticalAlignment,
	/** Inter-line spacing. */
	EAttLineSpacing,
	/** Control for EAttLineSpacing. */
	EAttLineSpacingControl,
	/** Space above paragraph. */
	EAttSpaceBefore,
	/** Space below paragraph. */
	EAttSpaceAfter,
	/** Whether a page break can occur within the paragraph. */
	EAttKeepTogether,
	/** Whether a page break can occur between this and the next paragraph. */
	EAttKeepWithNext,
	/** Whether a page break should be inserted before this paragraph. */
	EAttStartNewPage,
	/** Whether the last line of a paragraph can appear by itself at the top of a new 
	page, (widow), or the first line of a paragraph can appear by itself at the 
	bottom of the page, (orphan). */
	EAttWidowOrphan,
	/** Whether the paragraph should line wrap at the right margin. */
	EAttWrap,
	/** Distance between paragraph border and enclosed text. */
	EAttBorderMargin,
	/** Top of paragraph border. */
	EAttTopBorder,
	/** Bottom of paragraph border. */
	EAttBottomBorder,
	/** Left-hand side of paragraph border. */
	EAttLeftBorder,
	/** Right-hand side of paragraph border. */
	EAttRightBorder,
	/** Bullet point associated with paragraph. */
	EAttBullet,
	/** Spacing between default tab stops. */
	EAttDefaultTabWidth,
	/** Tab stop. */
	EAttTabStop,

	// Character format attributes.
	/** Language of individual characters within a paragraph for proofing. */
	EAttCharLanguage,
	/** Text colour. */
	EAttColor,
	/** Text highlight colour. */
	EAttFontHighlightColor,
	/** Text highlight style. */
	EAttFontHighlightStyle,
	/** Font height. */
	EAttFontHeight,
	/** Font posture (i.e. italics). */
	EAttFontPosture,
	/** Font stroke weight (i.e. bold). */
	EAttFontStrokeWeight,
	/** Subscript, superscript or normal print position. */
	EAttFontPrintPos,
	/** Underlining. */
	EAttFontUnderline,
	/** Strikethrough. */
	EAttFontStrikethrough,
	/** The typeface name. */
	EAttFontTypeface,
	/** Vertical picture alignment. */
	EAttFontPictureAlignment,
	/** Hidden text. */
	EAttFontHiddenText,

	/** Used internally to indicate the count of all attributes. */
	ETextFormatAttributeCount
	};

/*
Following enum and variable should not be used by external developers.
*/
enum {EVariableLengthValue = 0};

const TInt KMaxStyleName = 0x20;
const TInt KMaxParaAttributes = EAttTabStop;
const TInt KMaxCharAttributes = EAttFontHiddenText - KMaxParaAttributes;
const TInt KTabNotFound = -1;

/** 
A tab stop.

This is a position on a page used to align columns of text. It has a twips 
position and an alignment. The twips position is the width in twips (1/1440th 
of an inch) of the tab stop, i.e. the number of twips from the start of the 
line at which text can be inserted. It uniquely identifies the tab stop. The 
alignment (left, right, or centre) indicates how text inserted at the tab 
stop should be aligned.

Tab stops are paragraph format attributes. They are owned by the CParaFormat 
class, through which tab stops can be added and removed. 
@publishedAll
@released
*/
class TTabStop
	{
public:
	/** Text alignment at the tab stop. */
	enum TTabType
		{
		/** No tab. */
		ENullTab, 
		/** Text is aligned to the tab stop's leading edge (left for
		left-to-right paragraphs, right for right-to-left paragraphs). */
		ELeftTab, 
		/** Text is aligned to the tab stop's trailing edge (right for
		left-to-right paragraphs, left for right-to-left paragraphs). */
		ECenteredTab, 
		/** Text is right aligned at the tab stop. */
		ERightTab
		};
public:
	IMPORT_C TTabStop();
	IMPORT_C TTabStop(const TTabStop& aTabStop);
	IMPORT_C TTabStop& operator=(const TTabStop& aTabStop);
	IMPORT_C TBool operator==(const TTabStop& aTabStop) const;
	inline TBool operator!=(const TTabStop& aTabStop) const;
public:
	/** The twips position. This is the width in twips of the tab stop, i.e. 
	the number of twips from the start of the line at which text can be 
	inserted. */
	TUint32 iTwipsPosition;
	/** Text alignment at the tab stop. */
	TTabType iType;
	};

/** 
Defines the characteristics of one of the four sides of a paragraph border. 

These are the line style, thickness and colour. Paragraph borders are paragraph 
format attributes. They are owned by the CParaFormat class which allows 
paragraph borders to be added and removed. The CParaFormat::TParaBorderSide 
enumeration identifies which side of the paragraph the object applies to. 
@publishedAll
@released
*/
class TParaBorder
	{
public:
	
	/** Line styles. */
	enum TLineStyle 
		{
		/** No line style. */
		ENullLineStyle, 
		/** Solid line. */
		ESolid, 
		/** Double solid line. */
		EDouble,
		/** Dotted line. */
		EDotted, 
		/** Dashed line. */
		EDashed, 
		/** Alternating dots and dashes. */
		EDotDash, 
		/** Alternating sequence of two dots and a dash. */
		EDotDotDash
		};
	//
	IMPORT_C TParaBorder();
	IMPORT_C TBool operator==(const TParaBorder& aParaBorder) const;
	inline TBool operator!=(const TParaBorder& aParaBorder) const;
public:
	/** The line style. By default, ENullLineStyle. */
	TLineStyle iLineStyle;
	/** The line thickness in twips. By default, zero. */
	TInt iThickness;
	/** The line colour. By default, the system's default foreground colour. */
	TLogicalRgb iColor;
	/** ETrue indicates that the line colour is set to the default or current 
	text colour, overriding iColor. EFalse indicates that the iColor value is 
	used. By default, ETrue. */
	TBool iAutoColor;
	};

/** 
Stores the four sides of a paragraph border.

Paragraph borders sides are set individually using functions provided by class 
CParaFormat. 
@publishedAll
@released
*/
class TParaBorderArray
	{
public:
	TParaBorder iBorder[4];
	};

/** 
A bullet point. 

This is a paragraph format attribute, stored as the iBullet member of class 
CParaFormat.

Bullet points have a typeface, height, colour and a character code (defines 
the symbol displayed). Single level bullets only are supported. Bullets may 
also have a hanging indent. If set, this means that the rest of the paragraph 
following the line containing the bullet point is indented. 
@publishedAll
@released
*/
class TBullet
	{
	public:
	IMPORT_C TBullet();
	IMPORT_C TBool operator ==(const TBullet& aBullet) const;
	inline TBool operator !=(const TBullet& aBullet) const;

	/**
	Identifies the bullet style.

	Note: Styles other than ENullStyle and EBulletStyle are not currently supported.
	They have the same effect as the EBulletStyle.
	*/
	enum TStyle
		{
		/**
		No bullet. Used for style layers that override a bullet with the absence of a bullet.
		*/
		ENullStyle,
		/**
		A bullet point. Character with code 0x2022 is used by default.
		*/
		EBulletStyle,
		EArabicNumberStyle,
		ESmallRomanNumberStyle,
		ECapitalRomanNumberStyle,
		ESmallLetterStyle,
		ECapitalLetterStyle
		};

	/** Paragraph alignment */
	enum TAlignment
		{
		/** Paragraph left aligned. */
		ELeftAlign,
		/** Paragraph centre aligned. */
		ECenterAlign,
		/** Paragraph right aligned. */
		ERightAlign
		};

	/** The Unicode character used to represent the bullet point. By default 
	0x2022. */
	TChar iCharacterCode;		// the bullet or other symbol used if iStyle is EBulletStyle
	/** The height in twips of the font used for the bullet point character. 
	By default, zero. */
	TUint iHeightInTwips;
	/** The typeface used for the bullet point character. */
	TTypeface iTypeface;
	/** ETrue to indent the rest of the paragraph from the bullet point. 
	EFalse to align the bullet point character with the rest of the paragraph. */
	TBool iHangingIndent;
	/** The colour of the bullet point character. By default, the system's 
	default foreground colour. */
	TLogicalRgb iColor;
	TStyle iStyle;				// is this a bullet or a number or a letter?
	TInt iStartNumber;			// the number of the first paragraph in a run of paragraphs in this style
	TAlignment iAlignment;		// alignment of the bullet or number within the margin
	};

/** 
A transient container of paragraph format attributes, including tab stops, 
bullet points and paragraph borders. 

Rich and global text objects store paragraph formatting using paragraph format 
layers (see class CParaFormatLayer). The CParaFormat class is used to store 
the relevant attribute values when setting or sensing a CParaFormatLayer. 
It is normally used in combination with a TParaFormatMask, to specify which 
attributes are relevant to the function concerned.

On construction, all CParaFormat member data is initialised. The attributes 
which are not explicitly set are assigned default values. 
@publishedAll
@released
*/
class CParaFormat: public CBase
	{
public:
	/** Miscellaneous constants. */
	enum
		{
		/** The maximum number of paragraph borders (= 4). */
		EMaxParaBorder = 4
		};

	/** Paragraph border sides */
	enum TParaBorderSide
		{
		/** The border at the top of the paragraph. */
		EParaBorderTop,
		/** The border at the bottom of the paragraph. */
		EParaBorderBottom,
		/** The border on the left hand side. */
		EParaBorderLeft,
		/** The border on the right hand side. */
		EParaBorderRight
		};

	/** Line spacing control */
	enum TLineSpacingControl
		{
		/** Twips line spacing must be at least as wide as the 
		iLineSpacingInTwips value. */
		ELineSpacingAtLeastInTwips,
		/** Twips line spacing must be exactly the iLineSpacingInTwips value. */
		ELineSpacingExactlyInTwips,
		/** Pixels line spacing must be at least as wide as the line spacing 
		value in pixels. */
		ELineSpacingAtLeastInPixels,
		/** Pixels line spacing must be exactly the same as the line spacing 
		value in pixels. */
		ELineSpacingExactlyInPixels
		};

	/** Paragraph alignment */
	enum TAlignment
		{
		/** Paragraph aligned to the leading margin (left for left-to-right
		paragraphs, right for right-to-left paragraphs). */
		ELeftAlign,
		/** Paragraph top aligned. */
		ETopAlign = ELeftAlign,
		/** Paragraph centre aligned. */
		ECenterAlign,
		/** Paragraph aligned to the trailing margin (right for left-to-right
		paragraphs, left for right-to-left paragraphs). */
		ERightAlign,
		/** Paragraph bottom aligned. */
		EBottomAlign = ERightAlign,
		/** Paragraph justified. */
		EJustifiedAlign,
		/** Used by the spreadsheet application. Unlike ETopAlign and 
		EBottomAlign, provides no default implementation. */
		EUnspecifiedAlign,
		/** User-defined paragraph alignment. */
		ECustomAlign,
		/** Absolute left alignment */
		EAbsoluteLeftAlign,
		/** Absolute right alignment */
		EAbsoluteRightAlign
		};

	/** Attribute sense mode */
	enum TParaFormatGetMode
		{
		/** Indicates that all paragraph format attributes are written to the 
		result when sensing paragraph format attributes. */
		EAllAttributes,
		/** Indicates that tabs, bullets and borders are not sensed. */
		EFixedAttributes
		};

	IMPORT_C static CParaFormat* NewL();
	IMPORT_C static CParaFormat* NewLC();
	IMPORT_C static CParaFormat* NewL(const CParaFormat& aFormat);
	IMPORT_C CParaFormat();
	IMPORT_C ~CParaFormat();
	IMPORT_C void ResetNonDestructive();  // preserves any allocated tabs, bullets or borders.
	IMPORT_C void Reset();  // full reset, deletes and nulls any allocated tabs, bullets or borders.
	IMPORT_C void CopyL(const CParaFormat& aFormat, const TParaFormatMask& aMask);
	IMPORT_C void CopyL(const CParaFormat& aFormat);
	IMPORT_C void Strip();  // Cleans up this paragraph format.
	IMPORT_C TBool IsEqual(const CParaFormat& aFormat, const TParaFormatMask& aMask) const;
	IMPORT_C TBool IsEqual(const CParaFormat& aFormat) const;
	IMPORT_C void StoreTabL(const TTabStop& aTabStop);
	IMPORT_C void RemoveTab(TInt aTabTwipsPosition);
	inline void RemoveAllTabs();
	IMPORT_C const TTabStop TabStop(TInt aTabIndex) const;
	inline TInt TabCount() const;
	IMPORT_C TInt LocateTab(TInt aTabTwipsPosition) const;
	IMPORT_C void SetParaBorderL(TParaBorderSide aSide, const TParaBorder& aBorder);  // Overwrites any existing border for that side
	IMPORT_C void RemoveAllBorders();
	IMPORT_C const TParaBorder ParaBorder(TParaBorderSide aSide) const;

	/** Tests whether any paragraph borders have been set.
	
	@return ETrue if any paragraph borders have been set, EFalse if not. */
	inline TBool BordersPresent() const { return iParaBorderArray != NULL;}
	
    inline TParaBorder* ParaBorderPtr(TParaBorderSide aSide)

	/** Gets a pointer to the paragraph border on the side specified. If no 
	paragraph border array has been allocated, returns NULL.
	
	@param aSide The side for the paragraph border. 
	@return Pointer to the paragraph border on the specified side. */
		{ return iParaBorderArray ? &iParaBorderArray->iBorder[aSide] : NULL; }
	IMPORT_C TBool AllBordersEqual(const CParaFormat& aFormat) const;
	IMPORT_C TBool IsBorderEqual(TParaBorderSide aSide, const CParaFormat& aFormat) const;
private:
	CParaFormat(const CParaFormat& aFormat);
	void CreateTabListL();
	enum
		{
		ETabStoreGranularity = 2
		};

	CParaFormat& operator=(const CParaFormat& aParaFormat);	// intentionally unimplemented
private:
	CArrayFixFlat<TTabStop>* iTabList;			// ordered list of tab stops; null if none
	TParaBorderArray* iParaBorderArray;			// array of paragraph borders; null if none
public:
	/** The background colour of the paragraph. By default the default system 
	background colour. This colour applies to the area bounded by the paragraph 
	border, if one exists. */
	TLogicalRgb iFillColor;
	/** The language of the paragraph for proofing. By default 
	KParaDefaultLanguage. Used for example when spell checking a document 
	which contains text in more than one language, so that the program 
	recognises the text as being in another language. */
	TInt32 iLanguage;						
	/** The width in twips of the leading margin (left for left-to-right
	paragraphs, right for right-to-left paragraphs). By default
	KParaDefaultLeftMargin (zero). */
	TInt32 iLeftMarginInTwips;
	/** The width in twips of the trailing margin (right for left-to-right
	paragraphs, left for right-to-left paragraphs). By default
	KParaDefaultRightMargin (zero). */
	TInt32 iRightMarginInTwips;
	/** An indent for the first line in the paragraph, relative to the leading
	margin (left for left-to-right paragraphs, right for right-to-left
	paragraphs). By default KParaDefaultIndent (zero). */
	TInt32 iIndentInTwips;
	/** Horizontal alignment of paragraph. By default KParaDefaultHorizAlign 
	(left). */
	TAlignment iHorizontalAlignment;
	/** Vertical alignment of paragraph, (intended for use by spreadsheet 
	applications). 	By default KParaDefaultVertAlign (unspecified). */
	TAlignment iVerticalAlignment;
	/** Inter-line spacing within the paragraph, in twips. By default 
	KParaDefaultLineSpacing (200 twips). */
	TInt32 iLineSpacingInTwips;	// distance between successive baselines
	/** Control for the iLineSpacingInTwips value. By default, 
	KParaDefaultLineSpacingControl 	(ELineSpacingAtLeastInTwips). */
	TLineSpacingControl iLineSpacingControl;	// whether iLineSpacingInTwips means 'at least' or 'exactly'
	/** Space above paragraph. By default KParaDefaultSpaceBefore (zero). */
	TInt32 iSpaceBeforeInTwips;	
	/** Space below paragraph. By default KParaDefaultSpaceAfter (zero). */
	TInt32 iSpaceAfterInTwips;	
	/** Prevents a page break within paragraph if ETrue. By default 
	KParaDefaultKeepTogether (EFalse). */
	TBool iKeepTogether;	
	/** Prevents a page break between this paragraph and the following 
	paragraph if ETrue. By default, KParaDefaultKeepWithNext (EFalse). */
	TBool iKeepWithNext;	
	/** Inserts a page break immediately before this paragraph if ETrue. 
	By default, KParaDefaultStartNewPage (EFalse). */
	TBool iStartNewPage;		
	/** Prevents the printing of the last line of a paragraph at the top 
	of the page (referred to as a widow), or the first line of a paragraph 
	at the bottom of the page, (referred to as an orphan). By default, 
	KParaDefaultWidowOrphan (EFalse). */
	TBool iWidowOrphan;	
	/** Specifies whether the paragraph should line wrap at the right margin. 
	By default KParaDefaultWrap (ETrue). */
	TBool iWrap;
	/** Distance in twips between the paragraph border and the enclosed text. 
	By default KParaDefaultBorderMargin (zero). */
	TInt32 iBorderMarginInTwips;	
	/** The bullet point associated with the paragraph. A NULL value indicates 
	no bullet point. By default NULL. */
	TBullet* iBullet;		
	/** Specifies the default tab stop width. By default KParaDefaultTabWidth 
	(360 twips). */
	TUint32 iDefaultTabWidthInTwips;
	};

/** 
Masks the paragraph format attributes which are involved when setting and 
sensing paragraph formatting. 

Used in conjunction with an object of class CParaFormat. When setting formatting, 
only the attributes which are set in the mask will participate in the relevant 
function. When sensing formatting, on return, the mask indicates which attributes 
were sensed from the format layer, and were not taken from the default values. 
@publishedAll
@released
*/
class TParaFormatMask
	{
public:
	inline TParaFormatMask();
	inline void SetAttrib(TTextFormatAttribute aAttribute);
	inline void ClearAttrib(TTextFormatAttribute aAttribute);
	IMPORT_C void SetAll();
	IMPORT_C void ClearAll();
	inline TBool AttribIsSet(TTextFormatAttribute aAttribute) const;
	inline TBool IsNull() const;
	IMPORT_C TBool operator==(const TParaFormatMask& aMask) const;
	inline TBool operator!=(const TParaFormatMask& aMask) const;

private:
	TUint32 iGuard;
	};

/** 
Specifies the font-independent character format attributes, including bold, 
italics and underlining.

An instance of this class is owned by the character formatting container (class 
TCharFormat). 
@publishedAll
@released
*/
class TFontPresentation
	{
public:
		
	/** Highlight style */
	enum TFontHighlightStyle
		{
		/** No highlighting used. */
		EFontHighlightNone,
		/** Normal (square cornered) highlighting used. */
		EFontHighlightNormal,
		/** Rounded corner highlighting used. */
		EFontHighlightRounded,
		/** Text is drawn offset towards the bottom-right in the highlight 
		colour, (iHighlightColor) before being drawn again in the text colour, 
		(iTextColor) creating a shadow effect. */
		EFontHighlightShadow,
		/** Placeholder for "unrecognised word" highlighting style for FEPs */
		EFontHighlightNoMatchesIndicator,
		/** First custom highlighting style is used.
		@see MFormCustomDraw::DrawText(). */
		EFontHighlightFirstCustomStyle = 128,
		/** Second custom highlighting style is used. 
		@see MFormCustomDraw::DrawText(). */
		EFontHighlightLastCustomStyle = 255
		};
	
	
	/** Vertical picture alignment */
	enum TAlignment
		{
		/** The top of the picture is aligned flush with the top of the font's 
		ascent, so that the picture may descend below the line. */
		EAlignTop,
		/** The bottom of the picture is aligned flush with the bottom of the 
		font's descent so that the picture may extend above the line. */
		EAlignBottom,
		/** The picture is aligned so that its centre is positioned at the 
		baseline of the line. */
		EAlignCentered,
		/** The bottom of the picture is aligned with the baseline of the font.
		This is the default. */
		EAlignBaseLine
		};
	//
	IMPORT_C TFontPresentation();
	//
	// Enquiry function
	IMPORT_C TBool IsEqual(const TFontPresentation& aFontPresentation, const TCharFormatMask& aMask) const;
public:
	/** The text colour. By default, the default system foreground colour. */
	TLogicalRgb iTextColor;
	/** The highlight colour for selected text. Only takes effect if 
	iHighlightStyle is not EFontHighlightNone. By default, the default system 
	foreground colour. */
	TLogicalRgb iHighlightColor;  // Background color
	/** Style for character highlighting. By default EFontHighlightNone. */
	TFontHighlightStyle iHighlightStyle;
	/** The value of the strikethrough attribute. By default EStrikethroughOff. */
	TFontStrikethrough iStrikethrough;
	/** The value of the underline attribute. By default EUnderlineOff. */
	TFontUnderline iUnderline;
	/** Specifies whether or not text is hidden. Note that hidden text is not 
	currently supported by the text layout engine. This attribute is provided 
	to preserve information when copying from and to devices which support 
	hidden text. By default EFalse. */
	TBool iHiddenText;
	/** The vertical alignment of a picture character. By default 
	EAlignBaseLine. */
	TAlignment iPictureAlignment;
	};

/** 
A transient container of character format attributes, including 
font-dependent and font-independent attributes. 

The font-independent attributes are stored in a TFontPresentation object. 
Rich and global text objects store character formatting using character format 
layers (see class CCharFormatLayer). The TCharFormat class is used to store 
the relevant attribute values when setting or sensing a CCharFormatLayer. 
It is normally used in combination with a TCharFormatMask, to specify which 
attributes are relevant to the function concerned. 
@publishedAll
@released
*/
class TCharFormat
	{
public:
	IMPORT_C TCharFormat();
	IMPORT_C TCharFormat(const TDesC &aTypefaceName, TInt aHeight);
	//
	// Enquiry functions
	IMPORT_C TBool IsEqual(const TCharFormat& aFormat, const TCharFormatMask& aMask) const;
	IMPORT_C TBool IsEqual(const TCharFormat& aFormat) const;
public:
	/** Specifies the language of individual characters for proofing. Used for 
	example when spell checking a document which contains text in more than one 
	language, so that the program recognises the text as being in another 
	language. Language is also a paragraph format attribute. If the language 
	setting of a character is different from the language setting of the 
	containing paragraph, the character's setting takes precedence. */
	TInt32 iLanguage;
	/** Font independent character format attributes. */
	TFontPresentation iFontPresentation; 
	/** Device independent font specification. */
	TFontSpec iFontSpec;  
	};

/** 
Masks the character format attributes which are involved when setting and 
sensing character formatting.

Used in conjunction with an object of class TCharFormat.

When setting formatting, only the attributes which are set in the mask should 
participate in the relevant function. When sensing formatting, on return, 
the mask indicates which attributes were sensed from the format layer, and 
were not taken from the default values. 
@publishedAll
@released
*/
class TCharFormatMask
	{
public:
	inline TCharFormatMask();
	inline void SetAttrib(TTextFormatAttribute aAttribute);
	inline void ClearAttrib(TTextFormatAttribute aAttribute);
	inline TBool AttribIsSet(TTextFormatAttribute aAttribute) const;
	IMPORT_C void SetAll();
	IMPORT_C void ClearAll();
	inline TBool IsNull()const;
	IMPORT_C TBool operator==(const TCharFormatMask& aMask) const;
	inline TBool operator!=(const TCharFormatMask& aMask) const;

private:
	TUint32 iGuard;
	};

#include <txtfrmat.inl>

#endif
