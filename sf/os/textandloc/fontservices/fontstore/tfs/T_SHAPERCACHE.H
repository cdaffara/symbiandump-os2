/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __T_SHAPERCACHE_H__
#define __T_SHAPERCACHE_H__

#include <fntstore.h>
#include "TGraphicsHarness.h"


class CTShaperCache : public CTGraphicsBase
	{
public:
	CTShaperCache(CTestStep* aStep);
	~CTShaperCache();
protected:
//from 	CTGraphicsStep
	virtual void RunTestCaseL(TInt aCurTestCase);
	virtual void ConstructL();
private:
	void Test1L();
	void Test2L();
	void TestShaperCacheAPIsL();
private:
	RHeap* iHeap;
	CFontStore* iFontStore;
	};


class CTShaperCacheStep : public CTGraphicsStep
	{
public:
	CTShaperCacheStep();
protected:	
	//from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	};

_LIT(KTShaperCacheStep,"TShaperCache");



#endif
