/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __T_CACHE_H__
#define __T_CACHE_H__

#include <fntstore.h>
#include "TGraphicsHarness.h"
#include <gdi.h>
#include <fbs.h> 

class CTCache : public CTGraphicsBase
	{
public:
	CTCache(CTestStep* aStep);
	 ~CTCache();
protected:
//from 	CTGraphicsStep
	virtual void RunTestCaseL(TInt aCurTestCase);
private:
	void TFontCacheItem();
	void TFontCache();
	void TFontCacheForCov();
	void TGlyphCacheItem();
	void THintedGlyphCache();
	void TUnHintedGlyphCache();
	
private:
    RHeap*               iHeap;
    };


class CTCacheStep : public CTGraphicsStep
	{
public:
	CTCacheStep();
protected:	
	//from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	};

_LIT(KTCacheStep,"TCache");

#endif
