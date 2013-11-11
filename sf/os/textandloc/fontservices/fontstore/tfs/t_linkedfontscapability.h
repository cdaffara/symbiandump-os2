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


#ifndef T_LINKEDFONTSCAPABILITY_H_
#define T_LINKEDFONTSCAPABILITY_H_

#include "TGraphicsHarness.h"

class CTLinkedFontsCapabilityStep : public CTGraphicsStep
	{
public:
	CTLinkedFontsCapabilityStep();
protected:
	// From CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	};

_LIT(KTLinkedFontsCapabilityStep, "TLinkedFontsCapability");

class CTLinkedFontsCapability : public CTGraphicsBase
	{
public:
	CTLinkedFontsCapability(CTestStep* aStep);
	~CTLinkedFontsCapability();
	virtual void RunTestCaseL(TInt aCurTestCase);

private:
	void CreateBitmapDeviceL(TDisplayMode aMode);
	void TestRegisterL();
	void TestFetchL();
	void CheckLinkedFontsRasterizerInstalledL();
	TBool LinkedFontsRasterizerPresent();
private:
	TBool iHaveRasterizer;
	CFbsBitmapDevice* iBitmapDevice;
	CFbsBitmap* iBitmap;
	CFbsBitGc* iGc;
	};

#endif /*T_LINKEDFONTS2_H_*/
