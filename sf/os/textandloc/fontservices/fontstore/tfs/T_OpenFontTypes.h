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


#ifndef __T_OPENFONTTYPES_H__
#define __T_OPENFONTTYPES_H__

#include "TGraphicsHarness.h"

_LIT(KTOpenFontTypesStep, "TOpenFontTypes");

class CTOpenFontTypesStep : public CTGraphicsStep
	{
public:
	CTOpenFontTypesStep();
protected:	
	// from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	};

class CTOpenFontTypes : public CTGraphicsBase
	{
public:
	CTOpenFontTypes(CTestStep* aStep);
protected:
	// from CTGraphicsBase
	virtual void RunTestCaseL(TInt aCurTestCase);
private:
	static const TInt KOneIn16Dot16FixedPointFormat = 65536;
	void TestBitmapType(TOpenFontSpec& aOpenFontSpec);
	void TestHeight(TOpenFontSpec& aOpenFontSpec);
	void TestWidthFactor(TOpenFontSpec& aOpenFontSpec);
	void TestSlantFactor(TOpenFontSpec& aOpenFontSpec);
	void TestFontEffect(TOpenFontSpec& aOpenFontSpec);
	void VerifyFontEffect(TOpenFontSpec& aOpenFontSpec);
	void TestCompatibilityWithTFontSpec();
	void TestOperatorEqualEqual();
	void TestCompensateForAspectRatio();
	void VerifyCompensateForAspectRatio(
		const TOpenFontSpec& aOpenFontSpec, TInt aWidth, TInt aHeight,
		TInt32 aWidthFactorB4Compensation, TInt32 aSlantFactorB4Compensation);
	};

#endif
