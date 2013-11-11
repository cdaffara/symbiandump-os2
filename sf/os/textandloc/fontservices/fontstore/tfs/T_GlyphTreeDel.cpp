/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <bitdev.h>
#include <fntstore.h>
#include <fbs.h>
#include <graphics/fbsdefs.h>
#include "T_GlyphTreeDel.h"

const TDisplayMode testDisplayModes[] =	
	{
	EGray2,
	EGray4,
	EGray16,
	EGray256,
	EColor16,
	EColor256,
	EColor64K,
	EColor16M,
	EColor4K,
	EColor16MU,
	EColor16MA,
	EColor16MAP,
	};

// This test is based on code from the TTAGMA test in form


CTGlyphTreeDel::CTGlyphTreeDel(CTestStep* aStep):
	iStep(aStep)
	{
	}

CTGlyphTreeDel::~CTGlyphTreeDel()
	{
	delete iGc;
	delete iDev;
	RFbsSession::Disconnect();
	}

CTGlyphTreeDel* CTGlyphTreeDel::NewL(CTestStep* aStep)
	{
	CTGlyphTreeDel *theTest = new (ELeave) CTGlyphTreeDel(aStep);
	
	CleanupStack::PushL(theTest);
	theTest->ConstructL();
	CleanupStack::Pop();
	return theTest;
	}
	
void CTGlyphTreeDel::ConstructL()
	{
	iStep -> INFO_PRINTF1(_L("DEF047283"));
	RFbsSession::Connect();
	iFbs = RFbsSession::GetSession();
	if (iFbs == NULL)
		User::Leave(KErrGeneral);
	User::LeaveIfError(CreateScreenDevice());
	iDev->ChangeScreenDevice(NULL);
	iDev->SetAutoUpdate(ETrue);
	iDev->CreateContext(iGc);
	}

TInt CTGlyphTreeDel::CreateScreenDevice()
	{
	TInt err = KErrNone;
	TInt sizeOfDisplayMode = sizeof (testDisplayModes) / sizeof(testDisplayModes[0]);
	 
	for(TInt theScreenModeIndex = sizeOfDisplayMode - 1; theScreenModeIndex ; theScreenModeIndex--)
		{
		TDisplayMode disp = testDisplayModes[theScreenModeIndex];
		TRAP(err, iDev = CFbsScreenDevice::NewL(_L(""), disp)); //scdv
		if(err == KErrNone)
			{
			iStep->INFO_PRINTF2(_L("Device %d has been created"), disp);
			return KErrNone;
			}
		}
	return err;	
	}

// Get System free Ram

TInt CTGlyphTreeDel::GetRam()
	{
	TMemoryInfoV1Buf memory;
	UserHal::MemoryInfo(memory);
	return(TInt)(memory().iFreeRamInBytes);	
	}
	
void CTGlyphTreeDel::CheckRam( TInt beforeFreeRam)
	{
	TInt freeRam=GetRam();
	if( beforeFreeRam < freeRam)
		{
		iStep -> INFO_PRINTF2( _L("Freed  %08d bytes of RAM\n"), freeRam-beforeFreeRam);
		}
	else if( beforeFreeRam > freeRam)
		{
		iStep -> INFO_PRINTF2( _L("Leaked %08d bytes of RAM\n"), beforeFreeRam-freeRam);
		}
	}

/**
	@SYMTestCaseID
	GRAPHICS-FNTSTORE-0034

	@SYMTestCaseDesc
	Tests glyph cache. 
		
	@SYMTestActions
	1. Sets pen style to null pen.
	2. Sets brush color to gray.
	3. Sets brush style to solid.
	4. Retrieves number of typefaces.
	5. Retrieves the details of the typeface family.
	6. Gets the font which is the nearest to the given font specification.
	7. Sets the retrieved font to be used into the graphic context.
	8. Draws a string to the display.
	9. Releases the font.
	10. Discards the font.
	11. Glyph cache is filled by looping. The glyph cache heap should be large enough
		for all subsequent loops, if the glyph tree is successfully deleted after the font is
		discarded. 
	12. Waits three seconds after the loops complete for the first time. Then records the free RAM. 
	    No extra RAM should be required to be allocated after this point.
	13. Check if there is any RAM loss.
	
	@SYMTestExpectedResults
	Test should pass
*/
void CTGlyphTreeDel::TestGlyphTreeDeleteL()
	{
    iGc->SetPenStyle(CGraphicsContext::ENullPen);
    iGc->SetBrushColor(KRgbGray);
    iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);

	TInt typefaces = iDev->NumTypefaces();
	TTypefaceSupport info;	
	TInt fontSize = 24;
			
	TInt beforeFreeRam = GetRam(); 		// Initialise here because of WINS warning
	
	for(TInt loop=0;loop<100;++loop)
		{		
		if (loop&1) //if it is odd
			fontSize+=12;
		else
			fontSize-=12;

		TInt count=0;
		TInt forLoop = 0;
		
		for (count=0;count < typefaces; count++)
			{
			iDev->TypefaceSupport(info, count); 
			TFontSpec fs;
			fs.iTypeface = info.iTypeface;
			fs.iHeight = fontSize;
			CFbsFont* font = NULL;
			TInt err = iDev->GetNearestFontToDesignHeightInPixels((CFont*&)font, fs);

			if( font == NULL)
				{
				iStep -> INFO_PRINTF6(_L("[Couldn't get font Loop  % 2d, Iteration % 2d Font \"%S\", Size %d, err %d]\n"), forLoop, count, &fs.iTypeface.iName, fontSize, err);
				continue;
				}

			iGc->UseFont(font);			
			iGc->DrawText(_L("ABCDEFG123456789\n"), TPoint(10+loop, 200-(fontSize*2)+count));
			iDev->ReleaseFont(font);
			iGc->DiscardFont();	
			}
		
		forLoop=1;
		for (count=typefaces-1;count>=0;--count)
			{
			iDev->TypefaceSupport(info, count); 

			TFontSpec fs;
			fs.iTypeface = info.iTypeface;
			fs.iHeight = fontSize;
			CFbsFont* font = NULL;
			TInt err = iDev->GetNearestFontToDesignHeightInPixels((CFont*&)font, fs);

			if( font == NULL)
				{
				iStep -> INFO_PRINTF6(_L("[Couldn't get font Loop  % 2d, Iteration % 2d Font \"%S\", Size %d, err %d]\n"), forLoop, count, &fs.iTypeface.iName, fontSize, err);
				continue;
				}

			iGc->UseFont(font);
			iGc->DrawText(_L("ABCDEFG123456789\n"), TPoint(10+loop, 200-(fontSize*2)+count));
			iDev->ReleaseFont(font);
			iGc->DiscardFont();
			}

		// When the 2 loops above have completed once, the glyph cache heap should be large enough
		// for all subsequent loops, if the glyph tree is successfully deleted after the font is
		// discarded. So we wait 3 seconds after the 2 loops complete for the 1st time and then
		// record the free RAM. No extra RAM should be required to be allocated after this point.			
			
		if( loop==0)
			{
		
			// Wait to ensure font memory grabs are complete
			User::After(3000000);	
	
			// save initial free RAM figure
			beforeFreeRam = GetRam();
			}

		}
		// Wait for all requests to complete
		User::After(2000000);

		// If there is any RAM loss print it out before call to iTest 
		CheckRam(beforeFreeRam);		
		TInt afterFreeRam = GetRam();
		// Add a safety buffer to accouunt for miscellaneous system changes
		// affect the total ram. The defect this was set to catch was leaking
		// of the order of a megabyte so this will still catch that.
		afterFreeRam += 65536;
		
		// Any extra RAM grabbed for the glyph trees should be released by 
		// the calls to DiscardFont
		
		iStep-> testBooleanTrue((afterFreeRam>=beforeFreeRam), (TText8*)__FILE__, __LINE__);
	}


//--------------
CTGlyphTreeDelStep::CTGlyphTreeDelStep()
	{
	SetTestStepName(KTGlyphTreeDelStep);
	}

TVerdict CTGlyphTreeDelStep::doTestStepL()
	{
#if defined(__SUPPORT_CPP_EXCEPTIONS__)
	SetTestStepID(_L("GRAPHICS-FNTSTORE-0034"));
	FbsStartup();
	CTrapCleanup* TrapCleanup = CTrapCleanup::New();
	TInt error = KErrNone;
	try	{
		TTrapHandler* trapHandler = User::MarkCleanupStack();
		__UHEAP_MARK;

		User::After(10000000);	
		CTGlyphTreeDel* test_harness = CTGlyphTreeDel::NewL(this);
		test_harness->TestGlyphTreeDeleteL();
		delete test_harness;

		__UHEAP_MARKEND;
		User::UnMarkCleanupStack(trapHandler);
		}
	catch (XLeaveException& leave)
		{
		error = leave.GetReason();
		}
	catch (...)
		{
		User::Invariant();
		}
	delete TrapCleanup;

	if (error)
		{
		SetTestStepResult(EFail);
		}
	RecordTestResultL();
#endif	//__SUPPORT_CPP_EXCEPTIONS__
	return TestStepResult();
	}
