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
* T_FNTMEM.CPP
*
*/


#ifndef __T_FNTMEM_H__
#define __T_FNTMEM_H__

#include <fntstore.h>
#include "TGraphicsHarness.h"


class CTFntMem : public CTGraphicsBase
	{
public:
	CTFntMem(CTestStep* aStep);
protected:
//from 	CTGraphicsStep
	virtual void RunTestCaseL(TInt aCurTestCase);
private:
	void TestNewL();
	void TestAddFileL(const TDesC& aFileName);
	void TestAddFileTwiceL(const TDesC& aFileName);
	void TestGetNearestFontToDesignHeightInTwipsL();
	void TestGetNearestOpenFontLeaksL();
	void TestDuplicateBitmapFontFileEntriesL();
private:
	RHeap* iHeap;
	};


class CTFntMemStep : public CTGraphicsStep
	{
public:
	CTFntMemStep();
protected:	
	//from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	};

_LIT(KTFntMemStep,"TFntMem");



#endif
