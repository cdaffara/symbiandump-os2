/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

// the only way to get at the local to file static function MatchFontSpecsInPixels
#include "FNTSTORE.CPP"
#include "T_MatchFonts.h"

_LIT( KDjvMono, "DejaVu Sans Mono" );
_LIT( KDjvSerif, "DejaVu Serif Condensed" );
_LIT( KDjvSans, "DejaVu Sans Condensed" );

class CTMatchFonts : public CTGraphicsBase
	{
public:
	CTMatchFonts(CTestStep* aStep);

protected:
//from 	CTGraphicsStep
	virtual void RunTestCaseL(TInt aCurTestCase);
	virtual void ConstructL();
private:
	TOpenFontSpec iIdealFontSpec1;
	TOpenFontSpec iIdealFontSpec2;
	TOpenFontSpec iIdealFontSpec3;
	TOpenFontSpec iDjvMonoFontSpec;
	TOpenFontSpec iDjvSerifFontSpec;
	TOpenFontSpec iDjvSansFontSpec;
	};

CTMatchFonts::CTMatchFonts(CTestStep* aStep) :
	CTGraphicsBase(aStep)
	{
	
	}

void CTMatchFonts::ConstructL()
	{

	iIdealFontSpec1.SetName( KNullDesC );
	iIdealFontSpec1.SetHeight( 10 );
	iIdealFontSpec1.SetItalic( EFalse );
	iIdealFontSpec1.SetSlantFactor( 0 );
	iIdealFontSpec1.SetBold( EFalse );
	iIdealFontSpec1.SetBitmapType( EMonochromeGlyphBitmap );
	iIdealFontSpec1.SetMonoWidth( ETrue );
	iIdealFontSpec1.SetSerif( ETrue );

	iDjvMonoFontSpec.SetName( KDjvMono );
	iDjvMonoFontSpec.SetHeight( 10 );
	iDjvMonoFontSpec.SetItalic( EFalse );
	iDjvMonoFontSpec.SetSlantFactor( 0 );
	iDjvMonoFontSpec.SetBold( EFalse );
	iDjvMonoFontSpec.SetBitmapType( EMonochromeGlyphBitmap );
	iDjvMonoFontSpec.SetMonoWidth( ETrue );
	iDjvMonoFontSpec.SetSerif( ETrue );

	iIdealFontSpec2.SetName( KNullDesC );
	iIdealFontSpec2.SetHeight( 10 );
	iIdealFontSpec2.SetItalic( EFalse );
	iIdealFontSpec2.SetSlantFactor( 0 );
	iIdealFontSpec2.SetBold( EFalse );
	iIdealFontSpec2.SetBitmapType( EMonochromeGlyphBitmap );
	iIdealFontSpec2.SetMonoWidth( EFalse );
	iIdealFontSpec2.SetSerif( ETrue );

	iDjvSerifFontSpec.SetName( KDjvSerif );
	iDjvSerifFontSpec.SetHeight( 10 );
	iDjvSerifFontSpec.SetItalic( EFalse );
	iDjvSerifFontSpec.SetSlantFactor( 0 );
	iDjvSerifFontSpec.SetBold( EFalse );
	iDjvSerifFontSpec.SetBitmapType( EMonochromeGlyphBitmap );
	iDjvSerifFontSpec.SetMonoWidth( EFalse );
	iDjvSerifFontSpec.SetSerif( ETrue );

	iIdealFontSpec3.SetName( KNullDesC );
	iIdealFontSpec3.SetHeight( 10 );
	iIdealFontSpec3.SetItalic( EFalse );
	iIdealFontSpec3.SetSlantFactor( 0 );
	iIdealFontSpec3.SetBold( EFalse );
	iIdealFontSpec3.SetBitmapType( EMonochromeGlyphBitmap );
	iIdealFontSpec3.SetMonoWidth( EFalse );
	iIdealFontSpec3.SetSerif( EFalse );

	iDjvSansFontSpec.SetName( KDjvSans );
	iDjvSansFontSpec.SetHeight( 10 );
	iDjvSansFontSpec.SetItalic( EFalse );
	iDjvSansFontSpec.SetSlantFactor( 0 );
	iDjvSansFontSpec.SetBold( EFalse );
	iDjvSansFontSpec.SetBitmapType( EMonochromeGlyphBitmap );
	iDjvSansFontSpec.SetMonoWidth( EFalse );
	iDjvSansFontSpec.SetSerif( EFalse );
	
	}
	
/**
	@SYMTestCaseID
	GRAPHICS-FNTSTORE-0035

	@SYMTestCaseDesc
	Creates a reference font and compares it to different fonts
	and sees which font matches the reference font the best.
	
	@SYMTestActions
	1. Reference font: monospaced serif
	   Compared fonts: DejaVu Sans Mono, DejaVu Serif Condensed, DejaVu Sans Condensed
	2. Reference font: proporional serif font
	   Compared fonts: DejaVu Sans Mono, DejaVu Serif Condensed, DejaVu Sans Condensed
	3. Reference font: proporional sans serif
	   Compared fonts: DejaVu Sans Mono, DejaVu Serif Condensed, DejaVu Sans Condensed	   

	@SYMTestExpectedResults
	1. Should choose DejaVu Sans Mono above DejaVu Serif Condensed above DejaVu Sans Condensed
	2. Should choose DejaVu Serif Condensed above DejaVu Sans Condensed above DejaVu Sans Mono
	3. Should choose DejaVu Sans Condensed above DejaVu Serif Condensed above DejaVu Sans Mono
*/
void CTMatchFonts::RunTestCaseL( TInt aCurTestCase )
	{
	TInt weighting1;
	TInt weighting2;
	TInt weighting3;
	((CTMatchFontsStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);

	switch ( aCurTestCase )
		{
	case 1:
		((CTMatchFontsStep*)iStep)->SetTestStepID(_L("GRAPHICS-FNTSTORE-0035"));
		// Looking for a monospaced serif font (like DejaVu Sans Mono)
		weighting1 = MatchFontSpecsInPixels( iDjvMonoFontSpec, iIdealFontSpec1 );
		weighting2 = MatchFontSpecsInPixels( iDjvSerifFontSpec, iIdealFontSpec1 );
		weighting3 = MatchFontSpecsInPixels( iDjvSansFontSpec, iIdealFontSpec1 );
		// Should choose DejaVu Sans Mono above DejaVu Serif Condensed above DejaVu Sans Condensed
		TEST( ( weighting1 > weighting2 ) && ( weighting2 > weighting3 ) );
		break;
	case 2:
		((CTMatchFontsStep*)iStep)->SetTestStepID(_L("GRAPHICS-FNTSTORE-0035"));
		// Looking for a proporional serif font (like DejaVu Serif Condensed)
		weighting1 = MatchFontSpecsInPixels( iDjvMonoFontSpec, iIdealFontSpec2 );
		weighting2 = MatchFontSpecsInPixels( iDjvSerifFontSpec, iIdealFontSpec2 );
		weighting3 = MatchFontSpecsInPixels( iDjvSansFontSpec, iIdealFontSpec2 );
		// Should choose DejaVu Serif Condensed above DejaVu Sans Condensed above DejaVu Sans Mono
		TEST( ( weighting2 > weighting3 ) && ( weighting3 > weighting1 ) );
		break;
	case 3:
		((CTMatchFontsStep*)iStep)->SetTestStepID(_L("GRAPHICS-FNTSTORE-0035"));
		// Looking for a proporional sans serif font (like DejaVu Sans Condensed)
		weighting1 = MatchFontSpecsInPixels( iDjvMonoFontSpec, iIdealFontSpec3 );
		weighting2 = MatchFontSpecsInPixels( iDjvSerifFontSpec, iIdealFontSpec3 );
		weighting3 = MatchFontSpecsInPixels( iDjvSansFontSpec, iIdealFontSpec3 );
		// Should choose DejaVu Sans Condensed above DejaVu Serif Condensed above DejaVu Sans Mono
		TEST( ( weighting3 > weighting2 ) && ( weighting2 > weighting1 ) );
		break;
	case 4:
		((CTMatchFontsStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
		((CTMatchFontsStep*)iStep)->CloseTMSGraphicsStep();
		TestComplete();		
		break;
		}
	((CTMatchFontsStep*)iStep)->RecordTestResultL();
	}

//--------------
__CONSTRUCT_STEP__(MatchFonts)
