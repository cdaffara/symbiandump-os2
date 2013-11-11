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

#include "T_LinkedFontUpdateTests.h"

CTLinkedFontsUpdateStage1::CTLinkedFontsUpdateStage1(CTestStep* aStep)
:	CTGraphicsBase(aStep)
	{
	iFontLinkingSupported = EFalse;
	}

CTLinkedFontsUpdateStage1::~CTLinkedFontsUpdateStage1()
	{
	}

void CTLinkedFontsUpdateStage1::TestPanic(TInt aPanic)
	{
	_LIT(KFntTestStorePanicCategory,"TestFntStore");
	User::Panic(KFntTestStorePanicCategory,aPanic);
	}

void CTLinkedFontsUpdateStage1::RunTestCaseL(TInt aCurTestCase)
	{
	((CTLinkedFontsUpdateStage1Step*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);
	switch (aCurTestCase)
		{
		case 1:
			((CTLinkedFontsUpdateStage1Step*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
			SetupTestsL();
			break;
		case 2:
			((CTLinkedFontsUpdateStage1Step*)iStep)->SetTestStepID(_L("GRAPHICS-FNTSTORE-LINKEDFONTS-0200"));
			RegisterThenUpdateTestL();
			break;
		case 3:
			((CTLinkedFontsUpdateStage1Step*)iStep)->SetTestStepID(_L("GRAPHICS-FNTSTORE-LINKEDFONTS-0201"));
			NegativeUpdateTestsL();
			break;
		case 4:
			((CTLinkedFontsUpdateStage1Step*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
			CloseTestsL();
			break;
		default:
			((CTLinkedFontsUpdateStage1Step*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
			((CTLinkedFontsUpdateStage1Step*)iStep)->CloseTMSGraphicsStep();
			TestComplete();
			break;
		}
	((CTLinkedFontsUpdateStage1Step*)iStep)->RecordTestResultL();
	}

/**
Connect to the Font Bitmap Server and construct a CFbsTypeface store to access the fontstore.
*/
void CTLinkedFontsUpdateStage1::SetupTestsL()
	{
	TInt ret = RFbsSession::Connect();
	TEST2(ret, KErrNone);
	
	iFbs = RFbsSession::GetSession();
	
	iTfStore = CFbsTypefaceStore::NewL(NULL);
	}

void CTLinkedFontsUpdateStage1::CloseTestsL()
	{
	delete iTfStore;
	RFbsSession::Disconnect();
	}
/**
@SYMTestCaseID
GRAPHICS-FNTSTORE-LINKEDFONTS-0200

@SYMPREQ
PREQ2147

@SYMREQ
REQ11859

@SYMTestCaseDesc
Creates three brand new linked typefaces, then updates each. The first typeface has an
element added to it, the second has one removed and the third is replaced with a different
specification.

The outcome of the update process is checked after a reboot by test case 202.

@SYMTestPriority
Critical

@SYMTestStatus
Implemented

@SYMTestActions
Checks the update process has completed succesfully.

@SYMTestExpectedResults
Linked fonts are created and updated successfully. Validation of the new file is performed after a reboot.
*/

void CTLinkedFontsUpdateStage1::RegisterThenUpdateTestL()
	{
	CLinkedTypefaceSpecification* addSpec = CreateAddTypefaceLC(EFalse);
	CLinkedTypefaceSpecification* removeSpec = CreateRemoveTypefaceLC(EFalse);
	CLinkedTypefaceSpecification* updateSpec = CreateUpdateTypefaceLC(EFalse);
	
	//Create three new linked fonts
	TRAPD(ret, addSpec->RegisterLinkedTypefaceL(*iTfStore));
	
	//Ensure that a supporting rasterizer is present.
	if (ret == KErrNotSupported)
		{
		WARN_PRINTF1(_L("There is not a font rasterizer present supporting font linking installed, skipping test."));
		CleanupStack::PopAndDestroy(3, addSpec);
		return;
		}
	else
		{
		iFontLinkingSupported = ETrue;
		}
	
	TEST(ret == KErrNone);
	TRAP(ret, removeSpec->RegisterLinkedTypefaceL(*iTfStore));
	TEST(ret == KErrNone);
	TRAP(ret, updateSpec->RegisterLinkedTypefaceL(*iTfStore));
	TEST(ret == KErrNone);
	
	CleanupStack::PopAndDestroy(3);
	
	//Update the three linked fonts with their new specifications
	addSpec = CreateAddTypefaceLC(ETrue);
	TRAP(ret, addSpec->UpdateLinkedTypefaceL(*iTfStore));
	TEST(ret == KErrNone);
	
	removeSpec = CreateRemoveTypefaceLC(ETrue);
	TRAP(ret, removeSpec->UpdateLinkedTypefaceL(*iTfStore));
	TEST(ret == KErrNone);
	
	updateSpec = CreateUpdateTypefaceLC(ETrue);
	TRAP(ret, updateSpec->UpdateLinkedTypefaceL(*iTfStore));
	TEST(ret == KErrNone);
	
	CleanupStack::PopAndDestroy(3, addSpec);
	}

/**
@SYMTestCaseID
GRAPHICS-FNTSTORE-LINKEDFONTS-0201

@SYMPREQ
PREQ2147

@SYMREQ
REQ11859

@SYMTestCaseDesc
Attempt to update a linked font with Invalid or Non-Existent Elements.
Attempt to update a non-linked font.

@SYMTestPriority
Critical

@SYMTestStatus
Implemented

@SYMTestActions
Ensures the correct error codes are returned showing that no updates were made to the relevant typefaces.

@SYMTestExpectedResults
All invalid updates will return KErrNotFound or KErrAccessDenied
*/
void CTLinkedFontsUpdateStage1::NegativeUpdateTestsL()
	{
	if (!iFontLinkingSupported)
		{
		WARN_PRINTF1(_L("There is not a font rasterizer present supporting font linking installed, skipping test."));
		return;
		}
	//Update with invalid element
	CLinkedTypefaceSpecification* invalidSpec =  CreateUpdateTypefaceLC(EFalse);
	
	CLinkedTypefaceElementSpec* invalidElement = CLinkedTypefaceElementSpec::NewLC(_L("SomeThingInValid"), 1);
	invalidSpec->AddTypefaceAtBackL(*invalidElement);
	CleanupStack::Pop(invalidElement);
	
	TRAPD(ret, invalidSpec->UpdateLinkedTypefaceL(*iTfStore));
	TEST(ret == KErrNotFound);
	
	CleanupStack::PopAndDestroy(invalidSpec);
	
	//Update a typeface that doesn't exist.
	_LIT(KNonExistantTypeface, "DoesNotExist");
	CLinkedTypefaceSpecification* nonExistentTypeface = CreateNegativeTestTypefaceLC(KNonExistantTypeface);
	TRAP(ret, nonExistentTypeface->UpdateLinkedTypefaceL(*iTfStore));
	TEST(ret == KErrNotFound);
	
	CleanupStack::PopAndDestroy(nonExistentTypeface);
	
	//Update the default techview font (that is not a linked font).
	_LIT(KNonLinkedFontName, "DejaVu Sans Condensed");
	CLinkedTypefaceSpecification* nonLinkedTypeface = CreateNegativeTestTypefaceLC(KNonLinkedFontName);
	TRAP(ret, nonLinkedTypeface->UpdateLinkedTypefaceL(*iTfStore));
	TEST(ret == KErrAccessDenied);
	
	CleanupStack::PopAndDestroy(nonLinkedTypeface);

	}

/**
Create a typeface to test adding a new element to a linked font.
 @param TBool EFalse for Original, ETrue for Update version
 */ 
CLinkedTypefaceSpecification* CTLinkedFontsUpdateStage1::CreateAddTypefaceLC(TBool aUpdate)
	{
	CLinkedTypefaceSpecification* spec = CLinkedTypefaceSpecification::NewLC(KAddElementFontName);
	
	AddGroups(spec);
	
	CLinkedTypefaceElementSpec* element = CLinkedTypefaceElementSpec::NewLC(KElement1Name, 1);
	element->SetCanonical(ETrue);
	spec->AddTypefaceAtBackL(*element);
	CleanupStack::Pop(element);
	
	element = CLinkedTypefaceElementSpec::NewLC(KElement2Name, 2);
	spec->AddTypefaceAtBackL(*element);
	CleanupStack::Pop(element);
	
	if (aUpdate)
		{
		element = CLinkedTypefaceElementSpec::NewLC(KElement3Name, 2);
		spec->AddTypefaceAtBackL(*element);
		CleanupStack::Pop(element);
		}
	
	return spec;
	}

/**
Create a typeface to test removing an element from a linked font.
 @param TBool EFalse for Original, ETrue for Update version
 */ 
CLinkedTypefaceSpecification* CTLinkedFontsUpdateStage1::CreateRemoveTypefaceLC(TBool aUpdate)
	{
	CLinkedTypefaceSpecification* spec = CLinkedTypefaceSpecification::NewLC(KRemoveElementFontName);
	AddGroups(spec);
	
	CLinkedTypefaceElementSpec* element = CLinkedTypefaceElementSpec::NewLC(KElement1Name, 1);
	element->SetCanonical(ETrue);
	spec->AddTypefaceAtBackL(*element);
	CleanupStack::Pop(element);
	
	element = CLinkedTypefaceElementSpec::NewLC(KElement2Name, 2);
	spec->AddTypefaceAtBackL(*element);
	CleanupStack::Pop(element);
	
	if (!aUpdate)
		{
		element = CLinkedTypefaceElementSpec::NewLC(KElement3Name, 2);
		spec->AddTypefaceAtBackL(*element);
		CleanupStack::Pop(element);
		}
	
	return spec;
	}

/**
Create a typeface to test completely changing a linked font.
 @param TBool EFalse for Original, ETrue for Update version
 */ 
CLinkedTypefaceSpecification* CTLinkedFontsUpdateStage1::CreateUpdateTypefaceLC(TBool aUpdate)
	{
	CLinkedTypefaceSpecification* spec = CLinkedTypefaceSpecification::NewLC(KUpdateFontName);
	AddGroups(spec);
	
	CLinkedTypefaceElementSpec* element;
	
	if (!aUpdate)
		{
		element = CLinkedTypefaceElementSpec::NewLC(KElement1Name, 1);
		element->SetCanonical(ETrue);
		spec->AddTypefaceAtBackL(*element);
		CleanupStack::Pop(element);
		}
	
	element = CLinkedTypefaceElementSpec::NewLC(KElement2Name, 2);
	spec->AddTypefaceAtBackL(*element);
	CleanupStack::Pop(element);
	
	if (aUpdate)
		{
		CLinkedTypefaceElementSpec* element = CLinkedTypefaceElementSpec::NewLC(KElement3Name, 1);
		element->SetCanonical(ETrue);
		spec->AddTypefaceAtBackL(*element);
		CleanupStack::Pop(element);
		}
	
	return spec;
	}

/**
Create a typeface for use in the negative test cases.

This has a number of typeface groups already added to it.

@see CTLinkedFontsUpdate1::AddGroups()
@param aName The name of the typeface.
*/
CLinkedTypefaceSpecification* CTLinkedFontsUpdateStage1::CreateNegativeTestTypefaceLC(const TDesC& aName)
	{
	CLinkedTypefaceSpecification* spec = CLinkedTypefaceSpecification::NewLC(aName);
	
	AddGroups(spec);
	
	CLinkedTypefaceElementSpec* element = CLinkedTypefaceElementSpec::NewLC(_L("DejaVu Sans Condensed"), 1);
	element->SetCanonical(ETrue);
	spec->AddTypefaceAtBackL(*element);
	CleanupStack::Pop(element);	
	
	return spec;
	}

/**
Adds two simple linked typeface groups to the linked typeface group.

@param aSpec The specification to have groups added to
*/
void CTLinkedFontsUpdateStage1::AddGroups(CLinkedTypefaceSpecification* aSpec)
	{
	CLinkedTypefaceGroup* grp = CLinkedTypefaceGroup::NewLC(1);
	grp->SetBaselineShift(CLinkedTypefaceGroup::EBaselineShift);
	aSpec->AddLinkedTypefaceGroupL(*grp);
	CleanupStack::Pop(grp);
	
	grp = CLinkedTypefaceGroup::NewLC(2);
	grp->SetBaselineShift(CLinkedTypefaceGroup::ENoBaselineShift);
	aSpec->AddLinkedTypefaceGroupL(*grp);
	CleanupStack::Pop(grp);
	}

__CONSTRUCT_STEP__(LinkedFontsUpdateStage1)


//CTLinkedFontsUpdate2
CTLinkedFontsUpdateStage2::CTLinkedFontsUpdateStage2(CTestStep* aStep)
:	CTGraphicsBase(aStep)
	{
	}

CTLinkedFontsUpdateStage2::~CTLinkedFontsUpdateStage2()
	{
	}

void CTLinkedFontsUpdateStage2::TestPanic(TInt aPanic)
	{
	_LIT(KFntTestStorePanicCategory,"TestFntStore");
	User::Panic(KFntTestStorePanicCategory,aPanic);
	}

void CTLinkedFontsUpdateStage2::RunTestCaseL(TInt aCurTestCase)
	{
	((CTLinkedFontsUpdateStage2Step*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);
	switch (aCurTestCase)
		{
		case 1:
			((CTLinkedFontsUpdateStage2Step*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
			SetupTestsL();
			break;
		case 2:
			((CTLinkedFontsUpdateStage1Step*)iStep)->SetTestStepID(_L("GRAPHICS-FNTSTORE-LINKEDFONTS-0202"));
			ValidateLinkedFontsL();
			break;
		case 3:
			((CTLinkedFontsUpdateStage2Step*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
			CloseTestsL();
		default:
			((CTLinkedFontsUpdateStage2Step*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
			((CTLinkedFontsUpdateStage2Step*)iStep)->CloseTMSGraphicsStep();
			TestComplete();
			break;
		}
		((CTLinkedFontsUpdateStage2Step*)iStep)->RecordTestResultL();
	}

/**
Connect to the Font Bitmap Server and construct a CFbsTypeface store to access the fontstore.
*/
void CTLinkedFontsUpdateStage2::SetupTestsL()
	{
	TInt ret = RFbsSession::Connect();
	TEST2(ret, KErrNone);
	
	iFbs = RFbsSession::GetSession();
	
	iTfStore = CFbsTypefaceStore::NewL(NULL);
	}

void CTLinkedFontsUpdateStage2::CloseTestsL()
	{
	delete iTfStore;
	RFbsSession::Disconnect();
	}
/**
@SYMTestCaseID
GRAPHICS-FNTSTORE-LINKEDFONTS-0202

@SYMPREQ
PREQ2147

@SYMREQ
REQ11859

@SYMTestCaseDesc
Tests the updates made to three linked typefaces created by test case 200; this checks the linked fonts
have been correctly updated to the latest specification. 

@SYMTestPriority
Critical

@SYMTestStatus
Implemented

@SYMTestActions
Checks the update process has completed succesfully.

@SYMTestExpectedResults
The linked fonts match the specification they were updated to. 
Failures may be due to other failures in test case xxx as this is the second stage of these tests.
*/
void CTLinkedFontsUpdateStage2::ValidateLinkedFontsL()
	{
	CLinkedTypefaceSpecification* addSpec = CLinkedTypefaceSpecification::NewLC(KAddElementFontName);
	TRAPD(ret, addSpec->FetchLinkedTypefaceSpecificationL(*iTfStore));
	TEST(ret == KErrNone);
	
	CLinkedTypefaceSpecification* removeSpec = CLinkedTypefaceSpecification::NewLC(KRemoveElementFontName);
	TRAP(ret, removeSpec->FetchLinkedTypefaceSpecificationL(*iTfStore));
	TEST(ret == KErrNone);
	
	CLinkedTypefaceSpecification* updateSpec = CLinkedTypefaceSpecification::NewLC(KUpdateFontName);
	TRAP(ret, updateSpec->FetchLinkedTypefaceSpecificationL(*iTfStore));
	TEST(ret == KErrNone);
	
	//Test Elements are in correct order
	TEST(addSpec->TypefaceCount() == 3);
	
	if (addSpec->Typeface(0))
		TEST(addSpec->Typeface(0)->ElementName() == KElement1Name);
	else
		TEST(EFalse);
	
	if (addSpec->Typeface(1))
		TEST(addSpec->Typeface(1)->ElementName() == KElement2Name);
	else
		TEST(EFalse);
	
	if (addSpec->Typeface(2))
		TEST(addSpec->Typeface(2)->ElementName() == KElement3Name);
	else
		TEST(EFalse);
	
	TEST(removeSpec->TypefaceCount() == 2);
	if (addSpec->Typeface(0))
		TEST(removeSpec->Typeface(0)->ElementName() == KElement1Name);
	else
		TEST(EFalse);
	
	if (addSpec->Typeface(1))
		TEST(removeSpec->Typeface(1)->ElementName() == KElement2Name);
	else
		TEST(EFalse);
	
	TEST(updateSpec->TypefaceCount() == 2);
	if (addSpec->Typeface(1))
		TEST(updateSpec->Typeface(0)->ElementName() == KElement2Name);
	else
		TEST(EFalse);
	
	if (addSpec->Typeface(1))
		TEST(updateSpec->Typeface(1)->ElementName() == KElement3Name);
	else
		TEST(EFalse);
	
	CleanupStack::PopAndDestroy(3, addSpec);
	}

__CONSTRUCT_STEP__(LinkedFontsUpdateStage2)
