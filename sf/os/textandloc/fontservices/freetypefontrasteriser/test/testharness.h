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


#ifndef __TESTHARNESS_H__
#define __TESTHARNESS_H__

#include <e32base.h>

class CTestStep;
class RFbsSession;
class CFbsBitGc;
class CFbsScreenDevice;
class CFbsFont;
class CFbsFont;
class TTypefaceSupport;

// The class carries out the freetype component tests.
// It has reusable code such as connecting to the Fbs server, calculating display position etc.,
// which makes the test coding easier.
// New test case can be added as public member function.
class CTestHarness: public CBase
	{
public:
	static CTestHarness* NewL(CTestStep* aStep);
	~CTestHarness();
	void PerformTests();
	void SurrogateRasterizedTestsL();

private:
	CTestHarness(CTestStep* aStep);
	void ConstructL();
	void TestFont(TInt aTypeface);
	void TestFontDisplayAtSize(TTypefaceSupport& aSupport, TInt aPointSize, TBool aItalic);
	void TestFontMetricsDiagram(TTypefaceSupport& aSupport, TInt aPointSize, TBool aItalic);
	void TestFontHugeCharacters(TTypefaceSupport& aSupport, TInt aPointSize, TBool aItalic);
	void DisplayFont();
	void Clear();
	void Print(const TDesC& aText);
	void UseFont(CFbsFont* aFont);
	void PrintMetrics(TBool isClear = ETrue);
	void DrawCharMetrics(TPoint& aPos,TInt aCode);
	void DrawHugeCharacters();
	void DrawMetricsDiagram();

	RFbsSession* iFbs;
	CFbsBitGc* iGc;
	CFbsScreenDevice* iDev;
	CFbsFont* iStandardFont;
	CFbsFont* iTestFont;
	TPoint iPrintPos;
	TInt iBaseLine;
	TInt iLineHeight;
	CTestStep* iStep;
	TBuf<256> iBuffer;
	};

#endif
