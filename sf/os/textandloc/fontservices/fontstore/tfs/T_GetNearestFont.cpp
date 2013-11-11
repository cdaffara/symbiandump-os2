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
* T_NearestFont.cpp
*
*/


/**
 @file
 @test
 @internalComponent Internal Symbian test code
*/

#include "T_GetNearestFont.h"
#include <hal.h>

_LIT( KFontNotAvailable, "NotAvailableOnSystem" );
_LIT( KTypefaceName, "DejaVu Sans Condensed" );
_LIT( KFontAlias, "Alias");

class CTGetNearestFont : public CTGraphicsBase
	{
public:
	CTGetNearestFont(CTestStep* aStep);
	virtual ~CTGetNearestFont();

protected:
//from 	CTGraphicsStep
	virtual void RunTestCaseL(TInt aCurTestCase);
	virtual void ConstructL();
private:
	void TestINC064346();
	void AliasedFontCreationL();
	void TestFontStoreL();
	void TestSystemDefaultFont();
	
	CFbsTypefaceStore* iTfs;	
	};

CTGetNearestFont::CTGetNearestFont(CTestStep* aStep) :
	CTGraphicsBase(aStep)
	{
	
	}

void CTGetNearestFont::ConstructL()
	{
	iTfs = CFbsTypefaceStore::NewL(NULL);	
	}	
	
 CTGetNearestFont::~CTGetNearestFont()
 	{
 	delete iTfs; 	
 	}
	
void CTGetNearestFont::RunTestCaseL( TInt aCurTestCase )
	{	
	((CTGetNearestFontStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);
	switch ( aCurTestCase )
		{
	case 1:
		((CTGetNearestFontStep*)iStep)->SetTestStepID(_L("GRAPHICS-INC064346-0001"));
		INFO_PRINTF1(_L("TestINC064346 Started"));
		TestINC064346();
		INFO_PRINTF1(_L("TestINC064346 Finished"));
		break;
		
	case 2:
		((CTGetNearestFontStep*)iStep)->SetTestStepID(_L("GRAPHICS-FNTSTORE-0005"));
		INFO_PRINTF1(_L("TestFontStore Started"));
		TestFontStoreL();
		INFO_PRINTF1(_L("TestFontStore Finished"));
		break;	
		
	case 3:
		((CTGetNearestFontStep*)iStep)->SetTestStepID(_L("GRAPHICS-FNTSTORE-0006"));
		INFO_PRINTF1(_L("Test System default font Started"));
		TestSystemDefaultFont();
		INFO_PRINTF1(_L("Test System default font Finished"));
	break;
		
	case 4:
		((CTGetNearestFontStep*)iStep)->SetTestStepID(_L("GRAPHICS-FNTSTORE-0051"));
		INFO_PRINTF1(_L("AliasedFontCreationL Started"));
		AliasedFontCreationL();
		INFO_PRINTF1(_L("AliasedFontCreationL Finished"));
		break;

	case 5:
		((CTGetNearestFontStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
		((CTGetNearestFontStep*)iStep)->CloseTMSGraphicsStep();
		TestComplete();		
		break;
		}
	((CTGetNearestFontStep*)iStep)->RecordTestResultL();
	}

void CTGetNearestFont::TestINC064346()
	{
	/**
	@SYMTestCaseID GRAPHICS-INC064346-0001
	@SYMTestType UT
	@SYMTestPriority Medium
	@SYMDEF INC064346
	@SYMTestCaseDesc Tests that the correct font height is returned when matching with a superscript font.
	@SYMTestActions Get nearest font to design height for a font with the following parameters:
		Font name must be specified that is a font that does not exist on the system.
		Font must be superscript, proportional and serif.
	@SYMTestExpectedResults The correct height must be returned for the nearest font found which is checked with the expected value.
	*/
	TInt ret = KErrGeneral;
	CFont* font = NULL;	
	TFontSpec spec(KFontNotAvailable, 15);
	
	spec.iFontStyle = TFontStyle(EPostureUpright, EStrokeWeightNormal, EPrintPosSuperscript);
	spec.iTypeface.SetIsProportional(ETrue);
	spec.iTypeface.SetIsSerif(ETrue);		
	ret = iTfs->GetNearestFontToDesignHeightInPixels(font, spec);		
	TEST(ret == KErrNone);
	INFO_PRINTF2(_L("Font height = %d"), font->HeightInPixels());		
	TEST(10 == font->HeightInPixels());	
	}

void CTGetNearestFont::TestFontStoreL()
	{
	/**
	@SYMTestCaseID GRAPHICS-FNTSTORE-0005
	@SYMPREQ PREQ1228
	@SYMREQ REQ5921
	@SYMTestCaseDesc Tests the font store optimisation changes for reduce the font load time
	@SYMTestPriority High
	@SYMTestStatus Implemented
	@SYMTestActions Get the nearest font to the design height for a font which exists and one which does not exists
	@SYMTestExpectedResults Time taken for fetching an existing font should be less then time take for fetching a non existent font.
							In addition fetching a non existent font should take almost the same time in principle, as taken by fetching 
							n existing with old method (as old search method checks all existing fonts even if it finds one in between 
							during search procedure).
	*/
	CFont* font = NULL;	
	TFontSpec spec(KFontNotAvailable, 15);
	
	spec.iHeight = 15;
	spec.iTypeface.iName = KFontNotAvailable;
	spec.iFontStyle = TFontStyle(EPostureUpright, EStrokeWeightNormal, EPrintPosSuperscript);
	spec.iTypeface.SetIsProportional(ETrue);		
	
	//Set the Bold and Italic attribute for the font
	TOpenFontSpec openFontSpec(spec);
	openFontSpec.SetBold(ETrue);
	openFontSpec.SetItalic(ETrue);
	openFontSpec.GetTFontSpec(spec);
	
	TInt64 		diffTime[2];
		
	//Run it twice with same font specifications except different typeface name
	//First iteration for Non Existent font and second iteration for Existent font
	for (TInt cntr = 0; cntr < 2; cntr++)
		{		 	
		TInt 		freq = 0;							
		
		TUint32	starttime = User::FastCounter();
		TInt ret = iTfs->GetNearestFontToDesignHeightInPixels(font, spec);
		TUint32 endtime = User::FastCounter();
		TEST(ret == KErrNone);
		
		User::LeaveIfError(HAL::Get(HALData::EFastCounterFrequency, freq));
		TUint32 diff = endtime - starttime;
		diffTime[cntr] = (1000000 * TInt64(diff)) / (TInt64)freq;			

		TFontSpec returnedSpecs = font->FontSpecInTwips();
		INFO_PRINTF3(_L("Font Desired  : Name  = %S Height = %d "), &spec.iTypeface.iName, spec.iHeight);
		INFO_PRINTF4(_L("Font Returned : Name = %S Height = %d Time = %d Microseconds"), &returnedSpecs.iTypeface.iName, font->HeightInPixels(),TUint32(diffTime[cntr]));
		
		//Change the font spec to specify available typeface			
		spec.iTypeface.iName = KTypefaceName;
		
		iTfs->ReleaseFont(font);				
		}

	TEST(diffTime[1] < diffTime[0]);
	INFO_PRINTF2(_L("OPTIMISATION Factor %f"), (TReal(1)-(TReal(diffTime[1])/TReal(diffTime[0]))));	
	}

/**
@SYMTestCaseID GRAPHICS-FNTSTORE-0006
@SYMTestPriority Critical
@SYMREQ REQ6158

@SYMTestCaseDesc 
Default system font

@SYMTestActions 
1 - Test without setting system default font
2 - Set the system default font to typeface 1
3 - Set the system default font to typeface 2
4 - Set the system default font to an empty descriptor 
5 - Set the system default font to large descriptor 
6 - Set the system default font to a font that does not exist 
7 - Set system font to an alias font 

@SYMTestExpectedResults 
The following applies when the typeface name is empty when requesting a font:
If a default system typeface is not set then the nearest font to the specified font will be returned.
If a default system typeface name is set then this will be the returned font.
If the character length of the system typeface is greater than KMaxTypefaceNameLength then a KErrTooBig leave will occur.
*/
void CTGetNearestFont::TestSystemDefaultFont()
	{	
	const TInt numTypefaces = iTfs->NumTypefaces();
	TEST(numTypefaces >= 2);	// Must have at least 2 typefaces
	
	// Get the first typeface
	TTypefaceSupport tfSupport1;
	iTfs->TypefaceSupport(tfSupport1, 0);
	TPtrC typeface1 = tfSupport1.iTypeface.iName;
	
	// Get the second typeface
	TTypefaceSupport tfSupport2;
	iTfs->TypefaceSupport(tfSupport2, 1);
	TPtrC typeface2 = tfSupport2.iTypeface.iName;
	
	// Test without setting system default font and the returned font should not be typeface1 or typeface2
	CFont* font = NULL;	
	TFontSpec spec(KNullDesC, 15);		
	spec.iTypeface.SetIsProportional(ETrue);
	spec.iTypeface.SetIsSerif(ETrue);		
	TInt ret = iTfs->GetNearestFontToDesignHeightInPixels(font, spec);
	TEST(ret == KErrNone);	
	TFontSpec returnedSpecs = font->FontSpecInTwips();
	TEST(returnedSpecs.iTypeface.iName != typeface1 && returnedSpecs.iTypeface.iName != typeface2);
	iTfs->ReleaseFont(font);
		
	// Now set the system default font and the returned font should be typeface1
	iTfs->SetSystemDefaultTypefaceNameL(typeface1);
	ret = iTfs->GetNearestFontToDesignHeightInPixels(font, spec);
	TEST(ret == KErrNone);	
	returnedSpecs = font->FontSpecInTwips();
	TEST(returnedSpecs.iTypeface.iName == typeface1);
	iTfs->ReleaseFont(font);
	
	// Now set the system default font and the returned font should be typeface2
	iTfs->SetSystemDefaultTypefaceNameL(typeface2);
	ret = iTfs->GetNearestFontToDesignHeightInPixels(font, spec);
	TEST(ret == KErrNone);	
	returnedSpecs = font->FontSpecInTwips();
	TEST(returnedSpecs.iTypeface.iName == typeface2);
	iTfs->ReleaseFont(font);
	
	// Now set the system default font to an empty descriptor and the font returned should not be typeface1 or typeface2
	iTfs->SetSystemDefaultTypefaceNameL(KNullDesC);
	ret = iTfs->GetNearestFontToDesignHeightInPixels(font, spec);
	TEST(ret == KErrNone);	
	returnedSpecs = font->FontSpecInTwips();
	TEST(returnedSpecs.iTypeface.iName != typeface1 && returnedSpecs.iTypeface.iName != typeface2);
	iTfs->ReleaseFont(font);
	
	// Set the system default font to large descriptor and we should get an error
	TRAP(ret, iTfs->SetSystemDefaultTypefaceNameL(_L("abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz")));
	TEST(ret == KErrTooBig);		
	
	// Now set the system default font to a font that does not exist and the font returned should not be typeface 1 or typeface2
	_LIT( KFontNotExisting, "FontDoesNotExist" );
	iTfs->SetSystemDefaultTypefaceNameL(KFontNotExisting);
	ret = iTfs->GetNearestFontToDesignHeightInPixels(font, spec);
	TEST(ret == KErrNone);	
	returnedSpecs = font->FontSpecInTwips();
	TEST(returnedSpecs.iTypeface.iName != typeface1 && returnedSpecs.iTypeface.iName != typeface2);
	iTfs->ReleaseFont(font);	
	
	// Now set the system default font and set an alias on typeface1 - the returned font should still be typeface1
	iTfs->SetSystemDefaultTypefaceNameL(typeface1);
	iTfs->SetFontNameAliasL(typeface1, typeface2);
	ret = iTfs->GetNearestFontToDesignHeightInPixels(font, spec);
	TEST(ret == KErrNone);	
	returnedSpecs = font->FontSpecInTwips();
	TEST(returnedSpecs.iTypeface.iName == typeface1);
	iTfs->ReleaseFont(font);
	
	// Now set the system default font to an aliased font - the returned font should still be typeface1
	iTfs->SetSystemDefaultTypefaceNameL(KFontAlias);
	iTfs->SetFontNameAliasL(KFontAlias, typeface1);
	ret = iTfs->GetNearestFontToDesignHeightInPixels(font, spec);
	TEST(ret == KErrNone);
	returnedSpecs = font->FontSpecInTwips();
	TEST(returnedSpecs.iTypeface.iName == typeface1);
	iTfs->ReleaseFont(font);

	// tear down the default font
	iTfs->SetSystemDefaultTypefaceNameL(KNullDesC);
	}

/**
@SYMTestCaseID GRAPHICS-FNTSTORE-0051
@SYMTestType UT
@SYMTestPriority Medium
@SYMDEF INC127988
@SYMTestCaseDesc Tests that an alias does not prevent a real font from being loaded.
@SYMTestActions Create an alias using the name of a real font. Get the real font.  
@SYMTestExpectedResults Ensure the real font, not the aliased font is returned.
*/
void CTGetNearestFont::AliasedFontCreationL()
	{
	TInt numTypefaces = iTfs->NumTypefaces();
	if(numTypefaces <2)
	    return;
	
	TTypefaceSupport tfSupport1;
	
    iTfs->TypefaceSupport(tfSupport1, 0);
    INFO_PRINTF1(tfSupport1.iTypeface.iName);
	// Get the first different typeface
    TTypefaceSupport tfSupport2;
	while (--numTypefaces)
		{
		iTfs->TypefaceSupport(tfSupport2, numTypefaces);
		INFO_PRINTF1(tfSupport2.iTypeface.iName);
		if (tfSupport2.iTypeface.iName != tfSupport1.iTypeface.iName)
			{
			break;
			}
		}

	// Make sure we aren't at the end of the list
	TEST(numTypefaces);
	// Alias the other font with the same name as the original
	iTfs->SetFontNameAliasL(tfSupport1.iTypeface.iName, tfSupport2.iTypeface.iName);

	// Request the original font (making sure we don't get the aliased font back)
	CFont* font = NULL;
	TFontSpec spec(tfSupport1.iTypeface.iName, 0);
	TInt ret = iTfs->GetNearestFontToDesignHeightInPixels(font, spec);
	TEST(ret == KErrNone);
	TFontSpec returnedSpecs = font->FontSpecInTwips();
	INFO_PRINTF1(returnedSpecs.iTypeface.iName);
	TEST(returnedSpecs.iTypeface.iName == tfSupport1.iTypeface.iName);
	iTfs->ReleaseFont(font);
	}

//--------------
__CONSTRUCT_STEP__(GetNearestFont)
