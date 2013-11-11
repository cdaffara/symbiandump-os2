/*
* Copyright (c) 2003-2010 Nokia Corporation and/or its subsidiary(-ies).
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


#include <fntstore.h>
#include <gdi.h>
#include "FNTSTD.H"
#include <graphics/shapeimpl.h>
#include "ShaperCache.H"
#include "openfontsprivate.h"
#include <linkedfonts.h>
#include "linkedfontsprivate.h"
#include <graphics/openfontrasterizer.h>
#include <graphics/gdi/glyphsample.h>
#include <e32math.h>

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "OPENFONTTraces.h"
#endif


const TInt KSessionCacheEntries = 512;
const TInt KDefaultSlantFactor = 20480;
const TInt KOneIn16Dot16FixedPointFormat = 65536;

template<class T>
ROffsetArray<T>::ROffsetArray()
    : iOffset(0), iCount(0)
    {}

template<class T>
TInt ROffsetArray<T>::Create(RHeap* aHeap, TInt aCount)
    {
    if (iOffset != 0)
        {
        return KErrAlreadyExists;
        }
    TAny* p = aHeap->AllocZ(aCount * sizeof(TInt));
    if (p == NULL)
        {
        return KErrNoMemory;
        }
    iOffset = (TInt)p - (TInt)this;
    iCount = aCount;
    return KErrNone;
    }

template<class T>
void ROffsetArray<T>::Close(RHeap* aHeap)
    {
    if (iOffset != 0)
        {
        aHeap->Free(PtrAdd(this, iOffset));
        }
    iOffset = 0;
    iCount = 0;
    }

template<class T>
TInt ROffsetArray<T>::Count() const
    {
    return iCount;
    }

template<class T>
T* ROffsetArray<T>::operator[](TInt aIndex) const
    {
    TInt e = ((TInt*)PtrAdd(this, iOffset))[aIndex];
    return e != 0 ? (T*)PtrAdd(this, e) : NULL;
    }

template<class T>
void ROffsetArray<T>::SetAt(TInt aIndex, T* aEntry)
    {
    ((TInt*)PtrAdd(this, iOffset))[aIndex] = aEntry ? (TInt)aEntry - (TInt)this : 0;
    }


/*COpenFontGlyphCache*/

TShapeHeader* COpenFontGlyphCache::SearchShaperCache(TInt aSessionHandle, TFontShapeFunctionParameters*& aParams)
	{
	if (iShaperCacheSentinel == NULL)
		return NULL;
	COpenFontShaperCacheEntry* searchNode = iShaperCacheSentinel->iNext;
	
	TInt start = aParams->iStart;
	TInt end = aParams->iEnd;
	TInt script = aParams->iScript;
	const TUint16* text = (TUint16*)aParams->iText->Ptr();
	TUint16* cachedText;
	const TUint16* copyOfText;
	TInt noOfChars = end - start;
	TInt textIsSame = 1;
	while (searchNode != iShaperCacheSentinel)
		{
		//Future work: add Script check for further script support
		//Future work: add Language check for further language support
		if ((searchNode->iEnd == end) && (searchNode->iStart == start) && (searchNode->iScript == script))
			{
			// Check for the entire text (within the context) coming in.
			TInt i = 0;
			copyOfText = text + start;
			cachedText = (TUint16*)searchNode->iText + searchNode->iStart;
			textIsSame = 1;
			while (i < noOfChars && textIsSame != 0)
				{
				if (*cachedText != *copyOfText)
					textIsSame = 0;
				i++;
				copyOfText++;
				cachedText++;
				};

			if (textIsSame)
				{
				if (searchNode == iShaperCacheSentinel->iNext)
					{
					// now we need to update the reference count here for that session
					if (searchNode->IncRefCount(aSessionHandle) != KErrNone)
						return NULL;
					return iShaperCacheSentinel->iNext->iShapeHeader;
					}
				// We have found a node, now put that node to the top of the list as the most recently used node
				searchNode->iPrevious->iNext = searchNode->iNext;
				searchNode->iNext->iPrevious = searchNode->iPrevious;
				
				searchNode->iNext = iShaperCacheSentinel->iNext;
				iShaperCacheSentinel->iNext->iPrevious = searchNode;
				iShaperCacheSentinel->iNext = searchNode;
				searchNode->iPrevious = iShaperCacheSentinel;
				if (searchNode->IncRefCount(aSessionHandle)!= KErrNone)
					return NULL;
				return searchNode->iShapeHeader;
				}
			}
			searchNode = searchNode->iNext;
		}
	return NULL;
	}

TShapeHeader* COpenFontGlyphCache::Insert(TInt aSessionHandle, RHeap* aHeap, CShaper::TInput aInput, TShapeHeader* aShapeHeader, TInt& aAddedBytes)
	{
	TInt heapSizeBefAloc = 0;
	aHeap->AllocSize(heapSizeBefAloc);

	COpenFontShaperCacheEntry* new_entry;
	new_entry = COpenFontShaperCacheEntry::New(aHeap, aInput, aShapeHeader);
	if (new_entry != NULL)
		{
		// increase the reference count for this
		TInt ret=new_entry->IncRefCount(aSessionHandle);
		if (ret != KErrNone)
			{
			//no memory here
			COpenFontShaperCacheEntry::Delete(aHeap, new_entry);
			return NULL;
			}
		new_entry->iNext = iShaperCacheSentinel->iNext;
		iShaperCacheSentinel->iNext->iPrevious = new_entry;
		iShaperCacheSentinel->iNext = new_entry;
		new_entry->iPrevious = iShaperCacheSentinel;

		iNumberOfShaperCacheEntries++;
		TInt heapSizeOnAloc = 0;
		aHeap->AllocSize(heapSizeOnAloc);
		aAddedBytes = heapSizeOnAloc - heapSizeBefAloc;

		// Update the amount of memory used in creation of a new entry
		iShapingInfoCacheMemory += heapSizeOnAloc - heapSizeBefAloc;

		return new_entry->iShapeHeader;
		}
	return NULL;
	}

/**
Searches from the least recently used towards the most recently used
and try to free entry that is no longer referenced
*/
TInt COpenFontGlyphCache::DeleteLeastRecentlyUsedEntry(RHeap* aHeap)
	{
	// start from the least recently used
	COpenFontShaperCacheEntry* deleteNode = iShaperCacheSentinel->iPrevious;

	// if empty list there is nothing to delete
	if (deleteNode == iShaperCacheSentinel)
		return 0;
	// else navigating starting from the LRU entry
	while (deleteNode != iShaperCacheSentinel)
		{
		// cannot delete if the iHandleRefCount is greater than zero
		if (deleteNode->iHandleRefCount>0)
			{
			deleteNode = deleteNode->iPrevious;
			continue;
			}

		// otherwise we can delete the entry
		deleteNode->iPrevious->iNext = deleteNode->iNext;
		deleteNode->iNext->iPrevious = deleteNode->iPrevious;

		TInt heapSizeBeforeDel = 0;
		TInt heapSizeAfterDel = 0;
		aHeap->AllocSize(heapSizeBeforeDel);
		COpenFontShaperCacheEntry::Delete(aHeap, deleteNode);
		aHeap->AllocSize(heapSizeAfterDel);
		TInt deletedBytes = heapSizeBeforeDel - heapSizeAfterDel;

		iNumberOfShaperCacheEntries--;
		iShapingInfoCacheMemory -= deletedBytes;

		return deletedBytes;
		}
	// we have navigated through the whole list and cannot delete anything
	return 0;
	}

TInt COpenFont::DecrementCachedRefCount(TInt aSessionHandle, TShapeHeader* aShapeHeader, TBool aResetAll)
	{
	COpenFontShaperCacheEntry* ptr=NULL;
	COpenFontGlyphCache* glyphCache=GetGlyphCache();
	if (glyphCache != NULL)
		ptr=glyphCache->iShaperCacheSentinel;
	if (ptr == NULL)
		return KErrNone;

	TInt ret = KErrNotFound;
	CFontStore* thisFontStore = File()->GetFontStore();
	TInt allocBefDec = 0;
	TInt allocAfterDec = 0;
	TInt deletedBytes = 0;

	// loop through the cache entry to decrement the ref count for a particular session	
	while (ptr->iNext != NULL)
		{
		if (aResetAll)
			{
			// we want to reset any cache that has a matching the session handle
			// i.e. here we dont care about which TShapeHeader and hence we can
			// ignore the error code here if not found

			// Always update the memory usage of the cache as decreasing the ref count 
			// releases memory
			iHeap->AllocSize(allocBefDec);

			ptr->DecRefCount(aSessionHandle, ETrue);

			iHeap->AllocSize(allocAfterDec);
			deletedBytes = allocBefDec - allocAfterDec;
			glyphCache->iShapingInfoCacheMemory -= deletedBytes;
			thisFontStore->SetShaperCacheMemUsage(thisFontStore->GetShaperCacheMemUsage() - deletedBytes);

			ret=KErrNone;
			}
		else if (ptr->iShapeHeader != NULL && ptr->iShapeHeader==aShapeHeader)
			{
			// Always update the memory usage of the cache as decreasing the ref count 
			// releases memory
			iHeap->AllocSize(allocBefDec);

			ptr->DecRefCount(aSessionHandle);

			iHeap->AllocSize(allocAfterDec);
			deletedBytes = allocBefDec - allocAfterDec;
			glyphCache->iShapingInfoCacheMemory -= deletedBytes;
			thisFontStore->SetShaperCacheMemUsage(thisFontStore->GetShaperCacheMemUsage() - deletedBytes);

			return KErrNone;
			}
		ptr=ptr->iNext;
		if (ptr == glyphCache->iShaperCacheSentinel)
			{
			break;
			}
		}
	return ret;
	}

TInt COpenFont::FreeShaperCacheMemory(TInt aBytesNeeded)
	{
	TInt totalDeletedBytes = 0;
	TInt tempDeletedBytes = 0;
	CFontStore* thisFontStore = File()->GetFontStore();

	if (aBytesNeeded <= KMaxShaperSesssionCacheMemory)
		{
		// delete LRU entries from all the caches except the one owned by this COpenFont
		// The 'if' condition here is to avoid looping through every font in the system
		// if only one of the them has a non-empty cache. In situations where only one
		// cache is left and it is full, this strategy makes freeing the memory faster.
		if (thisFontStore->GetNumShaperCaches() > 1)
			{
			CArrayPtrFlat<COpenFont>* fontList;
			CArrayPtrFlat<COpenFontFile>* fontFileList = thisFontStore->GetOpenFontFileList();
			TInt numberOfFontFiles = fontFileList->Count();
			TInt i = 0; 
			while ((totalDeletedBytes < aBytesNeeded) && (i < numberOfFontFiles))
				{
				fontList = (*fontFileList)[i]->GetOpenFontList();
				TInt fontListCount=fontList->Count();
				TInt j = 0;
				while ((totalDeletedBytes < aBytesNeeded) && (j < fontListCount))
					{
					COpenFont* open_font = (*fontList)[j];
					COpenFontGlyphCache* glyphCache = open_font->GetGlyphCache();
					if ((open_font != this) && (glyphCache != NULL))
						{
						while ((totalDeletedBytes < aBytesNeeded) && (!glyphCache->ShaperCacheIsEmpty()))
							{
							totalDeletedBytes += glyphCache->DeleteLeastRecentlyUsedEntry(iHeap);
							if (glyphCache->ShaperCacheIsEmpty())
								{
								thisFontStore->DecNumShaperCaches();
								}
								
							// If totalDeletedBytes is zero mean we cannot delete from this font
							if (totalDeletedBytes == 0)
								{
								break;
								}
							}
						}
					j++;
					}
				i++;
                }
            }

        // If deleted bytes are still less than the required one delete from this font
        COpenFontGlyphCache* glyphCache = GetGlyphCache();
        if (glyphCache != NULL)
            {
            while (totalDeletedBytes < aBytesNeeded && !glyphCache->ShaperCacheIsEmpty())
                {
                tempDeletedBytes = glyphCache->DeleteLeastRecentlyUsedEntry(iHeap);
                if (tempDeletedBytes == 0)
                    break;
                totalDeletedBytes += tempDeletedBytes;
                }
            }   //if(glyphCache)
        }   //if(aBytesNeeded <= KMaxShaperSesssionCacheMemory)
		
	// Update the global CFontStore cache memory count
	if (totalDeletedBytes > 0)
		{
		thisFontStore->SetShaperCacheMemUsage(thisFontStore->GetShaperCacheMemUsage() - totalDeletedBytes);
		}

	return totalDeletedBytes;
	}

TShapeHeader* COpenFont::InsertShapedDataIntoCache(TInt aSessionHandle,TFontShapeFunctionParameters* aParams, TShapeHeader* aShapeHeader)
	{
	CShaper::TInput input;
	input.iEnd = aParams->iEnd;
	input.iStart = aParams->iStart;
	input.iScript = aParams->iScript;
	input.iLanguage = aParams->iLanguage;
	input.iText = aParams->iText;
	input.iMaximumAdvance = KMaxTInt;
	input.iFlags = 0;
	input.iSessionHandle = aSessionHandle;
	input.iReserved1 = 0;

	CFontStore* thisFontStore = File()->GetFontStore();
	
	// Create the glyph cache if it doesn't already exist.
    // This call can only come from FBSERV
    COpenFontGlyphCache* glyphCache = GetGlyphCache();
    if (glyphCache == NULL)
        {
        glyphCache = (COpenFontGlyphCache*) iHeap->Alloc(sizeof(COpenFontGlyphCache));
        if (glyphCache == NULL) // no memory
            {
            return NULL;
            }
        new (glyphCache) COpenFontGlyphCache(iHeap);
        SetGlyphCache(glyphCache);
        }
    // If there is no sentinel present, i.e. new cache
    if (glyphCache->iShaperCacheSentinel == NULL)
        {
        // Create a sentinel
        glyphCache->iShaperCacheSentinel = COpenFontShaperCacheEntry::New(iHeap);
        if (glyphCache->iShaperCacheSentinel == NULL)
            {
            // no memory
            return NULL;
            }
        glyphCache->iShaperCacheSentinel->iNext = glyphCache->iShaperCacheSentinel;
        glyphCache->iShaperCacheSentinel->iPrevious = glyphCache->iShaperCacheSentinel;
        glyphCache->iNumberOfShaperCacheEntries = 1;
        }

    // Before inserting into this cache, check if it was empty.
    // If empty, then increment the global cache count signifying one more cache is active
    if (glyphCache->ShaperCacheIsEmpty())
        {
        thisFontStore->IncNumShaperCaches();
        }

	TInt addedBytes = 0;
	TShapeHeader* cached_header = NULL;

	// Insert a new entry and return the newly inserted TShapeHeader entry
	cached_header = glyphCache->Insert(aSessionHandle, iHeap, input, aShapeHeader, addedBytes);
	if (cached_header == NULL)
		return NULL;

	// If the memory used by all the caches is greater than KMaxShaperSesssionCacheMemory, then
	// free some memory by releasing the same amount of memory that was just added
	if (thisFontStore->GetShaperCacheMemUsage() + addedBytes > KMaxShaperSesssionCacheMemory)
		{
		FreeShaperCacheMemory(addedBytes);
		}

	// Now update the memory count with the added memory for the new entry
	thisFontStore->SetShaperCacheMemUsage(thisFontStore->GetShaperCacheMemUsage() + addedBytes);
	return cached_header;
	}

TShapeHeader* COpenFont::GetShapedData(TInt aSessionHandle, TFontShapeFunctionParameters* aParams)
	{
    COpenFontGlyphCache* glyphCache = GetGlyphCache();
	if (glyphCache == NULL)
		return NULL;

	TShapeHeader* cachedHeader = NULL;
	CFontStore* thisFontStore = File()->GetFontStore();

	// Always update the memory usage of the cache as increasing the reference count of a found header uses up memory
	TInt allocBefInc = 0;
	TInt allocAfterInc = 0;
	iHeap->AllocSize(allocBefInc);

	cachedHeader = glyphCache->SearchShaperCache(aSessionHandle,aParams);
	
	iHeap->AllocSize(allocAfterInc);
	TInt addedBytes = allocAfterInc - allocBefInc;
	glyphCache->iShapingInfoCacheMemory += addedBytes;
	thisFontStore->SetShaperCacheMemUsage(thisFontStore->GetShaperCacheMemUsage() + addedBytes);

	return cachedHeader;
	}

TBool COpenFontGlyphCache::ShaperCacheIsEmpty()
	{
	if (iShaperCacheSentinel == NULL)
		return ETrue;

	if (iShaperCacheSentinel->iNext == iShaperCacheSentinel)
		return ETrue;
	else
		return EFalse;
	}

/**
C++ constructor taking shared heap, session cache list and font file as parameters.

You must either use this, or the other constructor, when creating your derived 
object. This constructor might be used, in preference to the other, if there 
is only a single typeface in the font file.

@param aHeap The shared heap. 
@param aSessionCacheList The session cache list.
@param aFile A pointer to the COpenFontFile object creating this COpenFont. 
e.g. when creating a COpenFont the COpenFontFile derived object would pass 
it this.
*/
EXPORT_C COpenFont::COpenFont(RHeap* aHeap,COpenFontSessionCacheList* aSessionCacheList,
							  COpenFontFile* aFile):
	iHeap(aHeap),
	iShaper(NULL),
	iFaceIndex(0)
	{	
    SetFile(aFile);
    SetSessionCacheList(aSessionCacheList);
	}

/**
C++ constructor taking shared heap, session cache list, font file and face 
index as parameters.

You must either use this, or the other constructor, when creating your derived 
object. This constructor would be used if the font file contains more than 
one typeface.

@param aHeap The shared heap.
@param aSessionCacheList The session cache list.
@param aFile A pointer to the COpenFontFile object creating this COpenFont. 
e.g. when creating a COpenFont the COpenFontFile derived object would pass 
it this.
@param aFaceIndex The index of the typeface within the font file aFile.
*/
EXPORT_C COpenFont::COpenFont(RHeap* aHeap,COpenFontSessionCacheList* aSessionCacheList, COpenFontFile* aFile,TInt aFaceIndex) :
 	iHeap(aHeap),
	iShaper(NULL),
	iFaceIndex(aFaceIndex)
	{
    SetFile(aFile);
    SetSessionCacheList(aSessionCacheList);
	}

/**
Destructor

This function frees all memory owned by the object, including the session 
cache list and the glyph list, prior to its destruction.
*/
EXPORT_C COpenFont::~COpenFont()
	{
	//Delete the shaper
	delete iShaper;

	File()->GetFontStore()->CleanupCacheOnOpenFontRemoval(this);
	
	COpenFontGlyphCache* glyphCache = GetGlyphCache();
	if (glyphCache != NULL)
		{
		glyphCache->iGlyphTreeById.ResetAndDestroy();
		glyphCache->iGlyphTreeByUnicode.ResetAndDestroy();

		// Delete the shaper cache as well
		if (glyphCache->iShaperCacheSentinel)
			{
			COpenFontShaperCacheEntry* previous  = NULL;
			COpenFontShaperCacheEntry* si = glyphCache->iShaperCacheSentinel->iPrevious;
			TInt heapBefore = 0;
			TInt heapAfter = 0;
			iHeap->AllocSize(heapBefore);
			while (glyphCache->iNumberOfShaperCacheEntries > 0)
				{
				previous = si->iPrevious;
				COpenFontShaperCacheEntry::Delete(iHeap, si);
				si = previous;
				glyphCache->iNumberOfShaperCacheEntries--;
				}
			iHeap->AllocSize(heapAfter);
			File()->GetFontStore()->SetShaperCacheMemUsage(File()->GetFontStore()->GetShaperCacheMemUsage() - (heapBefore - heapAfter));
			File()->GetFontStore()->DecNumShaperCaches();
			}

		iHeap->Free(glyphCache);
		}
	COpenFontSessionCacheList* sessionCacheList = SessionCacheList();
    if (sessionCacheList != NULL)
        {
        sessionCacheList->DeleteFontGlyphs(iHeap, this);
        }
    COpenFontFile* file = File();
    if (file != NULL)
        {
        file->RemoveFontFromList(this);
        }
	}

EXPORT_C void COpenFont::operator delete(TAny *aFont)
	{
	if(aFont != NULL)
		{
		COpenFont* f = (COpenFont*)aFont;
		if (f->iHeap)
			f->iHeap->Free(aFont);
		}
	}

/**
Rasterize a glyph

This function may only be called via an FBSERV message.

@param aSessionHandle Session handle of the calling session
@param aCode Unicode value or glyph code if top bit is set
@param aGlyphData
	Output data. May be null, in which case output may be
	obtained through a call to GetCharacterData.

@return
	ETrue if aGlyphData contains valid data (that is, if aGlyphData->Bitmap()
	and aGlyphData->Metrics() are valid), EFalse otherwise.
*/
TBool COpenFont::Rasterize(TInt aSessionHandle, TInt aCode,
	TOpenFontGlyphData* aGlyphData)
	{
	// create the cache if it doesn't exisit. As this call can only come from
	// FBSERV if the chunk has to be resized then no panic will happen.
    COpenFontGlyphCache* glyphCache = GetGlyphCache();
	if (glyphCache == NULL)
		{
        glyphCache = (COpenFontGlyphCache*)iHeap->Alloc(sizeof(COpenFontGlyphCache));
		if (glyphCache == NULL) // no memory
			{
			return EFalse;
			}
		new(glyphCache) COpenFontGlyphCache(iHeap);
		SetGlyphCache(glyphCache);
		}

	// Look in the Font Cache	
	const COpenFontGlyph* g = FontCacheGlyph(aCode);		 	

	// If it has already been rasterized return it.
	if (g != NULL)
		{
		if (aGlyphData != NULL)
			{
			aGlyphData->SetMetricsPointer(&g->iMetrics);
			aGlyphData->SetBitmapPointer(g->Bitmap());
			}

		return ETrue;
		}

	// Rasterize the glyph.
	TOpenFontGlyphData* temp_glyph_data = NULL;
	TInt error = KErrNone;
	TRAP(error, RasterizeHelperL(aCode, aGlyphData, temp_glyph_data));
	if (error != KErrNone)
		{
		iHeap->Free(temp_glyph_data);
		return EFalse;
		}

	TBool glyph_data_valid = ETrue;
	const TOpenFontGlyphData* cur_glyph_data = temp_glyph_data ? temp_glyph_data : aGlyphData;
	COpenFontGlyph* new_glyph = NULL;

	// If the maximum per-font cache memory will not be exceeded, put the glyph into the font cache.
	TInt bytes = sizeof(COpenFontGlyph) + cur_glyph_data->BytesNeeded();	
	if(glyphCache != NULL && bytes + glyphCache->iGlyphCacheMemory <= KMaxGlyphCacheMemory)
		{
		new_glyph = COpenFontGlyph::New(iHeap, aCode, cur_glyph_data->GlyphIndex(), *cur_glyph_data->Metrics(), cur_glyph_data->Bitmap());
		if (new_glyph != NULL)
            {
			if ((aCode & 0x80000000) != 0)
				{
				error = glyphCache->iGlyphTreeById.SetAt(aCode & 0x7FFFFFFF, new_glyph);
				}
			else
				{
				error = glyphCache->iGlyphTreeByUnicode.SetAt(aCode, new_glyph);
				}
			if (error == KErrNone)
			    {
                glyphCache->iGlyphCacheMemory += bytes;
			    }
			else
			    {
                iHeap->Free(new_glyph);
                new_glyph = NULL;
			    }
            }
		}
	// Otherwise put the glyph into the per-session cache.
	else
		{
		// Look in the session cache. Do not expect to find the glyph here
		// since the session cache has already been searched client-side.
		// However, SessionCacheGlyph() is called so that the session cache is
		// created if needed and an index is found where the new glyph will be
		// placed when added to the session cache.
		COpenFontSessionCache* cache = NULL;
		TInt index = 0;
		(void)SessionCacheGlyph(iHeap, aSessionHandle, aCode, cache, index, ETrue);
		if (cache == NULL)
			{			
			iHeap->Free(temp_glyph_data);
			return EFalse;
			}

		COpenFontSessionCacheEntry* new_entry =
			COpenFontSessionCacheEntry::New(iHeap, this, aCode, cur_glyph_data->GlyphIndex(), *cur_glyph_data->Metrics(), cur_glyph_data->Bitmap());
		new_glyph = new_entry;
		if (new_entry != NULL)
			{
			cache->Insert(iHeap, new_entry, index);
			}
		}

	if (temp_glyph_data != NULL)
		{
		iHeap->Free(temp_glyph_data);
        }
        
    // Fix up the returned glyph data pointers to point to the actual data.
    if (new_glyph == NULL)
        glyph_data_valid = EFalse;
    else if (aGlyphData != NULL)
        {
        aGlyphData->SetMetricsPointer(&new_glyph->iMetrics);
        aGlyphData->SetBitmapPointer(new_glyph->Bitmap());
        }

	return glyph_data_valid;
	}

void COpenFont::RasterizeHelperL(TInt aCode, TOpenFontGlyphData* aGlyphData, TOpenFontGlyphData*& aTempGlyphData)
	{
	aTempGlyphData = 0;
	MOpenFontShapingExtension* extensionInterface = 0;

	// if the MSB is set this is a request to rasterize a glyph code
	// rather than a unicode value.  This can only be done if the rasterizer
	// supports the extended API. 
	if ( aCode & 0x80000000 )
		{
		aCode = GLYPH_CODE(aCode);
		// get the extension API for RasterizeGlyphL() if available
		TAny* ext = NULL;
		ExtendedInterface(KUidOpenFontShapingExtension, ext);
		extensionInterface = reinterpret_cast<MOpenFontShapingExtension*>(ext);

		if (extensionInterface == NULL)
			// an attempt to rasterize a glyph when the rasterizer does not
			// support it; best to do nothing
			return;
		}
	TOpenFontGlyphData* currGlyphData = aGlyphData;

	if (currGlyphData == NULL)
		{
		aTempGlyphData = TOpenFontGlyphData::New(iHeap, 0);
		if (!aTempGlyphData)
			User::Leave(KErrNoMemory);
		currGlyphData = aTempGlyphData;
		}

	if (extensionInterface != NULL)
		extensionInterface->RasterizeGlyphL(aCode, currGlyphData);
	else
		RasterizeL(aCode, currGlyphData);

	// If the GlyphData object was not large enough, create a temporary one
	// that can then be deleted by the caller.
	if (currGlyphData->Overflow())
		{
		TInt bytesNeeded = currGlyphData->BytesNeeded();
		if (aTempGlyphData)
			iHeap->Free(aTempGlyphData);
		aTempGlyphData = TOpenFontGlyphData::New(iHeap, bytesNeeded);
		if (aTempGlyphData == NULL)
			User::Leave(KErrNoMemory);
		
		currGlyphData = aTempGlyphData;

		// If the extension interface was used above, then use again here
		if (extensionInterface != NULL)
			extensionInterface->RasterizeGlyphL(aCode, currGlyphData);
		else
			RasterizeL(aCode, currGlyphData);
		}
		
	if (currGlyphData->Metrics() == NULL) 
		{
		User::Leave(KErrArgument);
		}
	}

/** @internalComponent */
void COpenFont::SetShaper(CShaper* aShaper)
	{
	iShaper = aShaper;
	}
	
/** @internalComponent */
CShaper* COpenFont::GetShaper()
	{
	return iShaper;
	}

/** @internalComponent */
TBool COpenFont::HasShaper() const
	{
	return iShaper != NULL;
	}

void COpenFont::DeleteShaper() const
	{
	delete iShaper;
	}

TInt COpenFont::GetFontTable(TUint32 aTag, TAny*& aTableContent, TInt& aLength) 
    {
    // get the extension API for GetTrueTypeTable() if available
    TAny* ext = NULL;
    ExtendedInterface(KUidOpenFontTrueTypeExtension, ext);
    MOpenFontTrueTypeExtension* extensionInterface = 
        reinterpret_cast<MOpenFontTrueTypeExtension*>(ext);

    TInt ret = KErrNone;
    if (extensionInterface == NULL)
        {
        ret = KErrNotSupported;
        }
    else 
        {
        TUint32 tag = aTag;
        TInt len = 0;
        aTableContent = extensionInterface->GetTrueTypeTable(ret, tag, &len);
        if (KErrNone == ret) 
            {
            aLength = len;
            }
        }
    return ret;
    }

TInt COpenFont::GetGlyphOutline(TUint aCode, 
        TBool aHinted, TAny*& aOutline, TInt &aLength) 
    {
    // get the extension API for GetTrueTypeTable() if available
    TAny* ext = NULL;
    ExtendedInterface(KUidOpenFontGlyphOutlineExtension, ext);
    MOpenFontGlyphOutlineExtension *extensionInterface = 
        reinterpret_cast<MOpenFontGlyphOutlineExtension*>(ext);

    TInt ret = KErrNone;
    if (extensionInterface == NULL)
        {
        ret = KErrNotSupported;
        }
    else 
        {
        ret = extensionInterface->GetGlyphOutline(aCode, ETrue, 
                aHinted, aOutline, aLength);
        }
    return ret;
    }


/**
A constructor initialised with a TCharacterMetrics object.

This is the old-style character metrics object. As for other T classes, there 
is no need to explicitly cleanup TOpenFontCharMetrics objects.

@param aMetrics The old-style metrics object.
*/
EXPORT_C TOpenFontCharMetrics::TOpenFontCharMetrics(const TCharacterMetrics& aMetrics)
	{
	iWidth = (TInt16)(aMetrics.iMoveInPixels - aMetrics.iLeftAdjustInPixels - aMetrics.iRightAdjustInPixels);
	iHeight = aMetrics.iHeightInPixels;
	iHorizBearingX = aMetrics.iLeftAdjustInPixels;
	iHorizBearingY = aMetrics.iAscentInPixels;
	iHorizAdvance = aMetrics.iMoveInPixels;
	iVertBearingX = 0;
	iVertBearingY = 0;
	iVertAdvance = aMetrics.iHeightInPixels;
	iGlyphBitmapType = 0;
	}

/**
Converts a TOpenFontCharacterMetrics object to a TCharacterMetrics.

@param aMetrics On return, contains the character's old-style metrics. 
@return ETrue if it was possible to get the metrics, otherwise EFalse.
*/
EXPORT_C TBool TOpenFontCharMetrics::GetTCharacterMetrics(TCharacterMetrics& aMetrics) const
	{
	aMetrics.iAscentInPixels = iHorizBearingY;
	aMetrics.iHeightInPixels = iHeight;
	aMetrics.iLeftAdjustInPixels = iHorizBearingX;
	aMetrics.iMoveInPixels = iHorizAdvance;
	aMetrics.iRightAdjustInPixels = (TInt16)(iHorizAdvance - iHorizBearingX - iWidth);
	return ETrue;
	}

TBool COpenFont::GetCharacterData(TInt aSessionHandle, TInt aCode, const TOpenFontCharMetrics*& aMetrics, const TUint8*& aBitmap) const
	{
	const COpenFontGlyph* g = Glyph(aSessionHandle, aCode);
	if (g != NULL)
		{
		aMetrics = &g->iMetrics;
		aBitmap = g->Bitmap();
		return ETrue;
		}
	else
		{
		aMetrics = NULL;
		aBitmap = NULL;
		return EFalse;
		}
	}



void COpenFont::OnFileDeleted()
	{
    iFileOffset = 0;
	}

COpenFontGlyphCache* COpenFont::GetGlyphCache() const
	{
    if (iGlyphCacheOffset == 0)
        {
        return NULL;
        }
    return reinterpret_cast<COpenFontGlyphCache*>(PtrAdd(const_cast<COpenFont*>(this), iGlyphCacheOffset));
	}

const COpenFontGlyph* COpenFont::Glyph(TInt aSessionHandle, TInt aCode) const
	{	
	const COpenFontGlyph* glyph = const_cast<COpenFont*>(this)->FontCacheGlyph(aCode);
	if (glyph == NULL)
		{			
		COpenFontSessionCache* cache;
		TInt index;
		glyph = SessionCacheGlyph(iHeap, aSessionHandle, aCode, cache, index, EFalse);
	}

	return glyph;
	}

/**
Is the specified character present in the font?
*/
TBool COpenFont::HasCharacterL(TInt aCode) const
	{
	COpenFontFile* file = File();
	if (file != NULL)
		return file->HasUnicodeCharacterL(iFaceIndex, aCode);
	else
		return EFalse;
	}

/**
Retrieve glyph data from the per-font glyph cache.
If it is not found return NULL. 
If the cache hasn't been created, then return NULL.
Previous versions of this function created the cache, but as this function can potentially
run in the context of threads other than FBSERV the alloc could panic if iHeap's chunk had to 
be resized - this is not allowed by the kernel.
The cache is now created in COpenFont::Rasterize which can only be called within the context of FBSERV
@param aCode The code for the glpyh to look for in the cache
@return A pointer to the requested glyph if it was found in the glyph cache, NULL if it was not found.
*/
const COpenFontGlyph* COpenFont::FontCacheGlyph(TInt aCode) const
	{
	if (COpenFontGlyphCache* glyphCache = GetGlyphCache())
		{
		if ((aCode & 0x80000000) != 0)
			{
			return glyphCache->iGlyphTreeById.At(aCode & 0x7FFFFFFF);
			}
		else
			{
			return glyphCache->iGlyphTreeByUnicode.At(aCode);
			}
		}

	// No glyph found
	return NULL;
	}

/** Retrieve glyph data from the session cache. If the glyph is not in the
cache, return the cache and glyph index where the glyph data should be put. If
there is no session cache, optionally create it.

@param aHeap
	The heap on which to create the new cache, if appropriate. Not used if
	either the appropriate session cache already exists, or if aCreate is
	passed as false
@param aSessionHandle The session handle
@param aCode
	The code of the glyph to look up (Unicode or glyph code if the top bit is
	set)
@param aCache
	Returns the appropriate session cache, or NULL if an attempt to create it 
	has failed.
@param aIndex
	Returns where the glyph is, or should be put
@param aCreate
	If False, creation of a new cache is inhibited
@return The glyph data, or null if the glyph is not in the cache
@internalComponent
*/
const COpenFontGlyph* COpenFont::SessionCacheGlyph(RHeap* aHeap,
	TInt aSessionHandle, TInt aCode, COpenFontSessionCache*& aCache,
	TInt& aIndex, TBool aCreate) const
	{
	aIndex = 0;
	COpenFontSessionCacheList* cacheList = SessionCacheList();
	aCache = cacheList->FindCache(aSessionHandle);
	if (aCache != NULL)
		{
		return aCache->Glyph(this, aCode, aIndex);
		}
	
	if (aCreate)
		{
		COpenFontSessionCache* new_cache = NULL;
		TRAPD(error, new_cache = COpenFontSessionCache::NewL(aHeap, aSessionHandle, KSessionCacheEntries));
		
		if ((!error) && new_cache != NULL)
			{
			if (cacheList->AddCache(new_cache) != KErrNone)
				{
				new_cache->Delete(aHeap);	
				aHeap->Free(new_cache);
				return NULL;
				}

			aCache = new_cache;
			aIndex = GLYPH_CODE(aCode) % KSessionCacheEntries;
			}
		}
	return NULL;
	}

COpenFontSessionCacheList* COpenFont::SessionCacheList()const
    {
    if (iSessionCacheListOffset == 0)
        {
        return NULL;
        }
    return reinterpret_cast<COpenFontSessionCacheList*>(reinterpret_cast<TInt>(this) + iSessionCacheListOffset);
    }

void COpenFont::SetSessionCacheList(COpenFontSessionCacheList* aSessionCacheList)
    {
    iSessionCacheListOffset = aSessionCacheList ? reinterpret_cast<TInt>(aSessionCacheList) - reinterpret_cast<TInt>(this) : NULL;
    }

void COpenFont::SetFile(COpenFontFile* aFile)
    {
    iFileOffset = aFile ? reinterpret_cast<TInt>(aFile) - reinterpret_cast<TInt>(this) : NULL;
    }

void COpenFont::SetGlyphCache(COpenFontGlyphCache* aGlyphCache)
    {
    iGlyphCacheOffset = aGlyphCache ? reinterpret_cast<TInt>(aGlyphCache) - reinterpret_cast<TInt>(this) : NULL;
    }


/**
Create a glyph data object on the shared heap, given the code, metrics and the data bytes.
The data is copied; ownership remains with the caller.
*/
COpenFontGlyph* COpenFontGlyph::New(RHeap* aHeap, TInt aCode, TInt aGlyphIndex, const TOpenFontCharMetrics& aMetrics, const TDesC8& aBitmap)
	{
	COpenFontGlyph* glyph = (COpenFontGlyph*)aHeap->Alloc(sizeof(COpenFontGlyph) + aBitmap.Size());
    if (glyph == NULL)
        {
        return NULL;
        }
    new(glyph) COpenFontGlyph(aCode, aGlyphIndex, aMetrics);
	glyph->SetBitmap(glyph + 1);
	Mem::Copy(glyph + 1, aBitmap.Ptr(), aBitmap.Size());
	return glyph;
	}

COpenFontSessionCacheEntry* COpenFontSessionCacheEntry::New(RHeap* aHeap, const COpenFont* aFont, TInt aCode, TInt aGlyphIndex, const TOpenFontCharMetrics& aMetrics, const TDesC8& aBitmap) 
	{
	COpenFontSessionCacheEntry* entry = (COpenFontSessionCacheEntry*)aHeap->Alloc(sizeof(COpenFontSessionCacheEntry) + aBitmap.Size());
	if (entry == NULL)
		{
		return NULL;
		}
	new(entry) COpenFontSessionCacheEntry(aFont, aCode, aGlyphIndex, aMetrics);
	entry->SetBitmap(entry + 1);
	Mem::Copy(entry + 1, aBitmap.Ptr(), aBitmap.Size());
	return entry;
	}

void COpenFontGlyph::SetBitmap(const TAny* aBitmap)
	{
	iBitmapOffset = reinterpret_cast<TInt>(aBitmap) - reinterpret_cast<TInt>(this);
	}

COpenFontSessionCache* COpenFontSessionCache::NewL(RHeap* aHeap, TInt aSessionHandle, TInt aEntries)
	{
    COpenFontSessionCache* c = (COpenFontSessionCache*)aHeap->AllocL(sizeof(COpenFontSessionCache));
    new(c) COpenFontSessionCache(aSessionHandle);
    if (c->iEntryArray.Create(aHeap, aEntries) != KErrNone)
        {
        aHeap->Free(c);
        User::Leave(KErrNoMemory);
        }
    return c;
	}


void COpenFontSessionCache::Delete(RHeap* aHeap)
    {
    TInt numEntries = iEntryArray.Count();
    for (TInt i = 0; i < numEntries; ++i)
        {
        COpenFontSessionCacheEntry* entry = iEntryArray[i];
        if (entry != NULL)
            {
            COpenFont* font=const_cast<COpenFont*>(entry->Font());
            if (font != NULL)
                font->DecrementCachedRefCount(iSessionHandle,NULL,ETrue);
            COpenFontSessionCacheEntry::Delete(aHeap, entry);
            }
        }
    iEntryArray.Close(aHeap);
    }

const COpenFontGlyph* COpenFontSessionCache::Glyph(const COpenFont* aFont, TInt aCode, TInt& aIndex) const
	{
    aIndex = -1;
    TInt numEntries = iEntryArray.Count();
    TInt index = GLYPH_CODE(aCode) % numEntries;   // simple hash function to shorten searches
    for (TInt i = 0; i < numEntries; ++i, ++index)
        {
        if (index >= numEntries)
            {
            index = 0;
            }
        const COpenFontSessionCacheEntry* entry = iEntryArray[index];
        if (entry == NULL)
            {
            if (aIndex < 0)
                {
                aIndex = index;
                }
            }
        else if (entry->Font() == aFont && entry->iCode == aCode)
            {
            return entry;
            }
        }
    return NULL;
	}


void COpenFontSessionCache::Insert(RHeap* aHeap, COpenFontSessionCacheEntry* aEntry, TInt aIndex)
	{
    if (aIndex >= iEntryArray.Count())
        {
        Panic(EFntSessionCacheIndexOutOfRange);
        }
    if (aIndex < 0)
        {
        aIndex = Math::Rand(iRandomSeed) % iEntryArray.Count();
        }
    COpenFontSessionCacheEntry::Delete(aHeap, iEntryArray[aIndex]);
    iEntryArray.SetAt(aIndex, aEntry);
	}

COpenFontSessionCache::COpenFontSessionCache(TInt aSessionHandle):
    iSessionHandle(aSessionHandle),
    iRandomSeed(0)
    {
    }

TInt COpenFontSessionCacheList::AddCache(COpenFontSessionCache* aCache)
	{
	for (TInt index = 0; index < EMaxNumCaches; ++index)
		{
		if (iSessionHandleArray[index] == 0)
			{
			iSessionHandleArray[index] = aCache->SessionHandle();
			iCacheOffsetArray[index] = reinterpret_cast<TInt>(aCache) - reinterpret_cast<TInt>(this);
			return KErrNone;
			}
		}
	return KErrNoMemory;
	}

COpenFontSessionCache* COpenFontSessionCacheList::FindCache(TInt aSessionHandle) const
	{
	if (aSessionHandle == 0)
		{
		return NULL;
		}
	for (TInt index = 0; index < EMaxNumCaches; ++index)
		{
		if (iSessionHandleArray[index] == aSessionHandle)
			{
			return reinterpret_cast<COpenFontSessionCache*>(reinterpret_cast<TInt>(this) + iCacheOffsetArray[index]);
			}
		}
	return NULL;
	}

/** Delete all the items in the session cache if the current cache session handle
matches the passed session handle.

@param aHeap The heap base of the current process.
@param aSessionHandle The session handle of the cache to be deleted.
 */
void COpenFontSessionCacheList::DeleteCache(RHeap* aHeap, TInt aSessionHandle)
	{
	if (aSessionHandle == 0)
		{
		return;
		}
	for (TInt index = 0; index < EMaxNumCaches; ++index)
		{
		if (iSessionHandleArray[index] == aSessionHandle)
			{
			COpenFontSessionCache* cache = reinterpret_cast<COpenFontSessionCache*>(PtrAdd(this, iCacheOffsetArray[index]));
			cache->Delete(aHeap);
			aHeap->Free(cache);
			iSessionHandleArray[index] = 0;
			iCacheOffsetArray[index] = 0;
			return;
			}
        }
	}

/** Delete all the items in the current session cache.

@param aHeap The heap base of the current process.
 */
void COpenFontSessionCacheList::Delete(RHeap* aHeap)
	{
	for (TInt index = 0; index < EMaxNumCaches; ++index)
		{
		if (iCacheOffsetArray[index] != 0)
			{
			COpenFontSessionCache* cache = reinterpret_cast<COpenFontSessionCache*>(PtrAdd(this, iCacheOffsetArray[index]));
			cache->Delete(aHeap);
			aHeap->Free(cache);
			}
		}
	Mem::FillZ(this, sizeof(COpenFontSessionCacheList));
	}

/**
Delete all glyphs belonging to a particular font.
*/
void COpenFontSessionCacheList::DeleteFontGlyphs(RHeap* aHeap, const COpenFont* aFont)
	{
	for (TInt index = 0; index < EMaxNumCaches; ++index)
		{
		if (iCacheOffsetArray[index] != 0)
			{
			COpenFontSessionCache* cache = reinterpret_cast<COpenFontSessionCache*>(PtrAdd(this, iCacheOffsetArray[index]));
			TInt numEntries = cache->iEntryArray.Count();
			for (TInt i = 0; i < numEntries; ++i)
				{
				COpenFontSessionCacheEntry* entry = cache->iEntryArray[i];
				if (entry != NULL && entry->Font() == aFont)
					{
					COpenFontSessionCacheEntry::Delete(aHeap, entry);
					cache->iEntryArray.SetAt(i, NULL);
					}
				}
			}
        }
	}

/**
C++ constructor with a CFont parameter.

This creates a TOpenFontMetrics and initialises it with size, ascent, maximum 
height, descent, maximum depth and maximum character width information from 
the CFont that was passed as a parameter.

@param aFont The font from which to initialise the metrics object.
*/
EXPORT_C TOpenFontMetrics::TOpenFontMetrics(const CFont* aFont)
	{
	iDesignHeight = (TInt16) aFont->HeightInPixels();
	iAscent = iMaxHeight = (TInt16) aFont->AscentInPixels();
	iDescent = iMaxDepth = (TInt16)(iDesignHeight - iAscent);
	iMaxWidth = (TInt16)aFont->MaxCharWidthInPixels();
	iBaselineCorrection = 0;
	}

/** 
@publishedPartner
@prototype

@param aBaselineCorrection The baseline correction to be associated with this font

Sets the baseline correction applied to this font; this value is used to offset
the underlinke and strikethrough positions and is used by linked fonts only.
*/
const TUint16 KBitsForUnderline = 10;
const TUint16 KMaskUnderline = (1<<KBitsForUnderline)-1;  //bottom bits which are set 
const TUint16 KMaskBitmapType = ~KMaskUnderline;  //top bits which are set
const TUint16 KSignBit = 1<<(KBitsForUnderline-1); //sign bit for the lower number, which can be signed
const TUint32 KTop16Of32Bits = 0xffff0000; 

EXPORT_C void TOpenFontMetrics::SetBaselineCorrection(TInt aBaselineCorrection)
	{
	if (aBaselineCorrection >= (1<<(KBitsForUnderline-1)))
	    {
	    OstTrace1( TRACE_FATAL, TOPENFONTMETRICS_SETBASELINECORRECTION, "aBaselineCorrection=%d, Panic(EFntOverFlow)", aBaselineCorrection );
	    __ASSERT_DEBUG(0, Panic(EFntOverFlow));
	    }
	if (aBaselineCorrection <= (0-(1<<(KBitsForUnderline-1))))
	    {
	    OstTrace1( TRACE_FATAL, DUP1_TOPENFONTMETRICS_SETBASELINECORRECTION, "aBaselineCorrection=%d, Panic(EFntOverFlow)", aBaselineCorrection );
	    __ASSERT_DEBUG(0, Panic(EFntOverFlow));
	    }
	
	TUint16 value = iBaselineCorrection;
	value &=~KMaskUnderline; //zero all the underline position bits
	if (aBaselineCorrection<0)
		{
		//need to mask out extra sign bits for negative value
		iBaselineCorrection = value | (static_cast<TUint16>(aBaselineCorrection)&~KMaskBitmapType);
		}
	else
		{
		iBaselineCorrection = value | static_cast<TUint16>(aBaselineCorrection);
		}
	}

/**
@publishedPartner
@prototype

Gets the baseline correction applied to this font; this value is used to offset
the underlinke and strikethrough positions and is used by linked fonts only.

@return The baseline correction associated with this font
*/

EXPORT_C TInt TOpenFontMetrics::BaselineCorrection()
	{
	TUint16 value = iBaselineCorrection;  //read once for improved multi threading
	if (!(value & KSignBit))
		{
		//value is positive, no need to sign extend
		return value & KMaskUnderline;
		}
	else
		{
		//value is negative, need to stuff ones into the high bits
		//could shift up and shift down
		return static_cast<TInt>(static_cast<TUint>(value) | KMaskBitmapType | KTop16Of32Bits);
		}
	}

/**
C++ constructor with UID and filename.

Call this constructor in the constructor for your derived object, passing 
it aUid and aFileName arguments.

Non Symbian-platform-native font files are allocated IDs by the font store. 
These are passed in by the rasteriser class. UIDs are required by the 
font framework. However you should not use the ID to access the file, since 
a new UID will need to be allocated when the file is next loaded, e.g. after 
a device reboot. Instead use the font file name.

@param aUid The UID of the font file.
@param aFileName The full filename, including the path, of the font file.
*/
EXPORT_C COpenFontFile::COpenFontFile(TInt aUid, const TDesC& aFileName)
 :	iFaceAttrib(1),
	iUid(TUid::Uid(aUid)),
	iFileName(aFileName),
	iFontList(8)
	{
	}

/**
Destructor.

It is not allowed that file is deleted before its fonts
and the logic is handled in CFontStore::RemoveFile().
*/
EXPORT_C COpenFontFile::~COpenFontFile()
	{
    CFontStore *fs = GetFontStore();
    if (fs != NULL)
        {
        fs->CleanupCacheOnOpenFontFileRemoval(this);
        }
	delete iData;
	}

/**
Gets the nearest font in pixels.

Implementations of this pure virtual function should create the COpenFont 
derived object that most closely matches aFontSpec, and place a pointer to 
it in aFont. If this cannot be done, e.g. if the font name doesn't match, 
aFont should be set to NULL. 
	
The other two arguments, aHeap and aSessionCacheList, should be passed to 
the COpenFont constructor.
	
Implementations may use the utilitity function GetNearestFontHelper() to get 
the attributes of the closest matching font.
	
@param aHeap Shared heap. This value should be passed to the COpenFont derived 
classes' constructor.
@param aSessionCacheList The session cache list. This value should be passed 
to the COpenFont derived classes' constructor.
@param aDesiredFontSpec The desired font specification.
@param aPixelWidth The width of a pixel. Used with aPixelHeight for calculating 
the algorithmic slant of the typeface. 
@param aPixelHeight The height of a pixel. Used with aPixelWidth for calculating 
the algorithmic slant of the typeface.
@param aFont On return, contains a pointer to the newly created COpenFont 
derived object, or NULL if no font matching aDesiredFontSpec exists.
@param aActualFontSpec The actual font specification of the font retrieved 
into aFont. 
@see GetNearestFontHelper()
@deprecated Use GetNearestFontToDesignHeightInPixels
*/
TInt COpenFontFile::GetNearestFontInPixels(
	RHeap*						aHeap,
	COpenFontSessionCacheList*	aSessionCacheList,
	const TOpenFontSpec&		aDesiredFontSpec,
	TInt						aPixelWidth,
	TInt						aPixelHeight,
	COpenFont*&					aFont,
	TOpenFontSpec&				aActualFontSpec)
	{
	return GetNearestFontToDesignHeightInPixels(aHeap, aSessionCacheList, aDesiredFontSpec, aPixelWidth, aPixelHeight, aFont, aActualFontSpec);
	}

/**
Gets the nearest font in pixels.

Implementations of this pure virtual function should create the COpenFont 
derived object that most closely matches aFontSpec, and place a pointer to 
it in aFont. If this cannot be done, e.g. if the font name doesn't match, 
aFont should be set to NULL. 
	
The other two arguments, aHeap and aSessionCacheList, should be passed to 
the COpenFont constructor.
	
Implementations may use the utilitity function GetNearestFontHelper() to get 
the attributes of the closest matching font.
	
@param aHeap Shared heap. This value should be passed to the COpenFont derived 
classes' constructor.
@param aSessionCacheList The session cache list. This value should be passed 
to the COpenFont derived classes' constructor.
@param aDesiredFontSpec The desired font specification.
@param aPixelWidth The width of a pixel. Used with aPixelHeight for calculating 
the algorithmic slant of the typeface. 
@param aPixelHeight The height of a pixel. Used with aPixelWidth for calculating 
the algorithmic slant of the typeface.
@param aFont On return, contains a pointer to the newly created COpenFont 
derived object, or NULL if no font matching aDesiredFontSpec exists.
@param aActualFontSpec The actual font specification of the font retrieved 
into aFont. 
@see GetNearestFontHelper()
*/
TInt COpenFontFile::GetNearestFontToDesignHeightInPixels(
	RHeap*						aHeap,
	COpenFontSessionCacheList*	aSessionCacheList,
	const TOpenFontSpec&		aDesiredFontSpec,
	TInt						aPixelWidth,
	TInt						aPixelHeight,
	COpenFont*&					aFont,
	TOpenFontSpec&				aActualFontSpec)
	{
	aFont = NULL;
	TRAPD(error, GetNearestFontToDesignHeightInPixelsAndAddToListL(aHeap, aSessionCacheList, aDesiredFontSpec, aPixelWidth, aPixelHeight, aFont, aActualFontSpec));
	return error;
	}


void COpenFontFile::GetNearestFontToDesignHeightInPixelsAndAddToListL(
	RHeap*						aHeap,
	COpenFontSessionCacheList*	aSessionCacheList,
	const TOpenFontSpec&		aDesiredFontSpec,
	TInt						aPixelWidth,
	TInt						aPixelHeight,
	COpenFont*&					aFont,
	TOpenFontSpec&				aActualFontSpec)
	{
	COpenFont* fontPtr = NULL;
	GetNearestFontToDesignHeightInPixelsL(aHeap, aSessionCacheList, aDesiredFontSpec, aPixelWidth, aPixelHeight, fontPtr, aActualFontSpec);
	if (fontPtr != NULL)
		{ // found a matching font
		CleanupStack::PushL(fontPtr);
		iFontList.AppendL(fontPtr);
		// transfer ownership
		aFont = fontPtr;
		CleanupStack::Pop(fontPtr);
		}
	}


/**
Gets the nearest font in pixels that fits inside specified max height.

Implementations of this pure virtual function should create the COpenFont 
derived object that most closely matches aFontSpec, while fitting within
aMaxHeight, and place a pointer to it in aFont. If this cannot be done,
e.g. if the font name doesn't match, aFont should be set to NULL. 

The other two arguments, aHeap and aSessionCacheList, should be passed to 
the COpenFont constructor.

Implementations may use the utilitity function GetNearestFontHelper()
to get the attributes of the closest matching font.

@param aHeap Shared heap. This value should be passed to the COpenFont derived 
classes' constructor.
@param aSessionCacheList The session cache list. This value should be passed 
to the COpenFont derived classes' constructor.
@param aDesiredFontSpec The desired font specification.
@param aPixelWidth The width of a pixel. Used with aPixelHeight for calculating 
the algorithmic slant of the typeface. 
@param aPixelHeight The height of a pixel. Used with aPixelWidth for calculating 
the algorithmic slant of the typeface.
@param aFont On return, contains a pointer to the newly created COpenFont 
derived object, or NULL if no font matching aDesiredFontSpec exists.
@param aActualFontSpec The actual font specification of the font retrieved 
into aFont. 
@param aMaxHeight The maximum height within which the font must fit.
@see GetNearestFontHelper()
*/
TInt COpenFontFile::GetNearestFontToMaxHeightInPixels(
	RHeap*						aHeap,
	COpenFontSessionCacheList*	aSessionCacheList,
	const TOpenFontSpec&		aDesiredFontSpec,
	TInt						aPixelWidth,
	TInt						aPixelHeight,
	COpenFont*&					aFont,
	TOpenFontSpec&				aActualFontSpec,
	TInt						aMaxHeight)
	{
	aFont = NULL;
	TRAPD(error, GetNearestFontToMaxHeightInPixelsAndAddToListL(aHeap, aSessionCacheList, aDesiredFontSpec, aPixelWidth, aPixelHeight, aFont, aActualFontSpec, aMaxHeight));
	return error;
	}


void COpenFontFile::GetNearestFontToMaxHeightInPixelsAndAddToListL(
	RHeap*						aHeap,
	COpenFontSessionCacheList*	aSessionCacheList,
	const TOpenFontSpec&		aDesiredFontSpec,
	TInt						aPixelWidth,
	TInt						aPixelHeight,
	COpenFont*&					aFont,
	TOpenFontSpec&				aActualFontSpec,
	TInt						aMaxHeight)
	{
	COpenFont* fontPtr = NULL;
	GetNearestFontToMaxHeightInPixelsL(aHeap, aSessionCacheList, aDesiredFontSpec, aPixelWidth, aPixelHeight, fontPtr, aActualFontSpec, aMaxHeight);
	if (fontPtr != NULL)
		{  // found a matching font
		CleanupStack::PushL(fontPtr);
		iFontList.AppendL(fontPtr);
		// transfer ownership
		aFont = fontPtr;
		CleanupStack::Pop(fontPtr);
		}
	}


/**
Gets the nearest font helper function.

This function may be used by derived classes in their GetNearestFontInPixelsL() 
implementations. It finds the nearest font in the typeface attribute array, 
if any, to the provided font specification. If there is a possible match it 
places the face index in aFaceIndex and the actual specification (including 
algorithmic effects) in aActualFontSpec.

@param aDesiredFontSpec The desired font specification.
@param aPixelWidth The width of a pixel. Used with aPixelHeight for calculating 
the algorithmic slant of the typeface.
@param aPixelHeight The height of a pixel. Used with aPixelWidth for calculating 
the algorithmic slant of the typeface.
@param aFaceIndex The index of the typeface which contains the closest match 
to aDesiredFontSpec.
@param aActualFontSpec The actual font specification of the font with attributes 
closest to aDesiredFontSpec.
@return ETrue if there is a possible font match, otherwise EFalse.
*/
EXPORT_C TBool COpenFontFile::GetNearestFontHelper(
	const TOpenFontSpec&	aDesiredFontSpec,
	TInt					aPixelWidth,
	TInt					aPixelHeight,
	TInt&					aFaceIndex,
	TOpenFontSpec&			aActualFontSpec) const
	{
	const TInt faces = FaceCount();
	TInt best_points = 0;
	TInt best_index = -1;

	for (TInt i = 0; i < faces; i++)
		{
		TInt cur_points = 0;

		if (0 < aDesiredFontSpec.Name().Length())
			{
			cur_points = ScoreByName(aDesiredFontSpec, iFaceAttrib[i]);
			}
		else
			{
			cur_points = ScoreByStyle(aDesiredFontSpec, iFaceAttrib[i]);
			}

		if (cur_points)
			{
			if (aDesiredFontSpec.IsItalic() == iFaceAttrib[i].IsItalic())
				cur_points++;
			if (aDesiredFontSpec.IsBold() == iFaceAttrib[i].IsBold())
				cur_points++;
			}

		if (cur_points > best_points)
			{
			best_points = cur_points;
			best_index = i;
			}
		}

	if (best_index != -1)
		{
		aActualFontSpec = aDesiredFontSpec;
		// copy attributes & name
		aActualFontSpec.SetAttrib(iFaceAttrib[best_index]);
		aActualFontSpec.SetName(iFaceAttrib[best_index].FamilyName());
		// Set an algorithmic slant and adjust it for the pixel aspect ratio.
		if ((aDesiredFontSpec.IsItalic()) && (!iFaceAttrib[best_index].IsItalic()) && (0 == aDesiredFontSpec.SlantFactor()))
			{
			TInt factor = KDefaultSlantFactor;
			if (TOpenFontSpec::IsCompensationForAspectRatioNeeded(aPixelWidth, aPixelHeight))
				{
				TOpenFontSpec::ApplyRatio(factor, aPixelWidth, aPixelHeight);
				}
			aActualFontSpec.SetSlantFactor(factor);
			}
		}

	aFaceIndex = best_index;
	return best_index != -1;
	}

TInt COpenFontFile::ScoreByName(const TOpenFontSpec& aDesiredFontSpec, const TAttrib& aAttrib)
	{
	if (!aDesiredFontSpec.Name().CompareF(aAttrib.FullName()) || !aDesiredFontSpec.Name().CompareF(aAttrib.LocalFullName()))
		{
		return 4;
		}
	else if (!aDesiredFontSpec.Name().CompareF(aAttrib.ShortFullName()) || !aDesiredFontSpec.Name().CompareF(aAttrib.ShortLocalFullName()))
		{
		return 3;
		}
	else if (!aDesiredFontSpec.Name().CompareF(aAttrib.FamilyName()) || !aDesiredFontSpec.Name().CompareF(aAttrib.LocalFamilyName()))
		{
		return 2;
		}
	else if (!aDesiredFontSpec.Name().CompareF(aAttrib.ShortFamilyName()) || !aDesiredFontSpec.Name().CompareF(aAttrib.ShortLocalFamilyName()))
		{
		return 1;
		}
	return 0;
	}

TInt COpenFontFile::ScoreByStyle(const TOpenFontSpec& aDesiredFontSpec, const TAttrib& aAttrib)
	{
	if (aDesiredFontSpec.IsSymbol() == aAttrib.IsSymbol())
		{
		return 4;
		}
	else if(aDesiredFontSpec.IsMonoWidth() == aAttrib.IsMonoWidth())
		{
		return 3;
		}
	else if(aDesiredFontSpec.IsSerif() == aAttrib.IsSerif())
		{
		return 2;
		}

	return 0;
	}

#ifdef _DEBUG
/**  @internalComponent */
EXPORT_C TBool COpenFontFile::GetNearestFontHelperOld(const TOpenFontSpec& aDesiredFontSpec, TInt aPixelWidth,TInt aPixelHeight,TInt& aFaceIndex, TOpenFontSpec& aActualFontSpec) const
	{
	TInt faces = FaceCount();
	TInt best_points = 0;
	TInt best_index = -1;
	TBool slant = FALSE;
	for (TInt i = 0; i < faces; i++)
		{
		TPtrC family_name = iFaceAttrib[i].FamilyName();
		TPtrC full_name = iFaceAttrib[i].FullName();
		TPtrC local_family_name = iFaceAttrib[i].LocalFamilyName();
		TPtrC local_full_name = iFaceAttrib[i].LocalFullName();
		TPtrC desired_name = aDesiredFontSpec.Name();

		TInt cur_points = 0;
		if (desired_name.Length() > 0)
			{
			if ((full_name.CompareF(desired_name) == 0) || (local_full_name.CompareF(desired_name) == 0))
				cur_points = 4;
			else if ((family_name.CompareF(desired_name) == 0) || (local_family_name.CompareF(desired_name) == 0))
				cur_points = 2;
			}
		else
			{
			if ((aDesiredFontSpec.IsSerif() == iFaceAttrib[i].IsSerif()) && (aDesiredFontSpec.IsMonoWidth() == iFaceAttrib[i].IsMonoWidth()) && (aDesiredFontSpec.IsSymbol() == iFaceAttrib[i].IsSymbol()))
				cur_points = 2;
			}
		if (cur_points)
			{
			if (aDesiredFontSpec.IsItalic() == iFaceAttrib[i].IsItalic())
				cur_points++;
			if (aDesiredFontSpec.IsBold() == iFaceAttrib[i].IsBold())
				cur_points++;
			if (cur_points > best_points)
				{
				best_points = cur_points;
				best_index = i;
				slant = (aDesiredFontSpec.IsItalic()) && (!iFaceAttrib[i].IsItalic());
				}
			}
		}

	if (best_index != -1)
		{
		TInt32 slant_factor = aDesiredFontSpec.SlantFactor();

		// Set an algorithmic slant and adjust it for the pixel aspect ratio.
		if (slant && slant_factor == 0)
			{
			slant_factor = KDefaultSlantFactor;
			if (aPixelWidth>0 && aPixelHeight>0 && TOpenFontSpec::IsCompensationForAspectRatioNeeded(aPixelWidth,aPixelHeight))
				{
				TOpenFontSpec::ApplyRatio(slant_factor,aPixelWidth,aPixelHeight);
				}
			}

		aActualFontSpec = aDesiredFontSpec;
		// copy attributes & name
		aActualFontSpec.SetAttrib(iFaceAttrib[best_index]);
		aActualFontSpec.SetName(iFaceAttrib[best_index].FamilyName());
		aActualFontSpec.SetSlantFactor(slant_factor);
		aActualFontSpec.SetEffects(0);
		}

	aFaceIndex = best_index;
	return best_index != -1;
	}
#else //_DEBUG
/**
@internalComponent
*/
EXPORT_C TBool COpenFontFile::GetNearestFontHelperOld(const TOpenFontSpec&, TInt, TInt, TInt&, TOpenFontSpec&) const 
	{
	return EFalse;
	}
#endif //_DEBUG

/** This function is called (via iFile) by a COpenFont when it is destroyed. */
void COpenFontFile::RemoveFontFromList(const COpenFont* aFont)
	{
	TInt fonts = iFontList.Count();
	for (TInt i = 0; i < fonts; i++)
		if (iFontList[i] == aFont)
			{
			iFontList.Delete(i);
			break;
			}
	}

/**
Adds a typeface to this object's typeface array.

This function should be called during construction to add the attributes for 
each typeface in the font file to the typeface attribute array. 

Note:

The typeface array is what is searched for the closest match to a specified 
font by GetNearestFontHelper().

@param aAttrib The attributes for a typeface to be added to the typeface attribute 
array.
@see FaceAttrib()
@see FaceCount()
*/
EXPORT_C void COpenFontFile::AddFaceL(const TOpenFontFaceAttrib& aAttrib)
	{
	TAttrib& a = iFaceAttrib.ExtendL();
	(TOpenFontFaceAttrib&)a = aAttrib;
	}

void COpenFontFile::SetFontStoreL(CFontStore* aFontStore)
	{
	if (iData == NULL)
		{
		iData = new (ELeave) TOpenFontFileData;
		}
	iData->iFontStore = aFontStore;
	}

CFontStore* COpenFontFile::GetFontStore()
	{
	return iData ? iData->iFontStore : NULL;
	}

CArrayPtrFlat<COpenFont>* COpenFontFile::GetOpenFontList()
	{
	return &iFontList;
	}


static const TInt KTOpenFontSpecBitsNumSymbol = 1;
static const TInt KTOpenFontSpecBitsNumScript = 4;
static const TInt KTOpenFontSpecMaskSymbol = (1 << KTOpenFontSpecBitsNumSymbol) - 1;
static const TInt KTOpenFontSpecMaskScript = ((1 << KTOpenFontSpecBitsNumScript) - 1) << KTOpenFontSpecBitsNumSymbol;
static const TInt KTOpenFontSpecSymbolFlag = 0x1;

/**
Default C++ constructor setting
height to 16 pixels or twips,
width factor to 1 (65536 in 16.16 format),
slant factor to 0 (no slant),
effects to ENone,
symbol to 0 (assuming EScriptNone = 0),
print position to EPrintPosNormal.
*/
EXPORT_C TOpenFontSpec::TOpenFontSpec()
 :	iHeight(16),
	iWidthFactor(KOneIn16Dot16FixedPointFormat),
	iSlantFactor(0),
	iBitmapType(0),
	iEffects(FontEffect::ENone),
	iSymbol(0),
	iPrintPosition(EPrintPosNormal),
	iReserved2(0)
	{
	}

/**
C++ constructor taking a reference to a TFontSpec.

This object's members are initialised from the values of the aFontSpec parameter.

@param aFontSpec The font specification used to initialise this font specification.
*/
EXPORT_C TOpenFontSpec::TOpenFontSpec(const TFontSpec& aFontSpec)
	{
	*this = aFontSpec;
	}

/**
Assignment operator.

@param aFontSpec The old-style font specification to copy into this font specification.
*/
EXPORT_C void TOpenFontSpec::operator=(const TFontSpec& aFontSpec)
	{
	iSlantFactor = 0;
	iWidthFactor = KOneIn16Dot16FixedPointFormat;
	iHeight			= aFontSpec.iHeight;	// in twips
	iBitmapType		= aFontSpec.iFontStyle.BitmapType();
	iEffects		= aFontSpec.iFontStyle.Effects();
	iPrintPosition	= aFontSpec.iFontStyle.PrintPosition();
	iName			= aFontSpec.iTypeface.iName;
	SetScriptTypeForMetrics(aFontSpec.iTypeface.ScriptTypeForMetrics());
	const TBool symbol = aFontSpec.iTypeface.IsSymbol();
	SetSymbol(symbol);
	if (symbol)
		SetCoverage(0);	// no appropriate coverage value for the symbol set
	else
		SetCoverage(3);	// Latin and Latin-1 supplement
	iStyle = 0;
	if (!aFontSpec.iTypeface.IsProportional())
		iStyle |= TOpenFontFaceAttrib::EMonoWidth;
	if (aFontSpec.iTypeface.IsSerif())
		iStyle |= TOpenFontFaceAttrib::ESerif;
	if (aFontSpec.iFontStyle.Posture() == EPostureItalic)
		iStyle |= TOpenFontFaceAttrib::EItalic;
	if (aFontSpec.iFontStyle.StrokeWeight() == EStrokeWeightBold)
		iStyle |= TOpenFontFaceAttrib::EBold;
	}


/**
Adjust the width factor and slant factor to suit a pixel aspect ratio.
@publishedAll
@released
@param aPixelWidth The pixel width, in the same units as aPixelHeight.
@param aPixelHeight The pixel height, in the same units as aPixelWidth.
*/
EXPORT_C void TOpenFontSpec::CompensateForAspectRatio(TInt aPixelWidth, TInt aPixelHeight)
	{
	if (IsCompensationForAspectRatioNeeded(aPixelWidth, aPixelHeight))
		{
		ApplyRatio(iWidthFactor, aPixelHeight, aPixelWidth);
		ApplyRatio(iSlantFactor, aPixelWidth, aPixelHeight);
		}
	}

/**
Adjust the width factor and slant factor to suit a pixel aspect ratio stored 
in a MGraphicsDeviceMap derived object.
@publishedAll
@released
@param aMap The MGraphicsDeviceMap defining the pixel aspect ratio.
*/
EXPORT_C void TOpenFontSpec::CompensateForAspectRatio(const MGraphicsDeviceMap& aMap)
	{
	CompensateForAspectRatio(aMap.HorizontalPixelsToTwips(1000), aMap.VerticalPixelsToTwips(1000));
	}

/**
The pixel width and height are used to derive a ratio, and so can be 
in any units. Aspect ratios differing by less than 1/1000 are treated as 1:1.
@internalTechnology
*/
TBool TOpenFontSpec::IsCompensationForAspectRatioNeeded(TInt aPixelWidth,TInt aPixelHeight)
	{
	if ((aPixelWidth != aPixelHeight) && (0 < aPixelWidth) && (0 < aPixelHeight))
		{
		//If nearly square don't transform (0.999 < aPixelHeight/aPixelWidth < 1.001)
		TInt64 width = aPixelWidth;
		TInt64 height = aPixelHeight;
		width *= 999;	// Cannot do multiplication on declaration lines above as risk of TInt32 overflow
		height *= 1000;
		if (width <= height)	// 999 * aPixelWidth <= 1000 * aPixelHeight
			return ETrue;
		width += aPixelWidth;
		width += aPixelWidth;	// Cannot do with previous line as small risk of TInt32 overflow
		if (width >= height)	// 1001 * aPixelWidth >= 1000 * aPixelHeight
			return ETrue;
		}
	return EFalse;
	}

/**
Multiplies aValue by aNumerator/aDenominator but using TInt64's to avoid any overflows.
Returns ETrue if the final result has an overflow.
@internalTechnology
*/
TBool TOpenFontSpec::ApplyRatio(TInt& aValue, TInt aNumerator, TInt aDenominator)
	{
	TInt64 value(aValue);
	value = (value * aNumerator) / aDenominator;
	aValue = I64LOW(value);
	if (I64HIGH(value) != 0)
	    {
	    OstTrace1( TRACE_FATAL, TOPENFONTSPEC_APPLYRATIO, "value=%ld, Panic(EFntOverFlow)", value );
	    __ASSERT_DEBUG(0, Panic(EFntOverFlow));
	    }
	return I64HIGH(value) != 0;
	}

/**
Same as above function but this takes a TInt32 not a TInt
*/
TBool TOpenFontSpec::ApplyRatio(TInt32& aValue, TInt aNumerator, TInt aDenominator)
	{
	TInt value = aValue;
	TBool ret = ApplyRatio(value, aNumerator, aDenominator);
	aValue = value;
	return ret;
	}

EXPORT_C void TOpenFontSpec::SetAttrib(const TOpenFontFaceAttribBase& aAttrib)
/**
Sets the font attributes.

@param aAttrib The font attributes.
*/
	{
	TOpenFontFaceAttribBase* self = this;
	*self = aAttrib;
	}

/**
Gets the TFontSpec corresponding to this Open Font System font specification.
@publishedAll
@released
@param aFontSpec On return, contains the TFontSpec corresponding to this font 
specification.
*/
EXPORT_C void TOpenFontSpec::GetTFontSpec(TFontSpec& aFontSpec) const
	{
	aFontSpec = TFontSpec();
	TPtrC short_name(iName.Ptr(), Min(iName.Length(), KMaxTypefaceNameLength));
	aFontSpec.iTypeface.iName = short_name;
	aFontSpec.iTypeface.SetIsProportional(!IsMonoWidth());
	aFontSpec.iTypeface.SetIsSerif(IsSerif());
	aFontSpec.iTypeface.SetIsSymbol(Symbol());
	aFontSpec.iTypeface.SetScriptTypeForMetrics(ScriptTypeForMetrics());
	aFontSpec.iHeight = iHeight;	// as twips
	if (IsItalic() || (iSlantFactor > 0))
		aFontSpec.iFontStyle.SetPosture(EPostureItalic);
	if (IsBold() || IsEffectOn(FontEffect::EAlgorithmicBold))
		aFontSpec.iFontStyle.SetStrokeWeight(EStrokeWeightBold);
	aFontSpec.iFontStyle.SetPrintPosition(iPrintPosition);
	aFontSpec.iFontStyle.SetBitmapType(BitmapType());
	aFontSpec.iFontStyle.SetEffects(iEffects);
	}

/**
Sets a font effect to the given state.
@publishedAll
@released
@param aEffect The font effect to be set.
@param aOn True represents on, otherwise off.
@see TOpenFontSpec::IsEffectOn()
*/
EXPORT_C void TOpenFontSpec::SetEffects(FontEffect::TEffect aEffect, TBool aOn)
	{
	FontEffect::SetEffect(aEffect, aOn, iEffects);
	}

/** Checks if a font effect is on.
@publishedAll
@released
@return True represents the specified font effect is on, otherwise off.
@param aEffect The font effect to be checked.
@see TOpenFontSpec::SetEffects()
*/
EXPORT_C TBool TOpenFontSpec::IsEffectOn(FontEffect::TEffect aEffect) const
	{
	return FontEffect::IsEffectOn(aEffect, iEffects);
	}

/**
@deprecated This needs to be maintained to just call the inline methods.
*/
EXPORT_C void TOpenFontSpec::DoSetEffects(TUint32 aEffects)
	{
	SetEffects(aEffects);
	}

/**
@deprecated This needs to be maintained to just call the inline methods.
*/
EXPORT_C TUint32 TOpenFontSpec::DoEffects() const
	{
	return Effects();
	}

/**
Specifies the script which font metrics calculation will be based on.
@publishedAll
@released
@param aLanguage The language used to derive the required script.
*/
EXPORT_C void TOpenFontSpec::SetScriptTypeForMetrics(TLanguage aLanguage)
	{
	SetScriptTypeForMetrics(GlyphSample::TLanguage2TScript(aLanguage));
	}

/**
@internalTechnology
*/
void TOpenFontSpec::SetScriptTypeForMetrics(TInt aScript)
	{
	iSymbol &= ~KTOpenFontSpecMaskScript;
	iSymbol |= (KTOpenFontSpecMaskScript & (aScript << KTOpenFontSpecBitsNumSymbol));
	}

/**
Gets the script which the font metrics calculation will be based on.
@internalTechnology
@return The script.
*/
EXPORT_C TInt TOpenFontSpec::ScriptTypeForMetrics() const
	{
	return (KTOpenFontSpecMaskScript & iSymbol) >> KTOpenFontSpecBitsNumSymbol;
	}

/**
@internalTechnology
*/
void TOpenFontSpec::SetSymbol(TBool aSymbol)
	{
	iSymbol &= ~KTOpenFontSpecMaskSymbol;
	iSymbol |= (aSymbol ? KTOpenFontSpecSymbolFlag : 0);
	}

/**
@internalTechnology
*/
TBool TOpenFontSpec::Symbol() const
	{
	return (KTOpenFontSpecSymbolFlag & iSymbol) > 0;
	}

/**
@deprecated This needs to be maintained to just call the inline methods.
*/
EXPORT_C TBool TOpenFontSpec::OperatorEquality(const TOpenFontSpec& aOpenFontSpec) const
	{
	return this->operator == (aOpenFontSpec);
	}

/**
@internalTechnology
*/
TBool TOpenFontSpec::operator!=(const TOpenFontSpec& aOpenFontSpec) const
	{
	return !(this->operator == (aOpenFontSpec));
	}

/**
Static constructor for a TOpenFontGlyphData.

This constructor creates the object on a specified heap. It must be deleted 
using RHeap::Free().

@param aHeap The shared heap on which the object is constructed.
@param aBufferSize The amount of memory allocated for the glyph data.
@return A pointer to the newly created object.
*/
EXPORT_C TOpenFontGlyphData* TOpenFontGlyphData::New(RHeap* aHeap, TInt aBufferSize)
	{
	if (aBufferSize < 1)
		aBufferSize = 1;
	TInt bytes = sizeof(TOpenFontGlyphData) + aBufferSize - 1;
	TOpenFontGlyphData* g = (TOpenFontGlyphData*)aHeap->Alloc(bytes);
	if (g != NULL)
		{
		Mem::FillZ(g, bytes);
		g->iBitmapBufferSize = aBufferSize;
		}
	return g;
	}

// Virtual functions reserved for future expansion.
/**
@publishedPartner
@prototype
*/
EXPORT_C void COpenFontRasterizer::ExtendedInterface(TUid, TAny*&)
	{
	}

/** @internalComponent */
EXPORT_C void COpenFontFile::ExtendedInterface(TUid, TAny*&)
	{
	}

EXPORT_C void COpenFont::ExtendedInterface(TUid, TAny*&)
	{
	}
	
EXPORT_C CShaper::CShaper()
	{
	
	}
	
EXPORT_C CShaper::~CShaper()
	{
	
	}
/** @internalComponent */
EXPORT_C void* CShaper::ExtendedInterface(TUid)
	{
	return 0;
	}

/**
Sets the glyph bitmap type.

Normally the bitmap type belongs to the font, but for linked fonts this can 
be different between different font elements making up the linked font.

Note: This is only of use in conjunction with rasterizer based linked fonts.

@publishedPartner
@prototype
*/
EXPORT_C void TOpenFontCharMetrics::SetGlyphType(TGlyphBitmapType aGlyphBitmapType)
	{
	iGlyphBitmapType = aGlyphBitmapType;
	}

/**
Gets the glyph bitmap type.

@publishedPartner
@prototype
*/
EXPORT_C TGlyphBitmapType TOpenFontCharMetrics::GlyphType() const
	{
	if (iGlyphBitmapType == 0)
		return EGlyphBitmapTypeNotDefined;
	else
		return static_cast<TGlyphBitmapType>(iGlyphBitmapType);
	}
