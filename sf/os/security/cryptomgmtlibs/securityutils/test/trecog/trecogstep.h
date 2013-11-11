/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
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
 @internalComponent - Internal Symbian test code  
*/

#ifndef __TRECOGSTEP_H__
#define __TRECOGSTEP_H__

#include "trecogserver.h"
#include <apgcli.h>
#include <charconv.h>

_LIT(KRecogStep, "RecogStep");


class CRecogStep : public CTestStep
	{
public:
	CRecogStep();
	~CRecogStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	virtual TVerdict doTestStepL();
	void RunTestCases();

private:
	void ConstructL();
	void RunTestCasesL();
	HBufC8* ConvertDes16toHBufC8LC(TDesC& source);
	void TestRecognizeDataL();
	
	void ConvertFileToPrivateL();
	
private:
	RFs iTheFs;
	RApaLsSession iLs;
	TFileName iFileName;
	};

#endif




