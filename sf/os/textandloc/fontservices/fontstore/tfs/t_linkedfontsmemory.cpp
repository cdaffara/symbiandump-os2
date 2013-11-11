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


#include "FNTSTD.H"
#include "t_linkedfontsmemory.h"
#include "T_LinkedFonts2.h"
#include "fbsmessage.h"
#include <hal.h>
#include <linkedfonts.h>

class CTLinkedFontsMemory : public CTLinkedFonts2
	{
public:
	CTLinkedFontsMemory(CTestStep* aStep);
	~CTLinkedFontsMemory();
	//base class static void TestPanic(TInt aPanic);
protected:
// From CTGraphicsStep
	virtual void RunTestCaseL(TInt aCurTestCase);
private:
	
	TInt TestLinkedFontsMemory1L(TInt aTypefaces);
	void TestLinkedFontsMemory1L();
	TInt TestLinkedFontsCreateLinkedTypefacesL(const TDesC& aName,TInt aTypefaces);
	TInt TestLinkedFontsMemory_Part2(const TDesC& aName);
	void TestLinkedFontsMemory2();
	void TestLinkedFontsMemory3();
	void TestLinkedFontsMemory4();
	void TestMemoryUsage();
	
	void SetHeapFail(RFbsSession::THeapFailType aType, TInt aFailures);
	TInt HeapCount(RFbsSession::THeapFailType aType);
	void SetHeapReset(RFbsSession::THeapFailType aType);
	void SetHeapCheck(RFbsSession::THeapFailType aType);
	void TestCompareText();
	TInt FontAndBitmapServerSharedHeap();
	void DisplayRamInfo(const TDesC &aTextInfo) const;
	TBool CheckRasterizerInstalledL();
	};

CTLinkedFontsMemory::CTLinkedFontsMemory(CTestStep* aStep)
	:	CTLinkedFonts2(aStep)
	{
	}	

CTLinkedFontsMemory::~CTLinkedFontsMemory()
	{
	}

void CTLinkedFontsMemory::SetHeapFail(RFbsSession::THeapFailType aType, TInt aFailures)
	{
	__ASSERT_ALWAYS(iFbs, TestPanic(KErrGeneral));
	iFbs->SendCommand(EFbsMessSetHeapFail, aType, aFailures);
	}

TInt CTLinkedFontsMemory::HeapCount(RFbsSession::THeapFailType aType)
	{
	__ASSERT_ALWAYS(iFbs, TestPanic(KErrGeneral));
	return iFbs->SendCommand(EFbsMessHeapCount, aType);
	}

void CTLinkedFontsMemory::SetHeapReset(RFbsSession::THeapFailType aType)
	{
	__ASSERT_ALWAYS(iFbs, TestPanic(KErrGeneral));
	iFbs->SendCommand(EFbsMessSetHeapReset, aType);
	}

void CTLinkedFontsMemory::SetHeapCheck(RFbsSession::THeapFailType aType)
	{
	__ASSERT_ALWAYS(iFbs, TestPanic(KErrGeneral));
	iFbs->SendCommand(EFbsMessSetHeapCheck, aType);
	}

TInt CTLinkedFontsMemory::FontAndBitmapServerSharedHeap()
	{
	__ASSERT_ALWAYS(iFbs, TestPanic(KErrGeneral));
	return iFbs->SendCommand(EFbsMessHeap);
	}

_LIT(KLinkedFontsMemoryTestFont,"MemTestLinkedFnt");
TBool CTLinkedFontsMemory::CheckRasterizerInstalledL()
	{
	CFbsTypefaceStore* store = CFbsTypefaceStore::NewL(iDev);
	CleanupStack::PushL(store);
	
	// Create typeface to be linked
	CLinkedTypefaceSpecification *typefaceSpec;
	typefaceSpec = CLinkedTypefaceSpecification::NewLC(KLinkedFontsMemoryTestFont);
	
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
			return ETrue;
		default:
			INFO_PRINTF1(_L("Linked fonts memory: no linked fonts rasterizer found"));
			return EFalse;
		}
	}

_LIT(KDrawMe, "lInKED FOnTS");

void CTLinkedFontsMemory::TestCompareText()
	{
	_LIT(KWithout, "WithoutLinkedFontTT");
	TTypeface typeface;
	typeface.iName = KWithout;
	TFontSpec fs;
	fs.iTypeface = typeface;
	fs.iHeight = 20;
	CFbsFont* font = NULL;
	TInt err1 = iDev->GetNearestFontToDesignHeightInPixels(font, fs);

	if (font && err1 == KErrNone)
		{
		// Use the font
		TSize scrSize = iDev->SizeInPixels();
		iGc->UseFont(font);
		iGc->DrawText(KDrawMe, TPoint((scrSize.iWidth / 2) + 50, 50));
		iGc->DiscardFont();
		iDev->ReleaseFont(font);
		TPoint left(0, 0);
		TPoint right(scrSize.iWidth / 2, 0);
		TBool val = iDev->RectCompare(TRect(left, TPoint(scrSize.iWidth / 2, scrSize.iHeight)), *iDev,TRect(right, TPoint(scrSize.iWidth, scrSize.iHeight)));
		if (!val)
			{
			iGc->DrawRect(TRect(left, TPoint(scrSize.iWidth / 2, scrSize.iHeight)));
			iGc->DrawRect(TRect(right, TPoint(scrSize.iWidth, scrSize.iHeight)));
			}
		TEST(val);
		
		}
	else
		{
		TEST(0);  //there is a failure
		}
	}

/** these fonts need to be provided, and are used for creating linked typefaces
TLinkedFont1.ttf
Typeface name: LinkedFont1
This will be the dominant font. It contains 178 characters.
Some of the characters (13) are deleted from this file and separated in to other three files.
Characters missing are F, D, J, M, P, S, V, X, Z, a, n, l, y.

TLinkedFont2.ttf
Typeface name: LinkedFont2
Characters available (7):	F, J, M, S, Z, a, y.

TLinkedFont3.ttf
Typeface name: LinkedFont3
Characters available (4):	D, P, x, l

TLinkedFont4.ttf
Typeface name: LinkedFont4
Characters available (2):	V, n

TWithoutLinkedFont.ttf
Typeface name: WithoutLinkedFont
Characters available:	It contains 191 characters U+0020-U+007E and U+00A0-U+00FF
*/	

_LIT(KLinkedFont1, "LinkedFont1TT");
_LIT(KLinkedFont2, "LinkedFont2TT");
_LIT(KLinkedFont3, "LinkedFont3TT");
_LIT(KLinkedFont4, "LinkedFont4TT");

/** Memory test helper function to create different typefaces.  Function adds from 1 to four
 typefaces.
*/
TInt CTLinkedFontsMemory::TestLinkedFontsCreateLinkedTypefacesL(const TDesC &aName,TInt aTypefaces)
	{
	TInt ret = KErrNone;

	CLinkedTypefaceSpecification *typefaceSpec;

	typefaceSpec = CLinkedTypefaceSpecification::NewLC(aName);
	
	CLinkedTypefaceGroup* group1;

	group1 = CLinkedTypefaceGroup::NewLC(1); //may well leave, function trapped, groupID = 1
	typefaceSpec->AddLinkedTypefaceGroupL(*group1);
	CleanupStack::Pop(); //pop the group, now referenced via typeface
	
	CLinkedTypefaceElementSpec *elementSpec1;
	elementSpec1 = CLinkedTypefaceElementSpec::NewLC(KLinkedFont1, 1);	
	elementSpec1->SetCanonical(ETrue);
	typefaceSpec->AddTypefaceAtBackL(*elementSpec1);
	CleanupStack::Pop();  //pop the element, now referenced via typeface

	// Create and add second linked typeface
	if (aTypefaces>=2)
		{
		CLinkedTypefaceElementSpec *elementSpec2;
		elementSpec2 = CLinkedTypefaceElementSpec::NewLC(KLinkedFont2, 1);	
		typefaceSpec->AddTypefaceAtBackL(*elementSpec2);
		CleanupStack::Pop();  //pop the element, now referenced via typeface
		}

	// Create and add third typeface to be linked
	if (aTypefaces>=3)
		{
		CLinkedTypefaceElementSpec *elementSpec3;
		elementSpec3 = CLinkedTypefaceElementSpec::NewLC(KLinkedFont3, 1);	
		typefaceSpec->AddTypefaceAtBackL(*elementSpec3);
		CleanupStack::Pop();  //pop the element, now referenced via typeface
		}

	// Create and add fourth typeface to be linked
	if (aTypefaces>=4)
		{
		CLinkedTypefaceElementSpec *elementSpec4;
		elementSpec4 = CLinkedTypefaceElementSpec::NewLC(KLinkedFont4, 1);	
		TRAP(ret,typefaceSpec->AddTypefaceAtBackL(*elementSpec4));
		CleanupStack::Pop();  //pop the element, now referenced via typeface
		}

	// Try registering the typeface
	CFbsTypefaceStore* store = CFbsTypefaceStore::NewL(iDev);
	CleanupStack::PushL(store);
	typefaceSpec->RegisterLinkedTypefaceL(*store);
	CleanupStack::PopAndDestroy(2, typefaceSpec); //store and typefaceSpec
	return ret;
	}

/** This is the function which does the operations which are tested, called from OOM loop
 * in general the function does not return a low memory error; the function just leaves.
 * An exception may be for registration.
*/
TInt CTLinkedFontsMemory::TestLinkedFontsMemory1L(TInt aTypefaces)
	{
	TInt ret = KErrNone;
	
	//these are for the linked typefaces
	_LIT(KLinkedTypeface1, "LinkedMem1-1"); //has LinkedFont1
	_LIT(KLinkedTypeface2, "LinkedMem1-2"); //has LinkedFont1, LinkedFont2
	_LIT(KLinkedTypeface3, "LinkedMem1-3"); //has LinkedFont1, LinkedFont2, LinkedFont3
	_LIT(KLinkedTypeface4, "LinkedMem1-4"); //has LinkedFont1, LinkedFont2, LinkedFont3, LinkedFont4	
	
	// Create canonical typeface to be linked

	switch (aTypefaces)
		{
		case 1:
			ret = TestLinkedFontsCreateLinkedTypefacesL(KLinkedTypeface1,1);
			break;
		case 2:
			ret = TestLinkedFontsCreateLinkedTypefacesL(KLinkedTypeface2,2);
			break;
		case 3:
			ret = TestLinkedFontsCreateLinkedTypefacesL(KLinkedTypeface3,3);
			break;
		case 4:
			ret = TestLinkedFontsCreateLinkedTypefacesL(KLinkedTypeface4,4);
			break;
		default:
			__ASSERT_ALWAYS(ETrue,Panic(EFntTypefaceIndexOutOfRange));
			break;
		}
	
	RDebug::Printf("%d TestLinkedFontsMemory1L(%d)", ret, aTypefaces);
	return ret;
	}

/**
   @SYMTestCaseID			GRAPHICS-FNTSTORE-LINKEDFONTS-0102

   @SYMPREQ					PREQ2146

   @SYMREQ					REQ10924, REQ10925, REQ10926

   @SYMTestCaseDesc			To ensure that a linked typeface can be created and registered 
   							without memory leaks, under low memory conditions. 

   @SYMTestPriority			Critical

   @SYMTestStatus			Implemented

   @SYMTestActions			(1) Create an OOM loop,	setting this process's heap memory to fail.
   								testing the client side heap memory allocation.
								In the loop create and register a linked typeface with 1-4
								typefaces

   @SYMTestExpectedResults	(1) - There are no memory leaks, and the test exits the OOM loop.
*/
void CTLinkedFontsMemory::TestLinkedFontsMemory1L()
	{
	TInt failures;

	INFO_PRINTF1(_L("TestLinkedFontsMemory"));
	TInt steps;
	for (steps=1;steps<=4;steps++)
		{

		for (failures = 1; ; failures++)
			{
			__UHEAP_SETFAIL(RHeap::EDeterministic,failures);
			__UHEAP_MARK;

			// Test steps
			TInt err;
			TInt val = KErrNone;
			TRAP(err, val= TestLinkedFontsMemory1L(steps));
			__UHEAP_CHECK(0);
			__UHEAP_RESET;
			__UHEAP_MARKEND;

			if (err == KErrNotFound)
				{
				INFO_PRINTF3(_L("Failure below occurred on Step %i; failure=%i"), steps, failures);
				TEST(EFalse);
				break;
				}
			
			if ((err == KErrNone) && (val == KErrNone))
				{
				break;
				}
			}
		}
	}

/**
   @SYMTestCaseID			GRAPHICS-FNTSTORE-LINKEDFONTS-0103

   @SYMPREQ					PREQ2146

   @SYMREQ					REQ10924, REQ10925, REQ10926

   @SYMTestCaseDesc			To ensure that a linked typeface can be created and fonts created
							and used without memory leaks, under low memory conditions

   @SYMTestPriority			Critical

   @SYMTestStatus			Implemented

   @SYMTestActions			(1) Create an OOM loop, setting the server's heap memory to fail.
								In the loop create and register a linked typeface with 1-4
								typefaces.  
							(2) Draw text with the four linked fonts from the four
								linked typefaces.  
							(3) Compare the text output against an unlinked font with the same 
								character forms.

   @SYMTestExpectedResults	(1) - There are no memory leaks, and the test exits the OOM loop.
   							(2) - The text is draw successfully
   							(3) - The text compares identically.
*/	
void CTLinkedFontsMemory::TestLinkedFontsMemory2()
	{
	TInt failures;

	INFO_PRINTF1(_L("TestLinkedFontsMemory Server Heap"));

	// Initially test for linked typeface creation
	_LIT(name1, "typefaceNameMem2-1");
	_LIT(name2, "typefaceNameMem2-2");
	_LIT(name3, "typefaceNameMem2-3");
	_LIT(name4, "typefaceNameMem2-4");

	TInt steps;
	for (steps=1;steps<=4;steps++)
		{
		INFO_PRINTF2(_L("Testing creating linked typeface %d typefaces"),steps);
		for (failures = 1;;failures++)
			{
			// Try creating the typeface

			TInt err;
			TInt val = KErrNone;

			SetHeapFail(RFbsSession::EHeapFailTypeServerMemory, failures);
			SetHeapCheck(RFbsSession::EHeapFailTypeServerMemory); //on
			SetHeapCheck(RFbsSession::EHeapFailTypeHeapMemory); //on

			switch(steps)
				{
				case 1:
					TRAP(err, val = TestLinkedFontsCreateLinkedTypefacesL(name1, steps));
					break;
				case 2:
					TRAP(err, val = TestLinkedFontsCreateLinkedTypefacesL(name2, steps));
					break;
				case 3:
					TRAP(err, val = TestLinkedFontsCreateLinkedTypefacesL(name3, steps));
					break;
				case 4:
					TRAP(err, val = TestLinkedFontsCreateLinkedTypefacesL(name4, steps));
					break;
				}

			SetHeapReset(RFbsSession::EHeapFailTypeServerMemory);
			SetHeapCheck(RFbsSession::EHeapFailTypeServerMemory); //off
			SetHeapCheck(RFbsSession::EHeapFailTypeHeapMemory); //off

			//If the linked font cannot be created because the typefaces are not present or
			//it has already been created then fail the test.
			if (err == KErrNotFound || err == KErrAlreadyExists)
				{
				INFO_PRINTF1(_L("Failure below caused by missing linked font element file or linked font file already being generated."));
				TEST(EFalse);
				break;
				}
			
			if ((val >= 0) && (err == KErrNone))
				break;
			}
		}

	INFO_PRINTF1(_L("Testing linked typeface font creation and usage"));
	for(steps=1;steps<=4;steps++)
		{
		INFO_PRINTF2(_L("Testing font usage: linked font with %d fonts"),steps);
		for (failures = 1; ; failures++)
			{
			TInt val = 0;
#ifdef _DEBUG
			TInt serverBefore=HeapCount(RFbsSession::EHeapFailTypeServerMemory);
			TInt countBefore =0;

			RHeap *heap = (RHeap*)FontAndBitmapServerSharedHeap();
			
			if (heap)
				{
				__RHEAP_MARK(heap);
				countBefore = heap->Count();
				}
			SetHeapFail(RFbsSession::EHeapFailTypeServerMemory, failures);
			SetHeapCheck(RFbsSession::EHeapFailTypeServerMemory); //on
#endif

			switch(steps)
				{
				case 1:
					val=TestLinkedFontsMemory_Part2(name1);
					break;
				case 2:
					val=TestLinkedFontsMemory_Part2(name2);
					break;
				case 3:
					val=TestLinkedFontsMemory_Part2(name3);
					break;
				case 4:
					val=TestLinkedFontsMemory_Part2(name4);
					break;
				}
#ifdef _DEBUG
			TInt serverAfter = HeapCount(RFbsSession::EHeapFailTypeServerMemory);
			TInt countAfter=0;
			TInt badCell=0;
			if (heap)
				{
				countAfter = heap->Count();
				badCell = __RHEAP_MARKEND(heap);
				}

			SetHeapReset(RFbsSession::EHeapFailTypeServerMemory);
			SetHeapCheck(RFbsSession::EHeapFailTypeServerMemory); //off
#endif
			if (val == KErrNone)
				{
				if (steps == 4)
					{
					//do a comparison of the text
					TestCompareText();
					}
				break;
				}
#ifdef _DEBUG
			TEST(badCell == 0);
			TEST(countBefore == countAfter);
			TEST(serverBefore == serverAfter);
#endif
			}
		}
	}

TInt CTLinkedFontsMemory::TestLinkedFontsMemory_Part2(const TDesC &aName)
	{

	TTypeface Typeface;
	Typeface.iName = aName;
	TFontSpec fs;
	fs.iTypeface = Typeface;
	fs.iHeight = 20;
	CFbsFont* font = NULL;
	TInt err1 = iDev->GetNearestFontToDesignHeightInPixels(font, fs);

	if (font && err1 == KErrNone)
		{
		// Use the font
		iGc->UseFont(font);
		iGc->Clear();
		iGc->DrawText(KDrawMe, TPoint(50, 50));
		iGc->DiscardFont();
		iDev->ReleaseFont(font);
		}
	else if (font == NULL)
		{
		TEST(err1==KErrNoMemory);
		if (err1==KErrNone)
			{
			err1 = KErrGeneral;
			}
		}

	return err1;
	}

/**
   @SYMTestCaseID			GRAPHICS-FNTSTORE-LINKEDFONTS-0104

   @SYMPREQ					PREQ2146

   @SYMREQ					REQ10924, REQ10925, REQ10926

   @SYMTestCaseDesc			To ensure that a linked typeface can be created and fonts created
							and used under low memory conditions

   @SYMTestPriority			Critical

   @SYMTestStatus			Implemented

   @SYMTestActions			(1) This function checks that no memory has been retained from the
								usage of a linked font.  This tests server heap and shared heap
								memory.

   @SYMTestExpectedResults	(1) - There are no memory leaks.
*/
void CTLinkedFontsMemory::TestLinkedFontsMemory3()
	{
	_LIT( KTest1, "typefaceNameMem2-1");
	_LIT( KTest2, "typefaceNameMem2-2");
	_LIT( KTest3, "typefaceNameMem2-3");
	_LIT( KTest4, "typefaceNameMem2-4");

#if _DEBUG
		TInt serverBefore = HeapCount(RFbsSession::EHeapFailTypeServerMemory);
		// need to get heap from client side.
		RHeap *heap = (RHeap*)FontAndBitmapServerSharedHeap();
		TInt countBefore=0;
		if (heap)
			{
			__RHEAP_MARK(heap);
			countBefore = heap->Count();
			}
		TInt serverAfter;
#endif
	for (TInt numRuns = 0 ; numRuns <= 50 ; numRuns++)
		{
		for (TInt steps=1;steps<=4;steps++)
			{
			TTypeface Typeface;
			switch (steps)
				{
				case 1:
					Typeface.iName = KTest1;
					break;
				case 2:
					Typeface.iName = KTest2;
					break;
				case 3:
					Typeface.iName = KTest3;
					break;
				case 4:
					Typeface.iName = KTest4;
					break;
				}
			TFontSpec fs;
			fs.iTypeface = Typeface;
			fs.iHeight = 50;
			TInt count;
			CFbsFont *fnt[10];
	
			TInt err;
			for (count = 0; count < 10; count++)
				{
				err = iDev->GetNearestFontToDesignHeightInPixels(fnt[count], fs);
				//now try getting the same font twice, if count is 5
				if(count!=5)
					fs.iHeight++;
				TEST(err == KErrNone);
				}
			for (count = 0; count < 10; count++)
				{
				iDev->ReleaseFont(fnt[count]);
				}
			}
		}
#ifdef _DEBUG
		serverAfter = HeapCount(RFbsSession::EHeapFailTypeServerMemory);
		TInt countAfter=0;
		TInt badCell=0;
		if (heap)
			{
			countAfter = heap->Count();
			badCell = __RHEAP_MARKEND(heap);
			}
		TEST(badCell == 0);
		TEST (countAfter <= countBefore);

			RDebug::Printf("serverAfter(%i) <= serverBefore(%i)", serverAfter, serverBefore);
			INFO_PRINTF3(_L("serverAfter(%i) <= serverBefore(%i)"), serverAfter, serverBefore);

		TEST(serverAfter <= serverBefore); // Note some true type tables are freed.
#endif
	}

/**
   @SYMTestCaseID			GRAPHICS-FNTSTORE-LINKEDFONTS-0105

   @SYMPREQ					PREQ2146

   @SYMREQ					REQ10924, REQ10925, REQ10926

   @SYMTestCaseDesc			To ensure that a linked typeface can be created and fonts created
							and used under low memory conditions of the font and bitmap
							server shared heap.

   @SYMTestPriority			Critical

   @SYMTestStatus			Implemented

   @SYMTestActions			(1) Create an OOM loop, setting the shared heap memory to fail.
								In the loop create and register a linked typeface with 1-4
								typefaces.  Draw text with the four linked fonts from the four
								linked typefaces.

   @SYMTestExpectedResults	(1) - There are no memory leaks, and the test exits the OOM loop.
*/
void CTLinkedFontsMemory::TestLinkedFontsMemory4()
	{

	TInt failures;

	INFO_PRINTF1(_L("TestLinkedFontsMemory Shared Heap"));

	// Initially test for linked typeface creation
	// Initially test for linked typeface creation
	_LIT(name1, "typefaceNameMem4-1");
	_LIT(name2, "typefaceNameMem4-2");
	_LIT(name3, "typefaceNameMem4-3");
	_LIT(name4, "typefaceNameMem4-4");

	TInt steps;
	for(steps=1;steps<=4;steps++)
		{
		INFO_PRINTF2(_L("Testing creating linked typeface %d typefaces"),steps);
		for (failures = 1; ; failures++)
			{
			// Try creating the typeface

			TInt err;
			TInt val = KErrNone;

			SetHeapFail(RFbsSession::EHeapFailTypeHeapMemory, failures);
			SetHeapCheck(RFbsSession::EHeapFailTypeServerMemory); //on
			SetHeapCheck(RFbsSession::EHeapFailTypeHeapMemory); //on

			switch(steps)
				{
				case 1:
					TRAP(err, val = TestLinkedFontsCreateLinkedTypefacesL(name1, 1));
					break;
				case 2:
					TRAP(err, val = TestLinkedFontsCreateLinkedTypefacesL(name2, 2));
					break;
				case 3:
					TRAP(err, val = TestLinkedFontsCreateLinkedTypefacesL(name3, 3));
					break;
				case 4:
					TRAP(err, val = TestLinkedFontsCreateLinkedTypefacesL(name4, 4));
					break;
				}

			SetHeapReset(RFbsSession::EHeapFailTypeHeapMemory);
			SetHeapCheck(RFbsSession::EHeapFailTypeServerMemory); //off
			SetHeapCheck(RFbsSession::EHeapFailTypeHeapMemory); //off

			//If the linked font cannot be created because the typefaces are not present or
			//it has already been created then fail the test.
			if (err == KErrNotFound || err == KErrAlreadyExists)
				{
				INFO_PRINTF1(_L("Failure below caused by missing linked font element file or linked font file already being generated."));
				TEST(EFalse);
				break;
				}
			
			if ((val >= 0) && (err == KErrNone))
				break;
			}
		}

	INFO_PRINTF1(_L("Testing linked typeface font creation and usage"));
	for (steps=1;steps<=4;steps++)
		{
		INFO_PRINTF2(_L("Testing font usage: linked font with %d fonts"), steps);
		for (failures = 1; ; failures++)
			{
			TInt val = 0;
#ifdef _DEBUG
			TInt serverBefore=HeapCount(RFbsSession::EHeapFailTypeServerMemory);
			RHeap *heap=(RHeap*)FontAndBitmapServerSharedHeap();
			TInt countBefore = 0;
			if (heap)
				{
				__RHEAP_MARK(heap);
			 	countBefore = heap->Count();
				}
			SetHeapFail(RFbsSession::EHeapFailTypeHeapMemory, failures);
			SetHeapCheck(RFbsSession::EHeapFailTypeServerMemory); //on
#endif
			switch (steps)
				{
				case 1:
					val = TestLinkedFontsMemory_Part2(name1);
					break;
				case 2:
					val = TestLinkedFontsMemory_Part2(name2);
					break;
				case 3:
					val = TestLinkedFontsMemory_Part2(name3);
					break;
				case 4:
					val = TestLinkedFontsMemory_Part2(name4);
					break;
				}
#ifdef _DEBUG
			TInt serverAfter = HeapCount(RFbsSession::EHeapFailTypeServerMemory);
			TInt countAfter =0;
			TInt badCell = 0;
			if (heap)
				{
				countAfter = heap->Count();
				badCell = __RHEAP_MARKEND(heap);
				}
			SetHeapReset(RFbsSession::EHeapFailTypeHeapMemory);
			SetHeapCheck(RFbsSession::EHeapFailTypeServerMemory); //off
#endif
			if (val == KErrNone)
				{
				if (steps == 4)
					{
					TestCompareText();
					}
				break;
				}
#ifdef _DEBUG
			TEST(badCell == 0);
			TEST (countBefore == countAfter);
			TEST(serverBefore == serverAfter);
#endif
			}
		}
	}

void CTLinkedFontsMemory::DisplayRamInfo(const TDesC& aTextInfo) const
	{
	TInt val;
	HAL::Get(HALData::EMemoryRAMFree, val);
	TBuf <80> buf;
	_LIT(KTotalRamText, "RAM free: %d ");
	buf.Format(KTotalRamText,val);
	buf.Append(aTextInfo);
	iStep->INFO_PRINTF1(buf);
	}

/**
   @SYMTestCaseID			GRAPHICS-FNTSTORE-LINKEDFONTS-0106

   @SYMPREQ					PREQ2146

   @SYMREQ					REQ10924, REQ10925, REQ10926

   @SYMTestCaseDesc			To test memory usage performance of linked font creation. 
   							Test memory usage performance of linked font character access.

   @SYMTestPriority			High

   @SYMTestStatus			Implemented

   @SYMTestActions			(1) Display available ram.  Create a linked typeface with 1-4 linked typeface
								and display ram available.
							(2) Create four linked fonts from the four linked typefaces.  After each step
								display the available RAM.
							(3) Use each of the four linked fonts in turn for drawing text, and at each
								step display the available RAM.	

   @SYMTestExpectedResults	(1) (2) (3) RAM usage is displayed.  
   							There are no memory leaks.
*/
void CTLinkedFontsMemory::TestMemoryUsage()
	{

	INFO_PRINTF1(_L("Test Memory Usage 1"));

	// Initially test for linked typeface creation
	// Initially test for linked typeface creation
	_LIT(name0, "WithoutLinkedFontTT");
	_LIT(name1, "typefaceMemUse-1");
	_LIT(name2, "typefaceMemUse-2");
	_LIT(name3, "typefaceMemUse-3");
	_LIT(name4, "typefaceMemUse-4");

	TInt steps;
	_LIT(KPriorToTesting, "Prior to testing");

	DisplayRamInfo(KPriorToTesting);

	TInt err=KErrNone;
	TInt val=KErrNone;
	//get memory after registering a typeface
	for(steps=1;steps<=4;steps++)
		{
		switch(steps)
			{
			case 1:
				TRAP(err, val = TestLinkedFontsCreateLinkedTypefacesL(name1, 1));
				_LIT(KOneTypeface, "Typeface with one typeface created");
				DisplayRamInfo(KOneTypeface);
				break;
			case 2:
				TRAP(err, val = TestLinkedFontsCreateLinkedTypefacesL(name2, 2));
				_LIT(KTwoTypeface, "Typeface with two typefaces created");
				DisplayRamInfo(KTwoTypeface);
				break;
			case 3:
				TRAP(err, val = TestLinkedFontsCreateLinkedTypefacesL(name3, 3));
				_LIT(KThreeTypeface, "Typeface with three typefaces created");
				DisplayRamInfo(KThreeTypeface);
				break;
			case 4:
				TRAP(err,val = TestLinkedFontsCreateLinkedTypefacesL(name4, 4));
				_LIT(KFourTypeface, "Typeface with four typefaces created");
				DisplayRamInfo(KFourTypeface);
				break;
			}
		TEST(err == KErrNone);
		TEST(val == KErrNone);
		}

	CFont* font[5];
	memset (font,0, sizeof(CFont*)*5);

	_LIT(KGettingFonts, "Now getting 5 fonts, the first one of which is not linked");
	DisplayRamInfo(KGettingFonts);
	//get memory after retrieving a font
	for (steps=0;steps<5;steps++)
		{
		switch (steps)
			{
			case 0:
				{
				TTypeface typeface;
				typeface.iName = name0;
				TFontSpec fs;
				fs.iTypeface = typeface;
				fs.iHeight = 20;
				err = iDev->GetNearestFontToDesignHeightInPixels(font[steps], fs);
				TEST(err==KErrNone);
				_LIT(KOneFont, "Non linked font typeface");
				DisplayRamInfo(KOneFont);
				}
			break;

			case 1:
				{
				TTypeface typeface;
				typeface.iName = name1;
				TFontSpec fs;
				fs.iTypeface = typeface;
				fs.iHeight = 20;
				err = iDev->GetNearestFontToDesignHeightInPixels(font[steps], fs);
				TEST(err==KErrNone);
				_LIT(KOneFont, "LinkedFont one typeface");
				DisplayRamInfo(KOneFont);
				}
			break;
			case 2:
				{
				TTypeface typeface;
				typeface.iName = name2;
				TFontSpec fs;
				fs.iTypeface = typeface;
				fs.iHeight = 20;
				err = iDev->GetNearestFontToDesignHeightInPixels(font[steps], fs);
				TEST(err==KErrNone);
				_LIT(KTwoFont, "LinkedFont two typefaces");
				DisplayRamInfo(KTwoFont);
				}
			break;
			case 3:
				{
				TTypeface typeface;
				typeface.iName = name3;
				TFontSpec fs;
				fs.iTypeface = typeface;
				fs.iHeight = 20;
				err = iDev->GetNearestFontToDesignHeightInPixels(font[steps], fs);
				TEST(err==KErrNone);
				_LIT(KThreeFont, "LinkedFont three typefaces");
				DisplayRamInfo(KThreeFont);
				}
			break;
			case 4:
				{
				TTypeface typeface;
				typeface.iName = name4;
				TFontSpec fs;
				fs.iTypeface = typeface;
				fs.iHeight = 20;
				err = iDev->GetNearestFontToDesignHeightInPixels(font[steps], fs);
				TEST(err==KErrNone);
				_LIT(KFourFont, "LinkedFont four typefaces");
				DisplayRamInfo(KFourFont);
				}
			break;
			}
		}

	//get memory after font usage
	const TDesC* string[5];
	_LIT(KString0,"A");
	_LIT(KString1,"B");
	_LIT(KString2,"F");
	_LIT(KString3,"D");
	_LIT(KString4,"V");
	string[0]=&KString0();
	string[1]=&KString1();
	string[2]=&KString2();
	string[3]=&KString3();
	string[4]=&KString4();

	_LIT(KWritingCharacter,"Now outputing a single character from each of the five fonts");
	DisplayRamInfo(KWritingCharacter);
	for (steps=0;steps<5;steps++)
		{
		// Use the font
		iGc->UseFont(font[steps]);
		iGc->Clear();
		_LIT(KNowUsingFont,"Now using font");
		DisplayRamInfo(KNowUsingFont);
		iGc->DrawText(*string[steps], TPoint(100, 100));
		TBuf <80> buf;
		_LIT(KNumberOfSteps,"The font in use has %d linked fonts");
		buf.Format(KNumberOfSteps, steps);
		DisplayRamInfo(buf);
		iGc->DiscardFont();
		}

	//release the fonts
	for (steps=0;steps<5;steps++)
		{
		iDev->ReleaseFont(font[steps]);
		}
	_LIT(KTestEnd,"At the end of the memory test");
	DisplayRamInfo(KTestEnd);
	}

void CTLinkedFontsMemory::RunTestCaseL( TInt aCurTestCase )
	{
		
	((CTLinkedFontsMemoryStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);
	switch ( aCurTestCase )
		{
	case 1:
		((CTLinkedFontsMemoryStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
		StartServer();
		if (!CheckRasterizerInstalledL())
			{
			TestComplete();
			return;
			}
		break;
	case 2:
		((CTLinkedFontsMemoryStep*)iStep)->SetTestStepID(_L("GRAPHICS-FNTSTORE-LINKEDFONTS-0102"));
#ifdef _DEBUG
		//memory tests are not valid under DEBUG/ urel, and they will fail
		__UHEAP_MARK;
		// This is for client side memory
		TestLinkedFontsMemory1L();
		__UHEAP_MARKEND;
		__UHEAP_RESET;
#endif
		break;
	case 3:
		((CTLinkedFontsMemoryStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
#ifdef _DEBUG
		__UHEAP_MARK;
		//((CTLinkedFontsMemoryStep*)iStep)->SetTestStepID(_L("GRAPHICS-FNTSTORE-LINKEDFONTS-0103"));
		// This is for server side memory
		//TestLinkedFontsMemory2(); //- test crashes the server (from the rasterizer)
		//TEST(EFalse);
		__UHEAP_MARKEND;
		__UHEAP_RESET;
#endif
		break;
	case 4:
		((CTLinkedFontsMemoryStep*)iStep)->SetTestStepID(_L("GRAPHICS-FNTSTORE-LINKEDFONTS-0104"));
#ifdef _DEBUG
		__UHEAP_MARK;
		// This is to check that there are no leaks getting and releasing fonts
		//TestLinkedFontsMemory3();
		__UHEAP_MARKEND;
		__UHEAP_RESET;
#endif
		break;
	case 5:
		((CTLinkedFontsMemoryStep*)iStep)->SetTestStepID(_L("GRAPHICS-FNTSTORE-LINKEDFONTS-0105"));
#ifdef _DEBUG
		__UHEAP_MARK;
		// This is for heap memory
		//TestLinkedFontsMemory4();
		__UHEAP_MARKEND;
		__UHEAP_RESET;
#endif
		break;

	case 6:
#ifdef _DEBUG
		__UHEAP_MARK;
#endif		
		((CTLinkedFontsMemoryStep*)iStep)->SetTestStepID(_L("GRAPHICS-FNTSTORE-LINKEDFONTS-0106"));
		TestMemoryUsage();
#ifdef _DEBUG		
		__UHEAP_MARKEND;
		__UHEAP_RESET;
#endif		
		break;	
	case 7:
		((CTLinkedFontsMemoryStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
		((CTLinkedFontsMemoryStep*)iStep)->CloseTMSGraphicsStep();
		TestComplete();
		break;
		}
	((CTLinkedFontsMemoryStep*)iStep)->RecordTestResultL();
	}

// --------------
__CONSTRUCT_STEP__(LinkedFontsMemory)
