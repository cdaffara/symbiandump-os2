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


#include <e32test.h>
#include <bitdev.h>
#include <fntstore.h>
#include <fbs.h>
#include <test/ttmsgraphicsstep.h>

// This test is based on code from the TTAGMA test in form

#ifndef __T_GLYPHTREEDEL_H__
#define __T_GLYPHTREEDEL_H__

#include "TGraphicsHarness.h"

class CTGlyphTreeDel : public CBase
	{
public:
	static CTGlyphTreeDel* NewL(CTestStep* aStep);
	~CTGlyphTreeDel();
	
	void TestGlyphTreeDeleteL();

private:
	CTGlyphTreeDel(CTestStep* aStep);
	void ConstructL();
	TInt CreateScreenDevice();

	void CheckRam( TInt);
	TInt GetRam();
	RFbsSession* iFbs;
	CFbsBitGc* iGc;
	CFbsScreenDevice* iDev;
	CTestStep* iStep;
	};


class CTGlyphTreeDelStep : public CTTMSGraphicsStep
	{
public:
	CTGlyphTreeDelStep();
protected:	
	TVerdict doTestStepL();
	};

_LIT(KTGlyphTreeDelStep,"TGlyphTreeDel");



#endif
