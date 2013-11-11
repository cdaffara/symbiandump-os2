/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#include "FNTSTD.H"
#include "t_linkedfontscapability.h"
#include <hal.h>
#include <linkedfonts.h>

/**
@file 

@SYMPurpose This file is used for tests to check the security capabilities
of the the calls to the font and bitmap server regarding linked fonts

Registering a linked font requires write device data.  Fetching a linked
font requires read device data.

The function uses an ini file to indicate to this file what tests should be run.
There are 3 tests, read and write, read, write.  It is important to register a
the linked font test first so that it can be fetched.  The test script does this by 
calling tests which do not panic, initially.

*/
_LIT(KLinkedFontsCapabilityFont,"CapabilityTestFnt");
_LIT(KLinkedFontsCaseNumber,"CaseNumber");



void CTLinkedFontsCapability::TestRegisterL()
	{
	CFbsTypefaceStore* store = CFbsTypefaceStore::NewL(iBitmapDevice);
	CleanupStack::PushL(store);
	
	// Create typeface to be linked
	CLinkedTypefaceSpecification *typefaceSpec;
	typefaceSpec = CLinkedTypefaceSpecification::NewLC(KLinkedFontsCapabilityFont);
	
	CLinkedTypefaceGroup* group1 = CLinkedTypefaceGroup::NewLC(1);
	typefaceSpec->AddLinkedTypefaceGroupL(*group1);
	CleanupStack::Pop();
	
	// Add first typeface
	CLinkedTypefaceElementSpec *elementSpec1;
	_LIT(KLinkedTypefaceTT, "DejaVu Sans Condensed");
	elementSpec1 = CLinkedTypefaceElementSpec::NewLC(KLinkedTypefaceTT, 1);
	elementSpec1->SetCanonical(ETrue);
		
	typefaceSpec->AddTypefaceAtBackL(*elementSpec1);
	CleanupStack::Pop(1, elementSpec1);
	
	// Now try regstering the linked typeface
	TRAPD(ret, typefaceSpec->RegisterLinkedTypefaceL(*store));
	CleanupStack::PopAndDestroy(2, store);
	
	if (ret!=KErrAlreadyExists)
		{
		User::LeaveIfError(ret);
		}
	}

void CTLinkedFontsCapability::CheckLinkedFontsRasterizerInstalledL()
	{
	CFbsTypefaceStore* store = CFbsTypefaceStore::NewL(iBitmapDevice);
	CleanupStack::PushL(store);
	
	// Create typeface to be linked
	CLinkedTypefaceSpecification *typefaceSpec;
	typefaceSpec = CLinkedTypefaceSpecification::NewLC(KLinkedFontsCapabilityFont);
	
	CLinkedTypefaceGroup* group1 = CLinkedTypefaceGroup::NewLC(1);
	typefaceSpec->AddLinkedTypefaceGroupL(*group1);
	CleanupStack::Pop();
	
	// Add first typeface
	CLinkedTypefaceElementSpec *elementSpec1;
	_LIT(KLinkedTypefaceTT, "DejaVu Sans Condensed");
	elementSpec1 = CLinkedTypefaceElementSpec::NewLC(KLinkedTypefaceTT, 1);
	elementSpec1->SetCanonical(ETrue);
		
	typefaceSpec->AddTypefaceAtBackL(*elementSpec1);
	CleanupStack::Pop(1, elementSpec1);
	
	// Now try regstering the linked typeface
	TRAPD(ret, typefaceSpec->RegisterLinkedTypefaceL(*store));
	CleanupStack::PopAndDestroy(2, store);

	switch (ret)
		{
		case KErrAlreadyExists:
		case KErrNone:
			iHaveRasterizer = ETrue;
		default:
			break;			
		}
	
	if (!iHaveRasterizer)
		{
		INFO_PRINTF1(_L("Linked fonts capability: no linked fonts rasterizer found"));
		}
	}

TBool CTLinkedFontsCapability::LinkedFontsRasterizerPresent()
	{
	return iHaveRasterizer;
	}


void CTLinkedFontsCapability::TestFetchL()
	{
	CFbsTypefaceStore* store = CFbsTypefaceStore::NewL(iBitmapDevice);
	CleanupStack::PushL(store);
	
	CLinkedTypefaceSpecification *typefaceSpec;
	typefaceSpec = CLinkedTypefaceSpecification::NewLC(KLinkedFontsCapabilityFont);
	
	typefaceSpec->FetchLinkedTypefaceSpecificationL(*store);
	
	CleanupStack::PopAndDestroy(2, store);
	}

CTLinkedFontsCapability::~CTLinkedFontsCapability()
	{
	delete iGc;
	delete iBitmapDevice;
	delete iBitmap;
	}

CTLinkedFontsCapability::CTLinkedFontsCapability(CTestStep *aStep):
	CTGraphicsBase(aStep),
	iHaveRasterizer(EFalse)
	{	
	}

/** Create the screen device.
@param aMode display mode for which device has to be created
*/
void CTLinkedFontsCapability::CreateBitmapDeviceL(TDisplayMode aMode)
	{
	iBitmap = new(ELeave) CFbsBitmap;
	//Considering the intersection of the screen size for both hw and emulator so that same
	//baselined hash can be used to test for both
	iBitmap->Create(TSize(40, 40), aMode); //small, bitmap not used
	iBitmapDevice = CFbsBitmapDevice::NewL(iBitmap);
	User::LeaveIfError(iBitmapDevice->CreateContext((CGraphicsContext*&)iGc));
	}

/**
@SYMTestCaseID GRAPHICS-FNTSTORE-LINKEDFONTS-0110

@SYMTestPriority Critical

@SYMPREQ				PREQ2146

@SYMREQ					REQ10924, REQ10925, REQ10926

@SYMTestCaseDesc 
Checks security capabilities for linked fonts

@SYMTestActions 
1 - Run the test case with read device data and write device data
2 - Run the test case with write device data
3 - Run the test case with read device data
4 - Run the test case with no capabilities

@SYMTestExpectedResults 
1- A linked font can be registered or returns KErrAlreadyExists and linked fonts can be fetched.
2- Registering a linked font fails with panic E32USER-CBase panic 47, and linked fonts can be fetched.
3- Registering a linked font succeeds or returns KErrAlreadyExists and fetching a linked font fail with panic E32USER-CBase panic 47.
4- Registering a linked font fails with panic E32USER-CBase panic 47. Fetching a linked font fails with panic E32USER-CBase panic 47.

*/
void CTLinkedFontsCapability::RunTestCaseL(TInt /*aCurTestCase*/)
	{
	INFO_PRINTF1(_L("Linked fonts capability  tests" ));
	TInt curTestCase;
		//Read the case number from the ini file
	TBool res = iStep->GetIntFromConfig(iStep->ConfigSection(), KLinkedFontsCaseNumber, curTestCase);		
	if(!res)
		{
		TEST(EFalse);
		return ;
		}	
	
	CreateBitmapDeviceL(EColor16MU);
	TInt err=KErrNone;
	TInt err2=KErrNone;

	((CTLinkedFontsCapabilityStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);

	switch (curTestCase)
		{
		case 1:
			((CTLinkedFontsCapabilityStep*)iStep)->SetTestStepID(_L("GRAPHICS-FNTSTORE-LINKEDFONTS-0110"));
			CheckLinkedFontsRasterizerInstalledL();
			break;		
		case 2:
			((CTLinkedFontsCapabilityStep*)iStep)->SetTestStepID(_L("GRAPHICS-FNTSTORE-LINKEDFONTS-0110"));
			if (LinkedFontsRasterizerPresent())
				{
				TestRegisterL();
				TestFetchL();
				}
			break;
		case 3:
			((CTLinkedFontsCapabilityStep*)iStep)->SetTestStepID(_L("GRAPHICS-FNTSTORE-LINKEDFONTS-0110"));
			if (LinkedFontsRasterizerPresent())
				{
				TestRegisterL();//write only
				}
			break;
		case 4:
			((CTLinkedFontsCapabilityStep*)iStep)->SetTestStepID(_L("GRAPHICS-FNTSTORE-LINKEDFONTS-0110"));
			if (LinkedFontsRasterizerPresent())
				{
				TestFetchL();//read only
				}
			break;
			
		//following are similar to tests 2-4, but leaves if no permission to access the capabilities
		case 5:
			((CTLinkedFontsCapabilityStep*)iStep)->SetTestStepID(_L("GRAPHICS-FNTSTORE-LINKEDFONTS-0110"));
			if(LinkedFontsRasterizerPresent())
				{
				TRAP(err,TestRegisterL());
				TRAP(err2, TestFetchL());
				TEST((err == KErrPermissionDenied)&&(err2 == KErrPermissionDenied));
				}
			break;
		case 6:
			((CTLinkedFontsCapabilityStep*)iStep)->SetTestStepID(_L("GRAPHICS-FNTSTORE-LINKEDFONTS-0110"));
			if(LinkedFontsRasterizerPresent())
				{
				TRAP(err,TestRegisterL());//write only
				TEST(err == KErrPermissionDenied);
				}
			break;
		case 7:
			((CTLinkedFontsCapabilityStep*)iStep)->SetTestStepID(_L("GRAPHICS-FNTSTORE-LINKEDFONTS-0110"));
			if(LinkedFontsRasterizerPresent())
				{
				TRAP(err, TestFetchL());//read only
				TEST(err == KErrPermissionDenied);
				}
			break;			
		default:
			{
			((CTLinkedFontsCapabilityStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
			((CTLinkedFontsCapabilityStep*)iStep)->CloseTMSGraphicsStep();
			TEST(EFalse); //nothing should have the default value
			break;
			}
		}
	((CTLinkedFontsCapabilityStep*)iStep)->RecordTestResultL();
	TEST(ETrue);
	((CTLinkedFontsCapabilityStep*)iStep)->CloseTMSGraphicsStep();
	TestComplete();
	}

__CONSTRUCT_STEP__(LinkedFontsCapability)

