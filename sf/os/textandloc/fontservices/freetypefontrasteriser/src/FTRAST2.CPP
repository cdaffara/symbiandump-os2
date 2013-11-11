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
* FTRAST.CPP
* Wrapper code to connect FreeType with the EPOC rasterizer framework.
* This version uses FreeType 2.2.1
* Secure Rasterizer Plugin. The rasterizer is now implemented as 
* an ECOM Plugin. Secure FBServ uses ECOM APIs provided to load the 
* plugin.
*
*/


#include <e32uid.h>
#include <f32file.h>
#include <charconv.h>
#include <sys/reent.h>
#include <openfont.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_INTERNAL_TRUETYPE_TYPES_H
#include <ftsnames.h>
#include <ecom/ecom.h>
#include <ecom/implementationproxy.h>
#include <gdi.h>
#include <e32hashtab.h>
#include <graphics/openfontrasterizer.h>
#include <graphics/gdi/glyphsample.h>
#include <graphics/openfontconstants.h>

static const TInt KOneIn16Dot16FixedPointFormat = 65536;

class CFreeTypeContext;

class CFreeTypeRasterizer : public COpenFontRasterizer
	{
public:
	static COpenFontRasterizer* NewL();
	virtual ~CFreeTypeRasterizer();
	// COpenFontRasterizer implementation
	COpenFontFile* NewFontFileL(TInt aUid, const TDesC& aFileName, RFs& aFileSession);
private:
	CFreeTypeRasterizer();
private:
	CFreeTypeContext* iContext;
	};

NONSHARABLE_CLASS(CFaceListItem) : public CBase
	{
	friend class CFaceList;
public:
	virtual ~CFaceListItem();
	static CFaceListItem* NewL(CFreeTypeContext* aContext, const TText8* aFileName, TInt aFaceIndex);
	void SetFaceNamesL(TOpenFontFaceAttrib& aAttrib) const;
	void GetBiggestGlyphsFromSampleL(
		TInt aScript, TInt& aUnicodeAscender, TInt& aUnicodeDescender) const;
	TBool LoadGlyphAndGetAscenderDescender(
		TUint32 aCode, TInt32& aAscender, TInt32& aDescender) const;
	TInt DeriveDesignHeightFromMaxHeightWithGlyphSampleL(
		TInt aMaxHeightInPixel, TInt aUnicodeAscender, TInt aUnicodeDescender) const;
	TInt DeriveDesignHeightFromMaxHeightWithBBoxL(TInt aMaxHeightInPixel) const;
	FT_Face Face(void) const;
private:
	static void GetDesiredLocalLanguageAndMask(TUint16& aDesiredLocalLanguage, TUint16& aLocalLanguageMask);
	static TInt CopyName(TInt aDestMaxLen, TUint16* aDest, const TUint8* aSrc);
	CFaceListItem();
	void ConstructL(CFreeTypeContext* aContext, const TText8* aFileName, TInt aFaceIndex);
	TInt GetFamilyAndStyleNames(TInt aDestMaxLen, TUint16* aDest, TInt& aFamilyNameLen) const;
	void SetTTFFaceNamesL(TOpenFontFaceAttrib& aAttrib) const;
private:
	CFaceListItem*	iNext;
	const TText8*	iFileName;	// owned by the CFreeTypeFontFile object
	TInt			iFaceIndex;
	FT_Face			iFace;
	};

NONSHARABLE_CLASS(CFaceList) : public CBase
	{
public:
	CFaceList();
	virtual ~CFaceList();
	CFaceListItem* LoadFaceL(CFreeTypeContext* aContext, const TText8* aFileName, TInt aFaceIndex);
	void DeleteFace(const TText8* aFileName, TInt aFaceIndex);
private:
	static TInt MemoryUsed(void);
	void DeleteLastFace(void);
private:
	/*
	The memory threshold is not an absolute limit because we must allow
	at least one typeface to be loaded, but if it is exceeded typefaces
	will be unloaded until just one is left (the one being used) or until
	the memory available no longer exceeds the threshold.
	*/
	enum
		{
		EMemoryThreshold = 1024 * 1024
		};

	CFaceListItem* iFirstItem;
	};

NONSHARABLE_CLASS(CFreeTypeContext) : public COpenFontRasterizerContext
	{
public:
	static CFreeTypeContext* NewL();
	virtual ~CFreeTypeContext();
	void TranslateMonochromeGlyphBitmap (FT_GlyphSlot aGlyphSlot, TOpenFontGlyphData* aGlyphData);
	void TranslateAntiAliasedGlyphBitmap(FT_GlyphSlot aGlyphSlot, TOpenFontGlyphData* aGlyphData);
	FT_Library Library(void) const;
	CFaceListItem* LoadFaceL(const TText8* aFileName, TInt aFaceIndex);
	void DeleteFace(const TText8* aFileName, TInt aFaceIndex);
private:
	void ConstructL(void);
	void WriteMonoData(TUint8 aData, TInt aBitCount);
private:
	FT_Library iLibrary;
	CFaceList iFaceList;
	};

NONSHARABLE_CLASS(CFreeTypeFontFile) : public COpenFontFile
	{
public:
	static CFreeTypeFontFile* NewL(
		CFreeTypeContext* aContext, TInt aUid, const TDesC& aFileName, RFs& aFileSession);
	virtual ~CFreeTypeFontFile();
	CFaceListItem* LoadFaceL(TInt aFaceIndex);
	CFaceListItem* LoadFaceAndSetTransformL(TInt aFaceIndex,
		TInt aSizeInPixels, TInt32 aWidthFactor, TInt32 aSlantFactor);
	void RasterizeL(
		TInt aCode, TInt aFaceIndex, TInt aSizeinPixels, TInt32 aWidthFactor, 
		TInt32 aSlantFactor, TGlyphBitmapType aBitmapType, TOpenFontGlyphData* aGlyphData);
	void RasterizeGlyphL(
		TInt aCode, TInt aFaceIndex, TInt aSizeInPixels, TInt32 aWidthFactor,
		TInt32 aSlantFactor, TGlyphBitmapType aBitmapType, TOpenFontGlyphData* aGlyphData);
	// COpenFontFile implementation
	void GetNearestFontInPixelsL(
		RHeap* aHeap, COpenFontSessionCacheList* aSessionCacheList,
		const TOpenFontSpec& aDesiredFontSpec, TInt aPixelWidth, TInt aPixelHeight,
		COpenFont*& aFont, TOpenFontSpec& aActualFontSpec);
	void GetNearestFontToDesignHeightInPixelsL(
		RHeap* aHeap, COpenFontSessionCacheList* aSessionCacheList,
		const TOpenFontSpec& aDesiredFontSpec, TInt aPixelWidth, TInt aPixelHeight,
		COpenFont*& aFont, TOpenFontSpec& aActualFontSpec);
	void GetNearestFontToMaxHeightInPixelsL(
		RHeap* aHeap, COpenFontSessionCacheList* aSessionCacheList,
		const TOpenFontSpec& aDesiredFontSpec, TInt aPixelWidth, TInt aPixelHeight,
		COpenFont*& aFont, TOpenFontSpec& aActualFontSpec, TInt aMaxHeight);
	TBool HasUnicodeCharacterL(TInt aFaceIndex, TInt aCode) const;
	TAny* GetTrueTypeTable(TInt& aError, TInt aFaceIndex, TUint32 aTag, TInt* aLength);
	TInt GetGlyphOutline(TInt aFaceIndex, TUint aCode, 
	        TBool aIsGlyphId, TBool, TAny*& aOutline, TInt &aLength);

private:
	static void SetGlyphMetrics(FT_GlyphSlot aGlyphSlot, TOpenFontGlyphData* aGlyphData);
	void GetNearestFontInPixelsL(
		RHeap* aHeap, COpenFontSessionCacheList* aSessionCacheList,
		const TOpenFontSpec& aDesiredFontSpec, TInt aPixelWidth, TInt aPixelHeight,
		COpenFont*& aFont, TOpenFontSpec& aActualFontSpec, TInt aMaxHeight);
	CFreeTypeFontFile(TInt aUid, const TDesC& aFileName);
	void ConstructL(CFreeTypeContext* aContext, const TDesC& aFileName, RFs& aFileSession);
	void CreateFilenameInUTF8L(const TDesC& aFileName, RFs& aFileSession);
	static TUint32 TagHash(const TUint32& aTag);
	static TBool TagId(const TUint32& aTag1, const TUint32& aTag2);

	void DoRasterizeGlyphL(TInt aCode, const CFaceListItem* aFace,
		TGlyphBitmapType aBitmapType, TOpenFontGlyphData* aGlyphData);

private:
	CFreeTypeContext* iContext;
	TText8* iFileName; // in null-terminated UTF8 so that it can be passed to fopen
	RHashMap<TUint32, TPtrC8> iTableStore;
	};

NONSHARABLE_CLASS(CFreeTypeFont) : public COpenFont, public MOpenFontShapingExtension, 
    public MOpenFontTrueTypeExtension, public MOpenFontGlyphOutlineExtension
	{
public:
	static CFreeTypeFont* NewL(
		RHeap* aHeap, COpenFontSessionCacheList* aSessionCacheList, CFreeTypeFontFile* aFontFile,
		TInt aFaceIndex, TInt aSizeInPixels, TInt32 aWidthFactor, TInt32 aSlantFactor,
		TGlyphBitmapType aBitmapType, TInt aMaxHeight, TInt aScript);
	// COpenFont implementation
	void RasterizeL(TInt aCode, TOpenFontGlyphData* aGlyphData);
private:
	CFreeTypeFont(
		RHeap* aHeap, COpenFontSessionCacheList* aSessionCacheList, CFreeTypeFontFile* aFontFile,
		TInt aFaceIndex, TInt32 aWidthFactor, TInt32 aSlantFactor, TGlyphBitmapType aBitmapType);
	void ConstructL(TInt aSizeInPixels, TInt aMaxHeight, TInt aScript);
	void ComputeMetrics(const FT_Face& aFace, TInt aSizeInPixels, TInt aUnicodeAscender, TInt aUnicodeDescender);
	void ComputeMaxWidth(const FT_Face& aFace);

	// Overload to access extended API
	void ExtendedInterface(TUid aUid, TAny*& aParam);

	// virtual functions from MOpenFontShapingExtension
	void RasterizeGlyphL(TInt aCode,TOpenFontGlyphData* aGlyphData);
	TInt GlyphIndex(TInt aUnicodeCharacter) const;
	TBool GlyphPointInHintedPixels(TInt aGlyphIndex, TInt aPointNumber,
		TReal& aX, TReal& aY) const;
	TBool GlyphPointInFontUnits(TInt aGlyphIndex, TInt aPointNumber,
		TInt& aX, TInt& aY) const;
	void GetExtensionFontMetrics(
		MOpenFontShapingExtension::TExtensionFontMetrics& aOut);

	// virtual functions from MOpenFontTrueTypeExtension
	TAny* GetTrueTypeTable(TInt& aError, TUint32 aTag, TInt* aLength);
	TInt  ReleaseTrueTypeTable(TAny* aTable);
	TBool HasTrueTypeTable(TUint32 aTag);
	TInt GetGlyphOutline(TUint aCode, TBool aIsGlyphId, 
	            TBool aHinted, TAny*& aOutline, TInt &aLength);

	inline TAny* operator new(TUint aSize, TAny* aBase) __NO_THROW;
	inline TAny* operator new(TUint aSize) __NO_THROW;
	inline void operator delete(void*, TAny*) __NO_THROW;
	inline void operator delete(void*) __NO_THROW;

private:
	TInt32 iWidthFactor;				// algorithmic width factor as a 16.16 fixed-point number
	TInt32 iSlantFactor;				// algorithmic slant factor as a 16.16 fixed-point number
	TGlyphBitmapType iBitmapType;
	};

#ifdef TT_CONFIG_OPTION_BYTECODE_INTERPRETER
#warning Using patented technology!
#endif

inline TAny* CFreeTypeFont::operator new(TUint aSize, TAny* aBase) __NO_THROW
	{ return CBase::operator new(aSize, aBase); }
inline TAny* CFreeTypeFont::operator new(TUint aSize) __NO_THROW
	{ return CBase::operator new(aSize); }

inline void CFreeTypeFont::operator delete(void*, TAny*) __NO_THROW {}
inline void CFreeTypeFont::operator delete(void* a) __NO_THROW
	{
	// We happen to know that the constructor of CFreeTypeFont
	// will always set iHeap correctly before any leave.
	// We would be less certain of this in standard C++ where
	// constructors (and even initializer lists) may throw exceptions.
	CFreeTypeFont* f = reinterpret_cast<CFreeTypeFont*>(a);
	f->iHeap->Free(a);
	}

TInt CFaceList::MemoryUsed()
	{
	TInt total;
	User::AllocSize(total);
	return total;
	}

CFreeTypeContext* CFreeTypeContext::NewL()
	{
	CFreeTypeContext* context = new(ELeave) CFreeTypeContext;
	CleanupStack::PushL(context);
	context->ConstructL();
	CleanupStack::Pop();
	return context;
	}

void CFreeTypeContext::ConstructL()
	{
	if (FT_Init_FreeType(&iLibrary))
		{
		iLibrary = NULL;
		User::Leave(KErrNoMemory);
		}
	}

CFreeTypeContext::~CFreeTypeContext()
	{
	FT_Done_FreeType(iLibrary);
	}

FT_Library CFreeTypeContext::Library() const
	{
	return iLibrary;
	}

CFaceListItem* CFreeTypeContext::LoadFaceL(const TText8* aFileName, TInt aFaceIndex)
	{
	return iFaceList.LoadFaceL(this, aFileName, aFaceIndex);
	}

void CFreeTypeContext::DeleteFace(const TText8* aFileName, TInt aFaceIndex)
	{
	iFaceList.DeleteFace(aFileName, aFaceIndex);
	}

COpenFontRasterizer* CFreeTypeRasterizer::NewL()
	{
	CFreeTypeRasterizer* r = new(ELeave) CFreeTypeRasterizer;
	CleanupStack::PushL(r);
	r->iContext = CFreeTypeContext::NewL();
	CleanupStack::Pop();
	return r;
	}

CFreeTypeRasterizer::CFreeTypeRasterizer()
	{
	}

CFreeTypeRasterizer::~CFreeTypeRasterizer()
	{
	delete iContext;
	CloseSTDLIB();
	}

COpenFontFile* CFreeTypeRasterizer::NewFontFileL(TInt aUid, const TDesC& aFileName, RFs& aFileSession)
	{
	if (4 < aFileName.Length())
		{
		// Allow TrueType fonts
		_LIT(KFntStoreTrueTypeExtension,".ttf");
		// Allow OpenType fonts
		_LIT(KFntStoreOpenTypeExtension,".otf");
		TPtrC16 extension = aFileName.Right(4);
		if (0 == extension.CompareF(KFntStoreTrueTypeExtension)
			|| 0 == extension.CompareF(KFntStoreOpenTypeExtension))
			return CFreeTypeFontFile::NewL(iContext, aUid, aFileName, aFileSession);
		}
	return NULL;
	}

// Exported proxy for instantiation method resolution
// Define the interface UIDs
const TImplementationProxy ImplementationTable[] = 
	{
	IMPLEMENTATION_PROXY_ENTRY(0x101f7f5e, CFreeTypeRasterizer::NewL)
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
	return ImplementationTable;
	}

CFaceListItem* CFreeTypeFontFile::LoadFaceL(TInt aFaceIndex)
	{
	return iContext->LoadFaceL(iFileName, aFaceIndex);
	}

CFaceListItem* CFreeTypeFontFile::LoadFaceAndSetTransformL(TInt aFaceIndex,
	TInt aSizeInPixels, TInt32 aWidthFactor, TInt32 aSlantFactor)
	{
	CFaceListItem* face = iContext->LoadFaceL(iFileName, aFaceIndex);
	if (FT_Set_Pixel_Sizes(face->Face(), aSizeInPixels, aSizeInPixels))
		{
		User::Leave(KErrGeneral);
		}
	
	if (aWidthFactor != 0 &&
		(aWidthFactor != KOneIn16Dot16FixedPointFormat || aSlantFactor != 0))
		{
		FT_Matrix matrix;
		matrix.xx = aWidthFactor;
		matrix.xy = aSlantFactor;
		matrix.yx = 0;
		matrix.yy = KOneIn16Dot16FixedPointFormat;
		FT_Set_Transform(face->Face(),&matrix,NULL);
		}
	else
		FT_Set_Transform(face->Face(),NULL,NULL);
	return face;
	}


CFreeTypeFontFile* CFreeTypeFontFile::NewL(CFreeTypeContext* aContext, TInt aUid, const TDesC& aFileName, RFs& aFileSession)
	{
	CFreeTypeFontFile* f = new(ELeave) CFreeTypeFontFile(aUid, aFileName);
	CleanupStack::PushL(f);
	f->ConstructL(aContext, aFileName, aFileSession);
	CleanupStack::Pop();
	return f;
	}

/** Forms a hash value from a TrueType-style tag.
@param aTag
	Each byte will contain a lower-case ASCII value (or possibly a number).
@return
	Hash value. Because we want to avoid collisions, we will move the bottom
	four bits of all four bytes into non-overlapping positions. */
TUint32 CFreeTypeFontFile::TagHash(const TUint32& aTag)
	{
	TUint32 t = aTag;
	// The most useful bits are in the bottom nybbles of each byte
	t ^= t >> 12 | t << 20;
	return t ^ (t << 15 | t >> 17);
	}

TBool CFreeTypeFontFile::TagId(const TUint32& aTag1, const TUint32& aTag2)
	{
	return aTag1 == aTag2;
	}

CFreeTypeFontFile::CFreeTypeFontFile(TInt aUid, const TDesC& aFileName):
	COpenFontFile(aUid, aFileName),
	iTableStore(THashFunction32<TUint32>(CFreeTypeFontFile::TagHash),
		TIdentityRelation<TUint32>(CFreeTypeFontFile::TagId))
	{
	}

CFreeTypeFontFile::~CFreeTypeFontFile()
	{
	const TInt faces = FaceCount();
	for (TInt i = 0; i < faces; i++)
		iContext->DeleteFace(iFileName, i);
	delete[] iFileName;
	THashMapIter<TUint32, TPtrC8> it(iTableStore);
	it.NextValue();
	while (it.CurrentValue())
		{
		delete it.CurrentValue()->Ptr();
		it.NextValue();
		}
	iTableStore.Close();
	}

void CFreeTypeFontFile::ConstructL(CFreeTypeContext* aContext, const TDesC& aFileName, RFs& aFileSession)
	{
	iContext = aContext;

	CreateFilenameInUTF8L(aFileName, aFileSession);

	// Get the filename as a fallback for the typeface name.
	TParse p;
	p.Set(aFileName, NULL, NULL);
	const TPtrC name = p.Name();

	// Load the properties of all the faces in the file.
	//
	// A file may contain one face (e.g. TTF - TrueType Font) or many (e.g. TTC - TrueType Collection).
	// The number is determined from a value stored in the properties of the first face.
	//
	for (TInt face_index = 0, faces = 1; face_index < faces; face_index++)
		{
		// Load the face.
		const CFaceListItem* face = iContext->LoadFaceL(iFileName, face_index);

		// If this is the first face, get the total number of faces.
		if (face_index == 0)
			{
			faces = face->Face()->num_faces;
			if (faces < 1)
				User::Leave(KErrGeneral);
			}

		// Set the face names to the filename as a fallback in case SetFaceNameL doesn't find appropriate names.
		TOpenFontFaceAttrib attrib;
		attrib.SetFullName(name);
		attrib.SetFamilyName(name);
		attrib.SetLocalFullName(name);
		attrib.SetLocalFamilyName(name);

		// Set the face names to those stored in the file if possible.
		face->SetFaceNamesL(attrib);

		// Set the face attributes exposed via FT_Face.
		attrib.SetBold(FT_STYLE_FLAG_BOLD & face->Face()->style_flags);
		attrib.SetItalic(FT_STYLE_FLAG_ITALIC & face->Face()->style_flags);
		attrib.SetMonoWidth(FT_FACE_FLAG_FIXED_WIDTH & face->Face()->face_flags);

		// Set attributes present in TrueType information but not exposed via FT_Face.
		if (FT_FACE_FLAG_SFNT & face->Face()->face_flags)
			{
			const TT_Face tt_face = (TT_Face) face->Face();
			attrib.SetMinSizeInPixels(tt_face->header.Lowest_Rec_PPEM);
			attrib.SetCoverage(tt_face->os2.ulUnicodeRange1,tt_face->os2.ulUnicodeRange2,
							   tt_face->os2.ulUnicodeRange3,tt_face->os2.ulUnicodeRange4);
			const TInt serif_style = tt_face->os2.panose[1];
			attrib.SetSerif((serif_style >= 2 && serif_style <= 10) || serif_style >= 14);
			}

		// Add the face to the array of faces owned by the file.
		AddFaceL(attrib);

		// Unload the face from the cache; loading a font file should not result in the face remaining in the face cache.
		iContext->DeleteFace(iFileName, face_index);
		}
	}

void CFreeTypeFontFile::CreateFilenameInUTF8L(const TDesC& aFileName, RFs& aFileSession)
	{
	CCnvCharacterSetConverter* converter = CCnvCharacterSetConverter::NewLC();
	if (CCnvCharacterSetConverter::EAvailable !=
		converter->PrepareToConvertToOrFromL(KCharacterSetIdentifierUtf8,aFileSession))
		{
		User::Leave(KErrGeneral);
		}
	const TInt filename_length = aFileName.Length();
	TInt utf8_length = 0;
	for (TInt i = 0; i < filename_length; i++)
		{
		const TText c = aFileName[i];
		if (c < 0x80)
			utf8_length++;
		else if (c < 0x800)
			utf8_length += 2;
		else
			utf8_length += 3; // allow 3 bytes, even for parts of surrogate pairs, in case CHARCONV doesn't handle them
		}
	iFileName = new(ELeave) TText8[utf8_length + 1];
	TPtr8 q(iFileName, filename_length * 4);
	converter->ConvertFromUnicode(q, aFileName);
	iFileName[q.Length()] = 0;
	CleanupStack::PopAndDestroy(converter);
	}

#ifndef WORDS_BIGENDIAN
static void SwapWords(TUint16* aStart, TInt aCount)
	{
	TUint8* p = (TUint8*)aStart;
	while (aCount-- > 0)
		{
		const TUint8 temp = *p;
		*p = p[1];
		p[1] = temp;
		p += 2;
		}
	}
#endif

/*
Set the typeface names (family name and full name, in English and the local language).
If this is a TrueType or OpenType font get the names from the TrueType name table,
which are loaded by FreeType and stored in the TT_Face structure.
*/
void CFaceListItem::SetFaceNamesL(TOpenFontFaceAttrib& aAttrib) const
	{
	TInt familyNameLen = 0;
	TUint16 name[TOpenFontFaceAttrib::ENameLength];
	const TInt totalLen = GetFamilyAndStyleNames(TOpenFontFaceAttrib::ENameLength, name, familyNameLen);
	const TPtrC family_name(name, familyNameLen);
	const TPtrC full_name(name, totalLen);

	// Set the local and non-local family and full names to these default versions.
	aAttrib.SetFamilyName(family_name);
	aAttrib.SetLocalFamilyName(family_name);
	aAttrib.SetFullName(full_name);
	aAttrib.SetLocalFullName(full_name);

	// If this is a TrueType font, replace the default names with those in the name table if any.
	if (FT_FACE_FLAG_SFNT & iFace->face_flags)
		{
		SetTTFFaceNamesL(aAttrib);
		}
	}

TInt CFaceListItem::GetFamilyAndStyleNames(TInt aDestMaxLen, TUint16* aDest, TInt& aFamilyNameLen) const
	{
	aFamilyNameLen = 0;
	// Put the family name into <name>. Ignore non-ASCII characters
	const TUint8* p = (const TUint8*) iFace->family_name;
	if (p)
		aFamilyNameLen = CopyName(aDestMaxLen, aDest, p);

	TInt totalLen = aFamilyNameLen;
	// Append the style name if any,
	p = (const TUint8*) iFace->style_name;
	if (p && aFamilyNameLen < aDestMaxLen)
		{
		aDest[totalLen++] = ' ';
		totalLen += CopyName(aDestMaxLen - totalLen, aDest + totalLen, p);
		}
	return totalLen;
	}

//@pre:  aSrc != NULL
TInt CFaceListItem::CopyName(TInt aDestMaxLen, TUint16* aDest, const TUint8* aSrc)
	{
	TInt len = 0;
	while (len < aDestMaxLen && *aSrc)
		{
		if (*aSrc <= 127)
			aDest[len++] = *aSrc;
		aSrc++;
		}
	return len;
	}

void CFaceListItem::GetDesiredLocalLanguageAndMask(TUint16& aDesiredLocalLanguage, TUint16& aLocalLanguageMask)
	{
	switch (User::Language())
		{
		case ELangEnglish: aDesiredLocalLanguage = 0x0809; aLocalLanguageMask = 0xFFFF; break;
		case ELangSwissFrench:
		case ELangBelgianFrench:
		case ELangInternationalFrench:
		case ELangFrench: aDesiredLocalLanguage = 0x0C; break;
		case ELangSwissGerman:
		case ELangAustrian:
		case ELangGerman: aDesiredLocalLanguage = 0x07; break;
		case ELangSpanish:  aDesiredLocalLanguage = 0x0A; break;
		case ELangItalian: aDesiredLocalLanguage = 0x10; break;
		case ELangSwedish: aDesiredLocalLanguage = 0x1D; break;
		case ELangDanish: aDesiredLocalLanguage = 0x06; break;
		case ELangNorwegian: aDesiredLocalLanguage = 0x14; break;
		case ELangFinnish: aDesiredLocalLanguage = 0x0B; break;
		case ELangPortuguese: aDesiredLocalLanguage = 0x16; break;
		case ELangTurkish: aDesiredLocalLanguage = 0x1F; break;
		case ELangIcelandic: aDesiredLocalLanguage = 0x0F; break;
		case ELangRussian: aDesiredLocalLanguage = 0x19; break;
		case ELangHungarian: aDesiredLocalLanguage = 0x0E; break;
		case ELangBelgianFlemish:
		case ELangDutch: aDesiredLocalLanguage = 0x13; break;
		case ELangAustralian: aDesiredLocalLanguage = 0x0C09; aLocalLanguageMask = 0xFFFF; break;
		case ELangNewZealand: aDesiredLocalLanguage = 0x1409; aLocalLanguageMask = 0xFFFF; break;
		case ELangCzech: aDesiredLocalLanguage = 0x05; break;
		case ELangSlovak: aDesiredLocalLanguage = 0x1B; break;
		case ELangPolish: aDesiredLocalLanguage = 0x15; break;
		case ELangSlovenian: aDesiredLocalLanguage = 0x24; break;
		case ELangTaiwanChinese: aDesiredLocalLanguage = 0x0104; aLocalLanguageMask = 0xFFFF; break;
		case ELangHongKongChinese: aDesiredLocalLanguage = 0x0304; aLocalLanguageMask = 0xFFFF; break;
		case ELangPrcChinese: aDesiredLocalLanguage = 0x0204; aLocalLanguageMask = 0xFFFF; break;
		case ELangJapanese: aDesiredLocalLanguage = 0x11; break;
		case ELangKorean: aDesiredLocalLanguage = 0x12; break;
		case ELangThai: aDesiredLocalLanguage = 0x1e; break;
		default: break;
		}
	}

void CFaceListItem::SetTTFFaceNamesL(TOpenFontFaceAttrib& aAttrib) const
	{
	// Search for a suitable name record. Always use Windows Unicode encoding.
	const TUint16 desired_platform = 3;	// Windows
	const TUint16 desired_encoding = 1;	// Unicode
	const TUint16 desired_language = 0x0409;
	const TUint16 family_name_id = 1;
	const TUint16 full_name_id = 4;

	TUint16 local_language_mask = 0x00FF; // low byte only - high byte is country or other variant
	TUint16 desired_local_language = 0x09;
	GetDesiredLocalLanguageAndMask(desired_local_language, local_language_mask);

	const TUint name_records = FT_Get_Sfnt_Name_Count(iFace);
	for (TInt i = 0; i < name_records; i++)
		{
		FT_SfntName sfntName;
		if (FT_Get_Sfnt_Name(iFace, i, &sfntName))
			{
			User::Leave(KErrGeneral);
			}
		if (sfntName.platform_id == desired_platform &&
			sfntName.encoding_id == desired_encoding &&
			(sfntName.language_id == desired_language || (sfntName.language_id & local_language_mask) == desired_local_language) &&
			(sfntName.name_id == family_name_id || sfntName.name_id == full_name_id))
			{
			const TUint16* name_start = (TUint16*)sfntName.string;
			const TInt name_length = sfntName.string_len / sizeof(TUint16);
			// Copy the name into a properly aligned buffer.
			TBuf<TOpenFontFaceAttrib::ENameLength> name;
			const TInt truncated_length = Min(name_length,TOpenFontFaceAttrib::ENameLength);
			name.SetLength(truncated_length);
			Mem::Copy((void*)name.Ptr(), name_start, truncated_length * sizeof(TText));
			
#ifndef WORDS_BIGENDIAN
			// Byte-swap the Unicode text.
			SwapWords((TUint16*)name.Ptr(),truncated_length);
#endif

			if (sfntName.language_id == desired_language)
				{
				if (sfntName.name_id == family_name_id)
					{
					aAttrib.SetFamilyName(name);
					}
				else
					{
					aAttrib.SetFullName(name);
					}
				}
			if ((sfntName.language_id & local_language_mask) == desired_local_language)
				{
				if (sfntName.name_id == family_name_id)
					{
					aAttrib.SetLocalFamilyName(name);
					}
				else
					{
					aAttrib.SetLocalFullName(name);
					}
				}
			}
		}
	}

void CFreeTypeFontFile::GetNearestFontInPixelsL(
	RHeap*						aHeap,
	COpenFontSessionCacheList*	aSessionCacheList,
	const TOpenFontSpec&		aDesiredFontSpec,
	TInt						aPixelWidth,
	TInt						aPixelHeight,
	COpenFont*&					aFont,
	TOpenFontSpec&				aActualFontSpec)
	{
	GetNearestFontToDesignHeightInPixelsL(
		aHeap, aSessionCacheList, aDesiredFontSpec, aPixelWidth, aPixelHeight, aFont, aActualFontSpec);
	}

void CFreeTypeFontFile::GetNearestFontToDesignHeightInPixelsL(
	RHeap*						aHeap,
	COpenFontSessionCacheList*	aSessionCacheList,
	const TOpenFontSpec&		aDesiredFontSpec,
	TInt						aPixelWidth,
	TInt						aPixelHeight,
	COpenFont*&					aFont,
	TOpenFontSpec&				aActualFontSpec)
	{
	GetNearestFontInPixelsL(
		aHeap, aSessionCacheList, aDesiredFontSpec, aPixelWidth, aPixelHeight, aFont, aActualFontSpec, 0);
	}

void CFreeTypeFontFile::GetNearestFontToMaxHeightInPixelsL(
	RHeap*						aHeap,
	COpenFontSessionCacheList*	aSessionCacheList,
	const TOpenFontSpec&		aDesiredFontSpec,
	TInt						aPixelWidth,
	TInt						aPixelHeight,
	COpenFont*&					aFont,
	TOpenFontSpec&				aActualFontSpec,
	TInt						aMaxHeight)
	{
	GetNearestFontInPixelsL(
		aHeap, aSessionCacheList, aDesiredFontSpec, aPixelWidth, aPixelHeight, aFont, aActualFontSpec, aMaxHeight);
	}

void CFreeTypeFontFile::GetNearestFontInPixelsL(
	RHeap*						aHeap,
	COpenFontSessionCacheList*	aSessionCacheList,
	const TOpenFontSpec&		aDesiredFontSpec,
	TInt						aPixelWidth,
	TInt						aPixelHeight,
	COpenFont*&					aFont,
	TOpenFontSpec&				aActualFontSpec,
	TInt						aMaxHeight)
/**
Use the standard matcher.
*/
	{
	TInt face_index = 0;
	if (GetNearestFontHelper(aDesiredFontSpec, aPixelWidth, aPixelHeight, face_index, aActualFontSpec))
		{
		// Allow only monochrome and standard anti-aliasing.
		TGlyphBitmapType glyphBitmapType = aDesiredFontSpec.BitmapType();
		if (EAntiAliasedGlyphBitmap != glyphBitmapType)
			{
			glyphBitmapType = EMonochromeGlyphBitmap;
			}
		aFont = CFreeTypeFont::NewL(
			aHeap, aSessionCacheList, this, face_index, aActualFontSpec.Height(),
			aActualFontSpec.WidthFactor(), aActualFontSpec.SlantFactor(),
			glyphBitmapType, aMaxHeight, aActualFontSpec.ScriptTypeForMetrics());
		aActualFontSpec.SetBitmapType(glyphBitmapType);
		if (aMaxHeight)
			{
			aActualFontSpec.SetHeight(aFont->Metrics().Size());
			}
		}
	}

CFreeTypeFont* CFreeTypeFont::NewL(
	RHeap*						aHeap,
	COpenFontSessionCacheList*	aSessionCacheList,
	CFreeTypeFontFile*			aFontFile,
	TInt						aFaceIndex,
	TInt						aSizeInPixels,
	TInt32						aWidthFactor,
	TInt32						aSlantFactor,
	TGlyphBitmapType			aBitmapType,
	TInt						aMaxHeight,
	TInt						aScript)
	{
	CFreeTypeFont* f = (CFreeTypeFont*)aHeap->AllocL(sizeof(CFreeTypeFont));
	new(f) CFreeTypeFont(aHeap, aSessionCacheList, aFontFile, aFaceIndex, aWidthFactor, aSlantFactor, aBitmapType);
	CleanupStack::PushL(f);
	f->ConstructL(aSizeInPixels, aMaxHeight, aScript);
	CleanupStack::Pop();
	return f;
	}

CFreeTypeFont::CFreeTypeFont(RHeap* aHeap, COpenFontSessionCacheList* aSessionCacheList,
							 CFreeTypeFontFile* aFontFile, TInt aFaceIndex,
							 TInt32 aWidthFactor, TInt32 aSlantFactor, TGlyphBitmapType aBitmapType) :
	COpenFont(aHeap, aSessionCacheList, aFontFile, aFaceIndex),
	iWidthFactor(aWidthFactor),
	iSlantFactor(aSlantFactor),
	iBitmapType(aBitmapType)
	{
	}

static TUint TwentySixDotSix2Pixel(TInt32 aInt)
//
// Font units in FreeType are specified in a fixed point notation, which is a
// trick that makes it possible to store a fractional number in an integer variable.
// 26.6 means that the top 26 bits are the integer part, and the bottom 6 bits are
// the fractional part.
//
// 32 (binary 100000) is added to round the fractional part off.
//
	{
	if (0 > aInt)
		aInt = -aInt;
	return (aInt + 32) >> 6;
	}

void CFreeTypeFont::ConstructL(TInt aSizeInPixels, TInt aMaxHeight, TInt aScript)
	{
	CFreeTypeFontFile* file = (CFreeTypeFontFile*)File();
	User::LeaveIfNull( file );
	const CFaceListItem* face = file->LoadFaceL( FaceIndex() );
	TInt unicodeAscender = 0, unicodeDescender = 0;
	face->GetBiggestGlyphsFromSampleL(aScript, unicodeAscender, unicodeDescender);
	if (aMaxHeight)
		{
		aSizeInPixels =
			(unicodeAscender && unicodeDescender) ?
			face->DeriveDesignHeightFromMaxHeightWithGlyphSampleL(
				aMaxHeight, unicodeAscender, unicodeDescender) :
			face->DeriveDesignHeightFromMaxHeightWithBBoxL(aMaxHeight);
		}
	else
		{
		// Have to set the pixel size because no
		// DeriveDesignHeightFromMaxHeightWith...() was called.
		if (FT_Set_Pixel_Sizes(face->Face(), aSizeInPixels, aSizeInPixels))
			User::Leave( KErrGeneral );
		if (aSizeInPixels==0)
			aSizeInPixels=TwentySixDotSix2Pixel(face->Face()->size->metrics.height);
		}
	ComputeMetrics(face->Face(), aSizeInPixels, unicodeAscender, unicodeDescender);
	ComputeMaxWidth(face->Face());
	}

TBool CFaceListItem::LoadGlyphAndGetAscenderDescender(
	TUint32	aCode,
	TInt32&	aAscender,
	TInt32&	aDescender) const
//
//@param aCode Character code
//@param aAscender Vertical distance from the baseline to the topmost point of aCode
//@param aDescender Vertical distance from the baseline to the bottommost point of aCode
//@return ETrue if aCode exists in aFace and can be loaded, otherwise EFalse
//@pre FT_Set_Pixel_Sizes is called
//
	{
	const TUint glyphIndex = FT_Get_Char_Index(iFace, aCode);
	if (!glyphIndex || FT_Load_Glyph(iFace, glyphIndex, FT_LOAD_DEFAULT | FT_LOAD_IGNORE_TRANSFORM))
		return EFalse;
	aAscender  = iFace->glyph->metrics.horiBearingY;
	aDescender = iFace->glyph->metrics.height - iFace->glyph->metrics.horiBearingY;
	// if height < horiBearingY -> no descender possible
	if (0 > aDescender)
		aDescender = 0;
	return ETrue;
	}

void CFaceListItem::GetBiggestGlyphsFromSampleL(
	TInt	aScript,
	TInt&	aUnicodeAscender,
	TInt&	aUnicodeDescender) const
//
//@param aScript The script to determine which glyph sample to use
//@param aUnicodeAscender Unicode of the tallest ascender found in sample
//@param aUnicodeDescender Unicode of the 'deepest' descender found in sample
//
	{
	aUnicodeAscender = aUnicodeDescender = 0;
	const TPtrC glyphSample = GlyphSample::TScript2GlyphSample(aScript);
	if (!glyphSample.Ptr())
		return;
	// Set the face to an arbitrary value assuming the
	// biggest glyphs are the same for different sizes
	if (FT_Set_Pixel_Sizes(iFace, 0, 24))
		User::Leave( KErrGeneral );
	TInt32 maxAscender = 0, maxDescender = 0;
	const TInt totalGlyphsNum = glyphSample.Length();
	for (TInt i = 0; i < totalGlyphsNum; i++)
		{
		TInt32 ascender = 0, descender = 0;
		if (!LoadGlyphAndGetAscenderDescender(*(glyphSample.Ptr() + i), ascender, descender))
			continue;
		// Keep the max up to date
		if (maxAscender < ascender)
			{
			maxAscender = ascender;
			aUnicodeAscender = *(glyphSample.Ptr() + i);
			}
		if (maxDescender < descender)
			{
			maxDescender = descender;
			aUnicodeDescender = *(glyphSample.Ptr() + i);
			}
		}
	}

TInt CFaceListItem::DeriveDesignHeightFromMaxHeightWithGlyphSampleL(
	TInt	aMaxHeightInPixel,
	TInt	aUnicodeAscender,
	TInt	aUnicodeDescender) const
//
//@param aMaxHeightInPixel Maximum height within which the return design height has to fit into
//@param aUnicodeAscender Unicode of ascender with which font height is calculated
//@param aUnicodeDescender Unicode of descender with which font height is calculated
//@return The maximum design height that fits in aMaxHeightInPixel
//@post FT_Set_Pixel_Sizes is called with the return design height
//
	{
	TInt designHeightInPixel = aMaxHeightInPixel;
	do
		{
		if (FT_Set_Pixel_Sizes(iFace, 0, designHeightInPixel))
			User::Leave( KErrGeneral );
		TInt32 ascender = 0, descender = 0, dummyAscender = 0, dummyDescender = 0;
		LoadGlyphAndGetAscenderDescender(aUnicodeAscender,  ascender, dummyDescender);
		LoadGlyphAndGetAscenderDescender(aUnicodeDescender, dummyAscender, descender);
		if (TwentySixDotSix2Pixel(ascender + descender) <= aMaxHeightInPixel)
			{
			break;
			}
		} while (--designHeightInPixel);
	return designHeightInPixel;
	}

TInt CFaceListItem::DeriveDesignHeightFromMaxHeightWithBBoxL(TInt aMaxHeightInPixel) const
//
//@param aMaxHeightInPixel Maximum height within which the return design height has to fit into
//@return The maximum design height with which the bbox would fit in aMaxHeightInPixel
//@post FT_Set_Pixel_Sizes is called with the returned design height
//
	{
	// Calculation is as follows:
	//		boundingBoxHeightInPixel = aMaxHeightInPixel
	//		boundingBoxHeightInFontUnit = iFace->bbox.yMax - iFace->bbox.yMin
	//		PixelsPerFontUnit (PPFU) = boundingBoxHeightInPixel / boundingBoxHeightInFontUnit
	// And also:
	//		PixelsPerEm (PPE) = the design height we are looking for
	//		FontUnitsPerEm (FUPE) = iFace->units_per_EM
	//		PixelsPerFontUnit (PPFU) = PixelsPerEm / FontUnitsPerEm
	// Therefore by combining the equations we get:
	//		boundingBoxHeightInPixel / boundingBoxHeightInFontUnit = PixelsPerEm / FontUnitsPerEm
	// Rearranged:
	//		PixelsPerEm = boundingBoxHeightInPixel * FontUnitsPerEm / boundingBoxHeightInFontUnit
	// Now, if the result of the calculation is integer then that design height will fit
	// but if it is not then we should round down to guarantee the bounding box for the resultant
	// integer design height will fit inside our given maximum height
	//
	const TInt boundingBoxHeightInFontUnit = iFace->bbox.yMax - iFace->bbox.yMin;
	TInt designHeightInPixels = (aMaxHeightInPixel * iFace->units_per_EM) / boundingBoxHeightInFontUnit;
	if (FT_Set_Pixel_Sizes( iFace, designHeightInPixels, designHeightInPixels ))
		User::Leave( KErrGeneral );

	// Unfortunately, this is just the starting point. Actual design heights do not necessarily
	// have a mathematical relationship to font metrics so we need to adjust the design height
	// until we find the best value where the metrics do fit.
	const TInt maxHeightInFontUnit = aMaxHeightInPixel << 6;
	TInt currentMaxHeightInFontUnit = FT_MulFix( boundingBoxHeightInFontUnit, iFace->size->metrics.y_scale );
	while ( currentMaxHeightInFontUnit < maxHeightInFontUnit )
		{
		designHeightInPixels++;
		if (FT_Set_Pixel_Sizes( iFace, designHeightInPixels, designHeightInPixels ))
			User::Leave( KErrGeneral );
		currentMaxHeightInFontUnit = FT_MulFix( boundingBoxHeightInFontUnit, iFace->size->metrics.y_scale );
		}
	while ( currentMaxHeightInFontUnit > maxHeightInFontUnit )
		{
		designHeightInPixels--;
		if (FT_Set_Pixel_Sizes( iFace, designHeightInPixels, designHeightInPixels ))
			User::Leave( KErrGeneral );
		currentMaxHeightInFontUnit = FT_MulFix( boundingBoxHeightInFontUnit, iFace->size->metrics.y_scale );
		}
	return designHeightInPixels;
	}

void CFreeTypeFont::ComputeMetrics(
	const FT_Face&	aFace,
	TInt			aSizeInPixels,
	TInt			aUnicodeAscender,
	TInt			aUnicodeDescender)
//
//@param aFace FreeType face object
//@param aSizeInPixels Design height
//@param aUnicodeAscender Unicode of ascender with which maximum ascent is calculated
//@param aUnicodeDescender Unicode of descender with which maximum descent is calculated
//
// sTypoAscender/sTypoDescender are the only metrics in TrueType/OpenType
// that define in the Basic Latin character set.
// OpenType suggests the corresponding metrics in AFM file should be used.
// Adobe Font Metrics (AFM) File Format Specification in turn suggests that the y-value of the
// top of the lowercase 'd' and the bottom of the lowercase 'p' should be used for sTypoAscender
// and sTypoDescender respectively.
//
// Because there is no guarantee that these table values are accurate, we use them only when
// the characters 'd' and 'p' are absent in the font file.
//
// If aUnicodeAscender or aUnicodeDescender is not given, the 'max' metrics are computed with
// the coordinates of the bbox, which is an imaginary box that encloses _all_ glyphs from the font.
//
	{
	TUint glyphIndex = FT_Get_Char_Index(aFace, 0x0064);
	const TInt32 ascentBasicLatin =
		(glyphIndex && !FT_Load_Glyph(aFace, glyphIndex, FT_LOAD_DEFAULT | FT_LOAD_IGNORE_TRANSFORM)) ?
		aFace->glyph->metrics.horiBearingY :
		FT_MulFix(((TT_Face)aFace)->os2.sTypoAscender, aFace->size->metrics.y_scale);

	glyphIndex = FT_Get_Char_Index(aFace, 0x0070);
	const TInt32 descentBasicLatin =
		(glyphIndex && !FT_Load_Glyph(aFace, glyphIndex, FT_LOAD_DEFAULT | FT_LOAD_IGNORE_TRANSFORM)) ?
		aFace->glyph->metrics.height - aFace->glyph->metrics.horiBearingY :
		FT_MulFix(((TT_Face)aFace)->os2.sTypoDescender, aFace->size->metrics.y_scale);

	const TInt yMaxBBox = FT_MulFix(aFace->bbox.yMax, aFace->size->metrics.y_scale);
	const TInt yMinBBox = FT_MulFix(aFace->bbox.yMin, aFace->size->metrics.y_scale);

	glyphIndex = aUnicodeAscender ? FT_Get_Char_Index(aFace, aUnicodeAscender) : 0;
	const TInt32 ascenderMax =
		(glyphIndex && !FT_Load_Glyph(aFace, glyphIndex, FT_LOAD_DEFAULT | FT_LOAD_IGNORE_TRANSFORM)) ?
		aFace->glyph->metrics.horiBearingY :
		yMaxBBox;

	glyphIndex = aUnicodeDescender ? FT_Get_Char_Index(aFace, aUnicodeDescender) : 0;
	const TInt32 descenderMax =
		(glyphIndex && !FT_Load_Glyph(aFace, glyphIndex, FT_LOAD_DEFAULT | FT_LOAD_IGNORE_TRANSFORM)) ?
		aFace->glyph->metrics.height - aFace->glyph->metrics.horiBearingY :
		yMinBBox;

	iFontCapitalAscent		= TwentySixDotSix2Pixel(ascentBasicLatin);
	iFontStandardDescent	= TwentySixDotSix2Pixel(descentBasicLatin);
	iFontMaxAscent			= TwentySixDotSix2Pixel(ascenderMax);
	iFontMaxDescent			= TwentySixDotSix2Pixel(descenderMax);
	iFontLineGap =
		(FT_FACE_FLAG_SFNT & aFace->face_flags) ?
		iFontMaxAscent + iFontMaxDescent + TwentySixDotSix2Pixel(((TT_Face)aFace)->os2.sTypoLineGap) :
		( ( (iFontMaxAscent + iFontMaxDescent) * 12 ) + 5 ) / 10;
	// old version of metrics left for backward compatibility
	iMetrics.SetSize		(aSizeInPixels);
	iMetrics.SetAscent		(TwentySixDotSix2Pixel(aFace->size->metrics.ascender));
	iMetrics.SetDescent		(TwentySixDotSix2Pixel(aFace->size->metrics.descender));
	iMetrics.SetMaxHeight	(TwentySixDotSix2Pixel(yMaxBBox));
	iMetrics.SetMaxDepth	(TwentySixDotSix2Pixel(yMinBBox));
	}

void CFreeTypeFont::ComputeMaxWidth(const FT_Face& aFace)
//
// Scale the maximum advance by the width factor if it is not unity.
//
	{
	iMetrics.SetMaxWidth(TwentySixDotSix2Pixel(aFace->size->metrics.max_advance));
	if (iWidthFactor && KOneIn16Dot16FixedPointFormat != iWidthFactor)
		{
		TInt32 advance = iMetrics.MaxWidth() << 16;
		advance = FT_MulFix(advance, iWidthFactor);
		iMetrics.SetMaxWidth( advance >> 16 );
		}
	}

// Access to extended API though BC interface
void CFreeTypeFont::ExtendedInterface(TUid aUid, TAny*& aParam)
	{
	if (aUid == KUidOpenFontShapingExtension)
		aParam = static_cast<MOpenFontShapingExtension*>(this);
	else if (aUid == KUidOpenFontTrueTypeExtension)	
		aParam = static_cast<MOpenFontTrueTypeExtension*>(this);
	else if (aUid == KUidOpenFontGlyphOutlineExtension)
	    aParam = static_cast<MOpenFontGlyphOutlineExtension*>(this);
	else
		COpenFont::ExtendedInterface(aUid, aParam);
	}
 
// Rasterize a glyph from unicode value of aCode.
void CFreeTypeFont::RasterizeL(TInt aCode, TOpenFontGlyphData* aGlyphData)
	{
	CFreeTypeFontFile* file = (CFreeTypeFontFile*)File();
	if (!file)
		User::Leave(KErrGeneral);
	file->RasterizeL(
		aCode, FaceIndex(), iMetrics.Size(), iWidthFactor, iSlantFactor, iBitmapType, aGlyphData);
	}

// Rasterize a glyph from glyph code value of aCode.
void CFreeTypeFont::RasterizeGlyphL(TInt aCode,TOpenFontGlyphData* aGlyphData)
	{
	CFreeTypeFontFile* file = (CFreeTypeFontFile*)File();
	if (!file)
		User::Leave(KErrGeneral);
	file->RasterizeGlyphL(
		aCode, FaceIndex(), iMetrics.Size(), iWidthFactor, iSlantFactor, iBitmapType, aGlyphData);
	}

TInt CFreeTypeFont::GlyphIndex(TInt aUnicode) const
	{
	CFreeTypeFontFile* file = static_cast<CFreeTypeFontFile*>(File());
	if (!file)
		return 0;
	return FT_Get_Char_Index(file->LoadFaceL( FaceIndex() )->Face(), aUnicode);
	}

TBool CFreeTypeFont::GlyphPointInHintedPixels(TInt aGlyphIndex, TInt aPointNumber,
	TReal& aX, TReal& aY) const
	{
	CFreeTypeFontFile* file = static_cast<CFreeTypeFontFile*>(File());
	if (!file)
		return EFalse;
	FT_Face face = file->LoadFaceAndSetTransformL( FaceIndex(),
		iMetrics.Size(), iWidthFactor, iSlantFactor )->Face();
	// Load glyph outline, scale and hint but don't rasterize
	if (0 == FT_Load_Glyph(face, aGlyphIndex, FT_LOAD_NO_BITMAP))
		{
		if (face->glyph->format != FT_GLYPH_FORMAT_OUTLINE
			|| face->glyph->outline.n_points <= aPointNumber)
			return EFalse;
		FT_Vector& vec = face->glyph->outline.points[aPointNumber];
		// Convert from 1.25.6 fixed point to floating point
		aX = vec.x / 64.0;
		aY = vec.y / 64.0;
		return ETrue;
		}
	return EFalse;
	}
	
TBool CFreeTypeFont::GlyphPointInFontUnits(TInt aGlyphIndex, TInt aPointNumber,
	TInt& aX, TInt& aY) const
	{
	CFreeTypeFontFile* file = static_cast<CFreeTypeFontFile*>(File());
	if (!file)
		return EFalse;
	FT_Face face = file->LoadFaceL( FaceIndex() )->Face();
	// Load glyph outline but don't scale, hint or rasterize
	if (0 == FT_Load_Glyph(face, aGlyphIndex, FT_LOAD_IGNORE_TRANSFORM
		| FT_LOAD_NO_BITMAP | FT_LOAD_NO_SCALE))
		{
		if (face->glyph->format != FT_GLYPH_FORMAT_OUTLINE
			|| face->glyph->outline.n_points <= aPointNumber)
			return EFalse;
		FT_Vector& vec = face->glyph->outline.points[aPointNumber];
		aX = vec.x;
		aY = vec.y;
		return ETrue;
		}
	return EFalse;
	}
	
void CFreeTypeFont::GetExtensionFontMetrics(
	MOpenFontShapingExtension::TExtensionFontMetrics& aOut)
	{
	CFreeTypeFontFile* file = static_cast<CFreeTypeFontFile*>(File());
	if (file)
		{
		FT_Face face = file->LoadFaceAndSetTransformL( FaceIndex(),
			iMetrics.Size(), iWidthFactor, iSlantFactor )->Face();
		aOut.iUnitsPerEm = face->units_per_EM;
		aOut.iXPixelsPerEm = face->size->metrics.x_ppem;
		aOut.iYPixelsPerEm = face->size->metrics.y_ppem;
		aOut.iXScaleFactor = face->size->metrics.x_scale / 65536.0;
		aOut.iYScaleFactor = face->size->metrics.y_scale / 65536.0;
		}
	else
		{
		// This shouldn't happen, but we can't panic here.
		aOut.iUnitsPerEm = 1;
		aOut.iXPixelsPerEm = 1.0;
		aOut.iYPixelsPerEm = 1.0;
		aOut.iXScaleFactor = 1.0;
		aOut.iYScaleFactor = 1.0;
		}
	}


TInt CFreeTypeFont::GetGlyphOutline(TUint aCode, TBool aIsGlyphId, 
                TBool aHinted, TAny*& aOutline, TInt& aLength)
    {
    CFreeTypeFontFile* file = (CFreeTypeFontFile*)File();
    if (!file)
        {
        return KErrNotFound;
        }
    aHinted = EFalse; // to avoid 'unused' warning.
    
    return file->GetGlyphOutline(FaceIndex(), aCode, aIsGlyphId, aHinted, aOutline, aLength);
    }

TAny* CFreeTypeFont::GetTrueTypeTable(TInt& aError, TUint32 aTag, TInt* aLength)
	{
	CFreeTypeFontFile* file = (CFreeTypeFontFile*)File();
	if (!file)
		{
		aError = KErrNotFound;
		return 0;
		}

	return file->GetTrueTypeTable(aError, FaceIndex(), aTag, aLength);
	}

TInt CFreeTypeFont::ReleaseTrueTypeTable(TAny*)
	{
	// CFreeTypeFontFile keeps hold of tables until destruction
	return KErrNone;
	}

TBool CFreeTypeFont::HasTrueTypeTable(TUint32 aTag)
	{
	FT_ULong table_len = 0;

	CFreeTypeFontFile* file = (CFreeTypeFontFile*)File();
	if ( file == NULL )
		{
		return EFalse;
		}
	
	// locate the table & find the length
	CFaceListItem* faceList = file->LoadFaceL( FaceIndex() );
	FT_Face face = faceList->Face();
	TT_Face ttface = (TT_Face)faceList->Face();
	TInt aError = ttface->goto_table( ttface, aTag, face->stream, &table_len );

	return aError == KErrNone;
	}

void CFreeTypeFontFile::SetGlyphMetrics(FT_GlyphSlot aGlyphSlot,TOpenFontGlyphData* aGlyphData)
	{
	TOpenFontCharMetrics metrics;
	metrics.SetWidth(aGlyphSlot->bitmap.width);
	metrics.SetHeight(aGlyphSlot->bitmap.rows);
	metrics.SetHorizBearingX(aGlyphSlot->bitmap_left);
	metrics.SetHorizBearingY(aGlyphSlot->bitmap_top);
	metrics.SetHorizAdvance(TwentySixDotSix2Pixel(aGlyphSlot->advance.x));
	metrics.SetVertBearingX(0);
	metrics.SetVertBearingY(0);
	metrics.SetVertAdvance(aGlyphSlot->bitmap.rows);
	aGlyphData->SetMetrics(metrics);
	}

void CFreeTypeFontFile::RasterizeL(
	TInt				aCode,
	TInt				aFaceIndex,
	TInt				aSizeInPixels,
	TInt32				aWidthFactor,
	TInt32				aSlantFactor,
	TGlyphBitmapType	aBitmapType,
	TOpenFontGlyphData*	aGlyphData)
	{
	const CFaceListItem* face = LoadFaceAndSetTransformL(aFaceIndex,
		aSizeInPixels, aWidthFactor, aSlantFactor);
	TInt glyphCode = FT_Get_Char_Index(face->Face(), aCode);		
	DoRasterizeGlyphL(glyphCode, face, aBitmapType, aGlyphData);
	}

void CFreeTypeFontFile::RasterizeGlyphL(
	TInt				aCode,
	TInt				aFaceIndex,
	TInt				aSizeInPixels,
	TInt32				aWidthFactor,
	TInt32				aSlantFactor,
	TGlyphBitmapType	aBitmapType,
	TOpenFontGlyphData* aGlyphData)
	{
	const CFaceListItem* face = LoadFaceAndSetTransformL(aFaceIndex,
		aSizeInPixels, aWidthFactor, aSlantFactor);
	DoRasterizeGlyphL(aCode, face, aBitmapType, aGlyphData);
	}

void CFreeTypeFontFile::DoRasterizeGlyphL(
	TInt aCode, const CFaceListItem* aFace,
	TGlyphBitmapType aBitmapType, TOpenFontGlyphData* aGlyphData)
	{
	// Substitute it with Symbian's private use area replacement character
	// if this character is not in the font.  KReplacementCharacter might
	// also be absent in the font - this is valid and FT_Load_Glyph will
	// handle the case when glyph_index is zero.
	//
	const TUint glyph_index =
		(aCode != 0) ? aCode : FT_Get_Char_Index(aFace->Face(), KReplacementCharacter);

	// Set the flag that causes the isFixedPitch flag in the PostScript table to be ignored.
	// If we honour isFoxedPitch, CJK fonts like MSSONG space their Latin characters too widely because all
	// characters are given the width of the widest character in the font. Ignoring the flag does no
	// harm to genuine fixed-width fonts, which will return the same width for all characters.
	//
	const TInt32 load_flags =
		(aBitmapType == EMonochromeGlyphBitmap) ?
		FT_LOAD_RENDER | FT_LOAD_IGNORE_GLOBAL_ADVANCE_WIDTH | FT_LOAD_MONOCHROME :
		FT_LOAD_RENDER | FT_LOAD_IGNORE_GLOBAL_ADVANCE_WIDTH;

	if (FT_Load_Glyph(aFace->Face(), glyph_index, load_flags) != 0)
		User::Leave(KErrGeneral);

	// Translate the glyph data from FreeType to EPOC format.
	SetGlyphMetrics(aFace->Face()->glyph,aGlyphData);
	if (aBitmapType == EMonochromeGlyphBitmap)
		iContext->TranslateMonochromeGlyphBitmap(aFace->Face()->glyph,aGlyphData);
	else
		iContext->TranslateAntiAliasedGlyphBitmap(aFace->Face()->glyph,aGlyphData);
	}

enum vg_commands {
    VG_CMD_NONE = 0,
    VG_CMD_MOVETO,
    VG_CMD_LINETO,
    VG_CMD_CONICTO,
    VG_CMD_CUBICTO,
    VG_CMD_CLOSE
};


enum contour_states 
    {
    CONTOUR_STATE_NOT_STARTED = 0,
    CONTOUR_STATE_START,
    CONTOUR_STATE_CONIC,
    CONTOUR_STATE_CUBIC1,
    CONTOUR_STATE_CUBIC2,
    CONTOUR_STATE_MAX
    };

static const TInt StateTransitions[CONTOUR_STATE_MAX][3] = 
    {
        {CONTOUR_STATE_START, -1, -1},
        {CONTOUR_STATE_START, CONTOUR_STATE_CONIC, CONTOUR_STATE_CUBIC1},
        {CONTOUR_STATE_START, -1, -1},
        {-1, -1, CONTOUR_STATE_CUBIC2}, 
        {CONTOUR_STATE_START, -1, -1}, 
    };

static const TInt OutputCommands[CONTOUR_STATE_MAX][3] = 
    {
        {VG_CMD_MOVETO, -1, -1},
        {VG_CMD_LINETO, VG_CMD_NONE, VG_CMD_NONE},
        {VG_CMD_CONICTO, -1, -1},
        {-1, -1, VG_CMD_NONE}, 
        {VG_CMD_CUBICTO, -1, -1},
    };


class MVGCommandProcessor 
    {
public:
    virtual TInt ProcessCommand(TInt8 cmd, FT_Vector &start, FT_Vector &end) = 0;
    };

class COutlineStringBuilder: public MVGCommandProcessor, public CBase 
    {
private:
    TUint8 *iBuffer;
    TUint8 *iCur;
    TInt iLen;


public:
    COutlineStringBuilder():iBuffer(0), iLen(2000) 
        { 
        iBuffer = (TUint8 *)User::Alloc(iLen);
        iCur = iBuffer;
        }
    
    ~COutlineStringBuilder() 
        {
        // ownership of the buffer is transferred to the caller.
        }
    
    
    TUint8 *GetBuffer(TInt &aLen) 
        {
        aLen = iCur - iBuffer;
        *iCur = '\0';
        return iBuffer;
        }
    
    TInt AppendChar(char ch)
        {
        *(iCur++) = ch;
        return 0;
        }
    
    TInt AppendFTPos(FT_Pos n)
        {
        int divisor = 1;
        FT_Pos tmp = (n > 0) ? n : (-n);
        while (tmp/divisor >= 10) {
            divisor *= 10;
        }
        
        if (n < 0)
            {
            AppendChar('-');
            }
        
        for ( ; divisor >= 1; divisor /= 10)
            {
            AppendChar('0' + tmp/divisor);
            tmp = tmp % divisor;
            }
        
        return 0;
        }
    
    TInt AppendCoord(FT_Pos x, FT_Pos y) 
        {
        AppendFTPos(x);
        AppendChar(',');
        AppendFTPos(y);
        AppendChar(' ');
        return 0;
        }
    
    TInt
    ProcessCommand(TInt8 cmd, FT_Vector &start, FT_Vector &end) 
        {
        FT_Vector *st = &start;

        if (iCur + 64 > iBuffer + iLen) 
            {
            TUint distance = iCur - iBuffer;
            iLen += 1000;
            TUint8 *newBuffer = (TUint8 *)User::ReAlloc(iBuffer, iLen);
            iBuffer = newBuffer;
            iCur = iBuffer + distance;
            }
        
        if (VG_CMD_MOVETO == cmd) 
            {
            AppendChar('M');
            AppendCoord(start.x, start.y);
            }
        else if (VG_CMD_LINETO == cmd)
            {
            AppendChar('L');
            AppendCoord(end.x, end.y);
            }
        else if (VG_CMD_CONICTO == cmd)
            {
            AppendChar('Q');
            AppendCoord((st+1)->x, (st+1)->y);
            AppendCoord(end.x, end.y);
            }
        else if (VG_CMD_CUBICTO == cmd)
            {
            AppendChar('Q');
            AppendCoord((st+1)->x, (st+1)->y);
            AppendCoord((st+2)->x, (st+2)->y);
            AppendCoord(end.x, end.y);
            }
        else if (VG_CMD_CLOSE == cmd)
            {
            AppendChar('Z');
            AppendChar(' ');
            }
        
        return KErrNone;
        }
    };


class COutlineConvDirector: public CBase {
private:
    MVGCommandProcessor *iProcessor;
    const FT_Outline *iOutline; 
    FT_Outline iNewOutline;

    
private:
    char
    GetNextPointType(char aTag) 
        {
        char ret = FT_CURVE_TAG(aTag);
        if (FT_CURVE_TAG_ON == ret)
            {
            ret = 0;
            }
        else if (FT_CURVE_TAG_CONIC == ret)
            {
            ret = 1;
            }
        else if (FT_CURVE_TAG_CUBIC == ret)
            {
            ret = 2;
            }
        else 
            {
            __ASSERT_DEBUG(0, User::Panic(_L("IncorrectState"), -1));
            }
        return ret;
        }
    
    TInt SwapPoints(const TInt i1, const TInt i2)
        {
        FT_Vector tmpVector = iOutline->points[i1];
        char tmpTags = iOutline->tags[i1];
        iOutline->points[i1] = iOutline->points[i2];
        iOutline->tags[i1] = iOutline->tags[i2];
        iOutline->points[i2] = tmpVector;
        iOutline->tags[i2] = tmpTags;
        return 0;
        }
    
    TInt MoveFirstOnPointToBeginning(const TInt aStartIndex, const TInt aEndIndex) 
        {
        /* Contours of three or more points are valid, and single points 
         * (reference points, technically not contours) are also valid as 
         * special cases in TrueType. 
         */ 
        char curTag = FT_CURVE_TAG(iOutline->tags[aStartIndex]);
        
        // so a contour having only one point which is 'off' is invalid!
        __ASSERT_DEBUG(!(aEndIndex - aStartIndex == 0 && FT_CURVE_TAG_ON != curTag), 
                User::Panic(_L("Contour consisting of 1 'off' point."), -1));
        
        /* Contours consisting of two points are not a valid configuration. */
        __ASSERT_DEBUG(aEndIndex - aStartIndex != 1, 
                User::Panic(_L("Contour consisting of two points."), -1));
        
        if (FT_CURVE_TAG_ON == curTag) 
            {
            return KErrNone;
            }
        TInt firstOnIndex = -1;
        TInt i = 0;
        for (i = 1+aStartIndex; i < aEndIndex; ++i)
            {
            if (FT_CURVE_TAG_ON == FT_CURVE_TAG(iOutline->tags[i]))
                {
                firstOnIndex = i;
                break;
                }
            }
        __ASSERT_DEBUG(-1 != firstOnIndex, 
                User::Panic(_L("Contour containing no 'on' point."), -1));
        
        for (i = firstOnIndex-1; i >= aStartIndex; --i)
            {
            for (TInt j = i; j < aEndIndex; ++j)
                {
                SwapPoints(j, j+1);
                }
            }
        
        return KErrNone;
        }
    
    TInt
    ConvertContour(const TInt aStartIndex, const TInt aEndIndex)
        {
        /* Contours consisting of two 
         * points are not a valid configuration. 
         */
        __ASSERT_DEBUG(aEndIndex - aStartIndex != 1, 
                User::Panic(_L("Contour consisting of two points."), -1));
        
        TInt state = CONTOUR_STATE_NOT_STARTED, newState = -1;
        TInt cmdStart = aStartIndex, cmdCur = 0, command = -1;
        
        char ptype = GetNextPointType(iNewOutline.tags[cmdStart]);
        __ASSERT_DEBUG(0 == ptype, User::Panic(_L("IncorrectState"), -1)); 
        state = CONTOUR_STATE_START;
        iProcessor->ProcessCommand(VG_CMD_MOVETO, 
                iNewOutline.points[aStartIndex], iNewOutline.points[aStartIndex]);
        
        
        for (cmdCur = cmdStart + 1; cmdCur <= aEndIndex; ++cmdCur)
            {
            ptype = GetNextPointType(iNewOutline.tags[cmdCur]);
            newState = StateTransitions[state][ptype];
            __ASSERT_DEBUG(-1 != newState, User::Panic(_L("IncorrectState"), -1));
            command = OutputCommands[state][ptype];
            __ASSERT_DEBUG(-1 != command, User::Panic(_L("IncorrectState"), -1));
            
            if (VG_CMD_NONE != command)
                {
                iProcessor->ProcessCommand(command, 
                        iNewOutline.points[cmdStart], iNewOutline.points[cmdCur]);
                cmdStart = cmdCur;
                }
            state = newState;
            }
        
        if (CONTOUR_STATE_CONIC == state)
            {
            iProcessor->ProcessCommand(VG_CMD_CONICTO, iNewOutline.points[cmdStart], 
                    iNewOutline.points[aStartIndex]);
            }
        else if (CONTOUR_STATE_CUBIC2 == state)
            {
            iProcessor->ProcessCommand(VG_CMD_CUBICTO, iNewOutline.points[cmdStart], 
                    iNewOutline.points[aStartIndex]);
            }
        iProcessor->ProcessCommand(VG_CMD_CLOSE, 
                iNewOutline.points[aStartIndex], iNewOutline.points[aStartIndex]);
        
        return KErrNone;
        }


    TInt Preprocess() 
        {
        /* two successive conic "off" points forces the rasterizer to 
         * create (during the scan-line conversion process exclusively) a 
         * virtual "on" point amidst them, at their exact middle.
         */
        char prevTag = FT_CURVE_TAG(iOutline->tags[0]), currentTag = 0;
        TInt numNewPoints = 0;
        TInt contourIndex = 0;
        
        iNewOutline.contours = 0;
        iNewOutline.n_contours = iOutline->n_contours;
        iNewOutline.contours = (short *)
                User::Alloc(iNewOutline.n_contours * sizeof(short));
        
        if (0 == iOutline->contours[0]) 
            {
            iNewOutline.contours[0] = iOutline->contours[0]; // == 0
            ++contourIndex;
            }
        for (TInt i = 1; i < iOutline->n_points; ++i) 
            {
                currentTag = FT_CURVE_TAG(iOutline->tags[i]);
                if (FT_CURVE_TAG_CONIC == prevTag && prevTag == currentTag)
                    {
                    numNewPoints++;
                    }
                prevTag = currentTag;
                if (i == iOutline->contours[contourIndex]) 
                    {
                    iNewOutline.contours[contourIndex] =
                        iOutline->contours[contourIndex] + numNewPoints;
                    ++contourIndex;
                    }
            }
        
        
        iNewOutline.n_points = iOutline->n_points + numNewPoints;
        iNewOutline.flags = iOutline->flags;
        
        iNewOutline.points = 0;
        iNewOutline.tags = 0;
        
        iNewOutline.points = (FT_Vector *)
            User::Alloc(iNewOutline.n_points * sizeof(FT_Vector));

        if (iNewOutline.contours)
            {
            iNewOutline.tags = (char *)
                User::Alloc(iNewOutline.n_points * sizeof(char));
            }
        
        // copy the 'points' and 'tags' array, inserting new points
        // when necessary.
        TInt oldIndex = 0, newIndex = 0;
        for ( ; oldIndex < iOutline->n_points; ++oldIndex) 
            {
            char oldTag = FT_CURVE_TAG(iOutline->tags[oldIndex]);
            iNewOutline.points[newIndex] = iOutline->points[oldIndex];
            iNewOutline.tags[newIndex] = iOutline->tags[oldIndex];
            
            if (FT_CURVE_TAG_CONIC == oldTag && 
                    oldIndex + 1 < iOutline->n_points) 
                {
                char nextTag = FT_CURVE_TAG(iOutline->tags[oldIndex+1]);
                // insert a new 'on' point when there are two consecutive 
                // 'conic off' points.
                if (oldTag == nextTag)
                    {
                    newIndex++;
                    FT_Vector *cur = &(iOutline->points[oldIndex]);
                    FT_Vector *next = &(iOutline->points[oldIndex + 1]);
                    iNewOutline.points[newIndex].x = (cur->x + next->x)/2;
                    iNewOutline.points[newIndex].y = (cur->y + next->y)/2;
                    iNewOutline.tags[newIndex] = FT_CURVE_TAG_ON;
                    }
                }
            newIndex++;
            }
        
        return 0;
        }
    
public:
    COutlineConvDirector():iProcessor(0), iOutline(0) 
        {
        // a null constructor
        iNewOutline.contours = 0;
        iNewOutline.tags = 0;
        iNewOutline.points = 0;
        }
    
    ~COutlineConvDirector()
        {
        User::Free(iNewOutline.contours);
        User::Free(iNewOutline.points);
        User::Free(iNewOutline.tags);
        }
    
    TInt
    ConvertOutline(const FT_Outline &aOutline, MVGCommandProcessor *aProcessor) 
        {
        if (0 != aOutline.n_contours) 
            {
            iProcessor = aProcessor;
            iOutline = &aOutline;
            
            MoveFirstOnPointToBeginning(0, iOutline->contours[0]);
            TInt i = 0;
            for (i = 1; i < iOutline->n_contours; ++i)
                {
                MoveFirstOnPointToBeginning(iOutline->contours[i-1]+1, iOutline->contours[i]);
                }
                    
            Preprocess();
            
            ConvertContour(0, iNewOutline.contours[0]);
            for (i = 1; i < iNewOutline.n_contours; ++i)
                {
                ConvertContour(iNewOutline.contours[i-1]+1, iNewOutline.contours[i]);
                }
            }
        else
            {
			RDebug::Printf("Zero contour in outline: missing glyph.");
            FT_Vector dummyVector;
            aProcessor->ProcessCommand(VG_CMD_CLOSE, dummyVector, dummyVector);
            }
        return KErrNone;
        }
};


TInt CFreeTypeFontFile::GetGlyphOutline(TInt aFaceIndex, TUint aCode, 
        TBool aIsGlyphId, TBool, TAny*& aOutline, TInt &aLength) 
    {
    // The 4th param 'aHinted' is ignored in this reference implementation.
    // Need to add it back and implement accordingly if freetype is used in 
    // production code.
    CFaceListItem *faceList = LoadFaceL(aFaceIndex);
    FT_Face face = faceList->Face();
    TUint code = aCode;
    if (!aIsGlyphId) 
        {
        code = FT_Get_Char_Index(face, aCode);
        if (0 == code)
            {
            return KErrNotFound;
            }
        }
    
    TInt ret = FT_Load_Glyph(face, code, 
            FT_LOAD_NO_SCALE | FT_LOAD_IGNORE_TRANSFORM);
    
    if (0 != ret)
        {
        return KErrUnknown;
        }
    
    COutlineStringBuilder strBuilder;
    if (0 != strBuilder.GetBuffer(aLength)) 
        {
        FT_Outline &outline = face->glyph->outline;
        
        COutlineConvDirector d;
        d.ConvertOutline(outline, &strBuilder);
        }
    else
        {
        return KErrNoMemory;
        }

    TUint8 *buf = strBuilder.GetBuffer(aLength);
    RDebug::Printf("length of buffer is %d\n", aLength);
    RDebug::Printf("Outline for glyph %d: \n", aCode);
    RDebug::Printf("%s", buf);
    aOutline = (TAny*)buf;
    
    return KErrNone;
    }
TAny* CFreeTypeFontFile::GetTrueTypeTable(TInt& aError, TInt aFaceIndex,
	TUint32 aTag, TInt* aLength)
	{
	aError = KErrNone;

	// already loaded?
	TPtrC8* table = iTableStore.Find(aTag);
	if (table)
		{
		if (aLength)
			*aLength = table->Length();
		return (TAny*) table->Ptr();
		}

	CFaceListItem* faceList = LoadFaceL( aFaceIndex );
	FT_Face face = faceList->Face();
	TT_Face ttface = (TT_Face)face;

	// locate the table & find the length
	TUint32 length = 0;
	FT_Error fterror = ttface->goto_table(ttface, aTag, face->stream, &length);
	if (fterror != 0)
		{
		aError = KErrNotFound;
		return 0;
		}

	// allocate memory for the table
	unsigned char * buffer = reinterpret_cast<unsigned char *>(
		User::Alloc(length));
	if ( !buffer )
		{
		aError = KErrNoMemory;
		return 0;
		}

	// read the table 
	fterror = FT_Load_Sfnt_Table( face, aTag, 0, buffer, &length );

	if (fterror != 0)
		{
		User::Free(buffer);
		aError = KErrNotFound;
		return 0;
		}

	// store the table
	aError = iTableStore.Insert(aTag, TPtrC8(buffer, length));
	if (aError != KErrNone)
		{
		User::Free(buffer);
		return 0;
		}

	if(aLength)
		*aLength = length;

	return buffer;
	}

void CFreeTypeContext::TranslateMonochromeGlyphBitmap(FT_GlyphSlot aGlyphSlot,TOpenFontGlyphData* aGlyphData)
	{
	const TInt width = aGlyphSlot->bitmap.width;
	const TInt height = aGlyphSlot->bitmap.rows;
	const TInt width_bytes = aGlyphSlot->bitmap.pitch;

	StartGlyph(aGlyphData);
	const TUint8* p = (const TUint8*)aGlyphSlot->bitmap.buffer;
	TInt row = 0;
	while (row < height)
		{
		// Find the number of repeating or non-repeating rows (up to 15)
		TInt count = 1;
		TBool repeating = FALSE;
		const TUint8* cur = p;
		while (count < 15 && row + count < height)
			{
			const TUint8* prev = cur;
			cur += width_bytes;
			const TBool same = Mem::Compare(prev,width_bytes,cur,width_bytes) == 0;
			if (count == 1)
				repeating = same;
			else if (repeating != same)
				break;
			count++;
			}

		// Write a 0 for repeating or a 1 for non-repeating.
		WriteGlyphBit(repeating ? 0 : 1);

		// Write the count.
		WriteGlyphBit(count & 1 ? 1 : 0);
		WriteGlyphBit(count & 2 ? 1 : 0);
		WriteGlyphBit(count & 4 ? 1 : 0);
		WriteGlyphBit(count & 8 ? 1 : 0);

		// Write a single repeating row or all the non-repeating rows.
		const TInt rows_written = repeating ? 1 : count;
		cur = p;
		for (TInt row_written = 0; row_written < rows_written; row_written++, cur += width_bytes)
			{
			TInt col = 0;
			for (TInt byte = 0; byte < width_bytes; byte++)
				{
				unsigned char x = cur[byte];
				for (TInt bit = 0; bit < 8 && col < width; bit++, col++, x <<= 1)
					WriteGlyphBit(x & 128 ? 1 : 0);
				}
			}

		row += count;
		p += width_bytes * count;
		}

	EndGlyph();
	}

void CFreeTypeContext::TranslateAntiAliasedGlyphBitmap(FT_GlyphSlot aGlyphSlot, TOpenFontGlyphData* aGlyphData)
	{
	const TInt width = aGlyphSlot->bitmap.width;
	const TInt height = aGlyphSlot->bitmap.rows;
	const TInt width_bytes = aGlyphSlot->bitmap.pitch;

	StartGlyph(aGlyphData);
	const TUint8* p = (const TUint8*)aGlyphSlot->bitmap.buffer;
	/* 
	Some fonts include embedded bitmaps for certain characters and certain sizes.
	These embedded bitmaps can be 1 bit per pixel.
	*/
	if (aGlyphSlot->bitmap.pixel_mode == ft_pixel_mode_mono)
		{
		// how many whole bytes per line
		const TInt byteCount = width / 8;
		// how many bits used in the last byte - if any.
		const TInt bitCount = width % 8;
		for (TInt row = 0; row < height; row++)
			{
			for (TInt col = 0; col < byteCount; col++)
				{
				WriteMonoData(*p++,8);
				}
			if (bitCount > 0)
				{
				WriteMonoData(*p++,bitCount);
				}
			}
		}
	else	// assume 8 bits per pixel
		{
		TInt row = 0;
		while (row < height)
			{
			const TUint8* q = p;
			for (TInt i = 0; i < width; i++, q++)
				WriteGlyphByte(*q);
			row++;
			p += width_bytes;
			}
		}
	EndGlyph();
	}

void CFreeTypeContext::WriteMonoData(TUint8 aData, TInt aBitCount)
	{
	for (TInt j = 7; j >= 8 - aBitCount; j--)
		{
		WriteGlyphByte((1<<j) & aData ? 255 : 0);
		}
	}

TBool CFreeTypeFontFile::HasUnicodeCharacterL(TInt aFaceIndex, TInt aCode) const
	{
	const CFaceListItem* face = iContext->LoadFaceL(iFileName, aFaceIndex);
	return 0 != FT_Get_Char_Index(face->Face(), aCode);
	}

CFaceListItem::CFaceListItem()
	{
	}

CFaceListItem* CFaceListItem::NewL(CFreeTypeContext* aContext, const TText8* aFileName, TInt aFaceIndex)
	{
	CFaceListItem* item = new(ELeave) CFaceListItem;
	CleanupStack::PushL(item);
	item->ConstructL(aContext, aFileName, aFaceIndex);
	CleanupStack::Pop();
	return item;
	}

void CFaceListItem::ConstructL(CFreeTypeContext* aContext, const TText8* aFileName, TInt aFaceIndex)
	{
	// Open the face and select the Unicode character map.
	iFileName = aFileName;
	iFaceIndex = aFaceIndex;
    TInt error = FT_New_Face(
    	aContext->Library(), (const char *) aFileName, aFaceIndex, &iFace);
	if (!error)
		error = FT_Select_Charmap(iFace, ft_encoding_unicode);
	if (error)
		{
		iFace = NULL;
		User::Leave(KErrGeneral);
		}
	}

CFaceListItem::~CFaceListItem()
	{
	FT_Done_Face(iFace);
	}

FT_Face CFaceListItem::Face() const
	{
	return iFace;
	}

CFaceList::CFaceList()
	{
	}

CFaceList::~CFaceList()
	{
	CFaceListItem* next = NULL;
	for (CFaceListItem* p = iFirstItem; p; p = next)
		{
		next = p->iNext;
		delete p;
		}
	}

CFaceListItem* CFaceList::LoadFaceL(
	CFreeTypeContext*	aContext,
	const TText8*		aFileName,
	TInt				aFaceIndex)
	{
	CFaceListItem* prev = NULL;
	for (CFaceListItem* p = iFirstItem; p; prev = p, p = p->iNext)
		{
		if (p->iFileName == aFileName && p->iFaceIndex == aFaceIndex) // found; move to head of list
			{
			if (prev)
				{
				prev->iNext = p->iNext;
				p->iNext = iFirstItem;
				iFirstItem = p;
				}
			return p;
			}
		}

	// Load and add a new item to the head of the list
	CFaceListItem* new_item = CFaceListItem::NewL(aContext, aFileName, aFaceIndex);
	new_item->iNext = iFirstItem;
	iFirstItem = new_item;


    /*
    If too much memory has been used delete item(s) from the end of the list, but don't
    delete the last item, which is the one requested.
    */
#ifdef _DEBUG   
    TInt DeleteCount = 0;
#endif  
    
    while (iFirstItem->iNext && MemoryUsed() > EMemoryThreshold)
        {
#ifdef _DEBUG
        DeleteCount++;
#endif      
        DeleteLastFace();
        }
    
#ifdef _DEBUG   
    if(DeleteCount) 
        {
        RDebug::Print(_L("Delete %d in Face Cache: MemoryUsed() > EMemoryThreshold"),DeleteCount);
        }
#endif
    
    return iFirstItem;
	}

void CFaceList::DeleteFace(const TText8* aFileName, TInt aFaceIndex)
	{
	CFaceListItem* prev = NULL;
	for (CFaceListItem* p = iFirstItem; p; prev = p, p = p->iNext)
		{
		if (p->iFileName == aFileName && p->iFaceIndex == aFaceIndex) // found; delete it
			{
			if (prev)
				prev->iNext = p->iNext;
			else
				iFirstItem = p->iNext;
			delete p;
			return;
			}
		}
	}

void CFaceList::DeleteLastFace()
	{
	CFaceListItem* p = iFirstItem;
	if (p)
		{
		CFaceListItem* prev = NULL;
		while (p && p->iNext)
			{
			prev = p;
			p = p->iNext;
			}
		if (prev)
			prev->iNext = NULL;
		else
			iFirstItem = NULL;
		delete p;
		}
	}
