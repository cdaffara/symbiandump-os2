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
 @file CTestSuite.cpp
*/

#include "tefunit.h"

CTestSuite::CTestSuite(const TTestName& aName)
/**
 * Constructor
 *
 * @param aName - Test suite name
 */
	: CTestBase(aName)
	{
	}

CTestSuite::CTestSuite(CTestSuite &rhs)
/**
 * Constructor
 *
 * @param rhs - Test suite to copy
 */
	: CTestBase(rhs.iName)
	{
	iTests=rhs.iTests;
	}

void CTestSuite::ConstructL()
/**
 * ConstructL
 */
	{
	}

CTestSuite* CTestSuite::NewL()
/**
 * NewL
 *
 * @return - Instance of CTestSuite
 */
	{
	return NewL(_L("TEFUnit"));
	}

CTestSuite* CTestSuite::NewL(const TTestName& aName)
/**
 * NewL
 *
 * @param aName - Name of the test suite
 * @return - Instance of CTestSuite
 */
	{
	CTestSuite* lTestSuite = new (ELeave) CTestSuite(aName);
	CleanupStack::PushL(lTestSuite);
	lTestSuite->ConstructL();
	CleanupStack::Pop();
	return lTestSuite;
	}

CTestSuite::~CTestSuite()
/**
 * Destructor
 */
	{
	for(TInt lIndex = 0; lIndex < iTests.Count(); lIndex++)
		{
		delete iTests[lIndex];
		iTests[lIndex] = NULL;
		}
	iTests.Close();
	}

void CTestSuite::AddL(CTestBase* aTest)
/**
 * AddL
 *
 * @param aTest - Test to add to the suite
 */
	{
	User::LeaveIfError(	iTests.Append(aTest) );
	}

void CTestSuite::RunL(CTestConfig& aConfig, CTestExecuteLogger& aLogger)
/**
 * RunL
 *
 * @param aConfig - Config to use within the test case
 * @param aLogger - Logger to use within the test case
 */
	{
	// Log that the runner is currently in this suite
	CTEFLogger::LogTraverse( iName, aLogger );
	for(TInt lIndex = 0; lIndex < iTests.Count(); lIndex++)
		{
		TRAPD(err,iTests[lIndex]->RunL(aConfig, aLogger));
		if(err != KErrTEFUnitPass)
			{
			iError=err;
			}
		}
	if(iError != KErrTEFUnitPass)
		{
		User::Leave(iError);
		}
	}

void CTestSuite::AcceptL(MVisitor &aVisitor)
/**
 * AcceptL
 *
 * @param aVisitor - Visitor (runner) to accept
 */
	{
	aVisitor.VisitL(this);
	}

TInt CTestSuite::Count()
/**
 * Count
 *
 * @return - Number of tests cases/sub-suites
 */
	{
	return iTests.Count();
	}

TTestName CTestSuite::NameL( TInt anIndex )
/**
 * NameL
 *
 * @param anIndex - Index of name to retrieve
 * @return - Test name
 */
	{
	return TestL(anIndex)->Name();
	}

CTestBase*	CTestSuite::TestL( TInt anIndex )
/**
 * TestL
 *
 * @param anIndex - Index of test to retrieve
 * @return - Test case/suite
 */
	{
	if( anIndex>=iTests.Count() )
		{
		User::Leave(KErrTEFUnitFail);
		}

	return iTests[anIndex];
	}
