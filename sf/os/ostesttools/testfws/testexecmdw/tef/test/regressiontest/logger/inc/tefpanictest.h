/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalComponent - Internal Symbian test code 
*/

#include <test/testexecuteserverbase.h>
#include <test/testexecutestepbase.h>
#ifndef TEFPANICTEST_H_
#define TEFPANICTEST_H_

_LIT(KServerName,"TEF_LoggerTestStep");
class CTEFPanicBugServer : public CTestServer
	{
public:
	static CTEFPanicBugServer* NewL();
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);
private:
	};

_LIT(KTEFPanicBugStep, "TEFPanicBugStep");
class CTEFPanicBugStep : public CTestStep
	{
public:
	CTEFPanicBugStep();
	virtual TVerdict doTestStepL(void);
	};

_LIT(KTEFPanicSomeThingStep, "TEFPanicSomeThingStep");
class CTEFPanicSomeThingStep : public CTestStep
	{
public:
	CTEFPanicSomeThingStep();

	virtual TVerdict doTestStepL(void);
	};

_LIT(KTEFPanicEmptyStep, "TEFPanicEmptyStep");
class CTEFPanicEmptyStep : public CTestStep
	{
public:
	CTEFPanicEmptyStep();
	virtual TVerdict doTestStepL(void);
	};

_LIT(KTEFPanicDblSpaceStep, "TEFPanicDblSpaceStep");
class CTEFPanicDblSpaceStep : public CTestStep
	{
public:
	CTEFPanicDblSpaceStep();
	virtual TVerdict doTestStepL(void);
	};
_LIT(KTEFIgnoreStep, "TEFIgnoreStep");
class CTEFIgnoreStep : public CTestStep
	{
public:
	CTEFIgnoreStep();
	virtual TVerdict doTestStepL(void);
	};

_LIT(KTEFPassStep, "TEFPassStep");
class CTEFPassStep : public CTestStep
	{
public:
	CTEFPassStep();
	virtual TVerdict doTestStepL(void);
	};

#endif /*TEFPANICTEST_H_*/
