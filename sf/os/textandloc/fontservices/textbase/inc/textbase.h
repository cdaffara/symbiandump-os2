// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// This file is a subset of original gdi.h.
//
// IMPORTANT NOTE to maintainer:
//     If you add new exports, to make sure they are exported by GDI.DLL,
//     remember to update /graphics/graphicsdeviceinterface/gdi/sgdi/TextBaseStub.cpp.
//
// Text related types/consts are moved from gdi.h to this file, as listed below.
//
// enum TFontPosture
// enum TFontStrokeWeight
// enum TFontPrintPosition
//
// const KMaxTypefaceNameLength
// class TTypeface
// enum TGlyphBitmapType
// class FontEffect
// class TFontStyle
// class TFontSpec
// class TTypefaceSupport
// const KSuperSubScalingPercentage
// const KSuperscriptOffsetPercentage
// const KSubscriptOffsetPercentage
// class CTypefaceStore
// const KMaxFontCacheEntries
// class CFontCache
//
// const KFontCapitalAscent
// const KFontMaxAscent
// const KFontStandardDescent
// const KFontMaxDescent
// const KFontLineGap
// const KFontGetFontTable
// const KFontGetGlyphOutline
// const KFontReleaseGlyphOutline
// const KFontReleaseFontTable
// class CFont


#ifndef __TEXTBASE_H__
#define __TEXTBASE_H__

#include <e32base.h>
#include <f32file.h>
#include <s32std.h>

class TOpenFontCharMetrics;
class RShapeInfo;


#ifndef __GLYPHSEL_TEXTBASE_H__
#define __GLYPHSEL_TEXTBASE_H__
/**
This enumeration holds the possible panic codes that may be raised 
by the TextBase API on detecting an unrecoverable error. */
enum TTextBasePanic
	{
	/** Not used */
	ETextBasePanic_Unknown				= 0,
	/** One or more of the input parameters to the interface were invalid */
	ETextBasePanic_InvalidInputParam		= 1,
	/** Insufficient text for successful completion of the method */
	ETextBasePanic_OutOfText				= 2,
	/** Internal failure. */
	ETextBasePanic_Invariant				= 3,
	/** Reserved panic codes. Not to be used. */
	ETextBasePanic_Reserved1				= 4,
	ETextBasePanic_Reserved2				= 5,
	/** Setting a typeface name that is too long */
	ETextBasePanic_TypefaceNameOverflow	= 6,
	};
#endif


/**
Font posture flags.
Fonts can be either upright or italic. 
@publishedAll	
@released
*/
enum TFontPosture
	{
	/** Font posture is normal (upright). */
	EPostureUpright,
	/** Font posture is italic. */
	EPostureItalic
	};


/**
Font stroke weight flags. 
@publishedAll	
@released
*/
enum TFontStrokeWeight
	{
	/** Font stroke weight is normal. */
	EStrokeWeightNormal,
	/** Font stroke weight is bold. */
	EStrokeWeightBold
	};

/**
Font print position flags.
Fonts can be normal, superscript or subscript. 
@publishedAll	
@released
*/
enum TFontPrintPosition
	{
	/** Font is normal. */
	EPrintPosNormal,
	/** Font is superscript. */
	EPrintPosSuperscript,
	/** Font is subscript. */
	EPrintPosSubscript
	};


/**
The maximum length of a typeface name (in characters). 
@publishedAll	
@released
*/
const TInt KMaxTypefaceNameLength=0x18;


/** Typeface name and attributes.

This class identifies a typeface by name, and contains the combination of 
attributes of the typeface. These attributes define whether it is a symbol 
typeface, whether the typeface is proportional, and whether it is serif or 
sans-serif. 

The combination of attributes for a typeface are stored in a bitmask, with 
the various bits indicating different attributes. The bitmask is calculated 
for any particular attribute combination by ORing the enumerated value for 
each individual attribute. 
@publishedAll 
@released
*/
class TTypeface
    {
public:
	enum
		{
		/** Typeface is a proportional typeface (e.g. Swiss)
		*/
		EProportional = 1,
		/** Typeface is a serif typeface (e.g. Times)
		*/
		ESerif = 2,
		/** Typeface is a symbol typeface (e.g. Symbol)
		*/
		ESymbol = 4,
		};
public:
	IMPORT_C TTypeface();
	IMPORT_C TBool operator==(const TTypeface& aTypeface) const;
	IMPORT_C void InternalizeL(RReadStream& aStream);
	IMPORT_C void ExternalizeL(RWriteStream& aStream) const;
	IMPORT_C void SetAttributes(TInt aAttributes);
	IMPORT_C void SetIsProportional(TBool aIsProportional);
	IMPORT_C void SetIsSerif(TBool aIsSerif);
	IMPORT_C void SetIsSymbol(TBool aIsSymbol);
	IMPORT_C TInt Attributes() const;
	IMPORT_C TBool IsProportional() const;
	IMPORT_C TBool IsSerif() const;
	IMPORT_C TBool IsSymbol() const;
	IMPORT_C void SetScriptTypeForMetrics(TLanguage aLanguage);
	IMPORT_C void SetScriptTypeForMetrics(TInt aScript);
	IMPORT_C TInt ScriptTypeForMetrics() const;
	IMPORT_C void SetName(const TDesC& aName);
	IMPORT_C const TDesC& Name() const;
private:
	void ResetAttributes();
	void ResetScriptType();
public:
	/** The typeface name. */
    TBufC<KMaxTypefaceNameLength> iName;
private:
    TUint32 iFlags;
    };


/**
An enumerated type for the format of a glyph bitmap. This type is currently 
used to indicate whether glyph bitmaps for scalable fonts are drawn anti-aliased. 
Additional values may be defined in the future.

@see TFontStyle::SetBitmapType()
@see CFbsTypefaceStore::SetDefaultBitmapType() 
@publishedAll
@released	
*/
enum TGlyphBitmapType
	{
	/** The font store's default glyph bitmap format is used. */
	EDefaultGlyphBitmap = 0,
	/** The standard monochrome format: no anti-aliasing, 1 bit per pixel, 
	run-length encoded. */
	EMonochromeGlyphBitmap,
	/** Standard 8-bits-per-pixel with anti-aliasing. */
	EAntiAliasedGlyphBitmap,
	/** The format used when sub-pixel font rendering is used. */
	ESubPixelGlyphBitmap,
	/** The format used when outline and shadow font rendering is used. 

	If the raterizer supports the outline and shadow fonts, it will set the bitmaptype as 
	EFourColourBlendGlyphBitmap but only when glyph bitmap type is set as EAntiAliasedGlyphBitmap and 
	when any of the EDropShadow or EOutline effect is on. Only rasterizer providers can use this enum.
	*/
	EFourColourBlendGlyphBitmap,
	/**
	This is used for glyphs, and not fonts, and is needed to inform the font drawing routines
	that the character should be drawn using the overall font setting. 
	For Internal Use Only.
	*/
	EGlyphBitmapTypeNotDefined,
	/**
	This is used to inform the rasterizer that the best match should be
	found for the bitmap type based upon its knowledge.
	For Internal Use Only.
	*/
	EAntiAliasedOrMonochromeGlyphBitmap,
	};

/**
Defines a set of font effects flags.

@publishedAll 
@released
WARNING: This Class is for use by system/UI software ONLY.
*/
NONSHARABLE_CLASS(FontEffect)
	{
public:
	enum TEffect
		{
		ENone			= 0x0,		// No effects.
		EAlgorithmicBold= 0x10,		// Font is algorithmic bold (a.k.a pseudo bold.)
		EDropShadow		= 0x20,		// Font has a drop shadow.
		EOutline		= 0x40,		// Font is an outline font.
		EEmbossed		= 0x80,		// Font is embossed.
		EEngraved		= 0x100,	// Font is engraved.
		ESoftEdge		= 0x200,	// Font is soft edged.
		EReserved1		= 0x400,	// Reserved for Symbian use.
		EReserved2		= 0x800,	// Reserved for Symbian use.
		EReserved3		= 0x1000,	// Reserved for Symbian use.
		EReserved4		= 0x2000,	// Reserved for Symbian use.
		EReserved5		= 0x4000,	// Reserved for Symbian use.
		EReserved6		= 0x8000,	// Reserved for Symbian use.
		};
public:
	IMPORT_C static TBool IsEffectOn(TEffect aEffect, TUint32 aFontEffect);
	IMPORT_C static void SetEffect(TEffect aEffect, TBool aOn, TUint32& aFontEffect);
	};

/** Encapsulates a font style. 

The font style information is comprised of:

the posture of the font upright or italic

the stroke weight of the font  normal or bold

the print position of the font normal, subscript or superscript

Note that the underline and strike-through attributes are not included in 
this class, but are set in the graphics context.

@see CGraphicsContext::SetUnderlineStyle()
@see CGraphicsContext::SetStrikethroughStyle() 
@publishedAll
@released
*/
class TFontStyle
	{
public:
	IMPORT_C TFontStyle();
	IMPORT_C TFontStyle(TFontPosture aPost,TFontStrokeWeight aStrWgt,TFontPrintPosition aPrintPos);
	IMPORT_C void InternalizeL(RReadStream& aStream);
	IMPORT_C void ExternalizeL(RWriteStream& aStream) const;
	IMPORT_C TFontPosture Posture() const;
	IMPORT_C TFontStrokeWeight StrokeWeight() const;
	IMPORT_C TFontPrintPosition PrintPosition() const;
	IMPORT_C void SetPosture(TFontPosture aPosture);
	IMPORT_C void SetStrokeWeight(TFontStrokeWeight aStrokeWeight);
	IMPORT_C void SetPrintPosition(TFontPrintPosition aPrintPosition);
	inline TGlyphBitmapType BitmapType() const;
	inline void SetBitmapType(TGlyphBitmapType aBitmapType);
	IMPORT_C TBool operator==(const TFontStyle& aFontStyle) const;
	IMPORT_C TUint32 Effects() const;
	IMPORT_C TBool IsEffectOn(FontEffect::TEffect aEffect) const;
	IMPORT_C void SetEffects(TUint32 aEffects);
	IMPORT_C void SetEffects(FontEffect::TEffect aEffect, TBool aOn);
private:
	enum
		{
		EItalic=0x1,
		EBold=0x2,
		ESuper=0x4,
		ESub=0x8
		};
private:
	TUint32 iFlags; // bitmap type - 16 bits (high), font effects - 12 bits (middle), style - 4 bits (low)
	TAny* iReserved1;
	TAny* iReserved2;
	};



/**
Specifies the font specification in device independent terms.
@publishedAll 
@released
*/
class TFontSpec
	{
public:
	IMPORT_C TFontSpec();
	IMPORT_C TFontSpec(const TDesC& aTypefaceName,TInt aHeight);
	IMPORT_C TBool operator==(const TFontSpec& aFontSpec) const;
	IMPORT_C void InternalizeL(RReadStream& aStream);
	IMPORT_C void ExternalizeL(RWriteStream& aStream) const;
	IMPORT_C void SetScriptTypeForMetrics(TLanguage aLanguage);
	IMPORT_C TInt ScriptTypeForMetrics() const;
public:
	/** The typeface. */
	TTypeface iTypeface;
	/** The height of the typeface (in twips). */
	TInt iHeight;
	/** The font style of the typeface. */
	TFontStyle iFontStyle;
	};



/** Typeface family support information.

This data-only class includes the name and attributes of a typeface, how many 
font heights are available, its minimum and maximum heights, and whether or 
not it is scaleable  a typeface is scaleable if it supports heights at 
fixed intervals between the minimum and maximum heights. 
@publishedAll 
@released
*/
class TTypefaceSupport
    {
public:
	/** The name and attributes of the typeface. */
    TTypeface iTypeface;
	/** The number of distinct font heights available in the typeface. */
    TInt iNumHeights;
	/** The typeface's minimum font height, in twips. */
    TInt iMinHeightInTwips;
	/** The typeface's maximum font height, in twips. */
    TInt iMaxHeightInTwips;
	/** Whether the typeface is scaleable. ETrue if it is scaleable, otherwise 
	EFalse. */
    TBool iIsScalable; // supports heights from min to max at fixed interval
    };

/**
The percentage used to multiply a normal font height when calculating its 
superscript or subscript height. 
@publishedAll	
@released
*/
const TInt KSuperSubScalingPercentage=67;

/**
The percentage of a font height used to calculate its baseline offset for a 
superscript print position. 
@publishedAll	
@released
*/
const TInt KSuperscriptOffsetPercentage=-28;

/**
The percentage of a font height used to calculate its baseline offset for a 
subscript print position. 
@publishedAll	
@released
*/
const TInt KSubscriptOffsetPercentage=14;

class CFont;

/** Typeface store abstract base interface.

This class provides the interface to a store for typefaces.

See also CFontStore. 
@publishedAll
@released
*/
class CTypefaceStore : public CBase
	{
public:
	IMPORT_C ~CTypefaceStore();

	/**
	Gets the font which is the nearest to the given font specification.

	When the font is no longer needed, call @c ReleaseFont().

	Note that this deprecated function is replaced by the new @c GetNearestFontToDesignHeightInTwips() 
	yielding (virtually) the same result. However clients are strongly encouraged to use the new
	@c GetNearestFontToMaxHeightInTwips() function instead. This will guarantee that every 
	character within any given text string will fit within the given amount of twips, whereas the design 
	height is an aesthetic unit decided by the font designer without strict physical meaning, which 
	may result in cropped characters.

	@param aFont On return, contains a pointer to the nearest font.
	@param aFontSpec The specification of the font to be matched.
	@return KErrNone if successful; a system-wide error code otherwise.
	@publishedAll
	@deprecated Use GetNearestFontToDesignHeightInTwips
	*/
	virtual TInt GetNearestFontInTwips(CFont*& aFont, const TFontSpec& aFontSpec) = 0;

	/**
	Gets the font which is the nearest to the given font specification.
	
	When the font is no longer needed, call @c ReleaseFont().

	This new function replaces the deprecated @c GetNearestFontInTwips() yielding (virtually) the 
	same result. However clients are strongly encouraged to use the new
	@c GetNearestFontToMaxHeightInTwips() function instead. This will guarantee that every 
	character within any given text string will fit within the given amount of twips, whereas the design 
	height is an aesthetic unit decided by the font designer without strict physical meaning, which 
	may result in cropped characters.

	@param aFont On return, contains a pointer to the nearest font.
	@param aFontSpec The specification of the font to be matched.
	@return KErrNone if successful; a system-wide error code otherwise.
	@publishedAll
	@released
	*/
	virtual TInt GetNearestFontToDesignHeightInTwips(CFont*& aFont, const TFontSpec& aFontSpec) = 0;
	
	/**
	Gets the font which is the nearest to the given font specification.
	
	When the font is no longer needed, call @c ReleaseFont().

	The font and bitmap server returns a pointer to the nearest matching font 
	from those available. Matches to max height of font - this does its best 
	to return a font that will fit within the maximum height specified (but 
	note that variations due to hinting algorithms may rarely result in this 
	height being exceeded by up to one pixel). Problems can also be 
	encountered with bitmap fonts where the typeface exists but doesn't have 
	a font small enough.

	@param aFont On return, contains a pointer to the nearest font.
	@param aFontSpec The specification of the font to be matched.
	@param aMaxHeight The maximum height within which the font must fit.
	@return KErrNone if successful; a system-wide error code otherwise.
	@publishedAll
	@released
	*/
	virtual TInt GetNearestFontToMaxHeightInTwips(CFont*& aFont, const TFontSpec& aFontSpec, TInt aMaxHeight) = 0;

	/** Gets the number of typefaces supported by the typeface store.
	
	@return The number of supported typefaces. */
	virtual TInt NumTypefaces() const=0;

	/** Gets typeface information for a specified typeface index.
 
 	This information is returned in aTypefaceSupport, and
	includes the typeface name and typeface attributes, the number of font
 	heights, the maximum and minimum font heights, and whether it is a
	scaleable typeface.
 
 	@param aTypefaceSupport On return, if the function executed successfully, 
 	this object contains the typeface information.
  	@param aTypefaceIndex A typeface index number, in the range: zero to
             (NumTypefaces() - 1). */
	virtual void TypefaceSupport(TTypefaceSupport& aTypefaceSupport,TInt aTypefaceIndex) const=0;

	/** Gets the height of the font with specified height and typeface indices, 
	in twips.
	
	The value returned is rounded up or down to the nearest font height in twips.
	
	@param aTypefaceIndex A typeface index number, in the range: 0 to 
	(NumTypefaces() - 1). 
	@param aHeightIndex A font height index number.
	@return The height of the font, in twips. */
	virtual TInt FontHeightInTwips(TInt aTypefaceIndex,TInt aHeightIndex) const=0;
	IMPORT_C void ReleaseFont(CFont* aFont);
	IMPORT_C static TInt BaselineOffset(TInt aHeight,TFontPrintPosition aPos);
	IMPORT_C static TInt SuperSubHeight(TInt aHeight,TFontPrintPosition aPos);
protected:
	IMPORT_C CTypefaceStore();
	IMPORT_C void ConstructL();
	IMPORT_C void AddFontL(CFont* aFont);
	IMPORT_C TBool IncrementFontCount(const CFont* aFont);
private:
	TBool FindFont(const CFont* aFont, TInt& aIdx) const;
	NONSHARABLE_CLASS(TFontAccess)
	/**
	Pairs a font with a count of how many clients of the typeface store 
	are accessing that font. 
    */
		{
	public:
		/** A device specific font. */
		CFont* iFont;
		/** The number of clients accessing the font. */
		TInt iAccessCount;
		};
protected:
	/** A list of fonts accessed by clients of the typeface store, which pairs 
	a font with a count of the number of clients accessing the font.
	
	Implemented as an array of TFontAccess objects.
	
	An object is added to this array for every font accessed. If the font is 
	released by all clients, and the access count drops to zero, the font is 
	removed from the list. */
	CArrayFixFlat<TFontAccess>* iFontAccess;
	};

/** The maximum number of entries in the font cache.

@see CFontCache */
const TInt KMaxFontCacheEntries=32;

/** Font cache. 

When a CFont* needs to be found for a particular TFontSpec, the cache can 
be searched to see if the TFontSpec is already in the cache. If the TFontSpec 
is in the cache, its corresponding CFont* can be returned. Otherwise 
GetNearestFontInTwips() must be used to search all of the available fonts for 
the nearest CFont- a procedure which takes much longer than a simple cache
search.

The current font cache should be destroyed and a new cache created whenever 
the zoom factor or device map changes, as these changes break the relation 
between CFont and TFontSpec. 
@publishedAll
@released
*/
class CFontCache : public CBase
	{
public:
	IMPORT_C CFontCache();
	IMPORT_C CFontCache(TInt aMaxEntries);
	IMPORT_C ~CFontCache();
	IMPORT_C CFont* Search(const TFontSpec& aFontSpec);
	IMPORT_C CFont* AddEntryL(CFont* aFont,const TFontSpec& aFontSpec);
	IMPORT_C CFont* RemoveFirstEntry();
public:
	/** The number of cache hits since the font cache was created i.e. 
	successful results from CFontCache::Search(). */
	TInt iNumHits;
	/** The number of cache misses since the font cache was created i.e. 
	unsuccessful results from CFontCache::Search(). */
	TInt iNumMisses;
private:
	class CFontCacheEntry : public CBase
		{
	public:
		CFontCacheEntry(CFont* aFont,const TFontSpec& aFontSpec,CFontCacheEntry* aNext);
	public:
		CFont* iFont;
		TFontSpec iSpec;
		CFontCacheEntry* iNext;
		};
private:
	TInt iNumEntries;
	TInt iMaxEntries;
	CFontCacheEntry* iFirst;
	};

/**
WARNING: this Class is for internal use ONLY.  Compatibility is not guaranteed in future releases.
UIDs corresponding to the CFont API extension functions
@internalTechnology
*/
const TUid KFontCapitalAscent	= {0x1020498E};
const TUid KFontMaxAscent		= {0x10204B10};
const TUid KFontStandardDescent	= {0x10204B11};
const TUid KFontMaxDescent		= {0x10205AFC};
const TUid KFontLineGap			= {0x10204B12};
const TUid KFontGetFontTable    = {0x102872C1};
const TUid KFontGetGlyphOutline = {0x102872C2};
const TUid KFontReleaseGlyphOutline = {0x2002A1FD};
const TUid KFontReleaseFontTable = {0x2002AC24};


/** Abstract font interface.

The CFont class provides a device-independent interface to a device-dependent 
font usually obtained from a call to GetNearestFont...() on a graphics device. 
It is used as a handle in CGraphicsContext::UseFont() and to obtain 
device-dependent information about the font - notably the pixel width of a text 
string.

@see CFbsFont
@see CGraphicsContext::UseFont() 
@publishedAll 
@released
*/
class CFont : public CBase
	{
	friend class CTypefaceStore;
public:

	/** Text direction flags.

	This enum is used in TMeasureTextInput and determines whether the text is 
	drawn horizontally or vertically. Note: text is drawn vertically in some 
	styles of Japanese, etc.

	@see TMeasureTextInput */
	enum TTextDirection
		{
	/** Text is drawn horizontally. */
 		/** Text is drawn horizontally. */
   		EHorizontal,
 		/** Text is drawn vertically. */
		EVertical
		};

	/** Complicated parameter block used for contextual glyph selection, 
	ligature creation and diacritic placement when drawing text in complex 
	scripts

	This class declares a constructor, another scoped class, and several other 
	enums. However this class is unlikely to be useful to third party developers.

	@see CFont::GetCharacterPosition()
	@see CFont::GetCharacterPosition2()
	@publishedAll
	@released
	*/
	class TPositionParam
		{
	public:
		/** Standard constructor. */
		TPositionParam():
			iDirection(EHorizontal),
			iFlags(0),
			iPosInText(0),
			iOutputGlyphs(0)
			{
			}

		enum
			{
			EMaxInputChars = 18,	// ligatures cannot be made from more than 18 components
			EMaxOutputGlyphs = 8	// output can consist of up to 8 characters (one base and 7 combining characters)
			};

		/**Flags for TPositionParam::iFlags. */
		enum TFlags
			{
			/** Input text is logically ordered not visually ordered. */
			EFLogicalOrder = 1
			};

		/** Input: Orientation (EHorizontal or EVertical) in which to draw
		the text. */
		TInt16 iDirection;
		/** Input: Flags from TFlags. */
		TUint16 iFlags;
		/** Input: Text containing the characters to be positioned. */
		TPtrC iText;

		/** Input and output: Position within iText to shape. On exit
		it will index the first character not positioned */
		TInt iPosInText;
		/** Input and output: Pen position. */
		TPoint iPen;			

		/** Output of GetCharacterPosition and GetCharacterPosition2.
		@see CFont::GetCharacterPosition
		@see CFont::GetCharacterPosition2
		@publishedAll
		@released */
		class TOutput
			{
		public:
			/** Standard constructor. */
			TOutput() : iBitmapSize(TSize::EUninitialized),
				iBounds(TRect::EUninitialized) {}
			/** Character or glyph code. */
			TUint iCode;			
			/** Bitmap data for the glyph, if available */
			const TUint8* iBitmap;	
			/** Size of the bitmap before algorithmic bolding, size 
			multiplication, etc. */
			TSize iBitmapSize;		
			/** Bitmap bounds relative to the original pen position. */
			TRect iBounds;			
			};

		/** Information about the glyphs that were output. */
		TOutput iOutput[EMaxOutputGlyphs];	
		/** Number of glyphs actually output. */
		TInt iOutputGlyphs;					
		};	


	/** Input parameter block.

	This is optionally used by CFont::MeasureText(), which is the powerful text 
	measurement function underlying all the other text measurement functions.

	@see CFont::MeasureText() 
	@publishedAll
	@released
	*/
	class TMeasureTextInput
		{
	public:
		TMeasureTextInput():
			iStartInputChar(0),
			iEndInputChar(KMaxTInt),
			iDirection(EHorizontal),
			iFlags(0),
			iMaxAdvance(KMaxTInt),
			iMaxBounds(KMaxTInt),
			iCharJustNum(0),
			iCharJustExcess(0),
			iWordJustNum(0),
			iWordJustExcess(0)
			{
			}

		/**Flags for TMeasureTextInput::iFlags. */
		enum TFlags
			{
 			/** Input text is visually ordered left-to-right. */
 			EFVisualOrder = 1,
 			/** Input text is visually ordered right-to-left.
 			Overrides EFVisualOrder. */
 			EFVisualOrderRightToLeft = 2,
 			/** Flag to consider side bearings when checking bounds for line-break */
 			EFIncludePenPositionInBoundsCheck = 4
			};

		/** Starting index specifying first input character in iText.
	
		Together with iEndInputChar, this allows some context before and 
		after the measured text to be supplied so that shaping can work 
		properly. */
		TInt iStartInputChar;	
	
		/** Index specifying the final input character.
	
		Together with iStartInputChar, this allows some context before and 
		after the measured text to be supplied so that shaping can work 
		properly. */
		TInt iEndInputChar;		
	
		/** The direction in which to draw the text. */
		TUint16 iDirection;
	
		/** Flags from TFlags. */
		TUint16 iFlags;
	
		/** The maximum advance. */
		TInt iMaxAdvance;		
		
		/** The maximum width (or height if drawing vertically) of bounds. */
		TInt iMaxBounds;		
	
		/** The number of glyph groups to be letter-spaced. */
		TInt iCharJustNum;		
	
		/** The amount of space to be used for letter spacing. */
		TInt iCharJustExcess;	
		
		/** The number of spaces to be used for word spacing. */
		TInt iWordJustNum;		
	
		/** The amount of space to be used for word spacing. */
		TInt iWordJustExcess;	
		};

	/** Output parameter block.

	This is optionally used by CFont::MeasureText(), which is the powerful text 
	measurement function underlying all the other text measurement functions.

	@see CFont::MeasureText() 
	@publishedAll
	@released
	*/
	class TMeasureTextOutput
		{
		public:
		/** The number of input characters that would be drawn.
	
		This may be less than the length of the text if a maximum advance or bounding 
		box size is specified. */
		TInt iChars;			
		/** The number of glyphs that would be drawn. */
		TInt iGlyphs;			
		/** The number of groups that would be drawn.
	
		A group is a base glyph plus one or more combining characters. */
		TInt iGroups;			
		/** The number of word spaces (U+0020) that would be drawn. */
		TInt iSpaces;			
		/** The bounding box of all the glyphs that would be drawn. */
		TRect iBounds;			
		/** The maximum width and height of any glyph. */
		TSize iMaxGlyphSize;	
		};

	/**
	Data availability flags.

	Some fonts like printer fonts may only have width information and can return 
	ECharacterWidthOnly to show this: the text drawing routines in CFont synthesize 
	the rest of the data if necessary.

	@see GetCharacterData() 
	@publishedAll	
	@released
	*/
	enum TCharacterDataAvailability
		{
		/** No font information available. */
		ENoCharacterData,
		/** Width information only is available. */
		ECharacterWidthOnly,
		/** All character data is available. */
		EAllCharacterData
		};

private:
	// virtual functions have been made protected and public non-virtual ones
	// added to convert CFont to a handle-body pattern. SC is kept throught the
	// new functions and BC is kept by keeping the protected functions in the
	// same place in the class, and therefore in the same place in the vtable
	/**
	@publishedAll
	WARNING: Function for internal use ONLY.  Compatibility is not guaranteed in future releases. Please see derived class for implementation.
	*/
	virtual TUid DoTypeUid() const=0;
	/**
	@publishedAll
	WARNING: Function for internal use ONLY.  Compatibility is not guaranteed in future releases. Please see derived class for implementation.
	*/
	virtual TInt DoHeightInPixels() const=0;
	/**
	@publishedAll
	WARNING: Function for internal use ONLY.  Compatibility is not guaranteed in future releases. Please see derived class for implementation.
	*/
	virtual TInt DoAscentInPixels() const=0;
	IMPORT_C virtual TInt DoDescentInPixels() const;
	/**
	@publishedAll
	WARNING: Function for internal use ONLY.  Compatibility is not guaranteed in future releases. Please see derived class for implementation.
	*/
	virtual TInt DoCharWidthInPixels(TChar aChar) const=0;
	/**
	@publishedAll
	WARNING: Function for internal use ONLY.  Compatibility is not guaranteed in future releases. Please see derived class for implementation.
	*/
	virtual TInt DoTextWidthInPixels(const TDesC& aText) const=0;
	/**
	@publishedAll
	WARNING: Function for internal use ONLY.  Compatibility is not guaranteed in future releases. Please see derived class for implementation.
	*/
	virtual TInt DoBaselineOffsetInPixels() const=0;
	/**
	@publishedAll
	WARNING: Function for internal use ONLY.  Compatibility is not guaranteed in future releases. Please see derived class for implementation.
	*/
	virtual TInt DoTextCount(const TDesC& aText,TInt aWidthInPixels) const=0;
	/**
	@publishedAll
	WARNING: Function for internal use ONLY.  Compatibility is not guaranteed in future releases. Please see derived class for implementation.
	*/
	virtual TInt DoTextCount(const TDesC& aText,TInt aWidthInPixels,TInt& aExcessWidthInPixels) const=0;
	/**
	@publishedAll
	WARNING: Function for internal use ONLY.  Compatibility is not guaranteed in future releases. Please see derived class for implementation.
	*/
	virtual TInt DoMaxCharWidthInPixels() const=0;
	/**
	@publishedAll
	WARNING: Function for internal use ONLY.  Compatibility is not guaranteed in future releases. Please see derived class for implementation.
	*/
	virtual TInt DoMaxNormalCharWidthInPixels() const=0;
	/**
	@publishedAll
	WARNING: Function for internal use ONLY.  Compatibility is not guaranteed in future releases. Please see derived class for implementation.
	*/
	virtual TFontSpec DoFontSpecInTwips() const=0;

protected:	
	IMPORT_C virtual TCharacterDataAvailability DoGetCharacterData(TUint aCode, TOpenFontCharMetrics& aMetrics,const TUint8*& aBitmap,TSize& aBitmapSize) const;
	IMPORT_C virtual TBool DoGetCharacterPosition(TPositionParam& aParam) const;
	IMPORT_C virtual TInt DoExtendedFunction(TUid aFunctionId, TAny* aParam = NULL) const;

protected:
	IMPORT_C virtual ~CFont();

public:
	inline TInt FontCapitalAscent() const;
	inline TInt FontMaxAscent() const;
	inline TInt FontStandardDescent() const;
	inline TInt FontMaxDescent() const;
	inline TInt FontLineGap() const;
	inline TInt FontMaxHeight() const;

public:
	/** Gets run-time identity of the actual font type. This enables safe casting to 
	a derived type.
	
	For example, if the derived type is a CFbsFont, the return value is KCFbsFontUid. 
	You would need to cast to a CFbsFont to get a character bounding box. Similarly, 
	a CBitmapFont returns KCBitmapFontUidVal.
	
	@return The font-type identifier. */
	IMPORT_C TUid TypeUid() const;

	/** Gets the font height in pixels.
	Note that this deprecated function is replaced by the new @c FontMaxHeight().

	@return The font height in pixels.
	@see FontMaxHeight()
	@deprecated */
	IMPORT_C TInt HeightInPixels() const;

	/** Gets the font ascent in pixels.
	Note that this deprecated function is replaced by the new @c FontMaxAscent()
	or in some cases @c FontCapitalAscent().	
	
	@return The font ascent in pixels.
	@see FontCapitalAscent()
	@see FontMaxAscent()
	@deprecated */
	IMPORT_C TInt AscentInPixels() const;
	
	/** Gets the font descent in pixels.
	Note that this deprecated function is replaced by the new @c FontMaxDescent()
	or in some cases @c FontStandardDescent().	
	
	@return The font descent in pixels.
	@see FontStandardDescent() 
	@see FontMaxDescent()
	@deprecated */
	IMPORT_C TInt DescentInPixels() const;

	/** Gets the width in pixels in this font of the specified character.
	
	Note: For OpenType fonts this function returns the horizontal advance of
	the character, which may be different from the actual width.
	
	@param aChar The character whose width should be determined. 
	@return The width in pixels of the specified character in this font. */
	IMPORT_C TInt CharWidthInPixels(TChar aChar) const;

	/** Gets the width in pixels of the specified descriptor when displayed in this 
	font.
	
	@param aText The descriptor whose width should be determined. 
	@return The width of the specified descriptor when displayed in this font, 
	in pixels. */
	IMPORT_C TInt TextWidthInPixels(const TDesC& aText) const;

	/** Gets the baseline offset in pixels. 
	
	The baseline offset is how far a font is raised or lowered from its normal 
	baseline.
	
	@return Offset from normal baseline, in pixels. */
	IMPORT_C TInt BaselineOffsetInPixels() const;

	/** Gets how much of the specified descriptor can be displayed in this font without 
	exceeding the specified width.
	
	Note:
	
	This function does not display any of the descriptor itself - it is used 
	before display, to test whether the whole descriptor can be displayed.
	
	@param aText The descriptor. 
	@param aWidthInPixels The available width for character display. 
	@return The number of characters which will be able to be displayed without 
	exceeding the specified width. The count starts from the beginning of the 
	descriptor. */
	IMPORT_C TInt TextCount(const TDesC& aText,TInt aWidthInPixels) const;

	/** Gets how much of the specified descriptor can be displayed in this font without 
	exceeding the specified width. 
	
	It also returns the excess width - defined as the specified available width 
	minus the width of the portion of the descriptor which can be displayed without 
	exceeding the available width.
	
	@param aText The descriptor. 
	@param aWidthInPixels The available width for character display. 
	@param aExcessWidthInPixels The excess width after displaying the portion of 
	the descriptor, in pixels. 
	@return The number of characters which will be able to be displayed without 
	exceeding the specified width. The count starts from the beginning of the 
	descriptor. */
	IMPORT_C TInt TextCount(const TDesC& aText,TInt aWidthInPixels,TInt& aExcessWidthInPixels) const;

	/** Gets the width in pixels of the widest character in this font.
	
	@return The width of the maximum width character, in pixels. */
	IMPORT_C TInt MaxCharWidthInPixels() const;

	/** Gets the width in pixels of the widest normal character in this font.
	
	Normal characters include all character in a character set except non-alphabetic 
	characters (e.g. the copyright symbol, or a block graphics symbol, for example).
	
	@return The width of the maximum width normal character, in pixels. */
	IMPORT_C TInt MaxNormalCharWidthInPixels() const;

	/** Gets the font specification of this font in twips.
	
	@return The font specification of this font (in twips). */
	IMPORT_C TFontSpec FontSpecInTwips() const;
	IMPORT_C TCharacterDataAvailability GetCharacterData(TUint aCode, TOpenFontCharMetrics& aMetrics,const TUint8*& aBitmap,TSize& aBitmapSize) const;
	IMPORT_C TBool GetCharacterPosition(TPositionParam& aParam) const;
	IMPORT_C TInt WidthZeroInPixels() const;
	IMPORT_C TInt MeasureText(const TDesC& aText, const TMeasureTextInput* aInput = NULL, TMeasureTextOutput* aOutput = NULL) const;
	IMPORT_C static TBool CharactersJoin(TInt aLeftCharacter, TInt aRightCharacter);
	IMPORT_C TInt ExtendedFunction(TUid aFunctionId, TAny* aParam = NULL) const;
	IMPORT_C TBool GetCharacterPosition2(TPositionParam& aParam, RShapeInfo& aShapeInfo) const;

	/** Gets the width in pixels of the specified descriptor when displayed in this 
	font.
	
	@param aText The descriptor whose width should be determined.
	@param aParam Parameter block that controls how much of aText is measured
	@return The width of the specified descriptor when displayed in this font, 
	in pixels. */
	IMPORT_C TInt TextWidthInPixels(const TDesC& aText,const TMeasureTextInput* aParam) const;
	};


// inline functions from original gdi.inl
/** Gets the ascent of an ANSI capital letter in the font whether or not
there are any ANSI capitals in the font.
@return The positive distance from the font baseline to the top of a
standard ANSI capital letter
@publishedAll
@released
*/
inline TInt CFont::FontCapitalAscent() const
    {
    return ExtendedFunction(KFontCapitalAscent);
    }

/** Gets the max ascent of any pre-composed glyph in the font. This will
include accents or diacritics that form part of pre-composed glyphs. It is
not guaranteed to cover the max ascent of composite glyphs that have to be
created by a layout engine. This is also the recommended distance between
the top of a text box and the baseline of the first line of text. 
@return The positive distance from the font baseline to the top of the
highest pre-composed glyph (including accents) above the baseline
@publishedAll
@released
*/
inline TInt CFont::FontMaxAscent() const
    {
    return ExtendedFunction(KFontMaxAscent);
    }

/** Gets the descent of an ANSI descending character in the font.
Whether or not there are any ANSI descenders in the font.
@return The positive distance from the font baseline to the bottom of the
lowest ANSI descender
@publishedAll
@released
*/
inline TInt CFont::FontStandardDescent() const
    {
    return ExtendedFunction(KFontStandardDescent);
    }

/** Gets the max descent of any pre-composed glyph in the font. This will
include accents or diacritics that form part of pre-composed glyphs. It is
not guaranteed to cover the max descent of composite glyphs that have to be
created by a layout engine.
@return The positive distance from the font baseline to the bottom of the
lowest pre-composed glyph (including accents) below the baseline
@publishedAll
@released
*/
inline TInt CFont::FontMaxDescent() const
    {
    return ExtendedFunction(KFontMaxDescent);
    }

/** Gets the suggested line gap for the font. This is the recommended
baseline to baseline distance between successive lines of text in the font.
@return The positive recommended gap between successive lines
@publishedAll
@released
*/
inline TInt CFont::FontLineGap() const
    {
    return ExtendedFunction(KFontLineGap);
    }

/**
Gets the (positive) maximum height in pixels of the font.
This may differ from the design height.

@return The maximum height of the font.
@publishedAll
@released
*/
inline TInt CFont::FontMaxHeight() const
    {
    return FontMaxAscent() + FontMaxDescent();
    }


// from original gdistruct.h
/*
Structure used to pass input/output parameters between RFontTable and CFbsFont.
@internalTechnology
*/
class TGetFontTableParam 
    {
public:
    TUint32 iTag;
    TAny *iContent;
    TInt iLength;
    };

/*
Structure used to pass input/output parameters between RGlyphOutlineIterator and CFbsFont.
Used when opening an outline iterator.
@internalTechnology
*/
class TGetGlyphOutlineParam 
    {
public:
    TInt iCount;
    const TUint *iCodes; 
    TBool iHinted;
    TAny **iOutlines;
    TInt *iLengths; 
    };

/*
Structure used to pass input/output parameters between RGlyphOutlineIterator and CFbsFont.
Used when closing an outline iterator.
@internalTechnology
*/
class TReleaseGlyphOutlineParam 
    {
public:
    TInt iCount;
    const TUint *iCodes;
    TBool iHinted;
    };


#endif // __TEXTBASE_H__
