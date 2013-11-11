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


#ifndef T_LINKEDFONTS2_H_
#define T_LINKEDFONTS2_H_

#include "TGraphicsHarness.h"

class COpenFontLinkedTypefaceElementSpec;
class CLinkedTypefaceElementSpec;
class CLinkedTypefaceGroup;

class CTLinkedFonts2Step : public CTGraphicsStep
	{
public:
	CTLinkedFonts2Step();
protected:
	// From CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	};

_LIT(KTLinkedFonts2Step, "TLinkedFonts2");

class CTLinkedFonts2 : public CTGraphicsBase
	{
public:
	CTLinkedFonts2(CTestStep* aStep);
	~CTLinkedFonts2();
	static void TestPanic(TInt aPanic);
protected:
// From CTGraphicsStep
	virtual void RunTestCaseL(TInt aCurTestCase);
protected:
	//Test Functions
	void InitializeTestHarnessL();
	void CloseTestHarnessL();
	void TestLinkedClientGettersAndSettersL();
	void TestLinkedClientAddAndRemoveL();
	void TestOpenClientGettersAndSettersL();
	void TestOpenClientAddAndRemoveL();
	void TestLinkedRegistrationAndFetchL();
	TBool ComplexFontAvailable();
	void TestComplexLinkedFontsBasicL();
	void TestBitmapFontsL();
	void TestTOpenFontMetrics();
	void StartServer();
	void TestFontNameUsage();
	
	TBool CompareElementSpecs(CLinkedTypefaceElementSpec* aSpec1, CLinkedTypefaceElementSpec* aSpec2);
	TBool CompareElementSpecs(COpenFontLinkedTypefaceElementSpec* aSpec1, COpenFontLinkedTypefaceElementSpec* aSpec2);
	TBool CompareGroups(CLinkedTypefaceGroup* aGroup1, CLinkedTypefaceGroup* aGroup2);
protected:
	RFbsSession* iFbs;
	CFbsScreenDevice* iDev;
	CFbsBitGc* iGc;
	};

#endif /*T_LINKEDFONTS2_H_*/
