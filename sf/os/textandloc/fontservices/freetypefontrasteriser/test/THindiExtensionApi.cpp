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
*
*/


/**
 @file 
 @internalComponent - Internal Symbian test code
*/

#include <bitdev.h>
#include <fntstore.h>
#include <hal.h>
#include <fbs.h>

#include <e32test.h>
#include <bitdev.h>

#include <badesca.h>
#include <ecom/ecom.h>
#include <ecom/implementationproxy.h>

#include <graphics/openfontconstants.h>
#include <graphics/openfontrasterizer.h>

#include <test/testexecutestepbase.h>
#include "THindiExtensionApi.h"

/* 
Used for cleanup of RImplInfoArray implementationArray below 
This method is stolen from FbsTop.cpp
*/
void ResetAndDestroyRImplInfoPtrArray(TAny* aPtr)
	{
	RImplInfoPtrArray* array = reinterpret_cast <RImplInfoPtrArray*> (aPtr);
	array->ResetAndDestroy();
	}
	
	
/*
Load all ECOM implemented rasterizer DLLs. 
This method is stolen from CFbTop::LoadOpenFontLibraries()
*/
void LoadOpenFontLibraries(CFontStore* aFontStore)
	{

	RImplInfoPtrArray implementationArray;
	TCleanupItem cleanup(ResetAndDestroyRImplInfoPtrArray, &implementationArray);
	CleanupStack::PushL(cleanup);
	TInt error;
	TInt ecomerror;
	TInt ecomnotready;
	TUid uid = {KUidOpenFontRasterizerPlunginInterface};

	// Making sure that no race situation arises
	// If ECom is not ready, give it another chance and try again. if it still doesn't work 
	// after the third try, then it just carries on quietly and fails... 
	for (ecomnotready =0; ecomnotready <3; ecomnotready++)
		{
		TRAP(ecomerror,REComSession::ListImplementationsL(uid,implementationArray));
		if (!ecomerror)
			{
			break;
			}
		else
			{
			ecomerror = 0; 	
			User::After(0);
			}
		}

	const TInt availCount = implementationArray.Count();
	for (TInt count=0;count<availCount;++count)
		{
		const CImplementationInformation* info = implementationArray[count];
		TUid rasterizerUid = info->ImplementationUid();
 		// Create a rasterizer
 		COpenFontRasterizer* rasterizer=0;
 		TRAP(error,rasterizer = COpenFontRasterizer::NewL(rasterizerUid));
 		if (!error)
   			{
   			// Install it in the font store.
   			TRAP(error,aFontStore->InstallRasterizerL(rasterizer));
   			if (error)
   				delete rasterizer;
   			}
		}
	CleanupStack::PopAndDestroy(&implementationArray); 	

	}

void CRasterizerTest::LoadFontStore()
	{
	// test font - Note if this is changed all the test values needed to be changed to match
	_LIT16(KTestFontFile, "Z:\\Resource\\Fonts\\TNR_Dev_OT.ttf");
	_LIT16(KTestFont, "Devanagari OT Eval");

	//the font store can use the normal user heap
	iFontStore = CFontStore::NewL(&User::Heap());    

 	// set the dimensions.
	iFontStore->iKPixelWidthInTwips = 11860;
	iFontStore->iKPixelHeightInTwips = 11860;

	//load all ecom implemented rasterizer dlls. installs the rasterizer.
	LoadOpenFontLibraries(iFontStore);
	
	//add any required font files
	TRAPD(err, iFontStore->AddFileL(KTestFontFile));
	if (err != KErrNone)
		{
		WARN_PRINTF2(_L("Unable to load font file %S"), &KTestFontFile);
		return;
		}

	TFontSpec testFontSpec(KTestFont, 12); 
	CFont* cfont = 0;
	iFontStore->GetNearestFontToDesignHeightInPixels(*&cfont, testFontSpec);	
	iFont = (CBitmapFont*)cfont;
	}
	
void CRasterizerTest::CloseFontStore()
	{
	if (iFont)
		{
		iFontStore->ReleaseFont(iFont);
		}
	delete iFontStore;
	
	REComSession::FinalClose();
	}

/**
@file
@SYMTestCaseID				GRAPHICS-SYSLIB-FREETYPE-UT-1492
@SYMTestCaseDesc 			Test the Rasterizer API  
							MOpenFontShapingExtension::RasterizeGlyphL()
@SYMTestPriority 			High
@SYMTestActions  			This test checks the glyph data returns when rasterizering 
							the single glyphcode 141 for a single character
@SYMTestExpectedResults		The test must not fail.
@SYMPREQ					PREQ18
*/    
CTestRasterizeGlyph::CTestRasterizeGlyph()
	{
	SetTestStepName(KTestRasterizeGlyphStep);
	}

TVerdict CTestRasterizeGlyph::doTestStepL()
    {
	__UHEAP_MARK;
	SetTestStepID(_L("GRAPHICS-SYSLIB-FREETYPE-UT-1492"));

	// create a font store and load font for testing
	LoadFontStore();
	if (!iFont)
		{
		WARN_PRINTF1(_L("Skipping test"));
		CloseFontStore();
		__UHEAP_MARKEND;
		if (TestStepResult() == EPass)
			SetTestStepResult(EInconclusive);
		return TestStepResult();
		}

	// get the extension interface
	TAny* ext = 0;
	iFont->OpenFont()->ExtendedInterface(KUidOpenFontShapingExtension, ext);
	MOpenFontShapingExtension* extensionInterface
		= reinterpret_cast<MOpenFontShapingExtension*>(ext);
	TEST(extensionInterface != NULL);

	// call the RasterizeGlyphL interface
	TOpenFontGlyphData* glyphData = TOpenFontGlyphData::New(&User::Heap(), 1024) ;
	TInt const glyphCode = 141;
	extensionInterface->RasterizeGlyphL(glyphCode, glyphData);

	// Note these values are only good for the test font: TNR_Dev_OT.ttf 
	// check glyph data returned for glyphcode 141
	const TOpenFontCharMetrics* metrics = glyphData->Metrics();
	INFO_PRINTF2(_L("Height is %d"), metrics->Height());
	INFO_PRINTF2(_L("Width is %d"), metrics->Width());
	INFO_PRINTF2(_L("BytesNeeded is %d"), glyphData->BytesNeeded());

	TEST( metrics->Height() == 9);
	TEST( metrics->Width() == 6);
	TEST( metrics->HorizBearingX() == 0 );
	TEST( glyphData->BytesNeeded()==13);

	
	

	
	INFO_PRINTF2(_L("HorizBearingY is %d"), metrics->HorizBearingY());
	INFO_PRINTF2(_L("VertAdvance is %d"), metrics->VertAdvance());

	TEST( metrics->HorizBearingY() == 8);
	TEST( metrics->HorizAdvance() ==6 );
	TEST( metrics->VertBearingX() == 0);
	TEST( metrics->VertBearingY() == 0);
	TEST( metrics->VertAdvance() == 9); 

	
	// Tidy up and close
	User::Heap().Free(glyphData);
	CloseFontStore();
	RecordTestResultL();
	CloseTMSGraphicsStep();
	__UHEAP_MARKEND;	

	return TestStepResult();
    }
    
/**
@file
@SYMTestCaseID				GRAPHICS-SYSLIB-FREETYPE-UT-1493
@SYMTestCaseDesc 			Test the Rasterizer API  MOpenFontShapingExtension::GlyphIndex()
@SYMTestPriority 			High
@SYMTestActions  			Check the glyph index for a specific code
@SYMTestExpectedResults		The test must not fail.
@SYMPREQ					PREQ18
*/    
CTestGlyphIndex::CTestGlyphIndex()
	{
	SetTestStepName(KTestGlyphIndexStep);
	}

TVerdict CTestGlyphIndex::doTestStepL()
    {
	__UHEAP_MARK;
	SetTestStepID(_L("GRAPHICS-SYSLIB-FREETYPE-UT-1493"));

	// create a font store and load font for testing
	LoadFontStore();
	if (!iFont)
		{
		WARN_PRINTF1(_L("Skipping test"));
		CloseFontStore();
		__UHEAP_MARKEND;
		if (TestStepResult() == EPass)
			SetTestStepResult(EInconclusive);
		return TestStepResult();
		}

	// get the extension interface
	TAny* ext = 0;
	iFont->OpenFont()->ExtendedInterface(KUidOpenFontShapingExtension, ext);
	MOpenFontShapingExtension* extensionInterface
		= reinterpret_cast<MOpenFontShapingExtension*>(ext);
	TEST(extensionInterface != NULL);

	// test a sample of glyph index values returned 
	// Note: these values are only good for the test font: TNR_Dev_OT.ttf 
	TEST( 109 == extensionInterface->GlyphIndex(0x910) );
	TEST( 110 == extensionInterface->GlyphIndex(0x911) );
	TEST( 111 == extensionInterface->GlyphIndex(0x912) );
	TEST( 112 == extensionInterface->GlyphIndex(0x913) );
	TEST( 127 == extensionInterface->GlyphIndex(0x922) );

	// Tidy up and close
	CloseFontStore();
	RecordTestResultL();
	CloseTMSGraphicsStep();
	__UHEAP_MARKEND;	

	return TestStepResult();
    }

/**
@file
@SYMTestCaseID				GRAPHICS-SYSLIB-FREETYPE-UT-1494
@SYMTestCaseDesc 			Test the Rasterizer API MOpenFontShapingExtension::GlyphPointInHintedPixels()
@SYMTestPriority 			High
@SYMTestActions  			Check that the Glyph point in Hinted Pixels can be read from the rasterizer
@SYMTestExpectedResults		The test must not fail.
@SYMPREQ					PREQ18
*/    
CTestGlyphPointInHintedPixels::CTestGlyphPointInHintedPixels()
	{
	SetTestStepName(KTestGlyphPointInHintedPixels);
	}

TVerdict CTestGlyphPointInHintedPixels::doTestStepL()
    {
	__UHEAP_MARK;
	SetTestStepID(_L("GRAPHICS-SYSLIB-FREETYPE-UT-1494"));

	// create a font store and load font for testing
	LoadFontStore();
	if (!iFont)
		{
		WARN_PRINTF1(_L("Skipping test"));
		CloseFontStore();
		__UHEAP_MARKEND;
		if (TestStepResult() == EPass)
			SetTestStepResult(EInconclusive);
		return TestStepResult();
		}

	// get the extension interface
	TAny* ext = 0;
	iFont->OpenFont()->ExtendedInterface(KUidOpenFontShapingExtension, ext);
	MOpenFontShapingExtension* extensionInterface
		= reinterpret_cast<MOpenFontShapingExtension*>(ext);
	TEST(extensionInterface != NULL);

	TReal x=0.0;
	TReal y=0.0;

	// call the interface with some arbitrary test values and check the values returned
	// Note: these values are only good for the test font: TNR_Dev_OT.ttf 
	// get the coordinates of point 1 for glyph 0x141 
	extensionInterface->GlyphPointInHintedPixels(0x141, 1, x, y);
	INFO_PRINTF2(_L("y is %f"), y);

	TEST(x == 7.0);
	TEST(y == 7.125);

	
	
	// get the coordinates of point 2 for glyph 0x141 
	extensionInterface->GlyphPointInHintedPixels(0x141, 2, x, y);
	INFO_PRINTF2(_L("x is %f"), x);
	INFO_PRINTF2(_L("y is %f"), y);

	TEST(x == 3.953125);
	TEST(y == 7.125);


	// get the coordinates of point 3 for glyph 0x143 
	extensionInterface->GlyphPointInHintedPixels(0x143, 3, x, y);
	INFO_PRINTF3(_L("x is %f, y is %f"), x, y);

	TEST(x == 4.953125);
	TEST(y == 5.25);

	
	// tidy up	
	CloseFontStore();
	RecordTestResultL();
	CloseTMSGraphicsStep();
	__UHEAP_MARKEND;	

	return TestStepResult();
    }
            
/**
@file
@SYMTestCaseID				GRAPHICS-SYSLIB-FREETYPE-UT-1495
@SYMTestCaseDesc 			Test the Rasterizer API MOpenFontShapingExtension::GlyphPointInFontUnits()
@SYMTestPriority 			High
@SYMTestActions  			Check that the Glyph point in Font units can be read from the rasterizer
@SYMTestExpectedResults		The test must not fail.
@SYMPREQ					PREQ18
*/    
CTestGlyphPointInFontUnits::CTestGlyphPointInFontUnits()
	{
	SetTestStepName(KTestGlyphPointInFontUnits);
	}

TVerdict CTestGlyphPointInFontUnits::doTestStepL()
    {
	__UHEAP_MARK;
	SetTestStepID(_L("GRAPHICS-SYSLIB-FREETYPE-UT-1495"));

	// create a font store and load font for testing
	LoadFontStore();
	if (!iFont)
		{
		WARN_PRINTF1(_L("Skipping test"));
		CloseFontStore();
		__UHEAP_MARKEND;
		if (TestStepResult() == EPass)
			SetTestStepResult(EInconclusive);
		return TestStepResult();
		}

	// get the extension interface
	TAny* ext = 0;
	iFont->OpenFont()->ExtendedInterface(KUidOpenFontShapingExtension, ext);
	MOpenFontShapingExtension* extensionInterface
		= reinterpret_cast<MOpenFontShapingExtension*>(ext);
	TEST(extensionInterface != NULL);

	// call the interface with some test values
	TInt const glyphIndex1 = 0x141;
	TInt const glyphIndex2 = 0x142;
	TInt const glyphIndex3 = 0x143;
	TInt const glyphIndex4 = 0x144;
	TInt pointerNumber = 0;
	TInt x=0;
	TInt y=0;
	
	// and check the results 
	// Note these values are only good for the test font: TNR_Dev_OT.ttf 
	extensionInterface->GlyphPointInFontUnits(glyphIndex1, pointerNumber, x, y);
	TEST(x == 1117);
	TEST(y == 1246);
	TEST(pointerNumber == 0);

	extensionInterface->GlyphPointInFontUnits(glyphIndex2, pointerNumber, x, y);
	TEST(x == 1219);
	TEST(y == 1246);
	TEST(pointerNumber == 0);

	extensionInterface->GlyphPointInFontUnits(glyphIndex3, pointerNumber, x, y);
	TEST(x == 1096);
	TEST(y == 1246);
	TEST(pointerNumber == 0);

	extensionInterface->GlyphPointInFontUnits(glyphIndex4, pointerNumber, x, y);
	TEST(x == 1405);
	TEST(y == 1246);
	TEST(pointerNumber == 0);

	CloseFontStore();
	RecordTestResultL();
	CloseTMSGraphicsStep();
	__UHEAP_MARKEND;	

	return TestStepResult();
    }
            
/**
@file
@SYMTestCaseID				GRAPHICS-SYSLIB-FREETYPE-UT-1496
@SYMTestCaseDesc 			Test the Rasterizer API MOpenFontShapingExtension::GetExtensionFontMetrics()
@SYMTestPriority 			High
@SYMTestActions  			Check that the Extension Font Metrics can be read from the rasterizer
@SYMTestExpectedResults		The test must not fail.
@SYMPREQ					PREQ18
*/    
CTestGetExtensionFontMetrics::CTestGetExtensionFontMetrics()
	{
	SetTestStepName(KTestGetExtensionFontMetrics);
	}

TVerdict CTestGetExtensionFontMetrics::doTestStepL()
    {
	__UHEAP_MARK;
	SetTestStepID(_L("GRAPHICS-SYSLIB-FREETYPE-UT-1496"));

	// create a font store and load font for testing
	LoadFontStore();
	if (!iFont)
		{
		WARN_PRINTF1(_L("Skipping test"));
		CloseFontStore();
		__UHEAP_MARKEND;
		if (TestStepResult() == EPass)
			SetTestStepResult(EInconclusive);
		return TestStepResult();
		}

	// get the extension interface
	TAny* ext = 0;
	iFont->OpenFont()->ExtendedInterface(KUidOpenFontShapingExtension, ext);
	MOpenFontShapingExtension* extensionInterface
		= reinterpret_cast<MOpenFontShapingExtension*>(ext);
	TEST(extensionInterface != NULL);

	// call the interface
	MOpenFontShapingExtension::TExtensionFontMetrics metrics;
	extensionInterface->GetExtensionFontMetrics(metrics);
	 	
	// check the results
	// Note these values are only good for the test font: TNR_Dev_OT.ttf
	TEST( metrics.iUnitsPerEm == 2048);
	TEST( metrics.iXPixelsPerEm == 12.0);
	TEST( metrics.iYPixelsPerEm == 12.0);
	TEST( metrics.iXScaleFactor == 0.375);
	TEST( metrics.iYScaleFactor == 0.375);	
	
	CloseFontStore();
	RecordTestResultL();
	CloseTMSGraphicsStep();
	__UHEAP_MARKEND;	

	return TestStepResult();
    }

/**
@file
@SYMTestCaseID				GRAPHICS-SYSLIB-FREETYPE-UT-1497			
@SYMTestCaseDesc 			Test the Rasterizer API: MOpenFontTrueTypeExtension.
@SYMTestPriority 			High
@SYMTestActions  			Test access to the truetype tables via the Rasterizer API:
@SYMTestExpectedResults		The test must not fail.
@SYMPREQ					PREQ18
*/    
CTestTrueTypeTable::CTestTrueTypeTable()
	{
	SetTestStepName(KTestTrueTypeTableStep);
	}
	
TAny* CTestTrueTypeTable::TestGetTable(TExpectedResult aExpected,
	MOpenFontTrueTypeExtension* aTableStore, TUint32 aTag, TInt* aLength)
	{
	TBool has = aTableStore->HasTrueTypeTable(aTag);
	TEST( aExpected == EUnavailable? !has : has );
	TInt length = 0;
	TInt error = 1;
	__UHEAP_FAILNEXT(1);
	TAny* table = aTableStore->GetTrueTypeTable(error, aTag, &length);
	__UHEAP_RESET;
	
#ifdef _DEBUG
	// If it was EAvailableNew, it should have failed allocating memory
	TEST ( aExpected != EAvailableNew
		|| (error == KErrNoMemory && table == 0) );
#else
	//Shouldn't have any problem allocating memory in release mode as 
	//__UHEAP_FAILNEXT is only for debug mode
	TEST ( aExpected != EAvailableNew
		|| (error == KErrNone && table != 0) );
#endif

	// If it returned OK, it should be returning a table with positive length
	TEST ( error != KErrNone || (table && 0 < length) );
	if (!table)
		table = aTableStore->GetTrueTypeTable(error, aTag, &length);
	if (aLength && error == KErrNone)
		*aLength = length;
	// If the table is available, it should have returned, without error,
	// a table and a positive length
	TEST ( aExpected == EUnavailable ||
		(error == KErrNone && table && 0 < length) );
	return table;
	}

TVerdict CTestTrueTypeTable::doTestStepL()
    {
	__UHEAP_MARK;
	SetTestStepID(_L("GRAPHICS-SYSLIB-FREETYPE-UT-1497"));

	// create a font store and load font for testing
	LoadFontStore();
	if (!iFont)
		{
		WARN_PRINTF1(_L("Skipping test"));
		CloseFontStore();
		__UHEAP_MARKEND;
		if (TestStepResult() == EPass)
			SetTestStepResult(EInconclusive);
		return TestStepResult();
		}

	// truetype tag for the GSUB table 
	const TUint32  KGSubTableTag = 0x47535542UL;

	// get both the extended interfaces
	TAny* ext = 0;
	iFont->OpenFont()->ExtendedInterface(KUidOpenFontShapingExtension, ext);
	MOpenFontShapingExtension* extensionInterface
		= reinterpret_cast<MOpenFontShapingExtension*>(ext);
	TEST(extensionInterface != NULL);

	iFont->OpenFont()->ExtendedInterface(KUidOpenFontTrueTypeExtension, ext);
	MOpenFontTrueTypeExtension* trueTypeExtensionInterface
		= reinterpret_cast<MOpenFontTrueTypeExtension*>(ext);
	TEST(trueTypeExtensionInterface != NULL);
	
	// check if table available
	TEST( trueTypeExtensionInterface->HasTrueTypeTable( KGSubTableTag ) );

	// get a table from the rasterizer
	TInt length = 0;
	TAny* trueTypeTable = TestGetTable(EUnavailable,
		trueTypeExtensionInterface, 12345, &length);
	trueTypeTable = TestGetTable(EAvailableNew,
		trueTypeExtensionInterface, KGSubTableTag, &length);
	TEST( length == 31576 );	
	length = 0;
	trueTypeTable = TestGetTable(EAvailableExisting,
		trueTypeExtensionInterface, KGSubTableTag, &length);
	TEST( length == 31576 );	

	/* Compare the table returned against a stored copy */	
	/* this needs to be added */

	// release the table 	
  	trueTypeExtensionInterface->ReleaseTrueTypeTable(trueTypeTable);

	CloseFontStore();
	RecordTestResultL();
	CloseTMSGraphicsStep();
	__UHEAP_MARKEND;	
	
	return TestStepResult();
    }    
 
