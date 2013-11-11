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
 @file TEFUnit.h
*/

#ifndef __TEF_UNIT__
#define __TEF_UNIT__

#include <e32std.h>
#include <test/testexecutestepbase.h>
#include <test/cteflogger.h>

/*********************************************************************************
 * Assert Macros
 *********************************************************************************/
#define __ASSERT_SHARED(aFunction, aMessage) \
	if(!aFunction) \
		{ \
		Logger().LogExtra(((TText8*)__FILE__), __LINE__, ESevrAll, aMessage );\
		User::Leave(KErrTEFUnitFail);\
		}

#define ASSERT_EQUALS(aExpected, aActual) \
	__ASSERT_SHARED(AssertEquals(aExpected, aActual) , KAssertFailedEquals);

#define ASSERT_NULL(aPtr) \
	__ASSERT_SHARED(AssertNull(aPtr), KAssertFailedNull);

#define ASSERT_NOT_NULL(aPtr) \
	__ASSERT_SHARED(!AssertNull(aPtr), KAssertFailedNotNull);

#define ASSERT_SAME(aExpectedPtr, aActualPtr) \
	__ASSERT_SHARED(AssertSame(aExpectedPtr, aActualPtr), KAssertFailedSame);

#define ASSERT_NOT_SAME(aExpectedPtr, aActualPtr) \
	__ASSERT_SHARED(!AssertSame(aExpectedPtr, aActualPtr), KAssertFailedNotSame);

#define ASSERT_TRUE(aCondition) \
	__ASSERT_SHARED(AssertTrue(aCondition), KAssertFailedTrue);

#define ASSERT_FALSE(aCondition) \
	__ASSERT_SHARED(!AssertTrue(aCondition), KAssertFailedFalse);

/*********************************************************************************
 * CTestConfig class
 *********************************************************************************/

class CTestConfig
/**
@publishedPartner
@test
*/
	{
public:
	CTestConfig();
	CTestConfig(const CTestConfig& aTestConfig );
	CTestConfig( CTestStep* aTestStep );
	~CTestConfig();

	TBool GetBool(const TDesC& aSectionName, const TDesC& aKey, TBool& aResult) const;
	TBool GetInt(const TDesC& aSectionName, const TDesC& aKey, TInt& aResult) const;
	TBool GetHex(const TDesC& aSectionName, const TDesC& aKey, TInt& aResult) const;
	TBool GetString(const TDesC& aSectionName, const TDesC& aKey, TPtrC& aResult) const;

	// Use the section name passed in via the script
	TBool GetBool(const TDesC& aKey, TBool& aResult) const;
	TBool GetInt(const TDesC& aKey, TInt& aResult) const;
	TBool GetHex(const TDesC& aKey, TInt& aResult) const;
	TBool GetString(const TDesC& aKey, TPtrC& aResult) const;

	TBool WriteBool(const TDesC& aSectionName, const TDesC& aKey, TBool& aValue) const;
	TBool WriteInt(const TDesC& aSectionName, const TDesC& aKey, TInt& aValue) const;
	TBool WriteHex(const TDesC& aSectionName, const TDesC& aKey, TInt& aValue) const;
	TBool WriteString(const TDesC& aSectionName, const TDesC& aKey, TPtrC& aValue) const;

	// Use the section name passed in via the script
	TBool WriteBool(const TDesC& aKey, TBool& aValue) const;
	TBool WriteInt(const TDesC& aKey, TInt& aValue) const;
	TBool WriteHex(const TDesC& aKey, TInt& aValue) const;
	TBool WriteString(const TDesC& aKey, TPtrC& aValue) const;
	
	TBool GetInt(const TDesC& aSectionName, const TDesC& aKey, TInt64& aResult) const;
	TBool GetInt(const TDesC& aKey, TInt64& aResult) const;
	TBool WriteInt(const TDesC& aSectionName, const TDesC& aKey, TInt64& aValue) const;
	TBool WriteInt(const TDesC& aKey, TInt64& aValue) const;
		
private:
	CTestStep*	iTestStep;
	};


/*********************************************************************************
 * CTestFixture class
 *********************************************************************************/
	
class CTestFixture : public CBase
/**
@publishedPartner
@test
*/
	{
public:
	virtual	~CTestFixture();

	virtual void SetupL();
	virtual void TearDownL();
	
	void SetConfigL(CTestConfig& aConfig);
	void SetLoggerL( CTestExecuteLogger& aLogger );
	inline CTestExecuteLogger& Logger() { return iLogger; }

protected:
	CTestConfig			iConfig;
	CTestExecuteLogger	iLogger;
	};

/*********************************************************************************
 * CActiveTestFixture class
 *********************************************************************************/
 
class CActiveTestFixture : public CTestFixture
/**
@publishedPartner
@test
*/
	{
public:
	virtual	~CActiveTestFixture();
	virtual void SetupL();
	virtual void TearDownL();
 	// Construct the active environment
 	virtual void ConstructEnvL();
 	// Destroy the active environment
 	virtual void DestroyEnvL();
 	// Start the active environnment
   	virtual void StartEnvL();
 	// Stop the active environnment
   	virtual void StopEnvL();
	CActiveScheduler* iSched;
	};

class CTestCase;
class CTestSuite;

/*********************************************************************************
 * MVisitor class
 *********************************************************************************/
class MVisitor
	{
public: 
	virtual void VisitL(CTestCase* aTestCase) = 0;
	virtual void VisitL(CTestSuite* aTestSuite) = 0;
	};


const TInt KMaxTestName = 256;
typedef TBuf<KMaxTestName> TTestName;

const TInt KMaxPathSize = KMaxTestName*10;
typedef TBuf<KMaxPathSize> TTestPath;

/*********************************************************************************
 * CTestBase class
 *********************************************************************************/
class CTestBase : public CBase
	{
public:
	CTestBase(const TTestName &aName); 
	virtual ~CTestBase();
		
	const TTestName& Name() const; 
	virtual void RunL(CTestConfig& aConfig, CTestExecuteLogger& aLogger) = 0;
	virtual void AcceptL(MVisitor &aVisitor) = 0;
	
protected: 
	TTestName iName; 
	};


/*********************************************************************************
 * CTestCase class
 *********************************************************************************/
class CTestCase : public CTestBase
	{
public:
	CTestCase(const TTestName& aName);
	virtual ~CTestCase(); 

	virtual void RunL(CTestConfig& aConfig, CTestExecuteLogger& aLogger) = 0;
	virtual void AcceptL(MVisitor &aVisitor);
	};

/*********************************************************************************
 * Test Suite Macros
 *********************************************************************************/
#define START_SUITE \
	CTestSuite* lTestSuite = CTestSuite::NewL();

#define SUB_SUITE \
	CTestSuite* lTestSuite = CTestSuite::NewL(aName);

#define END_SUITE \
	return lTestSuite;
//PDEF115446 start
// By adding a condition marco to reconginize different compiler
#ifndef __GCCXML__
	// this means using gcc or armcc or mwccsym2 
	#define ADD_TEST_STEP(name) \
		_LIT( KTest ## name, #name); \
		AddTestCase(lTestSuite, KTest ## name, &name);
	
	#define ADD_ASYNC_TEST_STEP(name) \
		_LIT( KTest ## name, #name); \
		AddAsyncTestCase(lTestSuite, KTest ## name, &name);
#endif
#ifdef __GCCXML__
	// this means using gccxml
	// GCCXML compiler NOT use ADD_TEST_STEP 
	// Please use 2 parameter Macro ADD_(ASYNC)_TEST_STEP_CLASS	 
#endif
	
//PDEF115446
//new version marco
//For both GCCXML and Non-GCCXML compiler
#define ADD_TEST_STEP_CLASS(classname, callback) \
    _LIT( KTest ## callback, #callback);\
    AddTestCase(lTestSuite, KTest ## callback, & classname::callback);

#define ADD_ASYNC_TEST_STEP_CLASS(classname, callback) \
    _LIT( KTest ## callback, #callback);\
    AddAsyncTestCase (lTestSuite, KTest ## callback, & classname::callback);
//PDEF115446 end
	
#define ADD_TEST_SUITE(name) \
	_LIT( KTest ## name, #name); \
	lTestSuite->AddL( name::CreateSuiteL( KTest##name));


/*********************************************************************************
 * CTestSuite class
 *********************************************************************************/

class CTestSuite : public CTestBase
/**
@publishedPartner
@test
*/
	{
public: 
	CTestSuite(const TTestName& aName);
	CTestSuite(CTestSuite &rhs);
	void ConstructL();
	static CTestSuite* NewL();
	static CTestSuite* NewL(const TTestName& aName);
	virtual ~CTestSuite();
		
	// Add a test to the suite
	virtual void AddL(CTestBase* aTest);

	// Run all the tests in the suite
	virtual void RunL(CTestConfig& aConfig, CTestExecuteLogger& aLogger);

	virtual void AcceptL(MVisitor &aVisitor);

	TInt Count();
	TTestName NameL( TInt anIndex );
	CTestBase*	TestL( TInt anIndex );

private:
	RPointerArray<CTestBase>	iTests;
	TInt iError;
	};


/*********************************************************************************
 * CTestCaseTemplate class
 *********************************************************************************/
template <class Fixture>
class CTestCaseTemplate : public CTestCase
	{
private:
	typedef void (Fixture::*TestMethod)();
	
public:
	CTestCaseTemplate(const TDesC &aName, TestMethod aTestMethod );
	virtual ~CTestCaseTemplate();

	virtual void RunL(CTestConfig& aConfig, CTestExecuteLogger& aLogger);

protected:
	TestMethod	iTest;
	};

/*********************************************************************************
 * CActiveTester class
 *********************************************************************************/
const TInt KNumberOfDelays = 3;

template <class Fixture>
class CActiveTester : public CTimer
	{
private:
	typedef void (Fixture::*TestMethod)();

public:
	CActiveTester(Fixture* aFixture, TestMethod aTestMethod, TTimeIntervalMicroSeconds32 /*aDelays[KNumberOfDelays]*/ );
	virtual ~CActiveTester();

	virtual void RunL();
	virtual TInt RunError(TInt /*aError*/);
	TInt Error();
	virtual void ConstructEnvL();
	void DestroyEnvL();
	void StartEnvL();
	void SetLoggerL(CTestExecuteLogger& aLogger);
	void SetConfigL(CTestConfig& aConfig);

protected:
	enum TTestState
		{
		EConstruct = 0,
		ERunTest,
		EDestroy,
		EExit
		};

	TTestState					iState;
	Fixture*					iFixture;
	TestMethod					iTest;
	TTimeIntervalMicroSeconds32	iDelays[KNumberOfDelays];
	TInt						iError;
	};


/*********************************************************************************
 * CAsyncTestCase class
 *********************************************************************************/
template <class Fixture>
class CAsyncTestCase : public CTestCase
	{
private:
	typedef void (Fixture::*TestMethod)();

public:
	CAsyncTestCase(const TDesC &aName, TestMethod aTestMethod );
	virtual ~CAsyncTestCase();

	virtual void RunL(CTestConfig& aConfig, CTestExecuteLogger& aLogger);

protected:
	TestMethod iTest;
	};

#include <test/tefunit.inl>

#endif // __TEF_UNIT__
