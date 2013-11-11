/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalComponent Internal Symbian test code
*/

#ifndef __T_FONTMETRICS_H__
#define __T_FONTMETRICS_H__

#include <fntstore.h>
#include "TGraphicsHarness.h"

_LIT(KTFontMetricsStep, "TFontMetrics");

class CTFontMetricsStep : public CTGraphicsStep
	{
public:
	CTFontMetricsStep();
protected:	
	// from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	};

class CTFontMetrics : public CTGraphicsBase
	{
public:
	CTFontMetrics(CTestStep* aStep);
	~CTFontMetrics();
protected:
	// from CTGraphicsBase
	virtual void RunTestCaseL(TInt aCurTestCase);
	virtual void ConstructL(void);
private:
	void InstallRasterizerL(void);
	void AddFileL(void);
	TOpenFontSpec GetTOpenFontSpec(void);
	void TLanguage2GlyphSample(void);
	void GetNearestFontToDesignHeightInPixels(void);
	void Verify_GetNearestFontToDesignHeightInPixels(const CFont& aFont, TInt aScript);
	void GetNearestFontToMaxHeightInPixels(void);
	void Verify_GetNearestFontToMaxHeightInPixels(const CFont& aFont, TInt aScript);
	void VerifyTypefaceNameAndID(const CFont& aFont);
	void FontAccessInCTypefaceStore(void);
	void TestINC086513();
	void TestPDEF099034();
	void TestGetNearestFontToMaxHeightInPixels();
private:
	static const TInt KRequiredHeight;
private:
	CFontStore*	iFontStore;
	RHeap*		iHeap;
	};

#endif // __T_FONTMETRICS_H__
