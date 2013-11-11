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
* Symbian implementation of LEFontInstance
*
*/

// Symbian includes
#include <stdio.h>
#include <gdi.h>
#include <openfont.h>
#include <fntstore.h>
#include <string.h>

// Icu includes
#include "layout/LETypes.h"
#include "layout/LEFontInstance.h"
#include "layout/LESwaps.h"

#include "SymbianFontInstance.h"

//
// Finds the high bit by binary searching
// through the bits in n.
//
le_int8 SymbianFontInstance::highBit(le_int32 value)
	{
    if (value <= 0) 
    	{
        return -32;
    	}

    le_uint8 bit = 0;

    if (value >= 1 << 16) 
    	{
        value >>= 16;
        bit += 16;
    	}

    if (value >= 1 << 8) 
    	{
        value >>= 8;
        bit += 8;
    	}

    if (value >= 1 << 4) 
    	{
        value >>= 4;
        bit += 4;
    	}

    if (value >= 1 << 2) 
    	{
        value >>= 2;
        bit += 2;
    	}

    if (value >= 1 << 1) 
    	{
        value >>= 1;
        bit += 1;
    	}

    return bit;
	}

// This is used to build a shaper which does not require the rasteriser
// to support table access.  This is required for interworking testing.
// #define TRUETYPE_EXTENSION_NOT_SUPPORTED
#ifndef TRUETYPE_EXTENSION_NOT_SUPPORTED

	
SymbianFontInstance::SymbianFontInstance(CBitmapFont *aBitmapFont,
	LEErrorCode &status, le_bool aKeepGlyphOfZWJ)
	: fFile(NULL), fUnitsPerEM(0), fAscent(0), fDescent(0), fLeading(0),
      fDirectory(NULL), fCMAPMapper(NULL), fHMTXTable(0), fNumGlyphs(0),
      fNumLongHorMetrics(0), iFont(aBitmapFont), iKeepGlyphOfZWJ(aKeepGlyphOfZWJ)

	{
	// get the OpenFont details
	if (!aBitmapFont->IsOpenFont())
		{
		// the font supplied was not a Openfont so ..
		status = LE_ILLEGAL_ARGUMENT_ERROR;
		return;
		}

	COpenFont* font = aBitmapFont->OpenFont();

	// get the extended interface
	TAny* ext = 0;
	font->ExtendedInterface(KUidOpenFontShapingExtension, ext);
	MOpenFontShapingExtension* extensionInterface
		= reinterpret_cast<MOpenFontShapingExtension*>(ext);

	font->ExtendedInterface(KUidOpenFontTrueTypeExtension, ext);
	MOpenFontTrueTypeExtension* trueTypeExtensionInterface
		= reinterpret_cast<MOpenFontTrueTypeExtension*>(ext);

	/* Currently if the trueTypeExtensionInterface is not available the 
	truetype tables are accessed directly rather than via the rasteriser */
	if (!extensionInterface /* || !trueTypeExtensionInterface*/)
		{
		// this rasterizer does not support the required interface
		// so do not bother to shape
		status = LE_ILLEGAL_ARGUMENT_ERROR;
		return ;
		}

	iTrueTypeExtensionInterface = trueTypeExtensionInterface;
	iExtensionInterface = extensionInterface;
	
	// get the font file name from the COpenFontFile in the COpenFont
	TBuf8<KMaxFileName> fontFileName;
	fontFileName.Copy(font->File()->FileName());
	fontFileName.ZeroTerminate();

	/* get the font metrics via the rasterizer */
	MOpenFontShapingExtension::TExtensionFontMetrics fontMetrics;
	extensionInterface->GetExtensionFontMetrics(fontMetrics);

 	/* The number of font design units per em. */
	fUnitsPerEM	= fontMetrics.iUnitsPerEm;

	/* The size of the font's em square in pixels. */
	fXPixelsPerEm	= fontMetrics.iXPixelsPerEm;
	fYPixelsPerEm	= fontMetrics.iYPixelsPerEm;

	/* Scaling factors, pixels per font unit. */
	fDeviceScaleY = fontMetrics.iYScaleFactor;
	fDeviceScaleX = fontMetrics.iXScaleFactor;

	// open the font file
	fFile = fopen( (char *)fontFileName.Ptr(), "rb");
	if (fFile == 0)
		{
		status = LE_FONT_FILE_NOT_FOUND_ERROR;
		return;
		}

    // read in the directory
    SFNTDirectory tempDir;

    fread(&tempDir, sizeof tempDir, 1, fFile);

    le_int32 dirSize = sizeof tempDir + ((SWAPW(tempDir.numTables) - ANY_NUMBER) * sizeof(DirectoryEntry));
    const LETag headTag = LE_HEAD_TABLE_TAG;
    const LETag hheaTag = LE_HHEA_TABLE_TAG;
    const HEADTable *headTable = NULL;
    const HHEATable *hheaTable = NULL;
    le_uint16 numTables = 0;
    
    //coverity[incorrect_multiplication]
    //coverity[buffer_alloc]
    // dirSize is The actually sizoe of fDirectory which indeed contains more data thant SFNTDirectory defined.
    fDirectory = (const SFNTDirectory *) LE_NEW_ARRAY(char, dirSize);
   
    if (fDirectory == NULL) 
    	{
        status = LE_MEMORY_ALLOCATION_ERROR;
        goto error_exit;
    	}

    fseek(fFile, 0L, SEEK_SET);
    fread((void *) fDirectory, sizeof(char), dirSize, fFile);

    //
    // We calculate these numbers 'cause some fonts
    // have bogus values for them in the directory header.
    //
    numTables = SWAPW(fDirectory->numTables);
    fDirPower = 1 << highBit(numTables);
    fDirExtra = numTables - fDirPower;

    // read unitsPerEm from 'head' table
    headTable = (const HEADTable *) readFontTable(headTag);

    if (headTable == NULL) 
    	{
        status = LE_MISSING_FONT_TABLE_ERROR;
        goto error_exit;
    	}

    fUnitsPerEM = SWAPW(headTable->unitsPerEm);
    deleteTable(headTable);

    hheaTable = (HHEATable *) readFontTable(hheaTag);

    if (hheaTable == NULL) 
    	{
        status = LE_MISSING_FONT_TABLE_ERROR;
        goto error_exit;
    	}

    fAscent  = (le_int32) yUnitsToPoints((float) SWAPW(hheaTable->ascent));
    fDescent = (le_int32) yUnitsToPoints((float) SWAPW(hheaTable->descent));
    fLeading = (le_int32) yUnitsToPoints((float) SWAPW(hheaTable->lineGap));

    fNumLongHorMetrics = SWAPW(hheaTable->numOfLongHorMetrics);

    deleteTable((void *) hheaTable);

    fCMAPMapper = findUnicodeMapper();

    if (fCMAPMapper == NULL) 
    	{
        status = LE_MISSING_FONT_TABLE_ERROR;
        goto error_exit;
    	}

	return;
	
error_exit:
    fclose(fFile);
    fFile = NULL;
    return;
	}

SymbianFontInstance::~SymbianFontInstance()
	{
	if (fFile != NULL) 
		{
		fclose(fFile);
		deleteTable(fHMTXTable);
		delete fCMAPMapper;
		LE_DELETE_ARRAY(fDirectory);
		}
	};

#else
 	/* read the font directly.  This is used if rasterizers do not support
 	   the trueTypeExtensionInterface */
 
SymbianFontInstance::SymbianFontInstance(CBitmapFont *aBitmapFont,
 	LEErrorCode &status)
 	: fFile(NULL), fUnitsPerEM(0), fAscent(0), fDescent(0), fLeading(0),
       fDirectory(NULL), fCMAPMapper(NULL), fHMTXTable(0), fNumGlyphs(0),
       fNumLongHorMetrics(0), iFont(aBitmapFont)
 
 	{
 	// get the OpenFont details
 	if (!aBitmapFont->IsOpenFont())
 		{
 		// the font supplied was not a Openfont so ..
 		status = LE_ILLEGAL_ARGUMENT_ERROR;
 		return;
 		}
 
 	COpenFont* font = aBitmapFont->OpenFont();
 
 	// get the extended interface
 	TAny* ext = 0;
 	font->ExtendedInterface(KUidOpenFontShapingExtension, ext);
 	iExtensionInterface = reinterpret_cast<MOpenFontShapingExtension*>(ext);
 
 	// do not use the true type interface
 	iTrueTypeExtensionInterface = 0;
 
 	if (!iExtensionInterface  )
 		{
 		// this rasterizer does not support the required interface
 		// so do not bother to shape
 		status = LE_ILLEGAL_ARGUMENT_ERROR;
 		return ;
 		}
 
 	// get the font file name from the COpenFontFile in the COpenFont
 	TBuf8<KMaxFileName> fontFileName;
 	fontFileName.Copy(font->File()->FileName());
 	fontFileName.ZeroTerminate();
 
 	/* get the font metrics via the rasterizer */
 	MOpenFontShapingExtension::TExtensionFontMetrics fontMetrics;
 	iExtensionInterface->GetExtensionFontMetrics(fontMetrics);
   
    /* The number of font design units per em. */
   	fUnitsPerEM	= fontMetrics.iUnitsPerEm;
   
   	/* The size of the font's em square in pixels. */
   	fXPixelsPerEm	= fontMetrics.iXPixelsPerEm;
   	fYPixelsPerEm	= fontMetrics.iYPixelsPerEm;
   
   	/* Scaling factors, pixels per font unit. */
   	fDeviceScaleY = fontMetrics.iYScaleFactor;
   	fDeviceScaleX = fontMetrics.iXScaleFactor;
   
   	// open the font file
   	fFile = fopen( (char *)fontFileName.Ptr(), "rb");
   	if (fFile == 0)
   		{
   		status = LE_FONT_FILE_NOT_FOUND_ERROR;
   		return;
   		}
   
    // read in the directory
    SFNTDirectory tempDir;
   
    fread(&tempDir, sizeof tempDir, 1, fFile);
   
    le_int32 dirSize = sizeof tempDir + ((SWAPW(tempDir.numTables) - ANY_NUMBER) * sizeof(DirectoryEntry));
    const LETag headTag = LE_HEAD_TABLE_TAG;
    const LETag hheaTag = LE_HHEA_TABLE_TAG;
    const HEADTable *headTable = NULL;
    const HHEATable *hheaTable = NULL;
    le_uint16 numTables = 0;
    
    fDirectory = (const SFNTDirectory *) LE_NEW_ARRAY(char, dirSize);
    
    if (fDirectory == NULL) 
    	{
        status = LE_MEMORY_ALLOCATION_ERROR;
        goto error_exit;
    	}
   
    fseek(fFile, 0L, SEEK_SET);
    fread((void *) fDirectory, sizeof(char), dirSize, fFile);
   
    //
    // We calculate these numbers 'cause some fonts
    // have bogus values for them in the directory header.
    //
    numTables = SWAPW(fDirectory->numTables);
    fDirPower = 1 << highBit(numTables);
    fDirExtra = numTables - fDirPower;
   
    // read unitsPerEm from 'head' table
    headTable = (const HEADTable *) readFontTable(headTag);
   
    if (headTable == NULL) 
    	{
        status = LE_MISSING_FONT_TABLE_ERROR;
        goto error_exit;
    	}
   
    fUnitsPerEM = SWAPW(headTable->unitsPerEm);
    deleteTable(headTable);
   
    hheaTable = (HHEATable *) readFontTable(hheaTag);
   
    if (hheaTable == NULL) 
    	{
        status = LE_MISSING_FONT_TABLE_ERROR;
        goto error_exit;
    	}
   
    fAscent  = (le_int32) yUnitsToPoints((float) SWAPW(hheaTable->ascent));
    fDescent = (le_int32) yUnitsToPoints((float) SWAPW(hheaTable->descent));
    fLeading = (le_int32) yUnitsToPoints((float) SWAPW(hheaTable->lineGap));
   
    fNumLongHorMetrics = SWAPW(hheaTable->numOfLongHorMetrics);
   
    deleteTable((void *) hheaTable);
   
    fCMAPMapper = findUnicodeMapper();
   
    if (fCMAPMapper == NULL) 
    	{
        status = LE_MISSING_FONT_TABLE_ERROR;
        goto error_exit;
    	}
   
	return;
   	
   	error_exit:
       fclose(fFile);
       fFile = NULL;
       return;
   	}
   
SymbianFontInstance::~SymbianFontInstance()
   	{
       if (fFile != NULL) 
       	{
           fclose(fFile);
       
           deleteTable(fHMTXTable);
   
           delete fCMAPMapper;
   
           LE_DELETE_ARRAY(fDirectory);
       	}
   	};
   
#endif
   

void SymbianFontInstance::deleteTable(const void *table) const
	{
	LE_DELETE_ARRAY(table);
	}

const DirectoryEntry *SymbianFontInstance::findTable(LETag tag) const
	{
    if (fDirectory != NULL) 
    	{
        le_uint16 table = 0;
        le_uint16 probe = fDirPower;

        if (SWAPL(fDirectory->tableDirectory[fDirExtra].tag) <= tag) 
        	{
            table = fDirExtra;
        	}

        while (probe > (1 << 0)) 
        	{
            probe >>= 1;

            if (SWAPL(fDirectory->tableDirectory[table + probe].tag) <= tag) 
            	{
                table += probe;
            	}
        	}

        if (SWAPL(fDirectory->tableDirectory[table].tag) == tag) 
        	{
            return &fDirectory->tableDirectory[table];
        	}
    	}

    return NULL;
	}

const void *SymbianFontInstance::readTable(LETag aTag, le_uint32 *aLength) const
	{
	void *table = NULL;

	/* If the current rasteriser supports the TrueTypeExtensionInterface 
	   use it to access the true Type tables */
	if (iTrueTypeExtensionInterface)
		{
		TInt error;
		TInt length;
		TAny * trueTypeTable = iTrueTypeExtensionInterface->
			GetTrueTypeTable(error, aTag, &length);

	 	if (!trueTypeTable) 
			{
			*aLength = 0;
			return 0;
			}

		if (aLength)
			*aLength = length;

		// allocate some memory for the table & copy table into it
		table = LE_NEW_ARRAY(char, length);
		if (table) 
			{
			// copy the table from the rasteriser
			Mem::Copy(table, (char *) trueTypeTable, length);
			}
	  
	  	iTrueTypeExtensionInterface->ReleaseTrueTypeTable(trueTypeTable);
  	  	}
  	else
  	  	{
		// old method, read the table directly
		const DirectoryEntry *entry = findTable(aTag);

		if (entry == NULL) 
			{
			if (aLength)
				*aLength = 0;
			return NULL;
			}

		TInt length = SWAPL(entry->length);
		if (aLength)
			*aLength = length;

		table = LE_NEW_ARRAY(char, length);

		if (table != NULL) 
			{
			fseek(fFile, SWAPL(entry->offset), SEEK_SET);
			fread(table, sizeof(char), length, fFile);
			}
		}
	    
    return table;
    }
    
const void *SymbianFontInstance::getFontTable(LETag tableTag) const
	{
	if (iTrueTypeExtensionInterface)
		{
		TInt error;
		return iTrueTypeExtensionInterface->GetTrueTypeTable(error, tableTag, 0);
		}
	return FontTableCache::find(tableTag);
	}

const void *SymbianFontInstance::readFontTable(LETag tableTag) const
	{
	return readTable(tableTag, 0);
	}

CMAPMapper *SymbianFontInstance::findUnicodeMapper()
	{
	LETag cmapTag = LE_CMAP_TABLE_TAG;
	const CMAPTable *cmap = (CMAPTable *) readFontTable(cmapTag);

	return cmap ? CMAPMapper::createUnicodeMapper(cmap) : NULL;
	}

void SymbianFontInstance::getGlyphAdvance(
	LEGlyphID aGlyph, LEPoint &aAdvance) const
	{
	TInt glyph = LE_GET_GLYPH(aGlyph) | 0x80000000;
	TOpenFontCharMetrics metrics;
	const TUint8* bitmap = 0;
	
	// If an FFFF glyph code is received, avoid trying to rasterize it as there
	// is no character data associated with FFFF.
	// This will avoid the overhead of trying to rasterize it.
	if (glyph == 0x8000FFFF)
		{
		aAdvance.fX = 0;
		aAdvance.fY = 0;
		return;
		}
		
	if (!iFont->GetCharacterData(iSessionHandle, glyph, metrics, bitmap))
		{
		// Glyph not yet rasterized; rasterize it ourselves
		iFont->Rasterize(iSessionHandle, glyph, 0);
		if (!iFont->GetCharacterData(iSessionHandle, glyph, metrics, bitmap))
			{
			aAdvance.fX = 0;
			aAdvance.fY = 0;
			return;
			}
		}
	aAdvance.fX = metrics.HorizAdvance();
	aAdvance.fY = 0;
	return;
	}

le_bool SymbianFontInstance::getGlyphPoint(LEGlyphID glyph, le_int32 pointNumber, LEPoint &point) const
	{
	TReal x;
	TReal y;
	if (!iExtensionInterface->GlyphPointInHintedPixels(
		glyph, pointNumber, x, y ))
		return FALSE;
	point.fX = x;
	point.fY = y;
	return TRUE;
	}

void SymbianFontInstance::transformFunits(float xFunits, float yFunits, LEPoint &pixels) const
	{
	pixels.fX = xFunits / fUnitsPerEM * fXPixelsPerEm;
	pixels.fY = yFunits / fUnitsPerEM * fYPixelsPerEm;
	}

float SymbianFontInstance::xUnitsToPoints(float xUnits) const
	{
	// Seems like this function should have been called xUnitsToPixels?!
	return xUnits / fUnitsPerEM * fXPixelsPerEm;
	}

float SymbianFontInstance::yUnitsToPoints(float yUnits) const
	{
	// Seems like this function should have been called yUnitsToPixels?!
	return yUnits / fUnitsPerEM * fYPixelsPerEm;
	}

float SymbianFontInstance::getXPixelsPerEm() const
	{
	return fXPixelsPerEm;
	}

float SymbianFontInstance::getYPixelsPerEm() const
	{
	return fYPixelsPerEm;
	}

float SymbianFontInstance::getScaleFactorX() const
	{
	return fDeviceScaleX;
	}

float SymbianFontInstance::getScaleFactorY() const
	{
	return fDeviceScaleY;
	}
