/*
* Copyright (c) 1995-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "T_FNTMEM.H"
#include "T_FSOPEN.H"
#include <graphics/openfontrasterizer.h>

#ifdef __WINS__
	_LIT(KEonFontFileName,"z:\\resource\\fonts\\eon14.gdr");
#else
	_LIT(KEonFontFileName,"z:\\resource\\fonts\\eon.gdr");
#endif

// 'dummy' open fonts, as used by T_FSOPEN
_LIT(KFontDummy,"z:\\PlatTest\\Graphics\\TestData\\dummy_fonts\\dummy");
//_LIT(KFontDummy_b,"z:\\PlatTest\\Graphics\\TestData\\dummy_fonts\\dummy_b");
//_LIT(KFontDummy_i,"z:\\PlatTest\\Graphics\\TestData\\dummy_fonts\\dummy_i");
//_LIT(KFontDummy_bi,"z:\\PlatTest\\Graphics\\TestData\\dummy_fonts\\dummy_bi");



CTFntMem::CTFntMem(CTestStep* aStep):
	CTGraphicsBase(aStep),
	iHeap(NULL)
	{
	iHeap = UserHeap::ChunkHeap(NULL,0x80000,0x80000);
	INFO_PRINTF1(_L("FontStore"));
	}

void CTFntMem::RunTestCaseL(TInt aCurTestCase)
	{
	((CTFntMemStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);
	switch(aCurTestCase)
		{
	case 1:
		((CTFntMemStep*)iStep)->SetTestStepID(_L("GRAPHICS-FNTSTORE-0024"));
		TRAPD(err,TestNewL());
		TEST(err == KErrNone);
		break;
	case 2: // OOM handling for bitmap fonts
		((CTFntMemStep*)iStep)->SetTestStepID(_L("GRAPHICS-FNTSTORE-0025"));
		TRAP(err,TestAddFileL(KEonFontFileName));
		TEST(err == KErrNone);
		break;
	case 3: // OOM handling for open fonts
		((CTFntMemStep*)iStep)->SetTestStepID(_L("GRAPHICS-FNTSTORE-0025"));
		TRAP(err,TestAddFileL(KFontDummy));
		TEST(err == KErrNone);
		break;
	case 5: // OOM handling for second reference to a bitmap font
		((CTFntMemStep*)iStep)->SetTestStepID(_L("GRAPHICS-FNTSTORE-0026"));
		TRAP(err,TestAddFileTwiceL(KEonFontFileName));
		TEST(err == KErrNone);
		break;
	case 6: // OOM handling for second reference to an open font
		((CTFntMemStep*)iStep)->SetTestStepID(_L("GRAPHICS-FNTSTORE-0026"));
		TRAP(err,TestAddFileTwiceL(KFontDummy));
		TEST(err == KErrNone);
		break;
	case 7:
		((CTFntMemStep*)iStep)->SetTestStepID(_L("GRAPHICS-FNTSTORE-0027"));
		TRAP(err,TestGetNearestFontToDesignHeightInTwipsL());
		TEST(err == KErrNone);
		break;
	case 8:
		((CTFntMemStep*)iStep)->SetTestStepID(_L("GRAPHICS-NearestOpenFontLeaksDEF095184-0001"));
		TRAP(err,TestGetNearestOpenFontLeaksL());
		TEST(err == KErrNone);
		break;
	case 9:
		((CTFntMemStep*)iStep)->SetTestStepID(_L("GRAPHICS-CTFbs-TestDuplicateFontFileEntries-0002"));
		TRAP(err,TestDuplicateBitmapFontFileEntriesL());
		TEST(err == KErrNone);
		break;
	case 10:
		((CTFntMemStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
		((CTFntMemStep*)iStep)->CloseTMSGraphicsStep();
		TestComplete();				
		break;
	default:
		((CTFntMemStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
		break;
	}
	((CTFntMemStep*)iStep)->RecordTestResultL();
	}

/**
	@SYMTestCaseID
	GRAPHICS-FNTSTORE-0024

	@SYMTestCaseDesc
	Simulates out of memory errors of the heap when trying to allocate
	memory for a CFontStore object.

	@SYMTestActions
	1. Cancels simulated heap allocation failure for the current thread's heap.
	2. Simulates heap allocation failure for the current thread's heap.
	3. Marks the start of checking the current thread's heap. 
	4. In a loop, tries to create a new CFontStore object with a pointer to 
	   the heap class used for memory allocation.
	5. Checking is done to see if the CFontStore was allocated or
	   if out of memory. The loop is broken when an object is created correctly.  	
	
	@SYMTestExpectedResults
	Test should pass
*/
void CTFntMem::TestNewL()
	{
	INFO_PRINTF1(_L("NewL"));

	TInt ret=KErrGeneral;
	CFontStore *fs=NULL;
	TInt failRate=1;

	while (ret!=KErrNone)
		{
		__UHEAP_RESET;
		__UHEAP_SETFAIL(RHeap::EDeterministic,failRate);
		__UHEAP_MARK;
		TRAP(ret,fs=CFontStore::NewL(iHeap));
		RDebug::Print(_L("fs %d failrate %d, error %d\n"),fs,failRate, ret);
		if (ret==KErrNoMemory)
			{
			__UHEAP_MARKEND;
			__UHEAP_RESET;
			TEST(fs==NULL);
			}
		else if(ret==KErrNone)
			{
			delete fs;
			__UHEAP_MARKEND;
			__UHEAP_RESET;
			__UHEAP_SETFAIL(RHeap::ENone,failRate);
			}
		else
			User::Panic(_L("Unknown error"),ret);
		failRate++;
		}
	}

/**
	@SYMTestCaseID
	GRAPHICS-FNTSTORE-0025

	@SYMTestCaseDesc
	Tests the Out Of Memory handling for bitmap- and open fonts.

	@SYMTestActions
	1. Finds or creates a font file object to support a font file.
	   If an appropriate font file object exists then no new open font file is created. 
	   In this case the reference count of the font file object is incremented.
	2. Allocates the object from the heap and then initialises its contents
	   to binary zeroes.
	3. Installs and takes ownership of an Open Font rasterizer
	4. Releases a hold on one or all font file objects (COpenFontFiles or CFontStoreFiles)
	   If aFileUid identifies a font file object, then the reference count for this 
       object is decremented. If this brings the reference count down to zero then 
       the font file object is removed from the font store, as well as the fonts 
       and typefaces associated with this file.
       If, on the other hand, aFileUid's value is NULL, then all font file objects 
	   are removed, along with all fonts and typefaces in the font store.
	5. Simulates heap allocation failure for the current thread's heap.
	6. Tries to add a font file to the font store and checks for memory allocation errors.
		
	@SYMTestExpectedResults
	Test should pass
*/
void CTFntMem::TestAddFileL(const TDesC& aFileName)
	{
	INFO_PRINTF2(_L("AddFileL(\"%S\")"), &aFileName);
	TInt failRate=1;
	CFontStore *fs=CFontStore::NewL(iHeap);

	// Install the dummy rasterizer - used for 'Open Font's
	COpenFontRasterizer* r = CDummyRasterizer::NewL();
	CleanupStack::PushL(r);
	fs->InstallRasterizerL(r);
	CleanupStack::Pop();

	(void)fs->AddFileL(aFileName);
	fs->RemoveFile(KNullUid);
	TInt ret=KErrGeneral;

	while (ret!=KErrNone)
		{
		__UHEAP_RESET;
		__UHEAP_MARK;
		__UHEAP_SETFAIL(RHeap::EDeterministic,failRate);
		TRAP(ret, (void)fs->AddFileL(aFileName));
		RDebug::Print(_L("NumTypefaces %d failrate %d, error %d\n"),fs->NumTypefaces(), failRate, ret);
		if (ret==KErrNoMemory)
			{
			fs->RemoveFile(KNullUid);
			__UHEAP_MARKEND;
			__UHEAP_RESET;
			TEST(fs->NumTypefaces()==0);
			}
		else if(ret==KErrNone)
			{
			fs->RemoveFile(KNullUid);
			delete fs;
			__UHEAP_MARKEND;
			__UHEAP_RESET;
			__UHEAP_SETFAIL(RHeap::ENone,failRate);
			}
		else
			User::Panic(_L("Unknown error"),ret);
		failRate++;
		}
	}
		
/**
	@SYMTestCaseID
	GRAPHICS-FNTSTORE-0026

	@SYMTestCaseDesc
	Tests the Out Of Memory handling for second reference to a bitmap font and open fonts.

	@SYMTestActions
	1. Finds or creates a font file object to support a font file.
	   If an appropriate font file object exists then no new open font file is created. 
	   In this case the reference count of the font file object is incremented.
	2. Allocates the object from the heap and then initialises its contents.
	   to binary zeroes.
	3. Installs and takes ownership of an Open Font rasterizer.
	4. Loads a font file.
	5. Simulates heap allocation failure for the current thread's heap.
	6. Tries to add a font file a second time to the font store and checks for memory allocation errors.
	7. Removes all font file objects.
	
	@SYMTestExpectedResults
	Test should pass
*/
void CTFntMem::TestAddFileTwiceL(const TDesC& aFileName)
	{
	INFO_PRINTF2(_L("AddFileTwiceL(\"%S\")"), &aFileName);
	TInt failRate=1;
	CFontStore *fs=CFontStore::NewL(iHeap);

	// Install the dummy rasterizer - used for 'Open Font's
	COpenFontRasterizer* r = CDummyRasterizer::NewL();
	CleanupStack::PushL(r);
	fs->InstallRasterizerL(r);
	CleanupStack::Pop();

	// load the font file
	(void)fs->AddFileL(aFileName);
	TInt ret=KErrGeneral;

	while (ret!=KErrNone)
		{
		__UHEAP_SETFAIL(RHeap::EDeterministic,failRate);
		__UHEAP_MARK;
		TRAP(ret, (void)fs->AddFileL(aFileName));
		RDebug::Print(_L("NumTypefaces %d failrate %d, error %d\n"),fs->NumTypefaces(), failRate, ret);

		// neither success or failure should use more memory
		__UHEAP_MARKEND;
		__UHEAP_RESET;

		if ( (ret != KErrNoMemory) && (ret != KErrNone) )
			{
			User::Panic(_L("Unknown error"),ret);
			}
		failRate++;
		}

	fs->RemoveFile(KNullUid);
	delete fs;
	}
	
/**
	@SYMTestCaseID
	GRAPHICS-FNTSTORE-0027

	@SYMTestCaseDesc
	Tests the function GetNearestFontToDesignHeightInTwips in Out Of Memory 
	error conditions.

	@SYMTestActions
	1. Creates a new CFontStore object.
	2. Adds a font file to the font store.
	3. Creates a bitmap font object.
	4. Gets the nearest font to design height in twips of the DejaVu Sans Condensed font.
	5. Releases a hold on one or all font file objects (COpenFontFiles or CFontStoreFiles)
	   If aFileUid identifies a font file object, then the reference count for this 
       object is decremented. If this brings the reference count down to zero then 
       the font file object is removed from the font store, as well as the fonts 
       and typefaces associated with this file.
       If, on the other hand, aFileUid's value is NULL, then all font file objects 
	   are removed, along with all fonts and typefaces in the font store.
	6. Simulates heap allocation failure for the current thread's heap.
	7. Tries to get the nearest font to design height in twips for the font and 
	   checks for memory allocation errors.
	8. Deallocates the CFontStore object.
	
	@SYMTestExpectedResults
	Test should pass
*/
void CTFntMem::TestGetNearestFontToDesignHeightInTwipsL()
	{
	INFO_PRINTF1(_L("GetNearestFontToDesignHeightInTwips"));
	CFontStore *fs=CFontStore::NewL(iHeap);
	(void)fs->AddFileL(KEonFontFileName);
	TInt failRate=1;
	CBitmapFont *font;
	TFontSpec spec(_L("DejaVu Sans Condensed"),200);
	TInt ret;
	ret=fs->GetNearestFontToDesignHeightInTwips((CFont *&) font,spec);
	TEST(ret==KErrNone);
	if (ret!=KErrNone || font==NULL)
		return;	//otherwise the memory faulting test loop will deadlock!
	fs->ReleaseFont(font);
	ret=KErrNoMemory;
	while(ret!=KErrNone)
		{
		__UHEAP_RESET;
		__UHEAP_SETFAIL(RHeap::EDeterministic,failRate);
		__UHEAP_MARK;
		__RHEAP_MARK(iHeap);
		font=NULL;
		ret=fs->GetNearestFontToDesignHeightInTwips((CFont *&) font,spec);
		RDebug::Print(_L("Font %d failrate %d\n"),font,failRate);
		if (ret==KErrNoMemory)
			{
			TEST(font==NULL);
			__RHEAP_MARKEND(iHeap);
			__UHEAP_MARKEND;
			}
		else if(ret==KErrNone)
			{
			fs->ReleaseFont(font);
			font=NULL;
			__RHEAP_MARKEND(iHeap);
			__UHEAP_MARKEND;
			__UHEAP_SETFAIL(RHeap::ENone,failRate);
			}
		else
			User::Panic(_L("Unknown error"),ret);
		failRate++;
		}
				 
	delete fs;
	}
//----------------
/**
@SYMTestCaseID GRAPHICS-NearestOpenFontLeaksDEF095184-0001
@SYMTestPriority Med
@SYMREQ DEF095184

@SYMTestCaseDesc
Leak in font finder

@SYMTestActions
Tests for leak case loading both nearest open and bitmap font.
The first call to GetNearestFontToDesignHeightInTwips loads the bitmap font.
The looped calls change the memory fault frequency until this succeeds, 
but the open font is returned instead, and then until the bitmap font is again returned.
The test fontspec is specially chosen to cause these different results,
 and the test code is not working if the differing results are not fetched.

@SYMTestExpectedResults
failrate 1: KErrNoMemory
failrate 2: Same font returned
failrate 3: Different font returned
failrate 4: Same font returned
No leaks.
Note that the exact results may change fail rates 
if underlying changes are made to the font store.

**/
void CTFntMem::TestGetNearestOpenFontLeaksL()
	{
	INFO_PRINTF1(_L("TestGetNearestOpenFontLeaksL"));
	CFontStore *fs=CFontStore::NewL(iHeap);
	CleanupStack::PushL(fs);
	// Install the dummy rasterizer - used for 'Open Font's
	COpenFontRasterizer* r = CDummyRasterizer::NewL();
	CleanupStack::PushL(r);
	fs->InstallRasterizerL(r);
	//Need one of each font flavour
	(void)fs->AddFileL(KFontDummy);
	(void)fs->AddFileL(KEonFontFileName);
	CleanupStack::Pop(r);
	CleanupStack::Pop(fs);
	bool differentResultFetched=false;	
	CBitmapFont *firstFont;

	//Manufacture this font so that the bitmap font is favoured
	//Note that the best I could do was get them equal, but that is enough to get bitmap chosen!
	//Blank name is required to allow the dummy open font to be selected by similarity
	TFontSpec reqSpec(_L(""),190);
	reqSpec.iFontStyle.SetBitmapType(EMonochromeGlyphBitmap);
	TFontSpec firstSpec;
	TInt ret;	
	//first run - tests that the code actually works before faulting the memory 
	ret=fs->GetNearestFontToDesignHeightInTwips((CFont *&) firstFont,reqSpec);
	TEST(ret==KErrNone);
	TEST(firstFont!=NULL);
	if (ret!=KErrNone || firstFont==NULL)
		{
		return;	//otherwise the memory faulting test loop will deadlock!
		}
	if (firstFont)
		{
		firstSpec=firstFont->FontSpecInTwips();
		fs->ReleaseFont(firstFont);
		}
	ret=KErrNoMemory;
	TInt failRate=1;
	while(ret!=KErrNone || (failRate<30 && !differentResultFetched))
		{
		CBitmapFont *font=NULL;
		__UHEAP_RESET;
		__RHEAP_SETFAIL(iHeap,RHeap::EDeterministic,failRate);
		__UHEAP_MARK;
		__RHEAP_MARK(iHeap);
		TFontSpec spec(reqSpec);
		ret=fs->GetNearestFontToDesignHeightInTwips((CFont *&) font,reqSpec);
		if (ret==KErrNoMemory)
			{
			RDebug::Print(_L("failrate %d: KErrNoMemory\n"),failRate);
			TEST(font==NULL);
			}
		else if(ret==KErrNone)
			{
			TEST(font!=NULL);
			if (font)
				{
				TFontSpec spec=font->FontSpecInTwips();
				if (!(spec==firstSpec))
					{
					differentResultFetched=true;
					ret=KErrNoMemory;			//actually there was a handled memory fault
					RDebug::Print(_L("failrate %d: Different font returned\n"),failRate);
					}
				else
					{
					RDebug::Print(_L("failrate %d: Same font returned\n"),failRate);
					}
				}
			else
				{
				RDebug::Print(_L("failrate %d: Error: NULL font returned\n"),failRate);
				}
			fs->ReleaseFont(font);
			font=NULL;
			}
		else
			{
			User::Panic(_L("Unexpected error"),ret);
			}
		__RHEAP_CHECK(iHeap,0);
		__RHEAP_MARKEND(iHeap);
		__UHEAP_MARKEND;
		__RHEAP_SETFAIL(iHeap,RHeap::ENone,failRate);
		failRate++;
		}
	if (!differentResultFetched)
		{
		INFO_PRINTF1(_L("Test did not return different results under memory stress (unexpected)"));
		}
	delete fs;
	}


//The "bad" versions of these standard fonts have been hacked to give specific results
_LIT(KFBSERVFontFileBitmap, 	"Z:\\resource\\fonts\\ceurope.GDR");
//This file has the master GUID nobbled, but all the guid and typeface entries are duplicates.
_LIT(KFBSERVFontFileBadBitmap1, 	"Z:\\PlatTest\\Graphics\\TestData\\uniquified_fonts\\XXeuro.GDR");
//This file has the master GUID nobbled, one of the 8 fonts, and one of the typefaces has also been nobbled.
_LIT(KFBSERVFontFileBadBitmap2, 	"Z:\\PlatTest\\Graphics\\TestData\\uniquified_fonts\\YYeuro.GDR");
enum	{KNobbledFontEnum=0x10ff5912};		//official code for LatinBold17 is 0x10005912
_LIT(NobbledTypefaceName, 	"XatinBold17");


 /**
   @SYMTestCaseID GRAPHICS-CTFbs-TestDuplicateFontFileEntries-0002
  
   @SYMDEF  DEF094692
  
   @SYMTestCaseDesc  If Bitmap fonts are loaded that are marginally unique compared to the 
   					 existing fonts then they are not actually added to the typeface system, and may even be discarded.
   
   @SYMTestPriority Med 
  
   @SYMTestStatus Implemented
   
   @SYMTestActions \n
   		The original versions of the Bitmapfonts are first loaded to ensure that they have actually been loaded.
   		A version of the bitmap font which is identical except for the file UID should be rejected
   		Verify that the unique data for the next test does not already exist
   		A version of the bitmap font which has just 1 unique bitmap will be loaded, but only 1 typeface entry will be added.
   		Verify that the unique data for the previous test now does exist
   	    The memory allocations through these steps should be deterministic
   			
   API Calls:	AddFile\n	
      
   @SYMTestExpectedResults The test expects:
   		The original versions should load without changing the number of typefaces
   		The second load should return the same UID for the font
   		The identical fonts should return captured fail codes, and not change the typeface counts
   		The semi-identical font should perform a single increase to UIDs and typefaces
  */
void CTFntMem::TestDuplicateBitmapFontFileEntriesL()
	{
	
	INFO_PRINTF1(_L("Test Load semi-duplicate bitmap font files"));
	__RHEAP_MARK(iHeap);
	__UHEAP_MARK;
	CFontStore *fs=CFontStore::NewL(iHeap);
	CleanupStack::PushL(fs);
	
	TInt numFacesBefore=fs->NumTypefaces();
	TUid aIdBitMap1=TUid::Null();
	TUint numAllocsBefore=iHeap->Count();
	TUid aIdBitMap2=TUid::Null();
	TUid aIdBitMap3=TUid::Null();
	//make sure the originals of these fonts were safely opened
	//both should simply cause a reference-count increase on the originator files.
	TRAPD(err1,aIdBitMap1=fs->AddFileL(KFBSERVFontFileBitmap));
	TEST(err1==KErrNone);
	if (err1)
		{
		INFO_PRINTF1(_L("One of the expected fonts was missing. Test abandoned."));
		}
	else
		{
		TInt numFacesAfterLoadOriginals=fs->NumTypefaces();
		TEST(numFacesBefore<numFacesAfterLoadOriginals);
		TUint numAllocsAfterLoadOriginals=iHeap->Count();
		
		
		//This bitmap font file contains no new fonts, so will ultimately be discarded
		//The master UID has been nobbled so the file is not considered identical to its originator
		TRAPD(err3,aIdBitMap2=fs->AddFileL(KFBSERVFontFileBadBitmap1));
		TEST(err3==KErrAlreadyExists && aIdBitMap2==TUid::Null());
		if (err3!=KErrAlreadyExists)
				INFO_PRINTF2(_L("Unexpected error code was %d"),err3);	 
		TInt numFacesAfterLoadBadFile3=fs->NumTypefaces();
		TEST(numFacesAfterLoadOriginals==numFacesAfterLoadBadFile3);
		TUint numAllocsAfterLoadBadFile3=iHeap->Count();
		TEST(numAllocsAfterLoadBadFile3==numAllocsAfterLoadOriginals);
		
		//Before loading the next test check that the new data about to be added does not already exist
		CFont *font=NULL;
		TAlgStyle algStyle;
		TFontSpec fontSpec(NobbledTypefaceName,10);
		
		TInt err5a1=fs->GetFontById(font,TUid::Uid(KNobbledFontEnum),algStyle);
		TEST(font==NULL);	//the nobbled UID should not already exist

		font=NULL;
		
		TInt err5a2=fs->GetNearestFontInPixels(font, fontSpec);
		if (font)
			{
			TFontSpec specNoMatch=font->FontSpecInTwips();
			TEST(specNoMatch.iTypeface.iName!=NobbledTypefaceName);	 //Nobbled font name should not be known yet
			fs->ReleaseFont(font);
			}
		
		//This bitmap font file contains one new typeface and font
		//Actually, they are bit file edited versions of existing ones!
		TInt numAllocsBeforeLoadBadFile5 = iHeap->Count();
		TRAPD(err5,aIdBitMap3=fs->AddFileL(KFBSERVFontFileBadBitmap2));
		TEST(err5==KErrNone); 
		if (err5!=KErrNone)
				INFO_PRINTF2(_L("Unexpected error code was %d"),err5);	 
		
		//The number of typefaces has now increased
		TInt numFacesAfterLoadBadFile5=fs->NumTypefaces();
		TEST(numFacesAfterLoadOriginals+1==numFacesAfterLoadBadFile5);
		TUint numAllocsAfterLoadBadFile5=iHeap->Count();
		TEST(numAllocsAfterLoadBadFile5==numAllocsBeforeLoadBadFile5+2);
		

		TInt err5b1=fs->GetFontById(font,TUid::Uid(KNobbledFontEnum),algStyle);
		TEST(err5b1==KErrNone && font!=NULL);	//the nobbled UID should now exist
		if (err5b1!=KErrNone)
				INFO_PRINTF2(_L("Unexpected error code was %d"),err5b1);	 
		
		CFont *font2=NULL;
		TInt err5b2=fs->GetNearestFontInPixels(font2, fontSpec);
		TEST(err5b2==KErrNone && font2!=NULL);	
		if (err5b2!=KErrNone)
				INFO_PRINTF2(_L("Unexpected error code was %d"),err5b2);	 
		if (font2!=NULL)
			{
			TFontSpec specMatches=font->FontSpecInTwips();
			TEST(specMatches.iTypeface.iName==NobbledTypefaceName);	   //the nobbled typeface should now exist
			}
		TEST(font==font2);
		if (font)
			fs->ReleaseFont(font);
		if (font2)
			fs->ReleaseFont(font2);
		}
	
	if (aIdBitMap1!=TUid::Null())	fs->RemoveFile(aIdBitMap1);
	if (aIdBitMap2!=TUid::Null())	fs->RemoveFile(aIdBitMap2);
	if (aIdBitMap3!=TUid::Null())	fs->RemoveFile(aIdBitMap3);
	
	//The added typeface should have been uninstalled
	TInt numFacesAfterUninstall=fs->NumTypefaces();
	TEST(numFacesAfterUninstall==numFacesBefore);
	TUint numAllocsAfterUninstall=iHeap->Count();
	
	CleanupStack::PopAndDestroy(fs);
	__UHEAP_CHECK(0);
	__UHEAP_MARKEND;
	__RHEAP_CHECK(iHeap,0);
	__RHEAP_MARKEND(iHeap);
	
	}


//--------------
__CONSTRUCT_STEP__(FntMem)


