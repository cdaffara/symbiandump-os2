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

#include "T_IsolatedFontStore.h"
#include <f32file.h>
#include <e32test.h>
#include <fntstore.h>
#include <f32file.h>
#include <s32file.h>
#include <fbs.h>
#include "T_FNT.H"

#ifdef __WINS__
_LIT(KEonFontFileName,"z:\\resource\\fonts\\eon14.gdr");
#else
_LIT(KEonFontFileName,"z:\\resource\\fonts\\eon.gdr");
#endif

// Set KVerboseOutput to "ETrue" if extra INFO_PRINTFx for debug purposes are required
const TBool KVerboseOutput = EFalse;

_LIT(K7650FontFileName,"z:\\resource\\fonts\\ceurope.gdr");



CTFnt::CTFnt(CTestStep* aStep):
	CTGraphicsBase(aStep)
	{
	INFO_PRINTF1(_L("FontStore T_FNT test\n"));
	}

/**
	@SYMTestCaseID
	GRAPHICS-FNTSTORE-0023

	@SYMTestCaseDesc
	Tests adding a font file to the font store and checks
	the properties of the font.

	@SYMTestActions
	Creates a user heap. Creates a CFontStore object and
	uses the user heap for memory allocation. Adds a
	font file to the font store. Checks if added correctly.
	Gets the typeface information for the font. Retrieves the
	nearest available font. Sets the font style to superscript
	and subscript and tests getting nearest font. Then for all
	available typefaces it gets the nearest fonts and checks the
	result. The user heap is closed and the heap is checked for memory leaks.
	
	@SYMTestExpectedResults
	Test should pass
*/
void CTFnt::TestReadFileL(const TDesC& aFontFile)
	{
	
	INFO_PRINTF2(_L("Starting TestReadFile with parameter %S"), &aFontFile);
	
	__UHEAP_MARK;
	RHeap* heap=UserHeap::ChunkHeap(NULL,0x10000,0x10000);

// Add file

	TUid id1 = TUid::Uid(0);
	CFontStore *fs=CFontStore::NewL(heap);

	TRAPD(ret,id1=fs->AddFileL(aFontFile));
	TEST(ret==KErrNone);
	INFO_PRINTF2(_L("NumTypefaces %d"),fs->NumTypefaces());

// Get typeface info

	INFO_PRINTF1(_L("Get typeface support"));
	TTypefaceSupport support;
	fs->TypefaceSupport(support,0);
	TBuf<KMaxTypefaceNameLength> name;
	name.Copy(support.iTypeface.iName);
	INFO_PRINTF2(_L("Typeface Name %S"),&name);
	INFO_PRINTF2(_L("Typeface Isproportional %d"),support.iTypeface.IsProportional());
	INFO_PRINTF2(_L("Typeface IsSerif %d"),support.iTypeface.IsSerif());
	INFO_PRINTF2(_L("Typeface IsSymbol %d"),support.iTypeface.IsSymbol());
	INFO_PRINTF2(_L("NumHeights %d"),support.iNumHeights);
	INFO_PRINTF2(_L("MinHeightInTwips %d"),support.iMinHeightInTwips);
	INFO_PRINTF2(_L("MaxHeightInTwips %d"),support.iMaxHeightInTwips);
	INFO_PRINTF2(_L("IsScalable %d"),support.iIsScalable);

	if (KVerboseOutput)
		{
		for (TInt i=0; i<support.iNumHeights; i++)
			{
			INFO_PRINTF3(_L("HeightInPixels %d is %d"),i,fs->FontHeightInPixels(0/*fs->NumTypefaces()-2*/,i));
			}
		}

// Get nearest font
 
	INFO_PRINTF1(_L("Get nearest font"));
	CBitmapFont *font;
	TCharacterMetrics metrics;
	const TUint8* bytes;
	TFontSpec spec;
	support.iTypeface=support.iTypeface;
	spec.iTypeface.SetIsProportional(EFalse);
	spec.iTypeface.SetIsSerif(ETrue);
	spec.iTypeface.SetIsSymbol(EFalse);
	spec.iHeight=10;
	spec.iFontStyle.SetPosture(EPostureUpright);
	for (spec.iHeight=support.iMinHeightInTwips; spec.iHeight<=support.iMaxHeightInTwips; spec.iHeight++)
		{
		ret=fs->GetNearestFontToDesignHeightInTwips((CFont *&) font,spec);
		TEST(ret==KErrNone);
		if (KVerboseOutput)
			{
			INFO_PRINTF2(_L("FontSpec Height %d"),spec.iHeight);
			INFO_PRINTF2(_L("HeightInTwips %d"),font->FontSpecInTwips().iHeight);
			INFO_PRINTF2(_L("HeightInPixels %d"),font->HeightInPixels());
			INFO_PRINTF2(_L("AscentInPixels %d"),font->AscentInPixels());
			INFO_PRINTF3(_L("Font Posture %d StrokeWeight %d"),font->FontSpecInTwips().iFontStyle.Posture(),font->FontSpecInTwips().iFontStyle.StrokeWeight());
			INFO_PRINTF3(_L("Font AlgPosture %d AlgStrokeWeight %d"),((CBitmapFont*)font)->iAlgStyle.IsItalic(),((CBitmapFont*)font)->iAlgStyle.IsBold());
			}
		fs->ReleaseFont(font);
		}

	ret=fs->GetNearestFontToDesignHeightInTwips((CFont *&) font,spec);
	TEST(ret==KErrNone);
	INFO_PRINTF1(_L("Got nearest font"));
	metrics = font->CharacterMetrics(32,bytes);
	INFO_PRINTF5(_L("First 4 bytes %d %d %d %d"),*(bytes),*(bytes+1),*(bytes+2),*(bytes+3));
	INFO_PRINTF5(_L("Second 4 bytes %d %d %d %d"),*(bytes+4),*(bytes+5),*(bytes+6),*(bytes+7));

	INFO_PRINTF2(_L("LeftAdjustInPixels %d"),metrics.iLeftAdjustInPixels);
	INFO_PRINTF2(_L("MoveInPixels %d"),metrics.iMoveInPixels);
	INFO_PRINTF2(_L("RightAdjustInPixels %d"),metrics.iRightAdjustInPixels);
	INFO_PRINTF2(_L("TextWidthInPixels(EPOC) %d"),font->TextWidthInPixels(_L("EPOC")));
	INFO_PRINTF2(_L("TextWidthInPixels(\x00BB) %d"),font->TextWidthInPixels(_L("\x00BB")));		//Double Greater Than »
	CFont::TMeasureTextInput param;
	param.iStartInputChar = 34;
	INFO_PRINTF2(_L("TextWidthInPixels with supplied context(EPOC) %d"),font->TextWidthInPixels(_L("->This text will not be measured<-EPOC"),&param));
	INFO_PRINTF2(_L("TextWidthInPixels with supplied context(\x00BB) %d"),font->TextWidthInPixels(_L("->This text will not be measured<-\x00BB"),&param));
	INFO_PRINTF2(_L("TextCount(EPOC,50) %d"),font->TextCount(_L("EPOC"),50));

	INFO_PRINTF2(_L("WidthZeroInPixels %d"),font->WidthZeroInPixels());
	INFO_PRINTF2(_L("MaxCharWidthInPixels %d"),font->MaxCharWidthInPixels());
	INFO_PRINTF2(_L("MaxNormalCharWidthInPixels %d"),font->MaxNormalCharWidthInPixels());
	INFO_PRINTF2(_L("HeightInTwips %d"),font->FontSpecInTwips().iHeight);
	name.Copy(font->FontSpecInTwips().iTypeface.iName);
	INFO_PRINTF2(_L("Typeface Name %S"),&name);

	fs->ReleaseFont(font);

// Get nearest font
 
	INFO_PRINTF1(_L("Get nearest monoized font"));
	support.iTypeface.iName=_L("");//Mono Swiss");
	support.iTypeface.SetIsProportional(ETrue);//=support.iTypeface;
	support.iTypeface.SetIsSerif(ETrue);//=support.iTypeface;
	spec.iTypeface=support.iTypeface;
	spec.iHeight=18;
	spec.iFontStyle.SetPosture(EPostureUpright);
	spec.iFontStyle.SetPrintPosition(EPrintPosNormal);
	ret=fs->GetNearestFontToDesignHeightInPixels((CFont *&) font,spec);
	TEST(ret==KErrNone);
	INFO_PRINTF2(_L("HeightInPixels %d"),font->HeightInPixels());
	INFO_PRINTF2(_L("AscentInPixels %d"),font->AscentInPixels());
	metrics = font->CharacterMetrics(32,bytes);
	INFO_PRINTF5(_L("First 4 bytes %d %d %d %d"),*(bytes),*(bytes+1),*(bytes+2),*(bytes+3));
	INFO_PRINTF5(_L("Second 4 bytes %d %d %d %d"),*(bytes+4),*(bytes+5),*(bytes+6),*(bytes+7));
	INFO_PRINTF2(_L("LeftAdjustInPixels %d"),metrics.iLeftAdjustInPixels);
	INFO_PRINTF2(_L("MoveInPixels %d"),metrics.iMoveInPixels);
	INFO_PRINTF2(_L("RightAdjustInPixels %d"),metrics.iRightAdjustInPixels);
	INFO_PRINTF2(_L("TextWidthInPixels(EPOC) %d"),font->TextWidthInPixels(_L("EPOC")));
	INFO_PRINTF2(_L("WidthZeroInPixels %d"),font->WidthZeroInPixels());
	INFO_PRINTF2(_L("TextCount(EPOC,50) %d"),font->TextCount(_L("EPOC"),50));
	INFO_PRINTF2(_L("MaxCharWidthInPixels %d"),font->MaxCharWidthInPixels());
	INFO_PRINTF2(_L("MaxNormalCharWidthInPixels %d"),font->MaxNormalCharWidthInPixels());
	INFO_PRINTF2(_L("HeightInTwips %d"),font->FontSpecInTwips().iHeight);
	name.Copy(font->FontSpecInTwips().iTypeface.iName);
	INFO_PRINTF2(_L("Typeface Name %S"),&name);

	fs->ReleaseFont(font);

// Get superscript font
 
	INFO_PRINTF1(_L("Get superscript font"));
	spec.iFontStyle.SetPrintPosition(EPrintPosSuperscript);
	ret=fs->GetNearestFontToDesignHeightInPixels((CFont *&) font,spec);
	TEST(ret==KErrNone);
	INFO_PRINTF2(_L("HeightInPixels %d"),font->HeightInPixels());
	INFO_PRINTF2(_L("BaselineOffsetInPixels %d"),font->iAlgStyle.iBaselineOffsetInPixels);
	INFO_PRINTF2(_L("HeightInTwips %d"),font->FontSpecInTwips().iHeight);
	name.Copy(font->FontSpecInTwips().iTypeface.iName);
	INFO_PRINTF2(_L("Typeface Name %S"),&name);

	fs->ReleaseFont(font);

// Get subscript font
 
	INFO_PRINTF1(_L("Get subscript font"));
	spec.iFontStyle.SetPrintPosition(EPrintPosSubscript);
	ret=fs->GetNearestFontToDesignHeightInPixels((CFont *&) font,spec);
	TEST(ret==KErrNone);
	INFO_PRINTF2(_L("HeightInPixels %d"),font->HeightInPixels());
	INFO_PRINTF2(_L("BaselineOffsetInPixels %d"),font->iAlgStyle.iBaselineOffsetInPixels);
	INFO_PRINTF2(_L("HeightInTwips %d"),font->FontSpecInTwips().iHeight);
	name.Copy(font->FontSpecInTwips().iTypeface.iName);
	INFO_PRINTF2(_L("Typeface Name %S"),&name);

	fs->ReleaseFont(font);

//  aTest all fonts

	INFO_PRINTF1(_L("Get typeface support"));
	spec.iFontStyle.SetPrintPosition(EPrintPosNormal);
	spec.iFontStyle.SetPosture(EPostureUpright);
	spec.iFontStyle.SetStrokeWeight(EStrokeWeightNormal);
	for (TInt i=0; i<fs->NumTypefaces(); i++)
		{
		fs->TypefaceSupport(support,i);
		name.Copy(support.iTypeface.iName);
		INFO_PRINTF2(_L("Typeface Name %S"),&name);
		for (TInt j=0; j<support.iNumHeights; j++)
			{
			spec.iTypeface=support.iTypeface;
			spec.iHeight=fs->FontHeightInPixels(i,j);
			ret=fs->GetNearestFontToDesignHeightInPixels((CFont *&) font,spec);
			TEST(ret==KErrNone);
			INFO_PRINTF2(_L("Spec HeightInPixels %d"),spec.iHeight);
			INFO_PRINTF2(_L("Font HeightInPixels %d"),font->HeightInPixels());
			INFO_PRINTF3(_L("Font Posture %d StrokeWeight %d"),font->FontSpecInTwips().iFontStyle.Posture(),font->FontSpecInTwips().iFontStyle.StrokeWeight());
			INFO_PRINTF3(_L("Font AlgPosture %d AlgStrokeWeight %d"),font->iAlgStyle.IsItalic(),font->iAlgStyle.IsBold());
			fs->ReleaseFont(font);
			}
		}

// Close fontstore
	INFO_PRINTF1(_L("Destroy filestore\n\n"));	
	delete fs;

	heap->Close();
	__UHEAP_MARKEND;
	}

/**
@SYMTestCaseID
GRAPHICS-FNTSTORE-0052

@SYMPREQ
PREQ2147

@SYMREQ
REQ12172

@SYMTestCaseDesc
Creates an instance of fontstore to test that invalid font files generated by the test script
have not eclipsed the valid files already loaded.

@SYMTestPriority
Critical

@SYMTestStatus
Implemented

@SYMTestActions
The start of this test script will create an invalid font file in c:\resource\fonts. This test
creates an instance of font store and performs the font loading to check the invalid file is not
loaded and font store has fallen back to the correct file.

The test also ensures that the invalid file has been created sucessfully by T_WriteInvalidFontFile.exe

After this set off test have been run the file is removed by T_RemoveInvalidFontFile.exe

@SYMTestExpectedResults
The Invalid Font file will exist; font store will not load this file and instead will load the file that it
eclipses (from ROM)

*/
void CTFnt::TestEclipseFallbackWithInvalidFontL()
	{
	//The path of the invalid file that should not be loaded
	_LIT(KInvalidFontFilePath, "c:\\resource\\fonts\\DejaVuSansCondensed.ttf");
	
	//The path of the valid file that should be loaded
	_LIT(KValidFontPath, "z:\\resource\\fonts\\DejaVuSansCondensed.ttf");
	
	//Check the invalid file exists
	RFs fs;
	User::LeaveIfError(fs.Connect());
	
	RFile file;
	TEST(file.Open(fs, KInvalidFontFilePath, EFileRead) == KErrNone);
	
	file.Close();
	fs.Close();
	
	//Create an instance of CFontStore and load the current rasterizers and fonts
	CTIsolatedFontStore* isolatedFontStore1 = CTIsolatedFontStore::NewL();
	CleanupStack::PushL(isolatedFontStore1);
	
	isolatedFontStore1->LoadRasterizersL();
	
	CFontStore* fs1 = isolatedFontStore1->iFs;
	fs1->LoadFontsAtStartupL();
	
	TFileName fileName;
	fs1->GetFontFilePath(_L("DejaVu Sans Condensed"), fileName);

	TEST(fileName.CompareF(KValidFontPath) == 0);
		
	CleanupStack::PopAndDestroy(isolatedFontStore1);
	}

void CTFnt::RunTestCaseL(TInt aCurTestCase)
	{
	((CTFntStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);
	switch(aCurTestCase)
		{
	case 1:
		((CTFntStep*)iStep)->SetTestStepID(_L("GRAPHICS-FNTSTORE-0023"));
		INFO_PRINTF1(_L("#################### T_FNT test case 1 ########################\n"));
		TRAPD(err,TestReadFileL(KEonFontFileName));
		TEST(err == KErrNone);
		break;
	case 2:
		((CTFntStep*)iStep)->SetTestStepID(_L("GRAPHICS-FNTSTORE-0023"));
		INFO_PRINTF1(_L("#################### T_FNT test case 2 ########################\n"));
		TRAP(err,TestReadFileL(K7650FontFileName));
		TEST(err == KErrNone);
		break;
	case 3:
		((CTFntStep*)iStep)->SetTestStepID(_L("GRAPHICS-FNTSTORE-0052"));
		INFO_PRINTF1(_L("T_FNT test case 3 - Eclipsed font in \\resource\\fonts falls back to last good font"));
		TRAP(err,TestEclipseFallbackWithInvalidFontL());
		TEST(err == KErrNone);
	case 4:
		((CTFntStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
		((CTFntStep*)iStep)->CloseTMSGraphicsStep();
		TestComplete();				
		break;
		}
	((CTFntStep*)iStep)->RecordTestResultL();
	}


//--------------
__CONSTRUCT_STEP__(Fnt)



