/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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


/**
 @file
 @test
 @internalComponent Internal Symbian test code
*/


#include <f32file.h>
#include <e32test.h>
#include <fntstore.h>
#include <graphics/shapeimpl.h>
#include "ShaperCache.H"
#include "openfontsprivate.h"

#include "T_SHAPERCACHE.H"
#include "T_FSOPEN.H"

_LIT(KFontDummy,"z:\\PlatTest\\Graphics\\TestData\\dummy_fonts\\dummy");
_LIT(KFontDummy_b,"z:\\PlatTest\\Graphics\\TestData\\dummy_fonts\\dummy_b");
_LIT(KFontDummy_i,"z:\\PlatTest\\Graphics\\TestData\\dummy_fonts\\dummy_i");
_LIT(KFontDummy_bi,"z:\\PlatTest\\Graphics\\TestData\\dummy_fonts\\dummy_bi");

const TUint32 KDevanagariScriptCode = 0x64657661;
_LIT16(KTextToShape, "\x0915\x094D\x0937\x0924\x094D\x0930\x093F\x092F");
const TInt KResultNumberOfGlyphs = 4;

CTShaperCache::~CTShaperCache()
	{
	delete iFontStore;
	iHeap->__DbgMarkEnd(0);
	iHeap->Close();
	__UHEAP_MARKEND;
	User::Heap().Check();
	}

CTShaperCache::CTShaperCache(CTestStep* aStep):
	CTGraphicsBase(aStep)
	{
	INFO_PRINTF1(_L("FontStore T_ShaperCache test\n"));
	}
	
void CTShaperCache::ConstructL()
	{
	__UHEAP_MARK;
	iHeap = UserHeap::ChunkHeap(NULL,0x10000,0x100000);
	if (iHeap == NULL)
		User::Leave(KErrGeneral);
	iHeap->__DbgMarkStart();
	iFontStore = CFontStore::NewL(iHeap);
	
	// Install the dummy rasterizer.
	COpenFontRasterizer* r = CDummyRasterizer::NewL();
	CleanupStack::PushL(r);
	iFontStore->InstallRasterizerL(r);
	CleanupStack::Pop();

	// Install the dummy shaper
	CShaperFactory* shaperFactory = CDummyShaperFactory::NewL();
	CleanupStack::PushL(shaperFactory);
	iFontStore->InstallShaperFactoryL(shaperFactory);
	CleanupStack::Pop();
	}

/**
	@SYMTestCaseID
	GRAPHICS-FNTSTORE-0043

	@SYMTestCaseDesc
	Tests creating and deleting a new shaper cache entry.
	
	@SYMTestActions
	1. Allocated a TShapeHeader object on the heap/
	2. Creates a CShaper object on the stack.
	3. Allocates an entry for the TShapeHeader object in the cache.
	4. Frees memory taken up by the entry to the cache.
	5. The heap is checked for memory leaks.
	
	@SYMTestExpectedResults
	No memory leaks should exist.
*/
void CTShaperCache::Test1L()
	{
	INFO_PRINTF1(_L("ShaperCache Test1"));
	__UHEAP_MARK;

	TShapeHeader* shapeHeader=new TShapeHeader();
	shapeHeader->iGlyphCount=10;
	shapeHeader->iCharacterCount=20;

	_LIT(KDummyText,"dummy");
	CShaper::TInput input;
	input.iStart=0;
	input.iEnd=10;
	input.iText=&(KDummyText());

	//create/delete test a new shaper cache entry	
	COpenFontShaperCacheEntry* shapeEntry=COpenFontShaperCacheEntry::New(&User::Heap(),input,shapeHeader);
	COpenFontShaperCacheEntry::Delete(&User::Heap(),shapeEntry);
	
	delete shapeHeader;

	__UHEAP_MARKEND;
	}

/**
	@SYMTestCaseID
	GRAPHICS-FNTSTORE-0044

	@SYMTestCaseDesc
	Tests ncrementation and decrementation of the reference 
	count for a particular session handle.

	@SYMTestActions
	1. Creates a TShapeHeader object on the heap.
	2. Set som values to the object.
	3. Creates a CShaper object on the stack.
	4. Creates a new shaper cache entry.
	5. Associates a session with this entry.
	6. Increments the reference count for a particular session (A) handle.
	7. Increments the reference count for another session (B).
	8. Decrements an instance from session (A).
	9. Adds another session to the list (C).
	10. Decrements the ref count for a particular session handle (C).
	11. Tests decrement on non-existent session.
	12. Frees memory taken up by the entry to the cache.
	13. The heap is checked for memory leaks.
	
	@SYMTestExpectedResults
	The decrementation of the non-existent session should return KErrNotFound.
*/
void CTShaperCache::Test2L()
	{
	INFO_PRINTF1(_L("ShaperCache Test2"));
	__UHEAP_MARK;

	TShapeHeader* shapeHeader=new TShapeHeader();
	shapeHeader->iGlyphCount=10;
	shapeHeader->iCharacterCount=20;

	_LIT(KDummyText,"dummy");
	CShaper::TInput input;
	input.iStart=0;
	input.iEnd=10;
	input.iText=&(KDummyText());

	//create a new shaper cache entry	
	COpenFontShaperCacheEntry* shapeEntry=COpenFontShaperCacheEntry::New(&User::Heap(),input,shapeHeader);
	
	//now associate a session with this entry
	TInt dummySession=12;
	TEST(shapeEntry->IncRefCount(dummySession)==KErrNone);
	TEST(shapeEntry->iHandleRefCount==1);
	TEST(shapeEntry->iHandleRefList!=NULL);
	TEST(shapeEntry->iHandleRefList->iSessionHandle==dummySession);
	TEST(shapeEntry->iHandleRefList->iRefCount==1);	
	//increment for another session, the THandleCount::iRef for this session should be 2
	TEST(shapeEntry->IncRefCount(dummySession)==KErrNone);
	TEST(shapeEntry->iHandleRefCount==1);
	TEST(shapeEntry->iHandleRefList->iRefCount==2);				

	//another entry for different session
	TInt dummySession2=13;
	TEST(shapeEntry->IncRefCount(dummySession2)==KErrNone);
	TEST(shapeEntry->iHandleRefCount==2);
	TEST(shapeEntry->iHandleRefList->iSessionHandle==dummySession);
	TEST(shapeEntry->iHandleRefList->iRefCount==2);
	TEST(shapeEntry->iHandleRefList->iNext->iSessionHandle==dummySession2);
	TEST(shapeEntry->iHandleRefList->iNext->iRefCount==1);	
	
	//now decrement an instance from this session(DELETE FIRST ENTRY)
	TEST(shapeEntry->DecRefCount(dummySession)==KErrNone);
	TEST(shapeEntry->iHandleRefCount==2);
	TEST(shapeEntry->iHandleRefList->iNext->iRefCount==1);		
	TEST(shapeEntry->DecRefCount(dummySession)==KErrNone);
	TEST(shapeEntry->iHandleRefCount==1);
	TEST(shapeEntry->iHandleRefList->iRefCount==1);			
	TEST(shapeEntry->iHandleRefList->iSessionHandle=dummySession2);

	//now add another session to this list
	TInt dummySession3=14;
	TEST(shapeEntry->IncRefCount(dummySession3)==KErrNone);
	TEST(shapeEntry->iHandleRefCount==2);
	//test delete this last entry in the list(DELETE LAST ENTRY)
	TEST(shapeEntry->DecRefCount(dummySession3)==KErrNone);
	TEST(shapeEntry->iHandleRefCount==1);	
	TEST(shapeEntry->iHandleRefList->iRefCount==1);
	TEST(shapeEntry->iHandleRefList->iSessionHandle==dummySession2);


	//test Dec on non-existent session
	TEST(shapeEntry->DecRefCount(100)==KErrNotFound);

	COpenFontShaperCacheEntry::Delete(&User::Heap(),shapeEntry);
	delete shapeHeader;
	
	__UHEAP_MARKEND;		
	}
	
/**
	@SYMTestCaseID
	GRAPHICS-FNTSTORE-0045

	@SYMTestCaseDesc
	Tests the shaper cache APIs.

	@SYMTestActions
	1. Creates a dummy TShapeHeader object that will be cached and reused through the test.
	2. Gets some memory to pass back the results. It needs to be big enough for a TShapeHeader
	   object plus 10 bytes for every glyph returned.
	3. Finds or creates four font file objects to support a font file.
	4. Gets the number of supported typefaces.
	5. Creates a font.
	6. Gets a COpenFont object.
	7. For COpenFonts it attempts to put this into the shaper cache and test if it was successful.
	8. Calls DeleteShape to decrement the reference count.
	9. Changes the shaped data slightly to make it into a new shaped data and insert into the
	   cache as a new entry.
	10. Tests if the LRU cache is working by seeing if the first entry in the cache is the newest one.
	11. Calls DeleteShape to decrement the reference count
	12. Calls the GetShapedData API and tests the returned TShapeHeader. Also checks if the returned
		cache entry is at the top of the cache.
    13. Searches the cache for the first entry using the GetShapedData API.
    14. Calls DeleteShape to decrement the reference count.
    15. Brings the end value back to its original value.
	16. Fills the cache and then tests replacement policy by attempting to add a new entry.
	17. Prepares a raw input data structure.
	18. Keeps inserting pseudo-new shaped entries into the cache to fill it up.
	19. Changes the cached entries slightly to distinguish between each entry.
	20. Tries to add a new entry and checks that the entry replaced was the last entry in the cache.
	21. Calculates the memory needed for the new entry.
	22. Calculates the memory that will be freed to accommodate the new entry and the 
	    position of the last entry in the cache once deletion is done.
	23. Finds the glyphcount of the expected new last entry in the cache.
	24. Does the insertion and tests.
	25. Calls DeleteShape to decrement the reference count.
	26. Calculates the memory occupied by the last entry in the cache. This is the memory that will
		be released when the last entry is deleted.
	27. Tests with multiple open fonts with caches. Creates the new font.
	28. Does the testing only if the font is a COpenFont.
	29. Attempts to put a new entry into the second shaper cache and test if it was successful.
	30. Creates a pseudo-new shape header to put into the cache of this new open font.
	31. Inserst an entry into the cache.
	32. Tests cache memory freeing policy, i.e. delete from all other caches except the current one.
	33. Finds out how much memory needs freeing, i.e. how much was just added.
	34. Tries to free memory from the original open font cache. The result should be that
		memory should be freed from openFont2 cache, and not openFont cache.
	35. Checks
		a) if the memory used by openFont2 cache has reduced, i.e. back to original memory.
		b) if the last entry from openFont2 cache has been deleted.
		c) last entry from openFont cache is intact.
	36. Cleans up font2, font1 and removes all fonts.	
		
	@SYMTestExpectedResults
	All tests shall pass.

*/
void CTShaperCache::TestShaperCacheAPIsL()
	{
	INFO_PRINTF1(_L("The following results are for shaper cache API tests\n"));
	
	// Create a dummy TShapeHeader object that will be cached, and reused all through this test
	TShapeHeader* shape = 0;
	
	// get some memory to pass back the results,  
	// This needs to be big enough for a TShapeHeader
	// plus 10 bytes for every glyph returned (-1 for the 1 byte allocated in TShapeHeader for iBuffer)
	shape = reinterpret_cast<TShapeHeader*>( iHeap->AllocL(sizeof(TShapeHeader) + (KResultNumberOfGlyphs * 10) + 3) );

	// get the results into the shaper structure 'shape'
	shape->iGlyphCount = KResultNumberOfGlyphs;
	shape->iCharacterCount = 8;
 	shape->iReserved0 = 0;
	shape->iReserved1 = 0;

 	// iBuffer contains 10 bytes for every glyph
 	// the glyph code (4 bytes), position X(2 bytes) Y(2 bytes) and indices(2 bytes)
  	// first is glyph count * 4 byte glyph codes
 	TUint32* glyphOut = reinterpret_cast<TUint32*>(shape->iBuffer);
 	TInt16* posOut = reinterpret_cast<TInt16*>(shape->iBuffer +
 		(4 * KResultNumberOfGlyphs));
 	TInt16* indicesOut = reinterpret_cast<TInt16*>(shape->iBuffer +
 		(8 * KResultNumberOfGlyphs) + 4);
 	
 	*glyphOut++ = (TUint32)1461;
	*posOut++ = (TInt16)0;
	*posOut++ = (TInt16)0;
	*indicesOut++ = (TInt16)0;
	
	*glyphOut++ = (TUint32)1778;
	*posOut++ = (TInt16)12;
	*posOut++ = (TInt16)0;
	*indicesOut++ = (TInt16)7;
	
	*glyphOut++ = (TUint32)1550;
	*posOut++ = (TInt16)16;
	*posOut++ = (TInt16)0;
	*indicesOut++ = (TInt16)3;
	
	*glyphOut++ = (TUint32)1362;
	*posOut++ = (TInt16)28;
	*posOut++ = (TInt16)0;
	*indicesOut++ = (TInt16)7;
 		
 	// There is an extra pair of positions: this is the total advance
	posOut[0] = (TInt16)28;
	posOut[1] = (TInt16)0;
	
	TFontShapeFunctionParameters* params;
	params = reinterpret_cast<TFontShapeFunctionParameters*>( iHeap->AllocL(sizeof(TFontShapeFunctionParameters)));
	params->iEnd = 8;
	params->iLanguage = 0;
	params->iScript = KDevanagariScriptCode;
	params->iStart = 0;
	//TBufC16<9> text(KTextToShape);
	params->iText = &KTextToShape();

		// Add fonts
	TUid id1 = iFontStore->AddFileL(KFontDummy);
	TUid id2 = iFontStore->AddFileL(KFontDummy_b);
	TUid id3 = iFontStore->AddFileL(KFontDummy_i);
	TUid id4 = iFontStore->AddFileL(KFontDummy_bi);
	
	INFO_PRINTF1(_L("SHAPER CACHE API TESTS SET 1: Testing cache with a single COpentFont cache"));

	TInt typefaces = iFontStore->NumTypefaces();
	TInt typeface = 0;
	TInt height = 12;

	TTypefaceSupport support;
	iFontStore->TypefaceSupport(support,typeface);
	TFontSpec fs;
	fs.iTypeface = support.iTypeface;

	INFO_PRINTF3(_L("Typeface is %d and height is %d"), typeface, height);
	//Create a font
	CFont* font = NULL;
	fs.iHeight = iFontStore->FontHeightInTwips(typeface,height);
	// get a COpenFont object
	iFontStore->GetNearestFontToDesignHeightInPixels(font, fs);	
	CleanupStack::PushL(font);
	
	
	// Do the testing only if the font is a COpenFont
	COpenFont* openFont = NULL;
	CShaper::TInput input;
	if (((CBitmapFont*)font)->IsOpenFont())
		{
		__UHEAP_MARK;
		openFont = ((CBitmapFont*)font)->OpenFont();
		
		/***************************First Test***************************
		Attempt to put this into the shaper cache and then delete it. Test if it was successful
		*/
		TShapeHeader* cached_header = openFont->InsertShapedDataIntoCache(0,params, shape);

		INFO_PRINTF1(_L("SHAPER CACHE API TEST 1: Simple cache insertion test"));
		TEST(cached_header != NULL && openFont->GetGlyphCache()->iNumberOfShaperCacheEntries == 2);
		
		/* Call DeleteShape to decrement the reference count */
		((CBitmapFont*)font)->DeleteShape(0, cached_header);
		
		// Now delete this data
		openFont->FreeShaperCacheMemory(openFont->GetGlyphCache()->iShapingInfoCacheMemory);
		TEST(openFont->GetGlyphCache()->iNumberOfShaperCacheEntries == 1);
		// Put the shaped data back into the cache to continue with testing
		cached_header = openFont->InsertShapedDataIntoCache(0,params, shape);
		TEST(cached_header != NULL && openFont->GetGlyphCache()->iNumberOfShaperCacheEntries == 2);
		((CBitmapFont*)font)->DeleteShape(0, cached_header);		
		
		/**************************Second Test**************************
		Now change the shaped data slightly to make it into a new shaped data and insert into the
		cache as a new entry
		*/
		shape->iGlyphCount++;
		params->iEnd++;
		TShapeHeader* cached_header2 = openFont->InsertShapedDataIntoCache(0,params, shape);
		
		/* Now test if the LRU cache is working by seeing if the first entry in the cache is the newest one */
		COpenFontGlyphCache* glyphCache = openFont->GetGlyphCache();
		
		INFO_PRINTF1(_L("SHAPER CACHE API TEST 2: Test the LRU (Least Recently Used) cache structure"));
		TEST(glyphCache->iShaperCacheSentinel->iNext->iShapeHeader->iGlyphCount == KResultNumberOfGlyphs + 1);
		
		/* Call DeleteShape to decrement the reference count */
		((CBitmapFont*)font)->DeleteShape(0, cached_header2);
		
		
		/**************************Third Test**************************
		Call the GetShapedData API and test the returned TShapeHeader, and also check if the returned
		cache entry is now at the top of the cache
		*/
		
		/* Now search the cache for the first entry using the GetShapedData API */
		params->iEnd --;
		TInt expectedGlyphCount = glyphCache->iShaperCacheSentinel->iPrevious->iShapeHeader->iGlyphCount;
		TShapeHeader* searchedCachedHeader = openFont->GetShapedData(0,params);

		INFO_PRINTF1(_L("SHAPER CACHE API TEST 3: GetShapedData API test"));
		TEST(searchedCachedHeader->iGlyphCount == expectedGlyphCount &&
			 searchedCachedHeader->iGlyphCount ==glyphCache->iShaperCacheSentinel->iNext->iShapeHeader->iGlyphCount);
		
		/* Call DeleteShape to decrement the reference count */
		((CBitmapFont*)font)->DeleteShape(0, searchedCachedHeader);
		
		/* Bring the end value back to its original value */
		params->iEnd ++;
		
		
		/**************************Fourth Test**************************
		Fill the cache and then test replacement policy by attempting to add a new entry
		*/
	
		/* First prepare a raw input data structure */
		TInt memoryUsed = 0;
		input.iText = &KTextToShape();
		input.iStart = 0;
		input.iEnd = params->iEnd;
		input.iScript= KDevanagariScriptCode;
		input.iLanguage = 0;
		input.iMaximumAdvance = KMaxTInt;
		input.iFlags = 0;
		input.iSessionHandle = 0;
		input.iReserved1 = 0;
		TInt addedBytes = 0;
		TShapeHeader* chached_header3 = 0;
		openFont->FreeShaperCacheMemory(KMaxShaperSesssionCacheMemory);
		openFont->File()->GetFontStore()->SetShaperCacheMemUsage(0);
		RArray<TInt> cacheMemUsage;
		/* Keep inserting pseudo-new shaped entries into the cache to fill it up */
		while(openFont->File()->GetFontStore()->GetShaperCacheMemUsage() < KMaxShaperSesssionCacheMemory)
			{
			memoryUsed = openFont->File()->GetFontStore()->GetShaperCacheMemUsage();
			// Change the cached entries slightly to distinguish between each entry
			shape->iGlyphCount++;
			input.iEnd++;
			chached_header3 = glyphCache->Insert(0,iHeap, input, shape, addedBytes);
			if (chached_header3 != NULL)
				openFont->File()->GetFontStore()->SetShaperCacheMemUsage(memoryUsed + addedBytes);
			((CBitmapFont*)font)->DeleteShape(0, chached_header3);
			cacheMemUsage.AppendL(openFont->File()->GetFontStore()->GetShaperCacheMemUsage() - memoryUsed);
			chached_header3 = NULL;
			}
		
		/* Now try to add a new entry, and check that the entry replaced was the last entry in the cache */
		TInt last_entry_glyph_count = glyphCache->iShaperCacheSentinel->iPrevious->iShapeHeader->iGlyphCount;
		shape->iGlyphCount++;
		input.iEnd++;
		params->iEnd = input.iEnd;
		
		/* Calculate the memory needed for the new entry */

		
        TInt heapSizeBefAloc = 0;
        iHeap->AllocSize(heapSizeBefAloc);      
        COpenFontShaperCacheEntry *tempEntry = COpenFontShaperCacheEntry::New(iHeap, input, shape);
        TInt heapSizeAftAloc = 0;
        iHeap->AllocSize(heapSizeAftAloc);
        TInt bytes_needed = heapSizeAftAloc - heapSizeBefAloc;
        COpenFontShaperCacheEntry::Delete(iHeap, tempEntry);
        
		TInt bytesToDelete = 0;    
	    
		COpenFontShaperCacheEntry* previous = glyphCache->iShaperCacheSentinel->iPrevious;
		/* Calculate the memory that will be freed to accommodate the new entry,
		   and the position of the last entry in the cache once deletion is done */
		TInt index = 0;
		while (bytesToDelete <= bytes_needed)
			{

             bytesToDelete += cacheMemUsage[index++];
			 previous = previous->iPrevious;
			}
		cacheMemUsage.Close();	
		/* Find the glyphcount of the expected new last entry in the cache */
		expectedGlyphCount = previous->iShapeHeader->iGlyphCount;
		
		/* Finally, do the insertion, and test */
		TShapeHeader* cached_header4 = openFont->InsertShapedDataIntoCache(0, params, shape);
		
		INFO_PRINTF1(_L("SHAPER CACHE API TEST 4: LRU cache replacement policy test"));
		TEST(glyphCache->iShaperCacheSentinel->iPrevious->iShapeHeader->iGlyphCount == expectedGlyphCount);

		/* Call DeleteShape to decrement the reference cout */
		((CBitmapFont*)font)->DeleteShape(0, cached_header4);
		
		
		/**************************Fifth Test**************************
		Simply test the FreeShaperCacheMemory API
		*/
				
		/* Calculate the memory of occupied by the last entry in the cache. This is the memory that will
		   be released when the last entry is deleted
		 */
		last_entry_glyph_count = glyphCache->iShaperCacheSentinel->iPrevious->iShapeHeader->iGlyphCount;
		TInt bufferSize = (sizeof(TUint32) + sizeof(TInt16) + sizeof(TInt16) * 2)
				* last_entry_glyph_count + sizeof(TInt16) * 2;

		bytes_needed = bufferSize + sizeof(COpenFontShaperCacheEntry) + 
			sizeof(TShapeHeader) + sizeof(TUint16)*input.iText->Length();
		
		memoryUsed = openFont->File()->GetFontStore()->GetShaperCacheMemUsage();
		expectedGlyphCount = glyphCache->iShaperCacheSentinel->iPrevious->iPrevious->iShapeHeader->iGlyphCount;
		TInt memoryReleased = openFont->FreeShaperCacheMemory(bytes_needed);
		
		INFO_PRINTF1(_L("SHAPER CACHE API TEST 5: FreeShaperCacheMemory API test\n"));
		TEST(openFont->File()->GetFontStore()->GetShaperCacheMemUsage() == memoryUsed - memoryReleased &&
			 glyphCache->iShaperCacheSentinel->iPrevious->iShapeHeader->iGlyphCount == expectedGlyphCount);
	
		__UHEAP_MARKEND;
		}
		
	INFO_PRINTF1(_L("SHAPER CACHE API TESTS SET 2: Testing cache with a two COpentFont caches"));
	/* Now test with multiple open fonts with caches */
	/* Create the new font */
	height = 4;
	iFontStore->TypefaceSupport(support,typeface);
	fs.iTypeface = support.iTypeface;

	INFO_PRINTF3(_L("Typeface is %d and height is %d"), typeface, height);

	CFont* font2 = NULL;
	fs.iHeight = iFontStore->FontHeightInTwips(typeface,height);
	iFontStore->GetNearestFontToDesignHeightInPixels(font2, fs);	
	CleanupStack::PushL(font2);
	
	COpenFont* openFont2;
	/* Do the testing only if the font is a COpenFont */
	if (((CBitmapFont*)font2)->IsOpenFont())
		{
		__UHEAP_MARK;
		openFont2 = ((CBitmapFont*)font2)->OpenFont();
		
		/***************************Sixth Test***************************
		Attempt to put a new entry into the second shaper cache and test if it was successful
		*/
		
		/* Create a pseudo-new shape header to put into the cache of this new open font */
		shape->iGlyphCount++;
		params->iEnd++;
		
		/* Insert an entry into the cache */
		TShapeHeader* cached_header5 = openFont2->InsertShapedDataIntoCache(0, params, shape);
		((CBitmapFont*)font2)->DeleteShape(0, cached_header5);

		INFO_PRINTF1(_L("SHAPER CACHE API TEST 6: Second cache insertion test"));
		TEST(cached_header5 != NULL && openFont2->GetGlyphCache()->iNumberOfShaperCacheEntries == 2);

		/***************************Seventh, Eighth and Ninth Tests***************************
		Test cache memory freeing policy, i.e. delete from all other caches except the current one
		*/
		
		/* First find out how much memory needs freeing, i.e. how much was just added */
		TInt bufferSize = (sizeof(TUint32) + sizeof(TInt16) + sizeof(TInt16) * 2)
					* shape->iGlyphCount + sizeof(TInt16) * 2;

		TInt bytes_needed = bufferSize + sizeof(COpenFontShaperCacheEntry) + 
			sizeof(TShapeHeader) + sizeof(TUint16)*input.iText->Length();

		
		/* Now try to free memory from the original open font cache. The result should be that
		   memory should be freed from openFont2 cache, and not openFont cache
		 */
		TInt memoryUsed = openFont2->GetGlyphCache()->iShapingInfoCacheMemory;
		TInt expectedGlyphCount = openFont->GetGlyphCache()->iShaperCacheSentinel->iPrevious->iShapeHeader->iGlyphCount;
		TInt deletedMemory = openFont->FreeShaperCacheMemory(bytes_needed);
		
		/* Now check:
		   a) if the memory used by openFont2 cache has reduced, i.e. back to original memory
		   b) if the last entry from openFont2 cache has been deleted
		   c) last entry from openFont cache is intact
		*/
		INFO_PRINTF1(_L("SHAPER CACHE API TEST 7: Test if memory is freed from second COpenFont"));
		TEST(memoryUsed - openFont2->GetGlyphCache()->iShapingInfoCacheMemory ==  deletedMemory &&
			openFont2->GetGlyphCache()->iShapingInfoCacheMemory == 0);

		INFO_PRINTF1(_L("SHAPER CACHE API TEST 8: Test if last entry of second COpenFont is deleted"));
		TEST(openFont2->GetGlyphCache()->iNumberOfShaperCacheEntries == 1);

		INFO_PRINTF1(_L("SHAPER CACHE API TEST 9: Test if the last entry of the first COpenFont is intact"));
		TEST(openFont->GetGlyphCache()->iShaperCacheSentinel->iPrevious->iShapeHeader->iGlyphCount == expectedGlyphCount);
		
		/***********************************Tenth Tests*****************************************
		Test cache memory consistency and that there are no leakages when deleting memory from the cache
		*/
		
		/* Continue adding entries to second open font cache until the memory used by the first one
		   goes down to zero, at which point the number of cache entries should be 1, and memory from
		   the current open font should start getting released and eventually reach 0 when the number
		   of cached entries reaches 1.
		*/
		TShapeHeader* cached_header6 = NULL;
		/* Keep inserting pseudo-new shaped entries into the cache to fill it up */
		while(openFont->GetGlyphCache()->iNumberOfShaperCacheEntries > 1)
			{
			memoryUsed = openFont2->File()->GetFontStore()->GetShaperCacheMemUsage();
			// Change the cached entries slightly to distinguish between each entry
			shape->iGlyphCount++;
			params->iEnd++;
			cached_header6 = openFont2->InsertShapedDataIntoCache(0, params, shape);
			((CBitmapFont*)font2)->DeleteShape(0, cached_header6);
			cached_header6 = NULL;
			}
			
		INFO_PRINTF1(_L("SHAPER CACHE API TEST 10: Test consistency of cache memory"));
		TEST(openFont->GetGlyphCache()->iShapingInfoCacheMemory == 0);
		
		
		/**
		@SYMTestCaseID          GRAPHICS-SYSLIB-FNTSTORE-UT-4001
		@SYMTestCaseDesc        Test that when trying to delete a shape not in the cache it
								does not loop infinitely trying to find it.
		@SYMTestPriority        Medium
		@SYMTestActions         Pass a null shape header to the DeleteShape function.
		@SYMTestExpectedResults	The function call should return instead of looping infinitely.
		@SYMDEF                 PDEF125354
		*/
		((CTShaperCacheStep*)iStep)->RecordTestResultL();
		((CTShaperCacheStep*)iStep)->SetTestStepID(_L("GRAPHICS-SYSLIB-FNTSTORE-UT-4001"));
		INFO_PRINTF1(_L("SHAPER CACHE API TEST 11: Test that shape not found does not loop infinitely."));
		TShapeHeader* null_header = NULL;
		// Timeout set in TEF script to catch infinite loop.
		((CBitmapFont*)font2)->DeleteShape(0, null_header);
		
		((CTShaperCacheStep*)iStep)->CloseTMSGraphicsStep();
		__UHEAP_MARKEND;
		}
	
	/* Cleanup font2*/
	CleanupStack::Pop(font2);
	iFontStore->ReleaseFont(font2);
	
	/* Cleanup font1 */	
	CleanupStack::Pop(font);
	iFontStore->ReleaseFont(font);

	// Remove the fonts
	iFontStore->RemoveFile(id1);
	iFontStore->RemoveFile(id2);
	iFontStore->RemoveFile(id3);
	iFontStore->RemoveFile(id4);
	}

void CTShaperCache::RunTestCaseL(TInt aCurTestCase)
	{
	((CTShaperCacheStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);
	switch(aCurTestCase)
		{
	case 1:
		((CTShaperCacheStep*)iStep)->SetTestStepID(_L("GRAPHICS-FNTSTORE-0043"));
		INFO_PRINTF1(_L("#################### T_SHAPERCACHE test case 1 ########################\n"));
		TRAPD(err,Test1L());
		TEST(err == KErrNone);
		break;
	case 2:
		((CTShaperCacheStep*)iStep)->SetTestStepID(_L("GRAPHICS-FNTSTORE-0044"));
		INFO_PRINTF1(_L("#################### T_SHAPERCACHE test case 2 ########################\n"));
		TRAP(err,Test2L());
		TEST(err == KErrNone);
		break;
	case 3:
		((CTShaperCacheStep*)iStep)->SetTestStepID(_L("GRAPHICS-FNTSTORE-0045"));		
		INFO_PRINTF1(_L("#################### T_SHAPERCACHE test case 3 ########################"));
		TRAP(err,TestShaperCacheAPIsL());
		TEST(err == KErrNone);
		break;		
	case 4:
        	((CTShaperCacheStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
    		((CTShaperCacheStep*)iStep)->CloseTMSGraphicsStep();
		TestComplete();				
		break;
		}
	((CTShaperCacheStep*)iStep)->RecordTestResultL();
	}


//--------------
__CONSTRUCT_STEP__(ShaperCache)



