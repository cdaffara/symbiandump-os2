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
*
*/


/**
 @file
 @test
 @internalComponent Internal Symbian test code
*/

#include <ecom/ecom.h>
#include <gdi.h>
#include "T_FontMetrics.h"
#include <graphics/openfontrasterizer.h>
#include <graphics/openfontconstants.h>
#include <graphics/gdi/glyphsample.h>

const TInt CTFontMetrics::KRequiredHeight = 24;
_LIT(KTypefaceName, "DejaVu Sans Condensed");
_LIT(KFontFilePath, "z:\\resource\\fonts\\DejaVuSansCondensed.ttf");

LOCAL_C void ResetAndDestroyRImplInfoPtrArray(TAny* aPtr)
	{
	RImplInfoPtrArray* array = reinterpret_cast <RImplInfoPtrArray*> (aPtr);
	array->ResetAndDestroy();
	}

CTFontMetrics::CTFontMetrics(CTestStep* aStep) :
	CTGraphicsBase(aStep)
	{
	}

void CTFontMetrics::ConstructL()
	{
    iHeap = UserHeap::ChunkHeap(NULL, 0x10000, 0x10000);
	if (NULL == iHeap)
		User::Leave(KErrNoMemory);
	iHeap->__DbgMarkStart();
	iFontStore = CFontStore::NewL(iHeap);
	InstallRasterizerL();
	AddFileL();
	}

CTFontMetrics::~CTFontMetrics()
	{
	delete iFontStore;
	iHeap->__DbgMarkEnd(0);
	iHeap->Close();
	User::Heap().Check();
	REComSession::FinalClose();
	}

/**
Create a rasterizer and install it in the font store.
*/
void CTFontMetrics::InstallRasterizerL()
	{
	RImplInfoPtrArray implementationArray;
	TCleanupItem cleanup(ResetAndDestroyRImplInfoPtrArray, &implementationArray);
	CleanupStack::PushL(cleanup);
	const TUid uid = {KUidOpenFontRasterizerPlunginInterface};

	TRAPD(error, REComSession::ListImplementationsL(uid, implementationArray));
	TEST(error == KErrNone);
	TEST(0 < implementationArray.Count());

	COpenFontRasterizer* rasterizer =
		COpenFontRasterizer::NewL(implementationArray[0]->ImplementationUid());
	TRAP(error, iFontStore->InstallRasterizerL(rasterizer));
	if (error)
		{
		delete rasterizer;
		TEST(error == KErrNone);
		}
	CleanupStack::PopAndDestroy(&implementationArray); 	
	}

void CTFontMetrics::AddFileL()
	{
	iFontStore->AddFileL(KFontFilePath);
	}

TOpenFontSpec CTFontMetrics::GetTOpenFontSpec()
	{
	TOpenFontSpec openFontSpec;
	openFontSpec.SetName(KTypefaceName);
	openFontSpec.SetHeight(KRequiredHeight);
	openFontSpec.SetItalic(EFalse);
	openFontSpec.SetBold(EFalse);
	return openFontSpec;
	}

void CTFontMetrics::RunTestCaseL(TInt aCurTestCase)
	{
	((CTFontMetricsStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);
	switch ( aCurTestCase )
		{
	case 1:
		((CTFontMetricsStep*)iStep)->SetTestStepID(_L("GRAPHICS-FNTSTORE-0029"));
		FontAccessInCTypefaceStore();
		break;
	case 2:
		((CTFontMetricsStep*)iStep)->SetTestStepID(_L("GRAPHICS-FNTSTORE-0002"));
		TLanguage2GlyphSample();
		break;
	case 3:
		((CTFontMetricsStep*)iStep)->SetTestStepID(_L("GRAPHICS-FNTSTORE-0003"));
		GetNearestFontToDesignHeightInPixels();
		break;
	case 4:
		((CTFontMetricsStep*)iStep)->SetTestStepID(_L("GRAPHICS-FNTSTORE-0004"));
		GetNearestFontToMaxHeightInPixels();
		break;
	case 5:
		((CTFontMetricsStep*)iStep)->SetTestStepID(_L("GRAPHICS-FNTSTORE-0030"));
		TestINC086513();
		break;
	case 6:
		((CTFontMetricsStep*)iStep)->SetTestStepID(_L("GRAPHICS-FNTSTORE-0020"));
		TestGetNearestFontToMaxHeightInPixels();
		break;
	case 7:
        	((CTFontMetricsStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
		((CTFontMetricsStep*)iStep)->CloseTMSGraphicsStep();
		TestComplete();
		break;
		}
	((CTFontMetricsStep*)iStep)->RecordTestResultL();
	}

/**
   @SYMTestCaseID          GRAPHICS-FNTSTORE-0002

   @SYMPREQ                PREQ533

   @SYMREQ                 REQ3800

   @SYMTestCaseDesc        Verifying GlyphSample::TLanguage2TScript and GlyphSample::TScript2GlyphSample
							with different TLanguage values.

   @SYMTestPriority        Critical

   @SYMTestStatus          Implemented

   @SYMTestActions         (1) Call TLanguage2TScript and then TScript2GlyphSample
								with boundary cases ELangTest, ELangNone, ELangMaximum.
                           (2) Call TLanguage2TScript and then TScript2GlyphSample
								with languages belonging to unsupported scripts.
                           (3) Call TLanguage2TScript and then TScript2GlyphSample
								with languages belonging to Latin script.
                           (4) Repeat (3) for all other supported scripts.

   @SYMTestExpectedResults (1) and (2) - NULL
                           (3) and (4) - Languages belonging to the same script should return
						   the same address value pointing to the same set of glyph samples.
*/
void CTFontMetrics::TLanguage2GlyphSample()
	{
	// boundary cases
	TEST(NULL == GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangTest)).Ptr());
	TEST(NULL == GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangNone)).Ptr());
	TEST(NULL == GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangMaximum)).Ptr());
	// Not yet supported
	TEST(NULL == GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangAmharic)).Ptr());
	TEST(NULL == GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangTigrinya)).Ptr());
	TEST(NULL == GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangArmenian)).Ptr());
	TEST(NULL == GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangTagalog)).Ptr());
	TEST(NULL == GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangBengali)).Ptr());
	TEST(NULL == GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangBurmese)).Ptr());
	TEST(NULL == GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangGeorgian)).Ptr());
	TEST(NULL == GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangGujarati)).Ptr());
	TEST(NULL == GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangKannada)).Ptr());
	TEST(NULL == GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangKhmer)).Ptr());
	TEST(NULL == GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangLao)).Ptr());
	TEST(NULL == GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangMalayalam)).Ptr());
	TEST(NULL == GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangMongolian)).Ptr());
	TEST(NULL == GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangPunjabi)).Ptr());
	TEST(NULL == GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangSinhalese)).Ptr());
	TEST(NULL == GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangTamil)).Ptr());
	TEST(NULL == GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangTelugu)).Ptr());
	TEST(NULL == GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangTibetan)).Ptr());
	// Latin
	TEST(GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangEnglish)) == GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangFrench)));
	TEST(GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangEnglish)) == GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangGerman)));
	TEST(GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangEnglish)) == GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangSpanish)));
	TEST(GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangEnglish)) == GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangItalian)));
	TEST(GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangEnglish)) == GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangSwedish)));
	TEST(GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangEnglish)) == GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangDanish)));
	TEST(GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangEnglish)) == GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangNorwegian)));
	TEST(GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangEnglish)) == GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangFinnish)));
	TEST(GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangEnglish)) == GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangAmerican)));
	TEST(GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangEnglish)) == GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangSwissFrench)));
	TEST(GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangEnglish)) == GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangSwissGerman)));
	TEST(GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangEnglish)) == GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangPortuguese)));
	TEST(GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangEnglish)) == GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangTurkish)));
	TEST(GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangEnglish)) == GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangIcelandic)));
	TEST(GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangEnglish)) == GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangHungarian)));
	TEST(GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangEnglish)) == GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangDutch)));
	TEST(GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangEnglish)) == GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangBelgianFlemish)));
	TEST(GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangEnglish)) == GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangAustralian)));
	TEST(GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangEnglish)) == GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangBelgianFrench)));
	TEST(GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangEnglish)) == GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangAustrian)));
	TEST(GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangEnglish)) == GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangNewZealand)));
	TEST(GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangEnglish)) == GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangInternationalFrench)));
	TEST(GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangEnglish)) == GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangCzech)));
	TEST(GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangEnglish)) == GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangSlovak)));
	TEST(GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangEnglish)) == GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangPolish)));
	TEST(GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangEnglish)) == GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangSlovenian)));
	TEST(GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangEnglish)) == GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangAfrikaans)));
	TEST(GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangEnglish)) == GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangAlbanian)));
	TEST(GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangEnglish)) == GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangCatalan)));
	TEST(GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangEnglish)) == GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangCroatian)));
	TEST(GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangEnglish)) == GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangCanadianEnglish)));
	TEST(GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangEnglish)) == GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangInternationalEnglish)));
	TEST(GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangEnglish)) == GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangSouthAfricanEnglish)));
	TEST(GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangEnglish)) == GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangEstonian)));
	TEST(GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangEnglish)) == GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangCanadianFrench)));
	TEST(GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangEnglish)) == GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangScotsGaelic)));
	TEST(GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangEnglish)) == GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangIndonesian)));
	TEST(GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangEnglish)) == GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangIrish)));
	TEST(GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangEnglish)) == GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangSwissItalian)));
	TEST(GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangEnglish)) == GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangLatvian)));
	TEST(GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangEnglish)) == GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangLithuanian)));
	TEST(GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangEnglish)) == GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangMalay)));
	TEST(GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangEnglish)) == GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangMoldavian)));
	TEST(GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangEnglish)) == GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangNorwegianNynorsk)));
	TEST(GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangEnglish)) == GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangBrazilianPortuguese)));
	TEST(GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangEnglish)) == GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangRomanian)));
	TEST(GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangEnglish)) == GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangSomali)));
	TEST(GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangEnglish)) == GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangInternationalSpanish)));
	TEST(GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangEnglish)) == GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangLatinAmericanSpanish)));
	TEST(GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangEnglish)) == GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangSwahili)));
	TEST(GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangEnglish)) == GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangFinlandSwedish)));
	TEST(GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangEnglish)) == GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangCyprusTurkish)));
	TEST(GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangEnglish)) == GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangVietnamese)));
	TEST(GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangEnglish)) == GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangWelsh)));
	TEST(GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangEnglish)) == GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangZulu)));
	// Greek
	TEST(GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangGreek)) == GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangCyprusGreek)));
	// Cyrillic
	TEST(GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangRussian)) == GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangBelarussian)));
	TEST(GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangRussian)) == GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangBulgarian)));
	TEST(GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangRussian)) == GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangKazakh)));
	TEST(GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangRussian)) == GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangMacedonian)));
	TEST(GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangRussian)) == GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangSerbian)));
	TEST(GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangRussian)) == GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangTurkmen)));
	TEST(GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangRussian)) == GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangUkrainian)));
	// Hebrew
	TEST(NULL != GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangHebrew)).Ptr());
	// Arabic
	TEST(GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangArabic)) == GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangFarsi)));
	TEST(GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangArabic)) == GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangUrdu)));
	// Devanagari
	TEST(GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangHindi)) == GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangMarathi)));
	// Thai
	TEST(NULL != GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangThai)).Ptr());
	// HanIdeographs
	TEST(GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangTaiwanChinese)) == GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangHongKongChinese)));
	TEST(GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangTaiwanChinese)) == GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangPrcChinese)));
	TEST(GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangTaiwanChinese)) == GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangJapanese)));
	TEST(GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangTaiwanChinese)) == GlyphSample::TScript2GlyphSample(GlyphSample::TLanguage2TScript(ELangKorean)));
	}

/**
   @SYMTestCaseID          GRAPHICS-FNTSTORE-0003

   @SYMPREQ                PREQ533

   @SYMREQ                 REQ3800

   @SYMTestCaseDesc        Verifying font's metrics created by CFontStore::GetNearestFontToDesignHeightInPixels
							with different TLanguage values.

   @SYMTestPriority        Critical

   @SYMTestStatus          Implemented

   @SYMTestActions         (1) Call GetNearestFontToDesignHeightInPixels with ELangNone.
                           (2) Call CFont's HeightInPixels, AscentInPixels, DescentInPixels, FontCapitalAscent,
								FontStandardDescent, FontMaxAscent, FontMaxDescent, FontMaxHeight, and FontLineGap
								to verify the font metrics of the returned font object.
                           (3) Repeat (1) and (2) with a Latin-based language, a Greek-based language, and a
								Cyrillic-based language for the TLanguage parameter.

   @SYMTestExpectedResults See returned values in Verify_GetNearestFontToDesignHeightInPixels.
*/
void CTFontMetrics::GetNearestFontToDesignHeightInPixels()
	{
    INFO_PRINTF3(_L("iKPixelWidthInTwips = %d; iKPixelHeightInTwips = %d"), 
                iFontStore->iKPixelWidthInTwips, iFontStore->iKPixelHeightInTwips);
    
	TOpenFontSpec openFontSpec = GetTOpenFontSpec();

	INFO_PRINTF1(_L("GetNearestFontToDesignHeightInPixels().ELangNone"));
	CFont* font = NULL;
	openFontSpec.SetScriptTypeForMetrics(ELangNone);
	iFontStore->GetNearestFontToDesignHeightInPixels(font, openFontSpec);
	Verify_GetNearestFontToDesignHeightInPixels(*font, openFontSpec.ScriptTypeForMetrics());
	iFontStore->ReleaseFont(font);

	INFO_PRINTF1(_L("GetNearestFontToDesignHeightInPixels().ELangFinnish"));
	font = NULL;
	openFontSpec.SetScriptTypeForMetrics(ELangFinnish);
	iFontStore->GetNearestFontToDesignHeightInPixels(font, openFontSpec);
	Verify_GetNearestFontToDesignHeightInPixels(*font, openFontSpec.ScriptTypeForMetrics());
	iFontStore->ReleaseFont(font);

	INFO_PRINTF1(_L("GetNearestFontToDesignHeightInPixels().ELangGreek"));
	font = NULL;
	openFontSpec.SetScriptTypeForMetrics(ELangGreek);
	iFontStore->GetNearestFontToDesignHeightInPixels(font, openFontSpec);
	Verify_GetNearestFontToDesignHeightInPixels(*font, openFontSpec.ScriptTypeForMetrics());
	iFontStore->ReleaseFont(font);

	INFO_PRINTF1(_L("GetNearestFontToDesignHeightInPixels().ELangRussian"));
	font = NULL;
	openFontSpec.SetScriptTypeForMetrics(ELangRussian);
	iFontStore->GetNearestFontToDesignHeightInPixels(font, openFontSpec);
	Verify_GetNearestFontToDesignHeightInPixels(*font, openFontSpec.ScriptTypeForMetrics());
	iFontStore->ReleaseFont(font);
	}

void CTFontMetrics::Verify_GetNearestFontToDesignHeightInPixels(const CFont& aFont, TInt aScript)
	{
	VerifyTypefaceNameAndID(aFont);
    INFO_PRINTF4(_L("HeightInPixels() = %d; AscentInPixels() = %d; DescentInPixels() = %d"), 
            aFont.HeightInPixels(), aFont.AscentInPixels(), aFont.DescentInPixels());
    INFO_PRINTF4(_L("FontCapitalAscent() = %d; FontStandardDescent() = %d; FontMaxAscent() = %d"), 
            aFont.FontCapitalAscent(), aFont.FontStandardDescent(), aFont.FontMaxAscent());
    INFO_PRINTF4(_L("FontMaxDescent() = %d; FontMaxHeight() = %d; FontLineGap() = %d"), 
            aFont.FontMaxDescent(), aFont.FontMaxHeight(), aFont.FontLineGap());
	// Old metrics
	TEST(KRequiredHeight == aFont.HeightInPixels());
	TEST(19 == aFont.AscentInPixels());
	TEST(KRequiredHeight - aFont.AscentInPixels() == aFont.DescentInPixels());
	// New metrics
	TEST(18 == aFont.FontCapitalAscent());
	TEST(5  == aFont.FontStandardDescent());
	if (GlyphSample::EScriptNone == aScript ||
	        GlyphSample::EScriptLatin == aScript ||
	        GlyphSample::EScriptGreek == aScript ||
	        GlyphSample::EScriptCyrillic == aScript)	
		{
		TEST(28 == aFont.FontMaxAscent());
		TEST(8 	== aFont.FontMaxDescent());
		TEST(42 == aFont.FontLineGap());
		}
	else TEST(1 == 0);
	TEST(aFont.FontMaxAscent() + aFont.FontMaxDescent() == aFont.FontMaxHeight());
	INFO_PRINTF2(_L("MaxCharWidthInPixels() returns %d"), aFont.MaxCharWidthInPixels());
	}

/**
   @SYMTestCaseID          GRAPHICS-FNTSTORE-0004

   @SYMPREQ                PREQ533

   @SYMREQ                 REQ3800

   @SYMTestCaseDesc        Verifying font's metrics created by CFontStore::GetNearestFontToMaxHeightInPixels
							with different TLanguage values.

   @SYMTestPriority        Critical

   @SYMTestStatus          Implemented

   @SYMTestActions         (1) Call GetNearestFontToMaxHeightInPixels with ELangNone.
                           (2) Call CFont's HeightInPixels, AscentInPixels, DescentInPixels, FontCapitalAscent,
								FontStandardDescent, FontMaxAscent, FontMaxDescent, FontMaxHeight, and FontLineGap
								to verify the font metrics of the returned font object.
                           (3) Repeat (1) and (2) with a Latin-based language, a Greek-based language, and a
								Cyrillic-based language for the TLanguage parameter.

   @SYMTestExpectedResults See returned values in Verify_GetNearestFontToMaxHeightInPixels.
*/
void CTFontMetrics::GetNearestFontToMaxHeightInPixels()
	{
	TOpenFontSpec openFontSpec = GetTOpenFontSpec();

	INFO_PRINTF1(_L("GetNearestFontToMaxHeightInPixels().ELangNone"));
	CFont* font = NULL;
	openFontSpec.SetScriptTypeForMetrics(ELangNone);
	iFontStore->GetNearestFontToMaxHeightInPixels(font, openFontSpec, KRequiredHeight);
	Verify_GetNearestFontToMaxHeightInPixels(*font, openFontSpec.ScriptTypeForMetrics());
	iFontStore->ReleaseFont(font);

	INFO_PRINTF1(_L("GetNearestFontToMaxHeightInPixels().ELangFinnish"));
	font = NULL;
	openFontSpec.SetScriptTypeForMetrics(ELangFinnish);
	iFontStore->GetNearestFontToMaxHeightInPixels(font, openFontSpec, KRequiredHeight);
	Verify_GetNearestFontToMaxHeightInPixels(*font, openFontSpec.ScriptTypeForMetrics());
	iFontStore->ReleaseFont(font);

	INFO_PRINTF1(_L("GetNearestFontToMaxHeightInPixels().ELangGreek"));
	font = NULL;
	openFontSpec.SetScriptTypeForMetrics(ELangGreek);
	iFontStore->GetNearestFontToMaxHeightInPixels(font, openFontSpec, KRequiredHeight);
	Verify_GetNearestFontToMaxHeightInPixels(*font, openFontSpec.ScriptTypeForMetrics());
	iFontStore->ReleaseFont(font);

	INFO_PRINTF1(_L("GetNearestFontToMaxHeightInPixels().ELangRussian"));
	font = NULL;
	openFontSpec.SetScriptTypeForMetrics(ELangRussian);
	iFontStore->GetNearestFontToMaxHeightInPixels(font, openFontSpec, KRequiredHeight);
	Verify_GetNearestFontToMaxHeightInPixels(*font, openFontSpec.ScriptTypeForMetrics());
	iFontStore->ReleaseFont(font);
	}

void CTFontMetrics::Verify_GetNearestFontToMaxHeightInPixels(const CFont& aFont, TInt aScript)
	{
	VerifyTypefaceNameAndID(aFont);
    INFO_PRINTF4(_L("HeightInPixels() = %d; AscentInPixels() = %d; DescentInPixels() = %d"), 
            aFont.HeightInPixels(), aFont.AscentInPixels(), aFont.DescentInPixels());
    INFO_PRINTF4(_L("FontCapitalAscent() = %d; FontStandardDescent() = %d; FontMaxAscent() = %d"), 
            aFont.FontCapitalAscent(), aFont.FontStandardDescent(), aFont.FontMaxAscent());
    INFO_PRINTF4(_L("FontMaxDescent() = %d; FontMaxHeight() = %d; FontLineGap() = %d"), 
            aFont.FontMaxDescent(), aFont.FontMaxHeight(), aFont.FontLineGap());
	if (GlyphSample::EScriptNone == aScript)
		{
		// Old metrics
        TEST(16 == aFont.HeightInPixels());
        TEST(13 == aFont.AscentInPixels());
        TEST(16 - 13 == aFont.DescentInPixels());
        // New metrics
        TEST(12 == aFont.FontCapitalAscent());
        TEST(3  == aFont.FontStandardDescent());
        TEST(19 == aFont.FontMaxAscent());
        TEST(6  == aFont.FontMaxDescent());
        TEST(KRequiredHeight + 1 == aFont.FontMaxHeight());
        TEST(31 == aFont.FontLineGap());
		}
	else if (GlyphSample::EScriptLatin == aScript ||
	        GlyphSample::EScriptGreek == aScript ||
	        GlyphSample::EScriptCyrillic == aScript)
	    {
        // Old metrics
        TEST(24 == aFont.HeightInPixels());
        TEST(19 == aFont.AscentInPixels());
        TEST(24 - 19 == aFont.DescentInPixels());
        // New metrics
        TEST(18 == aFont.FontCapitalAscent());
        TEST(5  == aFont.FontStandardDescent());
        TEST(28 == aFont.FontMaxAscent());
        TEST(8  == aFont.FontMaxDescent());
        TEST(KRequiredHeight + 12 == aFont.FontMaxHeight());
        TEST(42 == aFont.FontLineGap());
	    }
	else TEST(1 == 0);
	INFO_PRINTF2(_L("MaxCharWidthInPixels() returns %d"), aFont.MaxCharWidthInPixels());
	}

void CTFontMetrics::VerifyTypefaceNameAndID(const CFont& aFont)
	{
	const TUid uid = {268435523};
	TEST(uid == aFont.TypeUid());
	TEST(!aFont.FontSpecInTwips().iTypeface.iName.CompareC(KTypefaceName));
	}

/**
	@SYMTestCaseID
	GRAPHICS-FNTSTORE-0029

	@SYMTestCaseDesc
	Tests if CTypefaceStore::iFontAccess generates false positives and
	thus prevents GetNearestFontToDesignHeightInPixels from returning the correct fonts.
	
	@SYMTestActions
	1. Creates a TOpenFontSpec object.
	2. Sets name, height, italic and bold
	3. Setting design height to some arbitrary value.
	4. Gets the font which is the nearest to the given font specification.
	5. Checks for errors.	
	6. Verifies that existing font object created with the GetNearestFontToDesignHeightInPixels
	   (having the wrong font metrics) should not prevent us from getting a new font object 
	   with the GetNearestFontToMaxHeightInPixels.
	7. Calling the GetNearestFontToMaxHeightInPixels function with
	   different languages should still refer to the same font object,
	   unless the language has an impact on metrics.
	 
	@SYMTestExpectedResults
	Test should pass
*/
void CTFontMetrics::FontAccessInCTypefaceStore()
	{
	TOpenFontSpec openFontSpec = GetTOpenFontSpec();
	const TInt KMaxHeight = 24;

	TInt rc = KErrGeneral;

	CFont* font01 = NULL;
	rc = iFontStore->GetNearestFontToDesignHeightInPixels(font01, openFontSpec);
	TEST(KErrNone == rc);
	TEST(NULL != font01);

	CFont* font02 = NULL;
	rc = iFontStore->GetNearestFontToDesignHeightInPixels(font02, openFontSpec);
	TEST(KErrNone == rc);
	TEST(NULL != font02);

	CFont* font03 = NULL;
	rc = iFontStore->GetNearestFontToMaxHeightInPixels(font03, openFontSpec, KMaxHeight);
	TEST(KErrNone == rc);
	TEST(NULL != font03);

	// Setting design height to some arbitrary value
	CFont* font04 = NULL;
	openFontSpec.SetHeight(0);
	rc = iFontStore->GetNearestFontToMaxHeightInPixels(font04, openFontSpec, KMaxHeight);
	TEST(KErrNone == rc);
	TEST(NULL != font04);

	// Setting design height to some arbitrary value
	CFont* font05 = NULL;
	openFontSpec.SetHeight(font03->HeightInPixels());
	rc = iFontStore->GetNearestFontToMaxHeightInPixels(font05, openFontSpec, KMaxHeight);
	TEST(KErrNone == rc);
	TEST(NULL != font05);

	// Setting design height to that of an old-API generated font object
	CFont* font06 = NULL;
	openFontSpec.SetHeight(font01->HeightInPixels());
	rc = iFontStore->GetNearestFontToMaxHeightInPixels(font06, openFontSpec, KMaxHeight);
	TEST(KErrNone == rc);
	TEST(NULL != font06);

	CFont* font07 = NULL;
	openFontSpec.SetScriptTypeForMetrics(ELangEnglish);
	rc = iFontStore->GetNearestFontToMaxHeightInPixels(font07, openFontSpec, KMaxHeight);
	TEST(KErrNone == rc);
	TEST(NULL != font07);

	CFont* font08 = NULL;
	openFontSpec.SetScriptTypeForMetrics(ELangSwedish);
	rc = iFontStore->GetNearestFontToMaxHeightInPixels(font08, openFontSpec, KMaxHeight);
	TEST(KErrNone == rc);
	TEST(NULL != font08);

	// Using the same font specifications should return font objects from cache.
	TEST(font01 == font02);
	TEST(font03 == font04);
	TEST(font03 == font05);
	// Design height should be ignored when calling the GetNearestFontToMaxHeight... API.
	TEST(font01 != font03);
	// The existing font object created with the GetNearestFontToDesignHeight... API
	// (thus having the wrong font metrics) should NOT prevent us from getting a new font
	// object with the GetNearestFontToMaxHeight... API (with improved metrics.)
	TEST(font01 != font06);
	// Languages belonging to the same script should return the same font object.
	TEST(font07 == font08);
	// Calling the GetNearestFontToMaxHeight... API with
	// languages belonging to different scripts may refer to different font objects.
	// (Actually different with DejaVu fonts between no script specified and latin script.)
	TEST(font06 != font07);

	iFontStore->ReleaseFont(font01);
	iFontStore->ReleaseFont(font02);
	iFontStore->ReleaseFont(font03);
	iFontStore->ReleaseFont(font04);
	iFontStore->ReleaseFont(font05);
	iFontStore->ReleaseFont(font06);
	iFontStore->ReleaseFont(font07);
	iFontStore->ReleaseFont(font08);
	}

/**
	@SYMTestCaseID
	GRAPHICS-FNTSTORE-0030

	@SYMTestCaseDesc
	Tests if two loaded fonts are the same if their glyphs are the same. 
	Second request for the font with different flags should give reference to the 
	same font.

	@SYMTestActions
	1. Creates a TOpenFontSpec object and sets name, height, italic and bold.
	2. Gets the font which is the nearest to the given font specification.
	3. Checks for errors.
	4. Gets the font specification in twips.
	5. Gets the font which is the nearest to the given font specification.
	6. Checks for errors.
	7. Verifies the fonts are the same.
	8. Releases the fonts.
	
	@SYMTestExpectedResults
	The fonts should be the same.
*/
void CTFontMetrics::TestINC086513()
	{
	TOpenFontSpec openFontSpec = GetTOpenFontSpec();
	CFont* theFont = NULL;
	TInt rc = iFontStore->GetNearestFontToDesignHeightInPixels(theFont, openFontSpec);
	TEST(KErrNone == rc);
	TFontSpec fontSpec = theFont->FontSpecInTwips();
	TTypeface typeface;
	typeface.iName=fontSpec.iTypeface.iName;
	typeface.SetIsProportional(!fontSpec.iTypeface.IsProportional());
	CFont* theFont1 = NULL;
	rc = iFontStore->GetNearestFontToDesignHeightInPixels(theFont1, openFontSpec);
	TEST(rc==KErrNone);

	TEST(theFont1==theFont);//fonts should be the same

	iFontStore->ReleaseFont(theFont1);
	iFontStore->ReleaseFont(theFont);
	}
	
/*
This test first marks a font as 'in use' and later tries to 
delete all the fonts in the font store by sending the special
code KNullUid. Before this would crash the server because 
there are still fonts in use, but now it would just return safely.
*/
void CTFontMetrics::TestPDEF099034()
	{
	INFO_PRINTF1(_L("Trying to remove all fonts in the font store\r\n"));
	
	TOpenFontSpec openFontSpec = GetTOpenFontSpec();
	CFont* font = NULL;
	TInt ret = iFontStore->GetNearestFontToDesignHeightInPixels(font, openFontSpec);
	TEST(ret == KErrNone);
	
	TUid uid;
	uid.iUid=0;
	iFontStore->RemoveFile(uid);
	
	iFontStore->ReleaseFont(font);
	}

/**
	@SYMTestCaseID GRAPHICS-FNTSTORE-0020
	
	@SYMDEF DEF101441

	@SYMTestCaseDesc Tests GetNearestFontToMaxHeightInPixels with boundary max height and above.
					 Tests GetNearestFontToMaxHeightInPixels with boundary min height i,e 1.

	@SYMTestPriority High
	
	@SYMTestStatus Implemented

	@SYMTestActions 1. Call GetNearestFontToMaxHeightInPixels with max height as 1025
					2  Call GetNearestFontToMaxHeightInPixels with max height as 1
					3. Call GetNearestFontToMaxHeightInPixels with max height as 1024
	API Calls:
	CFontStore::GetNearestFontToMaxHeightInPixels()

	@SYMTestExpectedResults 1. For first call it should return KErrTooBig
							2. For second call it should return KErrArgument
							3. For Third call it should create the font with out any problem
							return KErrNone.
*/
void CTFontMetrics::TestGetNearestFontToMaxHeightInPixels()
	{
	TOpenFontSpec openFontSpec = GetTOpenFontSpec();
	CFont* theFont = NULL;
	
	TInt rc = iFontStore->GetNearestFontToMaxHeightInPixels(theFont, openFontSpec, 1025);
	TEST(KErrTooBig == rc);
	TEST(NULL == theFont);

	rc = iFontStore->GetNearestFontToMaxHeightInPixels(theFont, openFontSpec, 1);
	TEST(KErrArgument == rc);
	TEST(NULL == theFont);

	rc = iFontStore->GetNearestFontToMaxHeightInPixels(theFont, openFontSpec, 1024);
	TEST(NULL != theFont);

	iFontStore->ReleaseFont(theFont);
	}

//--------------
__CONSTRUCT_STEP__(FontMetrics)
