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


/**
 @file
 @test
 @internalComponent
*/

#include "T_LinkedFonts2.h"
#include "linkedfontsprivate.h"
#include <graphics/openfontlinkedtypefacespecification.h>
#include <openfontlinkedtypefaceelementspec.h>

CTLinkedFonts2::CTLinkedFonts2(CTestStep* aStep)
:	CTGraphicsBase(aStep)
	{
	
	}

CTLinkedFonts2::~CTLinkedFonts2()
	{
	delete iGc;
	delete iDev;
	if (iFbs)
		iFbs->Disconnect();
	}

void CTLinkedFonts2::TestPanic(TInt aPanic)
	{
	_LIT(KFntTestStorePanicCategory,"TestFntStore");
	User::Panic(KFntTestStorePanicCategory,aPanic);
	}

void CTLinkedFonts2::RunTestCaseL(TInt aCurTestCase)
	{
	((CTLinkedFonts2Step*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);
	switch (aCurTestCase)
		{
		case 1:
			((CTLinkedFonts2Step*)iStep)->SetTestStepID(_L("GRAPHICS-FNTSTORE-LINKEDFONTS-0046"));
			InitializeTestHarnessL();
			break;
		case 2:
			((CTLinkedFonts2Step*)iStep)->SetTestStepID(_L("GRAPHICS-FNTSTORE-LINKEDFONTS-0001"));
			TestLinkedClientGettersAndSettersL();
			break;
		case 3:
			((CTLinkedFonts2Step*)iStep)->SetTestStepID(_L("GRAPHICS-FNTSTORE-LINKEDFONTS-0002"));
			TestLinkedClientAddAndRemoveL();
			break;
		case 4:
			((CTLinkedFonts2Step*)iStep)->SetTestStepID(_L("GRAPHICS-FNTSTORE-LINKEDFONTS-0003"));
			TestOpenClientAddAndRemoveL();
			break;
		case 5:
/**
@SYMTestCaseID GRAPHICS-FNTSTORE-LINKEDFONTS-0004
*/
			((CTLinkedFonts2Step*)iStep)->SetTestStepID(_L("GRAPHICS-FNTSTORE-LINKEDFONTS-0004"));
			TestLinkedRegistrationAndFetchL();
			break;
		case 6:
			((CTLinkedFonts2Step*)iStep)->SetTestStepID(_L("GRAPHICS-FNTSTORE-LINKEDFONTS-0100"));
			TestComplexLinkedFontsBasicL();
			break;
		case 7:
			((CTLinkedFonts2Step*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
			//((CTLinkedFonts2Step*)iStep)->SetTestStepID(_L("GRAPHICS-FNTSTORE-LINKEDFONTS-0101"));
			//TestBitmapFontsL();
			break;
		default:
			CloseTestHarnessL();
			TestComplete();
			((CTLinkedFonts2Step*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
			((CTLinkedFonts2Step*)iStep)->CloseTMSGraphicsStep();
			break;
		}
	((CTLinkedFonts2Step*)iStep)->RecordTestResultL();
	}

/**
	@SYMTestCaseID
	GRAPHICS-FNTSTORE-LINKEDFONTS-0046

	@SYMTestCaseDesc
	Starts a server session with the font and bitmap server.

	@SYMTestActions
	1. Creates a session with the Font and Bitmap server.
	2. Gets the current Font and Bitmap server session.
	3. Creates a CFbsScreenDevice object.
	4. Sets screen device to null.
	5. Sets auto update to true.
	6. Creates a font and bitmap server graphics context for the 
	   device and activates it.
	
	@SYMTestExpectedResults
	All steps 1-6 above succeed.  
*/
void CTLinkedFonts2::StartServer()
	{
	TInt ret = RFbsSession::Connect();
	TEST2(ret, KErrNone);
	iFbs = RFbsSession::GetSession();
	TEST(iFbs != NULL);
	
	if (iFbs==NULL)
		{
		//cannot continue
		TestComplete();
		return;
		}

	TInt err;
	TDisplayMode mode[4] = {EColor16MA,EColor16MU,EColor64K,EColor16MAP};

	TInt count;
	for (count=0; count < (sizeof(mode)/sizeof(mode[0])) ;count++)
		{
		TRAP(err, iDev = CFbsScreenDevice::NewL(KNullDesC, mode[count]));
		if (err!=KErrNotSupported)
			{
			break;
			}
		}

	if(err == KErrNone)
		{
		INFO_PRINTF2(_L("%d screen mode created"),mode[count]);
		iDev->ChangeScreenDevice(NULL);
		iDev->SetAutoUpdate(ETrue);
		iDev->CreateContext(iGc);
		
		//use and create a font
		//otherwise for memory tests there is a leak reported since the first AllocL
		//creates the space for the array (e.g. CTypefaceStore::AddFontL()) and 
		//although the array is emptied the array element storage is not deleted
		//until everything is closed
		_LIT(KSansSerif, "DejaVu Sans Condensed");
		TTypeface typeface;
		typeface.iName = KSansSerif;
		TFontSpec fs;
		fs.iTypeface = typeface;
		fs.iHeight = 20;
		CFbsFont* font = NULL;
		TInt err1 = iDev->GetNearestFontToDesignHeightInPixels(font, fs);
		TEST(err1==KErrNone);
		if (err1 == KErrNone)
			{
			TSize scrSize = iDev->SizeInPixels();
			iGc->UseFont(font);
			_LIT(KTestText,"TEST");
			iGc->DrawText(KTestText, TPoint((scrSize.iWidth / 2) + 50, 50));
			iGc->DiscardFont();
			iDev->ReleaseFont(font);
			}	
		}
	else
		{
		INFO_PRINTF2(_L("Could not create screen mode error %d"),err);
		}
	}


void CTLinkedFonts2::InitializeTestHarnessL()
	{
	INFO_PRINTF1(_L("Initialising the test harness T_LinkedFonts2 (PREQ2146)"));
	StartServer();
	}

void CTLinkedFonts2::CloseTestHarnessL()
	{
	}
/**
@SYMTestCaseID
GRAPHICS-FNTSTORE-LINKEDFONTS-0001

@SYMPREQ
PREQ2146

@SYMREQ
PREQ2146

@SYMTestCaseDesc
Create a linked typeface element & group. Set the parameters they provide.

@SYMTestPriority
Critical

@SYMTestStatus
Implemented

@SYMTestActions
Tests that the values set are stored correctly by the linked typeface element & group.

@SYMTestExpectedResults
Values are set correctly
*/
void CTLinkedFonts2::TestLinkedClientGettersAndSettersL()
	{
	INFO_PRINTF1(_L("CTLinkedFonts2::TestLinkedClientGettersAndSettersL"));
	CLinkedTypefaceElementSpec *elementSpec;
	_LIT(KLinkedTypefaceName, "TypefaceNumberOne");
	elementSpec = CLinkedTypefaceElementSpec::NewLC(KLinkedTypefaceName, 10);
	
	elementSpec->SetCanonical(ETrue);
	TEST(elementSpec->Canonical());
	
	//Test Remaining Getters & Setters
	elementSpec->SetCanonical(EFalse);

	CleanupStack::PopAndDestroy(elementSpec);
	
	CLinkedTypefaceGroup* grp = CLinkedTypefaceGroup::NewLC(10);
	const TInt KPercentageValue = 50;
	TInt ret;
	
	grp->SetBoldnessPercentage(KPercentageValue);
	ret = grp->BoldnessPercentage();
	TEST2(ret, KPercentageValue);
	
	grp->SetItalicAngle(KPercentageValue);
	ret = grp->ItalicAngle();
	TEST2(ret, KPercentageValue);
	
	grp->SetBaselineShift(CLinkedTypefaceGroup::ENoBaselineShift);
	CLinkedTypefaceGroup::TBaselineShift ret2 = grp->BaselineShift();
	TEST2(ret2, CLinkedTypefaceGroup::ENoBaselineShift);
	
	CleanupStack::PopAndDestroy(grp);
	}

/**
@SYMTestCaseID
GRAPHICS-FNTSTORE-LINKEDFONTS-0002

@SYMPREQ
PREQ2146

@SYMREQ
PREQ2146

@SYMTestCaseDesc
Tests construction of a Linked Typeface specification with both valid & invalid data.

@SYMTestPriority
Critical

@SYMTestStatus
Implemented

@SYMTestActions
Tests a number of different combinations (listed) of adding, removing and updating groups and
elements from a linked typeface specification.

Tests applicable to CLinkedTypefaceGroup / CLinkedTypefaceSpecification

1. Adding a valid group
2. Adding a duplicate group
3. Adding a group with the same Id as an existing one
4. Removal of group
12. Remove invalid GroupId
13. Adding more than KMaxLinkedTypefaceGroups groups
14. Retrieve Group with index out of bounds


Tests applicable to CLinkedTypefaceElementSpec / CLinkedTypefaceSpecification

5. Adding a valid element
6. Adding a duplicate element
7. Ensuring ordering of elements is as specified
8. Ensure canonical index is correct when element moved/removed
9. Ensuring correct error if extra canonical added
10. Ensuring correct error if canonical activated "in-place"
11. Removal of element
15. Create Specification & Element with too short (0)/long (KMaxTypefaceNameLength) names.
16. Adding more than KMaxLinkedTypefaces Elements
17. Retrieve Typeface with index out of bounds

@SYMTestExpectedResults
The canonical index is expected to be set to the values provided through the test after the 
various operations performed. Addition & removal should provide success or fail error codes
as indicated throughout the test. Actions are referenced within the code with expected results.
*/
void CTLinkedFonts2::TestLinkedClientAddAndRemoveL()
	{
	INFO_PRINTF1(_L("TestLinkedClientAddAndRemoveL"));
	
	CFbsTypefaceStore* store = CFbsTypefaceStore::NewL(NULL);
	CleanupStack::PushL(store);
	
	_LIT(KTestLinkedFont, "TestLinkedFont");
	CLinkedTypefaceSpecification* typefaceSpec = 
		CLinkedTypefaceSpecification::NewLC(KTestLinkedFont);
	
	TInt canIndex = typefaceSpec->CanonicalIndex();
	TEST(canIndex == KErrNotFound);
	TRAPD(ret, typefaceSpec->RegisterLinkedTypefaceL(*store));
	TEST(ret == KErrNotFound);

	
	const TInt KGroup1Id = 1;
	CLinkedTypefaceGroup* group1 = CLinkedTypefaceGroup::NewLC(KGroup1Id);
	//Action 1 : Valid group
	TRAP(ret, typefaceSpec->AddLinkedTypefaceGroupL(*group1));
	TEST(ret == KErrNone);
	CleanupStack::Pop(group1);
	
	//Action 2 : Same group again
	TRAP(ret, typefaceSpec->AddLinkedTypefaceGroupL(*group1));
	TEST(ret == KErrAlreadyExists);
	
	//Action 3 : Group with existing Id
	CLinkedTypefaceGroup* duplicateGroup = CLinkedTypefaceGroup::NewLC(KGroup1Id);
	TRAP(ret, typefaceSpec->AddLinkedTypefaceGroupL(*duplicateGroup));
	TEST(ret == KErrAlreadyExists);
	CleanupStack::PopAndDestroy(duplicateGroup);
	
	//Action 1 : Valid group
	const TInt KGroup2Id = 2;
	CLinkedTypefaceGroup* group2 = CLinkedTypefaceGroup::NewLC(KGroup2Id);
	group2->SetBaselineShift(CLinkedTypefaceGroup::EBaselineShift);
	group2->SetScalingOption(CLinkedTypefaceGroup::EScalingUpOrDown);
	TRAP(ret, typefaceSpec->AddLinkedTypefaceGroupL(*group2));
	TEST(ret == KErrNone);
	CleanupStack::Pop(group2);
	
	const TInt KGroup3Id = 4;
	CLinkedTypefaceGroup* group3 = CLinkedTypefaceGroup::NewLC(KGroup3Id);
	TRAP(ret, typefaceSpec->AddLinkedTypefaceGroupL(*group3));
	TEST(ret == KErrNone);
	CleanupStack::Pop(group3);
	
	//Action 5 : Japanese typefaces
	_LIT(KJapaneseElement, "Japanese");
	CLinkedTypefaceElementSpec* element1 = CLinkedTypefaceElementSpec::NewLC(KJapaneseElement, KGroup2Id);
	TRAP(ret, typefaceSpec->AddTypefaceAtBackL(*element1));
	CleanupStack::Pop(element1);
	
	//Action 5: Add the canonical element (Latin)
	_LIT(KLatinElement, "DejaVu Sans Mono Bold");
	CLinkedTypefaceElementSpec* element2 = CLinkedTypefaceElementSpec::NewLC(KLatinElement, KGroup1Id);
	element2->SetCanonical(ETrue);
	
	TRAP(ret, typefaceSpec->AddTypefaceAtIndexL(*element2,0));
	TEST(ret == KErrNone);
	CleanupStack::Pop(element2);
	
	//Action 8 : Check Canonical Index is correct
	canIndex = typefaceSpec->CanonicalIndex();
	TEST(canIndex == 0);
	
	//Action 5 : Hindi typefaces
	_LIT(KHindiElement, "Chandas");
	CLinkedTypefaceElementSpec* element3 = CLinkedTypefaceElementSpec::NewLC(KHindiElement, KGroup3Id);
	TRAP(ret, typefaceSpec->AddTypefaceAtBackL(*element3));
	TEST(ret == KErrNone);
	CleanupStack::Pop(element3);
	
	//Action 6 : Same element again
	TRAP(ret, typefaceSpec->AddTypefaceAtBackL(*element3));
	TEST(ret == KErrAlreadyExists);
	
	//Action 7 : Check ordering is correct
	TEST2(element1, typefaceSpec->Typeface(1));
	TEST2(element2, typefaceSpec->Typeface(0));
	TEST2(element3, typefaceSpec->Typeface(2));
	
	_LIT(KDummyTypeface, "DummyTypeface");
	CLinkedTypefaceElementSpec* dummyElement1 = CLinkedTypefaceElementSpec::NewLC(KDummyTypeface, KGroup2Id);
	
	//Action 9 : Add second canonical
	dummyElement1->SetCanonical(ETrue);
	TRAP(ret, typefaceSpec->AddTypefaceAtBackL(*dummyElement1));
	TEST(ret == KErrArgument);

	//Add at front of typeface to check canonical index is updated.
	dummyElement1->SetCanonical(EFalse);
	TRAP(ret, typefaceSpec->AddTypefaceAtIndexL(*dummyElement1, 0));
	TEST(ret == KErrNone);
	CleanupStack::Pop(dummyElement1);
	
	canIndex = typefaceSpec->CanonicalIndex();
	TEST(canIndex == 1);
	
	dummyElement1 = CLinkedTypefaceElementSpec::NewLC(KDummyTypeface, 180);
	TRAP(ret, typefaceSpec->AddTypefaceAtIndexL(*dummyElement1, 0));
	TEST(ret == KErrArgument);
	CleanupStack::PopAndDestroy(dummyElement1);
	
	//Action 11 : Remove Element
	typefaceSpec->RemoveTypeface(0);
	TEST2(element2, typefaceSpec->Typeface(0));
	
	//Action 8 : Check canonical index is correct
	canIndex = typefaceSpec->CanonicalIndex();
	TEST(canIndex == 0);
	
	//Action 10 : Check error if extra canonical activated "in place"
	typefaceSpec->Typeface(1)->SetCanonical(ETrue);
	canIndex = typefaceSpec->CanonicalIndex();
	TEST(canIndex == KErrOverflow);
	TRAP(ret, typefaceSpec->RegisterLinkedTypefaceL(*store));
	TEST(ret == KErrOverflow);
	//Reset back to be non-canonical
	typefaceSpec->Typeface(1)->SetCanonical(EFalse);
	
	//Action 8 : Check canonical index is correct
	typefaceSpec->RemoveTypeface(0);
	TEST2(element1, typefaceSpec->Typeface(0));
	
	//Action 8 : Check canonical index is correct
	canIndex = typefaceSpec->CanonicalIndex();
	TEST(canIndex == -1);

	//Action 4 : Remove the group
	TRAP(ret, typefaceSpec->RemoveLinkedTypefaceGroup(KGroup3Id));
	TEST(ret == KErrNone);
	CLinkedTypefaceGroup* groupRet = typefaceSpec->GroupById(KGroup3Id);
	TEST(groupRet == NULL);
	
	TEST2(element1, typefaceSpec->Typeface(0));

	//Action 12 : Remove invalid group
	ret = typefaceSpec->RemoveLinkedTypefaceGroup(180);
	TEST(ret == KErrNotFound);
	//Action 4 : Remove group
	ret = typefaceSpec->RemoveLinkedTypefaceGroup(KGroup1Id);
	TEST(ret == KErrNone);
	groupRet = typefaceSpec->GroupById(KGroup1Id);
	TEST(groupRet == NULL);
	
	CleanupStack::PopAndDestroy(typefaceSpec);
	
	//Negative Name length tests
	_LIT(KEmptyTypefaceName, "");
	//Action 15 : Create Specification & Element with 0 length names
	TRAP(ret, typefaceSpec = CLinkedTypefaceSpecification::NewLC(KEmptyTypefaceName));
	TEST(ret == KErrArgument);
	TRAP(ret, dummyElement1 = CLinkedTypefaceElementSpec::NewLC(KEmptyTypefaceName,1));
	TEST(ret < KErrNone);
	//Action 15: Create Specification & Element with name longer than KMaxTypefaceNameLength
	_LIT(KLongTypefaceName, "KErrArgumentKErrArgumentKErrArgument");
	TRAP(ret, typefaceSpec = CLinkedTypefaceSpecification::NewLC(KLongTypefaceName));
	TEST(ret == KErrArgument);
	TRAP(ret, dummyElement1 = CLinkedTypefaceElementSpec::NewLC(KLongTypefaceName,1));
	TEST(ret < KErrNone);
	
	typefaceSpec = CLinkedTypefaceSpecification::NewLC(KTestLinkedFont);
	TInt i;
	for (i = 0 ; i <= KMaxLinkedTypefaceGroups+1 ; i++)
		{
		CLinkedTypefaceGroup* group = CLinkedTypefaceGroup::NewLC(i);
		
		if (i < KMaxLinkedTypefaceGroups)
			{
			//Add Groups 1-KMaxLinkedTypefaceGroups
			TRAP(ret, typefaceSpec->AddLinkedTypefaceGroupL(*group));
			CleanupStack::Pop(group);
			}
		else
			{
			//Action 13 : Add group at KMaxLinkedTypefaceGroups+1
			TRAP(ret, typefaceSpec->AddLinkedTypefaceGroupL(*group));
			TEST(ret == KErrOverflow);
			CleanupStack::PopAndDestroy(group);
			}
		}

	_LIT(KFormatElementName, "Element%d");
	for (i = 0 ; i <= KMaxLinkedTypefaces ; i++)
		{
		TBuf<18> elementName;
		elementName.Format(KFormatElementName,i);
		CLinkedTypefaceElementSpec* element = CLinkedTypefaceElementSpec::NewLC(elementName,i);
		if (i < KMaxLinkedTypefaces)
			{
			//Add Elements 1-KMaxLinkedTypefaces
			TRAP(ret,typefaceSpec->AddTypefaceAtBackL(*element));
			CleanupStack::Pop(element);
			}
		else
			{
			//Action 16 : Add element KMaxLinkedTypefaces+1
			TRAP(ret,typefaceSpec->AddTypefaceAtBackL(*element));
			TEST(ret == KErrOverflow);
			CleanupStack::PopAndDestroy(element);
			}
		}
	
	//Action 14 : Retrieve out of bounds groups
	TEST2(typefaceSpec->Group(-1), NULL);
	TEST2(typefaceSpec->Group(typefaceSpec->GroupCount()+1), NULL);
	//Action 17 : Retrieve typeface with index out of bounds
	TEST2(typefaceSpec->Typeface(-1), NULL);
	TEST2(typefaceSpec->Typeface(typefaceSpec->TypefaceCount()+1), NULL);
	//Action 11 : Remove typeface with index out of bounds
	TEST2(typefaceSpec->RemoveTypeface(-1), KErrNotFound);
	TEST2(typefaceSpec->RemoveTypeface(typefaceSpec->TypefaceCount()+1), KErrNotFound);
	
	CleanupStack::PopAndDestroy(2, store);
	}

/**
@SYMTestCaseID
GRAPHICS-FNTSTORE-LINKEDFONTS-0003

@SYMPREQ
PREQ2146

@SYMREQ
PREQ2146

@SYMTestCaseDesc
Tests the Add & RemoveTypeface functions provided by COpenFontLinkedTypefaceSpecification

@SYMTestPriority
Critical

@SYMTestStatus
Implemented

@SYMTestActions
Tests applicable to CLinkedTypefaceGroup / COpenFontLinkedTypefaceSpecification

1. Adding a valid group
2. Adding a duplicate group (same object)
3. Adding a group with the same Id as an existing one
4. Removal of group
12. Remove invalid GroupId
13. Adding more than KMaxLinkedTypefaceGroups groups
14. Retrieve Group with index out of bounds

Tests applicable to COpenFontLinkedTypefaceElementSpec / COpenFontLinkedTypefaceSpecification

5. Adding a valid element
6. Adding a duplicate element (same object)
7. Ensuring ordering of elements is as specified
8. Ensure canonical index is correct when element moved/removed
9. Ensuring correct error if extra canonical added
10. Ensuring correct error if canonical activated "in-place"
11. Removal of element
15. Create Specification & Element with too short (0)/long (KMaxTypefaceNameLength) names.
16. Adding more than KMaxLinkedTypefaces Elements
17. Retrieve Typeface with index out of bounds

@SYMTestExpectedResults
The canonical index is expected to be set to the values provided through the test after the 
various operations performed. Addition & removal should provide success or fail error codes
as indicated throughout the test. Actions are referenced within the code with expected results.
*/
void CTLinkedFonts2::TestOpenClientAddAndRemoveL()
	{
	INFO_PRINTF1(_L("TestOpenClientAddAndRemoveL"));
	
	_LIT(KTestLinkedFont, "TestLinkedFont");
	COpenFontLinkedTypefaceSpecification* typefaceSpec = 
		COpenFontLinkedTypefaceSpecification::NewLC(KTestLinkedFont);
	
	TInt canIndex = typefaceSpec->CanonicalIndex();
	TEST(canIndex == -1);
	
	TInt ret;
	
	//Action 1 : Add a valid group
	const TInt KGroup1Id = 1;
	CLinkedTypefaceGroup* group1 = CLinkedTypefaceGroup::NewLC(KGroup1Id);
	TRAP(ret, typefaceSpec->AddLinkedTypefaceGroupL(*group1));
	TEST(ret == KErrNone);
	CleanupStack::Pop(group1);
	
	//Action 2 : Add a duplicate group
	TRAP(ret, typefaceSpec->AddLinkedTypefaceGroupL(*group1));
	TEST(ret == KErrAlreadyExists);
	
	//Action 3 : Add a group with the same id
	CLinkedTypefaceGroup* duplicateGroup = CLinkedTypefaceGroup::NewLC(KGroup1Id);
	TRAP(ret, typefaceSpec->AddLinkedTypefaceGroupL(*duplicateGroup));
	TEST(ret == KErrAlreadyExists);
	CleanupStack::PopAndDestroy(duplicateGroup);
	
	//Action 5 : Add a valid element
	const TInt KGroup2Id = 2;
	CLinkedTypefaceGroup* group2 = CLinkedTypefaceGroup::NewLC(KGroup2Id);
	group2->SetBaselineShift(CLinkedTypefaceGroup::EBaselineShift);
	group2->SetScalingOption(CLinkedTypefaceGroup::EScalingUpOrDown);
	TRAP(ret, typefaceSpec->AddLinkedTypefaceGroupL(*group2));
	TEST(ret == KErrNone);
	CleanupStack::Pop(group2);
	
	const TInt KGroup3Id = 4;
	CLinkedTypefaceGroup* group3 = CLinkedTypefaceGroup::NewLC(KGroup3Id);
	TRAP(ret, typefaceSpec->AddLinkedTypefaceGroupL(*group3));
	TEST(ret == KErrNone);
	CleanupStack::Pop(group3);
	
	//Action 5 : Add a valid element
	_LIT(KJapaneseElement, "Japanese");
	COpenFontLinkedTypefaceElementSpec* element1 = COpenFontLinkedTypefaceElementSpec::NewLC(KJapaneseElement, KGroup2Id);
	TRAP(ret, typefaceSpec->AddTypefaceAtBackL(*element1));
	CleanupStack::Pop(element1);
	
	//Action 5 : Add a valid element
	_LIT(KLatinElement, "DejaVu Sans Mono Bold");
	COpenFontLinkedTypefaceElementSpec* element2 = COpenFontLinkedTypefaceElementSpec::NewLC(KLatinElement, KGroup1Id);
	element2->SetCanonical(ETrue);
	
	TRAP(ret, typefaceSpec->AddTypefaceAtIndexL(*element2,0));
	TEST(ret == KErrNone);
	CleanupStack::Pop(element2);
	
	//Action 8 : Ensure canonical index correct
	canIndex = typefaceSpec->CanonicalIndex();
	TEST(canIndex == 0);
	
	_LIT(KHindiElement, "Chandas");
	COpenFontLinkedTypefaceElementSpec* element3 = COpenFontLinkedTypefaceElementSpec::NewLC(KHindiElement, KGroup3Id);
	TRAP(ret, typefaceSpec->AddTypefaceAtBackL(*element3));
	TEST(ret == KErrNone);
	CleanupStack::Pop(element3);
	
	//Action 6 : Duplicate element
	TRAP(ret, typefaceSpec->AddTypefaceAtBackL(*element3));
	TEST(ret == KErrAlreadyExists);
	
	//Action 7 : Ensure ordering correct
	TEST2(element1, typefaceSpec->Typeface(1));
	TEST2(element2, typefaceSpec->Typeface(0));
	TEST2(element3, typefaceSpec->Typeface(2));
	
	_LIT(KDummyTypeface, "DummyTypeface");
	COpenFontLinkedTypefaceElementSpec* dummyElement1 = COpenFontLinkedTypefaceElementSpec::NewLC(KDummyTypeface, KGroup2Id);
	
	//Action 9 : Add 2nd canonical
	dummyElement1->SetCanonical(ETrue);
	TRAP(ret, typefaceSpec->AddTypefaceAtBackL(*dummyElement1));
	TEST(ret == KErrArgument);
	
	//Add at front of typeface to check canonical index is updated.
	dummyElement1->SetCanonical(EFalse);
	TRAP(ret, typefaceSpec->AddTypefaceAtIndexL(*dummyElement1, 0));
	TEST(ret == KErrNone);
	CleanupStack::Pop(dummyElement1);
	
	//Action 8 : Check canonical updated
	canIndex = typefaceSpec->CanonicalIndex();
	TEST(canIndex == 1);
	
	//Action 11 : Remove element
	typefaceSpec->RemoveTypeface(0);
	TEST2(element2, typefaceSpec->Typeface(0));
	
	//Action 8 : Check canonical updated
	canIndex = typefaceSpec->CanonicalIndex();
	TEST(canIndex == 0);
	
	//Action 10 : Additional canonical set in-place
	typefaceSpec->Typeface(1)->SetCanonical(ETrue);
	canIndex = typefaceSpec->CanonicalIndex();
	TEST(canIndex == KErrOverflow);
	typefaceSpec->Typeface(1)->SetCanonical(EFalse);
	
	//Action 11 : Remove element
	typefaceSpec->RemoveTypeface(0);
	TEST2(element1, typefaceSpec->Typeface(0));
	
	//Action 8 : Canonical updated
	canIndex = typefaceSpec->CanonicalIndex();
	TEST(canIndex == -1);

	//Action 4 : Remove group
	TRAP(ret, typefaceSpec->RemoveLinkedTypefaceGroup(KGroup3Id));
	TEST(ret == KErrNone);
	CLinkedTypefaceGroup* groupRet = typefaceSpec->GroupById(KGroup3Id);
	TEST(groupRet == NULL);
	
	TEST2(element1, typefaceSpec->Typeface(0));
	
	//Action 12 : Remove invalid group Id
	ret = typefaceSpec->RemoveLinkedTypefaceGroup(180);
	TEST(ret == KErrNotFound);
	ret = typefaceSpec->RemoveLinkedTypefaceGroup(KGroup1Id);
	TEST(ret == KErrNone);
		
	CleanupStack::PopAndDestroy(typefaceSpec);	
	
	//Negative Name length tests
	_LIT(KEmptyTypefaceName, "");
	//Action 15 : Create Specifications 
	TRAP(ret, typefaceSpec = COpenFontLinkedTypefaceSpecification::NewLC(KEmptyTypefaceName));
	TEST(ret == KErrArgument);
	TRAP(ret, dummyElement1 = COpenFontLinkedTypefaceElementSpec::NewLC(KEmptyTypefaceName,1));
	TEST(ret < KErrNone);
	_LIT(KLongTypefaceName, "KErrArgumentKErrArgumentKErrArgument");
	TRAP(ret, typefaceSpec = COpenFontLinkedTypefaceSpecification::NewLC(KLongTypefaceName));
	TEST(ret == KErrArgument);
	TRAP(ret, dummyElement1 = COpenFontLinkedTypefaceElementSpec::NewLC(KLongTypefaceName,1));
	TEST(ret < KErrNone);
	
	typefaceSpec = COpenFontLinkedTypefaceSpecification::NewLC(KTestLinkedFont);
	TInt i;
	for (i = 0 ; i <= KMaxLinkedTypefaceGroups+1 ; i++)
		{
		CLinkedTypefaceGroup* group = CLinkedTypefaceGroup::NewLC(i);
		
		if (i < KMaxLinkedTypefaceGroups)
			{
			TRAP(ret, typefaceSpec->AddLinkedTypefaceGroupL(*group));
			CleanupStack::Pop(group);
			}
		else
			{
			TRAP(ret, typefaceSpec->AddLinkedTypefaceGroupL(*group));
			TEST(ret == KErrOverflow);
			CleanupStack::PopAndDestroy(group);
			}
		}

	_LIT(KFormatElementName, "Element%d");
	for (i = 0 ; i <= KMaxLinkedTypefaces ; i++)
		{
		TBuf<18> elementName;
		elementName.Format(KFormatElementName,i);
		COpenFontLinkedTypefaceElementSpec* element = COpenFontLinkedTypefaceElementSpec::NewLC(elementName,i);
		if (i < KMaxLinkedTypefaces)
			{
			TRAP(ret,typefaceSpec->AddTypefaceAtBackL(*element));
			CleanupStack::Pop(element);
			}
		else
			{
			TRAP(ret,typefaceSpec->AddTypefaceAtBackL(*element));
			TEST(ret == KErrOverflow);
			CleanupStack::PopAndDestroy(element);
			}
		}
	
	TEST2(typefaceSpec->Group(-1), NULL);
	TEST2(typefaceSpec->Group(typefaceSpec->GroupCount()+1), NULL);
	TEST2(typefaceSpec->Typeface(-1), NULL);
	TEST2(typefaceSpec->Typeface(typefaceSpec->TypefaceCount()+1), NULL);
	TEST2(typefaceSpec->RemoveTypeface(-1), KErrNotFound);
	TEST2(typefaceSpec->RemoveTypeface(typefaceSpec->TypefaceCount()+1), KErrNotFound);
	CleanupStack::PopAndDestroy(typefaceSpec);
	}

/* Function is used to determine if complex fonts are available.  It looks for a 
test Devanagari font
*/
TBool CTLinkedFonts2::ComplexFontAvailable()
	{
	_LIT(KTypefaceOT, "Chandas");

	// Create the linked font
	TTypeface typeface;
	typeface.iName = KTypefaceOT;
	TFontSpec fs1;
	fs1.iTypeface = typeface;
	fs1.iHeight = 4;
	CFbsFont* font = NULL;
	TInt err = iDev->GetNearestFontToDesignHeightInPixels(font, fs1);
	if (font)
		{
		TFontSpec fs2 = font->FontSpecInTwips();
		iDev->ReleaseFont(font);
		if (fs2.iTypeface.iName.Compare(KTypefaceOT) == 0)
			{
			return ETrue;
			}
		}
	INFO_PRINTF1(_L("(N.B. Complex font not available so complex parts of test skipped)"));
	return EFalse;
	}

/**
   @SYMTestCaseID			GRAPHICS-FNTSTORE-LINKEDFONTS-0100

   @SYMPREQ					PREQ2146

   @SYMREQ					REQ10924, REQ10925, REQ10926

   @SYMTestCaseDesc			To ensure that a linked typeface including a complex font can be created
							and used.  Test access to characters in a complex linked font.

   @SYMTestPriority			Critical

   @SYMTestStatus			Implemented

   @SYMTestActions			(1) Create a linked font with a complex typeface. Draw text, 
   								and compare	against text drawn using a non-linked font.

   @SYMTestExpectedResults	(1) - test allows a linked font with a complex typeface to be created
*/
void CTLinkedFonts2::TestComplexLinkedFontsBasicL()
	{
	INFO_PRINTF1(_L("LinkedFonts2::TestComplexLinkedFontsBasicL"));

	CFbsTypefaceStore* store = CFbsTypefaceStore::NewL(iDev);
	CleanupStack::PushL(store);

	TBool testComplex = ComplexFontAvailable();
	if (testComplex)
		{
		// Create typeface to be linked
		_LIT(KLinkedTypefaceTT, "DejaVu Sans Condensed");
		_LIT(KLinkedTypefaceOT, "Chandas");

		CLinkedTypefaceSpecification *typefaceSpec;
		_LIT(KLinkedTypefaceTTOT, "LinkedTypefaceComp");
		
		typefaceSpec = CLinkedTypefaceSpecification::NewLC(KLinkedTypefaceTTOT);
		
		CLinkedTypefaceGroup* group1 = CLinkedTypefaceGroup::NewLC(1);
		typefaceSpec->AddLinkedTypefaceGroupL(*group1);
		CleanupStack::Pop();
		
		CLinkedTypefaceGroup* group2 = CLinkedTypefaceGroup::NewLC(2);
		typefaceSpec->AddLinkedTypefaceGroupL(*group2);
		CleanupStack::Pop();
		
		// Add first typeface
		CLinkedTypefaceElementSpec *elementSpec1;
		elementSpec1 = CLinkedTypefaceElementSpec::NewLC(KLinkedTypefaceTT, 1);
		
		CLinkedTypefaceElementSpec *elementSpec2;
		elementSpec2 = CLinkedTypefaceElementSpec::NewLC(KLinkedTypefaceOT, 2);
		elementSpec2->SetCanonical(ETrue);
		
		typefaceSpec->AddTypefaceAtBackL(*elementSpec1);
		typefaceSpec->AddTypefaceAtBackL(*elementSpec2);
		CleanupStack::Pop(2, elementSpec1);
		
		TEST(typefaceSpec->TypefaceCount() == 2);
		TEST(typefaceSpec->Name() == KLinkedTypefaceTTOT);

		// Now try registering the linked typeface
		TRAPD(ret, typefaceSpec->RegisterLinkedTypefaceL(*store));

		if (ret == KErrNotSupported)
			{
			INFO_PRINTF1(_L("No Rasterizer is present that supports the extended Font Linking interface. This test case has been skipped."));
			CleanupStack::PopAndDestroy(2,store);
			}
		else
			{
			TEST(ret == KErrNone);
			
			// Create the linked font
			TTypeface typeface;
			typeface.iName = KLinkedTypefaceTTOT;
			TFontSpec fs;
			fs.iTypeface = typeface;
			fs.iHeight = 35;
			CFbsFont* font = NULL;
			TInt err = iDev->GetNearestFontToDesignHeightInPixels(font, fs);
			
			//check the font is the one requested
			TFontSpec fontSpec = font->FontSpecInTwips();
			TEST(fontSpec.iTypeface.iName == KLinkedTypefaceTTOT);
	
			_LIT16(KEnglish1,"ABCD12345 ");
			_LIT16(KHindi1,"\x0915\x094D\x0937\x0924\x094D\x0930\x093F\x092F\x0020");
			TEST(font != NULL);
			const TPoint englishLinked(10,75);
			const TPoint hindiLinked(10,150);
			if (font)
				{
				// Use the font
				iGc->UseFont(font);
				iGc->Clear();
				iGc->DrawText(KEnglish1, englishLinked);
				iGc->DrawText(KHindi1, hindiLinked);
				iGc->DiscardFont();
				iDev->Update();
				User::After(1000*1000*5);
				}
	
			// All the CFont functions can use a linked font and a non-linked
			// font - with the same base font, results should be the same.
			// Compare with the test function.
			
			//output the Hindi text
			TTypeface typeface2;
			typeface2.iName = KLinkedTypefaceOT;
			TFontSpec fs2;
			fs2.iTypeface = typeface2;
			fs2.iHeight = 35;
			CFbsFont* font2 = NULL;
			err = iDev->GetNearestFontToDesignHeightInPixels(font2, fs2);
			TEST(font2 != NULL);
			
			//check the font is the one requested
			fontSpec = font2->FontSpecInTwips();
			TEST(fontSpec.iTypeface.iName == KLinkedTypefaceOT);
	
			const TPoint englishHindiFont(240, 75);
			const TPoint hindiHindiFont(240, 150);
			if (font2)
				{
				// Use the font
				iGc->UseFont(font2);
				iGc->DrawText(KEnglish1, englishHindiFont);
				iGc->DrawText(KHindi1, hindiHindiFont);
				iGc->DiscardFont();
				iDev->Update();
				User::After(1000*1000);
				iDev->ReleaseFont(font2);
				}
			
			//output the English text
			TTypeface typeface3;
			typeface3.iName = KLinkedTypefaceTT;
			TFontSpec fs3;
			fs3.iTypeface = typeface3;
			fs3.iHeight = 35;
			CFbsFont* font3 = NULL;
			err = iDev->GetNearestFontToDesignHeightInPixels(font3, fs3);
			TEST(font3 != NULL);
	
			//check the font is the one requested
			fontSpec = font3->FontSpecInTwips();
			TEST(fontSpec.iTypeface.iName == KLinkedTypefaceTT);
	
			const TPoint englishSansSerifFont(440, 75);
			const TPoint hindiSansSerifFont(440, 150);
			if (font3)
				{
				// Use the font
				iGc->UseFont(font3);
				iGc->DrawText(KEnglish1, englishSansSerifFont);
				iGc->DrawText(KHindi1, hindiSansSerifFont);
				iGc->DiscardFont();
				iDev->Update();
				User::After(1000*1000);
				iDev->ReleaseFont(font3);
				}
			
			//compare rects.
			if (font)
				{
				CFont::TMeasureTextOutput compareEnglish;
				CFont::TMeasureTextOutput compareHindi;
				//get the size of a text box to do the comparison
				font->MeasureText(KEnglish1, NULL, &compareEnglish);
				font->MeasureText(KHindi1, NULL, &compareHindi);
				
				//compare English
				TRect englishLinkedRect (englishLinked.iX,englishLinked.iY,
							englishLinked.iX+compareEnglish.iBounds.Width(),
							englishLinked.iY+compareEnglish.iBounds.Height());
				TRect englishSansSerifRect  (englishSansSerifFont.iX,englishSansSerifFont.iY,
						englishSansSerifFont.iX+compareEnglish.iBounds.Width(),
						englishSansSerifFont.iY+compareEnglish.iBounds.Height());
				
				TBool val = iDev->RectCompare(englishLinkedRect, *iDev,englishSansSerifRect);
				//this value is not compared since the rasterizer may resize/reposition fonts
				
				//compare Hindi
				TRect hindiLinkedRect  (hindiLinked.iX,hindiLinked.iY,
						hindiLinked.iX+compareHindi.iBounds.Width(),
						hindiLinked.iY+compareHindi.iBounds.Height());
				TRect hindiHindiRect  (hindiHindiFont.iX,hindiHindiFont.iY,
						hindiHindiFont.iX+compareHindi.iBounds.Width(),
						hindiHindiFont.iY+compareHindi.iBounds.Height());
				val=iDev->RectCompare(hindiLinkedRect, *iDev,hindiHindiRect);
				//do not call TEST(val) since size/position may be slightly different
				iDev->ReleaseFont(font);
				}
			CleanupStack::PopAndDestroy(2, store);
			}
		}
	}

/**
   @SYMTestCaseID			GRAPHICS-FNTSTORE-LINKEDFONTS-0101

   @SYMPREQ					PREQ2146

   @SYMREQ					REQ10924, REQ10925, REQ10926

   @SYMTestCaseDesc			To test PREQ2146 linked fonts with bitmap fonts.  Bitmap fonts are not  
   							supported so an error needs to be returned on registration.

   @SYMTestPriority			Critical

   @SYMTestStatus			Implemented

   @SYMTestActions			(1) Create a linked typeface elements, with one of them from a bitmap font
   								try registering, and KErrNotSupported error should be returned

   @SYMTestExpectedResults	(1) - registering a bitmap font returns KErrNotSupported.
*/
void CTLinkedFonts2::TestBitmapFontsL()
	{
	INFO_PRINTF1(_L("LinkedFonts2::TestBitmapFontsL"));

	CFbsTypefaceStore* store = CFbsTypefaceStore::NewL(iDev);
	CleanupStack::PushL(store);
	
	// Create typeface to be linked
	_LIT(KLinkedTypefaceTT, "DejaVu Sans Condensed");
	_LIT(KLinkedTypeFace1, "LinkedFont1"); //this is a bitmap font

	CLinkedTypefaceSpecification *typefaceSpec;
	_LIT(KLinkedTypefaceBitmap, "LinkedTypefaceBitmap");
		
	typefaceSpec = CLinkedTypefaceSpecification::NewLC(KLinkedTypefaceBitmap);
	
	CLinkedTypefaceGroup* group1 = CLinkedTypefaceGroup::NewLC(1);
	typefaceSpec->AddLinkedTypefaceGroupL(*group1);
	CleanupStack::Pop();
	
	CLinkedTypefaceGroup* group2 = CLinkedTypefaceGroup::NewLC(2);
	typefaceSpec->AddLinkedTypefaceGroupL(*group2);
	CleanupStack::Pop();
	
	// Add first typeface
	CLinkedTypefaceElementSpec *elementSpec1;
	elementSpec1 = CLinkedTypefaceElementSpec::NewLC(KLinkedTypefaceTT, 2);
		
	CLinkedTypefaceElementSpec *elementSpec2;
	elementSpec2 = CLinkedTypefaceElementSpec::NewLC(KLinkedTypeFace1, 1);
	elementSpec2->SetCanonical(ETrue);
		
	typefaceSpec->AddTypefaceAtBackL(*elementSpec1);
	typefaceSpec->AddTypefaceAtBackL(*elementSpec2);
	CleanupStack::Pop(2, elementSpec1);
		
	TEST(typefaceSpec->TypefaceCount() == 2);
	TEST(typefaceSpec->Name() == KLinkedTypefaceBitmap);

	// Now try regstering the linked typeface
	TRAPD(ret, typefaceSpec->RegisterLinkedTypefaceL(*store));
	TEST(ret == KErrNotSupported);
	CleanupStack::PopAndDestroy(2, store);
	}

/**
   @SYMTestCaseID			GRAPHICS-FNTSTORE-LINKEDFONTS-0109

   @SYMPREQ					PREQ2146

   @SYMREQ					REQ10924, REQ10925, REQ10926

   @SYMTestCaseDesc			To test PREQ2146 linked fonts updates to the TOpenFontCharMetrics class.  
   
   @SYMTestPriority			Critical

   @SYMTestStatus			Implemented

   @SYMTestActions			(1) Create a TOpenFontCharMetrics object.
   							(2) Check that nothing is set
   							(3) Set a small positive baseline
   							(4) Check that the baseline is as set and the glyph type is still not set
   							(5) Set a large positive baseline
   							(6)	Check that the baseline is as set and the glyph type is still not set
							(7) Set a large negative baseline
							(8) Check that the baseline is as set and the glyph type is still not set
							(9) Set the glyph type.
							(10) Check that the baseline is as set and the glyph type is as set
							(11) Set the baseline to a again to various values
							(12) Check that the baseline is as set and the glyph type has not changed

   	@SYMTestExpectedResults	(2) the default values are 0 for baseline and EGlyphBitmapTypeNotDefined for glyph type
   							(4)(6)(8)(10)(12) The values are are set, and setting the baseline does 
   							not alter the glyph type and vice-versa.
*/
void CTLinkedFonts2::TestTOpenFontMetrics()
	{
	TOpenFontMetrics metrics;
	
	TInt len = metrics.BaselineCorrection();
	TEST(len==0);
	
	metrics.SetBaselineCorrection(22);
	len = metrics.BaselineCorrection();
	TEST(len==22);
	
	metrics.SetBaselineCorrection(444);
	len = metrics.BaselineCorrection();
	TEST(len==444);
	
	metrics.SetBaselineCorrection(-333);
	len = metrics.BaselineCorrection();
	TEST(len==-333);
	
	metrics.SetBaselineCorrection(444);
	len = metrics.BaselineCorrection();
	TEST(len==444);
	
	metrics.SetBaselineCorrection(-1);
	len = metrics.BaselineCorrection();
	TEST(len==-1);

	}

void CTLinkedFonts2::TestLinkedRegistrationAndFetchL()
	{
	INFO_PRINTF1(_L("CTLinkedFonts2::TestLinkedRegistrationAndFetchL"));
	CFbsTypefaceStore* store = CFbsTypefaceStore::NewL(iDev);
	CleanupStack::PushL(store);
	
	//Create and register a dummy linked typeface to register.
	_LIT(KFetchTypefaceName, "FetchFont");
	CLinkedTypefaceSpecification* typefaceSpec = CLinkedTypefaceSpecification::NewLC(KFetchTypefaceName);

	CLinkedTypefaceGroup* group1 = CLinkedTypefaceGroup::NewLC(1);
	group1->SetBoldnessPercentage(20);
	group1->SetItalicAngle(10);
	group1->SetBaselineShift(CLinkedTypefaceGroup::EBaselineShift);
	group1->SetScalingOption(CLinkedTypefaceGroup::EScalingDown);
	
	typefaceSpec->AddLinkedTypefaceGroupL(*group1);
	CleanupStack::Pop(group1);
	
	CLinkedTypefaceGroup* group2 = CLinkedTypefaceGroup::NewLC(2);
	group2->SetBoldnessPercentage(21);
	group2->SetItalicAngle(11);
	group2->SetBaselineShift(CLinkedTypefaceGroup::ENoBaselineShift);
	group2->SetScalingOption(CLinkedTypefaceGroup::EScalingUp);
	
	typefaceSpec->AddLinkedTypefaceGroupL(*group2);
	CleanupStack::Pop(group2);
	
	_LIT(KElement1Name, "DejaVu Sans Condensed");
	CLinkedTypefaceElementSpec* element1 = CLinkedTypefaceElementSpec::NewLC(KElement1Name, 1);
	element1->SetCanonical(ETrue);
	typefaceSpec->AddTypefaceAtBackL(*element1);
	CleanupStack::Pop(element1);
	
	_LIT(KElement2Name, "Chandas");
	CLinkedTypefaceElementSpec* element2 = CLinkedTypefaceElementSpec::NewLC(KElement2Name, 2);
	typefaceSpec->AddTypefaceAtBackL(*element2);
	CleanupStack::Pop(element2);
	
	TRAPD(ret, typefaceSpec->RegisterLinkedTypefaceL(*store));
	if (ret == KErrNotSupported)
		{
		INFO_PRINTF1(_L("No Rasterizer is present that supports the extended Font Linking interface. This test case has been skipped."));
		CleanupStack::PopAndDestroy(2,store);
		}
	else
		{
		TEST(ret == KErrNone);
		
		CLinkedTypefaceSpecification* fetchSpec = CLinkedTypefaceSpecification::NewLC(KFetchTypefaceName);
		TRAP(ret, fetchSpec->FetchLinkedTypefaceSpecificationL(*store));
		TEST(ret == KErrNone);
		
		//Compare Groups. Ordering isn't important so can get group by id rather than index.
		TEST(typefaceSpec->GroupCount() == fetchSpec->GroupCount());
		CLinkedTypefaceGroup* groupf = fetchSpec->GroupById(1);
		TEST(CompareGroups(group1,groupf));
		
		groupf = fetchSpec->GroupById(2);
		TEST(CompareGroups(group2,groupf));
			
		//Check elements match 
		TEST(typefaceSpec->TypefaceCount() == fetchSpec->TypefaceCount());
		
		CLinkedTypefaceElementSpec* elementf = fetchSpec->Typeface(0);
		TEST(CompareElementSpecs(elementf, element1));
		elementf = fetchSpec->Typeface(1);
		TEST(CompareElementSpecs(elementf, element2));
			
		CleanupStack::PopAndDestroy(3,store);
		}
	}

/**
   @SYMTestCaseID			GRAPHICS-FNTSTORE-LINKEDFONTS-0110

   @SYMDEF					DEF137360

   @SYMTestCaseDesc			Ensure that font full names are used to build a linked font  
   
   @SYMTestPriority			Critical

   @SYMTestStatus			Implemented

   @SYMTestActions			Attempt to register a linked font using a font full name rather than a family
						    name. DejaVu Sans Condensed is both a family name and a full name. DejaVu Sans Mono Bold is full name only.

   	@SYMTestExpectedResults	Registration of the linked font will be sucessful.
							Failing with KErrNotFound means either the test font is not available or there is a regression.
*/
void CTLinkedFonts2::TestFontNameUsage()
	{
	CFbsTypefaceStore* store = CFbsTypefaceStore::NewL(NULL);
	CleanupStack::PushL(store);
	
	CLinkedTypefaceSpecification* nameSpec = CLinkedTypefaceSpecification::NewLC(_L("FullNameTest"));
	
	CLinkedTypefaceGroup* group = CLinkedTypefaceGroup::NewLC(1);
	nameSpec->AddLinkedTypefaceGroupL(*group);
	CleanupStack::Pop(group);
	
	//Use a font whose name doesn't match a family name
	CLinkedTypefaceElementSpec* element1 = CLinkedTypefaceElementSpec::NewLC(_L("DejaVu Sans Mono Bold"), 1);
	element1->SetCanonical(ETrue);
	nameSpec->AddTypefaceAtBackL(*element1);
	CleanupStack::Pop(element1);
	
	CLinkedTypefaceElementSpec* element2 = CLinkedTypefaceElementSpec::NewLC(_L("Chandas"), 1);
	nameSpec->AddTypefaceAtBackL(*element2);
	CleanupStack::Pop(element2);
	
	TRAPD(ret, store->RegisterLinkedTypeface(*nameSpec));
	
	TEST(ret == KErrNone);
	
	CleanupStack::PopAndDestroy(2, store);
	}

TBool CTLinkedFonts2::CompareElementSpecs(CLinkedTypefaceElementSpec* aSpec1, CLinkedTypefaceElementSpec* aSpec2)
	{
	if ( (aSpec1 == NULL ) && (aSpec2 == NULL) )
		return ETrue;
	if ( (aSpec1 == NULL) || (aSpec2 == NULL) )
		return EFalse;
	if (aSpec1->ElementName().Compare(aSpec2->ElementName()) != 0)
		return EFalse;
	if (aSpec1->Canonical() != aSpec2->Canonical())
		return EFalse;
	if (aSpec1->GroupId() != aSpec2->GroupId())
		return EFalse;
	
	return ETrue;
	}

TBool CTLinkedFonts2::CompareElementSpecs(COpenFontLinkedTypefaceElementSpec* aSpec1, COpenFontLinkedTypefaceElementSpec* aSpec2)
	{
	if ( (aSpec1 == NULL ) && (aSpec2 == NULL) )
		return ETrue;
	if ( (aSpec1 == NULL) || (aSpec2 == NULL) )
		return EFalse;
	if (aSpec1->ElementName().Compare(aSpec2->ElementName()) != 0)
		return EFalse;
	if (aSpec1->Canonical() != aSpec2->Canonical())
		return EFalse;
	if (aSpec1->GroupId() != aSpec2->GroupId())
		return EFalse;
	
	return ETrue;
	}
/**
Compare two linked typeface groups; the first varable should be a locally created group and the second one fetched from the
rasterizer. If the local group has thresholds / metrics set to -1 ("best guess by rasterizer") then the differences between the
values are ignored.
 */
TBool CTLinkedFonts2::CompareGroups(CLinkedTypefaceGroup* aLocalGroup, CLinkedTypefaceGroup* aFetchedGroup)
	{
	if ( (aLocalGroup == NULL) && (aFetchedGroup == NULL) )
		return ETrue;
	if ( (aLocalGroup == NULL) || (aFetchedGroup == NULL) )
		{
		INFO_PRINTF1(_L("One of the specified groups is NULL"));
		return EFalse;
		}
	
	if (aLocalGroup->BaselineShift() != aFetchedGroup->BaselineShift())
		return EFalse;
	
	/*if (aLocalGroup->BoldnessPercentage() != -1)
		{
		if (aLocalGroup->BoldnessPercentage() != aFetchedGroup->BoldnessPercentage())
			{
			INFO_PRINTF1(_L("Fail below caused by non-matching boldness percentages"));
			return EFalse;
			}
		}
	
	if (aLocalGroup->ItalicAngle() != -1)
		{
		if (aLocalGroup->ItalicAngle() != aFetchedGroup->ItalicAngle())
			{
			INFO_PRINTF1(_L("Fail below caused by non-matching italic angles"));
			return EFalse;
			}
		}
	*/
	if (aLocalGroup->ScalingOption() != aFetchedGroup->ScalingOption())
		return EFalse;
	if (aLocalGroup->GroupId() != aFetchedGroup->GroupId())
		return EFalse;
	
	return ETrue;
	}

__CONSTRUCT_STEP__(LinkedFonts2)
