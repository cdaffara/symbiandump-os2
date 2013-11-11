/*
* Copyright (c) 1996-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __FNTBODY_H__
#define __FNTBODY_H__

#include <e32std.h>
#include "FNTSTD.H"

/**
@internalComponent
*/

const TInt KFontStoreFileUidVal = 0x10000039;
const TInt KCBitmapFontUidVal = 0x10000043;
const TInt KFnttranVersion = 42;
const TInt KFnttran7650Version = 39;

class CDirectFileStore;
NONSHARABLE_CLASS(CFontStoreFile) : public CBase
/**
@internalComponent
*/
	{
protected:
	CFontStoreFile();
	void ConstructL(const TParse& aParse, RFs& aFs);
public:
	static CFontStoreFile* NewL(const TParse& aParse, RFs& aFs);
	~CFontStoreFile();
	inline const TDesC& FullName() const;
public:
	TUid iCollectionUid;
	TInt iKPixelAspectRatio;
	TInt iUsageCount;
	CDirectFileStore* iFileStore;
	TInt iFileAddress;
	TStreamId iDataStreamId;
	TInt iFontVersion;
	HBufC* iFullName;
	};

class TBitmapCodeSection : public TCodeSection
/**
@internalComponent
*/
	{
public:
	TBitmapCodeSection();
	void InternalizeL(RReadStream& aStream);
	void InternalizeOffsetsL(const CStreamStore& aStreamStore, RHeap* aHeap, TInt& aAllocMemCounter);
	void InternalizeBitmapL(const CStreamStore& aStreamStore, RHeap* aHeap, TInt& aAllocMemCounter);
	void FixUpComponents(TInt aFileAddress);
	void DeleteOffsets(RHeap* aHeap);
	void DeleteBitmap(RHeap* aHeap);
	TBitmapFontCharacterOffset* CharacterOffsetsList(TBool aIsInRAM) const;
	TUint8* Bitmap(TBool aIsInRAM) const;

private:
	union TCharacterData
		{
		TCharacterData() : iOffsetsId(KNullStreamIdValue) {}
		TUint32 iOffsetsId;
		TInt iCharacterOffsetsListOffset;
		} iCharacterData;
	//iCharacterData is interpreted either as StreamId(casted from TUint32) or as TInt offset;
	union TBitmapData
		{
		TBitmapData() : iBitmapId(KNullStreamIdValue) {}
		TUint32 iBitmapId;
		TInt iBitmapOffset;
		} iBitmapData;
	//iBitmapData is interpreted either as StreamId(casted from TUint32) or as TInt offset;
	//The size of the original TBitmapCodeSection was 24 bytes.
	//Using the unions above (and removing RHeap* member) we reduce TBitmapCodeSection size 
	//to be 12 bytes per instance.
	//It is possible to do that because iCharacterData and iBitmapData are treated as IDs during 
	//initialization stage and as offsets during all the time after initialization.
	//So we can use the same memory for IDs and for offsets.
	};

class TCharacterMetricsTable
/**
Class to store the list of Character Metrics used in this font
@since 7.0s
@internalComponent
*/
	{
public:
	TCharacterMetricsTable(RHeap* aHeap);
	void InternalizeL(RReadStream& aStream);
	void InternalizeMetricsL(RReadStream& aStream);
	void RestoreL(const CStreamStore& aStreamStore);
	void FixUp(TInt aFileAddress);
	void Delete();
	const TBitmapFontCharacterMetrics* Metric(TInt aIndex) const;
	TInt NumberOfMetrics() const;
private:
	TBitmapFontCharacterMetrics* MetricsFromOffset(TInt aIndex) const;
private:
	RHeap* iHeap;
	TStreamId iMetricsStartId;
    // Sometimes this address is on the metrics heap and sometimes in a ROM file
    // If the address is on the heap, then this is actually an offset from
    // the address of this class.
    TInt iCharacterMetricsStartPtr;	
	TInt iNumberOfMetrics;
	TBool iMetricsOnHeap;
	};

NONSHARABLE_CLASS(CFontBitmap) : public CBase
/**
@internalComponent
*/
	{
public:
	CFontBitmap(RHeap* aHeap,CFontStoreFile* aFontStoreFile);
	void InternalizeL(RReadStream &aStream, TInt aFontversion);
	void UseL();
	void Release();
	TBitmapFontCharacterMetrics CharacterMetrics(TInt aCode,const TUint8*& aBytes) const;
	void operator delete(TAny*);
	inline void operator delete(TAny*, TAny*) {}
	void SetPosture(TFontPosture aPosture);
	TFontPosture Posture() const;
	void SetStrokeWeight(TFontStrokeWeight aStrokeWeight);
	TFontStrokeWeight StrokeWeight() const;
	void SetIsProportional(TBool aIsProportional);
	TBool IsProportional() const;
	CFontStoreFile* FontStoreFile() const;
	inline TInt FontCapitalAscent() const;
	inline TInt FontMaxAscent() const;
	inline TInt FontStandardDescent() const;
	inline TInt FontMaxDescent() const;
	inline TInt FontLineGap() const;
	inline TInt FontMaxHeight() const;
protected:
	~CFontBitmap();
	void RestoreComponentsL();	 //  Has reference to FontStoreFile
	void DeleteComponents();
	TBitmapCodeSection* CodeSectionList() const;
	TCharacterMetricsTable* MetricsTable() const;
public:
	RHeap* iHeap;
	TInt iFontStoreFileOffset;
	TUid iUid;
protected:
	TInt8 iPosture;
	TInt8 iStrokeWeight;
	TInt8 iIsProportional;
	TBool iIsInRAM;
	TInt iUsageCount;
public:
	TInt8 iCellHeightInPixels;
	TInt8 iAscentInPixels;
	TInt8 iMaxCharWidthInPixels;
	TInt8 iMaxNormalCharWidthInPixels;
	TInt iBitmapEncoding;
	TInt iNumCodeSections;
	TInt iCodeSectionListOffset;
private:
	TCharacterMetricsTable iCharacterMetricsTable;
	TBool iComponentsRestored;
	TInt iAllocMemCounter_Offsets;
	TInt iAllocMemCounter_Bitmaps;
	TInt iFontCapitalAscent;		// The positive distance in pixels from the baseline to
									// the top of an ANSI capital (whether or not there are
									// ANSI capitals in the font)
	TInt iFontMaxAscent;			// The positive distance in pixels from the baseline to
									// the top of the highest pre-composed glyph in the font
	TInt iFontStandardDescent;		// The positive distance in pixels from the baseline to
									// the bottom of the lowest ANSI descender (whether or
									// not there are ANSI chars in the font)
	TInt iFontMaxDescent;			// The positive distance in pixels from the baseline to
									// the bottom of the lowest pre-composed glyph in the font
	TInt iFontLineGap;				// The recommended baseline to baseline gap for successive
									// lines of text in the font
	};

class TTypefaceFontBitmap
/**
@internalComponent
*/
	{
public:
	TTypefaceFontBitmap();
	TTypefaceFontBitmap(TTypeface* aTypeface,CFontBitmap* aFontBitmap);
	TInt HeightInPixels() const;
public:
	TTypeface* iTypeface;
	CFontBitmap* iFontBitmap;
	TInt8 iHeightFactor;
	TInt8 iWidthFactor;
	};
	

/** Gets the ascent of an ANSI capital letter in the font whether or not
there are any ANSI capitals in the font.
@return The positive distance from the font baseline to the top of a
standard ANSI capital letter
@internalComponent
@released
@see AscentInPixels()
@see FontMaxAscent()
*/
inline TInt CFontBitmap::FontCapitalAscent() const
	{
	return iFontCapitalAscent;
	}

/** Gets the max ascent of any pre-composed glyph in the font. This will
include accents or diacritics that form part of pre-composed glyphs. It is
not guaranteed to cover the max ascent of composite glyphs that have to be
created by a layout engine. This is also the recommended distance between
the top of a text box and the baseline of the first line of text. 
@return The positive distance from the font baseline to the top of the
highest pre-composed glyph (including accents) above the baseline
@internalComponent
@released
@see AscentInPixels()
@see FontCapitalAscent()
*/
inline TInt CFontBitmap::FontMaxAscent() const
	{
	return iFontMaxAscent;
	}

/** Gets the descent of an ANSI descending character in the font.
Whether or not there are any ANSI descenders in the font.
@return The positive distance from the font baseline to the bottom of the
lowest ANSI descender
@internalComponent
@released
@see DescentInPixels()
*/
inline TInt CFontBitmap::FontStandardDescent() const
	{
	return iFontStandardDescent;
	}

/** Gets the max descent of any pre-composed glyph in the font. This will
include accents or diacritics that form part of pre-composed glyphs. It is
not guaranteed to cover the max descent of composite glyphs that have to be
created by a layout engine.
@return The positive distance from the font baseline to the bottom of the
lowest pre-composed glyph (including accents) below the baseline
@internalComponent
@released
@see DescentInPixels()
*/
inline TInt CFontBitmap::FontMaxDescent() const
	{
	return iFontMaxDescent;
	}

/** Gets the suggested line gap for the font. This is the recommended
baseline to baseline distance between successive lines of text in the font.
@return The positive recommended gap between successive lines
@internalComponent
@released
*/
inline TInt CFontBitmap::FontLineGap() const
	{
	return iFontLineGap;
	}

/** Gets the maximum height for the font. This is the sum of the max ascent 
of the font and the max descent of the font.
@return The positive maximum height of the font
@internalComponent
@released
@see HeightInPixels()
*/
inline TInt CFontBitmap::FontMaxHeight() const
	{
	return iFontMaxAscent + iFontMaxDescent;
	}


/** Full filename that the bitmap font was loaded from.
@internalComponent
*/
inline const TDesC& CFontStoreFile::FullName() const
	{
	return *iFullName;
	}


#endif // __FNTBODY_H__
