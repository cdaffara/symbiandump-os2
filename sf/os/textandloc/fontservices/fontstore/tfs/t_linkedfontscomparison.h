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


#ifndef T_LINKEDFONTSCOMPARISON_H_
#define T_LINKEDFONTSCOMPARISON_H_

#include "TGraphicsHarness.h"
#include "thashreferenceimages.h"

class CTLinkedFontsComparisonStep : public CTGraphicsStep
	{
public:
	CTLinkedFontsComparisonStep();
protected:
	// From CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	};

_LIT(KTLinkedFontsComparisonStep, "TLinkedFontsComparison");

class CTLinkedFontsComparison : public CTGraphicsBase
	{
public:
	CTLinkedFontsComparison(CTestStep* aStep);
	~CTLinkedFontsComparison();

protected:
	virtual void RunTestCaseL(TInt aCurTestCase);
	virtual void ConstructL();
private:
	TBool CheckMonoTypeInstalledL();
	void CreateBitmapDeviceL(TDisplayMode aMode);
	void RegisterLinkedTypefacesL();
	void TestDrawTextUsingLinkedFontsL();
	void FreeScreenDevice();
	void DoTestDrawingOutputL(const TDesC& aHashId, const TPtrC& aFont, const TDesC& aOutputString,
							 TPoint aPosition, TInt size, TUint32 attributes);
	void PrepareTestEnvironment(TFontSpec &aFontSpec, const TPtrC& aFont, TInt aSize, TUint32 aAttributes,
							TBool aSetEffects = ETrue, TBool aSetColors = EFalse);
	HBufC* GenerateTestDescriptionLC(TInt aSize,TUint32 aAttributes, const TPtrC &aTypefaceName, const TDesC &aTestDescription);

private:
	CFbsBitGc* iGc;
	CFbsBitmapDevice* iBitmapDevice;
	RFbsSession* iFbs;
	CFbsBitmap* iBitmap;
	CTestStep* iStep;
	CTHashReferenceImages *iHashUtil;
	};

#endif /*T_LINKEDFONTS2_H_*/
