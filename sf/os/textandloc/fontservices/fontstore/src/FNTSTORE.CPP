/*
* Copyright (c) 1995-2010 Nokia Corporation and/or its subsidiary(-ies).
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


#include <e32hal.h>
#include <hal.h>
#include <s32file.h>
#include <graphics/shapeimpl.h>
#include <fntstore.h>
#include "FNTBODY.H"
#include "FNTSTD.H"
#include <graphics/shaperparams.h>

#define DO_LOADFONT_OPTIMIZATION 1
#include <linkedfonts.h>
#include "linkedfontsprivate.h"
#include "openfontsprivate.h"
#include <openfontlinkedtypefaceelementspec.h>
#include <graphics/openfontlinkedtypefaceextension.h>
#include <graphics/openfontlinkedtypefacespecification.h>
#include <graphics/openfontrasterizer.h>
#include <graphics/openfontconstants.h>

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "FNTSTORETraces.h"
#endif


static const TUint32 KOutlineGlyphIdHashMask = 0x0000ffff;
static const TUint32 KOutlineFileUidHashMask = 0x00ff0000;
static const TUint32 KOutlineFaceIndexHashMask = 0x0f000000;
static const TUint32 KOutlineFontPtrHashMask = 0x0fff0000;

// uncomment to enable some verbose debug prints
//#define VERBOSE_DEBUG 1


/**  These constants set the default pixel width and height, in Twips x1000.
 Hal Data information overrides this if it is available. EDisplayXPixels & EDisplayXTwips, 
 EDisplayYPixels & EDisplayYTwips values are required respectively.

 (11860 is approx equivalent to 120 pixels per inch.)
@internalComponent
*/
const TInt KDefaultPixelWidthInTwips = 11860;
const TInt KDefaultPixelHeightInTwips = 11860;

// default granularity for arrays
const TInt KDefaultArrayGranularity = 8;

/* Maximum height of a character bitmap. When font height in pixel is 1024, each 
character bitmap is equal to (roughly) 1024 x 1024 x 8 bits/pixel = 1 MB.
@internalComponent
*/
const TInt KMaxFontHeightInPixels = 1024;

/* Minimum height of font in pixels that can be displayed on the screen.
@internalComponent
*/
const TInt KMinFontHeightInPixels = 2;
/**
The arbitrary list of sizes, in points, returned for scaleable fonts.
We can't return 2^32 - 1 sizes, or something that reflect the fact
that any size is okay, because a UI will probably try to create a listbox containing all the sizes.

Array stored in Twips as that is the only form of the value currently used.
@internalComponent
*/
#define POINTSIZE_IN_TWIPS(p)	((p) * 20)
const TInt gOpenFontSizeInTwipsArray[] =
	{
	// 4pt-18pt in steps of 1pt (15 sizes)
	POINTSIZE_IN_TWIPS(4), POINTSIZE_IN_TWIPS(5), POINTSIZE_IN_TWIPS(6),
	POINTSIZE_IN_TWIPS(7), POINTSIZE_IN_TWIPS(8), POINTSIZE_IN_TWIPS(9),
	POINTSIZE_IN_TWIPS(10), POINTSIZE_IN_TWIPS(11), POINTSIZE_IN_TWIPS(12),
	POINTSIZE_IN_TWIPS(13), POINTSIZE_IN_TWIPS(14), POINTSIZE_IN_TWIPS(15),
	POINTSIZE_IN_TWIPS(16), POINTSIZE_IN_TWIPS(17), POINTSIZE_IN_TWIPS(18),
	// 20pt-36pt in steps of 2pt (9 sizes)
	POINTSIZE_IN_TWIPS(20), POINTSIZE_IN_TWIPS(22), POINTSIZE_IN_TWIPS(24),
	POINTSIZE_IN_TWIPS(26), POINTSIZE_IN_TWIPS(28), POINTSIZE_IN_TWIPS(30),
	POINTSIZE_IN_TWIPS(32), POINTSIZE_IN_TWIPS(34), POINTSIZE_IN_TWIPS(36),
	// 40pt-72pt in steps of 4pt (9 sizes)
	POINTSIZE_IN_TWIPS(40), POINTSIZE_IN_TWIPS(44), POINTSIZE_IN_TWIPS(48),
	POINTSIZE_IN_TWIPS(52), POINTSIZE_IN_TWIPS(56), POINTSIZE_IN_TWIPS(60),
	POINTSIZE_IN_TWIPS(64), POINTSIZE_IN_TWIPS(68), POINTSIZE_IN_TWIPS(72),
	// 80pt-144pt in steps of 8pt (9 sizes)
	POINTSIZE_IN_TWIPS(80), POINTSIZE_IN_TWIPS(88), POINTSIZE_IN_TWIPS(96),
	POINTSIZE_IN_TWIPS(104), POINTSIZE_IN_TWIPS(112), POINTSIZE_IN_TWIPS(120),
	POINTSIZE_IN_TWIPS(128), POINTSIZE_IN_TWIPS(136), POINTSIZE_IN_TWIPS(144)
	};

const TInt KOpenFontSizeArrayCount = sizeof(gOpenFontSizeInTwipsArray) / sizeof(gOpenFontSizeInTwipsArray[0]);

/**
@internalTechnology
 The folder used to store linked fonts; the character represents the system drive
 location and the descriptor contains the private path of fbserv.
 */
_LIT(KLinkedFontFileFolder, "%c:%Slfonts\\");

/**
@internalTechnology
 The folder used to store updated linked fonts; the descriptor should be formatted
 with the formatted version of KLinkedFontFileFolder. Files will be moved from this
 folder to KLinkedFontFileFolder at boot time before the font loading procedure.
 */
_LIT(KLinkedFontFileTempFolder, "update\\");

/**
The font search string. The Y: directory prefix forces the search to 
start with drive Y:. The drives will be searched in the order Y: to A: then Z:
*/
_LIT(KFBSERVFontDirSecure, "y:\\resource\\fonts\\");

/**The maximum length of the linked font folder path*/
const TInt KMaxLinkedFontPathLength = 36;

/**The maximum length of a private path*/
const TInt KMaxPrivatePathLength = 19;

/** Container for TTypefaceSupport that describes an Open Font family.

There is an implicit assumption that all fonts in the family will have the same attributes
as the first instance found.  E.g. that the range of font heights is the same. 

An array of pointers to the COpenFontFile file objects that hold fonts in this family
also provides reference counting for the object.

@internalComponent
*/
NONSHARABLE_CLASS(CTypefaceSupportInfo) : public CBase
	{
public:
	CTypefaceSupportInfo();
	inline TInt AddFontFilePtr(COpenFontFile* aOpenFontFile);
	inline TInt AddUniqueFontFilePtr(COpenFontFile* aOpenFontFile,const TOpenFontFaceAttrib& aFaceAttrib);
	inline TInt FindFontFilePtr(COpenFontFile* aOpenFontFile);
	inline TBool RemoveFontFilePtr(TInt aIndex);
	void SetTypefaceInfo(const TOpenFontFaceAttrib& aFaceAttrib, TInt aMinHeightInTwips);
	inline const TTypefaceSupport* TypefaceSupport() const;
	inline TInt NearestPointSizeIndex() const;
	static TInt CompareFontNames(const CTypefaceSupportInfo& aTypeface1, const CTypefaceSupportInfo& aTypeface2);
	~CTypefaceSupportInfo();

public:
	TTypefaceSupport iSupport;
	// pointers back to the COpenFontFile object which has one or more typefaces in this family
	RPointerArray<COpenFontFile> iOpenFontFilePtrArray;
	// nearest standard point size index;
	TInt iNearestPointSizeIndex;
	};


/** helper class used with the cleanup stack
@internalComponent
*/
class TCleanupRemoveFontFile
	{
public:
	inline TCleanupRemoveFontFile(CFontStore* aFontStore, TUid aFontUid);
public:
	CFontStore* iFontStore;
	TUid iFontUid;
	};


// declare static functions
static TInt MatchFontSpecsInPixels(const TOpenFontSpec& aCandidateFontSpec,
	const TOpenFontSpec& aIdealFontSpec, TInt aCandidateMaxHeight = 0, TInt aIdealMaxHeight = 0);
static void ReadFromFileL(RFile& aFile, TDes8& aDes, TInt aLength);
static TUint TtfTableTagFromBufferL(TDes8& aDes);

#ifdef _DEBUG
static TUint32 StartTiming(void);
static TUint32 FinishTimingL(TUint32 startTime);
#endif

static TBool FileIsOnZ(TParse& aFileName);
static TBool FileIsInList(TParse& aFileName, RArray<TParse>& aList);


/** Helper function for converting a pointer to an offset from the passed
heap base. Use OffsetToPointer() to convert the returned offset back to a
useable pointer.
@param aAny A pointer to be converted to an offset.
@param aHeapBase The heap base of the current process.
@return An offset representing the passed pointer that can be converted
back to a pointer using the function OffsetToPointer(). 
@see OffsetToPointer()
 */
LOCAL_C TInt PointerToOffset(const TAny* aAny, TUint8* aHeapBase)
    {
    if (aAny && aHeapBase)
        {
        return reinterpret_cast<TInt>(aAny) - reinterpret_cast<TInt>(aHeapBase);
        }
    return 0;
    }

/** Helper function for converting an offset (that was calculated using
PointerToOffset()) back to a pointer relative to the passed heap base.
@param aOffset The offset to be converted to a pointer.
@param aHeapBase The heap base of the current process.
@return A pointer relative to the passed heap base.
@see PointerToOffset()
 */
LOCAL_C TAny* OffsetToPointer(TInt aOffset, TUint8* aHeapBase)
    {
    if (aOffset && aHeapBase)
        {
        return reinterpret_cast<TAny*>(aOffset + reinterpret_cast<TInt>(aHeapBase));
        }
    return NULL;
    }


// CTypefaceSupportInfo
CTypefaceSupportInfo::CTypefaceSupportInfo()
	{
	}


CTypefaceSupportInfo::~CTypefaceSupportInfo()
	{ // no ownership of font files, so just Close the array
	iOpenFontFilePtrArray.Close();
	}


/** Add a back pointer to an Open Font file, in sorted order.
@param aOpenFontFile font file that has a typeface in this family.
@return KErrNone if added, KErrAlreadyExists if file is already referenced, or KErrNoMemory.
@internalComponent
*/
TInt CTypefaceSupportInfo::AddFontFilePtr(COpenFontFile* aOpenFontFile)
	{
	return iOpenFontFilePtrArray.InsertInAddressOrder(aOpenFontFile);
	}

/** Add a back pointer to an Open Font file, in sorted order, but only if the font details are unique.
@param aOpenFontFile font file that has a typeface in this family.
@return KErrNone if added, KErrInUse if another file already defines this, KErrAlreadyExists if file is already referenced, or KErrNoMemory.
@internalComponent
*/
TInt CTypefaceSupportInfo::AddUniqueFontFilePtr(COpenFontFile* aOpenFontFile,const TOpenFontFaceAttrib& aFaceAttrib)
	{
#ifdef _DEBUG	
	_LIT(KLoadedFont, "TypefaceSupport: already loaded font: %S\n");
#endif
	
	TPtrC findName=aFaceAttrib.FullName();
	for (TInt i=0,maxi=iOpenFontFilePtrArray.Count();i<maxi;i++)
		{	//Do any of the fonts already registered with this family already provide this specific font?
		COpenFontFile* cmpFont=iOpenFontFilePtrArray[i];
		if (cmpFont==aOpenFontFile)
			{   //Another specific font in this font file has already been added to this family
				//InsertInAddressOrder would detect this.
#ifdef _DEBUG
				RDebug::Print(KLoadedFont, &findName);
#endif
				return KErrAlreadyExists;	
			}
		else
			{
			for (TInt j=0,maxj=cmpFont->FaceCount();j<maxj;j++)
				{	//The expectation is that open fonts will only have 1 face per file, so this loop is 1-pass generally.
				const TOpenFontFaceAttrib& cmpFace=cmpFont->FaceAttrib(j);
				TPtrC cmpName=cmpFace.FullName();
				if (cmpName==findName)
					{
#ifdef _DEBUG
					RDebug::Print(KLoadedFont, &findName);
#endif
					return KErrInUse;
					}
				}
			}
		}
	return iOpenFontFilePtrArray.InsertInAddressOrder(aOpenFontFile);
	}

/** Check font family for back pointer to an Open Font file.
@param aOpenFontFile font file that may have a typeface in this family.
@return KErrNotFound or the index of pointer
@internalComponent
*/
TInt CTypefaceSupportInfo::FindFontFilePtr(COpenFontFile* aOpenFontFile)
	{
	return iOpenFontFilePtrArray.FindInAddressOrder(aOpenFontFile);
	}


/** Remove a font file back pointer.
@param aIndex the index of pointer, from FindFontFilePtr()
@return ETrue if there are no more implementations of the font family
@internalComponent
*/
TBool CTypefaceSupportInfo::RemoveFontFilePtr(TInt aIndex)
	{
	iOpenFontFilePtrArray.Remove(aIndex);
	return iOpenFontFilePtrArray.Count() == 0;
	}


/** Set the details of the typeface family.
@param aFaceAttrib typeface to parameterise.
@internalComponent
*/
void CTypefaceSupportInfo::SetTypefaceInfo(const TOpenFontFaceAttrib& aFaceAttrib, TInt aMinHeightInTwips)
	{
	iSupport.iTypeface.iName = aFaceAttrib.ShortFamilyName();
	iSupport.iTypeface.SetIsProportional(!aFaceAttrib.IsMonoWidth());
	iSupport.iTypeface.SetIsSerif(aFaceAttrib.IsSerif());
	iSupport.iTypeface.SetIsSymbol(aFaceAttrib.IsSymbol());

	// Find minimum size in twips then find the next higher standard size.
	iSupport.iMinHeightInTwips = aMinHeightInTwips;
	TInt fontSizeIndex = 0;
	for (; fontSizeIndex < KOpenFontSizeArrayCount; ++fontSizeIndex)
		{
		const TInt KMinHeightInTwips = gOpenFontSizeInTwipsArray[fontSizeIndex];
		if (aMinHeightInTwips <= KMinHeightInTwips)
			{
			aMinHeightInTwips = KMinHeightInTwips;
			break;
			}
		}
	iNearestPointSizeIndex = fontSizeIndex;
	iSupport.iMaxHeightInTwips = Max(gOpenFontSizeInTwipsArray[KOpenFontSizeArrayCount - 1], iSupport.iMinHeightInTwips);
	iSupport.iNumHeights = Max(1,KOpenFontSizeArrayCount - iNearestPointSizeIndex);
	iSupport.iIsScalable = TRUE;
	}


/** Get the details of the typeface family.
@internalComponent
*/
const TTypefaceSupport* CTypefaceSupportInfo::TypefaceSupport() const
	{
	return &iSupport;
	}
	
/** Get the Nearest Standard Point Size Index.
@internalComponent
*/
TInt CTypefaceSupportInfo::NearestPointSizeIndex() const
	{
	return iNearestPointSizeIndex;
	}

// for InsertInOrder
TInt CTypefaceSupportInfo::CompareFontNames(const CTypefaceSupportInfo& aTypeface1, const CTypefaceSupportInfo& aTypeface2)
	{
	return aTypeface1.iSupport.iTypeface.iName.CompareF(aTypeface2.iSupport.iTypeface.iName);
	}


// TCleanupRemoveFontFile
TCleanupRemoveFontFile::TCleanupRemoveFontFile(CFontStore* aFontStore, TUid aFontUid) :
	iFontStore(aFontStore), iFontUid(aFontUid)
	{
	}


TBitmapFontCharacterOffset::TBitmapFontCharacterOffset()
 :	iBitmapOffset(0)
	{}

void TBitmapFontCharacterOffset::InternalizeL(RReadStream& aStream)
	{
	iBitmapOffset = aStream.ReadUint16L();
	};

EXPORT_C TCharacterMetrics::TCharacterMetrics()
 :	iAscentInPixels(0),
	iHeightInPixels(0),
	iLeftAdjustInPixels(0),
	iMoveInPixels(0),
	iRightAdjustInPixels(0)
	{
	}

TBitmapFontCharacterMetrics::TBitmapFontCharacterMetrics()
 :	iAscentInPixels(0),
	iHeightInPixels(0),
	iLeftAdjustInPixels(0),
	iMoveInPixels(0),
	iRightAdjustInPixels(0)
	{
	}

void TBitmapFontCharacterMetrics::InternalizeL(RReadStream& aStream)
	{
	iAscentInPixels = aStream.ReadInt8L();
	iHeightInPixels = aStream.ReadInt8L();
	iLeftAdjustInPixels = aStream.ReadInt8L();
	iMoveInPixels = aStream.ReadInt8L();
	iRightAdjustInPixels = aStream.ReadInt8L();
	}

/** Constructor. */
EXPORT_C TAlgStyle::TAlgStyle()
 :	iBaselineOffsetInPixels(0),
	iFlags(0),
	iWidthFactor(1),
	iHeightFactor(1)
	{
	}

/** Sets whether the font is bold.

@param aIsBold ETrue if the font is bold; otherwise EFalse. */
EXPORT_C void TAlgStyle::SetIsBold(TBool aIsBold)
	{
	if (aIsBold)
		iFlags |= EBold;
	else
		iFlags &= ~EBold;
	}

/** Sets whether the font is italic.

@param aIsItalic ETrue if the font is italic; otherwise EFalse. */
EXPORT_C void TAlgStyle::SetIsItalic(TBool aIsItalic)
	{
	if (aIsItalic)
		iFlags |= EItalic;
	else
		iFlags &= ~EItalic;
	}

/** Sets whether the font is mono width - i.e. all characters have the same width.

@param aIsMono ETrue if the font is mono width; otherwise EFalse. */
EXPORT_C void TAlgStyle::SetIsMono(TBool aIsMono)
	{
	if (aIsMono)
		iFlags|=EMono;
	else
		iFlags&=~EMono;
	}

/** Sets the width factor.

@param aWidthFactor A width factor. */
EXPORT_C void TAlgStyle::SetWidthFactor(TInt aWidthFactor)
	{
	iWidthFactor=(TInt8) aWidthFactor;
	}

/** Sets the height factor.

@param aHeightFactor A height factor. */
EXPORT_C void TAlgStyle::SetHeightFactor(TInt aHeightFactor)
	{
	iHeightFactor=(TInt8) aHeightFactor;
	}

/** Returns whether the font is bold.

@return ETrue if the font is bold; otherwise EFalse. */
EXPORT_C TBool TAlgStyle::IsBold() const
	{
	return iFlags&EBold;
	}

/** Returns whether the font is italic.

@return ETrue if the font is italic; otherwise EFalse. */
EXPORT_C TBool TAlgStyle::IsItalic() const
	{
	return iFlags&EItalic;
	}

/** Returns whether the font is mono - i.e. all characters have the same width.

@return ETrue if the font is mono; otherwise EFalse. */
EXPORT_C TBool TAlgStyle::IsMono() const
	{
	return iFlags&EMono;
	}

/** Returns the width factor.

@return A width factor. */
EXPORT_C TInt TAlgStyle::WidthFactor() const
	{
	return iWidthFactor;
	}

/** Returns the height factor.

@return A height factor. */
EXPORT_C TInt TAlgStyle::HeightFactor() const
	{
	return iHeightFactor;
	}

EXPORT_C TBool TAlgStyle::operator==(const TAlgStyle& aAlgStyle) const
	{
	return (iFlags==aAlgStyle.iFlags) && 
	       (iWidthFactor==aAlgStyle.iWidthFactor) &&
	       (iHeightFactor==aAlgStyle.iHeightFactor) &&
	       (iBaselineOffsetInPixels==aAlgStyle.iBaselineOffsetInPixels);
	}

/**
@internalTechnology
*/
TBool TAlgStyle::operator!=(const TAlgStyle& aAlgStyle) const
	{
	return !this->operator==(aAlgStyle);
	}

CBitmapFont::CBitmapFont(
	RHeap*				aHeap,
	const TFontSpec&	aFontSpecInTwips,
	const TAlgStyle&	aAlgStyle,
	CFontBitmap*		aFontBitmap):
	iFontSpecInTwips(aFontSpecInTwips),
	iAlgStyle(aAlgStyle),
	iHeap(aHeap)
	{
	iFontBitmapOffset = (TInt) ((TUint)(aFontBitmap) - (TUint)(this));
	}

CBitmapFont::CBitmapFont(
	RHeap*				aHeap,
	const TFontSpec&	aFontSpecInTwips,
	const TAlgStyle&	aAlgStyle,
	COpenFont*			aOpenFont):
	iFontSpecInTwips(aFontSpecInTwips),
	iAlgStyle(aAlgStyle),
	iHeap(aHeap)
	{
    // Set iOpenFont to be the offset of aOpenFont from the address of CBitmapFont.
    // In order to be able to identify iOpenFont as an offset instead of a pointer,
    // bitwise or the offset with 1.  Pointers will always be word aligned (and therefore even).
    if (aOpenFont)
        {
        __ASSERT_DEBUG(!(reinterpret_cast<TInt>(aOpenFont) & 1), Panic(EFntPointerNotByteAligned));
        iOpenFont = reinterpret_cast<COpenFont*>((reinterpret_cast<TInt>(aOpenFont) - reinterpret_cast<TInt>(this)) | 1);
        }
    else
        {
        iOpenFont = NULL;
        }
	}

/** This member is private and not intended for use. */
void CBitmapFont::ConstructL()
	{
	if (!IsOpenFont())
		FontBitmap()->UseL();
	}

/** This member is private and not intended for use. */
CBitmapFont::~CBitmapFont()
	{	
	if (!IsOpenFont() && iFontBitmapOffset)
		FontBitmap()->Release();
	else if (IsOpenFont())
		delete OpenFont();
	}	

CBitmapFont* CBitmapFont::NewL(
	RHeap*				aHeap,
	const TFontSpec&	aFontSpecInTwips,
	const TAlgStyle&	aAlgStyle,
	CFontBitmap*		aFontBitmap)
	{
	// font is placed in shared heap
	CBitmapFont* f = (CBitmapFont*) aHeap->AllocL(sizeof(CBitmapFont));
	new(f) CBitmapFont(aHeap, aFontSpecInTwips, aAlgStyle, aFontBitmap);
	CleanupStack::PushL(f);
	f->ConstructL();
	CleanupStack::Pop(f);
	return f;
	}

CBitmapFont* CBitmapFont::NewL(
	RHeap*				aHeap,
	const TFontSpec&	aFontSpecInTwips,
	const TAlgStyle&	aAlgStyle,
	COpenFont*			aOpenFont)
	{
	// font is placed in shared heap
	CBitmapFont* f = (CBitmapFont*) aHeap->AllocL(sizeof(CBitmapFont));
	new(f) CBitmapFont(aHeap,aFontSpecInTwips,aAlgStyle,aOpenFont);
	return f;
	}
	
/** Returns the font type identifier:KCBitmapFontUidVal.

@return The font type identifier. */
EXPORT_C TUid CBitmapFont::DoTypeUid() const
	{
	return TUid::Uid(KCBitmapFontUidVal);
	}

/** Returns a font identifier.

If it uses an open font the UID returned has a value of zero. Otherwise it 
has the UID value of the CFontBitmap it uses.

@return A font identifier. */
TUid CBitmapFont::Uid() const
	{
	if (IsOpenFont())
		return TUid::Uid(0);	
	else
		return FontBitmap()->iUid;
	}

/** Returns the font height in pixels.
For an open font this will be the design height (notional rather than exact).

@return Font height in pixels.
@see FontMaxHeight()
*/
EXPORT_C TInt CBitmapFont::DoHeightInPixels() const
	{
	if (IsOpenFont())
		return Height(OpenFont()->Metrics().Size());	
	else
		return Height(FontBitmap()->iCellHeightInPixels);
	}

/** Returns the font ascent in pixels.

The ascent is usually the height of a Latin capital letter above the baseline.
For an open font, the returned value is likely to be inaccurate.
Don't rely on it to get exact metrics at the pixel level.

@return Font ascent in pixels.
@see FontCapitalAscent()
@see FontMaxAscent()
*/
EXPORT_C TInt CBitmapFont::DoAscentInPixels() const
	{
	if (IsOpenFont())
		return Height(OpenFont()->Metrics().Ascent());	
	else
		return Height(FontBitmap()->iAscentInPixels);
	}

/** Returns the width, in pixels, of the given character.

If all characters have the same width (i.e. the font is mono - an attribute 
stored in iAlgStyle) then this is the maximum normal width, returned using 
MaxNormalCharWidthInPixels().

Note: For OpenType fonts this function returns the horizontal advance of
the character, which may be different from the actual width.

@param aChar A character.
@return The character width in pixels. */
EXPORT_C TInt CBitmapFont::DoCharWidthInPixels(TChar aChar) const
	{
	const TUint8* bytes;
	if (iAlgStyle.IsMono())
		return MaxNormalCharWidthInPixels();
	else
		return Width(CharacterMetrics(aChar,bytes).iMoveInPixels);
	}

/** Returns the width, in pixels, of the given text.

@param aText Text string.
@return Width of the text in pixels. */
EXPORT_C TInt CBitmapFont::DoTextWidthInPixels(const TDesC &aText) const
	{
	TMeasureTextInput* dummy = NULL;
	return DoTextWidthInPixels(aText, dummy);
	}

/** Returns the width, in pixels, of the given text.

@param aText Text string.
@return Width of the text in pixels. */
TInt CBitmapFont::DoTextWidthInPixels(const TDesC &aText, const TMeasureTextInput* aParam) const
	{
	TMeasureTextOutput output;
	TInt advance_width = MeasureText(aText,aParam,&output);
	return Max(advance_width,output.iBounds.Width());
	}


/** Returns the baseline offset in pixels, as stored in iAlgStyle.

@return The baseline offset in pixels. */
EXPORT_C TInt CBitmapFont::DoBaselineOffsetInPixels() const
	{
	return iAlgStyle.iBaselineOffsetInPixels;
	}

/** Returns the number of whole characters of aText, starting from the beginning, 
that fit into the given pixel width.

@param aText A text string.
@param aWidthInPixels A width in pixels.
@return The number of whole characters of aText that fit into aWidthInPixels.
*/
EXPORT_C TInt CBitmapFont::DoTextCount(const TDesC &aText,TInt aWidthInPixels) const
	{
	TInt count = 0;
	const TInt length = aText.Length();
	TInt width = 0;
	// accumulate text width character by character, until target is reached or exceeded
	TInt widthDiff = aWidthInPixels - width;
	while ( (widthDiff > 0) && (length > count) )
		{ 
		width += TextWidthInPixels(aText.Mid(count,1));
		widthDiff = aWidthInPixels - width;
		if (widthDiff >= 0)
			{ // width <= aWidthInPixels
			++count;
			}
		}
	return count;
	}

/** Returns the number of whole characters of aText, starting from the beginning, 
that fit into the given pixel width, and gets the excess width.

@param aText A text descriptor.
@param aWidthInPixels A width in pixels.
@param aExcessWidthInPixels On return, the width left over of aWidthInPixels 
after the maximum possible whole number of characters of aText have been fit 
into it.
@return The number of whole characters of aText that fit into aWidthInPixels. */
EXPORT_C TInt CBitmapFont::DoTextCount(const TDesC &aText,TInt aWidthInPixels,TInt &aExcessWidthInPixels) const
	{
	TInt count = TextCount(aText,aWidthInPixels);
	aExcessWidthInPixels = aWidthInPixels - TextWidthInPixels(aText.Left(count));
	return count;
	}

/** Returns the font's maximum character width in pixels.

@return The maximum character width in pixels. */
EXPORT_C TInt CBitmapFont::DoMaxCharWidthInPixels() const
	{
	if (IsOpenFont())
		return Width(OpenFont()->Metrics().MaxWidth());	
	else
		return Width(FontBitmap()->iMaxCharWidthInPixels);
	}

/** Returns the font's normal maximum character width in pixels.

Note that when a CFontBitmap is used this value is the same as the maximum 
character width in pixels returned by MaxCharWidthInPixels(), but when a COpenFont 
is used the value may be different.

@return The normal maximum character width in pixels. */
EXPORT_C TInt CBitmapFont::DoMaxNormalCharWidthInPixels() const
	{
	if (IsOpenFont())
		return Width(OpenFont()->Metrics().MaxWidth());
	else
		return Width(FontBitmap()->iMaxNormalCharWidthInPixels);
	}

/** Returns the device-independent font specification for the font.

Note that this is set when the bitmap font object is constructed.

@return A font specification. */
EXPORT_C TFontSpec CBitmapFont::DoFontSpecInTwips() const
	{
	return iFontSpecInTwips;
	}


/** Attempts to rasterize a character (aCode) into a data area (aGlyphData) in 
shared memory.

This works only for open fonts: where the bitmap font object is using a COpenFont.

Bitmap fonts are deemed to be fully rasterized after they have been loaded 
from file. 

Returns ETrue if the character was successfully rasterized or was already 
in the cache. This function can be called only by the server; rasterization 
uses memory and other resources (e.g., file handles) owned by the server. 

@param aSessionHandle A session handle for the open font system.
@param aCode A character code.
@param aGlyphData A data area in shared memory.
@return ETrue if the character was successfully rasterized or was already in 
the cache; otherwise EFalse. */
EXPORT_C TBool CBitmapFont::Rasterize(TInt aSessionHandle, TInt aCode, TOpenFontGlyphData* aGlyphData) const
	{
	if (IsOpenFont())
		return OpenFont()->Rasterize(aSessionHandle, aCode, aGlyphData);
	else
		return EFalse;
	}

/** Gets a pointer to a bitmap and the metrics for a specified character, but only 
if a CFontBitmap is being used by the bitmap font object.

This function does not work when a COpenFont is being used, but GetCharacterData() 
can be used instead.

If the specified character does not exist in the font then the bitmap pointer 
and character metrics are gotten for a replacement character, KReplacementCharacter.

@param aCode A character code.
@param aBytes On return, a pointer to the bitmap for the specified character.
@return Metrics for the specified character. */
EXPORT_C TCharacterMetrics CBitmapFont::CharacterMetrics(TInt aCode,const TUint8*& aBytes) const
	{
	/*
	This function does not work for Open Fonts because the character data need not be
	shared between sessions and a session handle is needed; GetCharacterData should be used instead.
	*/
	if (IsOpenFont())
		{
		aBytes = NULL;
		return TCharacterMetrics();
		}

	TBitmapFontCharacterMetrics bitmap_font_metrics = FontBitmap()->CharacterMetrics(aCode,aBytes);

	// Substitute the replacement character if this character doesn't exist in the font.
	if (aBytes == NULL)
		bitmap_font_metrics = FontBitmap()->CharacterMetrics(KReplacementCharacter,aBytes);

	TCharacterMetrics metrics;
	metrics.iAscentInPixels = bitmap_font_metrics.iAscentInPixels;
	metrics.iHeightInPixels = bitmap_font_metrics.iHeightInPixels;
	metrics.iLeftAdjustInPixels = bitmap_font_metrics.iLeftAdjustInPixels;
	metrics.iMoveInPixels = bitmap_font_metrics.iMoveInPixels;
	metrics.iRightAdjustInPixels = bitmap_font_metrics.iRightAdjustInPixels;
	
	if (iAlgStyle.IsMono())
		{
		TInt width = metrics.iMoveInPixels - metrics.iLeftAdjustInPixels - metrics.iRightAdjustInPixels;
		metrics.iMoveInPixels = FontBitmap()->iMaxNormalCharWidthInPixels;
		metrics.iLeftAdjustInPixels = (TInt16)((metrics.iMoveInPixels - width) / 2);
		metrics.iRightAdjustInPixels = (TInt16)(metrics.iMoveInPixels - width - metrics.iLeftAdjustInPixels);
		}

	return metrics;	 // N.B. Not doubled for double height and double width.
	}

 
/** Gets a pointer to a bitmap and the metrics for a specified character.

Note that this function calls CharacterMetrics() if a CFontBitmap is being 
used by the bitmap font object, and maps the TCharacterMetrics values returned 
by that function to aMetrics.

If the function fails to get the bitmap and metric values (because the character 
is in an open font and has not yet been rasterized) returns EFalse.

@param aSessionHandle A session handle for the open font system.
@param aCode A character code.
@param aMetrics On return, metrics for the specified character.
@param aBitmap On return, a pointer to the bitmap for the specified character.
@return ETrue if successful, otherwise EFalse. */
EXPORT_C TBool CBitmapFont::GetCharacterData(TInt aSessionHandle,TInt aCode,
											 TOpenFontCharMetrics& aMetrics,const TUint8*& aBitmap) const
	{
	if (IsOpenFont())
		{
		const TOpenFontCharMetrics* nm;
		if (OpenFont()->GetCharacterData(aSessionHandle,aCode,nm,aBitmap))
			{
			aMetrics = *nm;
			return ETrue;
			}
		else
			return EFalse;
		}
	else
		{
		TCharacterMetrics m = CharacterMetrics(aCode,aBitmap);
		aMetrics = TOpenFontCharMetrics(m);
		return ETrue;
		}
	}

/** Gets the open font metrics. 

These metrics distinguish between maximum character height and depth and typographic 
ascent and descent, so that the clipping rectangle for text can be distinguished 
from the distance to neighbouring baselines. 

@param aMetrics Open font metrics. */
 
EXPORT_C void CBitmapFont::GetFontMetrics(TOpenFontMetrics& aMetrics) const
	{
	if (IsOpenFont())
		aMetrics = OpenFont()->Metrics();
	else
		{
		new(&aMetrics) TOpenFontMetrics;
		aMetrics.SetSize(CBitmapFont::DoHeightInPixels());
		aMetrics.SetAscent(CBitmapFont::DoAscentInPixels());
		aMetrics.SetDescent(aMetrics.Size() - aMetrics.Ascent());
		aMetrics.SetMaxHeight(aMetrics.Ascent());
		aMetrics.SetMaxDepth(aMetrics.Descent());
		aMetrics.SetMaxWidth(CBitmapFont::DoMaxCharWidthInPixels());
		}
	}

 
/** Gets the open font typeface attributes if possible. 

At present no attempt is made to sythesize these attributes for bitmap fonts 
(CFontBitmaps). 

@param aAttrib On return, the open font typeface attributes.
@return ETrue if successful; EFalse if not possible to get the open font typeface 
attributes. */
EXPORT_C TBool CBitmapFont::GetFaceAttrib(TOpenFontFaceAttrib& aAttrib) const
	{
	if (IsOpenFont())
		{
		const TOpenFontFaceAttrib* a = OpenFont()->FaceAttrib();
		if (a)
			{
			aAttrib = *a;
			return ETrue;
			}
		}
	return EFalse;
	}

/** Gets encoding if a bitmap font (a CFontBitmap) is used.

@return Bitmap encoding value. */
EXPORT_C TInt CBitmapFont::BitmapEncoding() const
	{
	if (IsOpenFont())
		return 0;
	else
		return FontBitmap()->iBitmapEncoding;
	}

/** Gets whether the open or bitmap font has the specified character.

@param aCode A character code.
@return ETrue if the font has the specified character; otherwise EFalse. */
EXPORT_C TBool CBitmapFont::HasCharacterL(TInt aCode) const
	{
	if (IsOpenFont())
		return OpenFont()->HasCharacterL(aCode);
	else
		{
		const TUint8* bytes;
		FontBitmap()->CharacterMetrics(aCode,bytes);
		return (bytes != NULL);
		}
	}

/** Gets whether the specified character needs to be rasterised.

False is returned if it is a bitmap font (a CFontBitmap) being used by the 
bitmap font object (so no rasterization is required) or if is an open font 
(a COpenFont) and the character has been rasterized.

@param aSessionHandle A session handle for the open font system.
@param aCode A character code.
@return ETrue if the character needs to be rasterized; otherwise EFalse. */
EXPORT_C TBool CBitmapFont::CharacterNeedsToBeRasterized(TInt aSessionHandle,TInt aCode) const
	{
	if (IsOpenFont())
		return OpenFont()->CharacterNeedsToBeRasterized(aSessionHandle,aCode);
	else
		return FALSE;	// characters in bitmap fonts do not need to be rasterized	
	}

/** Turns text into glyph codes and positions.
@param aText The Unicode text to shape plus context
@return The output shape header from the per-font cache, or 0 on failure.
@internalTechnology
*/
EXPORT_C TShapeHeader* CBitmapFont::ShapeTextL(const TDesC16& aText,
	TInt aSessionHandle, const TShapeMessageParameters& aParams)
	{
	TShapeHeader* shape = 0;

	if(IsOpenFont())
		{
		// get the data in a CShaper::TInput for the shaper
		CShaper::TInput input;
		input.iText = &aText;
		input.iStart = aParams.iStart;
		input.iEnd = aParams.iEnd;
		input.iScript= aParams.iScript;
		input.iLanguage = aParams.iLanguage;
		input.iMaximumAdvance = KMaxTInt;
		input.iFlags = 0;
		input.iSessionHandle = aSessionHandle;
		input.iReserved1 = 0;

		TFontShapeFunctionParameters params;
		params.iEnd = input.iEnd;
		params.iLanguage = input.iLanguage;
		params.iScript = input.iScript;
		params.iStart = input.iStart;
		params.iText = input.iText;
		
		COpenFont* openFont = OpenFont();
		
		//if already exist just increase the reference count for that session
		shape = openFont->GetShapedData(aSessionHandle,&params);
		
		if (shape == NULL)
			{
			if (!openFont->HasShaper()
 				|| (openFont->HasShaper() && ( 
 				(TUint32)(openFont->GetShaper()->ExtendedInterface(KUidShaperGetScript)) != input.iScript
 				|| (TUint32)(openFont->GetShaper()->ExtendedInterface(KUidShaperGetLang)) != input.iLanguage)))
				{ // Install the shaper
				openFont->DeleteShaper();
				InstallOpenFontShaper(openFont, input);
				}

			if (openFont->HasShaper())
				{
				TInt error = openFont->GetShaper()->ShapeText(shape, input, iHeap);
				if (error != KErrNone)
					User::Leave(error);
				
				// Put this into the session cache
				TShapeHeader* cached_header = openFont->InsertShapedDataIntoCache(aSessionHandle,&params, shape);
				if (!cached_header)
					User::LeaveNoMemory();
				iHeap->Free(shape);
				shape = cached_header;
				}
			}
		}
	return shape;
	}


void CBitmapFont::InstallOpenFontShaper(COpenFont* aOpenFont, CShaper::TInput& aShaperInput)
	{
	CShaper* shaper = NULL;
	const CArrayPtrFlat<CShaperFactory>* shaperFactoryList = aOpenFont->File()->GetFontStore()->ShaperFactoryList();

	TInt factoryCount = shaperFactoryList->Count();
	for (TInt index = 0; index < factoryCount; index++)
		{
		TRAPD(err, shaper = (*shaperFactoryList)[index]->NewShaperL(this,
				aShaperInput.iScript, aShaperInput.iLanguage,  iHeap));
		if (err == KErrNone)
			{
			aOpenFont->SetShaper(shaper);
			OstTrace0( TRACE_IMPORTANT, CBITMAPFONT_INSTALLOPENFONTSHAPER, "A shaper is installed" );
			break;
			}
		}
	}


/** Frees the memory taken up as a result of shaping
@internalTechnology */
EXPORT_C void CBitmapFont::DeleteShape(TInt aSessionHandle,TShapeHeader* aHeader)
	{
	//safe to assume aHeader is never NULL?
	if (IsOpenFont())
		{
		//we just need to decrease the reference count, no deletion here
		//as the entry in cache can still be reused by other client
		//only in the case where the memory is full, the freeing will
		//delete any cache entry that is not referenced at all
		TInt ret=OpenFont()->DecrementCachedRefCount(aSessionHandle,aHeader);
		//panic in debug mode if trying to delete something that is not there.
		if ((ret != KErrNone) && (ret != KErrNotFound))
		    {
		    OstTrace1( TRACE_FATAL, CBITMAPFONT_DELETESHAPE, "OpenFont()->DecrementCachedRefCount() return %d, Invariant", ret);
		    __ASSERT_DEBUG(0, User::Invariant());
		    }
		}
	}

EXPORT_C void CBitmapFont::operator delete(TAny *aThis)
	{
	if (((CBitmapFont *)aThis)->iHeap)
		((CBitmapFont *)aThis)->iHeap->Free(aThis);
	}

TInt CBitmapFont::Width(TInt aNum) const
	{
	TInt widthfactor=iAlgStyle.WidthFactor();
	return ((widthfactor==1)? aNum: aNum*widthfactor);
	}

TInt CBitmapFont::Height(TInt aNum) const
	{
	TInt heightfactor=iAlgStyle.HeightFactor();
	return ((heightfactor==1)? aNum: aNum*heightfactor);
	}

CFontBitmap* CBitmapFont::FontBitmap() const
/** This member is private and not intended for use. */
	{
	if (IsOpenFont())
	    {
	    OstTrace0( TRACE_FATAL, CBITMAPFONT_FONTBITMAP, "Panic(EFntTypefaceHasNoFontBitmaps)" );
	    __ASSERT_ALWAYS(0, Panic(EFntTypefaceHasNoFontBitmaps));
	    }
    if(iFontBitmapOffset)
        return reinterpret_cast<CFontBitmap*>(reinterpret_cast<TInt>(this)+iFontBitmapOffset);
    else
        return NULL;
	}

/** Gets a font table.
@param aTag: Input. The name of the font table.
@param aTableContent: Output. To return the address of the table content.
@param aLength: Output. To return the length (in bytes) of the table.
@param aSessionHandle: Input. A handle to the session requesting this table.
@return KErrNone on success, specific error code on failure.
@internalTechnology
*/
EXPORT_C TInt CBitmapFont::GetFontTable(TUint32 aTag, TAny *&aTableContent, 
        TInt &aLength, TInt aSessionHandle) 
    {
    COpenFont *fontPtr = NULL;
    if (IsOpenFont())
        fontPtr = OpenFont();
    else
        return KErrNotSupported;
    
    // try to find it in cache.
    CFontStore *fntStore = fontPtr->File()->GetFontStore();
    TUid fileUid = fontPtr->File()->Uid();
    TInt ret = fntStore->FindFontTableInCache(fileUid, aTag, aTableContent, aLength);
    if (KErrNone == ret)
        {
        ret = fntStore->IncFontTableRefCount(fileUid, aTag, aSessionHandle);
        return ret;
        }
    
    // font table not found in cache.
    ret = fontPtr->GetFontTable(aTag, aTableContent, aLength);
    if (KErrNone == ret)
        {
        ret = fntStore->CacheFontTable(fileUid, aTag, aTableContent, aLength);
        if (KErrNone == ret)
            {
            ret = fntStore->IncFontTableRefCount(fileUid, aTag, aSessionHandle);
            }
        else 
            {
            aTableContent = NULL;
            }
        }
    
    return ret;
    }

/** Release a font table. Decrement its reference count. Remove from cache if 
 * reference decreases to zero.
@param aTag: Input. The name of the font table to be released.
@param aSessionHandle: Input. Handle to the session releasing this table.
@return KErrNone on success, specific error code on failure.
@internalTechnology
*/
EXPORT_C void CBitmapFont::ReleaseFontTable(TUint32 aTag, 
        TInt aSessionHandle)
    {
    COpenFont *fontPtr = NULL;
    if (IsOpenFont())
        fontPtr = OpenFont();
    else
        return;
    
    CFontStore *fntStore = fontPtr->File()->GetFontStore();
    TUid fileUid = fontPtr->File()->Uid();
    fntStore->ReleaseFontTable(fileUid, aTag, aSessionHandle);
    }


/** Release a number of glyph outlines. Decrement their reference count.
 * Remove it from cache if reference count decreases to zero.
@param aCount: Input. Number of outlines to be released.
@param aCodes: Input. An array of codes. Its interpretation depends on the parameter
    'aIsGlyphId' (see below).
@param aIsGlyphId: Input. When aIsGlyphId==ETrue, 'aCodes' is an array of glyph ID's.
    When aIsGlyphId==EFalse, 'aCodes' is an array of Unicode values.
@param aHinted: Input. To indicate if the outlines are hinted or unhinted.
@param aSessionHandle: Input. Handle to the session releasing the outlines.
@return KErrNone on success, specific error code on failure.
@internalTechnology
*/
EXPORT_C void CBitmapFont::ReleaseGlyphOutlines(TInt aCount, const TUint *aCodes, 
        TBool aHinted, TInt aSessionHandle)
    {
    COpenFont *fontPtr = NULL;
        if (IsOpenFont())
            fontPtr = OpenFont();
        else
            return;
    
    CFontStore *fontStore = fontPtr->File()->GetFontStore();
    
    for (TInt i = 0; i < aCount; ++i)
        {
        if (aHinted)
            {
            THintedOutlineId outlineId(fontPtr, aCodes[i]);
            fontStore->ReleaseHintedOutline(outlineId, aSessionHandle);
            }
        else
            {
            TInt faceId = fontPtr->FaceIndex();
            TUnhintedOutlineId outlineId(fontPtr->File()->Uid(), faceId, aCodes[i]);
            fontStore->ReleaseUnhintedOutline(outlineId, aSessionHandle);
            }
        }
    }

/** Gets a font table.
@param aCode: Input. An glyph code. Its interpretation depends on the parameter
    'aIsGlyphId' (see below).
@param aIsGlyphId: Input. When aIsGlyphId==ETrue, 'aCode' is a glyph ID.
    When aIsGlyphId==EFalse, 'aCode' is a Unicode values.
@param aHinted: Input. To indicate if hinted or unhinted outline is needed.
@param aOutline: Output. A 'void*' pointer, pointing to the outline in memory.
@param aLength: Output. A TInt, recording the lenght (in bytes) of the outline.
@param aSessionHandle: Input. Handle to the session requesting this outline.
@return KErrNone on success, specific error code on failure.
@internalTechnology
*/
EXPORT_C TInt CBitmapFont::GetGlyphOutline(TUint aCode, 
        TBool aHinted, TAny *&aOutline, TInt &aLength, TInt aSessionHandle)
    {
    COpenFont *fontPtr = NULL;
        if (IsOpenFont())
            fontPtr = OpenFont();
        else
            return KErrNotSupported;
    
    CFontStore *fontStore = fontPtr->File()->GetFontStore();
    TAny *outlineData = NULL; 
    TInt len = KErrGeneral;
    TInt ret = KErrNone;
    if (!aHinted)
        {
        TInt faceId = fontPtr->FaceIndex();
        TUnhintedOutlineId outlineId(fontPtr->File()->Uid(), faceId, aCode);
        ret = fontStore->FindUnhintedOutlineInCache(outlineId, outlineData, len);
        if (KErrNotFound == ret)
            {
            TAny* tmpOutline = 0; 
            TInt tmpLen = 0;
            ret = fontPtr->GetGlyphOutline(aCode, aHinted, tmpOutline, tmpLen);
            if (KErrNone == ret)
                {
                fontStore->CacheUnhintedOutline(outlineId,
                        tmpOutline, (TInt)tmpLen, outlineData, len);
                }
            User::Free(tmpOutline);
            }
        if (KErrNone == ret)
            {
            fontStore->IncreaseUnhintedOutlineRefCount(outlineId, aSessionHandle);
            }
        }
    else 
        {
        THintedOutlineId outlineId(fontPtr, aCode);
        ret = fontStore->FindHintedOutlineInCache(outlineId, outlineData, len);
        if (KErrNotFound == ret)
            {
            TAny* tmpOutline = 0; 
            TInt tmpLen = 0;
            ret = fontPtr->GetGlyphOutline(aCode, aHinted, tmpOutline, tmpLen);
            if (KErrNone == ret)
                {
                fontStore->CacheHintedOutline(outlineId,
                        tmpOutline, (TInt)tmpLen, outlineData, len);
                }
            User::Free(tmpOutline);
            }
        if (KErrNone == ret)
            {
            fontStore->IncreaseHintedOutlineRefCount(outlineId, aSessionHandle);
            }
        }
    
    aOutline = outlineData;
    aLength = len;    
    return KErrNone;
    }

EXPORT_C TUint32 CBitmapFont::UniqueFontId()
	{
	return iUniqueFontId;
	}

void CBitmapFont::SetUniqueFontId(TUint32 aUniqueFontId)
	{
	iUniqueFontId = aUniqueFontId;	
	}

/** API extension system that enables the caller to access a particular API
extension function. As an overload of this function in a derived class
it calls its immediate parent implementation for any extension function Uid
that it does not recognize and handle.
@param aInterfaceId UID of the required extension function
@param aParam Pointer to an arbitrary parameter block that can be used to
provide and/or return information to/from the particular extension function,
defaults to NULL.
@return Integer return value from extension function
@internalTechnology
@released
*/
EXPORT_C TInt CBitmapFont::DoExtendedFunction(TUid aFunctionId, TAny* aParam) const
	{
	if (KFontCapitalAscent == aFunctionId)
		{
		return Height(IsOpenFont() ? OpenFont()->FontCapitalAscent() : FontBitmap()->FontCapitalAscent());
		}
	else if (KFontMaxAscent == aFunctionId)
		{
		return Height(IsOpenFont() ? OpenFont()->FontMaxAscent() : FontBitmap()->FontMaxAscent());
		}
	else if (KFontStandardDescent == aFunctionId)
		{
		return Height(IsOpenFont() ? OpenFont()->FontStandardDescent() : FontBitmap()->FontStandardDescent());
		}
	else if (KFontMaxDescent == aFunctionId)
		{
		return Height(IsOpenFont() ? OpenFont()->FontMaxDescent() : FontBitmap()->FontMaxDescent());
		}
	else if (KFontLineGap == aFunctionId)
		{
		return Height(IsOpenFont() ? OpenFont()->FontLineGap() : FontBitmap()->FontLineGap());
		}
	
	return CFont::DoExtendedFunction(aFunctionId, aParam);
	}


CFontTableCacheItem::CFontTableCacheItem(TUid &aFileUid, const TUint32 aTag, 
        TInt aOffset, TInt aLength): iFileUid(aFileUid), iTag(aTag),
        iOffset(aOffset), iLength(aLength)
    {
    // a null constructor
    }

CFontTableCacheItem::~CFontTableCacheItem()
    {
    iUsers.ResetAndDestroy();
    iUsers.Close();
    }
    
TBool CFontTableCacheItem::HasOutstandingRefCount()
    {
    TInt count = iUsers.Count();
    for (TInt j = 0; j < count; ++j)
        {
        if (iUsers[j]->iRefCount > 0) 
            {
            return ETrue;
            }
        }
    return EFalse;
    }


TInt CFontTableCacheItem::FindUser(TInt aSessionHandle, TInt *id) 
    {
    TInt len = iUsers.Count();
    for (TInt i = 0; i < len; ++i)
        {
        if (aSessionHandle == iUsers[i]->iSessionHandle)
            {
            *id = i;
            return KErrNone;
            }
        }
    return KErrNotFound;
    }

TInt CFontTableCacheItem::DecRefCount(TInt aSessionHandle)
    {
    TInt id = 0; 
    TInt ret = FindUser(aSessionHandle, &id);
    if (KErrNone == ret)
        {
        iUsers[id]->iRefCount--;
        if (0 == iUsers[id]->iRefCount) 
            {
            delete iUsers[id];
            iUsers.Remove(id);
            }
        return iUsers.Count();
        }
    return KErrNotFound;
    }

TInt CFontTableCacheItem::IncRefCount(TInt aSessionHandle)
    {
    TInt id = 0; 
    TInt ret = FindUser(aSessionHandle, &id);
    if (KErrNone == ret)
        {
        iUsers[id]->iRefCount++;
        }
    else
        {
        TCacheUserInfo *newUser = new TCacheUserInfo(aSessionHandle, 1);
        if (NULL != newUser)
            {
            TRAP(ret, iUsers.AppendL(newUser));
            }
        else 
            {
            ret = KErrNoMemory;
            }
        //coverity[leaked_storage]
        // The 'newUser' is kept in iUsers. It will be deleted in DecRefCount(). 
        }
    return ret;
    }

TInt CFontTableCache::Append(TUid aFileUid, TUint32 aTag, 
        TAny *&aContent, TInt aLength) 
    {
    TInt ret = 0;
    if ((TUint32)iCacheMemMon.GetMemUsage() >= KFontTable_GlyphOutline_CacheMaxMem)
        {
        RDebug::Printf("Table/Glyph cache full. Unable to add new item.");
        return KErrNoMemory;
        }
    // make a copy of the table content on the shared heap.
    TAny *sharedCopy = iHeap->Alloc(aLength);
    if (NULL == sharedCopy) 
        {
        return KErrNoMemory;
        }
    
    Mem::Copy(sharedCopy, aContent, aLength);

    CFontTableCacheItem *newItem = NULL;
    TInt offset = PointerToOffset(sharedCopy, iHeap->Base());
    TRAP(ret, newItem = new(ELeave) CFontTableCacheItem(aFileUid, aTag, offset, aLength));
    if (KErrNone != ret)
        {
        iHeap->Free(sharedCopy);
        return ret;
        }
    
    TRAP(ret, iCacheItems.AppendL(newItem));
    if (KErrNone == ret)
        {
        // do not free 'aContent', because the mem is managed by
        // rasterizer cache.
        aContent = sharedCopy;
        iCacheMemMon.Inc(aLength);
        }
    else 
        {
        iHeap->Free(sharedCopy);
        }
#ifdef _DEBUG
    GetCacheState(__func__);
#endif
    return ret;
    }

TInt CFontTableCache::Find(TUid aFileUid, TUint32 aTag, TAny *&aContent, 
        TInt &aLength, TInt *id)
    {
    *id = KErrNotFound;
    TInt len = iCacheItems.Count();
    
    TInt ret = KErrNotFound;
    for (TInt i = 0; i < len; ++i)
        {
        CFontTableCacheItem *item = iCacheItems[i]; 
        if (item->iFileUid == aFileUid && item->iTag == aTag) 
            {
            aContent = OffsetToPointer(item->iOffset, iHeap->Base());
            aLength = item->iLength;
            *id = i;
            ret = KErrNone;
            break;
            }
        }
    
#ifdef _DEBUG
    GetCacheState(__func__);
#endif
    return ret;
    }

CFontTableCache::CFontTableCache(RHeap *aHeap, TFontTableGlyphOutlineCacheMemMonitor &aMon): 
    iCacheMemMon(aMon), iHeap(aHeap) 
    {
    // null constructor
    }

CFontTableCache::~CFontTableCache()
    {
    for (TInt i = 0; i < iCacheItems.Count(); ++i)
        {
        iHeap->Free(OffsetToPointer(iCacheItems[i]->iOffset, iHeap->Base()));
        }
    iCacheItems.ResetAndDestroy();
    iCacheItems.Close();
    }

TInt CFontTableCache::IncRefCount(TUid aFileUid, TUint32 aTag, TInt aSessionHandle)
    {
    TAny *outline = NULL;
    TInt len = 0;
    TInt id = 0;
    
    TInt ret = Find(aFileUid, aTag, outline, len, &id);
    if (KErrNone == ret)
        {
        ret = iCacheItems[id]->IncRefCount(aSessionHandle);
        }
    
#ifdef _DEBUG
    GetCacheState(__func__);
#endif
    return ret;
    }

TInt CFontTableCache::DecRefCount(TUid aFileUid, TUint32 aTag, TInt aSessionHandle)
    {
    TAny *outline = NULL;
    TInt len = 0;
    TInt id = 0;
    
    TInt ret = Find(aFileUid, aTag, outline, len, &id);
    if (KErrNone == ret) 
        {
        TInt numUsers = iCacheItems[id]->DecRefCount(aSessionHandle);
        if (0 == numUsers) 
            {
            // There is no outstanding reference to the cache item.
            iHeap->Free(outline);
            iCacheMemMon.Dec(len);
            delete (iCacheItems[id]);
            iCacheItems.Remove(id);
            }
        }
    
#ifdef _DEBUG
    GetCacheState(__func__);
#endif
    return ret;
    }

TBool CFontTableCache::HasOutstandingRefCount() 
    {
    TInt len = iCacheItems.Count();
   
    for (TInt i = 0; i < len; ++i)
        {
        if (iCacheItems[i]->HasOutstandingRefCount())
            {
            return ETrue;
            }
        }
    return EFalse;
    }

TBool CFontTableCache::HasOutstandingRefCountWithUid(TUid aFileUid)
    {
    TInt len = iCacheItems.Count();
   
    for (TInt i = 0; i < len; ++i)
        {
        if (iCacheItems[i]->iFileUid == aFileUid) 
            {
            if (iCacheItems[i]->HasOutstandingRefCount())
                {
                return ETrue;
                }
            }
        }
    return EFalse;
    }

void CFontTableCache::CleanupCacheOnOpenFontFileRemoval(COpenFontFile *)
    {
    // In CFontStore::RemoveFile(), a font file is not allowed to be removed if
    // there are outstanding ref counts on any table in it. If that check passed 
    // and this function is called, there shall be no cache item for that file.
    
    // Currently a cache item having a refcount of 0 is removed immediately.
    // If this strategy is changed in the future, we may need to do some
    // cleanup here.
    }

void CFontTableCache::CleanupCacheOnFbsSessionTermination(TInt aSessionHandle)
    {
    TInt len = iCacheItems.Count();
   
    for (TInt i = 0; i < len; ++i)
        {
        TInt id = -1;
        if (KErrNone == iCacheItems[i]->FindUser(aSessionHandle, &id))
            {
            iCacheItems[i]->iUsers.Remove(id);
            if (iCacheItems[i]->iUsers.Count() == 0)
                {
                iHeap->Free(OffsetToPointer(iCacheItems[i]->iOffset, iHeap->Base()));
                iCacheMemMon.Dec(iCacheItems[i]->iLength);
                delete iCacheItems[i];
                iCacheItems.Remove(i);
                }
            }
        }
    }


#ifdef _DEBUG
TInt CFontTableCache::GetCacheState(const char *func) 
    {
    RDebug::Printf("%s called from %s: ", __func__, func);
    TBuf<256> buf;
    
    int len = iCacheItems.Count();
    int numTables = 0, numSessions = 0, totalRef = 0;
    buf.Append(_L("Table cache: "));
    for (int i = 0; i < len; ++i)
        {
        ++numTables;
        TInt abc = iCacheItems[i]->iUsers.Count();
        numSessions += abc;
        for (int j = 0; j < abc; ++j)
            {
            totalRef += iCacheItems[i]->iUsers[j]->iRefCount;
            }
        }
    if (0 == iCacheItems.Count())
        {
        buf.Append(_L("cache empty. "));
        }
    else 
        {
        buf.AppendFormat(_L("%d tables referenced by %d sessions, total ref count %d"),
                numTables, numSessions, totalRef);
        }
    RDebug::RawPrint(buf);
    return 0;
    }
#endif



TInt COutlineCacheItem::FindUser(TInt aSessionHandle, TInt *id) 
    {
    TInt len = iUsers.Count();
    for (TInt i = 0; i < len; ++i)
        {
        if (aSessionHandle == iUsers[i]->iSessionHandle)
            {
            *id = i;
            return KErrNone;
            }
        }
    return KErrNotFound;
    }

COutlineCacheItem::~COutlineCacheItem() 
    { 
    iUsers.ResetAndDestroy(); 
    }

COutlineCacheItem::COutlineCacheItem(TInt aOffset, TInt aLength):
    iOffset(aOffset), iLength(aLength)
    {
    // a null constructor.
    }

TInt COutlineCacheItem::DecRefCount(TInt aSessionHandle)
    {
    TInt id = 0; 
    TInt ret = FindUser(aSessionHandle, &id);
    if (KErrNone == ret)
        {
        iUsers[id]->iRefCount--;
        if (0 == iUsers[id]->iRefCount) 
            {
            delete iUsers[id];
            iUsers.Remove(id);
            }
        return iUsers.Count();
        }
    return KErrNotFound;
    }

TInt COutlineCacheItem::IncRefCount(TInt aSessionHandle)
    {
    TInt id = 0; 
    TInt ret = FindUser(aSessionHandle, &id);
    if (KErrNone == ret)
        {
        iUsers[id]->iRefCount++;
        }
    else
        {
        TCacheUserInfo *newUser = new TCacheUserInfo(aSessionHandle, 1);
        if (NULL != newUser)
            {
            TRAP(ret, iUsers.AppendL(newUser));
            }
        else 
            {
            ret = KErrNoMemory;
            }
        //coverity[leaked_storage]
        // The 'newUser' is kept in iUsers. It will be deleted in DecRefCount().
        }
    return ret;
    }

#ifdef _DEBUG
TInt CUnhintedOutlineCache::GetCacheState(const char *func)
    {
    RDebug::Printf("%s called from %s: ", __func__, func);
    int numSessions = 0, totalRef = 0;
    THashMapIter<TUnhintedOutlineId, COutlineCacheItem*> it(iItemIdMap);
    it.NextValue();
    while (it.CurrentValue())
        {
        COutlineCacheItem **data = it.CurrentValue();
        int len = (*data)->iUsers.Count();
        numSessions += len;
        for (int j = 0; j < len; ++j)
            {
            totalRef += (*data)->iUsers[j]->iRefCount;
            }
        it.NextValue();
        }
    
    TBuf<256> buf;
    buf.Append(_L("Unhinted outline cache: "));
    TInt numItems = iItemIdMap.Count();
    if (0 == numItems)
        {
        buf.Append(_L("empty. "));
        }
    else 
        {
        buf.AppendFormat(_L("%d glyphs, %d sessions, total refcount %d"),
                numItems, numSessions, totalRef);
        }
        
    RDebug::RawPrint(buf);
            
    return 0;
    }
#endif

CUnhintedOutlineCache::CUnhintedOutlineCache(RHeap *aHeap, TFontTableGlyphOutlineCacheMemMonitor &aMon):
    iCacheMemMon(aMon), iHeap(aHeap), 
    iItemIdMap(THashFunction32<TUnhintedOutlineId>(CUnhintedOutlineCache::IdHash),
        TIdentityRelation<TUnhintedOutlineId>(CUnhintedOutlineCache::IdIdentity))
    { 
    
    }

CUnhintedOutlineCache::~CUnhintedOutlineCache()
    {
    THashMapIter<TUnhintedOutlineId, COutlineCacheItem*> it(iItemIdMap);
    it.NextValue();
    while (it.CurrentValue())
        {
        const TUnhintedOutlineId *outlineId = it.CurrentKey();
        COutlineCacheItem **data = it.CurrentValue();
        
        // loop body here!
        iHeap->Free(OffsetToPointer((*data)->iOffset, iHeap->Base()));
        delete (*data);
        iItemIdMap.Remove(*outlineId);
        // end loop body
        
        it.NextValue();
        }
    return;
    }


TInt CUnhintedOutlineCache::CleanupCacheOnOpenFontFileRemoval(COpenFontFile *aFontFile)
    {
    TUid fileUid = aFontFile->Uid();
    
    THashMapIter<TUnhintedOutlineId, COutlineCacheItem*> it(iItemIdMap);
    it.NextValue();
    while (it.CurrentValue())
        {
        const TUnhintedOutlineId *outlineId = it.CurrentKey();
        COutlineCacheItem **data = it.CurrentValue();
        
        // loop body here!
        if (outlineId->iFileUid == fileUid)
            {
            iHeap->Free(OffsetToPointer((*data)->iOffset, iHeap->Base()));
            iCacheMemMon.Dec((*data)->iLength);
            delete (*data);
            iItemIdMap.Remove(*outlineId);
            }
        // end loop body
        
        it.NextValue();
        }
    return KErrNone;
    }

TInt CUnhintedOutlineCache::CleanupCacheOnFbsSessionTermination(TInt aSessionHandle)
    {
    THashMapIter<TUnhintedOutlineId, COutlineCacheItem*> it(iItemIdMap);
    it.NextValue();
    while (it.CurrentValue())
        {
        const TUnhintedOutlineId *outlineId = it.CurrentKey();
        COutlineCacheItem **data = it.CurrentValue();
        
        // loop body here!
        TInt id = 0;
        TInt ret = (*data)->FindUser(aSessionHandle, &id);
        if (KErrNone == ret)
            {
            delete (*data)->iUsers[id];
            (*data)->iUsers.Remove(id);
            if (0 == (*data)->iUsers.Count())
                {
                iHeap->Free(OffsetToPointer((*data)->iOffset, iHeap->Base()));
                iCacheMemMon.Dec((*data)->iLength);
                delete (*data);
                iItemIdMap.Remove(*outlineId);
                }
            }
        // end loop body
        
        it.NextValue();
        }
    return KErrNone;
    }


TInt CUnhintedOutlineCache::CacheUnhintedOutline(const TUnhintedOutlineId &aOutlineId, 
        TAny* const aData, const TInt aLength, TAny *&aOutline, TInt &aLen)
    {
#ifdef _DEBUG
    GetCacheState(__func__);
#endif
    if ((TUint32)iCacheMemMon.GetMemUsage() >= KFontTable_GlyphOutline_CacheMaxMem)
        {
        RDebug::Printf("Table/Glyph cache full. Unable to add new item.");
        return KErrNoMemory;
        }
    
    aLen = KErrGeneral;
    TInt ret1= KErrNone;

    // make a copy of the outline data on the shared heap.
    TAny *sharedCopy = iHeap->Alloc(aLength);
    if (NULL == sharedCopy) 
        {
        return KErrNoMemory;
        }
    
    Mem::Copy(sharedCopy, aData, aLength);

    COutlineCacheItem *newItem = NULL;
    TInt offset = PointerToOffset(sharedCopy, iHeap->Base());
    TRAP(ret1, newItem = new(ELeave) COutlineCacheItem(offset, aLength));
    if (KErrNone != ret1)
        {
        iHeap->Free(sharedCopy);
        sharedCopy = NULL;
        }
    else 
        {
        TRAP(ret1, iItemIdMap.InsertL(aOutlineId, newItem));
        if (KErrNone != ret1)
            {
            delete newItem;
            iHeap->Free(sharedCopy);
            sharedCopy = NULL;
            }
        else
            {
            iCacheMemMon.Inc(aLength);
            aLen = aLength;
            }
        }
    aOutline = sharedCopy;
    return ret1;  
    }

TInt CUnhintedOutlineCache::IncRefCount(const TUnhintedOutlineId &aOutlineId,
        TInt aSessionHandle)
    {
#ifdef _DEBUG
    GetCacheState(__func__);
#endif
    COutlineCacheItem **ret = iItemIdMap.Find(aOutlineId);
    if (NULL != ret)
        {
        (*ret)->IncRefCount(aSessionHandle);
        }
    return (NULL==ret?KErrNotFound:KErrNone);
    }

TInt CUnhintedOutlineCache::DecRefCount(const TUnhintedOutlineId &aOutlineId,
        TInt aSessionHandle)
    {
#ifdef _DEBUG
    GetCacheState(__func__);
#endif
    COutlineCacheItem **ret = iItemIdMap.Find(aOutlineId); 
    if (NULL != ret) 
        {
        TInt numUsers = (*ret)->DecRefCount(aSessionHandle);
        if (0 == numUsers) 
            {
            // There is no outstanding reference to the cache item.
            iHeap->Free(OffsetToPointer((*ret)->iOffset, iHeap->Base()));
            iCacheMemMon.Dec((*ret)->iLength);
            delete (*ret);
            iItemIdMap.Remove(aOutlineId);
            }
        }
    return (NULL==ret?KErrNotFound:KErrNone);
    }

TInt CUnhintedOutlineCache::Find(const TUnhintedOutlineId &aOutlineId, TAny *&aData, 
        TInt &aLength)
    {
    COutlineCacheItem **ret = iItemIdMap.Find(aOutlineId);
    TInt ret2 = KErrNone;
    if (NULL != ret)
        {
        aData = OffsetToPointer((*ret)->iOffset, iHeap->Base());
        aLength = (*ret)->iLength;
        }
    else
        {
        ret2 = KErrNotFound;
        }
    return ret2;
    }
    
  
TUint32 CUnhintedOutlineCache::IdHash(const TUnhintedOutlineId &outlineId)
    {
    // The hash value: 
    // bits 0-15: glyph id; 
    // bits 16-23: lower 8 bit of font file uid
    // bits 24-27: lower 4 bit of the face index
    // bit 28: 'isGlyphId'
    TUint32 ret = 0;
    ret |= (outlineId.iId & KOutlineGlyphIdHashMask);
    ret |= (KOutlineFileUidHashMask & (outlineId.iFileUid.iUid << 16));
    ret |= (KOutlineFaceIndexHashMask & (outlineId.iFaceIndex << 24));
    ret = (ret % 701);
    return ret;
    }

TBool CUnhintedOutlineCache::IdIdentity(const TUnhintedOutlineId &id1, const TUnhintedOutlineId &id2)
    {
    return id1.iId == id2.iId && id1.iFaceIndex == id2.iFaceIndex &&
            id1.iFileUid == id2.iFileUid;
    }

// hinted outline cache
#ifdef _DEBUG
TInt CHintedOutlineCache::GetCacheState(const char *func)
    {
    RDebug::Printf("%s called from %s: ", __func__, func);
    int numSessions = 0, totalRef = 0;
    THashMapIter<THintedOutlineId, COutlineCacheItem*> it(iItemIdMap);
    it.NextValue();
    while (it.CurrentValue())
        {
        COutlineCacheItem **data = it.CurrentValue();
        int len = (*data)->iUsers.Count();
        numSessions += len;
        for (int j = 0; j < len; ++j)
            {
            totalRef += (*data)->iUsers[j]->iRefCount;
            }
        it.NextValue();
        }
    
    TBuf<256> buf;
    buf.Append(_L("Hinted outline cache: "));
    TInt numItems = iItemIdMap.Count();
    if (0 == numItems)
        {
        buf.Append(_L("empty. "));
        }
    else 
        {
        buf.AppendFormat(_L("%d glyphs, %d sessions, total refcount %d"),
                numItems, numSessions, totalRef);
        }
        
    RDebug::RawPrint(buf);
            
    return 0;
    }
#endif

TInt CHintedOutlineCache::CleanupCacheOnOpenFontRemoval(COpenFont *aFont)
    {
    THashMapIter<THintedOutlineId, COutlineCacheItem*> it(iItemIdMap);
    it.NextValue();
    while (it.CurrentValue())
        {
        const THintedOutlineId *outlineId = it.CurrentKey();
        COutlineCacheItem **data = it.CurrentValue();
        
        // loop body here!
        if (outlineId->iFont == aFont)
            {
            iCacheMemMon.Dec((*data)->iLength);
            iHeap->Free(OffsetToPointer((*data)->iOffset, iHeap->Base()));
            delete (*data);
            iItemIdMap.Remove(*outlineId);
            }
        // end loop body
        
        it.NextValue();
        }
    return KErrNone;
    }

TInt CHintedOutlineCache::CleanupCacheOnFbsSessionTermination(TInt aSessionHandle)
    {
    THashMapIter<THintedOutlineId, COutlineCacheItem*> it(iItemIdMap);
    it.NextValue();
    while (it.CurrentValue())
        {
        const THintedOutlineId *outlineId = it.CurrentKey();
        COutlineCacheItem **data = it.CurrentValue();
        
        // loop body here!
        TInt id = 0; 
        TInt ret = (*data)->FindUser(aSessionHandle, &id);
        if (KErrNone == ret)
            {
            delete (*data)->iUsers[id];
            (*data)->iUsers.Remove(id);
            if (0 == (*data)->iUsers.Count())
                {
                iCacheMemMon.Dec((*data)->iLength);
                iHeap->Free(OffsetToPointer((*data)->iOffset, iHeap->Base()));
                delete (*data);
                iItemIdMap.Remove(*outlineId);
                }
            }
        // end loop body
        
        it.NextValue();
        }

    return KErrNone;
    }


CHintedOutlineCache::CHintedOutlineCache(RHeap *aHeap, TFontTableGlyphOutlineCacheMemMonitor &aMon):
    iCacheMemMon(aMon), iHeap(aHeap), 
    iItemIdMap(THashFunction32<THintedOutlineId>(CHintedOutlineCache::IdHash),
        TIdentityRelation<THintedOutlineId>(CHintedOutlineCache::IdIdentity)) 
    { 
    // a null constructor
    }

TInt CHintedOutlineCache::CacheHintedOutline(const THintedOutlineId &aOutlineId,
        TAny* aData, TInt aLength, TAny *&aOutline, TInt &aLen)
    {
#ifdef _DEBUG
    GetCacheState(__func__);
#endif
    if ((TUint32)iCacheMemMon.GetMemUsage() >= KFontTable_GlyphOutline_CacheMaxMem)
        {
        RDebug::Printf("Table/Glyph cache full. Unable to add new item.");
        return KErrNoMemory;
        }
    
    aLen = KErrGeneral;
    TInt ret = KErrNone;
    // make a copy of the outline data on the shared heap.
    TAny *sharedCopy = iHeap->Alloc(aLength);
    if (NULL == sharedCopy) 
        {
        return KErrNoMemory;
        }
    
    Mem::Copy(sharedCopy, aData, aLength);

    COutlineCacheItem *newItem = NULL;
    TInt offset = PointerToOffset(sharedCopy, iHeap->Base());
    TRAP(ret, newItem = new(ELeave) COutlineCacheItem(offset, aLength));
    if (KErrNone != ret)
        {
        iHeap->Free(sharedCopy);
        sharedCopy = NULL;
        }
    else 
        {
        TRAP(ret, iItemIdMap.InsertL(aOutlineId, newItem));
        if (KErrNone != ret)
            {
            delete newItem;
            iHeap->Free(sharedCopy);
            sharedCopy = NULL;
            }
        else
            {
            iCacheMemMon.Inc(aLength);
            aLen = aLength;
            }
        }
    aOutline = sharedCopy;
    return ret;
    }

TInt CHintedOutlineCache::IncRefCount(const THintedOutlineId &aOutlineId, 
        TInt aSessionHandle)
    {
#ifdef _DEBUG
    GetCacheState(__func__);
#endif
    COutlineCacheItem **data = iItemIdMap.Find(aOutlineId);
    if (NULL != data)
        {
        (*data)->IncRefCount(aSessionHandle);
        }
    return (NULL==data?KErrNotFound:KErrNone);
    }

TInt CHintedOutlineCache::DecRefCount(const THintedOutlineId &aOutlineId,
        TInt aSessionHandle)
    {
#ifdef _DEBUG
    GetCacheState(__func__);
#endif
    COutlineCacheItem **data = iItemIdMap.Find(aOutlineId);
    if (NULL != data) 
        {
        TInt numUsers = (*data)->DecRefCount(aSessionHandle);
        if (0 == numUsers) 
            {
            // There is no outstanding reference to the cache item.
            iCacheMemMon.Dec((*data)->iLength);
            iHeap->Free(OffsetToPointer((*data)->iOffset, iHeap->Base()));
            delete (*data);
            iItemIdMap.Remove(aOutlineId);
            }
        }
    return (NULL==data?KErrNotFound:KErrNone);
    }

TInt CHintedOutlineCache::Find(const THintedOutlineId &aOutlineId, 
        TAny *&aData, TInt &aLength)
    {
    COutlineCacheItem **ret = iItemIdMap.Find(aOutlineId);
    TInt ret2 = KErrNone;
    if (NULL != ret)
        {
        aData = OffsetToPointer((*ret)->iOffset, iHeap->Base());
        aLength = (*ret)->iLength;
        }
    else
        {
        ret2 = KErrNotFound;
        }
    return ret2;
    }


TUint32 CHintedOutlineCache::IdHash(const THintedOutlineId &outlineId)
    {
    // The hash value:
    // bits 0-15: the outline id
    // bits 16-27: the lower 12 bits of the font pointer
    // bit 28: 'isGlyphId'
    
    TUint32 ret = 0;
    ret |= (KOutlineGlyphIdHashMask & outlineId.iId);
    ret |= (KOutlineFontPtrHashMask & (((TUint32)outlineId.iFont) << 16));
    ret = ret % 701;
    return ret;
    }

TBool CHintedOutlineCache::IdIdentity(const THintedOutlineId &id1, const THintedOutlineId &id2)
    {
    return id1.iId == id2.iId && id1.iFont == id2.iFont;
    }
// hinted cache


CFontStore::CFontStore(RHeap* aHeap):
	iKPixelWidthInTwips(KDefaultPixelWidthInTwips),
	iKPixelHeightInTwips(KDefaultPixelHeightInTwips),
	iFs(),
	iHeap(aHeap),
	iFontStoreFileList(KDefaultArrayGranularity),			// list of EPOC Bitmap font files loaded
	iTypefaceList(KDefaultArrayGranularity),
	iFontBitmapList(KDefaultArrayGranularity),
	iTypefaceFontBitmapList(KDefaultArrayGranularity),
	iOpenFontFileList(KDefaultArrayGranularity),			// list of 'Open Font' files loaded
	iOpenFontRasterizerList(KDefaultArrayGranularity),		// open rasterizers, in load order
	iOpenFontUid(1),
	iDefaultBitmapType(EMonochromeGlyphBitmap),
	iShaperFactoryList(KDefaultArrayGranularity),
	iOpenFontShaperCacheMemUsage(0),
	iNumberOfShaperCaches(0),
	iOpenFontTypefaceSupportList(KDefaultArrayGranularity),	// list of Open Font typefaces available
	iUniqueFontIdCount(0)									// used to give every font a unique id
	{
	}

void CFontStore::ConstructL()
	{
	// Cache is placed in the shared heap
	iOpenFontSessionCacheList = (COpenFontSessionCacheList*)iHeap->AllocL(sizeof(COpenFontSessionCacheList));
	new(iOpenFontSessionCacheList) COpenFontSessionCacheList;

	CTypefaceStore::ConstructL();
	User::LeaveIfError(iFs.Connect());

	TMachineInfoV1Buf machineInfoBuffer;
	User::LeaveIfError(UserHal::MachineInfo(machineInfoBuffer));

	const TSize twipSize = machineInfoBuffer().iPhysicalScreenSize;
	const TSize pixelSize = machineInfoBuffer().iDisplaySizeInPixels;

	if (twipSize.iWidth > 0 && pixelSize.iWidth > 0)
		iKPixelWidthInTwips = twipSize.iWidth * 1000 / pixelSize.iWidth;

	if (twipSize.iHeight > 0 && pixelSize.iHeight > 0)
		iKPixelHeightInTwips = twipSize.iHeight * 1000 / pixelSize.iHeight;
	
	iCacheMemMon = new(ELeave) TFontTableGlyphOutlineCacheMemMonitor;
	iUnhintedOutlineCache = new(ELeave) CUnhintedOutlineCache(iHeap, *iCacheMemMon);
	iHintedOutlineCache = new(ELeave) CHintedOutlineCache(iHeap, *iCacheMemMon);
	iFontTableCache = new(ELeave) CFontTableCache(iHeap, *iCacheMemMon);
	
	}

/** Creates a new CFontStore object.

Sets iKPixelWidthInTwips and iKPixelHeightInTwips.

Sets the default bitmap type, used when getting bitmap fonts, to EMonochromeGlyphBitmap.

Also initialises other private member variable values .

@param aHeap A pointer to the heap class used for memory allocation.
@leave KErrNoMemory Insufficient memory available on the heap. */
EXPORT_C CFontStore *CFontStore::NewL(RHeap* aHeap)
	{
	User::LeaveIfNull(aHeap);
	CFontStore *fontstore = new(ELeave) CFontStore(aHeap);
	CleanupStack::PushL(fontstore);
	fontstore->ConstructL();
	CleanupStack::Pop(fontstore);
	return fontstore;
	}

/** Destructor. */
EXPORT_C CFontStore::~CFontStore()
	{
	// Remove All font files, and Reset associated arrays
	RemoveFile(KNullUid);
	iOpenFontRasterizerList.ResetAndDestroy();
	iShaperFactoryList.ResetAndDestroy();
	if (iOpenFontSessionCacheList)
		{
		iOpenFontSessionCacheList->Delete(iHeap);
		iHeap->Free(iOpenFontSessionCacheList);
		}
	iFs.Close();
	
    delete iFontTableCache;
    delete iUnhintedOutlineCache;
    delete iHintedOutlineCache;
    delete iCacheMemMon;
	}


// local utility functions
// reads bytes from file, if the requested number of bytes are not available it leaves
void ReadFromFileL(RFile& aFile, TDes8& aDes, TInt aLength)
	{
	User::LeaveIfError(aFile.Read(aDes, aLength));
	if (aDes.Length() != aLength)
		{
		OstTrace0( TRACE_DUMP, _READFROMFILEL, "EOF reading structure from font file, Leave(KErrCorrupt)" );
		User::Leave(KErrCorrupt);
		}
	}

// local function(s) to read values from buffer & validate them
TUint TtfTableTagFromBufferL(TDes8& aDes)
	{
	TUint value = 0;
	for (TUint index = 0; index < 4;)
		{
		TUint temp = static_cast<TUint>(aDes[index++]);
		// must be ASCII character between 32 & 126 inclusive (per Apple's TTF specification document)
		if ( (temp < 32) || (temp > 126) )
			{
			OstTrace1( TRACE_DUMP, _TTFTABLETAGFROMBUFFERL, "invalid ASCII character (0x%x) in ttf table tag, Leave(KErrCorrupt)", temp);
			User::Leave(KErrCorrupt);
			}
		value = (value << 8) | temp;
		}
	return value;
	}

/* Sanity check for TrueType Font, checks that the font tables are sensible.
*/
void CFontStore::SanityCheckForTtfL(RFile& aFontFile, TUint aFontFileSize, TBool aStrictChecking)
	{
	OstTraceExt2( TRACE_DUMP, CFONTSTORE_SANITYCHECKFORTTFL, "TTF File Size is %u (0x%x) bytes", aFontFileSize, aFontFileSize );
	// check the Offset Table at the start of the file
	TBuf8<16>	fileBuffer;

	ReadFromFileL(aFontFile, fileBuffer, 12);
	TUint numTables = (fileBuffer[4] << 8) | (fileBuffer[5]);
	TUint searchRange = (fileBuffer[6] << 8) | (fileBuffer[7]);
	TUint entrySelector = (fileBuffer[8] << 8) | (fileBuffer[9]);
	TUint rangeShift = (fileBuffer[10] << 8) | (fileBuffer[11]);

	const TInt tableStart = 12 + (numTables << 4);	// lowest possible address for actual table data

	if ( (numTables == 0) || (numTables & 0xF0000000) || (tableStart > aFontFileSize) )
		{
		OstTrace1( TRACE_DUMP, DUP1_CFONTSTORE_SANITYCHECKFORTTFL, "# of tables (%d) in ttf is invalid, Leave(KErrCorrupt)", numTables );
		User::Leave(KErrCorrupt);
		}

#if defined(_DEBUG) && defined(VERBOSE_DEBUG)
	// scalar type is 0x00010000 for Windows fonts, other possible values include 0x74727565 and 0x74797031: not checked
	TUint scalarType = (fileBuffer[0] << 24) | (fileBuffer[1] << 16) | (fileBuffer[2] << 8) | (fileBuffer[3]);
	RDebug::Print(_L("ttf scalarType is 0x%x, # of tables is %u\n"), scalarType, numTables);
	RDebug::Print(_L("searchRange is 0x%x, entrySelector is 0x%x, rangeShift is 0x%x\n"), 
		searchRange, entrySelector, rangeShift);
#endif

	// check searchRange, entrySelector & rangeShift values
	// (some not quite TTF files fail the rangeShift check)
	if ( (searchRange < 16) || (entrySelector > 24)
		|| (aStrictChecking && (rangeShift != ( (numTables << 4) - searchRange )) ) )
		{
		OstTraceExt4( TRACE_DUMP, DUP2_CFONTSTORE_SANITYCHECKFORTTFL, "searchRange (0x%x), entrySelector (0x%x) or rangeShift (0x%x) invalid for numTables (%d), Leave(KErrCorrupt)", 
		        searchRange, entrySelector, rangeShift, numTables );
		User::Leave(KErrCorrupt);
		}

	// entrySelector is defined as Log2(Maximum power of 2 <= nmumTables)
	TUint exp = 1 << entrySelector;		// log to exponent
	if ( (numTables < exp) || (numTables > (exp << 1)) )
		{
		OstTraceExt2( TRACE_DUMP, DUP3_CFONTSTORE_SANITYCHECKFORTTFL, "entrySelector (0x%x) wrong for numTables(%d), Leave(KErrCorrupt)", 
		        entrySelector, numTables );
		User::Leave(KErrCorrupt);
		}

	// easy checks on the table directory
	TInt totalFontSize = tableStart;				// accumulated total directories plus table sizes
	TInt highestTableEnd = 0;						// highest value found for table start plus table length
	TUint lastTableTag = 0;							// to check that tags are in order
	TInt tableNum = 0;

	do 
		{ // each entry is 16 bytes, (though we don't check the checksum)
		ReadFromFileL(aFontFile, fileBuffer, 16);
		TUint tableTag = TtfTableTagFromBufferL(fileBuffer);
		TUint offset = (fileBuffer[8] << 24) | (fileBuffer[9] << 16) | (fileBuffer[10] << 8) | (fileBuffer[11]);
		TUint length = (fileBuffer[12] << 24) | (fileBuffer[13] << 16) | (fileBuffer[14] << 8) | (fileBuffer[15]);

#if defined(_DEBUG) && defined(VERBOSE_DEBUG)
		RDebug::Print(_L("ttf table tag ('%c%c%c%c'): offset is 0x%x, length is 0x%x\n"), 
			tableTag >> 24, (tableTag >> 16) & 0x7F, (tableTag >> 8) & 0x7F, tableTag & 0x7F, 
			offset, length);
#endif
		length = (length + 3) & ~3;	// round up, all tables must be a multiple of 4 bytes for checksumming

		// table Tags must be unique & in order
		if (tableTag <= lastTableTag)
			{
			OstTraceExt4( TRACE_DUMP, DUP4_CFONTSTORE_SANITYCHECKFORTTFL, "ttf table tag ('%c%c%c%c') is out of order, Leave(KErrCorrupt)",
			        tableTag >> 24, (tableTag >> 16) & 0x7F, (tableTag >> 8) & 0x7F, tableTag & 0x7F);
			User::Leave(KErrCorrupt);
			}


		// the offset must be 4-byte aligned, and after the table directory
		// offset + length must be bigger than the start offset (!)
		TInt end = length + offset;
		if ( (offset & 3) || (offset < tableStart) || (length == 0) || (end < offset) || (end > aFontFileSize))
			{
			OstTraceExt2( TRACE_DUMP, DUP5_CFONTSTORE_SANITYCHECKFORTTFL, "offset (0x%x) or length (0x%x) are bad, Leave(KErrCorrupt)",
			        offset, length );
			User::Leave(KErrCorrupt);
			}

		lastTableTag = tableTag;
		totalFontSize += length;
		if (end > highestTableEnd)
			{
			highestTableEnd = end;
			}
		}
		while (++tableNum < numTables);

#if defined(_DEBUG) && defined(VERBOSE_DEBUG)
	RDebug::Print(_L("last address used by any table is 0x%x, Minimum font file size to hold all tables is 0x%x\n"), 
			highestTableEnd, totalFontSize);
#endif

	// for single font files highestTableEnd & totalFontSize should be the same
	if (highestTableEnd != totalFontSize)
		{
		OstTraceExt2( TRACE_DUMP, DUP6_CFONTSTORE_SANITYCHECKFORTTFL, "Total Font Size (0x%x) is different from end of the last table (0x%x), Leave(KErrCorrupt)",
		        highestTableEnd, totalFontSize);
		User::Leave(KErrCorrupt);
		}
	}

/* Sanity checks on font files, currently only knows about TrueType Font files: .ttf and .otf
Protects FBserv / the client and the rasterizer plug-in from grossly corrupt data.
@leave KErrCorrupt if the font tables are daft
*/
void CFontStore::SanityCheckFontFileL(TParse& aParse)
	{

	// try to open the file and getting the file size
	RFile fontFile;
	const TInt openError = fontFile.Open(iFs, aParse.FullName(), EFileRead | EFileShareReadersOnly);
#ifdef _DEBUG
	if (KErrNone != openError)
		{
		const TDesC& fileName = aParse.FullName();
		RDebug::Print(_L("Sanity checking font file \"%S\", file open gave error %d\n"), &fileName, openError);
		}
#endif
	User::LeaveIfError(openError);
	CleanupClosePushL(fontFile);
	TInt fontFileSize = 0;
	User::LeaveIfError(fontFile.Size(fontFileSize));

	if (fontFileSize == 0)
		{ // no font can be zero length!
#ifdef _DEBUG
		RDebug::Print(_L("font file size (%i) is zero\n"), fontFileSize);
#endif
		User::Leave(KErrCorrupt);
		}
	else
		{
		const TDesC& fileExt = aParse.Ext();
		// TrueType fonts
		_LIT(KFntStoreTrueTypeExtension,".ttf");
		// OpenType fonts
		_LIT(KFntStoreOpenTypeExtension,".otf");
		// other font files that follow TTF format
		_LIT(KFntStoreCccFontFileExtension,".ccc");

		if ( (0 == fileExt.CompareF(KFntStoreTrueTypeExtension)) || (0 == fileExt.CompareF(KFntStoreOpenTypeExtension)) )
			{ // uses TrueType file format, perform strict format check
			SanityCheckForTtfL(fontFile, fontFileSize, ETrue);
			}
		else if (0 == fileExt.CompareF(KFntStoreCccFontFileExtension))
			{ // uses nearly TrueType file format, perform slightly less strict format check
			SanityCheckForTtfL(fontFile, fontFileSize, EFalse);
			}

		// extendible if required for other font file types ...
		}

	CleanupStack::PopAndDestroy(&fontFile);
	}


/** Finds or creates a font file object to support a font file. The specified font
file must be accessible to any process, i.e. not located inside an
application's private directory.

If an appropriate font file object exists then no new open font file is created. 
In this case the reference count of the font file object is incremented.

Notes:

If aName is recognised as an open font file, creates a COpenFontFile, which 
will manage the font file specified by aName, and adds it to the file store's 
list of open font files.

The list of open font files is used when getting the nearest font for a font 
specification (i.e. by the GetNearestFont...() functions of this class) if 
the font is generated via a rasterizer.

The appropriate rasterizer, which supports the font format of the font file, 
is used to generate the open font file using COpenFontRasterizer::NewFontFileL().

If aName is not recognised as an open font file then CFontStoreFile tries to open
the file as a Symbian Bitmap font.

@param aName The full path and filename of the font file to be supported.
@return The UID of the font file object supporting aName.
@leave KErrNotSupported if the file is not recognised at all
@leave KErrCorrupt if the font file data does not make sense
@leave or another system-wide error
*/
EXPORT_C TUid CFontStore::AddFileL(const TDesC& aName)
	{
	TParse fontFilename;
	User::LeaveIfError(iFs.Parse(aName, fontFilename));

	// increment reference count if font file is already loaded
	TUid fontUid = KNullUid;

	if (IncRefCountOfLoadedFont(fontFilename, fontUid))
		{
		return fontUid;
		}

	// Do not validate fonts on Z: (assumed to be ROM created with valid fonts)
	if (!FileIsOnZ(fontFilename))
		{
		// file is not open - check that the file exists and has a plausible content
		SanityCheckFontFileL(fontFilename);
		}
	
	AddSanityCheckedFontL(fontFilename, fontUid);
	
	return fontUid;
	}

/**
Finds or creates a font file object to support a font file.
@param aFileName The filename to check
@param aUid The list to check
@see CFontStore::AddFileL
*/
void CFontStore::AddSanityCheckedFontL(const TParse& aFileName, TUid& aUid)
	{
	// Try loading the file as an Open Font
	if (!LoadFileAsOpenFontL(aFileName, aUid))
		{
		// It could not be recognised as an Open Font file so load it as an EPOC bitmap font file.
		// (This always succeeds or Leaves.)
		aUid = LoadFileAsBitmapFontL(aFileName);
		}
	}


// Remove a Font File - called via the Cleanup Stack if a leave occurs during font install
void CFontStore::CleanupRemoveFontFile(TAny* aCleanupInfo)
	{
	TCleanupRemoveFontFile* cleanupInfo = (TCleanupRemoveFontFile *) aCleanupInfo;
	cleanupInfo->iFontStore->RemoveFile(cleanupInfo->iFontUid);
	}


/** Search through the rasterizers for one that will open the font filename.

Can leave if the rasterizer encounters an error, such as unexpected end of file, nonsense data, no memory.

@param aName full filename to open
@param aFontUid set to the font file UID if it is already loaded
@return ETrue if the font file is loaded, otherwise EFalse
@internalComponent
*/
TBool CFontStore::LoadFileAsOpenFontL(const TParse& aFileName, TUid& aFontUid)
	{
	const TDesC& fullName = aFileName.FullName();
	
	// ask each of the rasterizers in turn to create a COpenFontFile object.
	const TInt rasterizerCount = iOpenFontRasterizerList.Count();
	for (TInt index = 0; index < rasterizerCount; index++)
		{
		COpenFontFile* openFontFile = iOpenFontRasterizerList[index]->NewFontFileL(iOpenFontUid, fullName, iFs);
		if (NULL != openFontFile)
			{
			/* Uids for Open Fonts are allocated in order; the
			one for the COpenFontFile just created won't match any existing one.
			(When a file is removed, the fake Uid will match, because the one that was returned here is used.)
			*/
			CleanupStack::PushL(openFontFile);
			openFontFile->SetFontStoreL(this);
			iOpenFontFileList.AppendL(openFontFile);
			openFontFile->IncRefCount();
			CleanupStack::Pop(openFontFile);

			TInt typefaceAddError = AddTypefacesToSupportList(openFontFile);
			if (typefaceAddError)
				{
				// cleanup
				RemoveFile(openFontFile->Uid());
				// throw the error
				User::Leave(typefaceAddError);
				}

			/*
			iOpenFontUid is the fake Uid used for new font files. It is made modulo a 'zillion' (2^28)
			so that it cannot conflict with real Uids, which are >= 2^28. It starts at 1 so that
			it does not conflict with the value 0 (KNullUid) which is used as a sentinel in
			CFontStore::RemoveFileL.
			*/
			++iOpenFontUid;
			iOpenFontUid = iOpenFontUid % 0x1000000;

			aFontUid = openFontFile->Uid();
			return ETrue;
			}
		}
	return EFalse;
	}


/** Search loaded fonts for filename, and increment the reference count
@param aParse full filename to search for
@return the font file UID
@leave KErrNotSupported or another system wide error code
@internalComponent
*/
TUid CFontStore::LoadFileAsBitmapFontL(const TParse& aParse)
	{
	// open the file, presuming it is a bitmap font
	CFontStoreFile* fontStoreFile = CFontStoreFile::NewL(aParse, iFs);

	// See if a font with this UID is already open
	TInt match = FindBitmapFontFileIndexByUid(fontStoreFile->iCollectionUid);
	if (match != KErrNotFound) // The file is open.
		{ // close the new duplicate, inc the reference count
		delete fontStoreFile;
		fontStoreFile = iFontStoreFileList[match];
		fontStoreFile->iUsageCount++;
		}
	else
		{
		CleanupStack::PushL(fontStoreFile);
		iFontStoreFileList.AppendL(fontStoreFile);
		CleanupStack::Pop(fontStoreFile);

		// special object to removes the font file if a leave occurs during font install
		TCleanupRemoveFontFile cleanupHelper(this, fontStoreFile->iCollectionUid);
		CleanupStack::PushL(TCleanupItem(CleanupRemoveFontFile, &cleanupHelper));
		// install the new font
		InternalizeFontStoreFileL(fontStoreFile, fontStoreFile->iFontVersion);
		CleanupStack::Pop(&cleanupHelper); // CleanupRemoveFontFile
		}
	return fontStoreFile->iCollectionUid;
	}


/** Search open Bitmap font files for matching UID
@param aUid UID to search for
@return The index of the matching item in iFontStoreFileList[] array, or KErrNotFound
@internalComponent
*/
TInt CFontStore::FindBitmapFontFileIndexByUid(TUid aUid)
	{
	const TInt fontFileCount = iFontStoreFileList.Count();
	for (TInt index = 0; index < fontFileCount; index++)
		{
		if (iFontStoreFileList[index]->iCollectionUid == aUid)
			{
			return index;
			}
		}
	return KErrNotFound;
	}


/** Search loaded fonts for filename, and increment the reference count
@param aName full filename to search for
@param aFontUid set to the font file UID if it is already loaded
@return ETrue if the font file is already loaded, otherwise EFalse
@internalComponent
*/
TBool CFontStore::IncRefCountOfLoadedFont(const TParse& aFileName, TUid& aFontUid)
	{
	const TDesC& fullName = aFileName.FullName();
	
	// Is it already in the Open Font list? Compare by full filename, not (fake) Uid;
	const TInt openFontCount = iOpenFontFileList.Count();
	TInt i = 0;
	for (i = 0; i < openFontCount; ++i)
		{
		COpenFontFile* openFontFile = iOpenFontFileList[i];
		if (fullName.CompareF(openFontFile->FileName()) == 0)
			{
			// Open Font file already loaded
			openFontFile->IncRefCount();
			aFontUid = openFontFile->Uid();
			return ETrue;
			}
		}

	// already open as an EPOC bitmap font file?
	const TInt fontFileCount = iFontStoreFileList.Count();
	for (i = 0; i < fontFileCount; i++)
		{
		CFontStoreFile* fontStoreFile = iFontStoreFileList[i];
		if (fullName.CompareF(fontStoreFile->FullName()) == 0)
			{
			// font file already loaded
			++(fontStoreFile->iUsageCount);
			aFontUid = fontStoreFile->iCollectionUid;
			return ETrue;
			}
		}
	return EFalse;
	}


/** Releases a hold on one or all font file objects (COpenFontFiles or CFontStoreFiles).

If aFileUid identifies a font file object, then the reference count for this 
object is decremented. If this brings the reference count down to zero then 
the font file object and typefaces associated with this file are removed from
the font store, provided no fonts associated with this file are being accessed. 
If one or more fonts are being accessed then the file removal request will be ignored.

If, on the other hand, aFileUid's value is NULL, then all font file objects 
are removed, along with all fonts and typefaces in the font store, provided
that no fonts in the font store are being accessed (i.e. iFontAccess is empty),
otherwise it has no effect.

(If aFileUid is invalid, then no objects are removed.)

@param aFileUid UID of a hold on a font file object to be released, or KNullUid
if all font file objects are to be removed.
*/
EXPORT_C void CFontStore::RemoveFile(TUid aFileUid)
	{
	TInt i, count;
	if (aFileUid == KNullUid)	// this means 'delete all files'
		{
		if(iFontAccess)
			{
			TInt count = iFontAccess->Count();		
			for (TInt i = 0; i < count; i++)
				if((*iFontAccess)[i].iAccessCount)
					return;
			}
		if (iFontTableCache && iFontTableCache->HasOutstandingRefCount())
		    {
		    // disallow font file removal if any font tables are still cached
		    return;
		    }
		
		iTypefaceList.ResetAndDestroy();
		count = iFontBitmapList.Count();
		for (TInt i = 0; i < count; i++)
			iFontBitmapList[i]->Release();
		iFontBitmapList.Reset();
		iTypefaceFontBitmapList.Reset();
		iFontStoreFileList.ResetAndDestroy();
		iOpenFontFileList.ResetAndDestroy();
		iOpenFontTypefaceSupportList.ResetAndDestroy();
		}
	else
		{
		// See if it's an Open Font file.
		count = iOpenFontFileList.Count();
		for (i = 0; i < count; i++)
			{
			TInt fontAccessCount;
			if (iOpenFontFileList[i]->Uid() == aFileUid)
				{ // found the file
				if (iFontAccess)
					{ // Check to see if there are any references before we consider deleting
					fontAccessCount = iFontAccess->Count();
					for (TInt kk = 0; kk < fontAccessCount; ++kk)
						{
		  	  	  		CBitmapFont* bitmapFont = reinterpret_cast<CBitmapFont*>((*iFontAccess)[kk].iFont);
		  	  	  		COpenFont* openFont = bitmapFont->OpenFont();
		  	  	  		if (openFont && openFont->File()->Uid() == aFileUid )
		  	  	  			{
		  	  	  			if ((*iFontAccess)[kk].iAccessCount > 0) 
		  	  	  				return; // Outstanding reference found, so ignoring file removal request
		  	  	  			}
						}
					} // Safe to proceed with removing file
				// also check if there are outstanding references to any 
				// table in that file.
				if (iFontTableCache && iFontTableCache->HasOutstandingRefCountWithUid(aFileUid))
				    {
				    return; // outstanding reference to font table found.
				    }
				
				// Safe to proceed with removing file
				if (iOpenFontFileList[i]->DecRefCount())
					{ // unload the font file
					RemoveTypefacesFromSupportList(iOpenFontFileList[i]);
					//linked fonts deleted from this destructor
					delete iOpenFontFileList[i];
					iOpenFontFileList.Delete(i);
					}
				return;
				}
			}

		// Finds first fontstorefile with correct id
		TInt index = FindBitmapFontFileIndexByUid(aFileUid);
		if (index != KErrNotFound)  //  Checks fontstore file has been found
			{
			iFontStoreFileList[index]->iUsageCount--;
			if (!iFontStoreFileList[index]->iUsageCount)
				{
				TInt tfcount = iTypefaceFontBitmapList.Count();
				for (i=tfcount-1; i>=0; i--)
					if (aFileUid == iTypefaceFontBitmapList[i].iFontBitmap->FontStoreFile()->iCollectionUid)
						iTypefaceFontBitmapList.Delete(i);
				count = iFontBitmapList.Count();
				for (i=count-1; i>=0; i--)
					{
					
					if (aFileUid==iFontBitmapList[i]->FontStoreFile()->iCollectionUid)
						{
						iFontBitmapList[i]->Release();
						iFontBitmapList.Delete(i);
						}  
					}
				count = iTypefaceList.Count();
				for (i=count-1; i>=0; i--)	
					{
					TBool inlist=EFalse;  
					tfcount = iTypefaceFontBitmapList.Count();
					for (TInt j=0; j<tfcount; j++)
						if (iTypefaceList[i] == iTypefaceFontBitmapList[j].iTypeface)
							inlist = ETrue;
					if (!inlist)
						{
						delete iTypefaceList[i];
						iTypefaceList.Delete(i);
						}
					}
				delete iFontStoreFileList[index];
				iFontStoreFileList.Delete(index);
				}
			}
		}
	}

/**
@publishedAll
*/
EXPORT_C TInt CFontStore::GetNearestFontInTwips(CFont*& aFont, const TFontSpec& aFontSpec)
	{
	return GetNearestFontToDesignHeightInTwips(aFont, aFontSpec);
	}

/**
Gets the font which is the nearest to the given font specification.

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
EXPORT_C TInt CFontStore::GetNearestFontInTwips(CFont*& aFont, const TOpenFontSpec& aFontSpec)
	{
	return GetNearestFontToDesignHeightInTwips(aFont, aFontSpec);
	}

/**
@publishedAll
*/
EXPORT_C TInt CFontStore::GetNearestFontToDesignHeightInTwips(CFont*& aFont, const TFontSpec& aFontSpec)
	{
	TOpenFontSpec spec(aFontSpec);
	spec.SetHeight(VerticalTwipsToPixels(spec.Height()));
	return GetNearestFontToDesignHeightInPixels(aFont, spec);
	}

/**
Gets the font which is the nearest to the given font specification.

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
EXPORT_C TInt CFontStore::GetNearestFontToDesignHeightInTwips(CFont*& aFont, const TOpenFontSpec& aFontSpec)
	{
	TOpenFontSpec spec = aFontSpec;
	spec.SetHeight(VerticalTwipsToPixels(spec.Height()));
	return GetNearestFontToDesignHeightInPixels(aFont, spec);
	}

/**
@publishedAll
*/
EXPORT_C TInt CFontStore::GetNearestFontToMaxHeightInTwips(CFont*& aFont, const TFontSpec& aFontSpec, TInt aMaxHeight)
	{
	TOpenFontSpec spec(aFontSpec);
	spec.SetHeight(VerticalTwipsToPixels(spec.Height()));
	return GetNearestFontToMaxHeightInPixels(aFont, spec, VerticalTwipsToPixels(aMaxHeight));
	}

/**
Gets the font which is the nearest to the given font specification.

The font and bitmap server returns a pointer to the nearest matching font 
from those available. Matches to max height of font - this does its best 
to return a font that will fit within the maximum height specified (but 
note that variations due to hinting algorithms may rarely result in this 
height being exceeded by up to one pixel). Problems can also be 
encountered with bitmap fonts where the typeface exists but doesn't have 
a font small enough.

@param aFont On return, contains a pointer to the nearest font.
@param aFontSpec The specification of the font to be matched.
@param aMaxHeight The maximum height within which the font must fit. If maximum height
is greater than 1024 pixels, the function returns KErrTooBig. And returns KErrArgument
if equals to 1 pixel. This overrides the height specified in aFontSpec.
@return KErrNone if successful; a system-wide error code otherwise.
@publishedAll
@released
*/
EXPORT_C TInt CFontStore::GetNearestFontToMaxHeightInTwips(CFont*& aFont, const TOpenFontSpec& aFontSpec, TInt aMaxHeight)
	{
	TOpenFontSpec spec = aFontSpec;
	spec.SetHeight(VerticalTwipsToPixels(spec.Height()));
	return GetNearestFontToMaxHeightInPixels(aFont, spec, VerticalTwipsToPixels(aMaxHeight));
	}


/** Local utility method to compare two font specs; the higher the return value, the better the match.
It is not clear how to reliably test for symbol fonts or language coverage because some
TOpenFontSpecs are created from bitmap TFontSpecs that do not have this info
*/
TInt MatchFontSpecsInPixels(
	const TOpenFontSpec&	aCandidateFontSpec,
	const TOpenFontSpec&	aIdealFontSpec,
	TInt					aCandidateMaxHeight,
	TInt					aIdealMaxHeight)
	{
	// scores for matching (or similar) font attributes:
	const TInt KFontMatchScoreForName = 10;
	// favour fonts with heights closer together 
	const TInt KFontMaxScoreForHeight = 10;
	// A match on both italic-ness and presence of forward slant factor is worth 2 points.
	const TInt KFontMatchScoreForExactItalicAndSlant = 2;
	// A match between italic-ness in one font and presence of forward slant factor in the other is worth 1 point.
	const TInt KFontSimilarScoreForItalicAndSlant = 1;
	const TInt KFontMatchScoreForBold = 2;
	const TInt KFontMatchScoreForBitmapType = 2;
	// A match between outline or shadow effect in one font with other is worth 2 points. Its same as matching for bitmaptype.
	const TInt KFontMatchScoreForOutlineOrShadow = 2;
	// The monospaced/proportional status is rated higher at 3 because it has a major effect on the look and layout
	const TInt KFontMatchScoreForMonoSpace = 3;
	// Whether the font is serifed or not has a smaller effect on the look and layout so it is only rated at 1
	const TInt KFontMatchScoreForSerif = 1;

	TInt score = 0;

	// Favour perfect name and height matches.	
	if (0 == aCandidateFontSpec.Name().CompareF(aIdealFontSpec.Name()))
		{
		score += KFontMatchScoreForName;
		}

	// Check for max height if given, otherwise height in font spec
	TInt height_diff = 0;
	if (aCandidateMaxHeight && aIdealMaxHeight)
		{
		if (aIdealMaxHeight < aCandidateMaxHeight)
			return 0;
		height_diff = aIdealMaxHeight - aCandidateMaxHeight;
		}
	else
 		{
		height_diff = aIdealFontSpec.Height() - aCandidateFontSpec.Height();
		if (0 > height_diff)
			height_diff = -height_diff;
		}
	score += KFontMaxScoreForHeight - height_diff;

	if (aCandidateFontSpec.IsItalic() == aIdealFontSpec.IsItalic() &&
		(0 < aCandidateFontSpec.SlantFactor()) == (0 < aIdealFontSpec.SlantFactor()))
		{
		score += KFontMatchScoreForExactItalicAndSlant;
		}
	else if ((aCandidateFontSpec.IsItalic() && 0 < aIdealFontSpec.SlantFactor()) ||
			 (aIdealFontSpec.IsItalic() && 0 < aCandidateFontSpec.SlantFactor()))
		{
		score += KFontSimilarScoreForItalicAndSlant;
		}

	if (aCandidateFontSpec.IsBold() == aIdealFontSpec.IsBold())
		{
		score += KFontMatchScoreForBold;
		}
	
	TUint effectsFlag = aIdealFontSpec.Effects() & aCandidateFontSpec.Effects();
	TBool isShadowOutlineEffects = EFalse;

	if(effectsFlag & FontEffect::EOutline)
		{
		score += KFontMatchScoreForOutlineOrShadow;
		isShadowOutlineEffects = ETrue;
		}
	if(effectsFlag & FontEffect::EDropShadow)
		{
		score += KFontMatchScoreForOutlineOrShadow;
		isShadowOutlineEffects = ETrue;
		}
		
	//Match for bitmap glyph type only when effects are off.
	//BitmapType in aIdealFontSpec will be set to EFourColourBlendGlyphBitmap by rasterizer only
	//when any of the effects (outline/shadow) are on and if it supports outline and shadow fonts.
	if(!isShadowOutlineEffects && (aCandidateFontSpec.BitmapType() == aIdealFontSpec.BitmapType()))
		{
		score += KFontMatchScoreForBitmapType;
		}
	if (aCandidateFontSpec.IsMonoWidth() == aIdealFontSpec.IsMonoWidth())
		{
		score += KFontMatchScoreForMonoSpace;
		}
	if (aCandidateFontSpec.IsSerif() == aIdealFontSpec.IsSerif())
		{
		score += KFontMatchScoreForSerif;
		}

	return score;
	}


/**
Find and load the nearest Open Font by
1. using aDesiredFontSpec to check if its already loaded,
2. using aActualFontSpec to partially match	and create a new font.
Partial match means typeface name must match, other attributes need not match.
Font must fit vertically within the specified max height if given.

@param aFont Output, is the returned font.
@param aActualFontSpec Output, is used to represent the returned font and changed to twips. 
@param aDesiredFontSpec Input, the desired font specification
@param aMaxHeight Input, the maximum height in Pixels, or 0
@post if any Open Fonts are available this will always return something
*/
void CFontStore::GetNearestOpenFontInPixelsL(
	CFont*&					aFont,
	TOpenFontSpec&			aActualFontSpec,
	const TOpenFontSpec&	aDesiredFontSpec,
	TInt					aMaxHeight)
	{
	// Set the baseline offset appropriately for superscript or subscript.
	TAlgStyle algstyle;
	algstyle.iBaselineOffsetInPixels = BaselineOffset(aActualFontSpec.Height(), aActualFontSpec.PrintPosition());

	// Determine if an open font with a perfect match is already loaded.
	if (IsFontLoaded(aFont, algstyle, aDesiredFontSpec, aMaxHeight))
		{
		aActualFontSpec = aDesiredFontSpec;
		}
	else
		{ // If not, find the nearest match for aActualFontSpec.
		COpenFont* nearestFont = NULL;

		// fast search, if DO_LOADFONT_OPTIMIZATION is defined, search loaded font names for match
		TInt errorName=GetNearestOpenFontInPixelsByFontName(nearestFont, aActualFontSpec, aDesiredFontSpec, aMaxHeight);
		TInt errorSimilar=KErrNone;

		if (!nearestFont)
			{
			errorSimilar=GetNearestOpenFontInPixelsBySimilarity(nearestFont, aActualFontSpec, aDesiredFontSpec, aMaxHeight);
			}

		// If an imperfect match was found, is it already loaded? If not, create a new CBitmapFont.
		if (nearestFont)
			{
			//
			// Results of everything above are stored in nearestFont, and aActualFontSpec
			// First check if the not-exactly-matched font is already loaded
			// aMaxHeight is 0 to prevent duplicate CBitmapFontObjects being created
			//
			if (IsFontLoaded(aFont, algstyle, aActualFontSpec, 0))
				{
				delete nearestFont;
				}
			else
				{
				// Set the specification of the nearest match in twips.
				TOpenFontSpec openFontSpec(aActualFontSpec);
				openFontSpec.SetHeight(VerticalPixelsToTwips(openFontSpec.Height()));
				CleanupStack::PushL(nearestFont);
				aFont = NewFontL(openFontSpec, algstyle, nearestFont);
				CleanupStack::Pop(nearestFont);
				}
			}
		else
			{
			User::LeaveIfError(errorSimilar);
			User::LeaveIfError(errorName);
			}
		}
	}


/** Using the Desired Font Name try to find a matching font.

@param aNearestOpenFont Output, nearest matching open font
@param aActualFontSpec Output, is used to represent the returned font 
@param aDesiredFontSpec Input, the desired font specification
@param aMaxHeight Input, the maximum height in Pixels, or 0
*/
#if(DO_LOADFONT_OPTIMIZATION)
TInt CFontStore::GetNearestOpenFontInPixelsByFontName(
	COpenFont*&				aOpenFont,
	TOpenFontSpec&			aActualFontSpec,
	const TOpenFontSpec&	aDesiredFontSpec,
	TInt					aMaxHeight)
	{
	TOpenFontSpec actualFontSpec;
	COpenFont* nearestFont = NULL;

	if (aMaxHeight == 0 && aDesiredFontSpec.Name().Length() > 0)	// check if it is a request for OpenFont
		{
		TPtrC requestedName = aDesiredFontSpec.Name();
		// Typeface files may contain all those typefaces in a family, or may contain only some.
		// The optimization first tries the ShortFullName of the typeface in case the typefaces are
		// put into separate files.
		
		// First check FullName. The following assumes that there are never two COpenFontFiles in the 
		// list with the same ShortFullName. So italic and bold flags are not checked.

		const TInt openFontFileCount = iOpenFontFileList.Count();
		TInt index;
		TInt typicalError = KErrNone;
		for (index = 0; index < openFontFileCount; index++)
			{
			COpenFontFile *pFile = iOpenFontFileList[index]; 
			const TOpenFontFaceAttrib &faceAttrib = pFile->FaceAttrib(0);
			
			// Use short names. They are truncated to the same length as the request in TTypeface
			if ((faceAttrib.ShortFullName().CompareF(requestedName)==0) &&
				(faceAttrib.IsBold() == aActualFontSpec.IsBold()) &&
				(faceAttrib.IsItalic() == aActualFontSpec.IsItalic()) )			
				{
				TInt error=pFile->GetNearestFontToDesignHeightInPixels(
					iHeap, iOpenFontSessionCacheList, aActualFontSpec, iKPixelWidthInTwips,
					iKPixelHeightInTwips, nearestFont, actualFontSpec);				

				// search has finished if a match was found
				if (nearestFont)
					{
					// need to copy the actual to the value that is going to be used down below
					// to make a new font
					aOpenFont = nearestFont;
					aActualFontSpec = actualFontSpec;
					return KErrNone;
					}
				else
					{
					if (error)
						{
						typicalError=error;
						}
					}
				}
			}

		// If the FullName match above has failed, then the request is being made by
		// something other than a full name (assuming typeface is present).
		// Although there may be more than one typeface in a file,
		// It is not possible to conclude that ALL typefaces of the family are in one file.
		// So to be safe, checks are made on italic and bold, possibly downgrading the optimization.
		// Use short names. They are truncated to the same length as the request in TTypeface
		for (index = 0; index < openFontFileCount; index++)
			{
			COpenFontFile *pFile = iOpenFontFileList[index]; 
			const TOpenFontFaceAttrib &faceAttrib = pFile->FaceAttrib(0);

			if(	(faceAttrib.ShortFamilyName().CompareF(requestedName) == 0) &&
				(faceAttrib.IsItalic() == aDesiredFontSpec.IsItalic()) &&
				(faceAttrib.IsBold() == aDesiredFontSpec.IsBold()) )	
				{
				TInt error=pFile->GetNearestFontToDesignHeightInPixels(
					iHeap, iOpenFontSessionCacheList, aActualFontSpec, iKPixelWidthInTwips,
					iKPixelHeightInTwips, nearestFont, actualFontSpec);

				// search has finished if a match was found
				if (nearestFont)
					{
					// need to copy the actual to the value that is going to be used down below
					// to make a new font
					aOpenFont = nearestFont;
					aActualFontSpec = actualFontSpec;
					return KErrNone;
					}
				else
					{
					if (error)
						{
						typicalError=error;
						}
					}
				}
			}
		// no match
		return typicalError;
		}
	// not open request
	return KErrNone;
	}
#else
TInt CFontStore::GetNearestOpenFontInPixelsByFontName(
	COpenFont*&				/* aOpenFont */,
	TOpenFontSpec&			/* aActualFontSpec */,
	const TOpenFontSpec&	/* aDesiredFontSpec */,
	TInt					/* aMaxHeight*/)
	{ // no match
	return KErrNone;
	}
#endif


/** Using the Desired Font Name try to find a matching font.

@param aNearestOpenFont Output, nearest matching open font
@param aActualFontSpec Output, is used to represent the returned font 
@param aDesiredFontSpec Input, the desired font specification
@param aMaxHeight Input, the maximum height in Pixels, or 0
*/
TInt CFontStore::GetNearestOpenFontInPixelsBySimilarity(
	COpenFont*&				aNearestOpenFont,
	TOpenFontSpec&			aActualFontSpec,
	const TOpenFontSpec&	aDesiredFontSpec,
	TInt					aMaxHeight)
	{
	const TInt openFontFileCount = iOpenFontFileList.Count();
	TOpenFontSpec nearestFontSpec;
	COpenFont* nearestFont = NULL;
	TInt nearestMatch = 0;
    TInt typicalError=KErrNone;
	for (TInt index = 0; index < openFontFileCount; index++)
		{
		// ask font file for its best match, if any
		COpenFont* candidateFont = NULL;
		TOpenFontSpec actualFontSpec;
		TInt lastError=KErrNone;
		if (aMaxHeight)
			{
			lastError=iOpenFontFileList[index]->GetNearestFontToMaxHeightInPixels(
				iHeap, iOpenFontSessionCacheList, aActualFontSpec, iKPixelWidthInTwips,
				iKPixelHeightInTwips, candidateFont, actualFontSpec, aMaxHeight);
			}
		else
			{
			lastError=iOpenFontFileList[index]->GetNearestFontToDesignHeightInPixels(
				iHeap, iOpenFontSessionCacheList, aActualFontSpec, iKPixelWidthInTwips,
				iKPixelHeightInTwips, candidateFont, actualFontSpec);
			}

		// hold on to the best overall match
		if (candidateFont)
			{
			const TInt candidateMatch = MatchFontSpecsInPixels(
				actualFontSpec, aDesiredFontSpec, candidateFont->FontMaxHeight(), aMaxHeight);

			if (NULL == nearestFont || candidateMatch > nearestMatch)
				{
				if (nearestFont)
					{
					delete nearestFont;
					}
				nearestFont = candidateFont;
				nearestMatch = candidateMatch;
				nearestFontSpec	= actualFontSpec;
				}
			else
				{
				// Font match is no better than the current nearestFont
				//
				// Note this object is newed in GetNearestFontInPixels each time 
				// a matching font is found.
				delete candidateFont;
				}
			}
		else
			{
			if (lastError)
				{
				typicalError=lastError;
				}
			}
		}

	if (nearestFont != NULL)
		{ // result
		aNearestOpenFont = nearestFont;
		aActualFontSpec = nearestFontSpec;
		return KErrNone;
		}
	else
		{
		return typicalError;
		}
	}


/**
Get the nearest bitmap font to aFontSpec and place it in aFont. Font must 
fit vertically inside specified maximum height if given. GetNearestTypeface is
called which insists that there is at least one typeface. This function will always 
succeed unless there is an out-of-memory error or other abnormal error. 
Change aFontSpec to represent the returned font and change it to twips.
*/
void CFontStore::GetNearestBitmapFontInPixelsL(
	CFont*&		aFont,
	TFontSpec&	aFontSpec,
	TInt		aMaxHeight)
	{
	aFontSpec.iTypeface = *GetNearestTypeface(aFontSpec.iTypeface);
	TTypefaceFontBitmap tffb = GetNearestTypefaceFontBitmap(aFontSpec, aMaxHeight);
	const TInt height = tffb.HeightInPixels();
	if (aFontSpec.iFontStyle.PrintPosition() != EPrintPosNormal)
		{
		aFontSpec.iHeight = SuperSubHeight(height, aFontSpec.iFontStyle.PrintPosition());
		tffb = GetNearestTypefaceFontBitmap(aFontSpec, aMaxHeight);
		}
	aFontSpec.iHeight = height;
	TAlgStyle algstyle;
	algstyle.SetIsBold(
		EStrokeWeightBold == aFontSpec.iFontStyle.StrokeWeight() &&
		EStrokeWeightNormal == tffb.iFontBitmap->StrokeWeight());
	algstyle.SetIsItalic(
		EPostureItalic == aFontSpec.iFontStyle.Posture() &&
		EPostureUpright == tffb.iFontBitmap->Posture());
	algstyle.SetIsMono(!aFontSpec.iTypeface.IsProportional() && tffb.iFontBitmap->IsProportional());
	TInt widthfactor =
		((tffb.iWidthFactor * tffb.iFontBitmap->FontStoreFile()->iKPixelAspectRatio * iKPixelWidthInTwips) + (500 * iKPixelHeightInTwips)) / (1000 * iKPixelHeightInTwips);
	if (!widthfactor)
		widthfactor = 1;
	algstyle.SetWidthFactor(widthfactor);
	algstyle.SetHeightFactor( tffb.iHeightFactor );
	algstyle.iBaselineOffsetInPixels = BaselineOffset(height, aFontSpec.iFontStyle.PrintPosition());
	if (IsFontLoaded(aFont, algstyle, aFontSpec, tffb.iFontBitmap->iUid))
		return;
	TFontSpec spec(aFontSpec);
	spec.iHeight = VerticalPixelsToTwips(height);
	spec.iFontStyle.SetBitmapType(EMonochromeGlyphBitmap);
	aFont = NewFontL(spec, algstyle, tffb.iFontBitmap);
	}

/**
Gets the font which is the nearest to the given font specification.

Note that this deprecated function is replaced by the new @c GetNearestFontToDesignHeightInPixels() 
yielding (virtually) the same result. However clients are strongly encouraged to use the new
@c GetNearestFontToMaxHeightInPixels() function instead. This will guarantee that every 
character within any given text string will fit within the given amount of pixels, whereas the design 
height is an aesthetic unit decided by the font designer without strict physical meaning, which 
may result in cropped characters.

@param aFont On return, contains a pointer to the nearest font.
@param aFontSpec The specification of the font to be matched.
@return KErrNone if successful; a system-wide error code otherwise.
@publishedAll
@deprecated Use GetNearestFontToDesignHeightInPixels
*/
EXPORT_C TInt CFontStore::GetNearestFontInPixels(CFont*& aFont, const TFontSpec& aFontSpec)
	{
	return GetNearestFontToDesignHeightInPixels(aFont, aFontSpec);
	}

/**
Gets the font which is the nearest to the given font specification.

Note that this deprecated function is replaced by the new @c GetNearestFontToDesignHeightInPixels() 
yielding (virtually) the same result. However clients are strongly encouraged to use the new
@c GetNearestFontToMaxHeightInPixels() function instead. This will guarantee that every 
character within any given text string will fit within the given amount of pixels, whereas the design 
height is an aesthetic unit decided by the font designer without strict physical meaning, which 
may result in cropped characters.

@param aFont On return, contains a pointer to the nearest font.
@param aFontSpec The specification of the font to be matched.
@return KErrNone if successful; a system-wide error code otherwise.
@publishedAll
@deprecated Use GetNearestFontToDesignHeightInPixels
*/
EXPORT_C TInt CFontStore::GetNearestFontInPixels(CFont*& aFont, const TOpenFontSpec& aFontSpec)
	{
	return GetNearestFontToDesignHeightInPixels(aFont, aFontSpec);
	}

/**
Gets the font which is the nearest to the given font specification.

This new function replaces the deprecated @c GetNearestFontInPixels() yielding (virtually) the 
same result. However clients are strongly encouraged to use the new
@c GetNearestFontToMaxHeightInPixels() function instead. This will guarantee that every 
character within any given text string will fit within the given amount of pixels, whereas the design 
height is an aesthetic unit decided by the font designer without strict physical meaning, which 
may result in cropped characters.

@param aFont On return, contains a pointer to the nearest font.
@param aFontSpec The specification of the font to be matched.
@return KErrNone if successful; a system-wide error code otherwise.
@publishedAll
@released
*/
EXPORT_C TInt CFontStore::GetNearestFontToDesignHeightInPixels(CFont*& aFont, const TFontSpec& aFontSpec)
	{
	TOpenFontSpec spec(aFontSpec);
	return GetNearestFontToDesignHeightInPixels(aFont, spec);
	}

/**
Gets the font which is the nearest to the given font specification.

This new function replaces the deprecated @c GetNearestFontInPixels() yielding (virtually) the 
same result. However clients are strongly encouraged to use the new
@c GetNearestFontToMaxHeightInPixels() function instead. This will guarantee that every 
character within any given text string will fit within the given amount of pixels, whereas the design 
height is an aesthetic unit decided by the font designer without strict physical meaning, which 
may result in cropped characters.

@param aFont On return, contains a pointer to the nearest font.
@param aFontSpec The specification of the font to be matched.
@return KErrNone if successful; a system-wide error code otherwise.
@publishedAll
@released
*/
EXPORT_C TInt CFontStore::GetNearestFontToDesignHeightInPixels(CFont*& aFont, const TOpenFontSpec& aFontSpec)
	{
	return GetNearestFontInPixels(aFont, aFontSpec, 0);
	}

/**
Gets the font which is the nearest to the given font specification.

The font and bitmap server returns a pointer to the nearest matching font 
from those available. Matches to max height of font - this does its best 
to return a font that will fit within the maximum height specified (but 
note that variations due to hinting algorithms may rarely result in this 
height being exceeded by up to one pixel). Problems can also be 
encountered with bitmap fonts where the typeface exists but doesn't have 
a font small enough.

@param aFont On return, contains a pointer to the nearest font.
@param aFontSpec The specification of the font to be matched.
@param aMaxHeight The maximum height within which the font must fit. If maximum height
is greater than 1024 pixels, the function returns KErrTooBig. And returns KErrArgument
if equals to 1 pixel. This overrides the height specified in aFontSpec.
@return KErrNone if successful; a system-wide error code otherwise.
@publishedAll
@released
*/
EXPORT_C TInt CFontStore::GetNearestFontToMaxHeightInPixels(CFont*& aFont, const TFontSpec& aFontSpec, TInt aMaxHeight)
	{
	TOpenFontSpec spec(aFontSpec);
	return GetNearestFontToMaxHeightInPixels(aFont, spec, aMaxHeight);
	}

/**
Gets the font which is the nearest to the given font specification.

The font and bitmap server returns a pointer to the nearest matching font 
from those available. Matches to max height of font - this does its best 
to return a font that will fit within the maximum height specified (but 
note that variations due to hinting algorithms may rarely result in this 
height being exceeded by up to one pixel). Problems can also be 
encountered with bitmap fonts where the typeface exists but doesn't have 
a font small enough.

@param aFont On return, contains a pointer to the nearest font.
@param aFontSpec The specification of the font to be matched.
@param aMaxHeight The maximum height within which the font must fit. If maximum height
is greater than 1024 pixels, the function returns KErrTooBig. And returns KErrArgument
if equals to 1 pixel. This overrides the height specified in aFontSpec.
@return KErrNone if successful; a system-wide error code otherwise.
@publishedAll
@released
*/
EXPORT_C TInt CFontStore::GetNearestFontToMaxHeightInPixels(CFont*& aFont, const TOpenFontSpec& aFontSpec, TInt aMaxHeight)
	{
	if (KMaxFontHeightInPixels < aMaxHeight)
		{
		return KErrTooBig;
		}
	if (aMaxHeight > 0 && aMaxHeight < KMinFontHeightInPixels)
		{
		return KErrArgument;
		}
	return GetNearestFontInPixels(aFont, aFontSpec, aMaxHeight);
	}

TInt CFontStore::GetNearestFontInPixels(
	CFont*&					aFont,
	const TOpenFontSpec&	aDesiredFontSpec,
	TInt					aMaxHeight)
	{
	aFont = NULL;

	// Determine the ideal version of the font spec.
	TOpenFontSpec originalFontSpec = aDesiredFontSpec;
	originalFontSpec.CompensateForAspectRatio(iKPixelWidthInTwips, iKPixelHeightInTwips);
	TOpenFontSpec desiredFontSpec = originalFontSpec;
	if (EDefaultGlyphBitmap == desiredFontSpec.BitmapType())
		{
		desiredFontSpec.SetBitmapType(iDefaultBitmapType);
		}
	if (EPrintPosNormal != desiredFontSpec.PrintPosition())
		{
		desiredFontSpec.SetHeight(SuperSubHeight(desiredFontSpec.Height(), desiredFontSpec.PrintPosition()));
		}
	TOpenFontSpec actualOpenFontSpec = desiredFontSpec;

	// Try to get an open font.
	CFont* openFont = NULL;
	TRAPD(error, GetNearestOpenFontInPixelsL(
		openFont, actualOpenFontSpec, desiredFontSpec, aMaxHeight));

	// Try to get a bitmap font - if getting open font failed or the match was not perfect.
	CFont* bitmapFont = NULL;
	TInt errorBitmap=KErrNone;
	TOpenFontSpec actualBitmapFontSpec;
	if (error || !openFont || actualOpenFontSpec != desiredFontSpec)
		{
		TFontSpec f;
		originalFontSpec.GetTFontSpec(f);
		if (iTypefaceList.Count() > 0)
			{
			TRAP(errorBitmap, GetNearestBitmapFontInPixelsL(bitmapFont, f, aMaxHeight));
			}
		actualBitmapFontSpec = f;
		actualBitmapFontSpec.CompensateForAspectRatio(iKPixelWidthInTwips, iKPixelHeightInTwips);
		}

	//If both attempts fail then there is nothing more we can do...
	if (!openFont && !bitmapFont)
		{
		//If an error caused no fonts to be returned then forward the error
		if (error)
			return error;
		if (errorBitmap)
			return errorBitmap;
		}
	// Choose the best candidate
	if (NULL == openFont && NULL == bitmapFont)
		{
		// Try to get an open font again - unnamed font.
		// See COpenFontFile::GetNearestFontHelper - how it works with unnamed fonts.
		actualOpenFontSpec.SetName(KNullDesC);
		TRAP(error, GetNearestOpenFontInPixelsL(
			openFont, actualOpenFontSpec, desiredFontSpec, aMaxHeight));
		if (KErrNone == error && openFont)
			{
			aFont = openFont;
			return KErrNone;
			}
		else
			{
			OstTraceExt2( TRACE_FATAL, CFONTSTORE_GETNEARESTFONTINPIXELS, "GetNearestOpenFontInPixelsL() return %d, openFont is 0x%x, Panic(EFntNoFontFound)",
			        error, (unsigned int)openFont);
			__ASSERT_DEBUG(0, Panic(EFntNoFontFound));
			return KErrGeneral;
			}
		}
	else if (NULL == openFont)
		{
		if (actualBitmapFontSpec.Name().CompareF(desiredFontSpec.Name()) != 0)
			{
			// Try to get the nearest open font - because no exact bitmap font match for typeface name
			actualOpenFontSpec.SetName(KNullDesC);
			desiredFontSpec.SetName(KNullDesC);
			TRAPD(errorOpen, GetNearestOpenFontInPixelsL(
				openFont, actualOpenFontSpec, desiredFontSpec, aMaxHeight));
			if (KErrNone == errorOpen && openFont)
				{
				aFont = openFont;
				ReleaseFont(bitmapFont);
				}
			}
		if (aFont == NULL)
			{
			aFont = bitmapFont;
			}
		}
	else if (NULL == bitmapFont)
		{
		aFont = openFont;
		}

	if (aFont == NULL)
		{
		// Pick the closest match; for backward compatibility prefer a bitmap font if there is a dead heat.
		const TInt open_match = MatchFontSpecsInPixels(
			actualOpenFontSpec, desiredFontSpec, openFont->FontMaxHeight(), aMaxHeight);
		const TInt bitmap_match = MatchFontSpecsInPixels(
			actualBitmapFontSpec, desiredFontSpec, bitmapFont->FontMaxHeight(), aMaxHeight);
		if (open_match > bitmap_match)
			{
			aFont = openFont;
			ReleaseFont(bitmapFont);
			}
		else
			{
			aFont = bitmapFont;
			ReleaseFont(openFont);
			}
		}
	return KErrNone;
	}

/** Gets a bitmap font using the given font UID and algorithmic style.

Tries to find a bitmap font in the font store with the given UID. If successful, 
the given algorithmic style is applied to the font and this font (which may 
be a suitable one already existing in the store, or may be a newly generated 
font) is stored in aFont. If unsuccessful then no font is stored in aFont.

@param aFont On return, a device-dependent font.
@param aUid A bitmap font UID.
@param aAlgStyle An algorithmic style to apply to the font.
@return KErrNone if the font is found, KErrNotFound if the font is not found, 
or another system-wide error code. */
EXPORT_C TInt CFontStore::GetFontById(
	CFont*&				aFont,
	TUid				aUid,
	const TAlgStyle&	aAlgStyle)
	{
	aFont = NULL;
	CFontBitmap* fontbitmap = GetFontBitmapById(aUid);
	if (!fontbitmap)
		{
		return KErrNotFound;
		}

	const TInt height = fontbitmap->iCellHeightInPixels * aAlgStyle.HeightFactor();
	TFontSpec fontspec;
	fontspec.iHeight=height;	//Otherwise IsFontLoaded() compares with zero height!!!
	fontspec.iTypeface.SetIsProportional(!aAlgStyle.IsMono() && fontbitmap->IsProportional());
	if (aAlgStyle.IsBold() || EStrokeWeightBold == fontbitmap->StrokeWeight())
		{
		fontspec.iFontStyle.SetStrokeWeight(EStrokeWeightBold);
		}
	if (aAlgStyle.IsItalic() || EPostureItalic == fontbitmap->Posture())
		{
		fontspec.iFontStyle.SetPosture(EPostureItalic);
		}
	if (IsFontLoaded(aFont, aAlgStyle, fontspec, fontbitmap->iUid))
		{
		return KErrNone;
		}

	// Fill in the typeface by finding a typeface with the same font bitmap.
	fontspec.iHeight = VerticalPixelsToTwips(height);
	const TInt n = iTypefaceFontBitmapList.Count();
	for (TInt i = 0; i < n; i++)
		{
		if (iTypefaceFontBitmapList[i].iFontBitmap == fontbitmap)
			{
			fontspec.iTypeface = *iTypefaceFontBitmapList[i].iTypeface;
			break;
			}
		}
	TRAPD(ret, aFont = NewFontL(fontspec, aAlgStyle, fontbitmap));
	return ret;
	}

/** Gets the number of typefaces held in the font store.

Note that this includes both open font typefaces and non-scalable typefaces.

@return The number of supported typefaces. */
EXPORT_C TInt CFontStore::NumTypefaces() const
	{
	return iTypefaceList.Count() + iOpenFontTypefaceSupportList.Count();
	}

/** Gets a typeface support object for the typeface in the font store represented 
by the given index.

Returns benignly with an empty TTypefaceSupport if the index is too high; 
this can happen if another process removes a typeface after the first process 
has already got the number of typefaces. However, if the aTypefaceIndex<0 
the function panics with EFntTypefaceIndexOutOfRange.

@param aTypefaceSupport On return, a typeface support object.
@param aTypefaceIndex An index number representing a typeface, which is valid 
if in the range 0 to (NumTypefaces() - 1). */
EXPORT_C void CFontStore::TypefaceSupport(TTypefaceSupport &aTypefaceSupport,TInt aTypefaceIndex) const
	{
	if (aTypefaceIndex < 0)
		Panic(EFntTypefaceIndexOutOfRange);

	/*
	Return benignly with an empty TTypefaceSupport if the index is too high; this can happen if another
	process removes a font after the first process has already got the number of typefaces.
	*/
	int typefaces = NumTypefaces();
	if (aTypefaceIndex >= typefaces)
		{
		aTypefaceSupport = TTypefaceSupport();
		return;
		}

	//now return Typeface Support for Open Fonts
	if (aTypefaceIndex >= iTypefaceList.Count())
		{
		// copy Open Font typeface details
		aTypefaceSupport = *iOpenFontTypefaceSupportList[aTypefaceIndex - iTypefaceList.Count()]->TypefaceSupport();
		return;
		}
	
	if (!((aTypefaceIndex >= 0) && (aTypefaceIndex < iTypefaceList.Count())))
	    {
	    OstTraceExt2( TRACE_FATAL, CFONTSTORE_TYPEFACESUPPORT, "aTypefaceIndex=%d, iTypefaceList.Count()=%d Panic(EFntTypefaceIndexOutOfRange)",
	            aTypefaceIndex, iTypefaceList.Count());
	    __ASSERT_DEBUG(0,Panic(EFntTypefaceIndexOutOfRange));
	    }
	TTypeface* typeface = iTypefaceList[aTypefaceIndex];
	aTypefaceSupport.iTypeface = *typeface;
	TInt count = iTypefaceFontBitmapList.Count();
	TInt i;
	for(i = 0; i < count && iTypefaceFontBitmapList[i].iTypeface != typeface; i++)
		{  // Finds first fontbitmap with correct typeface
		}
	aTypefaceSupport.iMinHeightInTwips = VerticalPixelsToTwips(iTypefaceFontBitmapList[i].HeightInPixels()); 
	aTypefaceSupport.iNumHeights=0;
	TInt height=0;
	for(; (i<count) && (typeface == iTypefaceFontBitmapList[i].iTypeface); i++)
		if (height != iTypefaceFontBitmapList[i].HeightInPixels())
			{
			height = iTypefaceFontBitmapList[i].HeightInPixels();
			aTypefaceSupport.iNumHeights++;
			}
	aTypefaceSupport.iMaxHeightInTwips=VerticalPixelsToTwips(height);
	aTypefaceSupport.iIsScalable = EFalse;
	}

/** Returns a font height, in twips, for a certain typeface and height index.

The font height is a height allowed for the typeface represented by aTypefaceIndex. 
The height returned increases with aHeightIndex.

If aTypefaceIndex<0 the function panics with EFntTypefaceIndexOutOfRange. 
If aTypefaceIndex is greater than the number of typefaces or aHeightIndex<0 
then the function returns 0. If aHeightIndex is greater than the number of 
heights then the function returns the biggest height. 

@param aTypefaceIndex An index number representing a typeface, which is valid 
in the range 0 to (NumTypefaces() - 1).
@param aHeightIndex A font height index number, which is valid in the range 
0 to (TTypefaceSupport::iNumHeights - 1). Note that the typeface support object 
for the typeface can be got using TypefaceSupport(). 
@return The height of a font, in twips.
@see TypefaceSupport()
*/
EXPORT_C TInt CFontStore::FontHeightInTwips(TInt aTypefaceIndex,TInt aHeightIndex) const
	{
	if (aTypefaceIndex >= iTypefaceList.Count())
		{
		// it's an Open Font managed by a COpenFontFile
		if (aTypefaceIndex >= NumTypefaces() || aHeightIndex < 0)
			return 0;
		
		const CTypefaceSupportInfo* supportInfo = iOpenFontTypefaceSupportList[aTypefaceIndex - iTypefaceList.Count()];
		TInt i = Min( supportInfo->NearestPointSizeIndex() + aHeightIndex, 
					  KOpenFontSizeArrayCount - 1);

		return gOpenFontSizeInTwipsArray[i];
		}
	else
		{
		return VerticalPixelsToTwips(FontHeightInPixels(aTypefaceIndex,aHeightIndex));
		}
	}

/** Returns a font height, in pixels, for a certain typeface and height index.

The font height is a height allowed for the typeface represented by aTypefaceIndex. 
The height returned increases with aHeightIndex.

If aTypefaceIndex<0 the function panics with EFntTypefaceIndexOutOfRange. 
If aTypefaceIndex is greater than the number of typefaces or aHeightIndex<0 
then the function returns 0. If aHeightIndex is greater than the number of 
heights then the function returns the biggest height. 

@param aTypefaceIndex An index number representing a typeface, which is valid 
in the range 0 to (NumTypefaces() - 1).
@param aHeightIndex A font height index number, which is valid in the range 
0 to (TTypefaceSupport::iNumHeights - 1). Note that the typeface support object 
for the typeface can be got using TypefaceSupport(). 
@return The height of a font, in pixels.
@see TypefaceSupport()
*/
EXPORT_C TInt CFontStore::FontHeightInPixels(TInt aTypefaceIndex,TInt aHeightIndex) const
	{

	if (aTypefaceIndex >= iTypefaceList.Count())	// it's an Open Font managed by a COpenFontFile
		return VerticalTwipsToPixels(FontHeightInTwips(aTypefaceIndex,aHeightIndex));
	else
		{
		if (aTypefaceIndex < 0)
			Panic(EFntTypefaceIndexOutOfRange);
		TTypeface* typeface = iTypefaceList[aTypefaceIndex];
		TInt count = iTypefaceFontBitmapList.Count();
		TInt i;
		for(i=0; (i < count) && (iTypefaceFontBitmapList[i].iTypeface != typeface); i++)
			{  // Finds first fontbitmap with correct typeface
			}
		TInt height=0,h=0;
		for(; (i<count) && (iTypefaceFontBitmapList[i].iTypeface==typeface)
						&& (h<=aHeightIndex); i++)
			if (height != iTypefaceFontBitmapList[i].HeightInPixels())
				{
				height = iTypefaceFontBitmapList[i].HeightInPixels();
				h++;
				}
		return height;
		}
	}

void CFontStore::InternalizeFontStoreFileL(CFontStoreFile* aFontStoreFile, TInt aFontVersion)
	{
	RStoreReadStream stream;
	stream.OpenLC(*aFontStoreFile->iFileStore,aFontStoreFile->iDataStreamId);
	TInt i, size = stream.ReadInt32L();
	TInt opsPerformed=0;
	for (i=0; i<size; i++)
		{
		// font is placed in shared heap
		CFontBitmap* fontbitmap=(CFontBitmap*)iHeap->AllocL(sizeof(CFontBitmap));
		new(fontbitmap) CFontBitmap(iHeap,aFontStoreFile);
		CleanupReleasePushL(*fontbitmap);
		fontbitmap->InternalizeL(stream, aFontVersion);
		if (GetFontBitmapById(fontbitmap->iUid))
			{
			fontbitmap->Release();
			}
		else
			{
			iFontBitmapList.AppendL(fontbitmap);
			opsPerformed++;
			}
		// safely internalized & owned
		CleanupStack::Pop(fontbitmap);
		}
	size = stream.ReadInt32L();
	for (i=0; i<size; i++)
		{
		TTypeface* typeface=new(ELeave) TTypeface;
		CleanupStack::PushL(typeface);
		typeface->InternalizeL(stream);
		TInt index,count=iTypefaceList.Count(); 		
		for (index=0; (index<count) && typeface->iName.CompareF(iTypefaceList[index]->iName); index++)
			{  // Looks to see if typeface is already in list
			}
		if (index == count) // If typeface not in list
			{
			iTypefaceList.AppendL(typeface);
			index = iTypefaceList.Count()-1;
			opsPerformed++;
			CleanupStack::Pop();  // typeface
			}
		else
			{
			CleanupStack::Pop();  // typeface
			delete typeface;
			typeface=iTypefaceList[index];
			}
		TInt num = stream.ReadInt32L();
		for (TInt j=0; j<num; j++)
			{
			TUid uid;
			stream >> uid;
 			CFontBitmap* fontbitmap = GetFontBitmapById(uid);
 			if (!fontbitmap)
 			    {
 			    OstTrace0( TRACE_FATAL, CFONTSTORE_INTERNALIZEFONTSTOREFILEL, "Panic(EFntFontBitmapNotLoaded)" );
 			   __ASSERT_DEBUG(0,Panic(EFntFontBitmapNotLoaded));
 			    }
#ifndef _DEBUG
			User::LeaveIfNull(fontbitmap);
#endif			
			TTypefaceFontBitmap typefacefontbitmap(iTypefaceList[index],fontbitmap);
			typefacefontbitmap.iWidthFactor=stream.ReadInt8L();
			typefacefontbitmap.iHeightFactor=stream.ReadInt8L();
			count=iTypefaceFontBitmapList.Count();
			TInt pos;
			if (index == count)
				{
				pos=count;
				}
			else
				{
				
				for (pos=0; (pos<count) && (iTypefaceFontBitmapList[pos].iTypeface != typeface); pos++)
					{  // Finds position of first fontbitmap with same typeface
					}
				for (; (pos<count) && (iTypefaceFontBitmapList[pos].iTypeface == typeface)
				                   && (iTypefaceFontBitmapList[pos].HeightInPixels()<typefacefontbitmap.HeightInPixels()); pos++)
					{
					}
				for (; (pos<count) &&  (iTypefaceFontBitmapList[pos].iTypeface == typeface)
				                   &&  (iTypefaceFontBitmapList[pos].HeightInPixels()==typefacefontbitmap.HeightInPixels())
				                   &&  (iTypefaceFontBitmapList[pos].iFontBitmap->Posture()<fontbitmap->Posture()); pos++)
	       			{  // Finds position after fontbitmap with same height
					}
				for (; (pos<count) &&  (iTypefaceFontBitmapList[pos].iTypeface == typeface)
				                   &&  (iTypefaceFontBitmapList[pos].HeightInPixels()==typefacefontbitmap.HeightInPixels())
				                   &&  (iTypefaceFontBitmapList[pos].iFontBitmap->StrokeWeight()<fontbitmap->StrokeWeight()); pos++)
	       			{  // Finds position after fontbitmap with same height
					}
				}
			TTypefaceFontBitmap* tfbAtPos=(pos==count)
											?	NULL
											:	&iTypefaceFontBitmapList[pos];
			if (	!tfbAtPos	
				||	tfbAtPos->iTypeface!=typeface
				||	tfbAtPos->HeightInPixels()!=typefacefontbitmap.HeightInPixels()
				||	tfbAtPos->iFontBitmap->Posture()!=fontbitmap->Posture()
				||	tfbAtPos->iFontBitmap->StrokeWeight()!=fontbitmap->StrokeWeight()
				||	tfbAtPos->iWidthFactor!=typefacefontbitmap.iWidthFactor	
				||	tfbAtPos->iHeightFactor!=typefacefontbitmap.iHeightFactor	
				)
				{
				iTypefaceFontBitmapList.InsertL(pos,typefacefontbitmap);
				opsPerformed++;
				}
			}
		}
		CleanupStack::PopAndDestroy(); // stream
		if (!opsPerformed)
			User::Leave(KErrAlreadyExists);
	}

TTypeface* CFontStore::GetNearestTypeface(const TTypeface& aTypeface) const
	{
	TInt index,count = iTypefaceList.Count();
	if (count <= 0)
	    {
	    OstTrace1( TRACE_FATAL, CFONTSTORE_GETNEARESTTYPEFACE, "count=%d, Panic(EFntNoTypefaces)", count );
	    __ASSERT_DEBUG(0,Panic(EFntNoTypefaces));
	    }
	for (index=0; (index<count) && aTypeface.iName.CompareF(iTypefaceList[index]->iName); index++)	  
		{ // tries matching	typeface name
		}
	if (index==count)
		{
		if (!aTypeface.IsSymbol())
			{
			for (index=0; (index<count) && ((iTypefaceList[index]->IsSymbol()) ||
			                                (aTypeface.IsProportional() != iTypefaceList[index]->IsProportional()) || 
			                                (aTypeface.IsSerif() != iTypefaceList[index]->IsSerif())); index++)	  
				{ // tries matching	typeface flags
				}
			if (index==count)
				for (index=0; (index<count) && ((iTypefaceList[index]->IsSymbol()) ||
				                                (aTypeface.IsProportional()!=iTypefaceList[index]->IsProportional())); index++)	  
					{ // tries matching	typeface flags
					}
			if (index==count)
				for (index=0; (index<count) && iTypefaceList[index]->IsSymbol(); index++)	  
					{ // finds first non-symbol typeface
					}
			}
		else
			{
			for (index=0; (index<count) && (!iTypefaceList[index]->IsSymbol()); index++)	  
				{ // finds first symbol typeface
				}
			}
		}

	if (index==count)
		index=0;
	return iTypefaceList[index];
	}

TTypefaceFontBitmap CFontStore::GetNearestTypefaceFontBitmap(const TFontSpec& aFontSpecInPixels, TInt aMaxHeight)
	{
	TTypefaceFontBitmap typefacefontbitmap;
	TInt count = iTypefaceFontBitmapList.Count();
	if (count <= 0)
	    {
	    OstTrace1( TRACE_FATAL, CFONTSTORE_GETNEARESTTYPEFACEFONTBITMAP, "count=%d, Panic(EFntNoTypefaceFontBitmaps)", count );
	    __ASSERT_DEBUG(0, Panic(EFntNoTypefaceFontBitmaps));
	    }
	TInt i;
	TInt j;
	// Assumes there is at least one fontbitmap per typeface
	for (i = 0; (i < count) && !(aFontSpecInPixels.iTypeface == *iTypefaceFontBitmapList[i].iTypeface); i++)
		{  // Finds first fontbitmap with correct typeface
		}
	if (i >= count)
	    {
	    OstTraceExt2( TRACE_FATAL, DUP1_CFONTSTORE_GETNEARESTTYPEFACEFONTBITMAP, "i=%d, count=%d, Panic(EFntTypefaceHasNoFontBitmaps)", i, count );
	    __ASSERT_DEBUG(i < count, Panic(EFntTypefaceHasNoFontBitmaps));
	    }
	TTypeface* typeface = iTypefaceFontBitmapList[i].iTypeface;
	TInt height = 0;
	if (aMaxHeight > 0)
		{ // need to check against max height
		for (j = i; (j < count) && (iTypefaceFontBitmapList[ j ].iTypeface == typeface) 
								&& (iTypefaceFontBitmapList[j].iFontBitmap->FontMaxHeight() <= aMaxHeight); j++)
			{
			if (iTypefaceFontBitmapList[j].HeightInPixels() != height)
				{
				height = iTypefaceFontBitmapList[j].HeightInPixels();
				i = j;
				}
			}
		}
	else
		{ // just check against height
		for (j = i; (j < count) && (iTypefaceFontBitmapList[j].iTypeface == typeface)
								&& (iTypefaceFontBitmapList[j].HeightInPixels() <= aFontSpecInPixels.iHeight); j++)
			{
			if (iTypefaceFontBitmapList[j].HeightInPixels() != height)
				{
				height = iTypefaceFontBitmapList[j].HeightInPixels();
				i = j;
				}
			}
		}
	for (j = i; (j < count) && (iTypefaceFontBitmapList[j].iTypeface == typeface)
							&& (iTypefaceFontBitmapList[j].HeightInPixels() == height); j++) 
		{  // Finds position after fontbitmap correct posture
		if ((iTypefaceFontBitmapList[j].iFontBitmap->Posture() <= aFontSpecInPixels.iFontStyle.Posture()) &&
			(iTypefaceFontBitmapList[j].iFontBitmap->StrokeWeight() <= aFontSpecInPixels.iFontStyle.StrokeWeight()))
			i = j;
		}
	typefacefontbitmap = iTypefaceFontBitmapList[i];		
	return typefacefontbitmap;
	}

CFontBitmap* CFontStore::GetFontBitmapById(TUid aUid)
	{
	CFontBitmap* fontbitmap = NULL;
	TInt i, count = iFontBitmapList.Count();
	for (i = 0; (i < count) && (aUid != iFontBitmapList[i]->iUid); i++)
		{ // finds matching font bitmap
		}
	if (i<count) // bitmap found
		fontbitmap = iFontBitmapList[i];
	return fontbitmap;				
	}

/** The method uses the fontspec and algorithmic style only.
The Uid is not used for Open Fonts. They all have a Uid of 0.
*/
TBool CFontStore::IsFontLoaded(
	CFont*&					aFont,
	const TAlgStyle&		aAlgStyle,
	const TOpenFontSpec&	aFontSpecInPixels,
	TInt					aMaxHeight) const
	{
	TFontSpec fontSpec;
	aFontSpecInPixels.GetTFontSpec(fontSpec);
	return IsFontLoaded(aFont, aAlgStyle, fontSpec, TUid::Uid(0), aMaxHeight);
	}

//stub functions to help optimise IsFontLoaded.
//These shims are more efficient than a pointer-to-member.
static TInt GetHeightShimMaxHeight(CFont* aFont)	{	return	aFont->FontMaxHeight();		}
static TInt GetHeightShimHeightPixels(CFont* aFont)	{	return	aFont->HeightInPixels();	}

/**
Font spec comparison is based on
1. Max height if its given, otherwise design height
2. All other attributes in TFontSpec

@see CFbsTypefaceStore::IsFontLoaded
@see CPdrTypefaceStore::IsFontLoaded
*/
TBool CFontStore::IsFontLoaded(
	CFont*&				aFont,
	const TAlgStyle&	aAlgStyle,
	const TFontSpec&	aFontSpecInPixels,
	TUid				aUid,
	TInt				aMaxHeight) const
	{	//It is not worth copying iFontAccess to frame as it doesn't get register optimised.
	const TInt count = iFontAccess->Count();
	const TInt reqHeight =  aMaxHeight ? aMaxHeight : aFontSpecInPixels.iHeight;
	TInt (*heightFn)(CFont*) = aMaxHeight ? GetHeightShimMaxHeight : GetHeightShimHeightPixels;
	for (TInt i = 0; i < count; i++)
		{
		CBitmapFont* bitmapFont = reinterpret_cast<CBitmapFont*>((*iFontAccess)[i].iFont);
		if (bitmapFont->Uid() != aUid ||
			bitmapFont->iAlgStyle != aAlgStyle	||
			heightFn(bitmapFont) != reqHeight
			)
			{
			continue;
			}
		TFontSpec fontSpec = bitmapFont->FontSpecInTwips();
		if (	fontSpec.iFontStyle == aFontSpecInPixels.iFontStyle &&
				fontSpec.iTypeface.iName == aFontSpecInPixels.iTypeface.iName
			) 
			{
			(*iFontAccess)[i].iAccessCount++;
			aFont = static_cast<CFont*>(bitmapFont);
			return ETrue;
			}
		}
	return EFalse;
	}

CBitmapFont* CFontStore::NewFontL(
	const TFontSpec&	aFontSpecInTwips,
	const TAlgStyle&	aAlgStyle,
	CFontBitmap*		aFontBitmap)
	{
	CBitmapFont* f = CBitmapFont::NewL(iHeap, aFontSpecInTwips, aAlgStyle, aFontBitmap);
	CleanupStack::PushL(f);
	AddFontL(f);
	CleanupStack::Pop();
	f->SetUniqueFontId(++iUniqueFontIdCount);
	return f;
	}

CBitmapFont* CFontStore::NewFontL(
	const TOpenFontSpec&	aFontSpecInTwips,
	const TAlgStyle&		aAlgStyle,
	COpenFont*				aOpenFont)
	{
	TFontSpec spec;
	aFontSpecInTwips.GetTFontSpec(spec);
	CBitmapFont* f = CBitmapFont::NewL(iHeap, spec, aAlgStyle, aOpenFont);
	CleanupStack::PushL(f);
	AddFontL(f);
	CleanupStack::Pop();
	f->SetUniqueFontId(++iUniqueFontIdCount);
	return f;
	}

// This function is biased towards rounding up
// The cutoff is effectively at one-third of a pixel
// This is to match VerticalTwipsToPixels(...)
TInt CFontStore::VerticalPixelsToTwips(TInt aPixelHeight) const
	{
	return ((aPixelHeight * iKPixelHeightInTwips) + 667) / 1000; // Rounds up above one-third of a pixel
	}

// This function is biased towards rounding down
// The cutoff is effectively at two-thirds of a twip
// This is to support the legacy system of rounding down
// but with a sanity-check cutoff to round up past two-thirds
TInt CFontStore::VerticalTwipsToPixels(TInt aTwipsHeight) const
	{
	if (!iKPixelHeightInTwips)
	    {
	    OstTrace0( TRACE_FATAL, CFONTSTORE_VERTICALTWIPSTOPIXELS, "Panic(EFntKPixelHeightInTwipsZero)" );
	    __ASSERT_DEBUG(0, Panic(EFntKPixelHeightInTwipsZero));
	    }
	return ((aTwipsHeight * 1000) + (iKPixelHeightInTwips / 3)) / iKPixelHeightInTwips;  // Rounds down below two-thirds of a twip
	}

/** Installs and takes ownership of an Open Font rasterizer.

@param aRasterizer A pointer to an Open Font rasterizer. */
EXPORT_C void CFontStore::InstallRasterizerL(COpenFontRasterizer* aRasterizer)
	{
	iOpenFontRasterizerList.AppendL(aRasterizer);
	}
	
/** Install and takes ownership of the shaper

@param aShaperFactory A pointer to a shaper factory. */
EXPORT_C void CFontStore::InstallShaperFactoryL(CShaperFactory* aShaperFactory)
	{
	iShaperFactoryList.AppendL(aShaperFactory);
	}

/** Deletes the glyph cache belonging to a particular client. 

Called by ~CFbClient().

@param aSessionHandle A session handle. */
EXPORT_C void CFontStore::DeleteSessionCache(TInt aSessionHandle)
	{
	iOpenFontSessionCacheList->DeleteCache(iHeap,aSessionHandle);
	}

const CArrayPtrFlat<CShaperFactory>* CFontStore::ShaperFactoryList() const
	{
	return &iShaperFactoryList;
	}

/** Returns the list of session caches owned by the COpenFonts
@internalComponent */
COpenFontSessionCacheList* CFontStore::GetSessionCacheList()
	{
	return iOpenFontSessionCacheList;
	}
	
/** Returns the total cache memory used by all the COpenFonts in the system 
 	@return Total cache memory usage
	@internalComponent 
*/	
TInt CFontStore::GetShaperCacheMemUsage()
	{
	return iOpenFontShaperCacheMemUsage;
	}
	
/** Updates the total cache memory used by all the COpenFonts in the system
 	@param aUsage New value of total cache memory usage
	@internalComponent
*/	
void CFontStore::SetShaperCacheMemUsage(TInt aUsage)
	{
	iOpenFontShaperCacheMemUsage = aUsage;
	}
	
/** Returns the list of COpenFontFiles owned by the CFontStore object
	@internalComponent
*/	
CArrayPtrFlat<COpenFontFile>* CFontStore::GetOpenFontFileList()
	{
	return &iOpenFontFileList;
	}

void CFontStore::IncNumShaperCaches()
	{
	iNumberOfShaperCaches++;
	}
	
void CFontStore::DecNumShaperCaches()
	{
	iNumberOfShaperCaches--;
	}

TInt CFontStore::GetNumShaperCaches()
	{
	return iNumberOfShaperCaches;
	}


/** Adds the typeface( of the currently added openfont file from the store) to Open Font System typefaces Supported List
ignoring duplicate family names.
1. Prior to adding the entry into the typefaceList, it is searched whether already exists or not
2. If it is found only its reference count is incremented.
3. If it is not found then its reference count is set to 1 and added that entry into the typefacelist.
4. If 0 typefaces are added then KErrAlreadyExists is returned causing the new font to be unloaded.
@return KErrNone is successful, otherwise a system wide error code
@internalComponent
*/
TInt CFontStore::AddTypefacesToSupportList(COpenFontFile* aOpenFontFile)
	{
	TInt faceCount = aOpenFontFile->FaceCount();
	CTypefaceSupportInfo* typefaceFamily = NULL;
	TInt error = KErrNone;
	TBool facesAdded=0;
	// go through all fonts, stop early if an error
	for (TInt faceIndex = 0; (faceIndex < faceCount) && (error == KErrNone); ++faceIndex)
		{
		//Preparing the Supported Typeface Entry
		if (typefaceFamily == NULL)
			{ // need a new typeface object
			typefaceFamily = new CTypefaceSupportInfo();
			if (typefaceFamily == NULL)
				{ // failed
				error = KErrNoMemory;
				break;
				}
			}

		// initialise for next type face
		const TOpenFontFaceAttrib& faceAttrib = aOpenFontFile->FaceAttrib(faceIndex);
		typefaceFamily->SetTypefaceInfo(faceAttrib, VerticalPixelsToTwips(faceAttrib.MinSizeInPixels()));

		TInt fontPos = -1;	// an invalid index
		TInt findError = iOpenFontTypefaceSupportList.FindInOrder(typefaceFamily, fontPos, CTypefaceSupportInfo::CompareFontNames);
 
		// new font family?
		if (findError == KErrNotFound)
			{
			// back reference to the font file
			error = typefaceFamily->AddFontFilePtr(aOpenFontFile);
			if (error == KErrNone)
				{ // transfer ownership
				error = iOpenFontTypefaceSupportList.Insert(typefaceFamily, fontPos);
				if (error == KErrNone)
					{
					typefaceFamily = NULL;
					facesAdded++;					
					}
				}
			}
		else
			{
			// back reference to the new font file (allowed to fail with KErrAlreadyExists)
			TInt addError = iOpenFontTypefaceSupportList[fontPos]->AddUniqueFontFilePtr(aOpenFontFile,faceAttrib);
			if (addError==KErrNone)
				{
				facesAdded++;
				}
			else if (addError == KErrInUse )
				{
				//remove typeface from loaded font file????
				}
			else if (addError != KErrAlreadyExists )
				{
				error = addError;
				}
			//else KErrAlreadyExists because loaded by a lower-indexed face in this font file, so faces won't be zero.
			}
		}

	if (typefaceFamily != NULL)
		{
		delete typefaceFamily;
		}
	if (facesAdded==0 && error==KErrNone)
		{
			error= KErrAlreadyExists;
		}
#ifdef _DEBUG
	// check that array is in order
	const TInt countTypefaceFamilies = iOpenFontTypefaceSupportList.Count();
	for (TInt fontPos = 0; fontPos < countTypefaceFamilies - 1; ++fontPos)
		{
		TInt cmp = CTypefaceSupportInfo::CompareFontNames(*iOpenFontTypefaceSupportList[fontPos], *iOpenFontTypefaceSupportList[fontPos+1]);
		ASSERT(cmp < 0);
		}
#endif

	return error;
	}


/** Removes the typefaces from the Support List corresponding to the fontfile removed from
	the fontstore
This is done by 
1. Searching in the typefacesupportList for pointer references to the font file.
2. If the file is referenced by the typeface family remove the reference.
3. If typeface family no has any implementations remove the family
@internalComponent
*/
void CFontStore::RemoveTypefacesFromSupportList(COpenFontFile* aOpenFontFile)
	{
	const TInt countTypefaceFamilies = iOpenFontTypefaceSupportList.Count();

	// search typeface families
	for (TInt fontPos = countTypefaceFamilies - 1; fontPos >= 0; --fontPos)
		{
		CTypefaceSupportInfo* fontInfo = iOpenFontTypefaceSupportList[fontPos];

		// look for back pointer to this Font File
		TInt fontFileIndex = fontInfo->FindFontFilePtr(aOpenFontFile);

		if (fontFileIndex > KErrNotFound)
			{ // remove pointer
			if (fontInfo->RemoveFontFilePtr(fontFileIndex))
				{ // last implementation of the family, so delete it
				delete fontInfo;
				iOpenFontTypefaceSupportList.Remove(fontPos);
				}
			}
		}
	}

/**
This function checks the supplied typeface name against the existing bitmap and open 
typefaces, to see if it present.

@param aName The name of the typeface family name to be checked. 
@return ETrue if the typeface is present, EFalse otherwise.

@released
@internalTechnology 
*/
EXPORT_C TBool CFontStore::HaveTypefaceFamilyName(const TDesC& aName)
	{
	//there are two lists for typefaces
	//not supporting linked fonts here
	
	//first list is open fonts
	TInt count = iOpenFontTypefaceSupportList.Count();
	for (TInt index=count-1;index>=0;index--)
		{
		if (aName.CompareF(iOpenFontTypefaceSupportList[index]->iSupport.iTypeface.iName)==0)
			return ETrue;
		}

	//now look through bitmap fonts
	count = iTypefaceList.Count();
	for (TInt index=count-1;index>=0;index--)
		{
		if (aName.CompareF(iTypefaceList[index]->iName)==0)
			return ETrue;
		}

	return EFalse;
	}

/**
@internalTechnology
@released
*/
EXPORT_C TInt CFontStore::CreateLinkedTypeface(const TLinkedTypefaceSpecificationArgs &aLinkedTypefaceSpec, TInt aSession, TInt& aId)
	{
	TInt err;
	TInt retErr=0;
	TRAP(err,retErr = CreateLinkedTypefaceL(aLinkedTypefaceSpec,aSession,aId));
	if (err!=KErrNone)
		{	
		return err;
		}
	else
		{
		return retErr;	
		}
	}

/* this function is used to create the linked typeface, the linked typeface is created on
 * the font and bitmap server heap, not the shared heap.
 */	
TInt CFontStore::CreateLinkedTypefaceL(const TLinkedTypefaceSpecificationArgs &aLinkedTypefaceSpec, TInt /*aSession*/, TInt& /*aId*/)	
	//reconstuct a CLinkedTypefaceSpecification - could store as T type, but then the
	// maximum size would be used for every linked typeface
	//
	{
	const TAny* extensionPtr = FontLinkingInterface();
	
	if (extensionPtr == NULL)
		return KErrNotSupported;
	
	//Convert the TLinkedTypefaceSpecification args into the
	//COpenFontLinkedTypefaceSpecification recognised by the rasteriser 
	COpenFontLinkedTypefaceSpecification* spec = 
		COpenFontLinkedTypefaceSpecification::NewLC(aLinkedTypefaceSpec);
	
	TInt err = ValidateLinkedFontSpecificationL(*spec, EFalse);
	
	if (err == KErrNone)
    {
		GenerateLinkedFontFileL(*spec, extensionPtr, EFalse);
    }

	CleanupStack::PopAndDestroy(spec);
	return err;
	}

/*
@internalTechnology
Retrieve the specification from a linked typeface; the name of the typeface should be specified in the parameter.

@param aLinkedTypefaceSpec An empty linked typeface spec containing the name of the typeface to be retrieved.

@leave One of the system wide error codes.
*/
EXPORT_C void CFontStore::GetLinkedTypefaceL(TLinkedTypefaceSpecificationArgs &aLinkedTypefaceSpec)
	{
	const TAny* extensionPtr = FontLinkingInterface();
	
	if (extensionPtr == NULL)
		User::Leave(KErrNotSupported);

	MOpenFontLinkedTypefaceExtension* linkedExt = (MOpenFontLinkedTypefaceExtension*)extensionPtr;
	
	//Check whether the font is loaded
	CTypefaceSupportInfo* supinfo;
	TTypefaceSupport support;
	COpenFontFile* fetchFont=NULL;
	for (TInt count=0;count<(iOpenFontTypefaceSupportList.Count());count++)
		{
		supinfo = iOpenFontTypefaceSupportList[count];
		support = supinfo->iSupport;			
		
		if (aLinkedTypefaceSpec.iName==support.iTypeface.iName)
			{
			fetchFont = supinfo->iOpenFontFilePtrArray[0];
			
			break;	
			}
		}
	if (fetchFont==NULL)
		{
		User::Leave(KErrNotFound);	
		}

	COpenFontLinkedTypefaceSpecification* typefaceSpec = COpenFontLinkedTypefaceSpecification::NewLC(aLinkedTypefaceSpec.iName);
	TFileName fetchFile = fetchFont->FileName();
	linkedExt->GetLinkedTypefaceSpecificationL(fetchFile, *typefaceSpec);
	aLinkedTypefaceSpec = *typefaceSpec;
	CleanupStack::PopAndDestroy(typefaceSpec);
	}

/*
@internalTechnology
Updates an existing linked typeface to a new specification.

@param aLinkedTypefaceSpec The new specification for the typeface

@leave KErrNotSupported Font linking is not supported by any of the installed rasterizers
@leave One of the system wide error codes.
*/
EXPORT_C void CFontStore::UpdateLinkedTypefaceL(const TLinkedTypefaceSpecificationArgs& aLinkedTypefaceSpec)
	{
	const TAny* extensionPtr = FontLinkingInterface();
	
	if (extensionPtr == NULL)
		User::Leave(KErrNotSupported);
	
	//Convert the TLinkedTypefaceSpecification args into the
	//COpenFontLinkedTypefaceSpecification recognised by the rasteriser 
	COpenFontLinkedTypefaceSpecification* spec = 
		COpenFontLinkedTypefaceSpecification::NewLC(aLinkedTypefaceSpec);
	
	TInt err = ValidateLinkedFontSpecificationL(*spec, ETrue);
	User::LeaveIfError(err);
	
	GenerateLinkedFontFileL(*spec, extensionPtr, ETrue);

	CleanupStack::PopAndDestroy(spec);
	}

/**
@internalTechnology
Performs the loading of font files at boot time. This is a three stage procedure.

1. Copies linked fonts to be updated from their temporary directory into the linked fonts folder
2. Loads all the linked fonts from the linked fonts folder.
3. Loads all remaining font files from *:\resource\fonts
 */
EXPORT_C void CFontStore::LoadFontsAtStartupL()
	{
	//First copy any updated linked fonts from their temporary dir to the linked font dir
	CFileMan* fm = CFileMan::NewL(iFs);
	CleanupStack::PushL(fm);
	
	//Get the fbserv private folder
	TBuf<KMaxPrivatePathLength> privPath;
	iFs.PrivatePath(privPath);
	
	//Construct the linked font dir path
	TBuf<KMaxLinkedFontPathLength> linkedFontDir;
	linkedFontDir.Format(KLinkedFontFileFolder, KLinkedFontDrive, &privPath);
	
	//Construct the linked font temp dir path
	TBuf<KMaxLinkedFontPathLength> linkedFontTempDir;
	linkedFontTempDir.Append(linkedFontDir);
	linkedFontTempDir.Append(KLinkedFontFileTempFolder);
	
	//Copy the temp folder into the main folder
	TInt err = fm->Copy(linkedFontTempDir, linkedFontDir, CFileMan::EOverWrite);

	if (err == KErrNone)
		{
		//If sucessful copying remove the temp folder
		//A leave is not thrown as the empty directory remaining is not a significant enough
		//to prevent fbserv from starting
		fm->RmDir(linkedFontTempDir);
		}
	 
	CleanupStack::PopAndDestroy(fm);
	
	//Load Linked Fonts
	LoadFontsL(linkedFontDir);
	
	//Load the remaining fonts
	LoadFontsL(KFBSERVFontDirSecure);
	}
/**
@internalTechnology
Returns the pointer to the first rasterizer supporting font linking (KUidLinkedTypefaceRasterizerExtension).
*/
const TAny* CFontStore::FontLinkingInterface() const
	{
	TInt rastcount = iOpenFontRasterizerList.Count();
	COpenFontRasterizer* rast;
	TAny* extensionPtr = NULL;
	
	//Check each rasterizer in turn to check it supports the extended interface.
	//In reality there should only be one but multiple rasterizers are supported.
	for (TInt i = 0 ; i != rastcount ; i++)
		{
		rast = iOpenFontRasterizerList[i];
		rast->ExtendedInterface(KUidLinkedTypefaceRasterizerExtension, extensionPtr);
		
		if (extensionPtr != NULL)
			break;
		}
	
	return extensionPtr;
	}

/**
Validates the linked font specification passed as a parameter.

@param aSpec The specification to be validated
@param aOverwrite ETrue if this is an update operation, EFalse otherwise
*/
TInt CFontStore::ValidateLinkedFontSpecificationL(COpenFontLinkedTypefaceSpecification& aSpec, TBool aOverwrite) const
	{
	//see if the typeface is an existing regular typeface.
	if (!aOverwrite)
		{
		TInt numberTypefaces = iTypefaceList.Count();
		TInt c;
		
		for (c=0;c<numberTypefaces;c++)
			{
			TTypeface* ptr=(iTypefaceList)[c];
			
			/* only comparing on the name */
			if (0== ptr->iName.CompareF(aSpec.Name()))
				{
				//A typeface with this name is already loaded
				return KErrAlreadyExists;
				}
			}
			
		TInt numberOpenFontFiles = iOpenFontFileList.Count();
		for (c=0;c<numberOpenFontFiles;c++)
			{
			COpenFontFile* ptr=(iOpenFontFileList)[c];
			
			numberTypefaces= ptr->FaceCount();
			TInt c2;
			for (c2=0;c2<numberTypefaces;c2++)
				{
				const TOpenFontFaceAttrib& attrib = ptr->FaceAttrib(c2);

				/* only comparing on the name */
				if (0== attrib.ShortFullName().CompareF(aSpec.Name()))
					{
					//have a matching name, return error
					return KErrAlreadyExists;
					}
				if (0== attrib.ShortFamilyName().CompareF(aSpec.Name()))
					{
					//have a matching name, return error
					return KErrAlreadyExists;
					}	
				}
			}
		}
		//For each element, check whether the typeface exists and add the correct filename to the spec if it does
		TInt numTypefaces = aSpec.TypefaceCount();
		TFileName fileName;
		COpenFontLinkedTypefaceElementSpec* element;
		
		for (TInt counter=0;counter<numTypefaces;counter++)
			{
			element = aSpec.Typeface(counter);
			
			if (GetFontFilePath(element->ElementName(), fileName))
				{
				element->SetFileNameL(fileName);
				}
			else
				{
				return KErrNotFound;
				}
			}
		return KErrNone;
	}


/**
Sends the specification to the font rasterizer to be created/updated.

@param aSpec The linked font specification
@param aExtension The extension interface of the chosen rasterizer
@param aUpdate ETrue if the file is being updated, EFalse if it should be created

@leave KErrNotFound Attempting to update a file which does not exist
 */
void CFontStore::GenerateLinkedFontFileL(COpenFontLinkedTypefaceSpecification& aSpec, const TAny* aExtension, TBool aUpdate)
	{
	MOpenFontLinkedTypefaceExtension* linkedExt = (MOpenFontLinkedTypefaceExtension*)aExtension;

	//Generate the filename for the linked font file (excluding the extension
	//and the .) which are appended by the rasterizer.
	TFileName privPath;
	User::LeaveIfError(iFs.PrivatePath(privPath));
	TFileName fn;
	fn.Format(KLinkedFontFileFolder, KLinkedFontDrive, &privPath);
	
	//Choose the correct path to write the file to
	if (aUpdate)
		{
		fn.Append(KLinkedFontFileTempFolder);
		}
	
	// Create the directory if it does not exist
	TInt result = iFs.MkDirAll(fn);
	if (result != KErrAlreadyExists)
		{
		User::LeaveIfError(result);
		}
	
	//Append the linked font filename to the path for the rasterizer.
	fn.Append(aSpec.Name());
	
	// If we are updating, check that the file is a linked typeface before attempting the update.
	if (aUpdate)
		{
		TFileName fn;
		TBool exists = GetFontFilePath(aSpec.Name(), fn);
		
		if (exists)
			{
			COpenFontLinkedTypefaceSpecification* tempSpec = COpenFontLinkedTypefaceSpecification::NewLC(aSpec.Name());
			TRAPD(ret, linkedExt->GetLinkedTypefaceSpecificationL(fn, *tempSpec));
			User::LeaveIfError(ret);
			CleanupStack::PopAndDestroy(tempSpec);
			}
		else
			{
			User::Leave(KErrNotFound);
			}
		}
	
	//Use the rasterizer extension to create the linked typeface
	linkedExt->CreateLinkedTypefaceL(aSpec, fn);
	
	// Load the new linked typeface
	if (!aUpdate)
		{
		AddFileL(fn);
		}
	}

/**
Function called at system startup to find and load all fonts in the specified 
folder. The value of the folder will be the secure Data-caged private folder 
named "\resource\fonts". 

The first font of a given name overrides subsequent ones. 

The search order is: Y:, X:, W:, ..., C:, B:, A:, Z:

@see TFindFile 
@param aFontsDir The directory search pattern 
 */
void CFontStore::LoadFontsL(const TDesC& aFontsDir)
	{
#ifdef _DEBUG
	_LIT(KLoadStarted, "CFontStore::LoadFontsL Started.\r\n");
	RDebug::Print(KLoadStarted);
	TUint32	loadStartTime = StartTiming();
#endif
	
	RArray<TParse> fontsToLoad;
	CleanupClosePushL(fontsToLoad);
	TFindFile fileFinder(iFs);
	CDir* foundFileList = NULL;

	_LIT(KFBSERVFontFilePattern, "*");
	TInt findFileComplete = fileFinder.FindWildByDir(KFBSERVFontFilePattern, aFontsDir, foundFileList);

	while (!findFileComplete)
		{
		CleanupStack::PushL(foundFileList);
		const TInt foundFileCount = foundFileList->Count();
		const TDesC& pathInfo = fileFinder.File();
		
		// Build a list of fonts to be loaded eliminating duplicate filenames. 
		for (TInt ii = 0; ii < foundFileCount; ii++)
			{
			TParse fullFontFileName;
			if (fullFontFileName.Set((*foundFileList)[ii].iName, &pathInfo, NULL) == KErrNone)
				{
				// If the font has not already been loaded, validate it then add it to the list.
				if (!FileIsInList(fullFontFileName, fontsToLoad))
					{
					// Do not validate fonts on Z: (assumed to be ROM created with valid fonts)
					if (FileIsOnZ(fullFontFileName))
						{
						fontsToLoad.AppendL(fullFontFileName);
						}
					else
						{
						// Test font before loading
						TRAPD(err, SanityCheckFontFileL(fullFontFileName))
						if (KErrNone == err)
							{
							// Add file 
							fontsToLoad.AppendL(fullFontFileName);
							}
						else
							{
							_LIT(KCorruptFont, "CFontStore::LoadFontsL %S could not be loaded.\r\n");
							RDebug::Print(KCorruptFont, &fullFontFileName.FullName());
							}
						}
					}
				}
			}

		CleanupStack::PopAndDestroy(foundFileList);
		findFileComplete = fileFinder.FindWild(foundFileList);
		}
	
	// Load the fonts in reverse order (i.e. Z:, A: to Y:) this causes
	// fonts on Z: to take priority over fonts on A: to Y: when the internal 
	// font names are identical.
	TInt kk = fontsToLoad.Count();
	while(kk--)
		{
		TUid fontUid = KNullUid;
		
		// Load this font as a new font, and add it to the list of loaded fonts
		TRAPD(addFileError, AddSanityCheckedFontL(fontsToLoad[kk], fontUid));
		if (addFileError)
			{
			_LIT(KLoadedFont, "CFontStore::LoadFontsL failed with error %i to load font, filename: %S\r\n");
			RDebug::Print(KLoadedFont, addFileError, &(fontsToLoad[kk].FullName()));
			}
#ifdef _DEBUG
		else
			{
			// NOTE: This is to make testing of confirming what fonts are loaded a little easier as there is no 
			//		 easy way to get this information from outside the server.
			_LIT(KLoadedFont, "CFontStore::LoadFontsL loaded font filename: %S\r\n");
			RDebug::Print(KLoadedFont, &(fontsToLoad[kk].FullName()));
			}
#endif
		}
	
#ifdef _DEBUG
	TUint32 loadTime = FinishTimingL(loadStartTime);
	_LIT(KLoadEnded, "CFontStore::LoadFontsL Finished. Took: %dus\r\n");
	RDebug::Print(KLoadEnded, loadTime);
#endif
	CleanupStack::PopAndDestroy(&fontsToLoad);
	}
/**
Retrieves the full path and filename of the font with the specified name. This function
searches through all open font files currently loaded by font store.

@param aFontName The full name of the font to search for
@param aFilePath An empty descriptor to have the file name filled in

@return ETrue if the font is found; EFalse if not
*/
EXPORT_C TBool CFontStore::GetFontFilePath(const TDesC& aFontName, TFileName& aFilePath) const
	{
	TInt numTypefaces = iOpenFontFileList.Count();
	COpenFontFile* thisFile;
	TOpenFontFaceAttrib faceAttrib;
	
	//For every (Truetype) font file
	for (TInt i = 0 ; i != numTypefaces ; i++)
		{
		thisFile = iOpenFontFileList[i];
		
		//For every face within this file
		for (TInt face = 0 ; face != thisFile->FaceCount() ; face++)
			{
			faceAttrib = thisFile->FaceAttrib(face);
			if (faceAttrib.FullName().CompareF(aFontName) == 0)
				{
				aFilePath = thisFile->FileName();
				return ETrue;
				}
			}
		}
	return EFalse;	
	}

TInt CFontStore::CacheFontTable(TUid aFileUid, TUint32 aTag, 
        TAny *&aContent, TInt aLength)
    {
    return iFontTableCache->Append(aFileUid, aTag, aContent, aLength);
    }

TInt CFontStore::FindFontTableInCache(TUid aFileUid, TUint32 aTag, 
        TAny *&aContent, TInt &aLength)
    {
    TInt id;
    return iFontTableCache->Find(aFileUid, aTag, aContent, aLength, &id);
    }

TInt CFontStore::IncreaseUnhintedOutlineRefCount(const TUnhintedOutlineId &aOutlineId, 
        TInt aSessionHandle)
    {
    return iUnhintedOutlineCache->IncRefCount(aOutlineId, aSessionHandle);
    }

TInt CFontStore::IncreaseHintedOutlineRefCount(const THintedOutlineId &aOutlineId,
        TInt aSessionHandle)
    {
    return iHintedOutlineCache->IncRefCount(aOutlineId, aSessionHandle);
    }

TInt CFontStore::IncFontTableRefCount(TUid aFileUid, TUint32 aTag, 
        TInt aSessionHandle)
    {
    return iFontTableCache->IncRefCount(aFileUid, aTag, aSessionHandle);
    }

TInt CFontStore::DecFontTableRefCount(TUid aFileUid, TUint32 aTag, 
        TInt aSessionHandle)
    {
    return iFontTableCache->DecRefCount(aFileUid, aTag, aSessionHandle);
    }

TInt CFontStore::CacheUnhintedOutline(const TUnhintedOutlineId &aOutlineId, TAny * aData, 
        TInt aLength, TAny *&aOutline, TInt &aLen)
    {
    return iUnhintedOutlineCache->CacheUnhintedOutline(aOutlineId, aData, aLength, 
            aOutline, aLen);
    }

TInt CFontStore::CacheHintedOutline(const THintedOutlineId &aOutlineId,
        TAny * aData, TInt aLength, TAny *&aOutline, TInt &aLen)
    {
    return iHintedOutlineCache->CacheHintedOutline(aOutlineId, aData, aLength,
            aOutline, aLen);
    }

TInt CFontStore::ReleaseUnhintedOutline(const TUnhintedOutlineId &aOutlineId, 
        TInt aSessionHandle)
    {
    return iUnhintedOutlineCache->DecRefCount(aOutlineId, aSessionHandle);    
    }

TInt CFontStore::ReleaseHintedOutline(const THintedOutlineId &aOutlineId,
        TInt aSessionHandle)
    {
    return iHintedOutlineCache->DecRefCount(aOutlineId, aSessionHandle);    
    }

TInt CFontStore::ReleaseFontTable(TUid aFileUid, TUint32 aTag, 
        TInt aSessionHandle)
    {
    return iFontTableCache->DecRefCount(aFileUid, aTag, aSessionHandle);
    }

TInt CFontStore::FindUnhintedOutlineInCache(const TUnhintedOutlineId &aOutlineId, TAny *&aData, 
        TInt &aLength)
    {
    return iUnhintedOutlineCache->Find(aOutlineId, aData, aLength);
    }

TInt CFontStore::FindHintedOutlineInCache(const THintedOutlineId &aOutlineId, 
        TAny *&aData, TInt &aLength)
    {
    return iHintedOutlineCache->Find(aOutlineId, aData, aLength);
    }

void CFontStore::CleanupCacheOnOpenFontRemoval(COpenFont *aFont)
    {
    iHintedOutlineCache->CleanupCacheOnOpenFontRemoval(aFont);
    //iUnhintedOutlineCache.OnOpenFontRemoval(aFont);
    }

void CFontStore::CleanupCacheOnOpenFontFileRemoval(COpenFontFile *aFontFile) 
    {
    iUnhintedOutlineCache->CleanupCacheOnOpenFontFileRemoval(aFontFile);
    iFontTableCache->CleanupCacheOnOpenFontFileRemoval(aFontFile);
    }

/** Clean up font table and glyph outline caches when an FBS session is terminated.
 * All the reference counts related to that session are cleared.
@param aSession: Input. A pointer to the terminating session.
@return always returns KErrNone.
@internalTechnology
*/
EXPORT_C
void CFontStore::CleanupCacheOnFbsSessionTermination(TInt aSessionHandle)
    {
    iHintedOutlineCache->CleanupCacheOnFbsSessionTermination(aSessionHandle);
    iUnhintedOutlineCache->CleanupCacheOnFbsSessionTermination(aSessionHandle);
    iFontTableCache->CleanupCacheOnFbsSessionTermination(aSessionHandle);
    }


void TFontTableGlyphOutlineCacheMemMonitor::Inc(TInt aBytes)
    {
    iBytes += aBytes;
    }

void TFontTableGlyphOutlineCacheMemMonitor::Dec(TInt aBytes)
    {
    iBytes -= aBytes;
    }

TInt TFontTableGlyphOutlineCacheMemMonitor::GetMemUsage()
    {
    return iBytes;
    }

TFontTableGlyphOutlineCacheMemMonitor::TFontTableGlyphOutlineCacheMemMonitor(): 
    iBytes(0) 
    {
    // null constructor
    }

TUnhintedOutlineId::TUnhintedOutlineId(TUid aFileUid, TInt aFaceIndex, TUint aId):
iFileUid(aFileUid), iFaceIndex(aFaceIndex), iId(aId)
    {
    // a null constructor;
    }

THintedOutlineId::THintedOutlineId(COpenFont *aFont, TUint aId):
iFont(aFont), iId(aId)
    {
    // a null constructor;
    }

#ifdef _DEBUG
/**
Returns a timestamp for use by FinishTimingL().
@return The timestamp.
@see FinishTimingL
*/
TUint32 StartTiming(void)
	{
	return User::FastCounter();
	}

/**
Returns the time difference between a given time and now in microseconds.
@param aStartTime The start time returned by calling StartTiming()
@return The time difference in microseconds.
@see StartTiming()
*/
TUint32 FinishTimingL(TUint32 aStartTime)
	{
	TInt freq = 0;
	TUint32 endtime = User::FastCounter();
	User::LeaveIfError(HAL::Get(HALData::EFastCounterFrequency, freq));
	TUint32 diff = endtime - aStartTime;
	TInt64 diffTime = (1000000 * TInt64(diff)) / (TInt64)freq;
	return TUint32(diffTime);
	}
#endif

/**
Returns true if the filename passed in is on the Z: drive
@param aFileName The filename to check
@return ETrue if the file is on Z:
*/
TBool FileIsOnZ(TParse& aFileName)
	{
	TBool onZ = EFalse;
	
	const TDesC& fileDrive = aFileName.Drive();
	_LIT(KZDrive,"z:");
	if (0 == fileDrive.CompareF(KZDrive))
		{
		onZ = ETrue;
		}
	
	return onZ;
	}


/**
Returns true if the filename passed in is in the supplied list
@param aFileName The filename to check
@param aList The list to check
@return ETrue if the file is in the list
*/
TBool FileIsInList(TParse& aFileName, RArray<TParse>& aList)
	{
	TBool inList = EFalse;
	
	TInt jj = aList.Count();
	while(jj--)
		{
		TPtrC shortFilename = aFileName.NameAndExt();
		if (shortFilename.CompareF( aList[jj].NameAndExt() ) == 0)
			{
#ifdef _DEBUG
			_LIT(KEclipsesFont, "CFontStore::FileIsInList %S eclipses %S\r\n");
			RDebug::Print(KEclipsesFont, &aList[jj].FullName(), &aFileName.FullName());
#endif
			inList = ETrue;
			break;
			}
		}
	
	return inList;
	}
