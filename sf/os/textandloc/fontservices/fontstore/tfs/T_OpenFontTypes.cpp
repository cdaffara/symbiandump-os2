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

#include <openfont.h>
#include "T_OpenFontTypes.h"

CTOpenFontTypes::CTOpenFontTypes(CTestStep* aStep) :
	CTGraphicsBase(aStep)
	{
	}

void CTOpenFontTypes::RunTestCaseL(TInt aCurTestCase)
	{
	TOpenFontSpec openFontSpec;
	((CTOpenFontTypesStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);

	switch ( aCurTestCase )
		{
	case 1:
		((CTOpenFontTypesStep*)iStep)->SetTestStepID(_L("GRAPHICS-FNTSTORE-0036"));
		TestBitmapType(openFontSpec);
		((CTOpenFontTypesStep*)iStep)->RecordTestResultL();
		((CTOpenFontTypesStep*)iStep)->SetTestStepID(_L("GRAPHICS-FNTSTORE-0037"));
		TestHeight(openFontSpec);
		((CTOpenFontTypesStep*)iStep)->RecordTestResultL();
		((CTOpenFontTypesStep*)iStep)->SetTestStepID(_L("GRAPHICS-FNTSTORE-0038"));
		TestWidthFactor(openFontSpec);
		((CTOpenFontTypesStep*)iStep)->RecordTestResultL();
		((CTOpenFontTypesStep*)iStep)->SetTestStepID(_L("GRAPHICS-FNTSTORE-0039"));
		TestSlantFactor(openFontSpec);
		((CTOpenFontTypesStep*)iStep)->RecordTestResultL();
		((CTOpenFontTypesStep*)iStep)->SetTestStepID(_L("GRAPHICS-FNTSTORE-0001"));	
		TestFontEffect(openFontSpec);
		break;
	case 2:
		((CTOpenFontTypesStep*)iStep)->SetTestStepID(_L("GRAPHICS-FNTSTORE-0040"));
		TestCompatibilityWithTFontSpec();
		((CTOpenFontTypesStep*)iStep)->RecordTestResultL();
		((CTOpenFontTypesStep*)iStep)->SetTestStepID(_L("GRAPHICS-FNTSTORE-0041"));
		TestOperatorEqualEqual();
		((CTOpenFontTypesStep*)iStep)->RecordTestResultL();
		((CTOpenFontTypesStep*)iStep)->SetTestStepID(_L("GRAPHICS-FNTSTORE-0042"));
		TestCompensateForAspectRatio();
		break;
	case 3:
        	((CTOpenFontTypesStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
		((CTOpenFontTypesStep*)iStep)->CloseTMSGraphicsStep();
		TestComplete();		
		break;
		}
	((CTOpenFontTypesStep*)iStep)->RecordTestResultL();
	}

/**
	@SYMTestCaseID
	GRAPHICS-FNTSTORE-0036

	@SYMTestCaseDesc
	Tests setting and retrieving the bitmap type.

	@SYMTestActions
	1. Used bitmap types: EDefaultGlyphBitmap, EMonochromeGlyphBitmap, EAntiAliasedGlyphBitmap
	1. Sets the bitmap type.
	2. Retrieves the bitmap type and check if they are equal.
	
	@SYMTestExpectedResults
	Test should pass
*/
void CTOpenFontTypes::TestBitmapType(TOpenFontSpec& aOpenFontSpec)
	{
	const TGlyphBitmapType glyphBitmapType = aOpenFontSpec.BitmapType();

	aOpenFontSpec.SetBitmapType(EDefaultGlyphBitmap);
	TEST(EDefaultGlyphBitmap == aOpenFontSpec.BitmapType());
	aOpenFontSpec.SetBitmapType(EMonochromeGlyphBitmap);
	TEST(EMonochromeGlyphBitmap == aOpenFontSpec.BitmapType());
	aOpenFontSpec.SetBitmapType(EAntiAliasedGlyphBitmap);
	TEST(EAntiAliasedGlyphBitmap == aOpenFontSpec.BitmapType());

	aOpenFontSpec.SetBitmapType(glyphBitmapType);
	}

/**
	@SYMTestCaseID
	GRAPHICS-FNTSTORE-0037

	@SYMTestCaseDesc
	Tests setting and getting the height of the bitmap.

	@SYMTestActions
	1. Retrieves the height of the bitmap.
	2. Sets the height of the bitmap.
	3. Retrieves the height and checks if they are the same.
	
	@SYMTestExpectedResults
	Heights should be equal.
*/
void CTOpenFontTypes::TestHeight(TOpenFontSpec& aOpenFontSpec)
	{
	const TInt height = aOpenFontSpec.Height();
	aOpenFontSpec.SetHeight(height);
	TEST(height == aOpenFontSpec.Height());
	}

/**
	@SYMTestCaseID
	GRAPHICS-FNTSTORE-0038

	@SYMTestCaseDesc
	Tests setting and getting the width of the bitmap.

	@SYMTestActions
	1. Retrieves the width of the bitmap.
	2. Sets the width of the bitmap.
	3. Retrieves the width and checks if they are the same.
	
	@SYMTestExpectedResults
	Widths should be equal.
*/
void CTOpenFontTypes::TestWidthFactor(TOpenFontSpec& aOpenFontSpec)
	{
	const TInt32 widthFactor = aOpenFontSpec.WidthFactor();
	aOpenFontSpec.SetWidthFactor(widthFactor);
	TEST(widthFactor == aOpenFontSpec.WidthFactor());
	}

/**
	@SYMTestCaseID
	GRAPHICS-FNTSTORE-0039

	@SYMTestCaseDesc
	Tests setting and getting the slant factor of the bitmap.

	@SYMTestActions
	1. Retrieves the slant factor of the bitmap.
	2. Sets the slant factor of the bitmap.
	3. Retrieves the slant factor and checks if they are the same.
	
	@SYMTestExpectedResults
	Slant factor should be equal.
*/
void CTOpenFontTypes::TestSlantFactor(TOpenFontSpec& aOpenFontSpec)
	{
	const TInt32 slantFactor = aOpenFontSpec.SlantFactor();
	aOpenFontSpec.SetSlantFactor(slantFactor);
	TEST(slantFactor == aOpenFontSpec.SlantFactor());
	}

/**
   @SYMTestCaseID          GRAPHICS-FNTSTORE-0001

   @SYMPREQ                PREQ1301

   @SYMREQ                 REQ5381

   @SYMTestCaseDesc        Getting/setting font effects on TOpenFontSpec

   @SYMTestPriority        Critical

   @SYMTestStatus          Implemented

   @SYMTestActions         (1) Call SetEffects with attributes EAlgorithmicBold and EFalse
                           (2) Call IsEffectOn with attribute EAlgorithmicBold to check (1)
                           (3) Call SetEffects with attributes EAlgorithmicBold and ETrue
                           (4) Call IsEffectOn with attribute EAlgorithmicBold to check (3)
                           (5) Call SetEffects/Effects to verify the attribute set in (3)
                           (6) Repeat (1) - (5) with all supported font effects attributes

   @SYMTestExpectedResults Getters should return the expected results set by setters
                           (2) IsEffectOn should return a value EFalse
                           (4) IsEffectOn should return a value ETrue
                           (5) Effects should return the value set by SetEffects
*/
void CTOpenFontTypes::TestFontEffect(TOpenFontSpec& aOpenFontSpec)
	{
	aOpenFontSpec.SetEffects(FontEffect::EAlgorithmicBold, EFalse);
	TEST(!aOpenFontSpec.IsEffectOn(FontEffect::EAlgorithmicBold));
	aOpenFontSpec.SetEffects(FontEffect::EAlgorithmicBold, ETrue);
	TEST( aOpenFontSpec.IsEffectOn(FontEffect::EAlgorithmicBold));
	VerifyFontEffect(aOpenFontSpec);

	aOpenFontSpec.SetEffects(FontEffect::EDropShadow, EFalse);
	TEST(!aOpenFontSpec.IsEffectOn(FontEffect::EDropShadow));
	aOpenFontSpec.SetEffects(FontEffect::EDropShadow, ETrue);
	TEST( aOpenFontSpec.IsEffectOn(FontEffect::EDropShadow));
	VerifyFontEffect(aOpenFontSpec);

	aOpenFontSpec.SetEffects(FontEffect::EOutline, EFalse);
	TEST(!aOpenFontSpec.IsEffectOn(FontEffect::EOutline));
	aOpenFontSpec.SetEffects(FontEffect::EOutline, ETrue);
	TEST( aOpenFontSpec.IsEffectOn(FontEffect::EOutline));
	VerifyFontEffect(aOpenFontSpec);

	aOpenFontSpec.SetEffects(FontEffect::EEmbossed, EFalse);
	TEST(!aOpenFontSpec.IsEffectOn(FontEffect::EEmbossed));
	aOpenFontSpec.SetEffects(FontEffect::EEmbossed, ETrue);
	TEST( aOpenFontSpec.IsEffectOn(FontEffect::EEmbossed));
	VerifyFontEffect(aOpenFontSpec);

	aOpenFontSpec.SetEffects(FontEffect::EEngraved, EFalse);
	TEST(!aOpenFontSpec.IsEffectOn(FontEffect::EEngraved));
	aOpenFontSpec.SetEffects(FontEffect::EEngraved, ETrue);
	TEST( aOpenFontSpec.IsEffectOn(FontEffect::EEngraved));
	VerifyFontEffect(aOpenFontSpec);

	aOpenFontSpec.SetEffects(FontEffect::ESoftEdge, EFalse);
	TEST(!aOpenFontSpec.IsEffectOn(FontEffect::ESoftEdge));
	aOpenFontSpec.SetEffects(FontEffect::ESoftEdge, ETrue);
	TEST( aOpenFontSpec.IsEffectOn(FontEffect::ESoftEdge));
	VerifyFontEffect(aOpenFontSpec);
	}

void CTOpenFontTypes::VerifyFontEffect(TOpenFontSpec& aOpenFontSpec)
	{
	const TUint32 effects = aOpenFontSpec.Effects();
	aOpenFontSpec.SetEffects(effects);
	TEST(effects == aOpenFontSpec.Effects());
	}

/**
	@SYMTestCaseID
	GRAPHICS-FNTSTORE-0040

	@SYMTestCaseDesc
	Tests compatibility with font specifications.
	
	@SYMTestActions
	1. Constructs a TFontSpec object with the specified typeface and height. 
	   The object's font style is set to the default: EPostureUpright, EStrokeWeightNormal, 
	   and EPrintPosNormal.

	2. Specifies the script with which font metrics calculation will be based on.
	3. Sets the typeface's symbol attribute.
	4. The object's members are initialised from the values of the aFontSpec parameter.
	5. Gets the TFontSpec corresponding to this Open Font System font specification.
	
	@SYMTestExpectedResults
	The open font spec properties should match.
*/
void CTOpenFontTypes::TestCompatibilityWithTFontSpec()
	{
	_LIT(KFontName, "Bullock");
	const TInt height = 10;
	TFontSpec fontSpec0(KFontName, height);
	fontSpec0.SetScriptTypeForMetrics(ELangFinnish);
	fontSpec0.iTypeface.SetIsSymbol(ETrue);

	const TOpenFontSpec openFontSpec(fontSpec0);
	TEST(0 == openFontSpec.Name().Compare(KFontName));
	TEST(0 == openFontSpec.SlantFactor());
	TEST(KOneIn16Dot16FixedPointFormat == openFontSpec.WidthFactor());
	TEST(height == openFontSpec.Height());
	TEST(openFontSpec.Effects()			== fontSpec0.iFontStyle.Effects());
	TEST(openFontSpec.BitmapType()		== fontSpec0.iFontStyle.BitmapType());
	TEST(openFontSpec.PrintPosition()	== fontSpec0.iFontStyle.PrintPosition());
	TEST(openFontSpec.ScriptTypeForMetrics() == fontSpec0.iTypeface.ScriptTypeForMetrics());

	TFontSpec fontSpec1;
	openFontSpec.GetTFontSpec(fontSpec1);
	TEST(fontSpec0 == fontSpec1);
	}

/**
	@SYMTestCaseID
	GRAPHICS-FNTSTORE-0041

	@SYMTestCaseDesc
	Compares two open font specifications. 
	Compares all the properties of the two fonts.
	
	@SYMTestActions
	Uses the '==' operator to compare two font specifications.
	
	@SYMTestExpectedResults
	Test should pass
*/
void CTOpenFontTypes::TestOperatorEqualEqual()
	{
	const TOpenFontSpec openFontSpec0, openFontSpec1;
	TEST(openFontSpec0 == openFontSpec1);
	}

/**
	@SYMTestCaseID
	GRAPHICS-FNTSTORE-0042

	@SYMTestCaseDesc
	Tests compensation for aspect ratio.

	@SYMTestActions
	1. Creates a TOpenFontSpec object.
	2. Sets the slant factor to 0x10000
	3. Gets the width factor and slant factor.
	4. Adjust the width factor and slant factor to suit a pixel aspect ratio.
	5. Verifies the compensated aspect ratio.
	6. Changes the height and does step four and five again.
	7. Changes the height, retrieves the width and slant factor, compensate for
	   aspect ratio and verifies the aspect ratio.
	
	@SYMTestExpectedResults
	Test should pass
*/
void CTOpenFontTypes::TestCompensateForAspectRatio()
	{
	TOpenFontSpec openFontSpec;
	openFontSpec.SetSlantFactor(KOneIn16Dot16FixedPointFormat);
	TInt32 widthFactor = openFontSpec.WidthFactor();
	TInt32 slantFactor = openFontSpec.SlantFactor();
	const TInt width = 1000;
	TInt height = 1000;
	openFontSpec.CompensateForAspectRatio(width, height);
	VerifyCompensateForAspectRatio(openFontSpec, width, height, widthFactor, slantFactor);

	height = 998;
	openFontSpec.CompensateForAspectRatio(width, height);
	VerifyCompensateForAspectRatio(openFontSpec, width, height, widthFactor, slantFactor);

	height = 1002;
	widthFactor = openFontSpec.WidthFactor();
	slantFactor = openFontSpec.SlantFactor();
	openFontSpec.CompensateForAspectRatio(width, height);
	VerifyCompensateForAspectRatio(openFontSpec, width, height, widthFactor, slantFactor);
	}

void CTOpenFontTypes::VerifyCompensateForAspectRatio(
	const TOpenFontSpec&	aOpenFontSpec,
	TInt					aWidth,
	TInt					aHeight,
	TInt32					aWidthFactorB4Compensation,
	TInt32					aSlantFactorB4Compensation)
	{
	const TReal aspectRatio = (TReal) aHeight / (TReal) aWidth;
	TEST((TInt32)(aWidthFactorB4Compensation * aspectRatio) == aOpenFontSpec.WidthFactor());
	TEST((TInt32)(aSlantFactorB4Compensation / aspectRatio) == aOpenFontSpec.SlantFactor());
	}

//--------------
__CONSTRUCT_STEP__(OpenFontTypes)
