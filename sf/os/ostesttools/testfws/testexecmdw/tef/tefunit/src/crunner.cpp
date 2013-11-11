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
 @file CRunner.cpp
*/


#include "cteflogger.h"
#include "crunner.h"

_LIT( KWildCard, "*"); 

CRunner::CRunner( const CTestConfig& aConfig, const CTestExecuteLogger& aLogger)
/**
 * Constructor
 *
 * @param aConfig - Instance of the test config
 * @param aLogger - Instance of the logger
 */
	: iConfig(aConfig), iLogger(aLogger)
	{
	}

CRunner::~CRunner()
/**
 *	Destructor
 */
	{
	}

void CRunner::ConstructL( const TTestPath& aPath )
/**
 * ConstructL
 *
 * @param aPath - The path of the test case to be executed
 */
	{
	iPath.ConstructL( aPath );
	}

/*
 *	NewL
 *    Return an instance of CRunner
 */
CRunner* CRunner::NewL( const TTestPath &aPath, const CTestConfig& aConfig, const CTestExecuteLogger& aLogger )
/**
 * NewL
 *
 * @param aPath - The path of the test case to be executed
 * @param aConfig - Instance of the test config
 * @param aLogger - Instance of the logger
 * @return - Instance of CRunner
 */
	{
	CRunner* lRunner = new (ELeave) CRunner(aConfig, aLogger); 
	CleanupStack::PushL(lRunner); 
	lRunner->ConstructL(aPath);
	CleanupStack::Pop(); 
	return lRunner; 
	}

void CRunner::VisitL(CTestCase* aTestCase)
/**
 * VisitL
 *
 * @param aTestCase - The test case to visit
 */
	{
	aTestCase->RunL(iConfig, iLogger);
	}

void CRunner::VisitL(CTestSuite* aTestSuite)
/**
 * VisitL
 *
 * @param aTestSuite - The test suite to visit
 */
	{
	// Get the next field from the path and search for it
	if( iPath.Remaining() )
		{
		TTestName testSuite = iPath.NextL();
		if( testSuite == KWildCard )
			{
			// Run all the tests in the suite and sub suites
			aTestSuite->RunL( iConfig, iLogger );
			}
		else
			{
			TInt lIndex = 0;
			while( (lIndex < aTestSuite->Count()) &&
					(aTestSuite->NameL(lIndex)!=iPath.Current()) )
				{
				lIndex++;
				}
			if( (lIndex < aTestSuite->Count()) &&
					(aTestSuite->NameL(lIndex)==iPath.Current()) )
				{
				// Log that the runner is currently in this suite
				CTEFLogger::LogTraverse( aTestSuite->Name(), iLogger );
				aTestSuite->TestL( lIndex )->AcceptL( *this );
				}
			else
				{
				User::Leave(KErrTEFUnitTestSuiteError);
				}
			}
		}
	else
		{
		// Run all the tests in the suite and sub suites
		aTestSuite->RunL( iConfig, iLogger );
		}
	}

CPath::CPath()
/**
 * Constructor
 */
	: iCurrent(0)
	{
	}

void CPath::ConstructL( const TDes& aPath )
/**
 * ConstructL
 *
 * @param aPath - The path to be parsed
 */
	{
	// Prepend a NULL field for the top level suite
	User::LeaveIfError(	iFields.Append(_L("NULL")) );
	HBufC* ptrPath = HBufC::NewLC(KMaxPathSize);
	TPtr ptr(ptrPath->Des());
	ptr.Copy(aPath);
	// Populate the path fields array
	TInt offset = ptr.Locate( '.' );
	while( offset != KErrNotFound )
		{
		TTestName field;
		field.Append( ptr.Ptr(), offset );
		User::LeaveIfError(	iFields.Append(field) );
		ptr.Delete( 0, offset+1 );
		offset = ptr.Locate( '.' );
		}
	// There will be one more field
	TTestName field;
	field.Append( ptrPath->Ptr(), ptrPath->Size()/2 );
	CleanupStack::PopAndDestroy(ptrPath);
	User::LeaveIfError(	iFields.Append(field) );
	}

/*
 *	NewL
 *    Return an instance of CPath
 */
CPath* CPath::NewL( TDes& aPath )
/**
 * NewL
 *
 * @param aPath - The path to be parsed
 * @return - Instance of CPath
 */
	{
	CPath* lPath = new (ELeave) CPath();
	CleanupStack::PushL(lPath);
	lPath->ConstructL(aPath);
	CleanupStack::Pop();
	return lPath;
	}

CPath::~CPath()
/**
 * Destructor
 */
	{
	iFields.Close();
	}

TTestName CPath::Current() const
/**
 * Current
 *
 * @return - The current field within the path
 */
	{
	return iFields[iCurrent];
	}

TTestName CPath::NextL()
/**
 * NextL
 *
 * @return - The next field in the path
 */
	{
	if( iCurrent>(iFields.Count()-2) )
		{
		User::Leave(KErrTEFUnitFail);
		}

	return iFields[++iCurrent];
	}

TInt CPath::Count() const
/**
 * Count
 *
 * @return - The number of fields in the path
 */
	{
	return iFields.Count();	
	}

TInt CPath::Remaining() const
/**
 * Remaining
 *
 * @return - The number of remaining fields from the current location in the path
 */
	{
	return (iFields.Count()-1)-iCurrent;	
	}
