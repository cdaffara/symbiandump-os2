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
 @file SampleStep.h
*/
#if (!defined __SAMPLE_STEP_H__)
#define __SAMPLE_STEP_H__
#include <test/testexecutestepbase.h>
#include "sampleserver.h"

// __EDIT_ME__ - Create your own test step definitions
class CSampleStep1 : public CTestStep
	{
public:
	CSampleStep1();
	~CSampleStep1();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	virtual TVerdict doTestStepL();
private:
	};

class CSampleStep2 : public CTestStep
	{
public:
	CSampleStep2(CSampleServer& aParent);
	~CSampleStep2();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	virtual TVerdict doTestStepL();
private:
	CSampleServer& iParent;
	};

class CSampleStep3 : public CTestStep
	{
public:
	CSampleStep3(CSampleServer& aParent);
	~CSampleStep3();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	virtual TVerdict doTestStepL();
private:
	CSampleServer& iParent;
	};

class CSampleStep4 : public CTestStep
	{
public:
	CSampleStep4();
	~CSampleStep4();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	virtual TVerdict doTestStepL();
private:
	};

//mixin to reuse code
class CSampleStepSelective : public CTestStep
	{
public:
	CSampleStepSelective();
	~CSampleStepSelective();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	virtual TVerdict doTestStepL();
private:
	};
	

class CSampleStep5 : public CSampleStepSelective 
	{
public:
	CSampleStep5();
	~CSampleStep5(){};
	/*virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	virtual TVerdict doTestStepL();*/
private:
	};
	
class CSampleStep6 : public CSampleStepSelective 
	{
public:
	CSampleStep6();
	~CSampleStep6(){};
	/*virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	virtual TVerdict doTestStepL();*/
private:
	};
	
class CSampleStep7 : public CSampleStepSelective 
	{
public:
	CSampleStep7();
	~CSampleStep7(){};
	/*virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	virtual TVerdict doTestStepL();*/
private:
	};


class CSampleStep8 : public CSampleStepSelective 
	{
public:
	CSampleStep8();
	~CSampleStep8(){};
	/*virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	virtual TVerdict doTestStepL();*/
private:
	};


class CSampleStep9 : public CSampleStepSelective 
	{
public:
	CSampleStep9();
	~CSampleStep9(){};
	/*virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	virtual TVerdict doTestStepL();*/
private:
	};

class CSampleStep10 : public CSampleStepSelective 
	{
public:
	CSampleStep10();
	~CSampleStep10(){};
	/*virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	virtual TVerdict doTestStepL();*/
private:
	};

class CSampleStep11 : public CSampleStepSelective 
	{
public:
	CSampleStep11();
	~CSampleStep11(){};
	/*virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	virtual TVerdict doTestStepL();*/
private:
	};

class CSampleStep12 : public CSampleStepSelective 
	{
public:
	CSampleStep12();
	~CSampleStep12(){};
	/*virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	virtual TVerdict doTestStepL();*/
private:
	};

class CSampleStep13 : public CSampleStepSelective 
	{
public:
	CSampleStep13();
	~CSampleStep13()
	{};
	/*virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	virtual TVerdict doTestStepL();*/
private:
	};


_LIT(KSampleStep1,"SampleStep1");
_LIT(KSampleStep2,"SampleStep2");
_LIT(KSampleStep3,"SampleStep3");
_LIT(KSampleStep4,"SampleStep4");

//following family of steps will be used 
//for unit testing selective testing functinality


_LIT(KSampleStep5,"SampleStep5");
_LIT(KSampleStep6,"SampleStep6");
_LIT(KSampleStep7,"SampleStep7");
_LIT(KSampleStep8,"SampleStep8");
_LIT(KSampleStep9,"SampleStep9");
_LIT(KSampleStep10,"SampleStep10");
_LIT(KSampleStep11,"SampleStep11");
_LIT(KSampleStep12,"SampleStep12");
_LIT(KSampleStep13,"SampleStep13");



#endif
