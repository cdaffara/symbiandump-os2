/*
* Copyright (c) 1998-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* A header for the open font system, which allows SymbianOS
* to use fonts of arbitrary types, including TrueType/OpenType
* and other outline font formats.
*
*/


#ifndef __OPENFONT_H__
#define __OPENFONT_H__

#include <e32base.h>
#include <gdi.h>
#include <ecom/ecom.h>

#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <linkedfonts.h>
#include <graphics/openfontconstants.h>
#include <graphics/openfontrasterizer.h>
#include <openfontlinkedtypefaceelementspec.h>
#include <graphics/openfontlinkedtypefacespecification.h>
#include <graphics/openfontlinkedtypefaceextension.h>
#endif

class CFont;
class COpenFontFile;
class COpenFontGlyph;
class COpenFontGlyphCache;
class COpenFontPositioner;
class COpenFontSessionCache;
class COpenFontSessionCacheList;
class RFs;
class TCharacterMetrics;
class TFontSpec;
class TOpenFontFaceAttrib;
class CFontStore;
class TOpenFontFileData;
class TShapeHeader;
class CBitmapFont;
class CShaperFactory;
class CShaper;
class TShapeHeader;
class TFontShapeFunctionParameters;
class TFontShapeDeleteFunctionParameters;
class RShapeInfo;
class TShapeMessageParameters;
class CRasterizerLinkedTypefaceSpecification;
class TLinkedTypefaceSpecificationArgs;

/**
Supplied to COpenFont::ExtendedInterface() to access the extended
API interface MOpenFontShapingExtension.

@see	COpenFont::ExtendedInterface()
@see	MOpenFontShapingExtension
@publishedAll
@released
*/
const TUid KUidOpenFontShapingExtension = {0x10274DB9};

/**
Supplied to COpenFont::ExtendedInterface() to access the extended
API interface MOpenFontTrueTypeExtension.

@see	COpenFont::ExtendedInterface()
@see	MOpenFontTrueTypeExtension
@publishedAll
@released
*/
const TUid KUidOpenFontTrueTypeExtension = {0x1027553E};
const TUid KUidOpenFontGlyphOutlineExtension = {0x102872CE};

/**
Supplied to CShaper::ExtendedInterface() to get the language and script
code with which the current shaper is instatsiated.
@see	CShaper::ExtendedInterface()
@publishedAll
@released
*/
const TUid KUidShaperGetScript = {0x20009966};
const TUid KUidShaperGetLang = {0x20009967};

/**
Font metrics.

@see	CFbsFont::GetFontMetrics()
@publishedAll
@released
*/
class TOpenFontMetrics
	{
public:
	inline TOpenFontMetrics();
	IMPORT_C TOpenFontMetrics(const CFont* aFont);
	inline TInt Size() const;
	inline TInt Ascent() const;
	inline TInt Descent() const;
	inline TInt MaxHeight() const;
	inline TInt MaxDepth() const;
	inline TInt MaxWidth() const;
	inline void SetSize(TInt aSize);
	inline void SetAscent(TInt aAscent);
	inline void SetDescent(TInt aDescent);
	inline void SetMaxHeight(TInt aMaxHeight);
	inline void SetMaxDepth(TInt aMaxDepth);
	inline void SetMaxWidth(TInt aMaxWidth);
	/** WARNING: Function for internal and partner use ONLY. Compatibility is not guaranteed in future releases.*/
	IMPORT_C void SetBaselineCorrection(TInt aBaselineCorrection);
	IMPORT_C TInt BaselineCorrection();
private:
	TInt16 iDesignHeight;	// size of the font ('pointsize' in pixels)
	TInt16 iAscent;			// typographic ascent
	TInt16 iDescent;		// typographic descent
	TInt16 iMaxHeight;		// maximum height of a character; may be greater than iAscent
	TInt16 iMaxDepth;		// maximum depth of a character; may be greater than iDescent
	TInt16 iMaxWidth;		// maximum width of a character
	TInt16 iBaselineCorrection;
	TInt16 iReserved;
	};


/**
Character metrics  includes more information than TCharacterMetrics.

Character metrics allow characters to be placed horizontally or vertically. 
The character metrics encapsulated by this class are: Height, Width, Horizontal 
Bearing X, Horizontal Bearing Y, Horizontal Advance, Vertical Bearing X, 
Vertical Bearing Y, and Vertical Advance. Their meanings are described in the 
associated setter and getter functions.

Note : 

Vertical drawing (in the sense of characters drawn with horizontal baselines, 
but in a vertical line) is not yet supported by Symbian OS.

@see	CFont::GetCharacterData()
@see	CFbsFont::GetCharacterData() 
@publishedAll
@released
*/
class TOpenFontCharMetrics
	{
public:
	enum TUninitialized { EUninitialized };
public:
	/** Default constructor initializes all members to 0. */
	inline TOpenFontCharMetrics();

	/** Constructor that does not initialize any members. */
	TOpenFontCharMetrics(TUninitialized) {}
	IMPORT_C TOpenFontCharMetrics(const TCharacterMetrics& aMetrics);
	IMPORT_C TBool GetTCharacterMetrics(TCharacterMetrics& aMetrics) const;
	inline TInt Width() const;
	inline TInt Height() const;
	inline TInt HorizBearingX() const;
	inline TInt HorizBearingY() const;
	inline TInt HorizAdvance() const;
	inline TInt VertBearingX() const;
	inline TInt VertBearingY() const;
	inline TInt VertAdvance() const;
	inline void GetHorizBounds(TRect& aBounds) const;
	inline void GetVertBounds(TRect& aBounds) const;
	inline void SetWidth(TInt aWidth);
	inline void SetHeight(TInt aHeight);
	inline void SetHorizBearingX(TInt aHorizBearingX);
	inline void SetHorizBearingY(TInt aHorizBearingY);
	inline void SetHorizAdvance(TInt aHorizAdvance);
	inline void SetVertBearingX(TInt aVertBearingX);
	inline void SetVertBearingY(TInt aVertBearingY);
	inline void SetVertAdvance(TInt aVertAdvance);
	IMPORT_C void  SetGlyphType(TGlyphBitmapType);
	IMPORT_C TGlyphBitmapType GlyphType() const;
private:
	TInt16 iWidth;			// width of the glyph
	TInt16 iHeight;			// height of the glyph
	TInt16 iHorizBearingX;	// x component of horizontal bearing
	TInt16 iHorizBearingY;	// y component of horizontal bearing
	TInt16 iHorizAdvance;	// horizontal advance
	TInt16 iVertBearingX;	// x component of vertical bearing
	TInt16 iVertBearingY;	// y component of vertical bearing
	TInt16 iVertAdvance;	// vertical advance
	TUint16  iGlyphBitmapType;// Glyph bitmap type; TGlyphBitmapType
	TInt16 iReserved;
	};


/**
Font glyph data.

Objects of this type are used by rasterizers to supply glyph data to font 
and bitmap server clients. Unless you are writing a rasterizer you will not 
need to use an object of this type.

The object cannot be constructed and destroyed by normal means. It resides 
on a specified heap. It is created by New() and deleted by RHeap::Free().

@see	COpenFont::RasterizeL()
@publishedAll
@released
*/
class TOpenFontGlyphData
	{
public:
	IMPORT_C static TOpenFontGlyphData* New(RHeap* aHeap,TInt aBufferSize);
	inline TBool Overflow() const;
	inline TInt BytesNeeded() const;
	inline TPtrC8 Bitmap() const;
	inline const TUint8* BitmapPointer() const;
	inline const TOpenFontCharMetrics* Metrics() const;
	inline TInt GlyphIndex() const;
	inline TUint8* BufferStart();
	inline TUint8* BufferEnd();
	inline void SetBytesNeeded(TInt aBytes);
	inline void SetBitmapPointer(const TUint8* aBitmap);
	inline void SetMetricsPointer(const TOpenFontCharMetrics* aMetrics);
	inline void SetPointersToInternalBuffers();
	inline void SetMetrics(TOpenFontCharMetrics& aMetrics);
	inline void SetGlyphIndex(TInt aGlyphIndex);
private:
	/*
	Prevent construction and destruction by normal means; the object resides 
	on a specified heap and is created by New and deleted by RHeap::Free.
	*/
	TOpenFontGlyphData();
	~TOpenFontGlyphData();
private:
	TInt iBitmapBufferSize;					// size of the buffer in bytes
	TInt iBytesNeeded;						// bytes needed for the bitmap
	TOpenFontCharMetrics iMetricsBuffer;	// the metrics
	const TUint8* iBitmap;					// pointer to the bitmap; points either to iBitmapBuffer or to
											// the cache if the character was already rasterized
	const TOpenFontCharMetrics* iMetrics;	// pointer to the metrics; points either to iMetricsBuffer or to
											// the cache if the character was already rasterized
	TInt iGlyphIndex;						// the glyph index
	TAny* iReserved;						// unused; for future expansion
	TUint8 iBitmapBuffer[1];				// buffer used to write the bitmap when it is first rasterized; this
											// is actually of size iBitmapBufferSize.
	};

/**
Open Font System font abstract base class.

Derive a class from this class to represent an instance of a typeface at a 
particular size, provide bitmaps of the glyphs, and determine whether 
characters exist in the typeface.

Writing derived classes construction: 

You must call the constructor of this class when creating your derived object, 
passing the arguments aHeap and aSessionCacheList supplied to 
COpenFontFile::GetNearestFontInPixelsL(), and the address of the COpenFontFile 
object that creates the object as aFile.

The derived object must be created on the shared heap aHeap because it is 
shared by several processes: the font and bitmap server and its clients. 
To do this, use aHeap->AllocL() to obtain memory, then construct in place 
using a placement argument to new.

Derived classes must implement the pure virtual function RasterizeL(). 
Information about this function is provided in the function definition below. 

Information about deriving from this class is also provided in the API guide.

@see	COpenFontFile::GetNearestFontInPixelsL()
@publishedAll
@released
*/
class COpenFont: public CBase
	{
public:
	/** Creates a bitmap for the specified Unicode character.

	Implementations of this function should put the bitmap in 
	aGlyphData->iBitmapBuffer, and the character metrics are placed in 
	aGlyphData->iMetricsBuffer. The other parts of aGlyphData should be left 
	alone. 

	There are a number of strategies for achieving this, e.g. pass the 
	rasterization task all the way up to the rasterizer engine. These are 
	discussed in the API guide.

	At present you must write the bitmap in the Symbian platform's 
	run-length-encoded format. This is a packed binary format starting on a 
	byte boundary and made up of a number of sections. Each section starts 
	with a five-bit header. If the first bit of the header is 0 the next four 
	bits are a repeat count, starting with the least significant bit, and a 
	single row of bits (the number of bits in a row is specified by 
	aGlyphData->iMetricsBuffer.Width()) follows. If the first bit of the header 
	is 1 the next four bits are a count of non-repeating rows, again starting 
	with the least significant bit, and that many rows of bits follow.

	@param aCode The character code of the Unicode character for which the 
	bitmap is required.
	@param aGlyphData On return, contains a pointer to a TOpenFontGlyphData 
	containing the character's bitmap and metrics. */
	virtual void RasterizeL(TInt aCode,TOpenFontGlyphData* aGlyphData) = 0;
	IMPORT_C virtual void ExtendedInterface(TUid aUid, TAny*& aParam);
	IMPORT_C COpenFont(RHeap* aHeap,COpenFontSessionCacheList* aSessionCacheList,COpenFontFile* aFile);
	IMPORT_C COpenFont(RHeap* aHeap,COpenFontSessionCacheList* aSessionCacheList,COpenFontFile* aFile,TInt aFaceIndex);
	IMPORT_C ~COpenFont();
	IMPORT_C void operator delete(TAny*);
	inline const TOpenFontMetrics& Metrics() const;
	inline const TOpenFontFaceAttrib* FaceAttrib() const;
	inline COpenFontFile* File() const;
	inline TInt FaceIndex() const;
	inline TBool CharacterNeedsToBeRasterized(TInt aSessionHandle,TInt aCode) const;
	void SetShaper(CShaper* aShaper);
	CShaper* GetShaper();
	TBool HasShaper() const;
	TShapeHeader* GetShapedData(TInt aSessionHandle,TFontShapeFunctionParameters* aParams);
	TShapeHeader* InsertShapedDataIntoCache(TInt aSessionHandle,TFontShapeFunctionParameters* aParams, TShapeHeader* aShapeHeader);
	TInt FreeShaperCacheMemory(TInt aBytesNeeded);
	TInt DecrementCachedRefCount(TInt aSessionHandle,TShapeHeader* aShapeHeader,TBool aResetAll=EFalse);
	TBool Rasterize(TInt aSessionHandle,TInt aCode,TOpenFontGlyphData* aGlyphData);
	TBool HasCharacterL(TInt aCode) const;
	TBool GetCharacterData(TInt aSessionHandle,TInt aCode,const TOpenFontCharMetrics*& aMetrics,const TUint8*& aBitmap) const;
	void OnFileDeleted();
	COpenFontGlyphCache* GetGlyphCache() const;
	inline TInt FontCapitalAscent() const;
	inline TInt FontMaxAscent() const;
	inline TInt FontStandardDescent() const;
	inline TInt FontMaxDescent() const;
	inline TInt FontLineGap() const;
	inline TInt FontMaxHeight() const;
	void DeleteShaper() const;
	TInt GetFontTable(TUint32 aTag, TAny*& aTableContent, TInt& aLength);
	TInt GetGlyphOutline(TUint aCode, TBool aHinted, TAny*& aOutline, TInt& aLength);
	
protected:
	RHeap* iHeap;
	TOpenFontMetrics iMetrics;
private:
	/** The shaper for performing complex character positioning and
	substitution, if available. Null otherwise.
	
	WARNING: Member variable for internal use ONLY. Compatibility is not guaranteed in future releases. Please access using the provided get/set APIs only. 
    */
	CShaper* iShaper;

protected:
	/** The positive distance in pixels from the baseline to
		the top of an ANSI capital (whether or not there are
		ANSI capitals in the font) */
	TInt iFontCapitalAscent;
	/** The positive distance in pixels from the baseline to
		the top of the highest pre-composed glyph in the font */
	TInt iFontMaxAscent;
	/** The positive distance in pixels from the baseline to
		the bottom of the lowest ANSI descender (whether or
		not there are ANSI chars in the font)*/
	TInt iFontStandardDescent;
	/** The positive distance in pixels from the baseline to
		the bottom of the lowest pre-composed glyph in the font */
	TInt iFontMaxDescent;
	/** The recommended baseline to baseline gap for successive
		lines of text in the font */
	TInt iFontLineGap;
private:
	const COpenFontGlyph* Glyph(TInt aSessionHandle,TInt aCode) const;

	const COpenFontGlyph* FontCacheGlyph(TInt aCode) const;
	
	void SetGlyphCache(COpenFontGlyphCache* aGlyphCache);
	
	const COpenFontGlyph* SessionCacheGlyph(RHeap* aHeap,TInt aSessionHandle,TInt aCode,
											COpenFontSessionCache*& aCache,TInt& aIndex,TBool aCreate) const;
	void RasterizeHelperL(TInt aCode,TOpenFontGlyphData* aGlyphData,TOpenFontGlyphData*& aTempGlyphData);

	COpenFontSessionCacheList* SessionCacheList()const;
	
    void SetSessionCacheList(COpenFontSessionCacheList* aSessionCacheList);
    
    void SetFile(COpenFontFile* aFile);
private:
    
    // Offset from the address of the file used by this font.
    // If the file has been deleted or cannot be used, the offest will be zero.
    TInt iFileOffset;
    
	TInt iFaceIndex;										// index of the face in the font file

	// Offset from the address of this font of the per-font glyph cache which is owned by the font
    TInt iGlyphCacheOffset;

    // Offset from the address of this font of the list of per-session glyph
    // caches which are owned by CFontStore
    TInt iSessionCacheListOffset;

	TAny* iReserved; // unused; for future expansion
	};

/** Open Font System Extension Interface abstract base class.

COpenFont derivers should also derive from this interface to enable complex
font "shaping".

This interface should be returned by the overridden
COpenFont::ExtendedInterface function when KUidOpenFontShapingExtension is
supplied as the UID.

@see	COpenFont
@see	KUidOpenFontShapingExtension 
@publishedAll
@released
*/
class MOpenFontShapingExtension
	{
public:
	/** Various font metrics. */
	class TExtensionFontMetrics
		{
	public:
		/** The number of font design units per em. */
		TInt iUnitsPerEm;
		/** The width of the font's em square in pixels. */
		TReal iXPixelsPerEm;
		/** The height of the font's em square in pixels. */
		TReal iYPixelsPerEm;
		/** The horizontal scaling factor from the font's transform;
		the number of horizontal pixels per font unit. */
		TReal iXScaleFactor;
		/** The vertical scaling factor from the font's transform;
		the number of vertical pixels per font unit. */
		TReal iYScaleFactor;
	private:
		/** Reserved for future expansion. */
		TInt iReserved[4];
		};

	/** Creates a bitmap for the specified Glyph code.

	Implementations of this function should put the bitmap in
	aGlyphData->iBitmapBuffer, and the character metrics are placed in
	aGlyphData->iMetricsBuffer. The other parts of aGlyphData should be left
	alone.

	This function provides the same functionality as the
	COpenFont::RasterizeL() except that the glyph code is supplied rather than
	the unicode.

	For more information:
	@see	COpenFont::RasterizeL()

	@param	aCode		The glyph code of the character for which the bitmap is required.
	@param	aGlyphData	The function puts its output here.
	*/
	virtual void RasterizeGlyphL(TInt aCode,TOpenFontGlyphData* aGlyphData) = 0;

	/** Maps a character to a glyph index.
	@param aUnicodeCharacter Unicode character code to be mapped.
	@return Glyph code corresponding to aUnicodeCharacter. */
	virtual TInt GlyphIndex(TInt aUnicodeCharacter) const = 0;

	/** Returns the hinted pixel coordinates of a particular point in the
	outline of the given glyph.
	@param aGlyphPoint The glyph index.
	@param aPointNumber The number of the point.
	@param aX Returns the point's X pixel value.
	@param aY Returns the point's Y pixel value.
	@return True on success, false otherwise. */
	virtual TBool GlyphPointInHintedPixels(TInt aGlyphIndex, TInt aPointNumber,
		TReal& aX, TReal& aY) const = 0;

	/** Returns the coordinates of a particular (unhinted) point in the outline
	of the given glyph in font units.
	@param aGlyphPoint The glyph index.
	@param aPointNumber The number of the point.
	@param aX Returns the point's X co-ordinate in font units.
	@param aY Returns the point's Y co-ordinate in font units.
	@return True on success, false otherwise. */
	virtual TBool GlyphPointInFontUnits(TInt aGlyphIndex, TInt aPointNumber,
		TInt& aX, TInt& aY) const = 0;

	/** Returns font metrics.
	@param aOut Font metrics returned. */
	virtual void GetExtensionFontMetrics(TExtensionFontMetrics& aOut) = 0;
	};

/** TrueType extension for Open Fonts.

If an open font is able to load TrueType or OpenType tables it should derive
from this class.

This class will be used by 

This interface should be returned by the overridden
COpenFont::ExtendedInterface function when KUidOpenFontTrueTypeExtension is
supplied as the UID.
@see	KUidOpenFontTrueTypeExtension
@publishedAll
@released
*/
class MOpenFontTrueTypeExtension
	{
public:
	/** Returns the specified table. The table need not be released by the
	caller, so the deriver is responsible for ensuring that any memory
	allocated is ultimately cleaned up. The caller must not delete or modify
	the data returned.
	@param aError
		Returns KErrNone if successful or one of the system-wide error codes if
		unsuccessful. Returns KErrNotSupported if the table does not exist in
		the font file.
	@param aTag
		The tag of the table, with the first character in the most significant
		byte. For example, the "vhea" table is obtained by passing in 'v'<<24 |
		'h'<<16 | 'e'<<8 | 'a'.
	@param aLength
		The length of the table in bytes that has been returned or 0 if
		unsuccessful.
	@return The table, if successful.
	@see ReleaseTrueTypeTable */
	virtual TAny* GetTrueTypeTable(TInt& aError, TUint32 aTag, TInt* aLength) = 0;
	/** Releases a table obtained by GetTrueTypeTable. The caller must not
	use the table after this call.
	@param aTable The table to be released.
	@see GetTrueTypeTable */
	virtual TInt ReleaseTrueTypeTable(TAny* aTable) = 0;
	/** Determines if the font file has a particular table.
	@param aTag The tag of the table as for GetTrueTypeTable.
	@return True if the table exists in the font file, false otherwise.
	@see GetTrueTypeTable */
	virtual TBool HasTrueTypeTable(TUint32 aTag) = 0;
	};

class MOpenFontGlyphOutlineExtension 
    {
public:
    virtual TInt GetGlyphOutline(TUint aCode, TBool aIsGlyphId, 
            TBool aHinted, TAny*& aOutline, TInt& aLength) = 0;
    };
/** 
Font attribute base class. 

This class is not intended for user derivation.

@see	TOpenFontFaceAttrib
@see	TOpenFontSpec
@publishedAll
@released
*/
class TOpenFontFaceAttribBase
	{
public:
	inline TOpenFontFaceAttribBase();
	inline TBool HasLatin() const;
	inline TBool HasGreek() const;
	inline TBool HasCyrillic() const;
	inline TBool HasKana() const;
	inline TBool HasHangul() const;
	inline TBool HasCJK() const;
	inline TBool IsSymbol() const;
	inline TBool IsBold() const;
	inline TBool IsItalic() const;
	inline TBool IsSerif() const;
	inline TBool IsMonoWidth() const;
	inline TPtrC Name() const;
	inline const TUint* Coverage() const;
	inline void SetName(const TDesC& aName);
	inline void SetCoverage(TUint aCoverage0,TUint aCoverage1 = 0,TUint aCoverage2 = 0,TUint aCoverage3 = 0);
	inline void SetBold(TBool aBold);
	inline void SetItalic(TBool aItalic);
	inline void SetSerif(TBool aSerif);
	inline void SetMonoWidth(TBool aMonoWidth);
	inline TBool operator==(const TOpenFontFaceAttribBase& aAttrib) const;

	// Unicode ranges for iCoverage[0] (see the TrueType documentation for other values which are not included)
	enum
		{
		ELatinSet = 0x1,			// 0000-007F
		EGreekSet = 0x80,			// 0370-03FF
		ECyrillicSet = 0x200,		// 0400-04FF
		EArmenianSet = 0x400,		// 0530-058F
		EHebrewSet = 0x800,			// 0590-05FF
		EArabicSet = 0x2000,		// 0600-06FF
		EDevanagariSet = 0x8000,	// 0900-097F
		EBengaliSet = 0x10000,		// 0980-09FF
		EGurmukhiSet = 0x20000,		// 0A00-0A7F
		EGujuratiSet = 0x40000,		// 0A80-0AFF
		EOriyaSet = 0x80000,		// 0B00-0B7F
		ETamilSet = 0x100000,		// 0B80-0BFF
		ETeluguSet = 0x200000,		// 0C00-0C7F
		EKannadaSet = 0x400000,		// 0C80-0CFF
		EMalayalamSet = 0x800000,	// 0D00-0D7F
		EThaiSet = 0x1000000,		// 0E00-0E7F
		ELaoSet = 0x2000000,		// 0E80-0EFF
		EGeorgianSet = 0x8000000,	// 10A0-10FF
		EHangulJamoSet = 0x10000000,// 1100-11FF
		};

	// Unicode ranges for iCoverage[1]
	enum
		{
		ESymbolSets = 0xFFFE,
		EKanaSets = 0x60000,
		EHangulSet = 0x100000,
		ECJKSet = 0x8000000
		};

	// Attribute flags
	enum
		{
		EBold = 0x1,			// weight > 500 in Windows terms
		EItalic = 0x2,			// italic or oblique - any forward slant counts
		ESerif = 0x4,			// characters have serifs
		EMonoWidth = 0x8		// all characters are the same width
		};

	// Other constants
	enum
		{
		ENameLength = 32		// we allow 32 but names that are compatible with old-style fonts are limited to 24
		};
protected:
	TBufC<ENameLength> iName;				// the name of the typeface; whether this is the full name or family name
											// or is in English or another language depends on the context
	TUint iCoverage[4];						// bitmap of the character sets supported by the typeface;
											// these are the same as the fields ulUnicodeRange1 .. ulUnicodeRange4
											// in the OS/2 table in TrueType fonts
	TInt iStyle;							// bold, italic serif, and mono-width
	TInt32 iReserved;
	};


/** 
Typeface attributes.

These attributes include the name, family name, and supported scripts.

@see	CFbsFont::GetFaceAttrib()
@publishedAll
@released
*/
class TOpenFontFaceAttrib: public TOpenFontFaceAttribBase
	{
public:
	inline TOpenFontFaceAttrib();
	inline TBool operator==(const TOpenFontFaceAttrib& aAttrib) const;
	inline TPtrC FullName() const;
	inline TPtrC FamilyName() const;
	inline TPtrC LocalFullName() const;
	inline TPtrC LocalFamilyName() const;
	inline TPtrC ShortFullName() const;
	inline TPtrC ShortFamilyName() const;
	inline TPtrC ShortLocalFullName() const;
	inline TPtrC ShortLocalFamilyName() const;
	inline void SetFullName(const TDesC& aName);
	inline void SetFamilyName(const TDesC& aName);
	inline void SetLocalFullName(const TDesC& aName);
	inline void SetLocalFamilyName(const TDesC& aName);
	inline TInt MinSizeInPixels() const;
	inline void SetMinSizeInPixels(TInt aSize);
private:
	TBufC<ENameLength> iFamilyName;			// the name without any style suffixes; usually a truncation of iFullName
	TBufC<ENameLength> iLocalFullName;		// the full name in the language of the current locale
	TBufC<ENameLength> iLocalFamilyName;	// the family name in the language of the current locale
	TInt iMinSizeInPixels;					// minimum supported size, (or readable size if scalable) in pixels
	TInt32 iReserved2;
	};


/**
Font specification allowing more attributes to be specified than TFontSpec.

In addition to the attributes specified by TFontSpec, this font specification 
allows stretching and slanting in the x dimension, and other algorithmic effects. 
This is used to correct for non-square pixels, and to allow algorithmic slanting 
and bold. The width factor and slant factor transform any point (x,y) to (x 
* iWidthFactor + y * iSlantFactor,y). The width and slant factors are 16.16 
fixed-point numbers.

The font specification also provides access to information about the scripts 
which are supported by the font. This information is not always reliable, 
because it may be incorrectly specified, or not specified at all, by some 
font manufacturers. 
@publishedAll
@released
*/
class TOpenFontSpec : public TOpenFontFaceAttribBase
	{
public:
	IMPORT_C TOpenFontSpec();
	IMPORT_C TOpenFontSpec(const TFontSpec& aFontSpec);
	IMPORT_C void operator=(const TFontSpec& aFontSpec);
	inline TBool operator==(const TOpenFontSpec& aFontSpec) const;
	TBool operator!=(const TOpenFontSpec& aOpenFontSpec) const;
	IMPORT_C void SetAttrib(const TOpenFontFaceAttribBase& aAttrib);
	IMPORT_C void GetTFontSpec(TFontSpec& aFontSpec) const;
	IMPORT_C void CompensateForAspectRatio(TInt aPixelWidth,TInt aPixelHeight);
	IMPORT_C void CompensateForAspectRatio(const MGraphicsDeviceMap& aMap);
	inline TInt Height() const;
	inline TInt32 WidthFactor() const;
	inline TInt32 SlantFactor() const;
	inline TGlyphBitmapType BitmapType() const;
	inline TUint32 Effects() const;
	inline TFontPrintPosition PrintPosition() const;
	inline void SetHeight(TInt aHeight);
	inline void SetWidthFactor(TInt32 aWidthFactor);
	inline void SetSlantFactor(TInt32 aSlantFactor);
	inline void SetBitmapType(TGlyphBitmapType aBitmapType);
	inline void SetEffects(TUint32 aEffects);
	IMPORT_C void SetEffects(FontEffect::TEffect aEffect, TBool aOn);
	IMPORT_C TBool IsEffectOn(FontEffect::TEffect aEffect) const;
	IMPORT_C void SetScriptTypeForMetrics(TLanguage aLanguage);
	IMPORT_C TInt ScriptTypeForMetrics() const;
	static TBool IsCompensationForAspectRatioNeeded(TInt aPixelWidth, TInt aPixelHeight);
	static TInt ApplyRatio(TInt& aValue,TInt aNumerator,TInt aDenominator);
	static TInt ApplyRatio(TInt32& aValue,TInt aNumerator,TInt aDenominator);
public:
	/** Algorithmic effects flags.

	These can be combined using an OR operation.

	WARNING: Enum for internal and partner use ONLY.  Compatibility is not guaranteed in future releases.
 
	@deprecated Use FontEffect::TEffect instead.
	*/
	enum
		{
		/** Font is algorithmic bold. */
		EAlgorithmicBold = 1,
		/** Font has a drop shadow. */
		EDropShadow = 2,
		/** Font is an outline font. */
		EOutline = 4
		};
private:
	void SetScriptTypeForMetrics(TInt aScript);
	void SetSymbol(TBool aSymbol);
	TBool Symbol() const;
	IMPORT_C TBool OperatorEquality(const TOpenFontSpec& aOpenFontSpec) const;
	IMPORT_C TUint32 DoEffects() const;
	IMPORT_C void DoSetEffects(TUint32 aEffects);
private:
	TInt iHeight;						// em height
	TInt32 iWidthFactor;				// algorithmic width factor as a 16.16 fixed-point number
	TInt32 iSlantFactor;				// algorithmic slant factor as a 16.16 fixed-point number
	TInt iBitmapType;					// non-anti-aliased, standard anti-aliased, etc.; zero means 'default'
	TUint32 iEffects;					// bit flags for font effects
	TInt iSymbol;						// a symbol font as understood by TFontSpec (bit 1), script type (bits 2-5)
	// The following data member exists only to allow round-trip conversion between TFontSpec and TOpenFontSpec
	TFontPrintPosition iPrintPosition;	// normal, superscript or subscript

	TInt iReserved2;
	};

/**
Font file abstract base class.

Write a class derived from COpenFontFile to manage a file with the font format 
supported by your DLL. The derived class must implement the virtual 
GetNearestFontInPixelsL() function. This function takes a font description 
and creates a COpenFont derived object if the description matches a typeface 
contained in the font file.

Derived classes must also load typeface attributes from the file into the 
protected typeface attribute array during construction. This array is what 
is searched when getting font attribute information see AddFaceL(), 
GetNearestFontHelper().

Writing derived classes  construction:

Call the COpenFontFile constructor in the constructor for your derived object, 
passing it aUid and aFileName arguments. These values are the arguments passed 
when the constructor is called by COpenFontRasterizer::NewFontFileL().

A file may contain one or more typefaces. During construction the derived 
object should extract the information for each typeface and add it to this 
object's protected typeface attribute array see AddFaceL(). This process 
will probably leave under some conditions. It should therefore be implemented 
in a second phase constructor.

Writing derived classes  implement the pure virtual functions:

Derived classes must also implement the two pure virtual functions 
GetNearestFontInPixelsL() and HasUnicodeCharacterL(). Information about 
these functions is provided in the definitions below. Information about 
deriving from this class is also provided in the API guide. 

@see	COpenFontRasterizer::NewFontFileL()
@see	CWsScreenDevice::AddFile()
@publishedAll
@released
*/
class COpenFontFile : public CBase
	{
public:
	/**
	Gets the font which is the nearest to the given font specification.

	Implementations of this pure virtual function should create the COpenFont 
	derived object that most closely matches aDesiredFontSpec, while fitting within
	aMaxHeight, and place a pointer to it in aFont. If this cannot be done,
	e.g. if the font name doesn't match, aFont should be set to NULL.

	The other two arguments, aHeap and aSessionCacheList, should be passed to 
	the COpenFont constructor.

	Implementations may use the utility function GetNearestFontHelper()
	to get the attributes of the closest matching font.

	@param	aHeap				Shared heap. This value should be passed to the 
								COpenFont derived classes' constructor.
	@param	aSessionCacheList	The session cache list. This value should be passed 
								to the COpenFont derived classes' constructor.
	@param	aDesiredFontSpec	The desired font specification.
	@param	aPixelWidth			The width of a pixel. Used with aPixelHeight for calculating 
								the algorithmic slant of the typeface.
	@param	aPixelHeight		The height of a pixel. Used with aPixelWidth for calculating 
								the algorithmic slant of the typeface.
	@param	aFont				On return, contains a pointer to the newly created COpenFont 
								derived object, or NULL if no font matching aDesiredFontSpec exists.
	@param	aActualFontSpec		The actual font specification of the font retrieved into aFont.
	@see	GetNearestFontHelper()
	*/
	virtual void GetNearestFontInPixelsL(
		RHeap* aHeap, COpenFontSessionCacheList* aSessionCacheList,
		const TOpenFontSpec& aDesiredFontSpec, TInt aPixelWidth, TInt aPixelHeight,
		COpenFont*& aFont, TOpenFontSpec& aActualFontSpec) = 0;

	/**
	Gets the font which is the nearest to the given font specification.

	Implementations of this pure virtual function should create the COpenFont 
	derived object that most closely matches aDesiredFontSpec, while fitting within
	aMaxHeight, and place a pointer to it in aFont. If this cannot be done,
	e.g. if the font name doesn't match, aFont should be set to NULL.

	The other two arguments, aHeap and aSessionCacheList, should be passed to 
	the COpenFont constructor.

	Implementations may use the utility function GetNearestFontHelper()
	to get the attributes of the closest matching font.

	@param	aHeap				Shared heap. This value should be passed to the COpenFont 
								derived classes' constructor.
	@param	aSessionCacheList	The session cache list. This value should be passed 
								to the COpenFont derived classes' constructor.
	@param	aDesiredFontSpec	The desired font specification.
	@param	aPixelWidth			The width of a pixel. Used with aPixelHeight for calculating 
								the algorithmic slant of the typeface.
	@param	aPixelHeight		The height of a pixel. Used with aPixelWidth for calculating 
								the algorithmic slant of the typeface.
	@param	aFont				On return, contains a pointer to the newly created COpenFont 
								derived object, or NULL if no font matching aDesiredFontSpec exists.
	@param	aActualFontSpec		The actual font specification of the font retrieved into aFont.
	@see	GetNearestFontHelper()
	*/
	virtual void GetNearestFontToDesignHeightInPixelsL(
		RHeap* /*aHeap*/, COpenFontSessionCacheList* /*aSessionCacheList*/,
		const TOpenFontSpec& /*aDesiredFontSpec*/, TInt /*aPixelWidth*/, TInt /*aPixelHeight*/,
		COpenFont*& /*aFont*/, TOpenFontSpec& /*aActualFontSpec*/) {}
	/**
	Gets the font which is the nearest to the given font specification.

	Implementations of this pure virtual function should create the COpenFont 
	derived object that most closely matches aDesiredFontSpec, while fitting within
	aMaxHeight, and place a pointer to it in aFont. If this cannot be done,
	e.g. if the font name doesn't match, aFont should be set to NULL.

	The other two arguments, aHeap and aSessionCacheList, should be passed to 
	the COpenFont constructor.

	Implementations may use the utility function GetNearestFontHelper()
	to get the attributes of the closest matching font.

	@param	aHeap				Shared heap. This value should be passed to the COpenFont 
								derived classes' constructor.
	@param	aSessionCacheList	The session cache list. This value should be passed 
								to the COpenFont derived classes' constructor.
	@param	aDesiredFontSpec	The desired font specification.
	@param	aPixelWidth			The width of a pixel. Used with aPixelHeight for calculating 
								the algorithmic slant of the typeface.
	@param	aPixelHeight		The height of a pixel. Used with aPixelWidth for calculating 
								the algorithmic slant of the typeface.
	@param	aFont				On return, contains a pointer to the newly created COpenFont 
								derived object, or NULL if no font matching aDesiredFontSpec exists.
	@param	aActualFontSpec		The actual font specification of the font retrieved into aFont.
	@param	aMaxHeight			The maximum height (vertical extent) within which the font must fit.
	@see	GetNearestFontHelper()
	*/
	virtual void GetNearestFontToMaxHeightInPixelsL(
		RHeap* /*aHeap*/, COpenFontSessionCacheList* /*aSessionCacheList*/,
		const TOpenFontSpec& /*aDesiredFontSpec*/, TInt /*aPixelWidth*/, TInt /*aPixelHeight*/,
		COpenFont*& /*aFont*/, TOpenFontSpec& /*aActualFontSpec*/, TInt /*aMaxHeight*/) {}

	/** Tests whether a specified typeface contains a particular character.

	@param	aFaceIndex	The index of the typeface to be tested.
	@param	aCode		The Unicode character code for the character to be tested. 
	@return	ETrue if the typeface contains aCode, otherwise EFalse. */
	virtual TBool HasUnicodeCharacterL(TInt aFaceIndex,TInt aCode) const = 0; 
	IMPORT_C virtual void ExtendedInterface(TUid aUid, TAny*& aParam);
	IMPORT_C COpenFontFile(TInt aUid,const TDesC& aFileName);
	IMPORT_C ~COpenFontFile();
	IMPORT_C TBool GetNearestFontHelper(const TOpenFontSpec& aDesiredFontSpec,TInt aPixelWidth,TInt aPixelHeight,
										TInt& aFaceIndex,TOpenFontSpec& aActualFontSpec) const;
	IMPORT_C void AddFaceL(const TOpenFontFaceAttrib& aAttrib);
	inline TUid Uid() const;
	inline const TDesC& FileName() const;
	inline const TOpenFontFaceAttrib& FaceAttrib(TInt aFaceIndex) const;
	inline TInt FaceCount() const;
	inline void IncRefCount();
	inline TBool DecRefCount();
	TInt GetNearestFontInPixels(
		RHeap* aHeap, COpenFontSessionCacheList* aSessionCacheList,
		const TOpenFontSpec& aDesiredFontSpec, TInt aPixelWidth, TInt aPixelHeight,
		COpenFont*& aFont, TOpenFontSpec& aActualFontSpec);
	TInt GetNearestFontToDesignHeightInPixels(
		RHeap* aHeap, COpenFontSessionCacheList* aSessionCacheList,
		const TOpenFontSpec& aDesiredFontSpec, TInt aPixelWidth, TInt aPixelHeight,
		COpenFont*& aFont, TOpenFontSpec& aActualFontSpec);
	TInt GetNearestFontToMaxHeightInPixels(
		RHeap* aHeap, COpenFontSessionCacheList* aSessionCacheList,
		const TOpenFontSpec& aDesiredFontSpec, TInt aPixelWidth, TInt aPixelHeight,
		COpenFont*& aFont, TOpenFontSpec& aActualFontSpec, TInt aMaxHeight);
	void RemoveFontFromList(const COpenFont* aFont);
	void SetFontStoreL(CFontStore* aFontStore);
	CFontStore* GetFontStore();
	CArrayPtrFlat<COpenFont>* GetOpenFontList();
protected:
	IMPORT_C TBool GetNearestFontHelperOld(const TOpenFontSpec& aDesiredFontSpec,TInt aPixelWidth,TInt aPixelHeight,
										TInt& aFaceIndex,TOpenFontSpec& aActualFontSpec) const;
private:
	// A class to contain the public font attributes and private positioning information (for kerning, ligatures, etc.)
	class TAttrib: public TOpenFontFaceAttrib
		{
	public:
		COpenFontPositioner* iPositioner;	// if non-null, positioning information for the typeface
		};
	static TInt ScoreByName(const TOpenFontSpec& aDesiredFontSpec, const TAttrib& aAttrib);
	static TInt ScoreByStyle(const TOpenFontSpec& aDesiredFontSpec, const TAttrib& aAttrib);
	void GetNearestFontToDesignHeightInPixelsAndAddToListL(
		RHeap* aHeap, COpenFontSessionCacheList* aSessionCacheList,
		const TOpenFontSpec& aDesiredFontSpec, TInt aPixelWidth, TInt aPixelHeight,
		COpenFont*& aFont, TOpenFontSpec& aActualFontSpec);
	void GetNearestFontToMaxHeightInPixelsAndAddToListL(
		RHeap* aHeap, COpenFontSessionCacheList* aSessionCacheList,
		const TOpenFontSpec& aDesiredFontSpec, TInt aPixelWidth, TInt aPixelHeight,
		COpenFont*& aFont, TOpenFontSpec& aActualFontSpec, TInt aMaxHeight);

private:
	CArrayFixFlat<TAttrib> iFaceAttrib;
	TUid iUid;
	TBuf<KMaxFileName> iFileName;
	TInt iRefCount;
	CArrayPtrFlat<COpenFont> iFontList;
	TOpenFontFileData* iData;
	};

/**
Convenience class from which rasterizer contexts may be derived.

A rasterizer context object may (optionally) be created to provide the link 
between the rasterizer DLL code supported by the Open Font System, and the 
rasterizer engine code. A rasterizer context object class should get the 
requested bitmap from the associated rasterizer engine. It should then convert 
this into Symbian run-length-encoded format. This class contains convenience 
functions to make the conversion easier.

Deriving from this class is described in greater detail in the API guide.
@publishedAll
@released
*/
class COpenFontRasterizerContext: public CBase
	{
public:
	inline COpenFontRasterizerContext();
	inline void StartGlyph(TOpenFontGlyphData* aGlyphData);
	inline void WriteGlyphBit(TInt aBit);
	inline void WriteGlyphByte(TInt aByte);
	inline void EndGlyph();
private:
	TOpenFontGlyphData* iGlyphData;
	TUint8* iGlyphDataStart;
	TUint8* iGlyphDataPtr;
	TUint8* iGlyphDataEnd;
	TInt iGlyphBit;
	TInt iBytesNeeded;
	TBool iOverflow;
	TAny* iReserved; // unused; for future expansion
	};


/**
Shaper abstract class.  All shaper implementations derive from this
@publishedAll 
@released
*/
class CShaper : public CBase
	{
public:
	class TInput
		{
	public:
		/** The text to be shaped possibly including context. */
		const TDesC* iText;
		/** The index of the first character in iText to be shaped. */
		TInt iStart;
		/** (One past) the end of the text in iText to be shaped. */
		TInt iEnd;
		/** Script code, for example 'd'<<24 | 'e'<<16 | 'v'<<8 | 'a'
		for Devanagari. */
		TInt iScript;
		/** Language code. 0 implies "default" */
		TUint32 iLanguage;
		/** Maximum advance in pixels. Shaping beyond this
		advance is not required (but is not harmful). */
		TInt iMaximumAdvance;
		/** Flags. Currently none is defined. */
		TInt iFlags;
		/** Session handle. To be used if rasterization is needed. */
		TInt iSessionHandle;
		/** Reserved for future expansion. Must be set to 0. */
		TInt iReserved1;
		};
	/** Constructor */
	IMPORT_C CShaper();

	/** Destructor */
	IMPORT_C virtual ~CShaper();

	/** construct a shaper object
	@param	aBitmapFont	The font to be shaped.
	@param	aHeap		The heap to be used by the shaper.
	@return	KErrNone if this font can be shaper or system wide error code*/
	virtual TInt ConstructL(CBitmapFont*  aBitmapFont, TInt aScript, TInt aLanguage, RHeap* iHeap) = 0;

	/** If possible, shape the text described by aInput, placing the output on aHeapForOutput.
	@param	aOutput			The output, as a newly allocate object on aHeapForOutput.
	@param	aInput			The input text and other parameters.
	@param	aHeapForOutput	On success, aOutput should be allocated from this and nothing else. 
							On failure, nothing should be allocated from it.
	@return	Error value from one of the system-wide error codes on failure, KErrNone on success.
	@see	TShapeHeader */
	virtual TInt ShapeText(TShapeHeader*& aOutput, const TInput& aInput, RHeap* aHeapForOutput) = 0;

	/** For future expansion. Any overriders must base-call
	if aInterface is unrecognized.
	
	WARNING: Function for internal use ONLY. Compatibility is not guaranteed in future releases.
	
	@param	aInterfaceId	The ID of the interface to return.
	@return	A pointer to the extension interface.
	*/
	IMPORT_C virtual void* ExtendedInterface(TUid aInterfaceId);
	};

/** ECOM plug-in base class for shaper factories.
@publishedAll
@released
*/
class CShaperFactory : public CBase
	{
public:
	/** Create a shaper if possible, for typeface aFaceIndex
	within file aFileName.
	@param	aFont	The font to be shaped.
	@param	iHeap	The heap to use for constructing the shaper.
	@return	0 If the font is not understood or inappropriate for any shaper that might be constructed 
			by this class, otherwise returns the newly-constructed shaper on iHeap. */
	virtual CShaper* NewShaperL(CBitmapFont* aFont, TInt aScript, TInt aLanguage, RHeap* iHeap) = 0;

	inline static CShaperFactory* NewL(TUid aInterfaceImplUid);
	inline virtual ~CShaperFactory();

	/** For future expansion. Any overriders must base-call if aInterface is unrecognized.

	WARNING: Function for internal use ONLY. Compatibility is not guaranteed in future releases.

	@param	aInterfaceId	The ID of the interface to return.
	@return	A pointer to the extension interface.	
	*/
	virtual void* ExtendedInterface(TUid aInterfaceId);

private:
	TUid iDtor_ID_Key;//ECOM identifier used during destruction
	};

// Inline functions start here.
/** Default C++ constructor.

This creates then zero fills the object. */
inline TOpenFontMetrics::TOpenFontMetrics()
	{
	Mem::FillZ(this,sizeof(*this));
	}

/** Gets the font's size.

@return	The font's size. 
@see	SetSize() */
inline TInt TOpenFontMetrics::Size() const
	{
	return iDesignHeight;
	}

/** Gets the font's ascent.

This is the ascent for the Latin character which is highest above the baseline.

@return The font's ascent, in pixels.
@see SetAscent() */
inline TInt TOpenFontMetrics::Ascent() const
	{
	return iAscent;
	}

/** Gets the font's descent.

This is the descent for the Latin character in the font which falls furthest below the baseline.

@return The font's descent, in pixels. 
@see SetDescent() */
inline TInt TOpenFontMetrics::Descent() const
	{
	return iDescent;
	}

/** Sets the font's maximum height.

Note that if this object was initialised from the CFont this will be the same 
as the ascent.

This is the ascent for the character which is highest above the baseline. 
In many fonts this will be the height of an accented character like Â, 
including the accent.

@return The maximum height of the font, in pixels.
@see SetMaxDepth() */
inline TInt TOpenFontMetrics::MaxHeight() const
	{
	return iMaxHeight;
	}

/** Gets the font's maximum depth.

Note: If this object was initialised from the CFont this will be the same as the 
descent.

This is the descent for the character in the font which falls furthest below 
the baseline.

@return The font's maximum depth.
@see SetMaxDepth() */
inline TInt TOpenFontMetrics::MaxDepth() const
	{
	return iMaxDepth;
	}

/** Gets the maximum character width, in pixels.

@return The maximum character width, in pixels.
@see SetMaxWidth() */
inline TInt TOpenFontMetrics::MaxWidth() const
	{
	return iMaxWidth;
	}

/** Sets the font's size.

@param aSize The font's size.
@see Size() */
inline void TOpenFontMetrics::SetSize(TInt aSize)
	{
	iDesignHeight = static_cast<TInt16>(aSize);
	}

/** Sets the ascent.

@param aAscent The ascent, in pixels.
@see Ascent() */
inline void TOpenFontMetrics::SetAscent(TInt aAscent)
	{
	iAscent = static_cast<TInt16>(aAscent);
	}

/** Sets the descent.

@param aDescent The descent, in pixels.
@see Descent() */
inline void TOpenFontMetrics::SetDescent(TInt aDescent)
	{
	iDescent = static_cast<TInt16>(aDescent);
	}

/** Sets the font's maximum height.

@param aMaxHeight The font's maximum height, in pixels. 
@see MaxHeight() */
inline void TOpenFontMetrics::SetMaxHeight(TInt aMaxHeight)
	{
	iMaxHeight = static_cast<TInt16>(aMaxHeight);
	}

/** Sets the font's maximum depth.

@param aMaxDepth The font's maximum depth, in pixels.
@see MaxDepth() */
inline void TOpenFontMetrics::SetMaxDepth(TInt aMaxDepth)
	{
	iMaxDepth = static_cast<TInt16>(aMaxDepth);
	}

/** Sets the maximum character width, in pixels.

@param aMaxWidth The maximum character width, in pixels.
@see MaxWidth() */
inline void TOpenFontMetrics::SetMaxWidth(TInt aMaxWidth)
	{
	iMaxWidth = static_cast<TInt16>(aMaxWidth);
	}

/** Default C++ constructor. 

The constructor initialises all data members to zero. As for other T classes, 
there is no need to explicitly cleanup TOpenFontCharMetrics objects. */
inline TOpenFontCharMetrics::TOpenFontCharMetrics()
	{
	Mem::FillZ(this,sizeof(*this));
	}

/** Gets the width of the character's bitmap.

@return The width of the bitmap in pixels. */
inline TInt TOpenFontCharMetrics::Width() const
	{
	return iWidth;
	}

/** Gets the height of the character's bitmap.

@return The character's height in pixels. */
inline TInt TOpenFontCharMetrics::Height() const
	{
	return iHeight;
	}

/** Gets the horizontal bearing X. 

This is the distance in pixels from the pen point before the character is 
drawn (the origin) to the left edge of the bitmap, when drawing horizontally. 
A positive value means that the left edge of the bitmap is right of the origin.

@return The horizontal bearing X in pixels */
inline TInt TOpenFontCharMetrics::HorizBearingX() const
	{
	return iHorizBearingX;
	}

/** Gets horizontal bearing Y.

This is the vertical distance in pixels from the pen point before the character 
is drawn (the origin) to the top edge of the bitmap, when drawing horizontally. 
A positive value means that the top edge of the bitmap is above the origin

@return The horizontal bearing Y in pixels. */
inline TInt TOpenFontCharMetrics::HorizBearingY() const
	{
	return iHorizBearingY;
	}

/** Gets the horizontal advance.

This is the amount added to the x co-ordinate of the origin after the character 
is drawn   what most people understand by the width or escapement of a character. 
The origin here is the pen point before the character is drawn. 

@return The horizontal advance in pixels */
inline TInt TOpenFontCharMetrics::HorizAdvance() const
	{
	return iHorizAdvance;
	}

/** Gets the vertical bearing X.

This is the distance in pixels from the pen point before the character is 
drawn (the origin) to the left edge of the bitmap, when drawing vertically. 
A positive value means that the left edge of the bitmap is right of the origin.

@return The vertical bearing X in pixels. */
inline TInt TOpenFontCharMetrics::VertBearingX() const
	{
	return iVertBearingX;
	}

/** Gets the vertical bearing Y.

This is the distance in pixels from the pen point before the character is 
drawn (the origin) to the top edge of the bitmap, when drawing vertically. 
A positive value means that the top edge of the bitmap is above the origin.

@return The vertical bearing Y in pixels. */
inline TInt TOpenFontCharMetrics::VertBearingY() const
	{
	return iVertBearingY;
	}

/** Gets the vertical advance.

When drawing vertically, this is the amount added to the y co-ordinate of 
the origin after the character is drawn what most people understand by 
the height of a character. The origin here is the pen point before the character 
is drawn.

Note: Vertical drawing is not supported in v5.

@return The vertical advance in pixels. */
inline TInt TOpenFontCharMetrics::VertAdvance() const
	{
	return iVertAdvance;
	}

/** Gets the bounds of the character relative to its origin when setting text 
horizontally.

The origin here is the pen point before the character is drawn. 

@param aBounds The character's bounds. */
inline void TOpenFontCharMetrics::GetHorizBounds(TRect& aBounds) const
	{
	aBounds.iTl.iX = iHorizBearingX;
	aBounds.iTl.iY = -iHorizBearingY;
	aBounds.iBr.iX = aBounds.iTl.iX + iWidth;
	aBounds.iBr.iY = aBounds.iTl.iY + iHeight;
	}

/** Gets the bounds of the character relative to its origin when setting text 
vertically. 

The origin here is the pen point before the character is drawn.

@param aBounds The character's bounds. */
inline void TOpenFontCharMetrics::GetVertBounds(TRect& aBounds) const
	{
	aBounds.iTl.iX = -iVertBearingX;
	aBounds.iTl.iY = iVertBearingY;
	aBounds.iBr.iX = aBounds.iTl.iX + iWidth;
	aBounds.iBr.iY = aBounds.iTl.iY + iHeight;
	}

/** Sets the width of the character's bitmap.

@param aWidth The width of the bitmap in pixels. */
inline void TOpenFontCharMetrics::SetWidth(TInt aWidth)
	{
	iWidth = (TInt16)aWidth;
	}

/** Sets the height of the character's bitmap.

@param aHeight The character height (in pixels). */
inline void TOpenFontCharMetrics::SetHeight(TInt aHeight)
	{
	iHeight = (TInt16)aHeight;
	}

/** Sets the horizontal bearing X.

This is the distance in pixels from the pen point before the character is 
drawn (the origin) to the left edge of the bitmap, when drawing horizontally. 
A positive value means that the left edge of the bitmap is right of the origin.

@param aHorizBearingX The horizontal bearing X (in pixels). */
inline void TOpenFontCharMetrics::SetHorizBearingX(TInt aHorizBearingX)
	{
	iHorizBearingX = (TInt16)aHorizBearingX;
	}

/** Sets the horizontal bearing Y.

This is the distance in pixels from the pen point before the character is 
drawn (the origin) to the top edge of the bitmap, when drawing horizontally. 
A positive value means that the top edge of the bitmap is above the origin.

@param aHorizBearingY The horizontal bearing Y (in pixels). */
inline void TOpenFontCharMetrics::SetHorizBearingY(TInt aHorizBearingY)
	{
	iHorizBearingY = (TInt16)aHorizBearingY;
	}

/** Sets the horizontal advance.

This is the amount added to the x co-ordinate of the origin after the character 
is drawn, what most people understand by the width or escapement of a character. 
The origin here is the pen point before the character is drawn.

@param aHorizAdvance The horizontal advance (in pixels). */
inline void TOpenFontCharMetrics::SetHorizAdvance(TInt aHorizAdvance)
	{
	iHorizAdvance = (TInt16)aHorizAdvance;
	}

/** Set vertical bearing X.

This is the distance in pixels from the pen point before the character is 
drawn (the origin) to the left edge of the bitmap, when drawing vertically. 
A positive value means that the left edge of the bitmap is right of the origin.

@param aVertBearingX The vertical bearing X (in pixels). */
inline void TOpenFontCharMetrics::SetVertBearingX(TInt aVertBearingX)
	{
	iVertBearingX = (TInt16)aVertBearingX;
	}

/** Sets the vertical bearing Y.

This is the distance in pixels from the pen point before the character is 
drawn (the origin) to the top edge of the bitmap, when drawing vertically. 
A positive value means that the top edge of the bitmap is above the origin.

@param aVertBearingY The vertical bearing Y (in pixels). */
inline void TOpenFontCharMetrics::SetVertBearingY(TInt aVertBearingY)
	{
	iVertBearingY = (TInt16)aVertBearingY;
	}

/** Sets the vertical advance.

When drawing vertically, this is the amount added to the y co-ordinate of 
the origin after the character is drawn  what most people understand by 
the height of a character. The origin here is the pen point before the character 
is drawn.

Note: Vertical drawing is not supported in v5.

@param aVertAdvance The vertical advance (in pixels). */
inline void TOpenFontCharMetrics::SetVertAdvance(TInt aVertAdvance)
	{
	iVertAdvance = (TInt16)aVertAdvance;
	}

/** Gets the character metrics for this font.

@return The character metrics for this font. */
inline const TOpenFontMetrics& COpenFont::Metrics() const
	{
	return iMetrics;
	}

/** Gets the glyph index.

This is the index of a particular glyph within the font file.

Note: This makes it possible to gain access to glyphs which are not referenced 
by the Unicode character set. However, this feature is not yet supported by 
Symbian OS.

@return The glyph index.
@see SetGlyphIndex() */
inline TInt TOpenFontGlyphData::GlyphIndex() const
	{
	return iGlyphIndex;
	}

/** Gets the typeface attributes.

These are the attributes of the font represented by this object.

@return The typeface attributes. */
inline const TOpenFontFaceAttrib* COpenFont::FaceAttrib() const
	{
    return iFileOffset == 0 ? NULL : &File()->FaceAttrib(iFaceIndex);
	}

/** Gets a pointer to the COpenFontFile which created this object.

This is the COpenFontFile which owns the file that contains the definition 
of the typeface. It can be used to get information about the typeface, or 
to access the rasterizer context (engine).

@return The COpenFontFile which created this object. */
inline COpenFontFile* COpenFont::File() const
	{
    return iFileOffset == 0 ? NULL : reinterpret_cast<COpenFontFile*>(const_cast<COpenFont*>(PtrAdd(this, iFileOffset)));
	}

/** Gets the index of this typeface within the font file.

@return The index of this typeface within the font file. */
inline TInt COpenFont::FaceIndex() const
	{
	return iFaceIndex;
	}

/** Tests whether or not a character needs to be rasterized.

Characters that have been rasterized are cached  there is no need to regenerate 
the character bitmap. This function should only be called by the Font and 
Bitmap server.

@param aSessionHandle A handle to the font and bitmap server session.
@param aCode The code for the Unicode character.
@return ETrue if the character needs to be rasterized, otherwise EFalse. */
inline TBool COpenFont::CharacterNeedsToBeRasterized(TInt aSessionHandle,TInt aCode) const
	{
	return Glyph(aSessionHandle,aCode) == NULL;
	}

/** Gets the ascent of an ANSI capital letter in the font whether or not
there are any ANSI capitals in the font.
@return The positive distance from the font baseline to the top of a
standard ANSI capital letter
@publishedAll
@released
@see AscentInPixels()
*/
inline TInt COpenFont::FontCapitalAscent() const
	{
	return iFontCapitalAscent;
	}

/** Gets the max ascent of any pre-composed glyph in the font. This will
include accents or diacritics that form part of pre-composed glyphs. It is
not guaranteed to cover the max ascent of composite glyphs that have to be
created by a layout engine. This is also the recommended distance between
the top of a text box and the baseline of the first line of text. 

The value may be affected by the TLanguage value set by SetScriptTypeForMetrics().

@return The positive distance from the font baseline to the top of the
highest pre-composed glyph (including accents) above the baseline.
@publishedAll
@released
@see AscentInPixels()
@see SetScriptTypeForMetrics()
*/
inline TInt COpenFont::FontMaxAscent() const
	{
	return iFontMaxAscent;
	}

/** Gets the descent of an ANSI descending character in the font.
Whether or not there are any ANSI descenders in the font.

The value may be affected by the TLanguage value set by SetScriptTypeForMetrics().

@return The positive distance from the font baseline to the bottom of the
lowest ANSI descender.
@publishedAll
@released
@see DescentInPixels()
@see SetScriptTypeForMetrics()
*/
inline TInt COpenFont::FontStandardDescent() const
	{
	return iFontStandardDescent;
	}

/** Gets the max descent of any pre-composed glyph in the font. This will
include accents or diacritics that form part of pre-composed glyphs. It is
not guaranteed to cover the max descent of composite glyphs that have to be
created by a layout engine.

The value may be affected by the TLanguage value set by SetScriptTypeForMetrics().

@return The positive distance from the font baseline to the bottom of the
lowest pre-composed glyph (including accents) below the baseline
@publishedAll
@released
@see DescentInPixels()
@see SetScriptTypeForMetrics()
*/
inline TInt COpenFont::FontMaxDescent() const
	{
	return iFontMaxDescent;
	}

/** Gets the suggested line gap for the font. This is the recommended
baseline to baseline distance between successive lines of text in the font.
@return The positive recommended gap between successive lines
@publishedAll
@released
*/
inline TInt COpenFont::FontLineGap() const
	{
	return iFontLineGap;
	}

/** Gets the maximum height for the font. This is the sum of the max ascent 
of the font and the max descent of the font.

The value may be affected by the TLanguage value set by SetScriptTypeForMetrics().

@return The positive maximum height of the font
@publishedAll
@released
@see HeightInPixels()
@see SetScriptTypeForMetrics()
*/
inline TInt COpenFont::FontMaxHeight() const
	{
	return iFontMaxAscent + iFontMaxDescent;
	}

/** Default C++ constructor.

This sets all attribute fields to zero. As for other T classes, there is no 
need to explicitly clean-up objects derived from this class. */
inline TOpenFontFaceAttribBase::TOpenFontFaceAttribBase()
	{
	Mem::FillZ(this,sizeof(*this));
	}

/** Default C++ constructor.

The function initialises the minimum typeface size to zero, the names to NULL, 
and the coverage and style flags to zero. */
inline TOpenFontFaceAttrib::TOpenFontFaceAttrib():
	iMinSizeInPixels(0),
	iReserved2(0)
	{
	}

/** Tests for support of Latin characters.

Note: A return value of ETrue implies that the font has a usable set of 
characters. It does not imply exhaustive coverage.

@return ETrue if Latin characters are supported */
inline TBool TOpenFontFaceAttribBase::HasLatin() const
	{
	return iCoverage[0] & ELatinSet;
	}

/** Tests for support of Greek characters.

Note: A return value of ETrue implies that the font has a usable set of 
characters. It does not imply exhaustive coverage.

@return ETrue if Greek characters are supported. */
inline TBool TOpenFontFaceAttribBase::HasGreek() const
	{
	return iCoverage[0] & EGreekSet;
	}

/** Tests for support of Cyrillic characters.

Note: A return value of ETrue implies that the font has a usable set of 
characters. It does not imply exhaustive coverage.

@return ETrue if Cyrillic characters are supported. */
inline TBool TOpenFontFaceAttribBase::HasCyrillic() const
	{
	return iCoverage[0] & ECyrillicSet;
	}

/** Tests for support of Japanese syllabic characters.

This function tests for the presence of Hiragana and Katakana syllabic 
characters in the font, collectively called kana. These characters are not 
sufficient for the Japanese language, which also makes use of Chinese characters.

Note: A return value of ETrue implies that the font has a usable set of 
characters. It does not imply exhaustive coverage.

@return ETrue if Japanese characters are supported 
@see HasCJK() */
inline TBool TOpenFontFaceAttribBase::HasKana() const
	{
	return iCoverage[1] & EKanaSets;
	}

/** Tests for support of Korean Hangul characters.

Korean may also make use of Chinese characters.

Note: A return value of ETrue implies that the font has a usable set of 
characters. It does not imply exhaustive coverage.

@return ETrue if Korean characters are supported 
@see HasCJK() */
inline TBool TOpenFontFaceAttribBase::HasHangul() const
	{
	return iCoverage[1] & EHangulSet;
	}

/** Tests for support of Chinese ideographic characters.

These are used in Chinese, Japanese and Korean.

Note: A return value of ETrue implies that the font has a usable set of 
characters. It does not imply exhaustive coverage.

@return ETrue if Chinese ideographs are supported. */
inline TBool TOpenFontFaceAttribBase::HasCJK() const
	{
	return iCoverage[1] & ECJKSet;
	}

/** Tests if the typeface contains symbols only.

@return ETrue if the typeface contains symbols only. */
inline TBool TOpenFontFaceAttribBase::IsSymbol() const
	{
	return iCoverage[0] == 0 && iCoverage[2] == 0 && iCoverage[3] == 0 &&
		   iCoverage[1] & ESymbolSets && !(iCoverage[1] & ~ESymbolSets);
	}

/** Tests if the typeface is inherently bold.

@return ETrue if the typeface is inherently bold. */
inline TBool TOpenFontFaceAttribBase::IsBold() const
	{
	return iStyle & EBold;
	}

/** Tests if the typeface is inherently italic.

@return ETrue if the typeface is inherently italic. */
inline TBool TOpenFontFaceAttribBase::IsItalic() const
	{
	return iStyle & EItalic;
	}

/** Tests if the typeface has serifs.

@return ETrue if the typeface has serifs. */
inline TBool TOpenFontFaceAttribBase::IsSerif() const
	{
	return iStyle & ESerif;
	}

/** Tests if all the characters have the same width.

@return ETrue if all the characters have the same width. */
inline TBool TOpenFontFaceAttribBase::IsMonoWidth() const
	{
	return iStyle & EMonoWidth;
	}

/** Gets the typeface's name.

@return Descriptor containing typeface name. */
inline TPtrC TOpenFontFaceAttribBase::Name() const
	{
	return iName;
	}

/** Gets the full name.

The full name of the typeface includes style attributes like Italic, Bold, 
and Cursive.

@return The full name of the typeface.
@see FamilyName() */
inline TPtrC TOpenFontFaceAttrib::FullName() const
	{
	return Name();
	}

/** Gets the family name.

Note: The family name of the typeface does not include style attributes like 
"Italic".

@return The family name of the typeface. 
@see FullName() */
inline TPtrC TOpenFontFaceAttrib::FamilyName() const
	{
	return iFamilyName;
	}

/** Gets the local full name.

The local full name of the typeface includes style attributes like Italic, 
Bold, and Cursive.

The local name of the typeface is the name in the language of the current 
locale, where this is provided by the font file. If the local name is not 
available then the local name will be the same as the ordinary name.

@return The local full name of the typeface. */
inline TPtrC TOpenFontFaceAttrib::LocalFullName() const
	{
	return iLocalFullName;
	}

/** Gets the local family name.

The local name of the typeface is the name in the language of the current 
locale, where this is provided by the font file. If the local name is not 
available then the local name will be the same as the ordinary name.

Note: The family name of the typeface does not include style attributes like 
'Italic'.

@return The local family name of the typeface. */
inline TPtrC TOpenFontFaceAttrib::LocalFamilyName() const
	{
	return iLocalFamilyName;
	}

/** Gets the short full name.

This is the full name of the typeface, truncated to KMaxTypefaceNameLength, 
if necessary.

Note: Short names are names truncated to KMaxTypefaceNameLength (24) characters 
where necessary so that they can be used in the TTypeFace class. The Open 
Font Framework allows 32 characters as a maximum name length.

@return The short full name of the typeface.
@see FullName() */
inline TPtrC TOpenFontFaceAttrib::ShortFullName() const
	{
	// Can't use TDesC::Left for this because it panics if the desired length is > the existing length!
	return TPtrC(iName.Ptr(),Min(iName.Length(),KMaxTypefaceNameLength));
	}

/** Gets the short family name.

This is the family name, truncated to KMaxTypefaceNameLength, if necessary.

Note: Short names are names truncated to KMaxTypefaceNameLength (24) characters 
where necessary so that they can be used in the TTypeFace class. The Open 
Font Framework allows 32 characters as a maximum name length.

@return The short family name of the typeface. 
@see FamilyName() */
inline TPtrC TOpenFontFaceAttrib::ShortFamilyName() const
	{
	return TPtrC(iFamilyName.Ptr(),Min(iFamilyName.Length(),KMaxTypefaceNameLength));
	}

/** Gets the short local full name.

This is the local full name of the typeface, truncated to KMaxTypefaceNameLength, 
if necessary.

Note: Short names are names truncated to KMaxTypefaceNameLength (24) characters 
where necessary so that they can be used in the TTypeFace class. The Open 
Font Framework allows 32 characters as a maximum name length.

@return The short local full name of the typeface. 
@see LocalFullName() */
inline TPtrC TOpenFontFaceAttrib::ShortLocalFullName() const
	{
	return TPtrC(iLocalFullName.Ptr(),Min(iLocalFullName.Length(),KMaxTypefaceNameLength));
	}

/** Gets the short local family name.

This is the local family name of the typeface, truncated to KMaxTypefaceNameLength, 
if necessary.

Note: Short names are names truncated to KMaxTypefaceNameLength (24) characters 
where necessary so that they can be used in the TTypeFace class. The Open 
Font Framework allows 32 characters as a maximum name length.

@return The short local family name of the typeface.
@see LocalFamilyName() */
inline TPtrC TOpenFontFaceAttrib::ShortLocalFamilyName() const
	{
	return TPtrC(iLocalFamilyName.Ptr(),Min(iLocalFamilyName.Length(),KMaxTypefaceNameLength));
	}

/** Gets a pointer to the sets of flags that indicate the font's Unicode coverage.

Each flag that is set represents a supported Unicode range. The mapping is 
defined in the TrueType documentation under the OS/2 table. 

Note: Some useful subsets are defined as anonymous enumerated constants at the end 
of this class, see ELatinSet etc.

@return A pointer to the flags that indicate the font's Unicode coverage. 
The flags are stored in an array of four 32-bit integers. When no information 
is available, all four integers are zero.
@see SetCoverage() */
inline const TUint* TOpenFontFaceAttribBase::Coverage() const
	{
	return iCoverage;
	}

/** Gets the minimum typeface size.

This is the smallest size that can be drawn legibly.

@return The minimum typeface size (in pixels). */
inline TInt TOpenFontFaceAttrib::MinSizeInPixels() const
	{
	return iMinSizeInPixels;
	}

/** Sets the name attribute.

@param aName Descriptor containing typeface name. */
inline void TOpenFontFaceAttribBase::SetName(const TDesC& aName)
	{
	iName = TPtrC(aName.Ptr(),Min(aName.Length(),(TInt)ENameLength));
	}

/** Sets the full name.

@param aName The full name of the typeface.
@see FullName() */
inline void TOpenFontFaceAttrib::SetFullName(const TDesC& aName)
	{
	SetName(aName);
	}

/** Sets the family name.

@param aName The family name of the typeface. 
@see FamilyName() */
inline void TOpenFontFaceAttrib::SetFamilyName(const TDesC& aName)
	{
	iFamilyName = TPtrC(aName.Ptr(),Min(aName.Length(),(TInt)ENameLength));
	}

/** Sets the local full name.

@param aName The local full name of the typeface. 
@see LocalFullName() */
inline void TOpenFontFaceAttrib::SetLocalFullName(const TDesC& aName)
	{
	iLocalFullName = TPtrC(aName.Ptr(),Min(aName.Length(),(TInt)ENameLength));
	}

/** Sets the local family name.

@param aName The local family name of the typeface. 
@see LocalFamilyName() */
inline void TOpenFontFaceAttrib::SetLocalFamilyName(const TDesC& aName)
	{
	iLocalFamilyName = TPtrC(aName.Ptr(),Min(aName.Length(),(TInt)ENameLength));
	}

/** Sets the coverage flags.

The flags are held in four 32 bit integers. Each flag that is set represents 
a range of Unicode characters that is supported by the typeface: Latin, 
Greek, Cyrillic etc. The mapping is defined in the TrueType documentation 
under the OS/2 table.

Note: Some useful subsets are defined as anonymous enumerated constants at 
the end of this class, see ELatinSet etc.

@param aCoverage0 The first set of coverage flags (bits 0-31).
@param aCoverage1 The second set of coverage flags (bits 32-63).
@param aCoverage2 The third set of coverage flags (bits 64-95). 
@param aCoverage3 The fourth set of coverage flags (bits 96-127). */
inline void TOpenFontFaceAttribBase::SetCoverage(TUint aCoverage0,TUint aCoverage1,TUint aCoverage2,TUint aCoverage3)
	{
	iCoverage[0] = aCoverage0;
	iCoverage[1] = aCoverage1;
	iCoverage[2] = aCoverage2;
	iCoverage[3] = aCoverage3;
	}

/** Set the minimum typeface size.

This is the smallest size that can be drawn legibly.

@param aSize Sets the minimum typeface size (in pixels). 
@see MinSizeInPixels() */
inline void TOpenFontFaceAttrib::SetMinSizeInPixels(TInt aSize)
	{
	iMinSizeInPixels = aSize;
	}

/** Equality operator.

Compares this and another set of font attributes, including the coverage, 
the family name, the local full name, the local family name, and the minimum 
size in pixels.

@param aAttrib Contains the font attributes and names to compare.
@return ETrue if all values are equal, EFalse if not. */
inline TBool TOpenFontFaceAttrib::operator==(const TOpenFontFaceAttrib& aAttrib) const
	{
	return TOpenFontFaceAttribBase::operator==(aAttrib) &&
		   iFamilyName == aAttrib.iFamilyName &&
		   iLocalFullName == aAttrib.iLocalFullName &&
		   iLocalFamilyName == aAttrib.iLocalFamilyName &&
		   iMinSizeInPixels == aAttrib.iMinSizeInPixels;
	}

/** Sets the bold attribute.

@param aBold The bold attribute takes this value: ETrue or EFalse. */
inline void TOpenFontFaceAttribBase::SetBold(TBool aBold)
	{
	if (aBold)
		iStyle |= EBold;
	else
		iStyle &= ~EBold;
	}

/** Sets the italic attribute.

@param aItalic The italic attribute takes this value   ETrue or EFalse. */
inline void TOpenFontFaceAttribBase::SetItalic(TBool aItalic)
	{
	if (aItalic)
		iStyle |= EItalic;
	else
		iStyle &= ~EItalic;
	}

/** Sets the serif attribute.

@param aSerif The serif attribute takes this value  ETrue or EFalse. */
inline void TOpenFontFaceAttribBase::SetSerif(TBool aSerif)
	{
	if (aSerif)
		iStyle |= ESerif;
	else
		iStyle &= ~ESerif;
	}

/** Sets the mono-width attribute.

@param aMonoWidth The mono-width attribute takes this value: ETrue or EFalse. */
inline void TOpenFontFaceAttribBase::SetMonoWidth(TBool aMonoWidth)
	{
	if (aMonoWidth)
		iStyle |= EMonoWidth;
	else
		iStyle &= ~EMonoWidth;
	}

/** Equality operator. 

Compares this and a specified set of font attributes, including the coverage 
and the typeface name.

In version 6.1, and earlier, the return value was TInt.

@param aAttrib The font attributes to compare. This is an object of TOpenFontFaceAttribBase 
or of a derived class. 
@return ETrue if the values are equal. */
inline TBool TOpenFontFaceAttribBase::operator==(const TOpenFontFaceAttribBase& aAttrib) const
	{
	return iStyle == aAttrib.iStyle &&
		   iCoverage[0] == aAttrib.iCoverage[0] &&
		   iCoverage[1] == aAttrib.iCoverage[1] &&
		   iCoverage[2] == aAttrib.iCoverage[2] &&
		   iCoverage[3] == aAttrib.iCoverage[3] &&
		   iName.CompareF(aAttrib.iName) == 0;
	}

/** Compares this and another open font specification. Compares all the properties 
of the two fonts.
@publishedAll
@released
@param aFontSpec The open font specification to compare with this one.
@return ETrue if all values are equal, EFalse if not.
*/
inline TBool TOpenFontSpec::operator==(const TOpenFontSpec& aOpenFontSpec) const
	{
	return
		iHeight			== aOpenFontSpec.iHeight &&
		iWidthFactor	== aOpenFontSpec.iWidthFactor &&
		iSlantFactor	== aOpenFontSpec.iSlantFactor &&
		iBitmapType		== aOpenFontSpec.iBitmapType &&
		iEffects		== aOpenFontSpec.iEffects &&
		iSymbol			== aOpenFontSpec.iSymbol &&
		iPrintPosition	== aOpenFontSpec.iPrintPosition &&
		TOpenFontFaceAttribBase::operator==(aOpenFontSpec);
	}

/** Gets the height of the font.

@return The height of the font, in pixels or twips. 
@see SetHeight() */
inline TInt TOpenFontSpec::Height() const
	{
	return iHeight;
	}

/** Gets the algorithmic width factor.

@return The algorithmic width factor as a 16.16 fixed-point number.
@see SetWidthFactor() */
inline TInt32 TOpenFontSpec::WidthFactor() const
	{
	return iWidthFactor;
	}

/** Gets the algorithmic slant factor.

@return The algorithmic slant factor as a 16.16 fixed-point number.
@see SetSlantFactor() */
inline TInt32 TOpenFontSpec::SlantFactor() const
	{
	return iSlantFactor;
	}

/** Gets the anti-aliasing setting for the font, as set by SetBitmapType().

@return Indicates whether or not the font should be drawn using anti-aliasing. */
inline TGlyphBitmapType TOpenFontSpec::BitmapType() const
	{
	return (TGlyphBitmapType)iBitmapType;
	}

/** Gets the font effects flags.
Because the flags encoded in the Effects are anonymous, the return value should only
 be tested for the specific bits that are of interest, and never tested as a whole.
@publishedAll
@released
@return The font effects flags.
@see TOpenFontSpec::SetEffects()
*/
inline TUint32 TOpenFontSpec::Effects() const
	{
	return iEffects;
	}

/** Gets the print position.

@return The print position. */
inline TFontPrintPosition TOpenFontSpec::PrintPosition() const
	{
	return iPrintPosition;
	}

/** Sets the font's height.

@param aHeight The font's height, in pixels or twips.
@see Height() */
inline void TOpenFontSpec::SetHeight(TInt aHeight)
	{
	iHeight = aHeight;
	}

/** Sets the algorithmic width factor.

The width factor is multiplied by the pixel's x position to get the new position, 
causing characters to become wider or narrower. A width factor of 1 (65536 
in 16.16 fixed-point number format) should be used if the character width 
is not to be changed.

@param aWidthFactor The algorithmic width factor as a 16.16 fixed-point number.
@see WidthFactor() */
inline void TOpenFontSpec::SetWidthFactor(TInt32 aWidthFactor)
	{
	iWidthFactor = aWidthFactor;
	}

/** Sets the algorithmic slant factor.

Note: The slant factor is used to create an italic effect for characters which 
do not have an italic glyph in the typeface. When slanting is active, pixel x 
co-ordinates are shifted by a factor relative to the y co-ordinate (i.e. x 
= x + y x slant factor).

The slant factor is a 32 bit, 16.16 fixed-point number. This means that the 
first 16 bits are treated as a whole number, and the second 16 as the fractional 
part. e.g. if aSlantFactor=0, there is no slant. If aSlantFactor=65536 this 
is equivalent to an integer slant value of 1, which causes a 45 degree slant 
on the character.

@param aSlantFactor The slant factor as a 16.16 fixed-point number.
@see SlantFactor() */
inline void TOpenFontSpec::SetSlantFactor(TInt32 aSlantFactor)
	{
	iSlantFactor = aSlantFactor;
	}

/** Sets whether the font should be drawn using anti-aliasing. If set, this value 
overrides the default setting (set by CFbsTypefaceStore::SetDefaultBitmapType()) 
for this font.

Anti-aliasing can only be used for scalable fonts. There is currently no anti-aliasing 
support for bitmapped fonts.

@param aBitmapType Indicates whether or not the font should be drawn using 
anti-aliasing. */
inline void TOpenFontSpec::SetBitmapType(TGlyphBitmapType aBitmapType)
	{
	iBitmapType = aBitmapType;
	}

/** Sets the font effects flags.
Prior to calling this routine, the value from Effects() should be read,
 and its flags modified as required, before passing them back in.
@publishedAll
@released
@param aEffect The font effects flags to be set.
@see TOpenFontSpec::Effects()
*/
inline void TOpenFontSpec::SetEffects(TUint32 aEffects)
	{
	iEffects = aEffects;
	}

/** Gets the font file's UID.

@return The uid of the file. */
inline TUid COpenFontFile::Uid() const
	{
	return iUid;
	}

/** Gets the full path and filename of the font file

This is the filename that was passed to the constructor when the object is 
created.

@return The filename of the font file. */
inline const TDesC& COpenFontFile::FileName() const
	{
	return iFileName;
	}

/** Gets the typeface at a specified index in the typeface attribute array.

@param aFaceIndex The index of the typeface for which the attributes are required.
@return The attributes of the typeface with the specified index.
@see AddFaceL()
@see FaceCount() */
inline const TOpenFontFaceAttrib& COpenFontFile::FaceAttrib(TInt aFaceIndex) const
	{
	return iFaceAttrib[aFaceIndex];
	}

/** Gets the number of typefaces in the typeface attributes array.

This is the number of typefaces in the font file: the attributes for each 
typeface should be loaded into the array when the derived object is constructed.

@return The number of typefaces in the font file.
@see AddFaceL()
@see FaceAttrib() */
inline TInt COpenFontFile::FaceCount() const
	{
	return iFaceAttrib.Count();
	}

/** Increments a reference count by one.

@see DecRefCount() */
inline void COpenFontFile::IncRefCount()
	{
	iRefCount++;
	}

/** Decrement a reference count by one.

@return ETrue if the reference count has reached zero (i.e. is less than or 
equal to zero); EFalse if the reference count has not yet reached zero (i.e. 
is positive).
@see IncRefCount() */
inline TBool COpenFontFile::DecRefCount()
	{
	iRefCount--;
	return iRefCount <= 0;
	}

/** Default C++ constructor. */
inline COpenFontRasterizerContext::COpenFontRasterizerContext():
	iGlyphData(NULL)
	{
	}

/** Start writing the glyph data.

Use this function to initialise the buffer to which the glyph bitmap is to 
be written. Call WriteGlyphBit() to add bits to the buffer.

@param aGlyphData A pointer to the glyph data. */
inline void COpenFontRasterizerContext::StartGlyph(TOpenFontGlyphData* aGlyphData)
	{
	aGlyphData->SetPointersToInternalBuffers();
	iGlyphData = aGlyphData;
	iGlyphDataStart = iGlyphDataPtr = aGlyphData->BufferStart();
	// Allow 4 extra bytes; BITGDI requires this.
	iGlyphDataEnd = aGlyphData->BufferEnd() - 4;
	iGlyphBit = 1;
	*iGlyphDataPtr = 0;
	iBytesNeeded = 1;
	iOverflow = FALSE;
	}

/** Writes a bit to the glyph buffer.

Before calling this function you should first call StartGlyph().

@param aBit The bit to be added to the buffer. */
inline void COpenFontRasterizerContext::WriteGlyphBit(TInt aBit)
	{
	if (aBit && !iOverflow)
		*iGlyphDataPtr |= iGlyphBit;
	iGlyphBit <<= 1;
	if (iGlyphBit == 256)
		{
		iGlyphBit = 1;
		iBytesNeeded++;
		if (++iGlyphDataPtr < iGlyphDataEnd)
			*iGlyphDataPtr = 0;
		else
			iOverflow = TRUE;
		}
	}

/** Writes a byte to the glyph buffer.

Before calling this function you should first call StartGlyph().

@param aByte The byte to be added to the buffer. */	
inline void COpenFontRasterizerContext::WriteGlyphByte(TInt aByte)
 	{
	if (iGlyphDataPtr < iGlyphDataEnd)
		*iGlyphDataPtr++ = (TUint8)aByte;
	else
		iOverflow = TRUE;
	iBytesNeeded++;
	}

/** Completes writing glyph data.

Use this function to complete writing the glyph bitmap to the buffer. Call 
it after adding all necessary bits using WriteGlyphBit(). */
inline void COpenFontRasterizerContext::EndGlyph()
	{
	// Add 4 bytes to the data size; some BITGDI functions require this.
	iGlyphData->SetBytesNeeded(iBytesNeeded + 4);
	iGlyphData = NULL;
	}

/** Tests whether the bitmap buffer is large enough to hold the bitmap.

@return ETrue if the bitmap will overflow its buffer, otherwise EFalse. */
inline TBool TOpenFontGlyphData::Overflow() const
	{
	return iBytesNeeded > iBitmapBufferSize;
	}

/** Gets the number of bytes needed to store the glyph bitmap.

@return The number of bytes needed to store the glyph bitmap.
@see SetBytesNeeded() */
inline TInt TOpenFontGlyphData::BytesNeeded() const
	{
	return iBytesNeeded;
	}

/** Gets a constant pointer descriptor containing the bitmap this object 
represents. 

@return Pointer descriptor for the glyph. */
inline TPtrC8 TOpenFontGlyphData::Bitmap() const
	{
	return TPtrC8(iBitmap,iBytesNeeded);
	}

/** Gets a pointer to the bitmap.

This points either to the buffer used to write the bitmap when it is first 
rasterized, or to the cache if the character was already rasterized.

@return A pointer to the bitmap. */
inline const TUint8* TOpenFontGlyphData::BitmapPointer() const
	{
	return iBitmap;
	}

/** Gets the character's metrics.

@return The character's open font metrics. */
inline const TOpenFontCharMetrics* TOpenFontGlyphData::Metrics() const
	{
	return iMetrics;
	}

/** Gets a pointer to the start of the bitmap buffer.

@return A pointer to the start of the bitmap buffer. */
inline TUint8* TOpenFontGlyphData::BufferStart()
	{
	return iBitmapBuffer;
	}

/** Gets a pointer to the end of the bitmap buffer.

@return A pointer to the end of the bitmap buffer. */
inline TUint8* TOpenFontGlyphData::BufferEnd()
	{
	return iBitmapBuffer + iBitmapBufferSize;
	}

/** Sets the number of bytes needed to store the glyph bitmap.

@param aBytes The number of bytes needed to store the glyph bitmap.
@see BytesNeeded() */
inline void TOpenFontGlyphData::SetBytesNeeded(TInt aBytes)
	{
	iBytesNeeded = aBytes;
	}

/** Sets the pointer to the bitmap buffer.

@param aBitmap The pointer to the bitmap buffer. */
inline void TOpenFontGlyphData::SetBitmapPointer(const TUint8* aBitmap)
	{
	iBitmap = aBitmap;
	}

/** Sets the character's metrics, passing a pointer to a TOpenFontCharMetrics object.

@param aMetrics A pointer to the character's open font character metrics. */
inline void TOpenFontGlyphData::SetMetricsPointer(const TOpenFontCharMetrics* aMetrics)
	{
	iMetrics = aMetrics;
	}

/** Sets the bitmap and metrics pointers to point to the internal buffers.

The alternative is that the pointers point to the cache of metrics and bitmaps 
that have already been rasterized. */
inline void TOpenFontGlyphData::SetPointersToInternalBuffers()
	{
	iBitmap = iBitmapBuffer;
	iMetrics = &iMetricsBuffer;
	}

/** Sets the character's metrics, passing a reference to a TOpenFontCharMetrics 
object.

@param	aMetrics	The character's open font character metrics. */
inline void TOpenFontGlyphData::SetMetrics(TOpenFontCharMetrics& aMetrics)
	{
	iMetricsBuffer = aMetrics;
	iMetrics = &iMetricsBuffer;
	}

/** Sets the glyph index.

@param	aGlyphIndex	The glyph index. */
inline void TOpenFontGlyphData::SetGlyphIndex(TInt aGlyphIndex)
	{
	iGlyphIndex = aGlyphIndex;
	}

/** Uses ECOM plug-in framework to instantiate the shaper factory interface 
implementation given its implementation UID. 

@param	aInterfaceImplUid	The UID of the interface implementation required

@return	CShaperFactory*	A pointer to a CShaperFactory object. */
inline CShaperFactory* CShaperFactory::NewL(TUid aInterfaceImplUid)
	{
	return reinterpret_cast <CShaperFactory*> (
		REComSession::CreateImplementationL( 
			aInterfaceImplUid,
			_FOFF(CShaperFactory, iDtor_ID_Key))); 
	}

/** Default destructor */
inline CShaperFactory::~CShaperFactory()
{
	REComSession::DestroyedImplementation(iDtor_ID_Key);
}

#endif	// __OPENFONT_H__
