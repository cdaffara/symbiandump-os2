/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* SymbianFontInstance.CPP
* Symbian implementation of LEFontInstance
*
*/


#ifndef SYMBIANFontInstance_H_
#define SYMBIANFontInstance_H_

#include <stdio.h>

#include "layout/LETypes.h"
#include "layout/LEFontInstance.h"

#include "FontTableCache.h"

#include "sfnt.h"
#include "cmaps.h"

// Symbian includes
#include <gdi.h>

class MOpenFontExtensionInterface;

class SymbianFontInstance : public LEFontInstance, protected FontTableCache
	{
public:
	SymbianFontInstance(CBitmapFont* aBitmapFont, LEErrorCode &status, le_bool aKeepGlyphOfZWJ = false);
	virtual ~SymbianFontInstance();
	void SetSessionHandle(TInt aHandle) 
		{
		iSessionHandle = aHandle;
		}

	const void *getFontTable(LETag tableTag) const;

	le_int32 getUnitsPerEM() const
		{
		return fUnitsPerEM;
		}

	le_int32 getAscent() const
		{
		return fAscent;
		}

	le_int32 getDescent() const
		{
		return fDescent;
		}

	le_int32 getLeading() const
		{
		return fLeading;
		}

	LEGlyphID mapCharToGlyph(LEUnicode32 ch) const
		{
		// 1922 mlyl -->
		if (ch == 0x200D && !iKeepGlyphOfZWJ)
			return 1;
		else
		// <-- 1922 mlyl
			return fCMAPMapper->unicodeToGlyph(ch);
		}

	void getGlyphAdvance(LEGlyphID glyph, LEPoint &advance) const;

	le_bool getGlyphPoint(LEGlyphID glyph, le_int32 pointNumber, LEPoint &point) const;

	void transformFunits(float xFunits, float yFunits, LEPoint &pixels) const;
	float xUnitsToPoints(float xUnits) const;
	float yUnitsToPoints(float yUnits) const;
	float getXPixelsPerEm() const;
	float getYPixelsPerEm() const;
	float getScaleFactorX() const;
	float getScaleFactorY() const;

protected:
	const void *readFontTable(LETag tableTag) const;

private:
	FILE *fFile;	

	le_int32 fUnitsPerEM;
	le_int32 fAscent;
	le_int32 fDescent;
	le_int32 fLeading;

	const SFNTDirectory *fDirectory;
	le_uint16 fDirPower;
	le_uint16 fDirExtra;

	float fXPixelsPerEm;
	float fYPixelsPerEm;

	float fDeviceScaleX;
	float fDeviceScaleY;

	CMAPMapper *fCMAPMapper;

	const HMTXTable *fHMTXTable;
	le_uint16 fNumGlyphs;
	le_uint16 fNumLongHorMetrics;

	CBitmapFont* iFont;
	MOpenFontShapingExtension* iExtensionInterface;
	MOpenFontTrueTypeExtension* iTrueTypeExtensionInterface;
	TInt iSessionHandle;
	
	// --> 1922 mlyl: The script which this shaper will shape
	le_bool iKeepGlyphOfZWJ;
	// <-- 1922 mlyl

	static le_int8 highBit(le_int32 value);

	const DirectoryEntry *findTable(LETag tag) const;
	const void *readTable(LETag tag, le_uint32 *length) const;
	void deleteTable(const void *table) const;
	void getMetrics();

	CMAPMapper *findUnicodeMapper();
	};

#endif
