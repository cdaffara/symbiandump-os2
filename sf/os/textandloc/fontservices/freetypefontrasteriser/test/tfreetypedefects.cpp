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
* This file contains the test steps for defects
*
*/


#include "testharness.h"
#include "tfreetypedefects.h"


//-----------------
CTSurrogateRasterizedStep::CTSurrogateRasterizedStep()
	{
	SetTestStepName(KTSurrogateRasterizedStep);
	}

/*
@SYMTestCaseID GRAPHICS-FREETYPE-0001
@SYMTestCaseDesc Supplementary characters rasterization Test
@SYMTestPriority High
@SYMTestActions  This test checks that the free type rasterizer rasterizes supplementary characters 
properly.
It prints the descriptor containing supplementary characters with 2 Fonts. One font does not support
supplementary characters and the other does.
@SYMTestExpectedResults This step should be executed without error and the user should be able to see the 
above descriptor is displayed as a square with the first font and in the correct shape with the other font.
@SYMDEF DEF120018 Security issue in FreeType rasterizer! Supplementary characters rendered as BMP
*/
TVerdict CTSurrogateRasterizedStep::doTestStepL()
    {
    SetTestStepResult(EFail);
	SetTestStepID(_L("GRAPHICS-FREETYPE-0001"));

	__UHEAP_MARK;
	
	CTestHarness* testHarness = CTestHarness::NewL(this);
	TRAPD(error, testHarness->SurrogateRasterizedTestsL());	
	delete testHarness;
	
	__UHEAP_MARKEND;

	if(error == KErrNone)
		{
		SetTestStepResult(EPass);
		}
	RecordTestResultL();

	return TestStepResult();
    }
