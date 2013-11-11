/*
* Copyright (c) 2004-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* TFormat.cpp tests for TTmCharFormat and CTmParFormat
*
*/


#include "TAGMA.H"
#include <txtfrmat.h>
#include <e32test.h>
#include "tformat.h"

#define UNUSED_VAR(a) a = a

namespace LocalToTFormat
{
CTFormatStep* TestStep = NULL;
#define TESTPOINT(p) TestStep->testpoint(p,(TText8*)__FILE__,__LINE__)
#define TESTPRINT(p) TestStep->print(p,(TText8*)__FILE__,__LINE__)
}
using namespace LocalToTFormat;

void INC041190()
	{
	TTmCharFormat tmcf;
	TCharFormat cf;
	cf.iFontPresentation.iHighlightStyle
		= TFontPresentation::EFontHighlightNoMatchesIndicator;
	tmcf.iEffects = 0;
	tmcf = cf;
	TESTPOINT(tmcf.iEffects == TTmCharFormat::ENoMatchesIndicator);
	cf.iFontPresentation.iHighlightStyle
		= TFontPresentation::EFontHighlightNone;
	tmcf.GetTCharFormat(cf);
	TESTPOINT(cf.iFontPresentation.iHighlightStyle
		== TFontPresentation::EFontHighlightNoMatchesIndicator);
	}

TVerdict CTFormatStep::doTestStepL()
	{
    SetTestStepResult(EPass);
    TestStep = this;
    TESTPRINT(_L("TFormat - TTmCharFormat and CTmParFormat"));
    
	__UHEAP_MARK;

	TESTPRINT(_L(" @SYMTestCaseID:SYSLIB-FORM-LEGACY-TFORMAT-0001 INC041190 - EText's EFontHighlightNoMatchesIndicator isn't mappd to anythng in TTmCharFormat "));
	INC041190();

	__UHEAP_MARKENDC(0);
	return TestStepResult();
	}

