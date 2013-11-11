/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Tests FNTSTORE with respect to the Open Font system.
* Creates a dummy rasterizer and tests font file loading and unloading and
* font creation. This just exercises the way the Open Font system connects to the
* 'classic' bitmap-font-only font system.
*
*/


/**
 @file
 @test
 @internalComponent Internal Symbian test code
*/

#include "T_FSOPEN.H"
#include <e32std.h>
#include <graphics/shaperparams.h>
#include <graphics/fbsdefs.h>

#ifdef _DEBUG
_LIT(KFBSERVFontDir,"\\resource\\fonts\\");
#endif// _DEBUG


_LIT(KFontDummy,"z:\\PlatTest\\Graphics\\TestData\\dummy_fonts\\dummy");
_LIT(KFontDummy_b,"z:\\PlatTest\\Graphics\\TestData\\dummy_fonts\\dummy_b");
_LIT(KFontDummy_i,"z:\\PlatTest\\Graphics\\TestData\\dummy_fonts\\dummy_i");
_LIT(KFontDummy_bi,"z:\\PlatTest\\Graphics\\TestData\\dummy_fonts\\dummy_bi");

const TUint32 KDevanagariScriptCode = 0x64657661;
_LIT16(KTextToShape, "\x0915\x094D\x0937\x0924\x094D\x0930\x093F\x092F");




CDummyShaperFactory* CDummyShaperFactory::NewL()
	{
	CDummyShaperFactory* r = new(ELeave) CDummyShaperFactory;
	return r;
	}

CDummyShaperFactory::CDummyShaperFactory()
	{
	}

CDummyShaperFactory::~CDummyShaperFactory()
	{
	
	}


CShaper* CDummyShaperFactory::NewShaperL(CBitmapFont* aBitmapfont, TInt aScript, TInt aLanguage, RHeap* aHeap)
	{
	return CDummyShaper::NewL(aBitmapfont, aScript, aLanguage, aHeap);
	} 

void* CShaperFactory::ExtendedInterface(TUid /*aInterfaceId*/)
	{
	return 0;
	}


CShaper * CDummyShaper::NewL(CBitmapFont* aBitmapfont, TInt aScript, TInt aLanguage, RHeap* aHeap)
	{
	CDummyShaper* newShaper = new(ELeave)CDummyShaper();
	CleanupStack::PushL(newShaper);
	TInt error = newShaper->ConstructL(aBitmapfont, aScript, aLanguage, aHeap);
	
	// if the layout engine fails to construct with this font return NULL
	if (error == KErrNone )
		{
		CleanupStack::Pop(); // newShaper
		return newShaper;
		}
	else
		{
		CleanupStack::PopAndDestroy(); 
		return NULL;		
		}		
	} 


/**
Construct an instance of CDummyShaper 	
@param aOpenFont The required font
@param aHeap The heap to be used for storage by the engine
@return KErrNone if successful or a system wide error code. 
Note that KErrGeneral may be returned in certain out of memory situations.
@see CShaper
 */
 TInt CDummyShaper::ConstructL(CBitmapFont* /*aBitmapfont*/, TInt /*aScript*/, TInt /*aLanguage*/, RHeap* /*aHeap*/ )
	{
	return KErrNone;
	}

CDummyShaper::CDummyShaper()
	{
	}	
		
/** 
 Frees all resources owned by ...
 */
 CDummyShaper::~CDummyShaper()
	{
	}	
	
/** This is implementation of CShaper::ShapeText for the Icu layout Engine
 The data is taken from TInput and pass to the shaper.
 A memory buffer is allocated on aHeapForOutput starting with TShapeHeader is allocated.
 The results of the shaping are copied into this buffer and passed back via aOutput. 
 @param aOutput On success a new structure containing the results allocated on aHeapForOutput.  
 @param aInput The input text and other parameters.
 @param aHeapForOutput On success, aOutput should be allocated from this and nothing else. 
 		On failure, nothing should be allocated from it.
 @return Error value from one of the system-wide error codes on	failure, KErrNone on success.
 @see CShaper::ShapeText
 */		
TInt CDummyShaper::ShapeText(TShapeHeader*& /*aOutput*/, const TInput& /*aInput*/, RHeap* /*aHeapForOutput*/)
	{
	// This shaper doesnt return any results
 	return KErrNotSupported;
	}

class CDummyFontFile: public COpenFontFile
	{
public:
	static CDummyFontFile* NewL(TInt aUid,const TDesC& aFileName);
	TBool GetNearestFontHelperOld(const TOpenFontSpec& aDesiredFontSpec,TInt aPixelWidth,TInt aPixelHeight,
										TInt& aFaceIndex,TOpenFontSpec& aActualFontSpec) const;
private:
	CDummyFontFile(TInt aUid,const TDesC& aFileName);
	void ConstructL();
	// From COpenFontFile
	void GetNearestFontInPixelsL(RHeap* aHeap,COpenFontSessionCacheList* aSessionCacheList,
								 const TOpenFontSpec& aDesiredFontSpec,TInt aPixelWidth,TInt aPixelHeight,
								 COpenFont*& aFont,TOpenFontSpec& aActualFontSpec);
	void GetNearestFontToDesignHeightInPixelsL(RHeap* aHeap,COpenFontSessionCacheList* aSessionCacheList,
								 const TOpenFontSpec& aDesiredFontSpec,TInt aPixelWidth,TInt aPixelHeight,
								 COpenFont*& aFont,TOpenFontSpec& aActualFontSpec);
	void GetNearestFontToMaxHeightInPixelsL(RHeap* aHeap,COpenFontSessionCacheList* aSessionCacheList,
								 const TOpenFontSpec& aDesiredFontSpec,TInt aPixelWidth,TInt aPixelHeight,
								 COpenFont*& aFont,TOpenFontSpec& aActualFontSpec, TInt aMaxHeight);
	virtual TBool HasUnicodeCharacterL(TInt aFaceIndex,TInt aCode) const;
	};

class CDummyFont: public COpenFont
	{
public:
	static CDummyFont* NewL(RHeap* aHeap,COpenFontSessionCacheList* aSessionCacheList,CDummyFontFile* aFontFile,TInt aSizeInPixels);
private:
	CDummyFont(RHeap* aHeap,COpenFontSessionCacheList* aSessionCacheList,CDummyFontFile* aFontFile,TInt aSizeInPixels);
	// From COpenFont
	virtual void RasterizeL(TInt aCode,TOpenFontGlyphData* aGlyphData);
	};

CTFsOpen::CTFsOpen(CTestStep* aStep) :
	CTGraphicsBase(aStep)
	{
	INFO_PRINTF1(_L("FontStore and the Open Font System"));
	}

CTFsOpen::~CTFsOpen()
	{
	iFilesys.Close();
	delete iFontStore;
	iHeap->__DbgMarkEnd(0);
	iHeap->Close();
	__UHEAP_MARKEND;
	User::Heap().Check();
	}

void CTFsOpen::ConstructL()
	{
	__UHEAP_MARK;
	iHeap = UserHeap::ChunkHeap(NULL,0x10000,0x10000);
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

void CTFsOpen::RunTestCaseL(TInt aCurTestCase)
	{
	((CTFsOpenStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);
	switch(aCurTestCase)
		{
	case 1:
		((CTFsOpenStep*)iStep)->SetTestStepID(_L("GRAPHICS-FNTSTORE-0031"));
		AddAndRemoveFilesL();
		break;
	case 2:
		((CTFsOpenStep*)iStep)->SetTestStepID(_L("GRAPHICS-FNTSTORE-0032"));
		CreateAndReleaseFontsL();
		break;
	case 3:
		((CTFsOpenStep*)iStep)->SetTestStepID(_L("GRAPHICS-FNTSTORE-0033"));
#ifdef _DEBUG
		TestGetNearestFontHelperL();
#endif //_DEBUG
		break;
	case 4:
		((CTFsOpenStep*)iStep)->SetTestStepID(_L("GRAPHICS-SYSLIB-FNTSTORE-UT-1498"));
		CreateShaperL();
		break;
	case 5:
        	((CTFsOpenStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
		((CTFsOpenStep*)iStep)->CloseTMSGraphicsStep();
		TestComplete();
		break;
		}
	((CTFsOpenStep*)iStep)->RecordTestResultL();
	}

/**
	@SYMTestCaseID
	GRAPHICS-FNTSTORE-0031

	@SYMTestCaseDesc
	Tests adding and removing files from the font store.

	@SYMTestActions
	1. Finds or creates a font file object to support a font file. 
	   If an appropriate font file object exists then no new open font file is created. 
	   In this case the reference count of the font file object is incremented.
	2. Gets the number of typefaces held in the font store.
	3. Checks all fonts are in the same typeface family.
	4. Checks all fonts have different FontStore ids.
	5. Tries to add a font (which has been added once already) and checks it gets 
	   the same id as when it was added.
	6. Releases four load fonts.
	7. Checks font that was loaded twice should still be loaded.
	8. Removes last reference a font and checks all fonts are unloaded correctly.

	@SYMTestExpectedResults
	Test should pass
*/
void CTFsOpen::AddAndRemoveFilesL()
	{
	TEST(iFontStore->NumTypefaces() == 0);

	INFO_PRINTF1(_L("loading dummy Open Fonts\n"));
	
	TUid id1 = iFontStore->AddFileL(KFontDummy);
	TUid id2 = iFontStore->AddFileL(KFontDummy_b);
	TUid id3 = iFontStore->AddFileL(KFontDummy_i);
	TUid id4 = iFontStore->AddFileL(KFontDummy_bi);

	// all fonts are in the same typeface family
	INFO_PRINTF1(_L("increasing reference count for for an Open Font file\n"));
	TEST(iFontStore->NumTypefaces() == 1);

	// ids should each be different
	TEST(id1 != id2);
	TEST(id1 != id3);
	TEST(id1 != id4);
	TEST(id2 != id3);
	TEST(id2 != id4);
	TEST(id3 != id4);

	// ask for one of the font files again
	TUid id5 = iFontStore->AddFileL(KFontDummy_bi);

	// should get the same UID as the first time
	TEST(id4 == id5);

	INFO_PRINTF1(_L("unloading dummy Open Fonts\n"));
	iFontStore->RemoveFile(id1);
	iFontStore->RemoveFile(id2);
	iFontStore->RemoveFile(id3);
	iFontStore->RemoveFile(id4);

	// font that was loaded twice should still be loaded
	TEST(iFontStore->NumTypefaces() == 1);

	// remove last reference
	iFontStore->RemoveFile(id5);

	// all fonts unloaded
	TEST(iFontStore->NumTypefaces() == 0);
	}

void CTFsOpen::TestFont(const TFontSpec& aSpec)
	{
	iHeap->__DbgMarkStart();

	CFont* font = NULL;
	TInt error = iFontStore->GetNearestFontToDesignHeightInTwips(font,aSpec);
	TEST(error == KErrNone);
	TEST(font != NULL);
	INFO_PRINTF2(_L("desired font spec: %S: "),&aSpec.iTypeface.iName);
	if (aSpec.iFontStyle.StrokeWeight() == EStrokeWeightBold)
		INFO_PRINTF1(_L("bold "));
	if (aSpec.iFontStyle.Posture() == EPostureItalic)
		INFO_PRINTF1(_L("italic "));
	INFO_PRINTF2(_L("%dpt"),(aSpec.iHeight + 10) / 20);
	INFO_PRINTF1(_L("\n"));
	TFontSpec actual_spec = font->FontSpecInTwips();
	INFO_PRINTF2(_L("actual font spec: %S: "),&actual_spec.iTypeface.iName);
	if (actual_spec.iFontStyle.StrokeWeight() == EStrokeWeightBold)
		INFO_PRINTF1(_L("bold "));
	if (actual_spec.iFontStyle.Posture() == EPostureItalic)
		INFO_PRINTF1(_L("italic "));
	INFO_PRINTF2(_L("%dpt"),(actual_spec.iHeight + 10) / 20);
	INFO_PRINTF1(_L("\n"));
	iFontStore->ReleaseFont(font);

	TEST (iHeap->__DbgMarkEnd(0) == 0);
	}

/**
	@SYMTestCaseID
	GRAPHICS-FNTSTORE-0032

	@SYMTestCaseDesc
	Creates fonts and tests their properties
	and then releases them.

	@SYMTestActions
	1. Adds four files to the font store.
	2. Retrieves number of typefaces.
	3. For all of the typefaces available it gets
	   the typeface support and retrieves its font height
	   in twips.
	4. Tests the fonts properties.
	5. Releases a hold on all font file objects allocated.
	
	@SYMTestExpectedResults
	Test should pass
*/
void CTFsOpen::CreateAndReleaseFontsL()
	{
	TUid id1 = iFontStore->AddFileL(KFontDummy);
	TUid id2 = iFontStore->AddFileL(KFontDummy_b);
	TUid id3 = iFontStore->AddFileL(KFontDummy_i);
	TUid id4 = iFontStore->AddFileL(KFontDummy_bi);

	int typefaces = iFontStore->NumTypefaces();
	for (int typeface = 0; typeface < typefaces; typeface++)
		{
		TTypefaceSupport support;
		iFontStore->TypefaceSupport(support,typeface);
		TFontSpec fs;
		fs.iTypeface = support.iTypeface;
		for (int height = 0; height < support.iNumHeights; height++)
			{
			fs.iHeight = iFontStore->FontHeightInTwips(typeface,height);
			TestFont(fs);
			}
		}

	iFontStore->RemoveFile(id1);
	iFontStore->RemoveFile(id2);
	iFontStore->RemoveFile(id3);
	iFontStore->RemoveFile(id4);
	}
	
/**
@file
@SYMTestCaseID				GRAPHICS-SYSLIB-FNTSTORE-UT-1498
@SYMTestCaseDesc 			Test the Shaper API  
@SYMTestPriority 			High
@SYMTestActions  			This test checks the CBitmapFont::ShapeTextL() using a dummy font and a dummy shaper
@SYMTestExpectedResults		The test must not fail.
@SYMPREQ					PREQ18
*/
void CTFsOpen::CreateShaperL()
	{
	__UHEAP_MARK;
	INFO_PRINTF1(_L("Creating Shaper and Shaping Text\n"));
	
	// Add fonts
	TUid id1 = iFontStore->AddFileL(KFontDummy);
	TUid id2 = iFontStore->AddFileL(KFontDummy_b);
	TUid id3 = iFontStore->AddFileL(KFontDummy_i);
	TUid id4 = iFontStore->AddFileL(KFontDummy_bi);
	
	TInt error = 0;
	TInt err = 0;
	int typefaces = iFontStore->NumTypefaces();
	for (TInt typeface = 0; typeface < typefaces; typeface++)
		{
		TTypefaceSupport support;
		iFontStore->TypefaceSupport(support,typeface);
		TFontSpec fs;
		fs.iTypeface = support.iTypeface;
		for (TInt height = 0; height < support.iNumHeights; height++)
			{
			//Create a font
			CFont* font = NULL;
			fs.iHeight = iFontStore->FontHeightInTwips(typeface,height);
			// get a COpenFont object
			iFontStore->GetNearestFontToDesignHeightInPixels(font, fs);	
			CleanupStack::PushL(font);
			
			 //do some dummy shaping
			TShapeHeader* shape = 0;
						
			TShapeMessageParameters params;
			params.iStart = 0;
			params.iEnd = 8;
			params.iScript = KDevanagariScriptCode;
			params.iLanguage = 0;
			
					
			TRAP(err, shape = ((CBitmapFont*)font)->ShapeTextL(KTextToShape, 0, params));
			TEST(err == KErrNotSupported);
					
			// Now do some dummy deletion of the TShapeHeader
			TInt dummyHandle=0;
			TRAP(error, ((CBitmapFont*)font)->DeleteShape(dummyHandle,shape));
			TEST(error == KErrNone);
			CleanupStack::Pop(font);
			iFontStore->ReleaseFont(font);
			}
		}

	// Remove the fonts
	iFontStore->RemoveFile(id1);
	iFontStore->RemoveFile(id2);
	iFontStore->RemoveFile(id3);
	iFontStore->RemoveFile(id4);	

	__UHEAP_MARKEND;	
	}
	
/** End of Shaper Tests*/

/**
	@SYMTestCaseID
	GRAPHICS-FNTSTORE-0033

	@SYMTestCaseDesc
	Tests the GetNearestFontHelper function. This function may be used by 
	derived classes in their GetNearestFontInPixelsL() 
	implementations. It finds the nearest font in the typeface attribute array, 
	if any, to the provided font specification. If there is a possible match it 
	places the face index in aFaceIndex and the actual specification (including 
	algorithmic effects) in aActualFontSpec.

	@SYMTestActions
	1. Connects to the file system.
	2. Finds a specified file.
	3. Retrieves a font file.
	4. Sets different font attributes.
	5. Calls the GetNearestFontHelper and GetNearestFontHelperOld functions.
	6. Checks if the two functions give the same result.
		
	@SYMTestExpectedResults
	Test should pass
*/
#ifdef _DEBUG
void CTFsOpen::TestGetNearestFontHelperL()
	{
	INFO_PRINTF1(_L("GetNearestFontHelper\n"));
	User::LeaveIfError(iFilesys.Connect());
	TFindFile file_finder(iFilesys);
	CDir* file_list = NULL;
	_LIT(KFBSERVFontFilePattern, "*.ttf");
	TInt error = file_finder.FindWildByDir(KFBSERVFontFilePattern,KFBSERVFontDir,file_list);
	while (!error)
		{
		CleanupStack::PushL(file_list);
		for (int i = 0; i < file_list->Count(); i++)
			{
			TParse parse;
			if (parse.Set((*file_list)[i].iName,&file_finder.File(),NULL) == KErrNone)
				{
				TPtrC name = parse.Name();
				INFO_PRINTF2(_L("Font file: %S\r\n"), &name);
				CDummyFontFile* font_file = CDummyFontFile::NewL(0,parse.FullName());

				TOpenFontSpec font_spec1;
				TOpenFontSpec font_spec2_new;
				TOpenFontSpec font_spec2_old;
				
				TInt face_index = 0;

				font_spec1.SetName(KNullDesC);
				font_spec1.SetHeight(10);

				font_spec1.SetCoverage(0,0,0,0);
				font_spec1.SetBold(EFalse);
				font_spec1.SetItalic(EFalse);
				font_spec1.SetSerif(EFalse);
				font_spec1.SetMonoWidth(EFalse);
				TBool res1 = font_file->GetNearestFontHelper(font_spec1,0,0,face_index,font_spec2_new);
				TBool res2 = font_file->GetNearestFontHelperOld(font_spec1,0,0,face_index,font_spec2_old);
				TEST(res2 ? res1 && (font_spec2_new == font_spec2_old) : ETrue);

				font_spec1.SetBold(ETrue);
				res1 = font_file->GetNearestFontHelper(font_spec1,0,0,face_index,font_spec2_new);
				res2 = font_file->GetNearestFontHelperOld(font_spec1,0,0,face_index,font_spec2_old);
				TEST(res2 ? res1 && (font_spec2_new == font_spec2_old) : ETrue);

				font_spec1.SetItalic(ETrue);
				res1 = font_file->GetNearestFontHelper(font_spec1,0,0,face_index,font_spec2_new);
				res2 = font_file->GetNearestFontHelperOld(font_spec1,0,0,face_index,font_spec2_old);
				TEST(res2 ? res1 && (font_spec2_new == font_spec2_old) : ETrue);

				font_spec1.SetSerif(ETrue);
				res1 = font_file->GetNearestFontHelper(font_spec1,0,0,face_index,font_spec2_new);
				res2 = font_file->GetNearestFontHelperOld(font_spec1,0,0,face_index,font_spec2_old);
				TEST(res2 ? res1 && (font_spec2_new == font_spec2_old) : ETrue);

				font_spec1.SetMonoWidth(ETrue);
				res1 = font_file->GetNearestFontHelper(font_spec1,0,0,face_index,font_spec2_new);
				res2 = font_file->GetNearestFontHelperOld(font_spec1,0,0,face_index,font_spec2_old);
				TEST(res2 ? res1 && (font_spec2_new == font_spec2_old) : ETrue);

				font_spec1.SetCoverage(8);
				font_spec1.SetBold(EFalse);
				font_spec1.SetItalic(EFalse);
				font_spec1.SetSerif(EFalse);
				font_spec1.SetMonoWidth(EFalse);
				res1 = font_file->GetNearestFontHelper(font_spec1,0,0,face_index,font_spec2_new);
				res2 = font_file->GetNearestFontHelperOld(font_spec1,0,0,face_index,font_spec2_old);
				TEST(res2 ? res1 && (font_spec2_new == font_spec2_old) : ETrue);

				delete font_file;
				}
			}
		CleanupStack::PopAndDestroy(); // file_list
		if (!error)
			error = file_finder.FindWild(file_list);
		}
	}
#endif //_DEBUG

CDummyRasterizer* CDummyRasterizer::NewL()
	{
	return new(ELeave) CDummyRasterizer;
	}

COpenFontFile* CDummyRasterizer::NewFontFileL(TInt aUid,const TDesC& aFileName,RFs& /*aFileSession*/)
	{
	// Allow 'dummy', 'dummy_b', etc. Don't actually open a file; just create a dummy font.
	if (aFileName.CompareF(KFontDummy) == 0 ||
		aFileName.CompareF(KFontDummy_b) == 0 ||
		aFileName.CompareF(KFontDummy_i) == 0 ||
		aFileName.CompareF(KFontDummy_bi) == 0)
		return CDummyFontFile::NewL(aUid,aFileName);
	return NULL;
	}

CDummyFontFile::CDummyFontFile(TInt aUid,const TDesC& aFileName):
	COpenFontFile(aUid,aFileName)
	{
	}

CDummyFontFile* CDummyFontFile::NewL(TInt aUid,const TDesC& aFileName)
	{
	CDummyFontFile* f = new(ELeave)CDummyFontFile(aUid,aFileName);
	CleanupStack::PushL(f);
	f->ConstructL();
	CleanupStack::Pop();
	return f;
	}

TBool CDummyFontFile::GetNearestFontHelperOld(const TOpenFontSpec& aDesiredFontSpec,TInt aPixelWidth,TInt aPixelHeight,
											  TInt& aFaceIndex,TOpenFontSpec& aActualFontSpec) const
	{
	return COpenFontFile::GetNearestFontHelperOld(aDesiredFontSpec,aPixelWidth,aPixelHeight,aFaceIndex,aActualFontSpec);
	}

void CDummyFontFile::ConstructL()
	{
	const TDesC& filename = FileName();
	TOpenFontFaceAttrib attrib;

	if (filename.CompareF(KFontDummy) == 0)
		attrib.SetFullName(_L("Dummy"));
	else if (filename.CompareF(KFontDummy_b) == 0)
		{
		attrib.SetFullName(_L("Dummy Bold"));
		attrib.SetBold(TRUE);
		}
	else if (filename.CompareF(KFontDummy_i) == 0)
		{
		attrib.SetFullName(_L("Dummy Italic"));
		attrib.SetItalic(TRUE);
		}
	else if (filename.CompareF(KFontDummy_bi) == 0)
		{
		attrib.SetFullName(_L("Dummy Bold Italic"));
		attrib.SetBold(TRUE);
		attrib.SetItalic(TRUE);
		}
	attrib.SetFamilyName(_L("Dummy"));
	attrib.SetLocalFullName(attrib.FullName());
	attrib.SetCoverage(TOpenFontFaceAttrib::ELatinSet);
	attrib.SetMinSizeInPixels(8);
	AddFaceL(attrib);
	}

void CDummyFontFile::GetNearestFontInPixelsL(RHeap* aHeap,COpenFontSessionCacheList* aSessionCacheList,
											 const TOpenFontSpec& aDesiredFontSpec,TInt aPixelWidth,TInt aPixelHeight,
											 COpenFont*& aFont,TOpenFontSpec& aActualFontSpec)

	{
	aFont = NULL;
	TInt face_index = 0;
	if (GetNearestFontHelper(aDesiredFontSpec,aPixelWidth,aPixelHeight,face_index,aActualFontSpec))
		aFont = CDummyFont::NewL(aHeap,aSessionCacheList,this,aActualFontSpec.Height());
	}

void CDummyFontFile::GetNearestFontToDesignHeightInPixelsL(RHeap* aHeap,COpenFontSessionCacheList* aSessionCacheList,
											 const TOpenFontSpec& aDesiredFontSpec,TInt aPixelWidth,TInt aPixelHeight,
											 COpenFont*& aFont,TOpenFontSpec& aActualFontSpec)

	{
	aFont = NULL;
	TInt face_index = 0;
	if (GetNearestFontHelper(aDesiredFontSpec,aPixelWidth,aPixelHeight,face_index,aActualFontSpec))
		aFont = CDummyFont::NewL(aHeap,aSessionCacheList,this,aActualFontSpec.Height());
	}

void CDummyFontFile::GetNearestFontToMaxHeightInPixelsL(RHeap* aHeap,COpenFontSessionCacheList* aSessionCacheList,
											 const TOpenFontSpec& aDesiredFontSpec,TInt aPixelWidth,TInt aPixelHeight,
											 COpenFont*& aFont,TOpenFontSpec& aActualFontSpec,TInt /*aMaxHeight*/)

	{
	aFont = NULL;
	TInt face_index = 0;
	if (GetNearestFontHelper(aDesiredFontSpec,aPixelWidth,aPixelHeight,face_index,aActualFontSpec))
		aFont = CDummyFont::NewL(aHeap,aSessionCacheList,this,aActualFontSpec.Height());
	}

TBool CDummyFontFile::HasUnicodeCharacterL(TInt /*aFaceIndex*/,TInt /*aCode*/) const
	{
	return EFalse; // this dummy font has no characters
	}

CDummyFont* CDummyFont::NewL(RHeap* aHeap,COpenFontSessionCacheList* aSessionCacheList,
							 CDummyFontFile* aFontFile,TInt aSizeInPixels)
	{
	CDummyFont* f = (CDummyFont*)aHeap->AllocL(sizeof(CDummyFont));
	new(f) CDummyFont(aHeap,aSessionCacheList,aFontFile,aSizeInPixels);
	return f;
	}

CDummyFont::CDummyFont(RHeap* aHeap,COpenFontSessionCacheList* aSessionCacheList,CDummyFontFile* aFontFile,
					   TInt aSizeInPixels):
	COpenFont(aHeap,aSessionCacheList,aFontFile)
	{
	iMetrics.SetSize(aSizeInPixels);
	iMetrics.SetAscent(aSizeInPixels * 3 / 4);
	iMetrics.SetDescent(aSizeInPixels - iMetrics.Ascent());
	iMetrics.SetMaxHeight(iMetrics.Ascent());
	iMetrics.SetMaxDepth(iMetrics.Descent());
	iMetrics.SetMaxWidth(aSizeInPixels * 2);
	}

void CDummyFont::RasterizeL(TInt /*aCode*/,TOpenFontGlyphData* /*aGlyphData*/)
	{
	User::Leave(KErrNotSupported); // this dummy font has no glyphs
	}

static void ExpandCleanupStackL()
	{
	TInt count = 0;
	for (; count < 10; count++)
		CleanupStack::PushL((TUint32*)0x1);
	CleanupStack::Pop(count);
	}

//--------------
__CONSTRUCT_STEP__(FsOpen)


void CTFsOpenStep::TestSetupL()
	{
	ExpandCleanupStackL();
	}
