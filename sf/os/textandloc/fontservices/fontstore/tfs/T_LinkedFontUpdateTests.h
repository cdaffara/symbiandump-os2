/*
* Copyright (c) 1995-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* T_LinkedFontUpdateTests.cpp
*
*/

#ifndef T_LINKEDFONTUPDATETESTS_H_
#define T_LINKEDFONTUPDATETESTS_H_

#include "TGraphicsHarness.h"
//Global variables for Linked Font Names
_LIT(KAddElementFontName, "UpdateAddFont");
_LIT(KRemoveElementFontName, "UpdateRemoveFont");
_LIT(KUpdateFontName, "UpdateFont");

//Element Names
_LIT(KElement1Name, "DejaVu Sans Mono");
_LIT(KElement2Name, "DejaVu Sans Condensed");
_LIT(KElement3Name, "DejaVu Serif Condensed");

//Class for first linked fonts test step
class CTLinkedFontsUpdateStage1Step : public CTGraphicsStep
	{
public:
	CTLinkedFontsUpdateStage1Step();
protected:
	// From CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	};

_LIT(KTLinkedFontsUpdateStage1Step, "TLinkedFontsUpdateStage1");

class CTLinkedFontsUpdateStage1 : public CTGraphicsBase
	{
public:
	CTLinkedFontsUpdateStage1(CTestStep* aStep);
	~CTLinkedFontsUpdateStage1();
	static void TestPanic(TInt aPanic);
protected:
// From CTGraphicsStep
	virtual void RunTestCaseL(TInt aCurTestCase);
protected:
	void SetupTestsL();
	void CloseTestsL();
	void RegisterThenUpdateTestL();
	void NegativeUpdateTestsL();
	
private:
	CLinkedTypefaceSpecification* CreateAddTypefaceLC(TBool aUpdate);
	CLinkedTypefaceSpecification* CreateRemoveTypefaceLC(TBool aUpdate);
	CLinkedTypefaceSpecification* CreateUpdateTypefaceLC(TBool aUpdate);
	CLinkedTypefaceSpecification* CreateNegativeTestTypefaceLC(const TDesC& aName);
	void AddGroups(CLinkedTypefaceSpecification* aSpec);
	
private:
	RFbsSession* iFbs;
	CFbsTypefaceStore* iTfStore;
	TBool iFontLinkingSupported;
	};


//Class for second linked fonts test step
class CTLinkedFontsUpdateStage2Step : public CTGraphicsStep
	{
public:
	CTLinkedFontsUpdateStage2Step();
protected:
	// From CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	};

_LIT(KTLinkedFontsUpdateStage2Step, "TLinkedFontsUpdateStage2");

class CTLinkedFontsUpdateStage2 : public CTGraphicsBase
	{
public:
	CTLinkedFontsUpdateStage2(CTestStep* aStep);
	~CTLinkedFontsUpdateStage2();
	static void TestPanic(TInt aPanic);
protected:
// From CTGraphicsStep
	virtual void RunTestCaseL(TInt aCurTestCase);
protected:
	void SetupTestsL();
	void CloseTestsL();
	void ValidateLinkedFontsL();
private:
	RFbsSession* iFbs;
	CFbsTypefaceStore* iTfStore;
	};

#endif /*T_LINKEDFONTUPDATETESTS_H_*/
