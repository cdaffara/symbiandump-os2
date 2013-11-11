/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __T_TTIMES1_H__
#define __T_TTIMES1_H__

#include <test/testexecutestepbase.h>
#include <txtrich.h>

class CT_TTIMES1 : public CTestStep
	{
public:
	CT_TTIMES1();
protected:
	TVerdict doTestStepL();
private:
	void GenerateGlobalLayersL();
	void KillGlobalLayers();
	void KillText();
	void CreationTestsL();
	CRichText* LoadIntoTextL(TFileName& aFileName);
	void GetAppendTestTimeL(const CRichText* aSource,TInt aTestLoopCount);
	void GetBenchmarkAppendTestTimeL(const CRichText* aSource,TInt aTestLoopCount);
	void AppendTest1L();
	void AppendTest2L();
	void AppendTestsL();
	void DoTestsL();
	void setupCleanup();
	};

_LIT(KTestStep_T_TTIMES1, "TTIMES1");

#endif
