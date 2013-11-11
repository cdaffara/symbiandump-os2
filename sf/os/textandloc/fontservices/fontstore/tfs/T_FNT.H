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


#ifndef __T_FNT_H__
#define __T_FNT_H__

#include <fntstore.h>
#include "TGraphicsHarness.h"

class CTFnt : public CTGraphicsBase
	{
public:
	CTFnt(CTestStep* aStep);
protected:
//from 	CTGraphicsStep
	virtual void RunTestCaseL(TInt aCurTestCase);
private:
	void TestReadFileL(const TDesC& aFontFile);
	void TestEclipseFallbackWithInvalidFontL();
	};


class CTFntStep : public CTGraphicsStep
	{
public:
	CTFntStep();
protected:	
	//from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	};

_LIT(KTFntStep,"TFnt");



#endif
